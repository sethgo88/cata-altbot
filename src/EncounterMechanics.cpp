#include "EncounterMechanics.h"
#include <algorithm>

namespace
{
    // Sorted-by-spell_id; codegen guarantees the order. We use std::lower_bound
    // directly against the array, comparing on spellId only.
    AltbotMechanicDb::MechanicEntry const* FindEntry(uint32 spellId)
    {
        auto* begin = AltbotMechanicDb::kMechanicDb;
        auto* end   = AltbotMechanicDb::kMechanicDb + AltbotMechanicDb::kMechanicDbSize;
        auto* it = std::lower_bound(begin, end, spellId,
            [](AltbotMechanicDb::MechanicEntry const& e, uint32 v)
            { return e.spellId < v; });
        if (it == end || it->spellId != spellId)
            return nullptr;
        return it;
    }
}

namespace EncounterMechanics
{

MechanicEntry const* Lookup(uint32 spellId)
{
    return FindEntry(spellId);
}

bool IsAvoidable(uint32 spellId)
{
    auto* e = FindEntry(spellId);
    return e && e->mclass == MechanicClass::Avoidable;
}

DispelType GetDispelType(uint32 spellId)
{
    auto* e = FindEntry(spellId);
    return e ? e->dispel : DispelType::None;
}

InterruptPriority GetInterruptPriority(uint32 spellId)
{
    auto* e = FindEntry(spellId);
    return e ? e->priority : InterruptPriority::Default;
}

float AvoidRadius(uint32 spellId)
{
    auto* e = FindEntry(spellId);
    return e ? e->avoidRadiusY : 0.0f;
}

} // namespace EncounterMechanics
