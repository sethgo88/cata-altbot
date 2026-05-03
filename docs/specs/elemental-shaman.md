# Elemental Shaman — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide.

**Numerical data** verified against Wowhead's Cata 4.3.4 archive and Warcraft Wiki. Items tagged `UNVERIFIED` need DBC verification.

**Strategy content** reconciled across 10 community sources. See `docs/research/elemental-shaman-guide-survey.md`.

**SPEC-CHOICE RATIONALE.** Elemental is the only ranged-caster Shaman spec; Enhancement is melee, Restoration is healer (already documented). **A-tier** in Dragon Soul. This is the **sixth and final ranged-DPS spec** in the project.

---

## Identity

```yaml
spec: elemental_shaman
class: shaman
role: RANGED_DPS
resource_model: mana + lightning_shield_charges (max 9 with Fulmination, max 3 baseline)
defining_mechanic: Lightning Shield charge stack consumed via Fulmination Earth Shock at 7+
defining_buffs:
  - Lightning_Shield            # 3-9 charges; consumed by Fulmination
  - Wrath_of_Air_Totem          # +5% raid spell haste aura (40y)
  - Totemic_Wrath               # +10% raid spell power aura (replaces Flametongue Totem in Cata)
  - Bloodlust / Heroism         # 30% raid haste 40s, 5min CD
  - Elemental_Mastery           # instant cast next + 20% haste 30s, 2min CD talented
  - Spiritwalkers_Grace         # cast while moving 15s, 2min CD
defining_procs:
  - Lava_Surge                  # Flame Shock tick proc → next Lava Burst instant + resets CD
  - Elemental_Focus             # Clearcasting after crit (talent)
mastery: Elemental_Overload     # +16% base + 2%/pt chance for LB/LvB/CL to fire 75% overload
key_cooldowns:
  - Fire_Elemental_Totem        # 5 min, 2min Fire pet (DPS)
  - Earth_Elemental_Totem       # 10 min, 2min Earth pet (tank)
  - Bloodlust_Heroism           # 5 min, raid haste
  - Elemental_Mastery           # 2 min, instant + haste
  - Thunderstorm                # 45s, AoE knockback + 8% mana
  - Spiritwalkers_Grace         # 2 min, cast while moving
ranged: 30
notes: |
  Elemental Shaman in 4.3.4 plays as a charge-managed ranged caster. Heartbeat:
  apply Flame Shock (Fire DoT) → Lava Burst on CD (auto-crit on FS target) →
  Lightning Bolt filler (generates 1 LS charge per cast) → Earth Shock at 7+
  charges (Fulmination — expends all charges above 1). Lava Surge proc resets
  Lava Burst CD AND makes it instant.

  Strong AoE/cleave: Earthquake on 4+ stacked targets, Magma Totem AoE pulse,
  Chain Lightning (3-target jump, generates LS charge per hit), Fire Elemental
  Totem 5min CD with major sustained Fire damage.

  Pre-Lava-Burst leveling (under L34): Lightning Bolt + Flame Shock + Earth
  Shock rotation. Below L40 (no Fulmination), ES is direct damage on shock CD.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  CURRENT_DPS_TARGET:
  ENEMY_CASTING_INTERRUPTIBLE:
  AOE_CLUSTER:                  # ≥4 enemies within 8y of target (Earthquake/Magma)
  TOTEM_GROUND_TARGET:
  CC_MARKER_HEX:
```

---

## Spell Catalog

