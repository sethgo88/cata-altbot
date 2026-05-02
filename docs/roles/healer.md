# Healer — Role Generalist

**Class-agnostic healer patterns** that apply across all content (dungeon, raid, world). Per-spec details (which spell ID, which CD name, which mana model) live in `docs/specs/{spec}.md`. Per-encounter overrides (threshold tweaks, fight-specific CD plans) live in `docs/dungeons/{dungeon}/healer.md`.

This doc is the central reference for *how a healer thinks* — triage, dispel taxonomy, mana pacing, target selection, anti-patterns. The bot's healer logic in `AltbotCombat.cpp` should be structured around the patterns documented here, with class-specific bindings injected from the spec doc.

---

## Triage hierarchy

```yaml
triage:
  # Ordered priority. The "highest priority that fires" determines the heal target this tick.
  - tier: 1
    name: SELF (dead healer = wipe)
    target: SELF
    fire_when: SELF.hp_pct < SELF_EMERGENCY_THRESHOLD   # spec default ~35; raised on heroic encounters
    rationale: |
      A dead healer is a fight-ending event. Self-preservation is non-negotiable
      and ranks above tank rescue except in the narrow case where the tank is
      about to die from a known one-shot mechanic.

  - tier: 2
    name: TANK emergency
    target: TANK
    fire_when: TANK.hp_pct < TANK_EMERGENCY_THRESHOLD   # spec default ~40; encounter overrides per fight
    rationale: |
      Tank death cascades — boss aggro shifts, mechanics scatter, healer becomes
      the next target. Higher HP threshold than non-tank emergency because tanks
      take spike damage with little warning.

  - tier: 3
    name: NON-TANK ally emergency
    target: PARTY_LOWEST_HP (excluding TANK if tier 2 already fired this tick)
    fire_when: PARTY_LOWEST_HP.hp_pct < ALLY_EMERGENCY_THRESHOLD   # default ~50
    rationale: |
      Below this HP %, an ally is one mechanic away from dying. Cluster-heal
      isn't enough; needs a focused fast cast.

  - tier: 4
    name: AOE cluster heal
    target: cluster_primary
    fire_when: party.injured_count(radius=CLUSTER_RADIUS, hp_pct_lt=CLUSTER_THRESHOLD) >= CLUSTER_COUNT
    spec_defaults:
      CLUSTER_RADIUS: 10-12 (matches Chain Heal jump radius / Holy Radiance / Wild Growth)
      CLUSTER_THRESHOLD: 85
      CLUSTER_COUNT: 3 (5-man) / 4 (raid 10) / 5+ (raid 25)

  - tier: 5
    name: Heavy single-target heal
    target: PARTY_LOWEST_MISSING_HP    # absolute missing HP, favors tanks late-fight
    fire_when: target.hp_pct < HEAVY_HEAL_THRESHOLD   # default ~60
    rationale: |
      Different from tier 3 — this is for ramping damage that needs throughput,
      not panic. Use the heavy spell (Greater Healing Wave / Holy Light / Greater
      Heal / Healing Touch).

  - tier: 6
    name: Spot heal
    target: PARTY_LOWEST_HP
    fire_when: target.hp_pct < SPOT_HEAL_THRESHOLD    # default ~70
    spell_choice: prefer the cheap-instant or HoT-renewer over the burst spell

  - tier: 7
    name: Filler
    target: PARTY_LOWEST_HP
    fire_when: target.hp_pct < FILLER_THRESHOLD       # default ~90
    spell_choice: cheapest mana-efficient heal

target_filters:
  # Applied to ALL tiers — exclude these targets from selection regardless of HP
  - exclude: aura.active(target, MIND_CONTROL_OR_CHARM)
    rationale: |
      Healing a mind-controlled / charmed ally prolongs their hostility against
      the party. Encounter docs flag specific mechanics (Enslave on Ghur'sha,
      etc.). The bot must NEVER heal a target with an active charm/MC aura.
  - exclude: target.is_dead
  - exclude: target.is_pet AND not is_party_pet
  - exclude: target.distance_from_self > spell.max_range
```

The bot's healer rotation in `AltbotCombat.cpp` evaluates these tiers top-down and selects the first that fires. `SELF_EMERGENCY_THRESHOLD`, `TANK_EMERGENCY_THRESHOLD`, etc. are configurable per encounter via the dungeon docs.

---

## Dispel-type priority taxonomy

