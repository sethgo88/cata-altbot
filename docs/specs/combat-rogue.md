# Combat Rogue — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide.

**Strategy content** reconciled across 10 community sources. See `docs/research/combat-rogue-guide-survey.md`.

**SPEC-CHOICE RATIONALE.** Combat picked over Assassination/Subtlety because: Kick (10s CD off-GCD) is the **shortest melee interrupt in 4.3.4**, Blade Flurry cleave is uniquely strong in 5-mans, weapon-flexible (no daggers required), AR + KS burst window is mechanically simple. Assassination is S-tier ST but requires daggers + complex Vendetta/Envenom timing. Subtlety has rear-positional Backstab dependency. This is the **first melee-DPS spec** in the project.

---

## Identity

```yaml
spec: combat_rogue
class: rogue
role: MELEE_DPS
resource_model: energy + combo_points (target-bound, max 5 per target)
defining_mechanic: Bandit's Guile Insight stacking + Restless Blades CD reduction on CP spend
defining_buffs:
  - Slice_and_Dice              # +40% melee AS finisher buff (always-on)
  - Adrenaline_Rush             # +Energy regen + AS for 15s
  - Insight (Shallow/Moderate/Deep) # Bandit's Guile damage stack
  - Tricks_of_the_Trade_buff    # +15% damage on target ally
defining_procs:
  - Bandits_Guile               # SS/RvS stack to Deep Insight (+30% damage)
  - Main_Gauche                 # OH strike on auto-attack (mastery)
  - Restless_Blades             # CP spend → AR/KS/Redirect/Sprint CD reduction
mastery: Main_Gauche
key_cooldowns:
  - Adrenaline_Rush             # 3min CD, off-GCD
  - Killing_Spree               # 2min CD, off-GCD, 5 teleport hits
  - Vanish                      # 2min CD, restealth
  - Cloak_of_Shadows            # 1min CD, magic immunity
  - Evasion                     # 2min CD, 50% dodge
  - Combat_Readiness            # 2min CD, stacking parry/dodge
  - Sprint                      # 1min CD, movement
  - Tricks_of_the_Trade         # 30s CD, threat redirect + damage buff
melee_range: 5
notes: |
  Combat Rogue in 4.3.4 plays as an Energy/CP melee with Bandit's Guile
  damage stacking. Heartbeat: maintain Slice and Dice (+40% AS) → Sinister
  Strike for CP + Bandit's Guile stacks → Revealing Strike (+35% finisher)
  → Eviscerate at 5 CP. Kick on enemy casts (10s CD off-GCD).

  Blade Flurry toggle: ON for 2+ targets (35% OH cleave, -20% Energy regen);
  OFF for single-target. Killing Spree + Adrenaline Rush paired on first
  pull; subsequent uses on independent CDs (Restless Blades desyncs them).
  Tricks of the Trade on tank pre-pull, then on CD.

  Pre-Slice-and-Dice (under L14): SS → Eviscerate at 4-5 CP.
  Pre-AR/KS/BF (under L36/40/81): basic SS + RvS + Eviscerate + SnD priority.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  CURRENT_DPS_TARGET:
  ENEMY_CASTING_INTERRUPTIBLE:    # Kick range: 5y melee
  AOE_CLUSTER:                    # ≥2 enemies within 10y (Blade Flurry trigger)
  STEALTH_OPENER_TARGET:          # caster vs melee for Garrote/Cheap Shot pick
  TANK:                           # Tricks of the Trade primary
  TOP_DPS:                        # Tricks of the Trade follow-up
```

---

## Spell Catalog

