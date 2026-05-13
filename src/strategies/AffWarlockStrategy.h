#pragma once
#include "AltbotStrategy.h"
#include <array>
#include <cstdint>

class Player;
class Unit;

class AffWarlockStrategy : public AltbotStrategy
{
public:
    AffWarlockStrategy() = default;

    void         Update(Player* bot, Player* master, AltbotTickContext const& ctx) override;
    char const*  GetName() const override { return "affliction-warlock"; }

    void InvalidateCache() { _cacheResolved = false; _cache.fill(0); }

private:
    enum class Spell : uint8_t
    {
        FelArmor = 0,
        DemonArmor,
        CurseOfTheElements,
        Haunt,
        BaneOfDoom,
        BaneOfAgony,
        Corruption,
        UnstableAffliction,
        ShadowBolt,
        DrainSoul,
        DrainLife,
        LifeTap,
        SeedOfCorruption,
        DemonicCircleSummon,
        DemonicCircleTeleport,
        HowlOfTerror,
        DeathCoil,
        SummonFelhunter,
        HealthFunnel,
        // 142-pt build (Aff+Destro) additions. Spec doc:
        // `docs/research/142pt-aff-warlock-build.md` §G.1.
        // ImprovedSoulFire's proc-aura ID is intentionally NOT cached — the
        // talent passive shares its SpellName, so the maintenance helper
        // walks bot->GetAppliedAuras() and filters IsPassive() instead
        // (CLAUDE.md cast-pipeline gotcha #1).
        ChaosBolt,
        Shadowburn,
        SoulFire,
        BaneOfHavoc,
        Count
    };

    std::array<uint32, size_t(Spell::Count)> _cache{};
    bool _cacheResolved = false;

    // Tracks last Seed of Corruption cast (getMSTime() result, monotonic per-tick).
    // SoC explodes after ~1518 absorbed damage — with a multi-DPS group hitting
    // a 3-pack the seed pops in 2-4 seconds, which would otherwise cause us to
    // recast SoC the very next tick and starve the single-target DoT rotation.
    uint32 _lastSoCMs = 0;

    uint32 GetSpell(Spell s) const { return _cache[size_t(s)]; }
    void   ResolveSpellCache(Player* bot);

    enum class ManaMode { Normal, Conserve, Crisis };
    ManaMode GetManaMode(Player* bot) const;

    bool TryCast(Player* bot, Unit* target, Spell s) const;
    bool IsOnCooldown(Player* bot, uint32 spellId) const;

    void DoMaintenance(Player* bot, Unit* target);
    void PetMaintenance(Player* bot);
    bool DoDefensives(Player* bot);
    void MaintainImprovedSoulFire(Player* bot, Unit* target);

    bool Tier_Haunt(Player* bot, Unit* target) const;
    bool Tier_BaneOfDoom(Player* bot, Unit* target) const;
    bool Tier_Corruption(Player* bot, Unit* target) const;
    bool Tier_UnstableAffliction(Player* bot, Unit* target) const;
    bool Tier_AoE(Player* bot, Unit* target);
    bool Tier_BaneOfHavocCleave(Player* bot, Unit* target) const;
    bool Tier_ChaosBolt(Player* bot, Unit* target) const;
    bool Tier_Shadowburn(Player* bot, Unit* target) const;
    bool Tier_DrainSoul(Player* bot, Unit* target) const;
    bool Tier_ShadowBolt(Player* bot, Unit* target, ManaMode mode) const;
};
