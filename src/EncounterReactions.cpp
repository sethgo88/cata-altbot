#include "EncounterReactions.h"
#include "AltbotCombatLog.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Map.h"
#include "Player.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Unit.h"
#include "strategies/StrategyUtil.h"
#include <vector>

namespace
{
    // Helper: bot has the given spell off cooldown and at all-castable state.
    bool CanCast(Player* bot, uint32 spellId)
    {
        if (!bot || !spellId) return false;
        SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
        if (!info) return false;
        if (bot->GetSpellHistory()->HasCooldown(info)) return false;
        if (bot->GetSpellHistory()->HasGlobalCooldown(info)) return false;
        return true;
    }

    // Visitor: collect hostile units within `radius` whose current cast or
    // channel hits the mechanic DB at >= the requested priority.
    struct InterruptCandidate
    {
        Unit*  caster   = nullptr;
        uint32 spellId  = 0;
        EncounterMechanics::InterruptPriority priority =
            EncounterMechanics::InterruptPriority::Default;
    };

    struct InterruptScan
    {
        Player const* bot;
        EncounterMechanics::InterruptPriority minPriority;
        std::vector<InterruptCandidate> hits;

        void Visit(CreatureMapType& m)
        {
            for (auto iter = m.begin(); iter != m.end(); ++iter)
            {
                Creature* c = iter->GetSource();
                if (!c || !c->IsAlive()) continue;
                if (c->IsFriendlyTo(bot)) continue;

                Spell* s = c->GetCurrentSpell(CURRENT_GENERIC_SPELL);
                if (!s) s = c->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
                if (!s) continue;

                SpellInfo const* info = s->GetSpellInfo();
                if (!info) continue;
                uint32 sid = info->Id;

                auto pri = EncounterMechanics::GetInterruptPriority(sid);
                // Phase 3 dispatcher only acts on entries that explicitly
                // request `ShouldInterrupt` or higher. Default priority is
                // skipped so we don't kick random caster-mob filler.
                if (uint8(pri) < uint8(minPriority)) continue;

                hits.push_back({ c, sid, pri });
            }
        }
        template<class T> void Visit(GridRefManager<T>&) {}
    };

    // Visitor: collect hostile units within `radius` carrying an aura whose
    // SpellInfo::Dispel matches `removable`. Used by TryPurgeHostile.
    struct PurgeScan
    {
        Player const* bot;
        EncounterMechanics::DispelType removable;
        Unit*  bestTarget = nullptr;

        bool DispelMatches(uint32 dispel)
        {
            switch (removable)
            {
                case EncounterMechanics::DispelType::Magic:           return dispel == DISPEL_MAGIC;
                case EncounterMechanics::DispelType::Curse:           return dispel == DISPEL_CURSE;
                case EncounterMechanics::DispelType::Poison:          return dispel == DISPEL_POISON;
                case EncounterMechanics::DispelType::Disease:         return dispel == DISPEL_DISEASE;
                case EncounterMechanics::DispelType::EnragePurge:     return false;   // dispel != magic; uses Tranq/Soothe
                case EncounterMechanics::DispelType::MagicOffensive:  return dispel == DISPEL_MAGIC;
                default: return false;
            }
        }

        void Visit(CreatureMapType& m)
        {
            if (bestTarget) return;
            for (auto iter = m.begin(); iter != m.end() && !bestTarget; ++iter)
            {
                Creature* c = iter->GetSource();
                if (!c || !c->IsAlive()) continue;
                if (c->IsFriendlyTo(bot)) continue;
                for (auto const& [spellId, app] : c->GetAppliedAuras())
                {
                    Aura const* a = app->GetBase();
                    if (!a) continue;
                    SpellInfo const* info = a->GetSpellInfo();
                    if (!info) continue;
                    if (DispelMatches(info->Dispel))
                    {
                        bestTarget = c;
                        break;
                    }
                }
            }
        }
        template<class T> void Visit(GridRefManager<T>&) {}
    };
}

namespace EncounterReactions
{

bool TryInterruptNearbyCast(Player* bot,
                            uint32 interruptSpellId,
                            char const* specLabel,
                            float radius,
                            EncounterMechanics::InterruptPriority minPriority)
{
    if (!CanCast(bot, interruptSpellId))
        return false;

    InterruptScan scan;
    scan.bot         = bot;
    scan.minPriority = minPriority;
    Cell::VisitGridObjects(bot, scan, radius);

    if (scan.hits.empty())
        return false;

    // Pick highest-priority candidate; ties go to the closest.
    InterruptCandidate const* best = nullptr;
    for (auto& c : scan.hits)
    {
        if (!best
            || uint8(c.priority) > uint8(best->priority)
            || (c.priority == best->priority
                && bot->GetDistance(c.caster) < bot->GetDistance(best->caster)))
        {
            best = &c;
        }
    }
    if (!best || !best->caster) return false;

    return StrategyUtil::CastWithLog(bot, best->caster, interruptSpellId,
                                     specLabel, /*tierIdx*/ -2, "interrupt")
           == SPELL_CAST_OK;
}

bool TryDispelAlly(Player* bot,
                   Player* master,
                   EncounterMechanics::DispelType removable,
                   uint32 dispelSpellId,
                   char const* specLabel)
{
    if (!CanCast(bot, dispelSpellId))
        return false;

    auto matches = [removable](uint32 dispel)
    {
        switch (removable)
        {
            case EncounterMechanics::DispelType::Magic:    return dispel == DISPEL_MAGIC;
            case EncounterMechanics::DispelType::Curse:    return dispel == DISPEL_CURSE;
            case EncounterMechanics::DispelType::Poison:   return dispel == DISPEL_POISON;
            case EncounterMechanics::DispelType::Disease:  return dispel == DISPEL_DISEASE;
            default:                                       return false;
        }
    };

    auto scan = [&](Player* ally) -> Player*
    {
        if (!ally || !ally->IsAlive() || ally->GetMap() != bot->GetMap())
            return nullptr;
        for (auto const& [spellId, app] : ally->GetAppliedAuras())
        {
            Aura const* a = app->GetBase();
            if (!a) continue;
            SpellInfo const* info = a->GetSpellInfo();
            if (!info) continue;
            if (matches(info->Dispel))
                return ally;
        }
        return nullptr;
    };

    Player* target = nullptr;
    Group* group = bot->GetGroup();
    if (group)
    {
        for (GroupReference* itr = group->GetFirstMember(); itr && !target; itr = itr->next())
            target = scan(itr->GetSource());
    }
    else
    {
        target = scan(bot);
        if (!target && master) target = scan(master);
    }
    if (!target) return false;

    return StrategyUtil::CastWithLog(bot, target, dispelSpellId,
                                     specLabel, /*tierIdx*/ -2, "dispel")
           == SPELL_CAST_OK;
}

bool TryPurgeHostile(Player* bot,
                     EncounterMechanics::DispelType removable,
                     uint32 purgeSpellId,
                     char const* specLabel,
                     float radius)
{
    if (!CanCast(bot, purgeSpellId))
        return false;

    PurgeScan scan;
    scan.bot       = bot;
    scan.removable = removable;
    Cell::VisitGridObjects(bot, scan, radius);
    if (!scan.bestTarget) return false;

    return StrategyUtil::CastWithLog(bot, scan.bestTarget, purgeSpellId,
                                     specLabel, /*tierIdx*/ -2, "purge")
           == SPELL_CAST_OK;
}

} // namespace EncounterReactions