```yaml
spells:
  # ─────── Builders ───────

  SINISTER_STRIKE:
    id: 1752
    energy_cost: 40
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    combo_points_generated: 1
    notes: Default builder. Stacks Bandit's Guile.

  REVEALING_STRIKE:
    id: 84617
    energy_cost: 40
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    combo_points_generated: 1
    duration_ms: 15000              # debuff
    requires_spec: combat
    notes: |
      +35% damage of next finisher on target. Refresh ≤3s remaining.
      Stacks Bandit's Guile. Requires talent (level 50).

  BACKSTAB:
    id: 53
    energy_cost: 60
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_rear
    combo_points_generated: 1
    notes: Subtlety/dagger-only; SKIPPED in Combat rotation.

  AMBUSH:
    id: 8676
    energy_cost: 60
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_stealth_only
    requires_stealth: true
    notes: Stealth opener; weak in Combat (no dagger). SKIPPED.

  GARROTE:
    id: 703
    energy_cost: 50
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_stealth_only
    duration_ms: 18000              # bleed
    requires_stealth: true
    notes: Stealth opener vs casters (silence with talent).

  CHEAP_SHOT:
    id: 1833
    energy_cost: 60
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_stealth_only
    duration_ms: 4000               # 4s stun
    requires_stealth: true
    notes: Stealth opener vs melee trash.

  FAN_OF_KNIVES:
    id: 51723
    energy_cost: 35
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: pbaoe
    radius: 8
    notes: AoE primary builder.

  # ─────── Finishers ───────

  EVISCERATE:
    id: 2098
    energy_cost: 35
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy
    consumes: combo_points
    notes: ST finisher. Damage scales with CP.

  SLICE_AND_DICE:
    id: 5171
    energy_cost: 25
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    consumes: combo_points
    notes: |
      Self-buff: +40% melee AS. Duration scales with CP (6/12/18/24/30s).
      Maintain 100% uptime; refresh ≤3s remaining.

  RUPTURE:
    id: 1943
    energy_cost: 25
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy
    consumes: combo_points
    notes: SKIPPED in Combat priority.

  CRIMSON_TEMPEST:
    id: 121411
    energy_cost: 35
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: pbaoe
    consumes: combo_points
    notes: AoE bleed finisher; 5+ targets.

  KIDNEY_SHOT:
    id: 408
    energy_cost: 25
    cast_time_ms: 0
    cooldown_ms: 20000
    on_gcd: true
    range: 5
    target_type: enemy_melee
    consumes: combo_points
    notes: Stun finisher 2/3/4/5/6s by CP. Manual / interrupt fallback.

  RECUPERATE:
    id: 73651
    energy_cost: 30
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    consumes: combo_points
    notes: Self-HoT finisher. Solo/leveling only.

  EXPOSE_ARMOR:
    id: 8647
    energy_cost: 25
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy
    duration_ms: 30000              # -12% armor
    consumes: combo_points
    notes: Composition-conditional (skip if Sunder/FF in group).

  # ─────── Cooldowns ───────

  ADRENALINE_RUSH:
    id: 13750
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 180000              # 3 min talented (effective lower with Restless Blades)
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 15000
    requires_spec: combat
    notes: +Energy regen + +20% melee AS.

  KILLING_SPREE:
    id: 51690
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 120000              # 2 min (lower with Restless Blades)
    on_gcd: false
    range: 10                        # teleport range
    target_type: enemy
    duration_ms: 2500                # ~2.5s of teleport hits
    requires_spec: combat
    notes: 5 melee attacks teleporting between targets.

  BLADE_FLURRY:
    id: 13877
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 0                   # toggle
    on_gcd: false
    range: 0
    target_type: self_toggle
    requires_spec: combat
    notes: |
      Toggle ON: 35% OH weapon damage to 2nd nearby target. -20% Energy
      regen while active. ON 2+ targets, OFF 1 target.

  # ─────── Defensives ───────

  SPRINT:
    id: 2983
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 60000               # 1 min (lower with Restless Blades)
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 8000
    notes: +70% movement.

  VANISH:
    id: 1856
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 120000              # 2 min (talented to 90s with Elusiveness)
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 3000                # restealth window
    notes: Drops combat + restealth.

  CLOAK_OF_SHADOWS:
    id: 31224
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 5000
    notes: Magic immunity + dispel.

  EVASION:
    id: 5277
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 120000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 15000
    notes: 50% dodge.

  COMBAT_READINESS:
    id: 74001
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 120000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 10000
    notes: Stacking parry/dodge — up to ~50% damage reduction over duration.

  FEINT:
    id: 1966
    energy_cost: 20
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 6000
    notes: -50% AoE damage taken + 50% threat reduction.

  TRICKS_OF_THE_TRADE:
    id: 57934
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: false
    range: 100
    target_type: ally
    duration_ms: 6000               # damage redirect window
    threat_redirect_duration_ms: 30000
    notes: +15% damage to target + threat-redirect for 6s next damage.

  # ─────── Utility ───────

  KICK:
    id: 1766
    energy_cost: 15
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: false
    range: 5
    target_type: enemy_casting
    duration_ms: 5000               # school lockout
    notes: Shortest melee interrupt in 4.3.4.

  STEALTH:
    id: 1784
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 1800000             # toggle until cancelled
    notes: OoC permanent toggle.

  SAP:
    id: 6770
    energy_cost: 35
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_stealth_only
    duration_ms: 60000               # 1min CC PvE
    requires_stealth: true
    notes: Stealth-only CC. Manual.

  BLIND:
    id: 2094
    energy_cost: 15
    cast_time_ms: 0
    cooldown_ms: 120000
    on_gcd: true
    range: 15
    target_type: enemy
    duration_ms: 10000
    notes: 10s CC. Manual.

  GOUGE:
    id: 1776
    energy_cost: 45
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: true
    range: 5
    target_type: enemy_front
    duration_ms: 4000
    notes: Front incap. Manual.

  REDIRECT:
    id: 73981
    energy_cost: 10
    cast_time_ms: 0
    cooldown_ms: 60000               # talented down with Restless Blades
    on_gcd: false
    range: 0
    target_type: self
    notes: Transfers CPs to new target.

  SHIV:
    id: 5938
    energy_cost: 20
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    notes: Off-hand instant strike + applies poison.

  PICK_LOCK:
    id: 1804
    energy_cost: 0
    cast_time_ms: 4500
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: lock
    notes: Out-of-combat utility.

  PICK_POCKET:
    id: 921
    energy_cost: 0
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_humanoid
    notes: Out-of-combat utility.

  DISTRACT:
    id: 1725
    energy_cost: 30
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: true
    range: 30
    target_type: ground_targeted
    notes: Manual; pulls attention.
```

