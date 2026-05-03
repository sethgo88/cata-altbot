#pragma once
#include "AltbotStrategy.h"
#include <array>
#include <cstdint>

class Player;
class Unit;

class MmHunterStrategy : public AltbotStrategy
{
public:
    MmHunterStrategy() = default;

    void         Update(Player* bot, Player* master, AltbotTickContext const& ctx) override;
    char const*  GetName() const override { return "marksmanship-hunter"; }

    void InvalidateCache() { _cacheResolved = false; _cache.fill(0); }

private:
    enum class Spell : uint8_t
    {
        AspectOfTheHawk = 0,
        HuntersMark,
        SerpentSting,
        ChimeraShot,
        AimedShot,
        KillShot,
        ArcaneShot,
        SteadyShot,
        MultiShot,
        Misdirection,
        MendPet,
        CallPet1,
        Disengage,
        FeignDeath,
        Deterrence,
        Count
    };

    std::array<uint32, size_t(Spell::Count)> _cache{};
    bool _cacheResolved = false;

    uint32 GetSpell(Spell s) const { return _cache[size_t(s)]; }
    void   ResolveSpellCache(Player* bot);

    bool TryCast(Player* bot, Unit* target, Spell s) const;
    bool IsOnCooldown(Player* bot, uint32 spellId) const;

    void DoMaintenance(Player* bot, Player* master, Unit* target);
    void PetMaintenance(Player* bot);
    void DoMisdirection(Player* bot, Player* master);
    bool DoDefensives(Player* bot);

    bool Tier_AimedShotProc(Player* bot, Unit* target) const;
    bool Tier_SerpentSting(Player* bot, Unit* target) const;
    bool Tier_ChimeraShot(Player* bot, Unit* target) const;
    bool Tier_KillShot(Player* bot, Unit* target) const;
    bool Tier_AoE(Player* bot, Unit* target) const;
    bool Tier_AimedShotHard(Player* bot, Unit* target) const;
    bool Tier_ArcaneShot(Player* bot, Unit* target) const;
    bool Tier_SteadyShot(Player* bot, Unit* target) const;
};
