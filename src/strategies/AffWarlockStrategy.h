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
        Count
    };

    std::array<uint32, size_t(Spell::Count)> _cache{};
    bool _cacheResolved = false;

    uint32 GetSpell(Spell s) const { return _cache[size_t(s)]; }
    void   ResolveSpellCache(Player* bot);

    enum class ManaMode { Normal, Conserve, Crisis };
    ManaMode GetManaMode(Player* bot) const;

    bool TryCast(Player* bot, Unit* target, Spell s) const;
    bool IsOnCooldown(Player* bot, uint32 spellId) const;

    void DoMaintenance(Player* bot, Unit* target);
    void PetMaintenance(Player* bot);
    bool DoDefensives(Player* bot);

    bool Tier_Haunt(Player* bot, Unit* target) const;
    bool Tier_BaneOfDoom(Player* bot, Unit* target) const;
    bool Tier_Corruption(Player* bot, Unit* target) const;
    bool Tier_UnstableAffliction(Player* bot, Unit* target) const;
    bool Tier_AoE(Player* bot, Unit* target) const;
    bool Tier_DrainSoul(Player* bot, Unit* target) const;
    bool Tier_ShadowBolt(Player* bot, Unit* target, ManaMode mode) const;
};