---

## Maintenance pass

```yaml
maintenance:
  - name: Pre-pull Stealth
    when:
      - NOT in_combat
      - boss_pull_imminent_in_30s OR exploring_dungeon
      - aura.missing(SELF, STEALTH)
    cast: STEALTH

  - name: Slice and Dice maintenance
    when:
      - in_combat
      - any of:
          - aura.missing(SELF, SLICE_AND_DICE)
          - aura.expires_in_ms(SELF, SLICE_AND_DICE) < 3000
      - combo_points >= 1
    cast: SLICE_AND_DICE on SELF
    notes: Always-on; refresh ≤3s remaining.

  - name: Tricks of the Trade on tank pre-pull
    when:
      - boss_pull_imminent_in_6s
      - target_resolves(TANK)
      - spell.off_cooldown(TRICKS_OF_THE_TRADE)
    cast: TRICKS_OF_THE_TRADE on TANK

  - name: Tricks of the Trade on CD (post-pull)
    when:
      - in_combat
      - any of:
          - target_resolves(TOP_DPS) AND tank.threat_safe
          - target_resolves(TANK)
      - spell.off_cooldown(TRICKS_OF_THE_TRADE)
    cast: TRICKS_OF_THE_TRADE on resolved_target

  - name: Poison maintenance
    when:
      - aura.missing(SELF, INSTANT_POISON_MH) OR aura.missing(SELF, DEADLY_POISON_OH)
      - NOT in_combat
    cast: APPLY_POISON (Instant MH + Deadly OH)
```

---

## Rotation — Priority List (Single-Target)

