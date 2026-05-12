#pragma once
#include "AltbotStrategy.h"
#include <array>
#include <cstdint>

class Player;
class Unit;

class FrostMageStrategy : public AltbotStrategy
{
public:
    FrostMageStrategy() = default;

    void         Update(Player* bot, Player* master, AltbotTickContext const& ctx) override;
    char const*  GetName() const override { return "frost-mage"; }

    void InvalidateCache() { _cacheResolved = false; _cache.fill(0); }

private:
    enum class Spell : uint8_t
    {
        MoltenArmor = 0,
        MageArmor,
        FrostArmor,
        Frostbolt,
        FrostfireBolt,
        Fireball,
        IceLance,
        DeepFreeze,
        FrostNova,
        ConeOfCold,
        Blizzard,
        SummonWaterElemental,
        IceBlock,
        MageWard,
        Blink,
        Counterspell,        // 24y interrupt; 24s CD; locks the school for 7s
        Count
    };

    std::array<uint32, size_t(Spell::Count)> _cache{};
    bool _cacheResolved = false;

    uint32 GetSpell(Spell s) const { return _cache[size_t(s)]; }
    void   ResolveSpellCache(Player* bot);

    bool TryCast(Player* bot, Unit* target, Spell s) const;
    bool IsOnCooldown(Player* bot, uint32 spellId) const;

    void PetMaintenance(Player* bot);
    void DoMaintenance(Player* bot);
    bool DoDefensives(Player* bot);

    bool Tier_DeepFreeze(Player* bot, Unit* target) const;
    bool Tier_FFB_BothProcs(Player* bot, Unit* target) const;
    bool Tier_FFB_BrainFreeze(Player* bot, Unit* target) const;
    bool Tier_IceLance(Player* bot, Unit* target) const;
    bool Tier_AoE(Player* bot, Unit* target) const;
    bool Tier_Frostbolt(Player* bot, Unit* target) const;
};
