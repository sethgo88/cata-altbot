# Fury Warrior — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide.

**Strategy content** reconciled across 10 community sources. See `docs/research/fury-warrior-guide-survey.md`.

**SPEC-CHOICE RATIONALE.** Fury picked over Arms because: S-tier in DS 4.3 (5/6 sources rank Fury > Arms once T13 4P online), Bloodthirst-driven priority cleaner to encode than Arms' Mortal Strike + Overpower-proc weave, Titan's Grip dual-2H (loot parity with Arms), Inner Rage solves rage cap. Protection out of scope (tank role doc later). This is the **second melee-DPS spec** in the project.

---

## Identity

```yaml
spec: fury_warrior
class: warrior
role: MELEE_DPS
resource_model: rage (max 100, damage-based regen, Fury ~2x Arms multiplier)
defining_mechanic: Bloodthirst on CD + Bloodsurge/Raging Blow procs + Inner Rage rage-cap protection + Colossus Smash armor-ignore window
defining_buffs:
  - Battle_Shout                # +AP raid buff
  - Commanding_Shout            # +max-HP raid buff (alt)
  - Berserker_Stance            # DPS stance (no damage-taken penalty in 4.3.4)
  - Death_Wish                  # 3min CD, +20% damage 30s
  - Recklessness                # 5min CD, +50% crit 12s, Berserker-only
  - Enrage                      # +10% physical damage; from Berserker Rage / Bloodrage / Enrage talent crit
  - Inner_Rage                  # 15s buff: HS/Cleave off-limit at +50% rage cost
defining_procs:
  - Bloodsurge                  # BT hits proc free instant Slam (3 stacks Cata)
  - Raging_Blow                 # off-CD only when Enrage active
  - Mastery: Unshackled Fury    # +% damage while Enraged
mastery: Unshackled_Fury
key_cooldowns:
  - Death_Wish                  # 3min CD, talented +20% damage
  - Recklessness                # 5min CD, +50% crit, Berserker-only
  - Colossus_Smash              # 20s CD, armor-ignore debuff
  - Heroic_Leap                 # 45s CD, 8-40y AoE-on-land
  - Inner_Rage                  # 1min CD rage-cap protection
  - Spell_Reflection            # 25s CD, requires shield
  - Rallying_Cry                # 3min CD, raid +20% max-HP 10s
  - Charge                      # 12s CD gap-closer
  - Intervene                   # 30s CD ally redirect
melee_range: 5
notes: |
  Fury Warrior in 4.3.4 plays as a Berserker-Stance dual-2H rage dump.
  Heartbeat: maintain Battle Shout + Inner Rage rage-cap protection;
  Bloodthirst on CD (3s); Raging Blow on CD when Enraged; Whirlwind on
  CD (ST+AoE); Slam on Bloodsurge proc (highest priority); Heroic Strike
  rage dump at threshold. Colossus Smash on CD aligns first pull with
  Death Wish + Recklessness + pre-pot.

  Pummel is Berserker-only — bot stays in Berserker permanently. No
  stance-dance required.

  Pre-Bloodthirst (under L10): HS + Charge + Rend.
  Pre-Raging Blow (under L81): basic BT + WW + Slam (Bloodsurge) + HS + Execute.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  CURRENT_DPS_TARGET:
  ENEMY_CASTING_INTERRUPTIBLE:    # Pummel range: 5y melee
  AOE_CLUSTER:                    # ≥2 enemies within 8y of self (Whirlwind)
  CLEAVE_RAGE_DUMP_TARGET:        # off-GCD 3-target Cleave
  ALLY_PROTECTION_TARGET:         # Intervene
  HEROIC_LEAP_GROUND:             # 8-40y movement target
```

---

## Spell Catalog