```yaml
rotation:
  # ───────── Tier 1: Slice and Dice maintenance ─────────
  - tier: 1
    name: Slice and Dice — refresh ≤3s
    when:
      - any of:
          - aura.missing(SELF, SLICE_AND_DICE)
          - aura.expires_in_ms(SELF, SLICE_AND_DICE) < 3000
      - combo_points >= 1
    cast: SLICE_AND_DICE on SELF

  # ───────── Tier 2: Revealing Strike maintenance ─────────
  - tier: 2
    name: Revealing Strike — refresh ≤3s before finisher
    when:
      - has_talent(REVEALING_STRIKE)
      - any of:
          - aura.missing(CURRENT_DPS_TARGET, REVEALING_STRIKE)
          - aura.expires_in_ms(CURRENT_DPS_TARGET, REVEALING_STRIKE) < 3000
      - combo_points < 5                # don't overcap
      - energy >= 40
    cast: REVEALING_STRIKE on CURRENT_DPS_TARGET

  # ───────── Tier 3: Eviscerate at 5 CP ─────────
  - tier: 3
    name: Eviscerate — finisher at 5 CP
    when:
      - combo_points == 5
      - aura.active(SELF, SLICE_AND_DICE)
      - aura.active(CURRENT_DPS_TARGET, REVEALING_STRIKE) OR NOT has_talent(REVEALING_STRIKE)
      - energy >= 35
    cast: EVISCERATE on CURRENT_DPS_TARGET

  # ───────── Tier 3b: Eviscerate at 4 CP if SnD/RvS expiring this GCD ─────────
  - tier: 3b
    name: Eviscerate — 4 CP edge case
    when:
      - combo_points == 4
      - any of:
          - aura.expires_in_ms(SELF, SLICE_AND_DICE) < 1500
          - aura.expires_in_ms(CURRENT_DPS_TARGET, REVEALING_STRIKE) < 1500
      - energy >= 35
    cast: EVISCERATE on CURRENT_DPS_TARGET

  # ───────── Tier 4: Sinister Strike builder ─────────
  - tier: 4
    name: Sinister Strike — CP builder
    when:
      - combo_points < 5
      - energy >= 40
    cast: SINISTER_STRIKE on CURRENT_DPS_TARGET
    notes: Stacks Bandit's Guile + generates 1 CP.
```

---

## AoE Mode

Activated by `enemies_within_10y_of_self >= 2`. Hysteresis: exit `<2 sustained for 5s`.

```yaml
rotation_aoe:
  - tier: 1: blade_flurry_off AND aoe_mode_active -> toggle BLADE_FLURRY ON
  - tier: 2: blade_flurry_on AND NOT aoe_mode_active -> toggle BLADE_FLURRY OFF
  - tier: 3: SnD maintenance (always)
  - tier: 4: enemies >= 4 AND energy >= 35 -> FAN_OF_KNIVES (AoE builder)
  - tier: 5: enemies >= 5 AND combo_points == 5 -> CRIMSON_TEMPEST (AoE bleed finisher)
  - tier: 6: enemies <= 3 -> Sinister Strike (Blade Flurry handles 2nd target)
notes: BF stays ON during Killing Spree (cleave > ST clarity).
```

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: ADRENALINE_RUSH
    when:
      - in_combat
      - spell.off_cooldown(ADRENALINE_RUSH)
      - any of:
          - boss_engagement OR fight_just_started
          - cooldown_window_aligned (Bloodlust + KS available)
    rationale: Pair with KS on first pull.

  - spell: KILLING_SPREE
    when:
      - in_combat
      - spell.off_cooldown(KILLING_SPREE)
      - enemies_within_10y >= 1
      - any of:
          - aura.active(SELF, ADRENALINE_RUSH)        # opener pairing
          - first_pair_already_used                   # subsequent CD-independent
    rationale: 2min CD; subsequent uses on CD without waiting for AR.

  - spell: TRICKS_OF_THE_TRADE
    when:
      - in_combat
      - spell.off_cooldown(TRICKS_OF_THE_TRADE)
      - target_resolves(TANK) OR target_resolves(TOP_DPS)
    rationale: 30s CD; on tank pre-pull, then alternate.

  - spell: KICK
    when:
      - target.casting AND target.cast_remaining_ms > 300
      - target_in_range(5)
      - spell.off_cooldown(KICK)
      - NOT (other_interrupt_class_off_cd_in_2s AND interrupt_rotation_position != self)
    rationale: 10s CD; auto-interrupt with rotation discipline.

  - spell: BLADE_FLURRY (toggle)
    when:
      - any of:
          - aoe_mode_active AND blade_flurry_off
          - NOT aoe_mode_active AND blade_flurry_on
    rationale: Toggle on AoE-mode entry/exit.

  - spell: VANISH
    when:
      - any of:
          - manual_command
          - wipe_imminent
          - cloak_failed_AND_high_threat
      - spell.off_cooldown(VANISH)
    rationale: Manual / wipe-recovery only.

  - spell: CLOAK_OF_SHADOWS
    when:
      - SELF.taking_magic_damage_imminent
      - spell.off_cooldown(CLOAK_OF_SHADOWS)
    rationale: Defensive vs magic.

  - spell: EVASION
    when:
      - SELF.taking_physical_damage_spike
      - spell.off_cooldown(EVASION)
    rationale: Defensive vs physical.

  - spell: COMBAT_READINESS
    when:
      - SELF.physical_heavy_fight
      - spell.off_cooldown(COMBAT_READINESS)
    rationale: Stacking dodge/parry buff.

  - spell: FEINT
    when:
      - SELF.taking_aoe_damage
      - energy >= 60
      - aura.active(SELF, SLICE_AND_DICE) AND aura.expires_in_ms(SELF, SLICE_AND_DICE) > 6000
    rationale: AoE survival.

  - spell: SPRINT
    when:
      - movement_required AND distance_to_target > 10
      - spell.off_cooldown(SPRINT)
    rationale: Mobility.