```yaml
spells:
  # ─────── Single-target core ───────

  LIGHTNING_BOLT:
    id: 403
    mana_pct: 6
    cast_time_ms: 2500              # haste-affected; 2000ms with Glyph + talents
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: nature
    target_type: enemy
    notes: |
      Primary single-target filler. Generates 1 Lightning Shield charge per
      cast (with Rolling Thunder talent). Mastery overload at 75% damage.

  LAVA_BURST:
    id: 51505
    mana_pct: 10
    cast_time_ms: 2000              # 1.5s with talents
    cooldown_ms: 8000
    on_gcd: true
    range: 30
    school: fire
    target_type: enemy
    requires_talent: true
    notes: |
      Auto-crit when target has Flame Shock. Top damage per cast. Lava Surge
      proc resets CD + makes instant.

  FLAME_SHOCK:
    id: 8050
    mana_pct: 17
    cast_time_ms: 0
    cooldown_ms: 6000               # shared shock CD
    on_gcd: true
    range: 25
    school: fire
    target_type: enemy
    duration_ms: 18000              # 24s+ with talents/glyph
    tick_ms: 3000
    notes: |
      Fire DoT. **Lava Burst depends on this** — maintenance mandatory.
      Lava Surge procs on tick.

  EARTH_SHOCK:
    id: 8042
    mana_pct: 18
    cast_time_ms: 0
    cooldown_ms: 6000               # shared shock CD
    on_gcd: true
    range: 25
    school: nature
    target_type: enemy
    notes: |
      Primary purpose: consume Lightning Shield charges via Fulmination
      at 7+ charges. Without Fulmination, ES is just a low-priority shock.

  FROST_SHOCK:
    id: 8056
    mana_pct: 18
    cast_time_ms: 0
    cooldown_ms: 6000               # shared shock CD
    on_gcd: true
    range: 25
    school: frost
    target_type: enemy
    duration_ms: 8000               # 50% slow
    notes: kiting/manual only.

  # ─────── AoE ───────

  CHAIN_LIGHTNING:
    id: 421
    mana_pct: 26
    cast_time_ms: 2000
    cooldown_ms: 3000               # talented from 6s
    on_gcd: true
    range: 30
    school: nature
    target_type: enemy
    jumps: 3                        # 3-target chain
    notes: AoE. Generates 1 LS charge per hit. Cast at 3+ targets.

  THUNDERSTORM:
    id: 51490
    mana_pct: 8
    cast_time_ms: 0
    cooldown_ms: 45000
    on_gcd: false
    range: 0
    target_type: pbaoe_self
    radius: 10
    requires_spec: elemental
    notes: |
      AoE Nature damage + 20y knockback + restores 8% caster mana.
      Manual knockback awareness (raid positioning).

  EARTHQUAKE:
    id: 61882
    mana_pct: 60
    cast_time_ms: 2500
    cooldown_ms: 10000
    on_gcd: true
    range: 35
    target_type: ground_targeted
    duration_ms: 10000
    requires_talent: true
    notes: ground AoE channel; 10s pulses. Cast on CD at 4+ stacked.

  # ─────── Totems (Fire) ───────

  SEARING_TOTEM:
    id: 3599
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 60000              # 1 min
    notes: single-target damage shots. Default Fire totem.

  MAGMA_TOTEM:
    id: 8190
    mana_pct: 18
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 21000
    radius: 8
    notes: AoE Fire pulse. 3+ enemies within 8y of totem footprint.

  FIRE_ELEMENTAL_TOTEM:
    id: 2894
    mana_pct: 23
    cast_time_ms: 0
    cooldown_ms: 300000              # 5 min
    on_gcd: false
    range: 0
    target_type: ground_at_self
    duration_ms: 120000              # 2 min (longer with Glyph)
    notes: Fire pet for sustained DPS + AoE. Cast on CD; pre-pull.

  EARTH_ELEMENTAL_TOTEM:
    id: 2062
    mana_pct: 24
    cast_time_ms: 0
    cooldown_ms: 600000              # 10 min
    on_gcd: false
    range: 0
    target_type: ground_at_self
    duration_ms: 120000
    notes: Earth pet (tank). Manual / emergency CD.

  # ─────── Totems (Air) ───────

  WRATH_OF_AIR_TOTEM:
    id: 3738
    mana_pct: 11
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 300000              # 5 min
    notes: +5% raid spell haste aura (40y).

  # ─────── Totems (Earth) ───────

  STONESKIN_TOTEM:
    id: 8071
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 300000
    notes: armor + dodge raid buff.

  STRENGTH_OF_EARTH_TOTEM:
    id: 8075
    mana_pct: 8
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 300000
    notes: Str/Agi raid buff. Default Earth totem when raid lacks coverage.

  TREMOR_TOTEM:
    id: 8143
    mana_pct: 2
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 6000
    notes: pulses to break Fear/Charm/Sleep. Manual swap.

  # ─────── Totems (Water) ───────

  MANA_SPRING_TOTEM:
    id: 5675
    mana_pct: 4
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 300000
    notes: passive party Spirit-mana regen aura. Default Water totem.

  CLEANSING_TOTEM:
    id: 8170
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 60000
    notes: periodic dispel. Manual swap when needed.

  # ─────── Cooldowns ───────

  ELEMENTAL_MASTERY:
    id: 16166
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 120000              # 2min talented from 3min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 30000
    requires_talent: true
    notes: instant cast on next spell + 20% haste 30s. Sync with Lava Burst.

  BLOODLUST:
    id: 2825
    mana_pct: 26
    cast_time_ms: 0
    cooldown_ms: 300000              # 5 min
    on_gcd: false
    range: 0
    target_type: self_aura_raid
    duration_ms: 40000
    notes: 30% raid haste. Horde version. Applies Sated/Exhaustion 10min.

  HEROISM:
    id: 32182
    mana_pct: 26
    cast_time_ms: 0
    cooldown_ms: 300000              # 5 min
    on_gcd: false
    range: 0
    target_type: self_aura_raid
    duration_ms: 40000
    notes: Alliance version of Bloodlust.

  SPIRITWALKERS_GRACE:
    id: 79206
    mana_pct: 12
    cast_time_ms: 0
    cooldown_ms: 120000              # 2 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 15000
    notes: cast while moving. Movement-DPS CD.

  REINCARNATION:
    id: 20608
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 1800000              # 30 min
    on_gcd: false
    range: 0
    target_type: self
    notes: self-rez. Out of bot scope.

  # ─────── Utility & defensives ───────

  WIND_SHEAR:
    id: 57994
    mana_pct: 8
    cast_time_ms: 0
    cooldown_ms: 6000
    on_gcd: false
    range: 25
    target_type: enemy_casting
    notes: interrupt + 2s school lockout. Auto-fire on enemy casts.

  HEX:
    id: 51514
    mana_pct: 12
    cast_time_ms: 1500
    cooldown_ms: 45000
    on_gcd: true
    range: 30
    target_type: enemy
    duration_ms: 60000               # PvE
    notes: CC. Manual command-only.

  PURGE:
    id: 370
    mana_pct: 14
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: enemy
    notes: removes 1 magic buff. Manual / situational.

  GHOST_WOLF:
    id: 2645
    mana_pct: 6
    cast_time_ms: 2000
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    notes: +30% run speed. Out-of-combat travel.

  HEALING_WAVE:
    id: 331
    mana_pct: 9
    cast_time_ms: 3000
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    notes: pre-Healing-Surge fallback. Solo emergency.

  HEALING_SURGE:
    id: 8004
    mana_pct: 27
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    notes: emergency self-heal.

  ASTRAL_RECALL:
    id: 556
    mana_pct: 5
    cast_time_ms: 10000
    cooldown_ms: 900000               # 15 min
    on_gcd: true
    range: 0
    target_type: self
    notes: teleport to bind. Out-of-combat utility.
```