```yaml
spells:
  # ─────── Builders ───────

  BLOODTHIRST:
    id: 23881
    rage_cost: 30                     # 20 with Cruelty talent
    cast_time_ms: 0
    cooldown_ms: 3000
    on_gcd: true
    range: 5
    target_type: enemy_melee
    requires_spec: fury
    notes: Primary builder. Heals self for 1% max HP.

  RAGING_BLOW:
    id: 85288
    rage_cost: 10
    cast_time_ms: 0
    cooldown_ms: 8000
    on_gcd: true
    range: 5
    target_type: enemy_melee
    requires_spec: fury
    requires_buff: ENRAGE
    notes: MH+OH dual-strike. Highest single-cast damage.

  WHIRLWIND:
    id: 1680
    rage_cost: 25
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: true
    range: 0
    target_type: pbaoe
    radius: 8
    requires_spec: fury
    notes: MH+OH all enemies in 8y. Cast on CD ST+AoE.

  COLOSSUS_SMASH:
    id: 86346
    rage_cost: 20
    cast_time_ms: 0
    cooldown_ms: 20000
    on_gcd: true
    range: 5
    target_type: enemy_melee
    duration_ms: 6000                 # debuff window
    notes: Applies armor-ignore debuff. ~70% PvE armor ignore.

  EXECUTE:
    id: 5308
    rage_cost: 30                     # +up to 30 extra rage consumed
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    requires_target_hp_pct: 20
    notes: Spammable in execute phase. Excess rage → damage scaling.

  HEROIC_STRIKE:
    id: 78
    rage_cost: 30                     # reduced by Improved HS
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: false                     # off-GCD
    range: 5
    target_type: enemy_melee
    notes: |
      On-next-swing rage dump. Off-GCD. Queue when rage ≥ 60.
      Inner Rage active: ≥ 30 rage threshold.

  CLEAVE:
    id: 845
    rage_cost: 20                     # off-GCD on-next-swing
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: false
    range: 5
    target_type: cleave_3_targets
    notes: AoE rage dump (3 targets max).

  SLAM:
    id: 1464
    rage_cost: 15                     # but Fury Bloodsurge: 0 rage + instant
    cast_time_ms: 1500                # Bloodsurge: instant
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    notes: |
      Fury never hardcasts Slam. Bloodsurge proc only — 3 stacks Cata.
      Highest priority on proc to prevent overcap.

  THUNDER_CLAP:
    id: 6343
    rage_cost: 20
    cast_time_ms: 0
    cooldown_ms: 6000
    on_gcd: true
    range: 0
    target_type: pbaoe
    radius: 8
    notes: AoE damage + slow + (with talent) -10% physical damage debuff.

  REND:
    id: 772
    rage_cost: 10
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    duration_ms: 18000
    notes: Bleed DoT. Skip in Fury (Arms talent buffs).

  HAMSTRING:
    id: 1715
    rage_cost: 10
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    duration_ms: 15000
    notes: 50% snare. Manual / kiting.

  VICTORY_RUSH:
    id: 34428
    rage_cost: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    duration_ms: 20000                # post-kill window
    notes: 20% max-HP self-heal post-kill. Manual.

  # ─────── Cooldowns ───────

  DEATH_WISH:
    id: 12292
    rage_cost: 10
    cast_time_ms: 0
    cooldown_ms: 180000                # 3 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 30000
    requires_talent: true
    notes: +20% damage / +damage-taken (talent-reduced). Sync first w/ Recklessness.

  RECKLESSNESS:
    id: 1719
    rage_cost: 0
    cast_time_ms: 0
    cooldown_ms: 300000                # 5 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 12000
    requires_stance: berserker
    notes: +50% crit. Berserker-only.

  BERSERKER_RAGE:
    id: 18499
    rage_cost: 0
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 6000
    notes: |
      Removes fear/sap/incap + applies Enrage. Press to maintain Enrage
      uptime for Raging Blow.

  INNER_RAGE:
    id: 1134
    rage_cost: 0
    cast_time_ms: 0
    cooldown_ms: 60000                 # 1 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 15000
    requires_spec: fury
    requires_level: 83
    notes: HS/Cleave off-limit at +50% rage cost. Burst rage-dump.

  HEROIC_LEAP:
    id: 6544
    rage_cost: 0
    cast_time_ms: 0
    cooldown_ms: 45000
    on_gcd: true
    range: 40                          # 8-40y target ground
    target_type: ground_targeted
    requires_level: 85
    notes: AoE damage on land. Mobility / engage.

  CHARGE:
    id: 100
    rage_cost: 0                       # generates +15 rage
    cast_time_ms: 0
    cooldown_ms: 12000
    on_gcd: false
    range: 25                          # 8-25y
    target_type: enemy
    notes: Pull / re-engage. Generates 15 rage.

  INTERVENE:
    id: 3411
    rage_cost: 0
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: false
    range: 25
    target_type: ally
    duration_ms: 10000
    notes: Ally redirect + damage-redirect. Utility.

  RALLYING_CRY:
    id: 97462
    rage_cost: 0
    cast_time_ms: 0
    cooldown_ms: 180000
    on_gcd: false
    range: 0
    target_type: self_aura_raid
    duration_ms: 10000
    requires_level: 83
    notes: +20% max-HP raid 10s.

  SHATTERING_THROW:
    id: 64382
    rage_cost: 25
    cast_time_ms: 1500
    cooldown_ms: 300000
    on_gcd: true
    range: 30
    target_type: enemy
    duration_ms: 10000
    notes: Strips immunity + +20% physical debuff. Manual / niche.

  BLOODRAGE:
    id: 2687
    rage_cost: 0
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: false
    range: 0
    target_type: self
    notes: +20 rage immediately + 10/sec for 10s. Pre-pull rage gen.

  ENRAGED_REGENERATION:
    id: 55694
    rage_cost: 0
    cast_time_ms: 0
    cooldown_ms: 180000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 10000
    notes: HoT 30% max-HP over 10s. Defensive.

  # ─────── Defensives & utility ───────

  PUMMEL:
    id: 6552
    rage_cost: 10                      # 0 with Improved Pummel
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: false
    range: 5
    target_type: enemy_casting
    duration_ms: 4000                  # 4s lockout
    requires_stance: berserker
    notes: Interrupt. Berserker-only. Auto-fire.

  SPELL_REFLECTION:
    id: 23920
    rage_cost: 25
    cast_time_ms: 0
    cooldown_ms: 25000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 5000
    notes: Reflects next spell. Requires shield (Battle/Defensive).

  STANCE_BATTLE:
    id: 2457
    notes: starting stance pre-30.

  STANCE_BERSERKER:
    id: 2458
    requires_level: 30
    notes: Fury permanent stance.

  STANCE_DEFENSIVE:
    id: 71
    requires_level: 8
    notes: tank stance; reactive defensive only.

  BATTLE_SHOUT:
    id: 6673
    rage_cost: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: party_or_raid
    duration_ms: 120000                # 2 min
    notes: +AP raid buff. Default.

  COMMANDING_SHOUT:
    id: 469
    rage_cost: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: party_or_raid
    duration_ms: 120000
    notes: +max-HP raid buff. Use if no other Stam buff.

  SUNDER_ARMOR:
    id: 7386
    rage_cost: 15
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    duration_ms: 30000
    notes: Armor debuff. Skip if Warrior-tank/Druid armor source present.

  DEMORALIZING_SHOUT:
    id: 1160
    rage_cost: 10
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: pbaoe_debuff
    radius: 10
    duration_ms: 30000
    notes: -AP debuff. Skip if equivalent in group.

  TAUNT:
    id: 355
    notes: Tank-only. Skip for Fury.
```

