# The Vortex Pinnacle — Healer

**Healer-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the healer reacts. Class-agnostic where possible.

**Layering**:
- Cross-content healer patterns (triage hierarchy, dispel taxonomy, mana pacing, anti-patterns) live in `docs/roles/healer.md`
- Per-spec rotation / CD / mana model lives in the active healer's `docs/specs/` doc (resto-shaman, holy-paladin, holy-priest, disc-priest, resto-druid)
- This doc layers Vortex-Pinnacle-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_healer_overview:
  total_bosses: 3
  estimated_full_clear_minutes: 25-45
  mana_profile:
    ertan: low                              # short fight; tank pressure only; predictable cyclone retract
    altairus: moderate                      # Chilling Breath spike on cone target; Twisting Winds chip damage on heroic
    asaad: high                             # heroic dispel pressure (Static Cling) + Skyfall Star bleed + Supremacy ticks
  drink_windows:
    - after Ertan
    - after Altairus                        # MOST IMPORTANT — go into Asaad full mana on heroic
  key_responsibilities:
    - magic_dispel_static_cling             # CRITICAL on Asaad heroic
    - poison_dispel_lethargic_poison        # trash (Empyrean Assassin)
    - dispel_blacklist_per_fight            # do NOT dispel Downwind of Altairus
    - mana_pacing_for_asaad_heroic
    - cooldown_pacing                       # Mana Tide / Spirit Link / Hymn / Innervate placement
    - position_inside_ring (Ertan)          # explicit override of healer-at-max-range default
    - position_upwind (Altairus)            # group repositioning aware
    - position_inside_triangle (Asaad)      # Unstable Grounding Field stack response
```

---

## Trash — healer notes

```yaml
trash_healer:
  cyclone_summit_packs:
    dispels: []                             # no significant dispel calls except Gust Soldier DoT
    cooldown_use: none required; reactive only
    notes: |
      Watch for Cloud Prince Turbulence (AoE-burst on cast) — pre-shield melee/tank.
      Wild Vortex caster cast — interrupt role for DPS; healer reactive.

  bridge_howling_gales:
    notes: |
      Group DPSes Howling Gales to disable. Healer stays back from the AoE
      footprint. KEY: do NOT walk into the gale to top off a stray melee —
      knock-off = teleport to dungeon entrance for the healer (= dead-to-fight).

  nimbus_rise_packs:
    dispels:
      - { name: Empyrean Assassin Lethargic Poison, type: poison, priority: HIGH }
      - { name: Turbulent Squall self-buff, type: magic (purge), priority: MEDIUM }   # if class can purge
    cooldown_use: |
      Young Storm Dragon trash is the highest-pressure trash in the dungeon.
      Plan a CD (Spirit Link / Tranquility / Healing Tide / equivalent) for
      that pack if pulling at low gear.
    notes: interrupt role is DPS's; healer reactive

  temple_of_asaad_packs:
    dispels: []                             # no specific dispel pressure
    cooldown_use: none required pre-Asaad
    notes: |
      Temple Adept Greater Heal interrupts are DPS responsibility. Healer's
      role: standard tank-heal flow + reactive DPS heals if multi-target
      pull goes long.