---

## Maintenance pass

```yaml
maintenance:
  - name: Lightning Shield (always)
    when:
      - aura.missing(SELF, LIGHTNING_SHIELD) OR aura.expires_in_ms(SELF, LIGHTNING_SHIELD) < 10000
    cast: LIGHTNING_SHIELD on SELF

  - name: Wrath of Air Totem (Air slot)
    when:
      - air_totem.empty OR air_totem.expires_in_ms < 10000
    cast: WRATH_OF_AIR_TOTEM

  - name: Strength of Earth Totem (Earth slot)
    when:
      - earth_totem.empty OR earth_totem.expires_in_ms < 10000
      - NOT runtime_flag.tremor_totem_required
    cast: STRENGTH_OF_EARTH_TOTEM

  - name: Mana Spring Totem (Water slot)
    when:
      - water_totem.empty OR water_totem.expires_in_ms < 10000
      - NOT runtime_flag.cleansing_totem_required
    cast: MANA_SPRING_TOTEM

  - name: Searing Totem (Fire slot, single-target)
    when:
      - fire_totem.empty OR fire_totem.expires_in_ms < 5000
      - aoe_mode == false
      - in_combat OR boss_pull_imminent
    cast: SEARING_TOTEM
    notes: Magma Totem replaces Searing in AoE mode.

  - name: Fire Elemental Totem (pre-pull)
    when:
      - boss_pull_imminent_in_15s
      - spell.off_cooldown(FIRE_ELEMENTAL_TOTEM)
    cast: FIRE_ELEMENTAL_TOTEM
    notes: 2min duration covers Bloodlust + opener.
```

---

## Rotation — Priority List (Single-Target)