---

## Maintenance pass

```yaml
maintenance:
  - name: Berserker Stance (Fury permanent)
    when:
      - level >= 30
      - current_stance != BERSERKER
    cast: STANCE_BERSERKER

  - name: Battle Shout maintenance
    when:
      - any of:
          - aura.missing(SELF, BATTLE_SHOUT_BUFF)
          - aura.expires_in_ms(SELF, BATTLE_SHOUT_BUFF) < 10000
      - NOT runtime_flag.use_commanding_shout
    cast: BATTLE_SHOUT

  - name: Commanding Shout (alt)
    when:
      - aura.missing(SELF, COMMANDING_SHOUT_BUFF) OR aura.expires_in_ms(SELF, COMMANDING_SHOUT_BUFF) < 10000
      - runtime_flag.use_commanding_shout
    cast: COMMANDING_SHOUT

  - name: Pre-pull Bloodrage
    when:
      - NOT in_combat
      - boss_pull_imminent_in_2s
      - spell.off_cooldown(BLOODRAGE)
    cast: BLOODRAGE

  - name: Berserker Rage for Enrage uptime
    when:
      - in_combat
      - aura.missing(SELF, ENRAGE) OR aura.expires_in_ms(SELF, ENRAGE) < 2000
      - spell.off_cooldown(BERSERKER_RAGE)
      - raging_blow_off_cd_in_2s OR raging_blow_currently_off_cd
    cast: BERSERKER_RAGE
```

---

## Rotation — Priority List (Single-Target)