```

---

## Proc Reactions

```yaml
procs:
  BANDITS_GUILE:
    talent_passive: true
    triggered_by: Sinister Strike OR Revealing Strike cast
    effect: stacks to Shallow → Moderate → Deep Insight (+10/+20/+30% damage)
    duration_ms: 15000              # buff window per insight level
    ai_behavior: |
      Passive — track stack depth. Soft priority: aim finishers during
      Deep Insight. Bot doesn't gate on it (rotation continues regardless).

  RESTLESS_BLADES:
    talent_passive: true
    triggered_by: CP-spending finisher
    effect: -2s/CP on AR/KS/Redirect/Sprint cooldowns
    ai_behavior: Mechanical baseline; CDs naturally reduce via finisher density.

  MAIN_GAUCHE_MASTERY:
    mastery_passive: true
    triggered_by: melee auto-attack
    effect: chance for OH weapon Strike (Combat-specific)
    ai_behavior: Passive — informs reforge.

  COMBAT_POTENCY:
    talent_passive: true
    triggered_by: OH attack
    effect: 20% chance to gain 15 Energy
    ai_behavior: Passive — implicit Energy regen.

  VITALITY:
    talent_passive: true
    effect: +25% AP from Stamina/Agility
    ai_behavior: Passive.
```

---

## Energy / Combo Point Management

```yaml
energy:
  max: 100
  base_regen_per_sec: 10
  scales_with_haste: true
  adrenaline_rush_multiplier: ~2.0   # +Energy regen during AR

  thresholds:
    sinister_strike_floor: 40
    revealing_strike_floor: 40
    eviscerate_floor: 35
    slice_and_dice_floor: 25
    avoid_cap_threshold: 95           # never let Energy cap

  in_combat_management:
    - energy < 40 -> wait for regen (no cast)
    - energy 40-94 -> rotation priority
    - energy >= 95 -> any cast mandatory (cap-protect)

  no_pooling_default: true            # only pool for AR press window

combo_points:
  max: 5
  target_bound: true                  # CPs are on the target, not on the rogue
  redirect_handles_swap: true

  rules:
    - SnD priority at 1+ CP if not active
    - RvS refresh priority at 1-4 CP if expiring
    - Eviscerate at 5 CP (or 4 CP edge case)
    - Never overcap (prefer finisher over builder at 5)
```

---

## Consumables

```yaml
consumables:
  flask:
    item: Flask_of_the_Winds
    item_id: 58087                  # +300 Agility

  food:
    primary:
      item: Skewered_Eel
      item_id: 62290                # +90 Agility

  dps_potion:
    item: Potion_of_the_Tolvir
    item_id: 58145
    use_when:
      - boss_pull_imminent_in_2s    # pre-pot
      - OR (in_combat AND fight_elapsed_ms ~ 60000 AND adrenaline_rush_off_cd AND killing_spree_off_cd)

  healthstone:
    item_id: 36892

  poisons:
    main_hand: Instant_Poison
    off_hand: Deadly_Poison
    alternates:
      caster_boss_or_caster_trash: Mind_numbing_Poison_MH
      kiting_required: Crippling_Poison_MH
      magic_dispel_target: Wound_Poison_MH