```yaml
rotation:
  # ───────── Tier 1: Lava Surge proc — instant Lava Burst ─────────
  - tier: 1
    name: Lava Burst — Lava Surge instant proc
    when:
      - aura.active(SELF, LAVA_SURGE_PROC)
      - target_in_range(30)
    cast: LAVA_BURST on CURRENT_DPS_TARGET

  # ───────── Tier 2: Lava Burst on CD with Flame Shock active ─────────
  - tier: 2
    name: Lava Burst — auto-crit on FS target
    when:
      - spell.off_cooldown(LAVA_BURST)
      - aura.active(CURRENT_DPS_TARGET, FLAME_SHOCK)
      - target_in_range(30)
    cast: LAVA_BURST on CURRENT_DPS_TARGET

  # ───────── Tier 3: Earth Shock at 9 charges (cap-protect) ─────────
  - tier: 3
    name: Earth Shock — Fulmination at 9 charges (mandatory)
    when:
      - has_talent(FULMINATION)
      - lightning_shield_charges == 9
      - spell.off_cooldown(EARTH_SHOCK)
      - target_in_range(25)
    cast: EARTH_SHOCK on CURRENT_DPS_TARGET
    notes: Cap-protect; never let charges overflow.

  # ───────── Tier 4: Flame Shock maintenance ─────────
  - tier: 4
    name: Flame Shock — apply/refresh
    when:
      - target_in_range(25)
      - any of:
          - aura.missing(CURRENT_DPS_TARGET, FLAME_SHOCK)
          - aura.expires_in_ms(CURRENT_DPS_TARGET, FLAME_SHOCK) < 3000
      - spell.off_cooldown(FLAME_SHOCK)            # shared shock CD
    cast: FLAME_SHOCK on CURRENT_DPS_TARGET

  # ───────── Tier 5: Earth Shock at 7+ charges (Fulmination spend) ─────────
  - tier: 5
    name: Earth Shock — Fulmination spend at 7+ charges
    when:
      - has_talent(FULMINATION)
      - lightning_shield_charges >= 7
      - spell.off_cooldown(EARTH_SHOCK)
      - aura.active(CURRENT_DPS_TARGET, FLAME_SHOCK)    # don't waste shock CD on FS-less target
      - target_in_range(25)
    cast: EARTH_SHOCK on CURRENT_DPS_TARGET

  # ───────── Tier 6: Lightning Bolt filler ─────────
  - tier: 6
    name: Lightning Bolt — primary filler
    when:
      - target_in_range(30)
      - SELF.mana_pct > 25
    cast: LIGHTNING_BOLT on CURRENT_DPS_TARGET
    notes: generates LS charges via Rolling Thunder talent.
```

---

## AoE Mode

Activated by `enemies_within_8y_of(CURRENT_DPS_TARGET) >= 4 AND target_lifetime_estimate >= 10000`. Hysteresis: exit `<3 sustained for 5s`.