```yaml
rotation:
  # ───────── Tier 1: Slam (Bloodsurge proc) ─────────
  - tier: 1
    name: Slam — Bloodsurge proc consumption
    when:
      - aura.active(SELF, BLOODSURGE_PROC)
      - target_in_range(5)
    cast: SLAM on CURRENT_DPS_TARGET
    notes: Highest priority — prevents 3-stack overcap.

  # ───────── Tier 2: Colossus Smash on CD ─────────
  - tier: 2
    name: Colossus Smash on CD
    when:
      - spell.off_cooldown(COLOSSUS_SMASH)
      - rage >= 20
      - target_in_range(5)
    cast: COLOSSUS_SMASH on CURRENT_DPS_TARGET
    notes: Apply armor-ignore window.

  # ───────── Tier 3: Bloodthirst on CD ─────────
  - tier: 3
    name: Bloodthirst on CD
    when:
      - spell.off_cooldown(BLOODTHIRST)
      - rage >= 30                       # 20 with Cruelty
      - target_in_range(5)
    cast: BLOODTHIRST on CURRENT_DPS_TARGET

  # ───────── Tier 4: Raging Blow on CD when Enraged ─────────
  - tier: 4
    name: Raging Blow — Enrage-required
    when:
      - has_talent(RAGING_BLOW)
      - spell.off_cooldown(RAGING_BLOW)
      - aura.active(SELF, ENRAGE)
      - rage >= 10
      - target_in_range(5)
    cast: RAGING_BLOW on CURRENT_DPS_TARGET

  # ───────── Tier 5: Whirlwind on CD ─────────
  - tier: 5
    name: Whirlwind — ST + AoE
    when:
      - spell.off_cooldown(WHIRLWIND)
      - rage >= 25
      - melee_in_8y >= 1
    cast: WHIRLWIND
    notes: Cast on CD even ST (OH hit + Mastery scaling).

  # ───────── Tier 6: Execute (execute phase) ─────────
  - tier: 6
    name: Execute — execute phase
    when:
      - CURRENT_DPS_TARGET.hp_pct <= 20
      - rage >= 30
      - target_in_range(5)
    cast: EXECUTE on CURRENT_DPS_TARGET
    notes: Spammable below 20% HP.

  # ───────── Tier 7: Heroic Strike rage dump (off-GCD) ─────────
  - tier: 7
    name: Heroic Strike — rage dump
    when:
      - rage >= heroic_strike_threshold
        # threshold = 60 default; 30 during Death Wish + Inner Rage
      - target_in_range(5)
      - NOT in_execute_phase                # let Execute consume rage
    cast: HEROIC_STRIKE on CURRENT_DPS_TARGET
    notes: Off-GCD; queues onto next swing. Inner Rage allows continuous use.

  # ───────── Tier 8: Auto-attack (no cast) ─────────
  - tier: 8
    name: Auto-attack — fallback
    when:
      - target_in_range(5)
    notes: Default melee swing; no GCD trigger.
```

---

## AoE Mode

Activated by `enemies_within_8y_of_self >= 2`. Hysteresis: exit `<2 sustained for 5s`.