```yaml
dispel_priority:
  # Higher tier = dispel first; ties broken by remaining duration (sooner = first).

  - tier: 1
    name: TIME_CRITICAL
    description: |
      Debuffs with a timed detonation, ramping damage, or mind control. These have
      a hard window — missing the dispel = damage event lands. Dispel within 1 GCD
      of application.
    examples:
      - timed magic detonation (e.g., Lightning Surge — encounter.md flags these)
      - charm / mind control (where dispellable)
      - ramping DoT that compounds (some boss-specific debuffs)
    bot_action: PRE-EMPT current cast — interrupt rotation to dispel

  - tier: 2
    name: MOVEMENT_IMPAIRING
    description: |
      Slows / roots that prevent the target from escaping a follow-up mechanic
      (e.g., Curse of Fatigue on Ulthok — slowed players die in fissures). Time-
      sensitive but allows ~1-2 GCDs of warning.
    examples:
      - movement slows when a ground AoE / fissure spawn is imminent
      - silences on healer (special case — see below)
    bot_action: dispel within 1-2 GCDs

  - tier: 3
    name: HEALING_REDUCTION
    description: |
      Debuffs that reduce healing received (e.g., Mortal Strike, heroic-only
      Veil of Shadow on ToTT Ozumat). Critical when the target is also taking
      damage; less critical on a topped-off ally.
    bot_action: dispel when target.hp_pct < 80 AND damage_taken_pct_recent > 0

  - tier: 4
    name: SUSTAINED_DOT
    description: |
      Standard damage-over-time debuffs (Disease, sustained Magic). Lower
      priority because the damage is already being absorbed by the heal flow.
    examples:
      - Fungal Spores (Disease, ToTT Naz'jar)
      - Wall of Corruption (Disease, ToTT trash)
    bot_action: dispel when GCD is otherwise free

  - tier: 5
    name: COSMETIC / LOW_PRIORITY
    description: |
      Debuffs with minimal damage or utility impact. Skip if any tier 1-4
      dispels are pending.
    bot_action: ignore unless idle GCD

dispel_type_to_class_map:
  # Which classes can dispel which type. For reference when designing per-class healer specs.
  curse:
    - resto_shaman          # Cleanse Spirit (with Cleansing Waters talent in 4.3.4)
    - druid_(any_spec)      # Remove Corruption / Nature's Cure (resto-talented)
    - mage                  # Remove Curse
  magic:
    - holy_priest           # Dispel Magic (offensive too)
    - disc_priest           # Dispel Magic
    - holy_paladin          # Cleanse (with Sacred Cleansing talent for Magic)
    - resto_shaman          # Cleanse Spirit (with Improved Cleanse Spirit talent for Magic in 4.3.4)
    - mage                  # Spellsteal (offensive purge)
  disease:
    - holy_paladin          # Cleanse
    - priest_(any_spec)     # Cure Disease / Mass Dispel
    - resto_shaman          # Cleanse Spirit (with Improved Cleanse Spirit for Disease — actually no, Disease is Paladin/Priest/DK)
    - death_knight          # any spec; Sacred Cleansing-equivalent
  poison:
    - holy_paladin          # Cleanse
    - resto_druid           # Remove Corruption (resto)
    - rogue                 # Cloak of Shadows (self only)
    - shaman_(any_spec)     # Cleansing Totem (passive); Cleanse Spirit doesn't remove Poison in 4.3.4
  enrage_offensive:         # purges hostile enrage buffs (different from cleansing allies)
    - hunter                # Tranq Shot
    - druid                 # Soothe (Cata)
    - shaman_(any_spec)     # Purge (Resto cannot Purge in 4.3.4 — Enhancement/Elemental only)
```

Per-spec healer docs declare which dispel-types their class handles; the role-level priority rules above apply uniformly.

---

## Mana pacing

