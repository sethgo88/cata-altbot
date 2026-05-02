# Throne of the Tides — Healer

**Healer-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the healer reacts. Class-agnostic where possible — Resto Shaman spell mappings are noted where they make a class-specific cooldown decision (e.g., Mana Tide vs Innervate vs Hymn of Hope).

**Layering**:
- Cross-content healer patterns (triage hierarchy, dispel taxonomy, mana pacing, anti-patterns) live in `docs/roles/healer.md`
- Per-spec rotation / CD / mana model lives in `docs/specs/resto-shaman.md` (or whichever healer spec is active)
- This doc layers ToTT-specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_healer_overview:
  total_bosses: 4
  estimated_full_clear_minutes: 20-30
  mana_profile:
    lady_nazjar: low_to_moderate     # short fight; mostly tank healing + 1-2 dispels
    commander_ulthok: moderate        # constant low-grade damage + dispels
    mindbender_ghursha: moderate_to_high   # P2 Mind Fog + raid heal through Unrelenting Agony
    ozumat: high                      # P3 stacking Blight DoT — pop everything
  drink_windows:
    - after Naz'jar (before naga gauntlet end if you survived)
    - after Ulthok (before faceless wing)
    - after Ghur'sha (before Ozumat)         # MOST IMPORTANT — go in full mana
  key_responsibilities:
    - dispel_priority_per_fight   # see per-boss blocks
    - cooldown_pacing             # save Mana Tide / Spirit Link for the right moments
    - anti_pattern_awareness      # NEVER heal enslaved player on Ghur'sha P2
```

---

## Trash — healer notes

```yaml
trash_healer:
  naga_gauntlet:
    dispels:
      - Hex (curse) — from Naz'jar Spiritmender on random ally
      - Lightning Surge (magic, timed detonation) — dispel before pop
    cooldown_use: none required; reactive only
    notes: priority is for melee/ranged to interrupt Spiritmender heals; healer just keeps tank topped through Crushing Depths stacks if pulls go long

  faceless_wing:
    dispels:
      - haste_debuff from Gilgoblin Aquamage (magic)
    cooldown_use: |
      Use Healing Stream Totem aggressively — Faceless Watcher Crush is constant on tank.
      Save Mana Tide for Ghur'sha P2.
    notes: |
      Watch for Clenching Tentacles lift on party members — they get Ground Pound
      shortly after. Burst-heal lifted players if no trinket / escape available.

  corruption_hallway:
    dispels:
      - Wall of Corruption (disease) — triggers when Unstable Corruption dies; dispel between pulls
    cooldown_use: none required
    notes: Wall stacks if not dispelled — clear before next pull