```yaml
rotation_aoe:
  - tier: 1: Slam (Bloodsurge proc) — single-target priority preserved
  - tier: 2: Colossus Smash on CD — primary target
  - tier: 3: Bloodthirst on CD
  - tier: 4: Whirlwind on CD — primary AoE
  - tier: 5: Cleave (off-GCD) — rage dump for AoE
  - tier: 6: Thunder Clap on CD — secondary AoE damage + slow
  - tier: 7: Raging Blow when Enraged
  - tier: 8: Heroic Strike — only if rage >= 90 + Inner Rage active
```

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: DEATH_WISH
    when:
      - in_combat
      - has_talent(DEATH_WISH)
      - spell.off_cooldown(DEATH_WISH)
      - any of:
          - boss_engagement OR fight_just_started
          - cooldown_window_aligned (Recklessness ≤2s OR Bloodlust)
    rationale: 3min CD; sync first w/ Recklessness.

  - spell: RECKLESSNESS
    when:
      - in_combat
      - current_stance == BERSERKER
      - spell.off_cooldown(RECKLESSNESS)
      - any of:
          - boss_engagement
          - cooldown_window_aligned (Death Wish active)
    rationale: 5min CD; sync first w/ Death Wish.

  - spell: INNER_RAGE
    when:
      - in_combat
      - rage >= 80
      - any of:
          - aura.active(SELF, DEATH_WISH)
          - aura.active(SELF, RECKLESSNESS)
          - aura.active(COLOSSUS_SMASH_DEBUFF) AND target_in_range(5)
      - spell.off_cooldown(INNER_RAGE)
    rationale: 1min CD rage dump during burst windows.

  - spell: HEROIC_LEAP
    when:
      - target_distance >= 8 AND target_distance <= 40
      - movement_required OR new_pull
      - spell.off_cooldown(HEROIC_LEAP)
    rationale: 45s CD; mobility only (no DPS-only cast).

  - spell: CHARGE
    when:
      - target_distance >= 8 AND target_distance <= 25
      - new_pull OR re_engage
      - spell.off_cooldown(CHARGE)
    rationale: 12s CD; +15 rage opener.

  - spell: PUMMEL
    when:
      - target.casting AND target.cast_remaining_ms > 300
      - target_in_range(5)
      - spell.off_cooldown(PUMMEL)
    rationale: 10s CD interrupt; auto-fire.

  - spell: SPELL_REFLECTION
    when:
      - SELF.has_shield_equipped
      - incoming_single_target_spell_imminent
      - spell.off_cooldown(SPELL_REFLECTION)
    rationale: Reactive defensive.

  - spell: RALLYING_CRY
    when:
      - level >= 83
      - any_party_member.hp_pct < 40
      - no_other_raid_cd_active_in_last_30000ms
      - spell.off_cooldown(RALLYING_CRY)
    rationale: Group emergency CD.

  - spell: ENRAGED_REGENERATION
    when:
      - SELF.hp_pct < 40
      - spell.off_cooldown(ENRAGED_REGENERATION)
    rationale: Self-heal HoT defensive.
```

---

## Proc Reactions

```yaml
procs:
  BLOODSURGE:
    talent_passive: true
    triggered_by: Bloodthirst hit
    effect: free + instant Slam (3 stacks Cata)
    duration_ms: 10000
    ai_behavior: Tier 1 priority — consume immediately.

  ENRAGE:
    talent_passive: true
    triggered_by: Berserker Rage / Bloodrage / crit auto-attack (Enrage talent)
    duration_ms: 9000               # talent-rank
    effect: +10% physical damage; required for Raging Blow
    ai_behavior: maintain via Berserker Rage when RB ready + Enrage absent.

  RESTLESS_BLADES:
    notes: NOT a Warrior mechanic — Rogue-only. Fury has independent CDs.

  MAIN_GAUCHE:
    notes: NOT a Warrior mechanic — Rogue-only. Fury Mastery is Unshackled Fury.

  UNSHACKLED_FURY_MASTERY:
    mastery_passive: true
    triggered_by: Enrage active
    effect: +% damage while Enraged
    base_pct: 11.2                  # UNVERIFIED
    per_point_pct: 1.4
    ai_behavior: Passive — informs reforge.

  TASTE_FOR_BLOOD:
    notes: Arms-only proc.

  TITANS_GRIP:
    talent_passive: true
    effect: dual-wield 2H weapons; -10% physical penalty (talent-reducible)
    ai_behavior: Passive — talent allocation prerequisite.
```

---

## Rage Management

```yaml
rage:
  max: 100
  decay_per_sec_out_of_combat: 5
  generation:
    auto_attack: damage-based formula (Fury ~2x Arms)
    charge: +15
    bloodrage: +20 instant + 10/sec for 10s
    incoming_damage: ~2.5/1% max-HP (tank mechanic)

  thresholds:
    heroic_strike_default: 60
    heroic_strike_burst_window: 30
    inner_rage_trigger: 80
    avoid_cap: 95

  in_combat_management:
    - rage < 30 -> wait for regen / use rage-free abilities (BR, BR, etc.)
    - rage 30-59 -> standard rotation (BT, RB, WW, CS as available)
    - rage 60-79 -> Heroic Strike rage dump (off-GCD queue)
    - rage 80-94 -> press Inner Rage if available + any major CD active
    - rage >= 95 -> any cast mandatory (cap-protect)

  no_pooling: true                  # always spend; no rage banking
```

---

## Consumables

```yaml
consumables:
  flask:
    item: Flask_of_Titanic_Strength
    item_id: 58088                  # +300 Strength

  food:
    primary:
      item: Beer_Basted_Crocolisk
      item_id: 62669                # +90 Strength

  dps_potion:
    item: Golemblood_Potion
    item_id: 58146
    use_when:
      - boss_pull_imminent_in_2s    # pre-pot
      - OR (in_combat AND fight_elapsed_ms ~ 60000 AND death_wish_off_cd AND recklessness_off_cd)

  healthstone:
    item_id: 36892