```yaml
mana_pacing:
  # Three operational modes. The bot transitions between them based on self.mana_pct.

  modes:
    - mode: NORMAL
      threshold_enter: mana_pct >= NORMAL_FLOOR     # default 50
      behavior: |
        Full rotation available. Use heavy heals (GHW / Holy Light / Greater Heal)
        when needed. Cast filler heals to top off ambient damage. Maintain HoTs
        and shields freely.

    - mode: CONSERVE
      threshold_enter: mana_pct < CONSERVE_FLOOR    # default 50
      threshold_exit: mana_pct >= NORMAL_FLOOR + 10 (hysteresis)
      behavior: |
        Skip heavy heals (cost > value). Prefer cheap-instant or filler heals.
        Drop tier 7 (filler) entirely if mana < CONSERVE_FLOOR - 10.
        Hold mana cooldowns (Mana Tide, Innervate, Hymn of Hope) for the next
        damage spike.

    - mode: CRISIS
      threshold_enter: mana_pct < CRISIS_FLOOR      # default 25
      threshold_exit: mana_pct >= CONSERVE_FLOOR + 10
      behavior: |
        Triage tiers 1-3 only (self / tank / non-tank emergency). Skip cluster
        heals unless 4+ allies in crisis. Force filler-tier mana-cheapest spell
        regardless of Tidal Waves / Holy Power / proc state.
        Fire mana CDs IMMEDIATELY (Mana Tide, Innervate, Hymn of Hope).
        Pop mana potion if available.

  encounter_overrides:
    # Specific dungeons/bosses raise these floors due to fight characteristics
    long_fights_with_heavy_raid_damage:
      example: ToTT Ozumat P3
      override: CRISIS_FLOOR raised from 25 to 30 (fire mana potion earlier)
      override: cooldown_hoarding_disabled (use Mana Tide on phase start regardless)

cooldown_pacing:
  # How to think about healer cooldowns at the role level.

  categories:
    - name: PERSONAL_DEFENSIVES
      examples_by_class:
        resto_shaman: Stoneform racial, Spiritwalker's Grace (movement)
        holy_paladin: Divine Protection, Lay on Hands (self), Hand of Sacrifice
        holy_priest: Desperate Prayer, Fade
        disc_priest: Pain Suppression on self, Power Word: Barrier
        resto_druid: Barkskin
      use: reactive on SELF.hp_pct < threshold OR mechanic-incoming
      max_uses_per_fight: cooldown-bound; usually 2-4

    - name: MAJOR_RAID_CDS
      examples_by_class:
        resto_shaman: Spirit Link Totem
        holy_paladin: Aura Mastery, Divine Guardian
        holy_priest: Divine Hymn, Guardian Spirit (single-target survival)
        disc_priest: Power Word: Barrier, Pain Suppression (single-target)
        resto_druid: Tranquility
      use: |
        Save for KNOWN damage spikes (named encounter mechanics) OR
        emergency unfocused damage when 3+ allies are in tier 3 crisis.
      anti_pattern: do NOT save these "for later" — late = dead

    - name: MANA_EXTENSION_CDS
      examples_by_class:
        resto_shaman: Mana Tide Totem
        holy_paladin: Divine Plea (Cata baseline)
        holy_priest: Hymn of Hope
        disc_priest: Hymn of Hope
        resto_druid: Innervate (self or another caster)
      use: fire when self.mana_pct < threshold AND fight will continue >30s

    - name: BURST_INSTANT_CDS
      examples_by_class:
        resto_shaman: Nature's Swiftness (paired with Greater Healing Wave)
        holy_paladin: Hand of Sacrifice (on tank), Holy Avenger (Cata: situational)
        holy_priest: Holy Word: Sanctuary (zone)
        disc_priest: Power Infusion (on self for cast speed)
        resto_druid: Nature's Swiftness (paired with Healing Touch)
      use: |
        Bind to a specific consumer cast. Never fire without the empowered
        cast queued — wasted CD.
```

---

## Target-selection rules

```yaml
target_selection:
  # Generic rules for resolving target tokens like PARTY_LOWEST_HP, CHAIN_HEAL_PRIMARY, etc.

  PARTY_LOWEST_HP:
    definition: ally with lowest hp_pct, in spell range, alive, not filtered
    tiebreaker: lowest absolute HP missing
    use_in: spot heal tiers, filler tier

  PARTY_LOWEST_MISSING_HP:
    definition: ally with greatest absolute missing HP (max_hp - current_hp), in range, alive, not filtered
    tiebreaker: tank role flag (favors tanks in late-fight when everyone is below 100%)
    use_in: heavy single-target heal tier

  CHAIN_HEAL_PRIMARY (and equivalents — Wild Growth, Holy Radiance):
    definition: ally satisfying:
      1. hp_pct < SPOT_HEAL_THRESHOLD
      2. count_allies_within(radius=spell.jump_radius, hp_pct_lt=CLUSTER_THRESHOLD) >= 2
    tiebreaker: prefer ally with active class HoT (Riptide for Shaman; Lifebloom for Druid; Renew for Priest)
    rationale: pre-existing HoT amplifies chained/spread heals (Resto Shaman: +25% on Riptide consume)

  TANK:
    definition: priority order:
      1. group member with explicit TANK role flag (set by group manager)
      2. group member with highest threat on current boss target
      3. master (the player who summoned the bot) if soloing/duoing
    fallback: if no candidate resolves, treat tier 2 (tank emergency) as inactive

target_filter_excludes:
  - aura.active(target, MIND_CONTROL_OR_CHARM)   # see Triage anti-pattern
  - target.is_dead
  - target.is_in_vehicle AND vehicle.healable == false
  - target.distance > spell.max_range
  - target.hp_pct == 100 AND not aura.preheal_target(target)   # don't waste GCDs on full-HP targets
```