```

---

## Non-encounter content

### Solo combat

```yaml
solo_offensive_rotation:
  - tier 1: SELF.hp_pct < 30 -> EVASION if off_cd; else CLOAK if magic damage
  - tier 2: aura.missing(SELF, SLICE_AND_DICE) AND combo_points >= 1 -> SnD
  - tier 3: target.casting AND off_cd(KICK) -> KICK
  - tier 4: combo_points == 5 -> EVISCERATE
  - tier 5: energy >= 40 -> SINISTER_STRIKE
  - tier 6: energy < 40 -> wait (auto-attack only)
notes: Pre-Stealth pull: STEALTH + GARROTE/CHEAP_SHOT for soloing elites.
```

### Travel & out-of-combat

```yaml
travel:
  sprint:
    use_when:
      - not in_combat
      - distance_to_destination > 30 AND distance_to_destination < 60
      - spell.off_cooldown(SPRINT)
    cast: SPRINT

  mount:
    use_when:
      - not in_combat
      - distance_to_destination >= 80

ooc_maintenance:
  - check: aura.missing(SELF, INSTANT_POISON_MH) -> apply MH poison
  - check: aura.missing(SELF, DEADLY_POISON_OH) -> apply OH poison
  - check: SELF.hp_pct < 95 AND no_enemies_within(40) -> eat
  - check: aura.missing(SELF, STEALTH) AND boss_pull_imminent_in_30s -> STEALTH
```

### Idle

```yaml
idle:
  primary_action: follow master
  secondary_actions:
    - maintain Stealth pre-pull
    - maintain poisons
  do_not:
    - do not engage neutral mobs unprompted
```

### Disengage

```yaml
disengage:
  vanish:
    use_when:
      - SELF.threat_pct > 95 OR SELF.hp_pct < 20
      - spell.off_cooldown(VANISH)
    cast: VANISH

  cloak_of_shadows:
    use_when:
      - SELF.has_dispellable_magic_debuff OR magic_damage_imminent
      - spell.off_cooldown(CLOAK_OF_SHADOWS)
    cast: CLOAK_OF_SHADOWS

  sprint:
    use_when:
      - SELF.hp_pct < 50 AND need_to_kite
    cast: SPRINT
```

---

## Leveling rotation

Pure Combat from L10 (spec choice).

### Spec recommendation

```yaml
spec_choice:
  for_leveling_dungeon_dps_bot:
    spec: Combat
    talent_split: 31 Combat / 5 Subtlety / 5 Assassination
    talent_picks_combat: Improved Sinister Strike 2/2 + Precision 3/3 + Improved Slice and Dice 2/2 + Improved Kick 2/2 + Combat Potency 3/3 + Lightning Reflexes 3/3 + Armored to the Teeth 3/3 + Restless Blades 3/3 + Bandit's Guile 3/3 + Savage Combat 2/2 + Vitality 3/3 + Aggression 3/3 + Improved Eviscerate (capstone-adjacent)
    talent_picks_sub: Nightstalker 3/3 + Elusiveness 2/2
    talent_picks_assn: Improved Recuperate 2/2 + Lethality 3/3
    rationale: 31/5/5 Icy Veins consensus.