```

---

## Non-encounter content

### Solo combat

```yaml
solo_offensive_rotation:
  - tier 1: SELF.hp_pct < 40 -> ENRAGED_REGENERATION if off_cd
  - tier 2: SELF.hp_pct < 25 -> RALLYING_CRY if off_cd (lvl 83+)
  - tier 3: aura.active(SELF, BLOODSURGE_PROC) -> SLAM
  - tier 4: target.hp_pct <= 20 -> EXECUTE
  - tier 5: spell.off_cooldown(COLOSSUS_SMASH) -> COLOSSUS_SMASH
  - tier 6: spell.off_cooldown(BLOODTHIRST) -> BLOODTHIRST
  - tier 7: aura.active(SELF, ENRAGE) AND off_cd(RAGING_BLOW) -> RAGING_BLOW
  - tier 8: off_cd(WHIRLWIND) AND melee_in_8y >= 1 -> WHIRLWIND
  - tier 9: rage >= 60 -> HEROIC_STRIKE off-GCD
  - tier 10: post-kill within 20s AND SELF.hp_pct < 80 -> VICTORY_RUSH
notes: Pre-Bloodthirst (under L10): HS + Charge + Rend.
```

### Travel & out-of-combat

```yaml
travel:
  charge:
    use_when:
      - in_combat (re-engage)
      - target_in_range(8-25)
    cast: CHARGE

  mount:
    use_when:
      - not in_combat
      - distance_to_destination >= 80

ooc_maintenance:
  - check: aura.missing(SELF, BATTLE_SHOUT_BUFF) -> BATTLE_SHOUT
  - check: current_stance != BERSERKER AND level >= 30 -> STANCE_BERSERKER
  - check: SELF.hp_pct < 95 AND no_enemies_within(40) -> eat
```

### Idle

```yaml
idle:
  primary_action: follow master
  secondary_actions:
    - maintain Battle Shout
    - maintain Berserker Stance
  do_not:
    - do not engage neutral mobs unprompted
```

### Disengage

```yaml
disengage:
  spell_reflection:
    use_when:
      - SELF.has_shield_equipped
      - incoming_spell_imminent
    cast: SPELL_REFLECTION

  intervene:
    use_when:
      - ally.hp_pct < 30 AND ally_in_range(25)
    cast: INTERVENE on ally

  enraged_regeneration:
    use_when:
      - SELF.hp_pct < 40
    cast: ENRAGED_REGENERATION
```

---

## Leveling rotation

Pure Fury from L10 (spec choice).

### Spec recommendation

```yaml
spec_choice:
  for_leveling_dungeon_dps_bot:
    spec: Fury
    talent_split: 31 Fury / 5 Arms / 5 Prot
    talent_picks_fury: Cruelty 3/3 + Battle Trance 3/3 + Executioner 3/3 + Bloodsurge 3/3 + Rude Interruption 2/2 + Booming Voice 2/2 + Rampage 1/1 + Flurry 3/3 + Enrage 3/3 + Death Wish 1/1 + Intensify Rage 3/3 + Meat Cleaver 3/3 + Single-Minded Fury 0/3 (alt to Titan's Grip — DEFAULT Titan's Grip 1/1)
    talent_picks_arms: Field Dressing 2/2 + War Academy 3/3
    talent_picks_prot: Cruelty (Prot tree, +crit) 3/3 + Incite 2/3
    rationale: 31/5/5 Icy Veins consensus; Titan's Grip dual-2H default.