```yaml
rotation_aoe:
  - tier: 1: Lava Surge proc -> Lava Burst on focus target (still single-target damage)
  - tier: 2: spell.off_cooldown(EARTHQUAKE) AND mana_pct > 50 -> EARTHQUAKE at cluster centroid
  - tier: 3: fire_totem != MAGMA_TOTEM OR magma.expires < 5s -> swap to MAGMA_TOTEM
  - tier: 4: maintain Flame Shock on focus target (FS for Lava Surge proc gen)
  - tier: 5: enemies_in_3target_chain >= 3 -> CHAIN_LIGHTNING (generates LS charges per hit)
  - tier: 6: lightning_shield_charges >= 7 -> EARTH_SHOCK Fulmination (still Tier-3-priority cap-protect)
  - tier: 7: fall back to Lightning Bolt filler
notes: Earthquake is the primary AoE damage; Chain Lightning generates LS charges quickly.
```

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: FIRE_ELEMENTAL_TOTEM
    when:
      - in_combat OR boss_pull_imminent_in_15s
      - spell.off_cooldown(FIRE_ELEMENTAL_TOTEM)
    rationale: 5min CD; major sustained DPS contribution. Cast on CD.

  - spell: BLOODLUST
    when:
      - in_combat
      - level >= 70
      - bloodlust_owner == true              # runtime flag, default true
      - spell.off_cooldown(BLOODLUST)
      - aura.missing(SELF, SATED)
      - boss_engagement OR raid_burn_phase
    rationale: 5min raid CD. Default pull-time; togglable via chat command.

  - spell: ELEMENTAL_MASTERY
    when:
      - in_combat
      - spell.off_cooldown(ELEMENTAL_MASTERY)
      - any of:
          - lava_burst_off_cd_in_3s
          - boss_engagement
    rationale: 2min CD instant + haste. Sync with Lava Burst.

  - spell: SPIRITWALKERS_GRACE
    when:
      - in_combat
      - movement_required_in_next_5s OR currently_moving
      - spell.off_cooldown(SPIRITWALKERS_GRACE)
    rationale: 2min CD movement DPS.

  - spell: THUNDERSTORM
    when:
      - in_combat
      - SELF.mana_pct < 60
      - NOT (boss_positioning_critical AND knockback_will_break_positioning)
      - spell.off_cooldown(THUNDERSTORM)
    rationale: 45s CD mana CD (8% restore) + AoE damage. Manual knockback override.

  - spell: WIND_SHEAR
    when:
      - target.casting AND cast_remaining_ms < 300
      - spell.off_cooldown(WIND_SHEAR)
      - target_in_range(25)
    rationale: 6s CD interrupt; auto on enemy cast finish.

  - spell: EARTH_ELEMENTAL_TOTEM
    when:
      - manual_command_override OR (raid_emergency AND tank_dead)
      - spell.off_cooldown(EARTH_ELEMENTAL_TOTEM)
    rationale: 10min CD tank emergency.

  - spell: HEX
    when:
      - cc_marker_assigned == HEX
      - target.creature_type IN (humanoid, beast, critter)
      - spell.off_cooldown(HEX)
    rationale: 45s CD CC. Manual command.
```

---

## Proc Reactions

```yaml
procs:
  LAVA_SURGE:
    talent_id: 77756             # UNVERIFIED — Talent.dbc
    triggered_by: Flame Shock tick
    proc_chance_pct: 10          # UNVERIFIED — exact rate per rank
    effect: next Lava Burst is instant + resets Lava Burst CD
    duration_ms: 10000           # UNVERIFIED
    ai_behavior: Tier 1 priority — consume immediately.

  ELEMENTAL_FOCUS:
    talent_passive: true
    triggered_by: spell crit
    effect: Clearcasting — next 2 spells -40% mana cost
    duration_ms: 15000
    ai_behavior: passive — implicit mana benefit.

  ROLLING_THUNDER:
    talent_passive: true
    triggered_by: Lightning Bolt cast (60% chance per rank, 2 ranks)
    effect: generates 1 Lightning Shield charge
    ai_behavior: passive — drives Fulmination charge economy.

  ELEMENTAL_OVERLOAD_MASTERY:
    mastery_passive: true
    triggered_by: Lightning Bolt / Lava Burst / Chain Lightning cast
    proc_chance_pct: 16 + (mastery_points * 2)   # baseline 16%
    effect: secondary "overload" cast at 75% damage
    ai_behavior: passive — informs reforge.

  FULMINATION:
    talent_passive: true
    triggered_by: Earth Shock cast at 7+ Lightning Shield charges
    effect: consumes all charges above 1; +damage per charge
    ai_behavior: gated in Tier 3/5 priority.
```

---

## Mana Management

```yaml
mana:
  thresholds:
    drink_floor_pct: 30
    drink_resume_pct: 95
    thunderstorm_floor_pct: 60
    conserve_mode_pct: 50
    crisis_mode_pct: 25

  conserve_behavior:
    when: SELF.mana_pct < 50
    rules:
      - skip Earthquake unless 5+ targets
      - skip Chain Lightning at 3 targets (single-target instead)
      - keep core priority (FS + LvB + Fulmination ES + LB)

  crisis_behavior:
    when: SELF.mana_pct < 25
    rules:
      - fire Thunderstorm immediately (8% restore)
      - drop AoE-mode
      - prefer LB filler over higher-mana casts
      - fire Mythical Mana Potion if off CD

  out_of_combat:
    drink_when:
      - not in_combat
      - SELF.mana_pct < drink_floor_pct
      - no enemies within 40y for 5000ms
    eat_when: out_of_combat AND SELF.hp_pct < 80

lightning_shield_charges:
  base_max: 3
  fulmination_max: 9
  generation:
    - Lightning Bolt: 1 charge per cast (Rolling Thunder 60%/100% per rank)
    - Chain Lightning: 1 charge per hit (up to 3)
  consumption:
    - Earth Shock (Fulmination): expends all charges above 1