```

### Spell unlock table

```yaml
spell_unlocks:
  STEALTH:                 {level: 1,  spell_id: 1784}
  SINISTER_STRIKE:         {level: 1,  spell_id: 1752}
  EVISCERATE:              {level: 4,  spell_id: 2098}
  GOUGE:                   {level: 6,  spell_id: 1776}
  SPRINT:                  {level: 7,  spell_id: 2983}
  BACKSTAB:                {level: 8,  spell_id: 53}
  PICK_POCKET:             {level: 10, spell_id: 921}
  KICK:                    {level: 12, spell_id: 1766}
  SAP:                     {level: 14, spell_id: 6770}
  SLICE_AND_DICE:          {level: 14, spell_id: 5171}
  RECUPERATE:              {level: 14, spell_id: 73651}
  AMBUSH:                  {level: 16, spell_id: 8676}
  VANISH:                  {level: 18, spell_id: 1856}
  CHEAP_SHOT:              {level: 18, spell_id: 1833}
  EVASION:                 {level: 20, spell_id: 5277}
  RUPTURE:                 {level: 20, spell_id: 1943}
  POISONS:                 {level: 20, spell_id: 2842}
  GARROTE:                 {level: 22, spell_id: 703}
  DISTRACT:                {level: 22, spell_id: 1725}
  TRICKS_OF_THE_TRADE:     {level: 24, spell_id: 57934}
  PICK_LOCK:               {level: 24, spell_id: 1804}
  BLIND:                   {level: 26, spell_id: 2094}
  KIDNEY_SHOT:             {level: 30, spell_id: 408}
  EXPOSE_ARMOR:            {level: 32, spell_id: 8647}
  FEINT:                   {level: 32, spell_id: 1966}
  SHIV:                    {level: 36, spell_id: 5938}
  ADRENALINE_RUSH:         {level: 36, spell_id: 13750, requires_spec: combat}
  FAN_OF_KNIVES:           {level: 36, spell_id: 51723}
  BLADE_FLURRY:            {level: 40, spell_id: 13877, requires_spec: combat}
  REDIRECT:                {level: 50, spell_id: 73981}
  REVEALING_STRIKE:        {level: 50, spell_id: 84617, requires_spec: combat}
  CLOAK_OF_SHADOWS:        {level: 56, spell_id: 31224}
  CRIMSON_TEMPEST:         {level: 64, spell_id: 121411}
  COMBAT_READINESS:        {level: 78, spell_id: 74001}
  KILLING_SPREE:           {level: 81, spell_id: 51690, requires_spec: combat}
  # Talent-gated:
  IMPROVED_SINISTER_STRIKE: {requires_talent: true}
  PRECISION:               {requires_talent: true}
  IMPROVED_SLICE_AND_DICE: {requires_talent: true}
  IMPROVED_KICK:           {requires_talent: true}
  COMBAT_POTENCY:          {requires_talent: true}
  LIGHTNING_REFLEXES:      {requires_talent: true}
  ARMORED_TO_THE_TEETH:    {requires_talent: true}
  RESTLESS_BLADES:         {requires_talent: true}
  BANDITS_GUILE:           {requires_talent: true}
  SAVAGE_COMBAT:           {requires_talent: true}
  VITALITY:                {requires_talent: true}
  AGGRESSION:              {requires_talent: true}
  NIGHTSTALKER:            {requires_talent: true}
  ELUSIVENESS:             {requires_talent: true}
  IMPROVED_RECUPERATE:     {requires_talent: true}
  LETHALITY:               {requires_talent: true}
  # Mastery / passives:
  MASTERY_MAIN_GAUCHE:     {passive_mastery: true, base_pct: 16, per_point_pct: 2}
```

### Leveling brackets

```yaml
bracket_1_3:
  available: STEALTH, SINISTER_STRIKE
  rotation: SS spam; pre-pull Stealth

bracket_4_6:
  available: + EVISCERATE
  rotation: SS to 5 CP -> EVISCERATE

bracket_7_13:
  available: + SPRINT (7), + KICK (12)
  cooldown_addition: Kick on enemy casts

bracket_14_19:
  available: + SnD (14), + RECUPERATE (14), + VANISH (18), + CHEAP_SHOT (18)
  rotation_change: SnD becomes Tier 1 maintenance

bracket_20_31:
  available: + EVASION (20), + RUPTURE (20), + POISONS (20), + GARROTE (22), + ToT (24), + KIDNEY_SHOT (30)
  maintenance: Apply poisons; ToT on tank

bracket_32_35:
  available: + EXPOSE_ARMOR (32), + FEINT (32)
  notes: Composition-conditional EA.

bracket_36_39:
  available: + AR (36), + FAN_OF_KNIVES (36)
  cooldown_addition: AR on CD; AoE-mode at 4+ targets