```

---

## Boss 1 — Grand Vizier Ertan

```yaml
boss_healer:
  boss: grand_vizier_ertan
  difficulty_grade: easy
  mana_intensity: low

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 45    # standard — Lightning Bolt damage steady, predictable
    tier_4_aoe_cluster: party.injured_count(radius=10, hp_pct_lt=80) >= 3  # raised — group is stacked-in-ring already

  predictable_spike_events:
    - cyclone_shield_retract:
        warning: ~30s cadence, visual telegraph (cyclones pull inward)
        damage: 23,750-26,250 Nature on contact + 60% slow debuff
        response: |
          Pre-HoT group BEFORE retract. If a player gets clipped, focus-heal
          them through the slow debuff (12s).
          - Resto Shaman: Healing Stream Totem + Riptide on tank pre-retract
          - Holy Paladin: Beacon + Holy Radiance pre-retract
          - Holy Priest: Renew + Circle of Healing pre-retract
          - Disc Priest: PW:S on most-likely-clip targets
          - Resto Druid: Wild Growth pre-retract + Rejuv-roll group
    - lurking_tempest_lightning_bolt (heroic only):
        warning: tempest spawns at platform edge; activates only if faced/targeted
        damage: 6,562-8,437 Nature damage on activator
        response: |
          Bot rule: tempest should NOT be activated. If somehow active,
          reactive heal the activator + the DPS will kill the tempest.

  cooldown_use:
    standard: reactive HoT on tank (Riptide / Beacon / Renew / PW:S / Lifebloom)
    aoe_window: align cooldown to cyclone retract IF group HP collapses (rare)
    no_unique_cd_save: |
      Save Mana Tide / Innervate / Hymn / Spirit Link for Asaad fight.
      Ertan should not burn major mana CD.

  dispel_priority: []                       # no dispels required

  interrupt_role:
    ertan_lightning_bolt: PARTICIPATE in interrupt rotation if class has kick (Resto Shaman Wind Shear, Holy Pally Rebuke, etc.) — MEDIUM priority

  heroic_delta: |
    - Lurking Tempest add spawns (target-suppression by DPS/tank; healer reactive)
    - Cyclone retract damage higher; pre-HoT more important
    - Lightning Bolt hits harder; interrupt rotation more meaningful

  bot_anti_patterns:
    - DO NOT stand outside the cyclone ring (Storm's Edge DoT punishes max-range default)
    - DO NOT face Lurking Tempest spawn locations on heroic (camera-rule; bot AI uses target-suppression instead)
    - DO NOT burn Mana Tide / equivalent on Ertan (save for Asaad)
    - DO NOT chase strays during cyclone retract (stay near boss; pre-HoT instead)
```

---

## Boss 2 — Altairus

```yaml
boss_healer:
  boss: altairus
  difficulty_grade: medium
  mana_intensity: moderate

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50    # Chilling Breath splash on tank possible
    tier_3_chilling_breath_target: |
      Cone-targeted player takes 25-35k Frost (normal) or 47-52k (heroic).
      Treat as soft-emergency target for the cast duration.

  predictable_spike_events:
    - chilling_breath:
        warning: ~2s cast bar; aimed at random target
        damage: 25,500-34,500 Frost (normal) / 47,500-52,500 (heroic)
        response: |
          Pre-shield / pre-HoT the cone target. Heavy-heal post-cast if HP
          drops. Heroic: mandatory burst-heal on the target.
    - twisting_winds_contact (heroic only):
        damage: 11,700-12,300 Nature + KNOCKUP (fall-off-platform risk)
        response: |
          Reactive heal anyone clipped. CRITICAL: a knocked-up player who
          falls = teleport to dungeon entrance (= dead-to-fight). Bot can't
          prevent the fall; reactive heal post-recovery only.
    - wind_direction_shift:
        warning: ~30s cadence; environmental cue
        damage: none directly — but Downwind debuff drops haste/movement
        response: |
          REPOSITION — healer also wants Upwind buff. Move with the group;
          the bot cannot dispel Downwind (positional, not dispellable).

  cooldown_use:
    standard: standard reactive flow
    save_for_asaad: hold Mana Tide / Innervate / Hymn for Asaad
    chilling_breath_burst: |
      On heroic, if Chilling Breath target HP drops below 40%, pop instant
      burst-heal cooldown (Tranquility-equivalent NOT needed — single-target
      burst sufficient).

  dispel_priority:
    DISPEL_BLACKLIST:
      - { spell_id: UNVERIFIED, name: Downwind of Altairus, reason: positional debuff — not dispellable }
    notes: |
      Downwind of Altairus is a debuff but NOT dispellable. Bot dispel logic
      must skip it. Wasted GCD if attempted.

  interrupt_role:
    chilling_breath: SKIP — UNVERIFIED interruptibility; standard strat does not kick

  heroic_delta: |
    - Twisting Winds patrolling tornadoes (NEW MECHANIC) — chip damage + fall risk
    - Chilling Breath damage roughly doubles; cone-target burst heal mandatory
    - Wind shift cadence same; reposition window tighter

  bot_anti_patterns:
    - DO NOT auto-dispel Downwind of Altairus (UNVERIFIED spell ID — flag for DBC)
    - DO NOT stand still on heroic (Twisting Winds will clip you)
    - DO NOT stand at platform edges (knockup = fall = teleport to entrance)
    - DO NOT chase the cone target — let them re-position; you stay upwind
```

---

## Boss 3 — Asaad, Caliph of Zephyrs

```yaml
boss_healer:
  boss: asaad_caliph_of_zephyrs
  difficulty_grade: hard (heroic) / medium (normal)
  mana_intensity: high (heroic) / moderate (normal)
  notes: |
    THE healer skill check of the dungeon. Multiple concurrent damage sources
    (Skyfall Stars, Chain Lightning bounces, Supremacy of the Storm ticks)
    plus dispel pressure on heroic (Static Cling). Mana pacing is critical.

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 45    # standard
    tier_3_static_cling_target: |
      Heroic only. Rooted player is at risk of dying outside Grounding Field
      triangle. Treat as emergency dispel target FIRST, heal target SECOND.

  predictable_spike_events:
    - skyfall_star_arcane_barrage:
        warning: star spawns; visible add
        damage: 5-7k normal / 10-12k heroic Arcane on random ranged
        response: |
          Reactive heal random ranged; ranged DPS will kill star fast.
          Continuous low-grade pressure throughout fight.
    - chain_lightning_bounces:
        warning: ~2s cast; random initial target
        damage: 25-29k Nature initial; 50% per bounce
        response: |
          IF group properly spread (15y+) → only initial target hits. Heal
          target reactively. IF group failed to spread → multi-bounce damage,
          AoE heal cooldown.
    - static_cling_root (heroic only):
        warning: ~2s cast; Magic-school telegraph
        damage: 18s root (NOT damage — but locks player out of Grounding Field positioning)
        response: |
          PRIMARY = MAGIC DISPEL the rooted player. CRITICAL TIMING: dispel
          BEFORE Unstable Grounding Field channel completes. Otherwise the
          rooted player is stuck outside the triangle and dies to Supremacy
          of the Storm.

          Fallback if no Magic dispel: class-specific immobilize-break
          (Hand of Freedom, Cleanse, Lichborne, Druid Powershift, AMS).

          Per Glow's healer guide: Asaad casts Static Cling TWICE between
          Supremacy events — only the SECOND application strictly needs
          dispelling on tightest mana comps. BOT DEFAULT: dispel ALL casts
          (safer; mana cost negligible on a 5-man).
    - supremacy_of_the_storm:
        warning: Unstable Grounding Field channel (~5s) precedes
        damage: 19,475-21,525 Nature per 0.5s tick for 6s — INSIDE triangle, suppressed; OUTSIDE, one-shots
        response: |
          ALL bots (healer included) move into triangle on Grounding Field
          spawn. Inside, sustain group through the channel:
          - Resto Shaman: Healing Stream + Healing Rain + Chain Heal
          - Holy Paladin: Holy Radiance + Light of Dawn + Beacon transfer
          - Holy Priest: Circle of Healing + PoH spam
          - Disc Priest: Power Word: Barrier ground-place INSIDE triangle + PoM spread
          - Resto Druid: Wild Growth + Tranquility on heroic if HP collapses

  cooldown_use:
    mana_tide_totem: deploy ~50% boss HP (Asaad is the Mana Tide window for this dungeon)
    innervate: self-cast at ~30% mana (Resto Druid)
    hymn_of_hope: ~40% mana (Holy Priest)
    spirit_link_totem: deploy ON Grounding Field spawn — equalize group HP entering Supremacy of the Storm channel
    tranquility: emergency raid heal during Supremacy of the Storm if multiple bots near death (heroic only)
    power_word_barrier: GROUND-PLACE inside triangle (Disc Priest) — covers entire group during Supremacy
    avenging_wrath: rotational on Holy Pally; align with Supremacy if possible

  dispel_priority:
    - { spell_id: 87618, name: Static Cling, dispel_type: magic, priority: HIGH, heroic_only: true }
    notes: |
      Static Cling is the ONLY routine dispel in this dungeon. Magic dispel
      role required if heroic + healer-class has Magic dispel:
      - Resto Shaman: Cleanse Spirit (no Magic) — CANNOT dispel Static Cling
      - Holy Paladin: Cleanse (Magic) — CAN dispel
      - Holy Priest: Mass Dispel / Dispel Magic — CAN dispel
      - Disc Priest: Dispel Magic / Mass Dispel — CAN dispel
      - Resto Druid: Remove Corruption (no Magic) — CANNOT dispel Static Cling
    fallback_no_magic_dispel: |
      Group depends on class-specific immobilize-breaks:
      - Hand of Freedom (Holy/Ret Pally) on rooted player
      - Lichborne self-cast (DK)
      - Druid powershift self
      - Anti-Magic Shell (DK; absorbs root cast on every other application)
      - PvP trinket (any class)

  interrupt_role:
    none: no kick-mandatory casts

  heroic_delta: |
    - Static Cling dispel pressure (NEW MECHANIC) — Magic dispel role mandatory
    - Skyfall Star damage higher; spawn cadence faster
    - Chain Lightning damage higher
    - Supremacy of the Storm tick damage may scale (UNVERIFIED)

  bot_anti_patterns:
    - DO NOT skip Static Cling dispel "to optimize mana" — bot defaults to dispel-every-cast
    - DO NOT attempt JUMP-avoidance for Static Cling (sub-second timing unreliable for AI)
    - DO NOT stand outside the Grounding Field triangle during Supremacy of the Storm (ONE-SHOT)
    - DO NOT cluster with another player (Chain Lightning bounce hazard)
    - DO NOT walk to a Skyfall Star to heal a target standing near it (let ranged kill the star first)
    - DO NOT burn Tranquility / equivalent before Supremacy of the Storm channel — it's the predictable raid-heal window
```

---

## Bot AI hooks

```yaml
ai_hooks:
  ertan:
    - encounter_state_active → maintain_position_inside_cyclone_ring (override default healer-max-range)
    - encounter_detect_cyclone_retract_telegraph → pre_hot_group
    - encounter_detect_lightning_bolt_cast → kick_rotation_participate (if class has kick)
    - encounter_state_lurking_tempest_active(heroic) → reactive_heal_activator

  altairus:
    - encounter_detect_chilling_breath_cast → pre_shield_target + reactive_heal_post
    - encounter_detect_wind_direction_shift → reposition_upwind
    - encounter_state_self_downwind_aura → reposition_immediate
    - encounter_detect_twisting_winds_contact(heroic) → reactive_heal_clipped_player
    - DISPEL_BLACKLIST: Downwind of Altairus

  asaad:
    - encounter_detect_skyfall_star_spawn → continue_dps_rotation; reactive heal random ranged
    - encounter_detect_chain_lightning_cast → group_aoe_heal_pre_emptive
    - encounter_detect_static_cling_applied(target)(heroic) → MAGIC_DISPEL_PRIORITY_HIGH
    - encounter_detect_unstable_grounding_field_spawn → move_into_triangle + spirit_link_totem_drop
    - encounter_detect_supremacy_of_the_storm_channel → sustain_group_in_triangle (CD-stack)
    - encounter_state_supremacy_of_the_storm_ending → mana_tide_if_below_50pct

dispel_blacklist:
  - { spell_id: UNVERIFIED, name: Downwind of Altairus, scope: altairus_fight, force: true }

dispel_whitelist_priority:
  - { spell_id: 87618, name: Static Cling, scope: asaad_fight_heroic, dispel_type: magic, priority: HIGH }
  - { spell_id: UNVERIFIED, name: Lethargic Poison, scope: empyrean_assassin_trash, dispel_type: poison, priority: HIGH }

mana_cd_save_priority:
  ertan:    [no_save]
  altairus: [no_save — minor reactive only]
  asaad:    [Mana Tide / Innervate / Hymn of Hope / Spirit Link Totem / Power Word: Barrier all available; deploy on Grounding Field spawn or ~50% boss HP]

position_overrides:
  ertan:    INSIDE_CYCLONE_RING (override max-range default)
  altairus: UPWIND_OF_BOSS (continuous re-position on direction shifts)
  asaad:    SPREAD_15Y_BASELINE → STACK_IN_TRIANGLE_ON_GROUNDING_FIELD
```