```

### Spell unlock table

```yaml
spell_unlocks:
  CHARGE:                  {level: 1,  spell_id: 100}
  HEROIC_STRIKE:           {level: 1,  spell_id: 78}
  BATTLE_SHOUT:            {level: 1,  spell_id: 6673}
  REND:                    {level: 4,  spell_id: 772}
  VICTORY_RUSH:            {level: 5,  spell_id: 34428}
  BLOODRAGE:               {level: 6,  spell_id: 2687}
  THUNDER_CLAP:            {level: 6,  spell_id: 6343}
  DEFENSIVE_STANCE:        {level: 8,  spell_id: 71}
  HAMSTRING:               {level: 8,  spell_id: 1715}
  TAUNT:                   {level: 8,  spell_id: 355}
  BLOODTHIRST:             {level: 10, spell_id: 23881, requires_spec: fury}
  SUNDER_ARMOR:            {level: 10, spell_id: 7386}
  EXECUTE:                 {level: 12, spell_id: 5308}
  OVERPOWER:               {level: 14, spell_id: 7384}
  CLEAVE:                  {level: 20, spell_id: 845}
  BERSERKER_STANCE:        {level: 30, spell_id: 2458}
  ENRAGED_REGENERATION:    {level: 30, spell_id: 55694}
  BERSERKER_RAGE:          {level: 32, spell_id: 18499}
  WHIRLWIND:               {level: 36, spell_id: 1680, requires_spec: fury}
  INTERVENE:               {level: 36, spell_id: 3411}
  PUMMEL:                  {level: 38, spell_id: 6552}
  RECKLESSNESS:            {level: 40, spell_id: 1719, requires_stance: berserker}
  DEATH_WISH:              {level: 40, spell_id: 12292, requires_talent: true}
  SPELL_REFLECTION:        {level: 56, spell_id: 23920}
  COMMANDING_SHOUT:        {level: 70, spell_id: 469}
  SHATTERING_THROW:        {level: 70, spell_id: 64382}
  COLOSSUS_SMASH:          {level: 81, spell_id: 86346}
  RAGING_BLOW:             {level: 81, spell_id: 85288, requires_spec: fury}
  RALLYING_CRY:            {level: 83, spell_id: 97462}
  INNER_RAGE:              {level: 83, spell_id: 1134, requires_spec: fury}
  HEROIC_LEAP:             {level: 85, spell_id: 6544}
  TITANS_GRIP:             {requires_talent: true, spell_id: 46917}
  # Talent-gated:
  CRUELTY:                 {requires_talent: true}
  BATTLE_TRANCE:           {requires_talent: true}
  EXECUTIONER:             {requires_talent: true}
  BLOODSURGE:              {requires_talent: true}
  RUDE_INTERRUPTION:       {requires_talent: true}
  PIERCING_HOWL:           {requires_talent: true}
  BOOMING_VOICE:           {requires_talent: true}
  RAMPAGE:                 {requires_talent: true}
  FLURRY:                  {requires_talent: true}
  ENRAGE:                  {requires_talent: true}
  INTENSIFY_RAGE:          {requires_talent: true}
  MEAT_CLEAVER:            {requires_talent: true}
  SINGLE_MINDED_FURY:      {requires_talent: true}
  WAR_ACADEMY:             {requires_talent: true}
  FIELD_DRESSING:          {requires_talent: true}
  TOUGHNESS:               {requires_talent: true}
  INCITE:                  {requires_talent: true}
  # Mastery / passives:
  MASTERY_UNSHACKLED_FURY: {passive_mastery: true, base_pct: 11.2, per_point_pct: 1.4}
```

### Leveling brackets

```yaml
bracket_1_5:
  available: CHARGE, HEROIC_STRIKE, BATTLE_SHOUT, REND, VICTORY_RUSH
  rotation: Charge -> HS spam -> Rend -> Victory Rush post-kill

bracket_6_9:
  available: + BLOODRAGE (6), + THUNDER_CLAP (6), + DEFENSIVE_STANCE (8), + HAMSTRING (8)
  rotation_addition: Bloodrage pre-pull rage gen

bracket_10_19:
  available: + BLOODTHIRST (10 Fury), + SUNDER_ARMOR (10), + EXECUTE (12), + OVERPOWER (14)
  rotation_change: BT becomes Tier 3 priority on CD

bracket_20_29:
  available: + CLEAVE (20)
  rotation_addition: AoE rage dump

bracket_30_35:
  available: + BERSERKER_STANCE (30), + ENRAGED_REGENERATION (30), + BERSERKER_RAGE (32)
  rotation_change: switch to Berserker Stance permanently for Fury

bracket_36_39:
  available: + WHIRLWIND (36 Fury), + INTERVENE (36), + PUMMEL (38)
  rotation_addition: WW becomes Tier 5; Pummel auto-interrupt

bracket_40_55:
  available: + RECKLESSNESS (40), + DEATH_WISH (40 talent)
  cooldown_addition: Reck + DW on CD

bracket_56_69:
  available: + SPELL_REFLECTION (56)
  defensive_addition: SR vs single spells