bracket_40_49:
  available: + BLADE_FLURRY (40)
  rotation_addition: BF ON 2+ targets

bracket_50_55:
  available: + REDIRECT (50), + REVEALING_STRIKE (50)
  rotation_change: RvS becomes Tier 2 maintenance

bracket_56_63:
  available: + CLOAK_OF_SHADOWS (56)
  cooldown_addition: Cloak vs magic damage

bracket_64_77:
  available: + CRIMSON_TEMPEST (64)
  rotation_addition: AoE finisher at 5+ targets

bracket_78_80:
  available: + COMBAT_READINESS (78)
  cooldown_addition: CR vs physical damage

bracket_81_85:
  available: + KILLING_SPREE (81)
  rotation: full encounter rotation per §"Rotation"
```

**Rogue is energy-only** — no mana, no rage. Energy regen scales with haste.

---

## UNVERIFIED items

| # | Item | Spec value | Verification |
|---|---|---|---|
| 1 | SS Energy reductions | Improved SS values | Talent.dbc |
| 2 | RvS finisher buff | +35% | Spell.dbc 84617 |
| 3 | SnD AS bonus | +40% | Spell.dbc 5171 |
| 4 | Eviscerate damage formula per CP | TBD | Spell.dbc 2098 |
| 5 | KS damage per hit + teleport range | 10y / 5 hits | Spell.dbc 51690 |
| 6 | AR Energy regen multiplier | +100% (vs +200%) | Spell.dbc 13750 |
| 7 | BF OH 35% scaling | yes | Spell.dbc 13877 |
| 8 | Bandit's Guile Insight values | 10/20/30% | Talent.dbc |
| 9 | Restless Blades 2s/CP | yes | Talent.dbc |
| 10 | Mastery Main Gauche formula | 16% + 2%/pt | Mastery DBC |
| 11 | Kick 10s CD off-GCD + 5s lockout | yes | Spell.dbc 1766 |
| 12 | Vanish CD vs Elusiveness | 90s talented | Talent.dbc |
| 13 | Cloak magic-immunity coverage | yes | Spell.dbc 31224 |
| 14 | Combat Readiness stack mechanics | 50% damage reduction | Spell.dbc 74001 |
| 15 | ToT +15% damage + threat redirect | yes | Spell.dbc 57934 |
| 16 | Poison proc rates per weapon speed | TBD | Spell.dbc + Item.dbc |
| 17 | Glyph values (SS, RvS, AR, KS, BF, Feint, Eviscerate) | TBD | Glyph.dbc |
| 18 | Tolvir Potion +1200 Agility duration | 25s | Item DBC |
| 19 | T13 2P/4P set bonuses (Combat) | TBD | Item set DBC |
| 20 | Eviscerate per-CP coefficient | TBD | Spell.dbc 2098 |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kCombatRogue_Spells[]` |
| Rotation tiers | `static const RotationTier kCombatRogue_Rotation[]` |
| AoE rotation | `static const RotationTier kCombatRogue_RotationAoE[]` |
| Cooldown Profile | `static const CooldownTrigger kCombatRogue_Cooldowns[]` |
| Energy management | `EnergyState` enum + threshold helpers |
| Combo Point tracking | `uint8 combo_points` (0-5, target-bound) |
| Bandit's Guile depth | `BanditsGuileInsight` enum (None/Shallow/Moderate/Deep) |
| Blade Flurry toggle | `bool blade_flurry_active` |
| Stealth state | `bool stealth_active` |
| Leveling rotation | `LevelBracketRotation kCombatRogue_Leveling[]` |

**Key new helper APIs:**
- `Energy(bot)` — read 0-100.
- `ComboPoints(bot, target)` — read 0-5 on target.
- `BanditsGuileLevel(bot)` — read None/Shallow/Moderate/Deep.
- `IsBladeFlurryActive(bot)`.
- `IsStealthActive(bot)`.
- `IsAoEMode(bot)` — `enemies_within_10y_self >= 2`.
- `KickOffCdAndInRange(target)`.

Cross-reference to `docs/roles/melee-dps.md` (rear-arc / gap-closer / interrupt-as-primary patterns).