---

## Support DPS during healing downtime

```yaml
support_dps:
  # When the bot has no useful heal target AND mana is comfortable, fire offensive
  # spells to contribute to group damage. Healing remains priority 1; this is the
  # LOWEST rotation tier and only resolves when every triage tier is null.

  principle: |
    Healers should not stand idle when nothing needs healing. Most healer specs
    have offensive tools that contribute meaningful damage during downtime windows
    (tank stable, party topped off, no debuffs to dispel, mana > comfortable threshold).
    For one spec (Disc Priest via Atonement), offensive casts ARE the healing model
    and this is not "downtime DPS" — it's the core rotation.

  gating_rules:
    # All must hold before the support-DPS tier fires
    - all_triage_tiers_null: true                 # no ally needs healing
    - no_dispels_pending: true                    # no high-priority debuffs
    - SELF.mana_pct >= SUPPORT_DPS_MANA_THRESHOLD # spec default ~70-80%
    - aura.active(TANK, healer_specific_maintenance_buff)  # tank's main healer-applied buff (Earth Shield / Beacon / Renew / Lifebloom) is up
    - no_predicted_damage_in_next_2s              # if a known scripted damage event is coming, don't lock into a hard-cast

  cast_safety_priority:
    # Within the support-DPS tier, prefer instant casts so the bot can react to incoming damage
    1_instant_dot_or_debuff: |
      Apply or refresh a damage-over-time effect on the boss target (instant cast,
      no GCD lock for healing reactions). Examples: Flame Shock (Resto Shaman),
      Holy Fire (Holy Priest, instant via Chakra), Moonfire (Resto Druid).
    2_instant_direct_damage: |
      Cast an instant direct-damage spell. Examples: Earth Shock (Resto Shaman if
      shock CD off + DoT already applied), Holy Shock offensive (Holy Paladin).
    3_hard_cast_filler: |
      Only when CONFIDENT no heal will be needed in the next cast time. Examples:
      Lightning Bolt (Resto Shaman with Telluric Currents talent — turns the cast
      mana-positive when the talent is allocated), Smite (Disc/Holy Priest),
      Wrath (Resto Druid).

  cancel_conditions:
    # Mid-cast cancellation rules
    - cancel_if: any triage tier 1-3 (self/tank/non-tank emergency) becomes true
    - cancel_if: time-critical dispel becomes pending
    - cancel_if: SELF.mana_pct drops below SUPPORT_DPS_MANA_THRESHOLD

  default_thresholds:
    SUPPORT_DPS_MANA_THRESHOLD: 75    # below this, conserve for healing
    PARTY_TOP_THRESHOLD: 95           # above this for ALL party members, support DPS unblocked
    HARD_CAST_MANA_THRESHOLD: 85      # only hard-cast Lightning Bolt / Smite / Wrath above this
    HARD_CAST_TIME_LIMIT_MS: 2500     # hard-casts longer than this (with haste) skip; default 2.5s safe

class_spec_support_tools:
  resto_shaman:
    primary_dot: Flame Shock (instant, 14s)
    primary_instant: Earth Shock (shared shock CD with Flame Shock)
    primary_filler: Lightning Bolt (2.5s; Telluric Currents talent returns 20-40% damage as mana)
    talent_recommendation: |
      Take Telluric Currents (Resto tier 5, 2/2) for any bot expected to run dungeons.
      Turns Lightning Bolt into a mana-positive cast in damage-output windows.
      Skip only for raid-progression specs where every GCD is a heal.
    notes: |
      Maelstrom Weapon proc on Enhancement makes Lightning Bolt instant — irrelevant
      for Resto. Resto Shaman's support DPS is non-trivial but caster-bound.

  holy_paladin:
    primary_dot: n/a (no DoT)
    primary_instant: Crusader Strike + Judgment + Holy Shock (offensive)
    primary_filler: Exorcism (instant on Art of War proc)
    talent_recommendation: Tower of Radiance (Holy tier 5) generates Holy Power from Flash of Light; pairs with offensive HP-builders
    notes: |
      Holy Power generated from offensive abilities (CS, Judgment, Holy Shock-offensive)
      can be spent on Word of Glory healing — bidirectional flow. Holy Paladin support
      DPS is significant.

  holy_priest:
    primary_dot: Holy Fire (instant in Chakra: Chastise mode, also a DoT)
    primary_instant: Holy Shock-offensive (in Chakra: Chastise)
    primary_filler: Smite (1.5s, buffed by Chakra: Chastise)
    talent_recommendation: Chakra (Holy tier 6) — Chakra: Chastise mode buffs offensive output for trash/single-target downtime
    notes: |
      Chakra: Serenity is the tank-heal mode; Chakra: Sanctuary is the AoE-heal mode;
      Chakra: Chastise is the support-DPS mode. Bot toggles based on encounter need.

  disc_priest:
    primary_dot: Holy Fire (when Atonement specced)
    primary_instant: Penance (offensive)
    primary_filler: Smite (with Atonement, heals lowest-HP ally for 100% of Smite damage)
    talent_recommendation: Atonement (Disc tier 1) — Smite/Holy Fire heal via Atonement
    notes: |
      Atonement Disc Priest doesn't HAVE downtime DPS — DPS IS the healing model
      for Smite-Atonement specs. This bot's "rotation" is a damage rotation that
      happens to heal as a side effect. Treat as a special case in spec docs.

  resto_druid:
    primary_dot: Moonfire + Insect Swarm (instants, 12s + 12s)
    primary_instant: n/a (Wrath is hard-cast; Starsurge is balance-tree only)
    primary_filler: Wrath (2.0s)
    talent_recommendation: |
      No specific talent enables this for Druid; Resto's offensive output is
      modest but the DoTs are free mana when applied.
    notes: |
      Resto Druid has the cleanest "downtime DPS" feel — DoT spread + Wrath spam
      while keeping HoTs rolling.

anti_pattern: |
  Do NOT support-DPS at the expense of healing. The gating rules above must hold
  EVERY tick. The moment any triage tier becomes true OR mana drops below threshold,
  the support-DPS tier is suppressed. This is enforced by the rotation evaluator's
  top-down "first-firing tier wins" property.
```