```

---

## Boss 1 — Lady Naz'jar

```yaml
boss_healer:
  boss: lady_nazjar
  difficulty_grade: moderate
  mana_intensity: low

  threshold_overrides:
    # Layered on top of docs/specs/resto-shaman.md tier predicates
    tier_2_tank_emergency: tank.hp_pct < 50    # raised from 40 — Shock Blast spike windows
    notes: |
      Shock Blast (76008) every 10-15s is the dominant tank-damage event. Tank HP can
      drop fast if a Shock Blast lands without interrupt. Pre-empt with HoT/Earth Shield
      maintenance and keep Riptide rolling on tank.

  pre_pull:
    - Earthliving_Weapon imbue if missing
    - Water_Shield up if missing
    - Earth_Shield on tank
    - Healing_Stream_Totem placed before pull (totem doesn't break stealth/pull)

  per_phase:
    phase_1_2_3:
      cooldown_plan: hold all major CDs; reactive heals only
      dispels:
        - Fungal Spores (76001 — DISEASE) — dispel within 2s of application; allow time for splash to be moot
        - Lightning Surge (75992 — MAGIC) on adds — dispel before 5s detonation
      positioning: stay 8-10y from cluster center to avoid Geyser knockback radius

    waterspout_phases:
      cooldown_plan: |
        - Spirit_Link_Totem ONLY if 3+ allies are <40% HP within 10y of each other
          (rare on this fight; usually skippable here, save for Ghur'sha or Ozumat).
        - Healing_Stream_Totem maintained — provides passive raid mitigation during add chaos.
      dispels:
        - Lightning Surge (magic) — high priority; missed dispel = AoE on cluster
        - Fungal Spores (disease) — continues to apply during add phase
        - Honor Guard Enrage (purgeable) — if class can purge enrages, use it
      raid_heal_emphasis: |
        Tempest Witch Chain Lightning hits 3 — expect ~3-target damage clusters.
        Chain Heal is high-value here.

  anti_patterns:
    - do not chase the boss during Waterspout — boss is immune; focus add survival

  cooldown_summary:
    mana_tide: skip (mana not stressed enough yet)
    spirit_link: conditional only
    spiritwalker_grace: reactive (movement events from Geyser knockback)
    natures_swiftness: reactive on tank emergency (paired with Greater Healing Wave)
    earth_elemental: skip (5-man, not raid)

  reference_mechanics:
    - encounter.md: Shock Blast (76008), Fungal Spores (76001), Geyser (75722), Lightning Surge (75992)
```

---

## Boss 2 — Commander Ulthok

```yaml
boss_healer:
  boss: commander_ulthok
  difficulty_grade: moderate
  mana_intensity: moderate

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 45    # slightly raised — no major spike, but constant pressure
    chain_heal_trigger: party.injured_count(radius=12, hp_pct_lt=80) >= 3   # tighter than baseline 85% — fight is mobile

  pre_pull:
    - Earthliving_Weapon imbue if missing
    - Water_Shield up if missing
    - Earth_Shield on tank
    - Healing_Stream_Totem placed before pull

  per_phase:
    single_phase:
      cooldown_plan: |
        - Mana_Tide_Totem: when self mana <55% (slightly tighter than baseline 60% — fight is short).
        - Spirit_Link_Totem: skip on this fight; party rarely clusters tightly enough due to fissure-kiting.
        - Healing_Stream_Totem: must be placed near tank's CURRENT path, not start position — re-drop if tank kites away from it.
      dispels:
        - Curse of Fatigue (76094 — CURSE) — TOP PRIORITY DISPEL. Slowed players cannot escape Dark Fissure (76047).
          Dispel within 1 GCD; this is the fight-defining healer task.
      raid_heal_emphasis: |
        Squeeze (76026) targets a random non-tank for 6s stun + heavy DoT.
        Pre-cast Healing Wave on the squeezed target as soon as Squeeze lands.
      positioning: |
        Stay mobile with the tank's kite path. Do NOT stand still — Dark Fissure
        spawns can land under you. Use Spiritwalker's Grace if a long cast (GHW)
        starts during a fissure spawn telegraph.

  anti_patterns:
    - do NOT delay Curse of Fatigue dispel — slowed = dead in fissure
    - do NOT stand still in melee range during fissure spawns (heroic)

  cooldown_summary:
    mana_tide: self.mana_pct < 55
    spirit_link: skip
    spiritwalker_grace: reactive — if mid-cast on GHW and a fissure telegraph lands underfoot
    natures_swiftness: reactive on tank emergency
    earth_elemental: skip

  reference_mechanics:
    - encounter.md: Squeeze (76026), Dark Fissure (76047), Curse of Fatigue (76094)
```

---

## Boss 3 — Mindbender Ghur'sha

```yaml
boss_healer:
  boss: mindbender_ghursha
  difficulty_grade: high
  mana_intensity: moderate_to_high

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 40    # baseline (no extreme tank spikes here; threat is on Erunak P1, then small boss P2)
    raid_heal_priority: ELEVATED               # P2 has Mind Fog + Unrelenting Agony

  pre_pull:
    - drink to full mana
    - Earthliving_Weapon imbue if missing
    - Water_Shield up if missing
    - Earth_Shield on tank
    - Healing_Stream_Totem placed before pull

  per_phase:
    phase_1_erunak:
      cooldown_plan: |
        - Hold Mana_Tide for P2 (Unrelenting Agony channels are mana-heavy on raid heal).
        - Healing_Stream_Totem maintained.
      dispels:
        - Emberstrike (76165 — MAGIC) on tank — dispel ramping debuff to keep tank healing manageable
        - Magma Splash DoT (76170 — MAGIC) on cone-clipped allies (rare if tank positioning is correct)
      positioning: behind boss with the rest of the party (Magma Splash is frontal cone)

    phase_2_ghursha:
      cooldown_plan: |
        - Mana_Tide_Totem: cast at start of P2 OR when self.mana < 60% — P2 is the mana-heavy phase.
        - Spirit_Link_Totem: hold until raid is taking heavy Unrelenting Agony damage with bad positioning.
        - Spiritwalker_Grace: PROACTIVE — pop before Mind Fog (76230) spawns if you're cast-locked, since Fog forces movement.
      dispels:
        - Absorb Magic (76308 — MAGIC OFFENSIVE / PURGE) on Ghur'sha — HIGHEST PRIORITY DISPEL IN INSTANCE.
          Resto Shaman cannot purge directly (Purge is Enhancement-only in Cata).
          Bot logic: stop offensive casts (lightning bolts, etc.) until purge lands;
          flag the cast for melee/ranged purgers in chat ping.
      raid_heal_emphasis: |
        Unrelenting Agony (76339) channels deal ~2k Shadow/s to all party members for 10s.
        Raid heal mode: Chain Heal on cluster, Healing Rain if available (Resto Shaman gets
        Healing Rain at level 81; Chain Heal as primary AoE).
      positioning: spread to avoid Mind Fog overlap; LoS the Unrelenting Agony cast if geometry permits

  anti_patterns:
    - DO NOT HEAL THE ENSLAVED PLAYER (76207). They must be DPS'd to 50% HP to break free.
      Healing them keeps them mind-controlled and stat-buffed against the party.
      This is the single biggest healer trap in this dungeon.
      Bot rule: when an ally has the Enslave aura, REMOVE them from the heal target pool entirely
      until aura.expires_in_ms < 2000 OR aura.removed.
    - DO NOT stand in Mind Fog clouds — silenced/pacified healer = dead party.

  cooldown_summary:
    mana_tide: at P2 start, OR self.mana_pct < 60
    spirit_link: P2 reactive — when 3+ allies <40% from Unrelenting Agony cluster
    spiritwalker_grace: PROACTIVE before Mind Fog spawn (P2)
    natures_swiftness: reactive on tank emergency
    earth_elemental: skip (5-man)

  reference_mechanics:
    - encounter.md: Emberstrike (76165), Magma Splash (76170), Lava Bolt (76171),
      Earth Shards (84931), Enslave (76207), Mind Fog (76230), Absorb Magic (76308),
      Unrelenting Agony (76339)
```

---

## Boss 4 — Ozumat

```yaml
boss_healer:
  boss: ozumat
  difficulty_grade: extreme            # P3 specifically — the hardest healer phase in the instance
  mana_intensity: high

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50              # raised — Behemoth Shadow Blast is huge in P1
    chain_heal_trigger: party.injured_count(radius=12, hp_pct_lt=85) >= 2  # lowered count from 3; raid is small (5) and damage is constant
    crisis_mode_pct: 30                                  # raised from baseline 25 — drink before this fight, manage tightly

  pre_pull:
    - drink to FULL MANA — most important drink window in instance
    - Earthliving_Weapon imbue if missing
    - Water_Shield up if missing
    - Earth_Shield on tank
    - Healing_Stream_Totem placed before pull

  per_phase:
    phase_1_filthy_tide:
      cooldown_plan: |
        - Hold Mana_Tide and Spirit_Link for P3.
        - Healing_Stream_Totem maintained.
      dispels:
        - Veil of Shadow (CURSE, heroic only) — dispel ASAP, -50% healing is severe
      raid_heal_emphasis: |
        Brain Spike (Mindlasher) hits party for ~7k Shadow + mana drain — interrupting is melee/ranged duty,
        but if missed, immediately Chain Heal to top.
        Behemoth Shadow Blast hits 5y radius — keep tank topped, cushion non-tanks if knocked.
      positioning: near Neptulon's pedestal; do NOT chase adds far from pedestal

    phase_2_sappers:
      cooldown_plan: |
        - Healing_Stream_Totem maintained.
        - Hold Mana_Tide for P3 unless self.mana < 50%.
      dispels:
        - Veil of Shadow (heroic) if Mindlashers still up
      raid_heal_emphasis: |
        Tank takes Aura of Dread + Blight Beast melee while kiting.
        Party is mobile dodging Blight pools (83561). Pre-cast HoTs.
      positioning: stay near tank's kite path; avoid Aura of Dread radius (7y)

    phase_3_tidal_surge:
      cooldown_plan: |
        - POP EVERYTHING FROM PHASE START.
        - Mana_Tide_Totem: cast immediately at P3 start.
        - Spirit_Link_Totem: cast on first Blight stack ramp moment (~5-10 stacks) when party clusters on Ozumat.
        - Spiritwalker_Grace: cycle on cooldown — fight is mobile + cast-heavy.
        - Natures_Swiftness: save for tank emergency or major Blight pool overlap.
      dispels: none mandatory in P3 (Aura of Dread vulnerability is non-dispellable per most guides)
      raid_heal_emphasis: |
        Tidal Surge buff gives +500% healing — your numbers are MASSIVE.
        Default to Chain Heal on cluster. Healing Rain if available.
        Stacking Blight DoT scales linearly with stacks; survival = burning Ozumat fast enough.
      positioning: stack with party on Ozumat; move only to dodge fresh Blight pools

  anti_patterns:
    - do NOT save cooldowns for "later" in P3 — there is no later; if you don't pop now, you die at high stacks
    - do NOT stand in Blight pools (83561) — even with Tidal Surge buff they still hurt

  cooldown_summary:
    mana_tide: at P3 START (do not delay)
    spirit_link: P3 mid-stack-ramp
    spiritwalker_grace: P3 cycle on cooldown
    natures_swiftness: reactive
    earth_elemental: defensible — drop in P3 if SLT on CD and tank in trouble (this is the one fight in ToTT where EET earns its slot)

  reference_mechanics:
    - encounter.md: Brain Spike (Mindlasher abilities), Blight Spray (Behemoth),
      Aura of Dread (Blight Beast), Blight of Ozumat (83561), Summon Blight of Ozumat (83524),
      Tidal Surge (Neptulon party buff)
```

---

## Cross-fight cooldown budget (full clear, healer perspective)

```yaml
cooldown_budget:
  # Counts the number of times each cooldown should fire across a full clear,
  # given the per-fight plans above.

  mana_tide_totem:
    expected_uses: 2-3
    distribution:
      - Ghur'sha P2: 1 (mandatory — reset window)
      - Ozumat P3: 1 (mandatory — at phase start)
      - Ulthok / Naz'jar: 0-1 if mana drops

  spirit_link_totem:
    expected_uses: 1-2
    distribution:
      - Ozumat P3 mid-stack-ramp: 1 (mandatory)
      - Ghur'sha P2 cluster moment: 0-1 (situational)

  spiritwalker_grace:
    expected_uses: 4-6
    distribution:
      - Ulthok fissure dodging: ~1-2
      - Ghur'sha Mind Fog: ~1-2
      - Ozumat P3 cycling: ~2-3

  natures_swiftness:
    expected_uses: 2-4 (tank emergency dependent)

  earth_elemental_totem:
    expected_uses: 0-1
    distribution:
      - Ozumat P3 only: 0-1 (defensible; not mandatory)
```

---

## Healer-specific anti-pattern summary

```yaml
anti_patterns:
  - id: enslave_heal_trap
    fight: mindbender_ghursha
    rule: |
      When an ally has the Enslave aura (76207), REMOVE them from heal target selection.
      They must be DPS'd to 50% HP to break free. Healing them prolongs the mind control.
    bot_implementation: |
      Add a target-filter check in PARTY_LOWEST_HP / CHAIN_HEAL_PRIMARY / all party-target
      selectors: if aura.active(target, 76207), skip target unless aura.expires_in_ms < 2000.

  - id: cooldown_hoarding_p3
    fight: ozumat
    rule: |
      Do NOT save Mana Tide / Spirit Link "for later" in Ozumat P3. Phase 3 is a
      stacking-DoT race; cooldowns saved for "later" never get used because either
      the boss dies or the party wipes.
    bot_implementation: |
      Override default cooldown gates in P3: mana_tide fires at phase start regardless
      of mana%; spirit_link fires on first cluster + 5 Blight stacks observed.

  - id: chase_during_waterspout
    fight: lady_nazjar
    rule: do not waste GCDs trying to heal/melee the boss during Waterspout — focus add survival
    bot_implementation: |
      Bot already only heals; this is melee/ranged DPS concern. Healer doc notes it
      so target-priority logic for healers excludes the boss during this phase.
```

---

## How this layers on the Resto Shaman spec

`docs/specs/resto-shaman.md` defines the base rotation, cooldown profile, mana thresholds, and target taxonomy. This dungeon doc only specifies *deviations* per encounter:

| What the spec defines | What this doc adjusts |
|---|---|
| Tier 2 tank emergency at hp<40% | Raised to <50% on Naz'jar / <45% on Ulthok / <50% on Ozumat |
| Tier 5 Chain Heal at 3+ <85% | Tightened to 3+ <80% on Ulthok; loosened to 2+ <85% on Ozumat |
| Cooldown Mana Tide at self<60% | Tied to specific phase triggers (Ghur'sha P2 start, Ozumat P3 start) |
| Cooldown Spirit Link at 3+ <40% clustered | Same predicate, but pre-allocated to Ozumat P3 + Ghur'sha P2 |
| Crisis mode at mana<25% | Raised to <30% on Ozumat |
| Target taxonomy PARTY_LOWEST_HP | Filtered to exclude Enslaved players on Ghur'sha P2 |

When `AltbotCombat.cpp` is implemented, the boss-detection layer should load this dungeon's threshold-overrides into the active-encounter profile and apply them on top of the spec defaults.
