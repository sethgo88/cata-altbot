#pragma once
#include "AltbotStrategy.h"
#include <array>
#include <cstdint>
#include <functional>

class Player;
class Unit;

class RestoShamanStrategy : public AltbotStrategy
{
public:
    RestoShamanStrategy();

    void         Update(Player* bot, Player* master, AltbotTickContext const& ctx) override;
    char const*  GetName() const override { return "resto-shaman"; }

    void InvalidateCache() { _cacheResolved = false; _cache.fill(0); }

private:
    enum class Spell : uint8_t
    {
        HealingSurge = 0,
        HealingWave,
        GreaterHealingWave,
        Riptide,
        ChainHeal,
        EarthShield,
        WaterShield,
        SpiritLinkTotem,
        ManaTideTotem,
        LightningBolt,
        CleanseSpirit,         // dispel: curse (and magic via Improved Cleanse Spirit talent)
        WindShear,             // interrupt (instant, 6s CD, off-GCD)
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

    void DoMaintenance(Player* bot, Player* master);
    void CheckCooldowns(Player* bot, ManaMode mode);

    bool Tier1_SelfEmergency(Player* bot) const;
    bool Tier2_TankEmergency(Player* bot, Player* master, ManaMode mode) const;
    bool Tier3_Riptide(Player* bot, Player* master) const;
    bool Tier4_SpiritLink(Player* bot, Player* master) const;
    bool Tier5_ChainHeal(Player* bot, Player* master, ManaMode mode) const;
    bool Tier6_GHW(Player* bot, Player* master, ManaMode mode) const;
    bool Tier7_HealingSurgeNonTank(Player* bot, Player* master, ManaMode mode) const;
    bool Tier8_HealingWaveFiller(Player* bot, Player* master) const;
    bool Tier9_SupportDPS(Player* bot, Player* master, ManaMode mode, AltbotTickContext const& ctx) const;
};
