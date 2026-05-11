#pragma once
#include "data/MechanicDb.gen.h"
#include <cstdint>

// Thin runtime query API on top of the generated `MechanicDb.gen.{h,cpp}`
// table. Phase 2 ships only the avoidable-spell lookup; interrupt and dispel
// lookups arrive in Phase 3.
//
// Lookup is a binary search over a sorted-by-spell_id constexpr array.
// O(log N) with N ≈ 100 entries → ~7 comparisons. Cheap to call from the
// per-tick `AltbotPositionManager::FastTick` hazard scan.
namespace EncounterMechanics
{
    using MechanicEntry     = AltbotMechanicDb::MechanicEntry;
    using MechanicClass     = AltbotMechanicDb::MechanicClass;
    using DispelType        = AltbotMechanicDb::DispelType;
    using InterruptPriority = AltbotMechanicDb::InterruptPriority;

    // Returns nullptr when the spell isn't in the table.
    MechanicEntry const* Lookup(uint32 spellId);

    // Convenience wrapper. Returns false for unknown IDs and for IDs whose
    // mechanic_class isn't `Avoidable` (e.g. a future Interrupt-only entry).
    bool IsAvoidable(uint32 spellId);

    // Same shape; matches Phase 3 hooks. Will return non-default when the
    // dispel/interrupt rows land — for now both surface "Default" / "None".
    DispelType        GetDispelType(uint32 spellId);
    InterruptPriority GetInterruptPriority(uint32 spellId);

    // Avoid-radius accessor. Returns 0 if the spell isn't in the table —
    // callers should branch on Lookup() first or treat 0 as "no opinion".
    float AvoidRadius(uint32 spellId);
}