```

---

## Consumables

```yaml
consumables:
  flask:
    item: Flask_of_the_Draconic_Mind
    item_id: 58086

  food:
    primary:
      item: Severed_Sagefish_Head
      item_id: 62669

  dps_potion:
    item: Volcanic_Potion
    item_id: 58091
    use_when:
      - boss_pull_imminent_in_2s
      - OR (in_combat AND fight_elapsed_ms ~ 60000 AND fire_elemental_off_cd AND elemental_mastery_off_cd)

  mana_potion:
    item: Mythical_Mana_Potion
    item_id: 57192

  healthstone:
    item_id: 36892
```

---

## Non-encounter content

### Solo combat

```yaml
solo_offensive_rotation:
  - tier 1: SELF.hp_pct < 30 -> Healing Surge self
  - tier 2: aura.missing(target, FLAME_SHOCK) -> FLAME_SHOCK
  - tier 3: aura.active(SELF, LAVA_SURGE_PROC) -> LAVA_BURST
  - tier 4: lava_burst.off_cd AND aura.active(target, FLAME_SHOCK) -> LAVA_BURST
  - tier 5: lightning_shield_charges >= 9 -> EARTH_SHOCK Fulmination
  - tier 6: SELF.mana_pct > 25 -> LIGHTNING_BOLT
  - tier 7: SELF.mana_pct <= 25 -> wand auto-attack
notes: Pre-Lava-Burst (under L34): just LB + FS + ES rotation.
```

### Travel & out-of-combat

```yaml
travel:
  ghost_wolf:
    use_when:
      - not in_combat
      - distance_to_destination > 30 AND distance_to_destination < 80
      - aura.missing(SELF, GHOST_WOLF)
    cast: GHOST_WOLF
    notes: +30% run speed; breaks on melee.

  mount:
    use_when:
      - not in_combat
      - distance_to_destination >= 80

ooc_maintenance:
  - check: aura.missing(SELF, LIGHTNING_SHIELD) -> LIGHTNING_SHIELD
  - check: SELF.mana_pct < 80 AND no_enemies_within(40) -> drink
  - check: SELF.hp_pct < 95 AND no_enemies_within(40) -> eat
```

### Idle

```yaml
idle:
  primary_action: follow master
  secondary_actions:
    - maintain Lightning Shield
    - maintain totem set (Wrath of Air + Stoneskin/SoE + Mana Spring)
  do_not:
    - do not cast offensive spells while idle
```

---

## Leveling rotation

Pure Elemental from L10 (spec choice).

### Spec recommendation

```yaml
spec_choice:
  for_leveling_dungeon_dps_bot:
    spec: Elemental
    talent_split: 31 Elemental / 2 Enhancement / 8 Restoration
    talent_picks_elemental: Convection 3/3 + Concussion 5/5 + Elemental Focus 1/1 + Call of Flame 3/3 + Elemental Reach 2/2 + Elemental Devastation 3/3 + Reverberation 2/2 + Elemental Fury 5/5 + Improved Fire Nova 2/2 + Lava Flows 3/3 + Elemental Precision 3/3 + Elemental Oath 2/2 + Lava Surge 2/2 + Storm Earth and Fire 3/3 + Totemic Wrath 1/1 + Lightning Mastery 3/3 + Earthquake 1/1 + Rolling Thunder 2/2 + Lava Burst 1/1 + Fulmination 1/1 + Elemental Mastery 1/1
    talent_picks_enh: Acuity 3/3 (+Crit) OR Concussion 2/2
    talent_picks_resto: Tidal Focus 5/5 + Improved Shields 3/3 (Lightning Shield charge max +)
    rationale: 31/2/8 Icy Veins consensus.