---

## Anti-patterns (cross-content)

```yaml
anti_patterns:
  - id: heal_charmed_or_mc_ally
    rule: NEVER heal a target with an active mind-control / charm aura
    rationale: |
      Healing a hostile-controlled ally prolongs their attack on the party.
      Specific mechanics: Enslave (ToTT Ghur'sha), various raid charm mechanics.
    bot_implementation: |
      Add aura-based target filter at the target_selection layer. Filter applies
      to ALL triage tiers and all target tokens.

  - id: cooldown_hoarding
    rule: do not save raid CDs "for later" if a known damage event is imminent
    rationale: |
      Late = dead. The cost of using a CD now and re-rolling for next phase
      is much lower than the cost of a wipe.
    bot_implementation: |
      Per-encounter override the default conserve gates with phase-start triggers
      (e.g., Mana Tide fires on Ozumat P3 entry regardless of mana%).

  - id: drink_in_combat
    rule: never attempt to drink while in_combat
    rationale: drink interrupts on damage; wasted GCD and possibly food
    bot_implementation: |
      `combat.in_combat == false` is a hard precondition for drink/eat actions.

  - id: overheal_topped_target
    rule: do not cast heals on targets at hp_pct == 100
    rationale: GCD waste; mana waste
    bot_implementation: |
      Filter target_filter_excludes already covers this. Do not pre-cast heals
      "in case" unless explicit pre-heal aura is set (target_aura == "preheal").

  - id: forget_dispel_during_burst
    rule: time-critical dispels (tier 1) interrupt the rotation; do NOT delay them for an in-progress heal cast
    rationale: missing a tier-1 dispel = avoidable damage event lands
    bot_implementation: |
      Dispel-priority check runs BEFORE the rotation check each tick. If a tier-1
      dispel is pending and bot is currently casting a non-emergency heal, cancel
      the cast and dispel.

  - id: ignore_water_shield_orb_count
    rule: |
      Resto Shaman specifically — keep Water Shield orbs >= 1 between fights;
      Holy Paladin keep Beacon of Light up; Holy Priest renew the right buff
      (Inner Fire vs Inner Will); Disc Priest keep Power Word: Shield active
      on tank if no Atonement spam.
    rationale: per-class passive maintenance is a meaningful throughput / mana boost
    bot_implementation: per-class spec doc maintenance pass enforces this
```