bracket_70_80:
  available: + COMMANDING_SHOUT (70), + SHATTERING_THROW (70)

bracket_81_82:
  available: + COLOSSUS_SMASH (81), + RAGING_BLOW (81 Fury)
  rotation_change: full encounter rotation activates (CS Tier 2, RB Tier 4)

bracket_83_84:
  available: + RALLYING_CRY (83), + INNER_RAGE (83 Fury)
  cooldown_addition: Rallying Cry group; Inner Rage rage cap

bracket_85:
  available: + HEROIC_LEAP (85)
  rotation: full encounter rotation per §"Rotation"
```

**Warrior is rage-only** — no mana / energy / focus.

---

## UNVERIFIED items

| # | Item | Spec value | Verification |
|---|---|---|---|
| 1 | Fury rage-gen multiplier vs Arms | ~2x | Spell.dbc + Talent.dbc |
| 2 | Bloodthirst rage cost | 30 (20 Cruelty) | Spell.dbc 23881 + Talent |
| 3 | Raging Blow MH+OH percentages | TBD | Spell.dbc 85288 |
| 4 | Whirlwind MH+OH | 100/100? | Spell.dbc 1680 |
| 5 | HS rage cost reduction (Improved HS) | TBD | Talent.dbc |
| 6 | Inner Rage exact effect | 50% rage cost + off-limit | Spell.dbc 1134 |
| 7 | Colossus Smash armor-ignore PvE | 70% (assumed) | Spell.dbc 86346 |
| 8 | Bloodsurge stack max | 3 (Cata) | Talent.dbc |
| 9 | Titan's Grip damage penalty | -10% (talent-reducible) | Talent.dbc |
| 10 | Mastery Unshackled Fury formula | 11.2% + 1.4%/pt | Mastery DBC |
| 11 | Death Wish damage-taken | TBD | Talent.dbc 12292 |
| 12 | Recklessness +crit | 50% | Spell.dbc 1719 |
| 13 | Pummel CD vs Rude Interruption | 10s base | Talent.dbc |
| 14 | Heroic Leap radius + damage | TBD | Spell.dbc 6544 |
| 15 | Rallying Cry max-HP | +20% | Spell.dbc 97462 |
| 16 | Shattering Throw mechanics | immunity + +20% physical | Spell.dbc 64382 |
| 17 | Berserker Stance damage-taken penalty | NONE in 4.3.4 | Stance DBC |
| 18 | Intercept Cata removal | TBD | Spell.dbc 20252 |
| 19 | T13 2P/4P set bonuses (Fury) | TBD | Item set DBC |
| 20 | Glyph values (BT/RB/Slam/WW/Cleave/CS/DW) | TBD | Glyph.dbc |
| 21 | Golemblood Potion duration | 25s | Item DBC |
| 22 | Battle Shout AP value | TBD | Spell.dbc 6673 |
| 23 | Berserker Rage Enrage duration | 6s base | Spell.dbc 18499 |
| 24 | Enrage talent crit-on-auto-attack proc rate | TBD | Talent.dbc |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kFuryWarrior_Spells[]` |
| Rotation tiers | `static const RotationTier kFuryWarrior_Rotation[]` |
| AoE rotation | `static const RotationTier kFuryWarrior_RotationAoE[]` |
| Cooldown Profile | `static const CooldownTrigger kFuryWarrior_Cooldowns[]` |
| Rage management | `RageState` enum + threshold helpers |
| Bloodsurge proc tracking | aura listener |
| Enrage proc tracking | aura listener |
| Stance management | `WarriorStance` enum (Battle/Berserker/Defensive) |
| Inner Rage state | `bool inner_rage_active` |
| Leveling rotation | `LevelBracketRotation kFuryWarrior_Leveling[]` |

**Key new helper APIs:**
- `Rage(bot)` — read 0-100.
- `BloodsurgeStacks(bot)` — read 0-3.
- `EnrageActive(bot)` + remaining ms.
- `InnerRageActive(bot)`.
- `CurrentStance(bot)` — read Battle/Berserker/Defensive.
- `IsAoEMode(bot)` — `enemies_within_8y_self >= 2`.
- `IsExecutePhase(target)` — `target.hp_pct <= 20`.

Cross-reference to `docs/roles/melee-dps.md` (rear-arc / gap-closer / interrupt-as-primary patterns).