```

### Spell unlock table

```yaml
spell_unlocks:
  LIGHTNING_BOLT:           {level: 1,  spell_id: 403}
  EARTH_SHOCK:              {level: 4,  spell_id: 8042}
  HEALING_WAVE:             {level: 1,  spell_id: 331}
  STONESKIN_TOTEM:          {level: 4,  spell_id: 8071}
  HEALING_SURGE:            {level: 7,  spell_id: 8004}
  LIGHTNING_SHIELD:         {level: 8,  spell_id: 324}
  FLAME_SHOCK:              {level: 10, spell_id: 8050}
  SEARING_TOTEM:            {level: 10, spell_id: 3599}
  GHOST_WOLF:               {level: 16, spell_id: 2645}
  STRENGTH_OF_EARTH_TOTEM:  {level: 16, spell_id: 8075}
  WIND_SHEAR:               {level: 16, spell_id: 57994}
  TREMOR_TOTEM:             {level: 18, spell_id: 8143}
  FROST_SHOCK:              {level: 18, spell_id: 8056}
  PURGE:                    {level: 22, spell_id: 370}
  MAGMA_TOTEM:              {level: 26, spell_id: 8190}
  MANA_SPRING_TOTEM:        {level: 26, spell_id: 5675}
  CLEANSING_TOTEM:          {level: 26, spell_id: 8170}
  CHAIN_LIGHTNING:          {level: 28, spell_id: 421}
  ASTRAL_RECALL:            {level: 30, spell_id: 556}
  REINCARNATION:            {level: 30, spell_id: 20608}
  HEX:                      {level: 40, spell_id: 51514}
  WRATH_OF_AIR_TOTEM:       {level: 42, spell_id: 3738}
  FIRE_ELEMENTAL_TOTEM:     {level: 56, spell_id: 2894}
  EARTH_ELEMENTAL_TOTEM:    {level: 56, spell_id: 2062}
  THUNDERSTORM:             {level: 60, spell_id: 51490, requires_spec: elemental}
  EARTHQUAKE:               {level: 60, spell_id: 61882, requires_talent: true}
  BLOODLUST:                {level: 70, spell_id: 2825}
  HEROISM:                  {level: 70, spell_id: 32182}
  SPIRITWALKERS_GRACE:      {level: 85, spell_id: 79206}
  # Talent-gated:
  LAVA_BURST:               {requires_talent: true, spell_id: 51505}
  ELEMENTAL_MASTERY:        {requires_talent: true, spell_id: 16166}
  FULMINATION:              {requires_talent: true}
  LAVA_SURGE:               {requires_talent: true}
  ELEMENTAL_FOCUS:          {requires_talent: true}
  CONVECTION:               {requires_talent: true}
  CONCUSSION:               {requires_talent: true}
  CALL_OF_FLAME:            {requires_talent: true}
  ELEMENTAL_PRECISION:      {requires_talent: true}
  ELEMENTAL_OATH:           {requires_talent: true}
  STORM_EARTH_AND_FIRE:     {requires_talent: true}
  TOTEMIC_WRATH:            {requires_talent: true}
  ROLLING_THUNDER:          {requires_talent: true}
  REVERBERATION:            {requires_talent: true}
  ELEMENTAL_REACH:          {requires_talent: true}
  IMPROVED_FIRE_NOVA:       {requires_talent: true}
  # Mastery / passives:
  MASTERY_ELEMENTAL_OVERLOAD: {passive_mastery: true, base_pct: 16, per_point_pct: 2}
  LIGHTNING_SHIELD_CHARGES:   {passive_resource: true, base_max: 3, fulmination_max: 9}
```

### Leveling brackets

```yaml
bracket_1_3:
  available: LIGHTNING_BOLT, HEALING_WAVE
  rotation: LB spam

bracket_4_7:
  available: + EARTH_SHOCK, + STONESKIN_TOTEM
  rotation: LB filler + ES on shock-CD

bracket_8_9:
  available: + LIGHTNING_SHIELD
  maintenance: LS always-on

bracket_10_15:
  available: + FLAME_SHOCK, + SEARING_TOTEM
  rotation_change: FS becomes Tier 4 maintenance; Searing Totem as default Fire totem

bracket_16_17:
  available: + GHOST_WOLF, + STRENGTH_OF_EARTH_TOTEM, + WIND_SHEAR
  cooldown_addition: Wind Shear interrupt

bracket_18_27:
  available: + TREMOR_TOTEM, + FROST_SHOCK, + PURGE
  notes: situational utility.

bracket_28_29:
  available: + CHAIN_LIGHTNING, + MAGMA_TOTEM, + MANA_SPRING_TOTEM, + CLEANSING_TOTEM
  rotation_addition: AoE-mode entry on 3+ enemies (CL filler).

bracket_30_33:
  available: + HEX (40), + REINCARNATION
  notes: pre-Lava-Burst — rotation is LB + FS + ES + CL + totems.

bracket_34_55:
  available: + LAVA_BURST (talent ~L34)
  rotation_change: Lava Burst becomes Tier 1-2 priority.