---

## Class-spec mapping table

This table shows how each Cata 4.3.4 healer spec realizes the role-level patterns above. Per-spec docs (`docs/specs/{spec}.md`) declare these bindings; this table is a quick cross-reference.

| Pattern | Resto Shaman | Holy Paladin | Holy Priest | Disc Priest | Resto Druid |
|---|---|---|---|---|---|
| Self emergency heal | Healing Surge | Holy Shock + Word of Glory | Flash Heal + PW:Shield | PW:Shield + Penance | Healing Touch + Healing Surge equivalent (Regrowth) |
| Tank emergency heal | Healing Surge → Greater Healing Wave (NS-empowered) | Holy Shock + Holy Light | Flash Heal + Greater Heal | Penance + Power Word: Shield | Regrowth + Healing Touch (NS-empowered) |
| Heavy single-target | Greater Healing Wave | Holy Light | Greater Heal | Penance | Healing Touch |
| Cheap filler | Healing Wave | Holy Light (with Tower of Radiance HP gen) | Heal | Penance / Heal | Nourish |
| HoT-rolling | Riptide | Beacon of Light + Sacred Shield | Renew | Renew (with talents) | Lifebloom + Rejuvenation |
| Smart-AoE | Chain Heal | Holy Radiance (Cata 4.3) | Prayer of Healing | Prayer of Healing + PW:B | Wild Growth |
| Major raid CD | Spirit Link Totem | Aura Mastery + Divine Guardian | Divine Hymn | Power Word: Barrier | Tranquility |
| Mana CD | Mana Tide Totem | Divine Plea | Hymn of Hope | Hymn of Hope | Innervate |
| Instant-burst CD | Nature's Swiftness | Holy Avenger (where applicable) / Hand of Sacrifice | Surge of Light proc | Power Infusion (self) | Nature's Swiftness |
| Cleanse Curse | Cleanse Spirit (talented) | n/a | Mass Dispel (curse only via talents) | Mass Dispel | Remove Corruption / Nature's Cure |
| Cleanse Magic | Cleanse Spirit (talented) | Cleanse (talented) | Dispel Magic | Dispel Magic | Nature's Cure (resto-talented) |
| Cleanse Disease | n/a (Resto Shaman cannot dispel Disease in 4.3.4) | Cleanse | Cure Disease | Cure Disease | n/a |
| Cleanse Poison | n/a (Cleansing Totem passive only) | Cleanse | n/a | n/a | Remove Corruption |
| Mind-Control break (charmed ally) | DAMAGE — Hex (offensive) | DAMAGE — Repentance breaks on damage; no heal-side cure | Mass Dispel (where applicable) | Mass Dispel | n/a — bot relies on DPS |

When a per-class spec doc is authored, it should declare its bindings against this table and only document deviations from the role-generalist patterns above.

---

## How dungeon-level docs reference this doc

Per-dungeon healer docs (`docs/dungeons/{dungeon}/healer.md`) should:

1. Reference this doc by file path in the preamble
2. Only document **deviations** from the generalist patterns (encounter-specific threshold tweaks, fight-specific CD plans, dungeon-specific anti-patterns like the Enslave heal trap)
3. Not duplicate the triage hierarchy, dispel taxonomy, mana pacing, or anti-patterns documented here

The bot's `AltbotCombat.cpp` healer logic should be implemented around this doc's structure and inject:
- Class-specific spell IDs and CD names from the active spec doc
- Encounter-specific threshold overrides from the active dungeon doc

---

## Open hooks for `AltbotCombat.cpp`

The bot's healer evaluation order each tick:

```
1. resolve target tokens (TANK, PARTY_LOWEST_HP, etc.)
2. apply target_filter_excludes (skip MC'd, dead, out-of-range)
3. evaluate dispel-priority (tier 1 dispel pre-empts everything else)
4. evaluate triage tiers top-down
5. select first-firing tier; cast its bound spell on resolved target
6. update mana mode (NORMAL / CONSERVE / CRISIS) for next tick
```

Class-specific bindings come from `docs/specs/{active_spec}.md`.
Encounter-specific overrides come from `docs/dungeons/{active_dungeon}/healer.md`.