bracket_56_59:
  available: + FIRE_ELEMENTAL_TOTEM, + EARTH_ELEMENTAL_TOTEM
  cooldown_addition: Fire Elemental on CD.

bracket_60_69:
  available: + THUNDERSTORM, + EARTHQUAKE (talent)
  cooldown_addition: Thunderstorm mana CD; Earthquake AoE.

bracket_70_84:
  available: + BLOODLUST/HEROISM
  cooldown_addition: Bloodlust on pull (default).

bracket_85:
  available: + SPIRITWALKERS_GRACE + all talents fully allocated
  rotation: full encounter rotation per §"Rotation"
```

### Mana floors by era

```yaml
mana_floors_by_level:
  1_60:    { conserve: 0,  crisis: 0  }
  60_70:   { conserve: 55, crisis: 30 }
  70_80:   { conserve: 45, crisis: 25 }
  80_85:   { conserve: 50, crisis: 25 }
```

---

## UNVERIFIED items

| # | Item | Spec value | Verification |
|---|---|---|---|
| 1 | Lava Burst cast time | 1.5s with 5/5 talent | Spell.dbc 51505 |
| 2 | Flame Shock duration with Glyph | 24s (vs 18s base) | Glyph + Spell.dbc 8050 |
| 3 | Earth Shock Fulmination damage formula | per-charge multiplier | Spell.dbc 8042 + Talent.dbc |
| 4 | Lightning Shield charge generation | LB 100% via Rolling Thunder 2/2 | Talent.dbc |
| 5 | Lava Surge proc rate per FS tick | 10% (assumed) | Talent.dbc |
| 6 | Elemental Mastery talented CD | 2min (vs 3min base) | Talent.dbc |
| 7 | Earthquake damage formula | SP coefficient + 10s pulses | Spell.dbc 61882 |
| 8 | Mastery Elemental Overload | 16% + 2%/pt | Mastery DBC |
| 9 | Haste soft caps for FS 7-tick + LB | TBD | Spell.dbc + haste model |
| 10 | T13 set bonuses | TBD | Item set DBC |
| 11 | Glyph of Flame Shock duration extension | TBD | Glyph.dbc |
| 12 | Glyph of Lightning Bolt damage / range | TBD | Glyph.dbc |
| 13 | Glyph of Lava Burst SP coefficient | TBD | Glyph.dbc |
| 14 | Glyph of Fire Elemental Totem duration | TBD | Glyph.dbc |
| 15 | Wrath of Air Totem +5% spell haste aura | yes | Spell.dbc 3738 |
| 16 | Spiritwalker's Grace duration / CD | 15s / 2min | Spell.dbc 79206 |
| 17 | Volcanic Potion +1200 Int duration | 25s | Item DBC |
| 18 | Bloodlust 30% haste 40s | yes | Spell.dbc 2825 |
| 19 | Hex PvE 1min duration | yes | Spell.dbc 51514 |
| 20 | Wind Shear 2s school lockout | yes | Spell.dbc 57994 |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kElementalShaman_Spells[]` |
| Rotation tiers | `static const RotationTier kElementalShaman_Rotation[]` |
| AoE rotation | `static const RotationTier kElementalShaman_RotationAoE[]` |
| Cooldown Profile | `static const CooldownTrigger kElementalShaman_Cooldowns[]` |
| Mana Management | `ManaState` enum + threshold helpers |
| Lightning Shield charge tracking | `uint8 lightning_shield_charges` (0-9) + Fulmination talent gate |
| Lava Surge proc tracking | aura listener |
| Totem slot tracking | per-element (Fire/Air/Earth/Water) totem state + remaining ms |
| Bloodlust owner flag | `bool bloodlust_owner` runtime flag |
| Leveling rotation | `LevelBracketRotation kElementalShaman_Leveling[]` |

**Key new helper APIs:**
- `LightningShieldCharges(bot)` — read 0-9.
- `LavaSurgeActive(bot)` — proc state.
- `TotemSlotState(bot, element)` — read per-slot (Fire/Air/Earth/Water).
- `IsAoEMode(bot)` — `enemies_within_8y_of_target >= 4 AND target_lifetime >= 10s`.
- `BloodlustOwner(bot)` — runtime flag for whether bot fires BL on pull.

Cross-reference to `docs/roles/ranged-dps.md` and prior ranged-DPS specs.
