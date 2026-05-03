# Marksmanship Hunter — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide.

**Numerical data** verified against Wowhead's Cata 4.3.4 archive and Warcraft Wiki. Items tagged `UNVERIFIED` need DBC verification.

**Strategy content** reconciled across 17 community sources. See `docs/research/marksmanship-hunter-guide-survey.md`.

**SPEC-CHOICE RATIONALE.** Survival is **A-tier (rank #5)** in Dragon Soul; Marksmanship is **D-tier (rank #20-21)**; Beast Mastery is also D-tier. Project picks Marksmanship on rotation-engine simplicity (single proc system + execute-style window vs SV's Lock-and-Load proc cascade). Future option: Survival sibling spec for max-level raid optimization. This is the **third ranged-DPS spec** in the project.

**HUNTER IS FOCUS-ONLY since patch 4.0.1.** Mana floors / cross-class mana brackets do NOT apply.

---

## Identity

```yaml
spec: marksmanship_hunter
class: hunter
role: RANGED_DPS
resource_model: focus (max 100, base 4/sec regen, scales with haste)
defining_mechanic: Master Marksman stack-and-Fire! + Careful Aim window (HP > 90%)
defining_buffs:
  - Aspect_of_the_Hawk         # +ranged AP (default damage aspect)
  - Aspect_of_the_Fox          # cast-while-moving Steady (movement override)
  - Improved_Steady_Shot       # +15% ranged haste 8s (2x Steady consecutive)
  - Trueshot_Aura              # raid +10% RAP
  - Hunters_Mark               # debuff +20 RAP for all attackers
defining_procs:
  - Master_Marksman            # 60% per Steady → 5-stack → Fire! free instant Aimed Shot
  - Careful_Aim                # +60% crit on Aimed/Steady vs target HP > 90%
mastery: Wild_Quiver           # 16.8% base + 2.1%/pt extra ranged shot proc
key_cooldowns:
  - Rapid_Fire                 # 5 min, 15s, +40% ranged haste
  - Readiness                  # 3 min, resets all Hunter CDs
  - Call_of_the_Wild           # pet Ferocity, +10% AP self+pet, 20s, 5 min
  - Misdirection               # 30s, 4s damage-redirect, 30s threat-fade
  - Disengage                  # 25s (21s talent), backwards leap
  - Deterrence                 # 2 min, 5s parry-everything
  - Feign_Death                # 30s, threat-dump
ranged: 40 (46 with Hawk Eye 2/2)
notes: |
  Marksmanship Hunter in 4.3.4 plays as a focus-management ranged caster.
  Heartbeat: maintain Improved Steady Shot (2x Steady = +15% haste 8s),
  apply Serpent Sting once (refreshed by Chimera Shot), Chimera Shot on CD,
  consume "Fire!" procs on instant Aimed Shot, hard-cast Aimed Shot during
  Careful Aim window (target HP > 90%) AND when stationary AND focus ≥ 65,
  Arcane Shot focus dump otherwise, Steady Shot filler, Kill Shot at HP ≤ 20%.

  Pet: Cat (Ferocity, Roar of Courage +4% Str/Agi). Wolf for Furious Howl
  if no melee in group. Devilsaur for Sunder Armor coverage.

  Cobra Shot is BM/SV-only — MM never casts it. Volley REMOVED in 4.0.1.
  Hunter is FOCUS-ONLY since 4.0.1; no mana management.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  PET:
  CURRENT_DPS_TARGET:
  ENEMY_CASTING_INTERRUPTIBLE:    # for Silencing Shot
  AOE_CLUSTER:                     # ≥3 enemies within 8y of target (Multi-Shot AoE radius)
  TANK:                            # for Misdirection
  TRAP_GROUND_TARGET:              # for Trap Launcher
```

---

## Spell Catalog

```yaml
spells:
  # ─────── Filler / focus generation ───────

  STEADY_SHOT:
    id: 56641
    focus_cost: -9                # generates 9 focus
    cast_time_ms: 2000             # haste-affected; 1500ms with ISS
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: enemy
    notes: |
      Filler. Generates Master Marksman stacks (60%/cast at 3/3). Two
      consecutive Steady Shots apply Improved Steady Shot buff (+15% haste 8s).

  AUTO_SHOT:
    id: 75
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 0                # auto-fires between casts
    on_gcd: false
    range: 40
    target_type: enemy
    notes: always-on; fires while moving in Cata.

  # ─────── Focus dumps ───────

  AIMED_SHOT:
    id: 19434
    focus_cost: 50
    cast_time_ms: 2900             # haste-affected
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: enemy
    notes: |
      132% weapon damage. Stationary cast. Default focus dump when focus ≥ 65
      AND stationary AND (Careful Aim window OR Master Marksman opportunity).

  AIMED_SHOT_INSTANT:
    id: 82928                      # "Fire!" buff version
    focus_cost: 0                  # free
    cast_time_ms: 0                # instant
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: enemy
    requires_buff: FIRE_BUFF       # from Master Marksman 5-stack
    notes: instant + free. Tier 1 priority on proc.

  ARCANE_SHOT:
    id: 3044
    focus_cost: 25                 # cheap dump
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: enemy
    school: arcane
    notes: focus dump on the move OR when target HP ≤ 90% (no Careful Aim).

  CHIMERA_SHOT:
    id: 53209
    focus_cost: 50
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: true
    range: 40
    target_type: enemy
    requires_talent: true          # MM tier 7 capstone
    notes: |
      Instant. Refreshes Serpent Sting on target. Heals self for 5% max HP.
      Cast on CD if Serpent Sting applied.

  SERPENT_STING:
    id: 1978
    focus_cost: 25
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: enemy
    duration_ms: 15000             # base; refreshed by Chimera
    notes: apply once per target. Improved Serpent Sting talent: +30% damage + instant tick on apply.

  KILL_SHOT:
    id: 53351
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: true
    range: 45
    target_type: enemy
    requires_target_hp_pct: 20     # ≤20%
    notes: execute. Glyph: second cast within 6s if target survives.

  MULTI_SHOT:
    id: 2643
    focus_cost: 40
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: enemy_aoe
    radius: 8
    notes: AoE focus dump. 85% bonus weapon damage.

  CONCUSSIVE_SHOT:
    id: 5116
    focus_cost: 5
    cast_time_ms: 0
    cooldown_ms: 5000
    on_gcd: true
    range: 40
    target_type: enemy
    duration_ms: 6000
    notes: 50% slow. PvE on caster trash.

  # ─────── Cooldowns ───────

  RAPID_FIRE:
    id: 3045
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 300000            # 5 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 15000
    notes: +40% ranged attack speed. On-CD opener.

  READINESS:
    id: 23989
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 180000            # 3 min
    on_gcd: false
    range: 0
    target_type: self
    notes: resets all Hunter CDs except Readiness itself.

  CALL_OF_THE_WILD:
    id: 53434                      # pet ability
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 300000            # 5 min
    on_gcd: false
    range: 0
    target_type: self_pet_aura
    duration_ms: 20000
    requires_pet_talent: true      # Ferocity
    notes: +10% AP self + pet for 20s. Macro with Rapid Fire.

  TRUESHOT_AURA:
    id: 19506                      # UNVERIFIED — Cata version
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self_aura_raid
    duration_ms: 1800000           # 30 min, until cancelled
    requires_talent: true
    notes: +10% raid RAP. Skip if redundant with Battle Shout + Blessing of Might.

  SILENCING_SHOT:
    id: 34490
    focus_cost: 25
    cast_time_ms: 0
    cooldown_ms: 24000
    on_gcd: true
    range: 40
    target_type: enemy_casting
    duration_ms: 3000              # 3s silence
    requires_talent: true
    notes: interrupt + 3s silence. Bot's interrupt.

  # ─────── Defensives & utility ───────

  DETERRENCE:
    id: 19263
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 120000             # 2 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 5000
    notes: parry/deflect everything 5s.

  FEIGN_DEATH:
    id: 5384
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 360000             # 6 min unless cancelled
    notes: -90% threat. Drops aggro.

  DISENGAGE:
    id: 781
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 25000              # 21s with Posthaste talent
    on_gcd: false
    range: 0
    target_type: self
    notes: backwards leap. Mobility/escape.

  HUNTERS_MARK:
    id: 1130
    focus_cost: 0                   # 0 in 4.0.1+
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 100
    target_type: enemy
    duration_ms: 300000             # 5 min
    notes: +20 ranged AP for all attackers.

  MISDIRECTION:
    id: 34477
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: false
    range: 30
    target_type: ally
    duration_ms: 30000              # 30s threat-fade window
    redirect_window_s: 4
    notes: redirects bot's threat to ally for 4s of damage. Cast on tank pre-pull.

  TRANQUILIZING_SHOT:
    id: 19801
    focus_cost: 25
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: enemy
    notes: dispels enrage / magic effect. Auto-purge.

  # ─────── Aspects (mutually exclusive) ───────

  ASPECT_OF_THE_HAWK:
    id: 13165
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    notes: +ranged AP. Default damage aspect.

  ASPECT_OF_THE_FOX:
    id: 82661
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    notes: cast Steady/Cobra while moving. Party-wide +2 focus per melee hit (UNVERIFIED scope).

  ASPECT_OF_THE_CHEETAH:
    id: 5118
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    notes: solo +30% move speed. Daze on hit.

  ASPECT_OF_THE_PACK:
    id: 13159
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self_aura_party
    duration_ms: 1800000
    notes: party +30% move speed. Daze risk.

  # ─────── Traps ───────

  TRAP_LAUNCHER:
    id: 77769
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 1500              # 1.5s GCD
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 15000
    level_required: 75              # UNVERIFIED — wiki says 48
    notes: |
      Buff. Allows next trap to be launched at 40y (vs ground-place at feet).
      Tier 4 of AoE rotation when stationary cluster.

  EXPLOSIVE_TRAP:
    id: 13813
    focus_cost: 25
    cast_time_ms: 0
    cooldown_ms: 30000              # shared trap CD
    on_gcd: true
    range: 0                        # 40y with Trap Launcher
    target_type: ground_at_self
    radius: 10
    duration_ms: 20000               # ground DoT
    notes: AoE Fire DoT trap.

  FROST_TRAP:
    id: 13809
    focus_cost: 25
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: true
    range: 0
    target_type: ground_at_self
    radius: 10
    duration_ms: 30000
    notes: AoE slow 50%.

  # ─────── Pet ───────

  TAME_BEAST:
    id: 1515
    focus_cost: 0
    cast_time_ms: 20000
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: enemy_beast
    notes: tame a wild beast. Out of combat only.

  CALL_PET:
    id: 883
    focus_cost: 0
    cast_time_ms: 6000
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self_pet_summon
    notes: summon active pet from stable.

  REVIVE_PET:
    id: 982
    focus_cost: 0
    cast_time_ms: 6000
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self_pet
    notes: revive dead pet.

  MEND_PET:
    id: 136
    focus_cost: 0
    cast_time_ms: 0
    cooldown_ms: 0                  # channel
    on_gcd: true
    range: 45
    target_type: self_pet
    duration_ms: 10000
    notes: pet HoT 10s.
```

---

## Maintenance pass

```yaml
maintenance:
  - name: Aspect of the Hawk (default)
    when:
      - aura.missing(SELF, ASPECT_OF_THE_HAWK)
      - aspect_mode == HAWK
    cast: ASPECT_OF_THE_HAWK

  - name: Aspect of the Fox (movement override)
    when:
      - aura.missing(SELF, ASPECT_OF_THE_FOX)
      - aspect_mode == FOX
    cast: ASPECT_OF_THE_FOX

  - name: Trueshot Aura
    when:
      - aura.missing(SELF, TRUESHOT_AURA)
      - has_talent(TRUESHOT_AURA)
      - NOT runtime_flag.trueshot_redundant
    cast: TRUESHOT_AURA

  - name: Pet — keep summoned
    when:
      - pet.dead OR pet.absent
      - any of:
          - NOT in_combat
          - in_combat AND fight_remaining_estimate > 30000
    cast: CALL_PET (or REVIVE_PET if dead-with-corpse)

  - name: Mend Pet
    when:
      - in_combat
      - pet.alive
      - pet.hp_pct < 60
    cast: MEND_PET on PET

  - name: Hunter's Mark on priority target
    when:
      - in_combat
      - target_resolves(CURRENT_DPS_TARGET)
      - aura.missing(CURRENT_DPS_TARGET, HUNTERS_MARK)
      - target.is_boss OR target.is_priority
    cast: HUNTERS_MARK on CURRENT_DPS_TARGET
```

---

## Rotation — Priority List (Single-Target)

```yaml
rotation:
  # ───────── Tier 1: Master Marksman "Fire!" Aimed Shot ─────────
  - tier: 1
    name: Aimed Shot — Fire! free instant
    when:
      - aura.active(SELF, FIRE_BUFF)
      - target_in_range(40)
      - target.hp_pct > 20            # else Tier 4 Kill Shot wins
    cast: AIMED_SHOT_INSTANT on CURRENT_DPS_TARGET

  # ───────── Tier 2: Serpent Sting maintenance ─────────
  - tier: 2
    name: Serpent Sting — apply once per target
    when:
      - aura.missing(CURRENT_DPS_TARGET, SERPENT_STING)
      - focus >= 25
      - target_in_range(40)
    cast: SERPENT_STING on CURRENT_DPS_TARGET

  # ───────── Tier 3: Improved Steady Shot maintenance ─────────
  - tier: 3
    name: Improved Steady Shot — 2x Steady refresh
    when:
      - has_talent(IMPROVED_STEADY_SHOT)
      - any of:
          - aura.missing(SELF, IMPROVED_STEADY_SHOT_BUFF)
          - aura.expires_in_ms(SELF, IMPROVED_STEADY_SHOT_BUFF) < 1500
      - target_in_range(40)
    cast: STEADY_SHOT on CURRENT_DPS_TARGET
    notes: |
      Tracked via steady_shot_consecutive_count. After 2 in a row (count == 2),
      ISS buff applies. Tier-3 fires Steady when buff is about to expire.

  # ───────── Tier 4: Kill Shot ─────────
  - tier: 4
    name: Kill Shot — execute at ≤20% HP
    when:
      - CURRENT_DPS_TARGET.hp_pct <= 20
      - spell.off_cooldown(KILL_SHOT)
      - target_in_range(45)
    cast: KILL_SHOT on CURRENT_DPS_TARGET

  # ───────── Tier 5: Chimera Shot on CD ─────────
  - tier: 5
    name: Chimera Shot on CD
    when:
      - has_talent(CHIMERA_SHOT)
      - spell.off_cooldown(CHIMERA_SHOT)
      - aura.active(CURRENT_DPS_TARGET, SERPENT_STING)
      - focus >= 50
      - target_in_range(40)
    cast: CHIMERA_SHOT on CURRENT_DPS_TARGET
    notes: refreshes Serpent Sting + heals 5% max HP.

  # ───────── Tier 6: Aimed Shot (Careful Aim or focus capped) ─────────
  - tier: 6
    name: Aimed Shot — hard-cast focus dump
    when:
      - focus >= 65
      - target_in_range(40)
      - NOT in_movement_mode
      - any of:
          - CURRENT_DPS_TARGET.hp_pct > 90    # Careful Aim window
          - master_marksman_stack_progressing  # consume opportunity (4 stacks at 5)
    cast: AIMED_SHOT on CURRENT_DPS_TARGET
    notes: 2.9s hard-cast; only fire when stationary.

  # ───────── Tier 7: Arcane Shot (focus dump on the move OR not Careful Aim) ─────────
  - tier: 7
    name: Arcane Shot — focus dump
    when:
      - focus >= 50
      - target_in_range(40)
      - any of:
          - in_movement_mode
          - CURRENT_DPS_TARGET.hp_pct <= 90    # past Careful Aim
    cast: ARCANE_SHOT on CURRENT_DPS_TARGET

  # ───────── Tier 8: Steady Shot filler ─────────
  - tier: 8
    name: Steady Shot — filler / focus regen
    when:
      - target_in_range(40)
    cast: STEADY_SHOT on CURRENT_DPS_TARGET
    notes: |
      Default fallback. Generates 9 focus + Master Marksman stack progress
      + maintains Improved Steady Shot timing.
```

---

## AoE Mode

Activated by `enemies_within_8y_of(CURRENT_DPS_TARGET) >= 3`. Hysteresis: exit `<2 sustained for 5s` OR boss-flagged target.

```yaml
rotation_aoe:
  - tier: 1: aura.active(SELF, FIRE_BUFF) -> AIMED_SHOT_INSTANT (still highest single-target hit)
  - tier: 2: aura.missing(SELF, IMPROVED_STEADY_SHOT_BUFF) -> STEADY_SHOT (maintain ISS)
  - tier: 3: focus >= 40 -> MULTI_SHOT (8y AoE focus dump)
  - tier: 4: stationary_8s_predicted AND has_talent(TRAP_LAUNCHER) AND off_CD -> TRAP_LAUNCHER + EXPLOSIVE_TRAP
  - tier: 5: focus >= 25 AND target_in_range(40) -> ARCANE_SHOT (single-target finisher)
  - tier: 6: STEADY_SHOT filler

notes: Multi-Shot hits 8y around target; match radius for entry threshold.
```

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: RAPID_FIRE
    when:
      - in_combat
      - spell.off_cooldown(RAPID_FIRE)
      - any of:
          - boss_engagement OR fight_just_started
          - cooldown_window_aligned (Bloodlust + trinkets)
    rationale: 5-min CD primary DPS cooldown. Macro with Call of the Wild.

  - spell: CALL_OF_THE_WILD
    when:
      - in_combat
      - pet.alive
      - pet_talent_ferocity
      - spell.off_cooldown(CALL_OF_THE_WILD)
      - aura.active(SELF, RAPID_FIRE_BUFF) OR boss_engagement
    rationale: pet 5-min CD; align with Rapid Fire.

  - spell: READINESS
    when:
      - in_combat
      - spell.off_cooldown(READINESS)
      - spell.on_cooldown(RAPID_FIRE)
      - aura.expires_in_ms(SELF, RAPID_FIRE_BUFF) < 5000 OR rapid_fire_already_consumed_in_burst
    rationale: 3-min CD. Reset Rapid Fire + Chimera + Kill Shot for second burst.

  - spell: SILENCING_SHOT
    when:
      - target.casting AND cast_remaining > 0.5s
      - spell.off_cooldown(SILENCING_SHOT)
      - target_in_range(40)
      - NOT (other_interrupt_class_off_cd_in_2s AND interrupt_rotation_position != self)
    rationale: 24s CD interrupt; respect rotation discipline.

  - spell: MISDIRECTION
    when:
      - boss_pull_imminent_in_4s OR self.threat_pct > 80
      - target_resolves(TANK)
      - spell.off_cooldown(MISDIRECTION)
    rationale: 30s CD threat-redirect.

  - spell: TRANQUILIZING_SHOT
    when:
      - target.has_dispellable_enrage OR target.has_dispellable_magic
      - spell.off_cooldown(TRANQUILIZING_SHOT)
      - focus >= 25
    rationale: auto-purge.

  - spell: DETERRENCE
    when:
      - SELF.hp_pct < 35
      - no_other_save_ready_in_1500ms
      - spell.off_cooldown(DETERRENCE)
    rationale: 2-min CD parry-everything.

  - spell: FEIGN_DEATH
    when:
      - any of:
          - SELF.threat_pct > 90 AND NOT main_threat_intended
          - SELF.hp_pct < 25
      - spell.off_cooldown(FEIGN_DEATH)
    rationale: threat dump.

  - spell: DISENGAGE
    when:
      - any of:
          - melee_count_within_8y >= 2 AND SELF.hp_pct < 60
          - need_to_kite
      - spell.off_cooldown(DISENGAGE)
    rationale: mobility/escape.
```

---

## Proc Reactions

```yaml
procs:
  MASTER_MARKSMAN:
    talent_id: 56826             # UNVERIFIED — Talent.dbc
    triggered_by: Steady Shot cast (60% at 3/3)
    effect: applies "Ready, Set, Aim..." stack (max 5, 30s)
    on_5_stacks: applies FIRE_BUFF — next Aimed Shot is instant + free
    fire_buff_duration_ms: 30000     # UNVERIFIED — assumed 30s window
    ai_behavior: Tier 1 consumes immediately on Fire! buff (unless target HP ≤ 20% — Kill Shot wins).

  CAREFUL_AIM:
    talent_passive: true
    triggered_by: target HP > 90%
    effect: +60% crit chance on Aimed Shot, Steady Shot, Cobra Shot
    ai_behavior: Tier 6 fires Aimed Shot during Careful Aim window aggressively.

  PIERCING_SHOTS:
    talent_passive: true
    triggered_by: critical Aimed/Steady/Chimera Shot
    effect: 30% bleed over 8s; refreshes
    ai_behavior: passive; informs crit valuation.

  WILD_QUIVER:
    mastery_passive: true
    triggered_by: ranged auto-attack
    proc_chance_pct: 16.8 + (mastery_points * 2.1)
    effect: extra ranged shot hit
    ai_behavior: passive.

  IMPROVED_STEADY_SHOT:
    talent_passive: true
    triggered_by: 2 consecutive Steady Shots
    effect: +15% ranged haste 8s
    ai_behavior: Tier 3 maintains via consecutive Steady casts.
```

---

## Focus Management

```yaml
focus:
  max: 100
  base_regen_per_sec: 4
  scales_with_haste: true

  thresholds:
    aimed_shot_floor: 65            # below this, prefer Arcane Shot
    arcane_shot_floor: 50           # below this, prefer Steady Shot
    multi_shot_floor: 40
    chimera_shot_floor: 50

  in_combat_management:
    - focus < 25 -> Steady Shot only (refill)
    - focus 25-49 -> Steady Shot + maintenance Tier 2-3 (Serpent Sting, ISS)
    - focus 50-64 AND on_move -> Arcane Shot
    - focus >= 65 AND stationary AND careful_aim_window -> Aimed Shot
    - focus >= 65 AND stationary AND focus capped or capping -> Aimed Shot
    - focus >= 50 AND focus would cap on next regen tick -> Arcane Shot (cap prevention)

  out_of_combat:
    drink_when: NOT applicable (Hunter has no mana)
    eat_when: SELF.hp_pct < 80 AND no_enemies_within(40)
```

---

## Consumables

```yaml
consumables:
  flask:
    item: Flask_of_the_Winds
    item_id: 58087               # +300 Agility

  food:
    primary:
      item: Skewered_Eel
      item_id: 62290             # +90 Agility
    fallback:
      item: Seafood_Magnifique_Feast
      item_id: 62290

  dps_potion:
    item: Potion_of_the_Tolvir
    item_id: 58145
    use_when:
      - boss_pull_imminent_in_2s          # pre-pot
      - OR (in_combat AND fight_elapsed_ms ~ 60000 AND rapid_fire_off_cd AND call_of_the_wild_off_cd)

  healthstone:
    item_id: 36892
    use_when:
      - SELF.hp_pct < 30
      - silenced OR cant_cast_for_ms > 1500
```

---

## Non-encounter content

### Solo combat

```yaml
solo_offensive_rotation:
  - tier 1: SELF.hp_pct < 30 -> DETERRENCE if off_cd else FEIGN_DEATH (drop aggro)
  - tier 2: melee_count_within_8y >= 2 -> DISENGAGE (escape)
  - tier 3: pet.alive AND pet.hp<60 -> MEND_PET
  - tier 4: aura.missing(target, SERPENT_STING) -> SERPENT_STING (lvl 4+)
  - tier 5: target.hp_pct <= 20 AND off_cd(KILL_SHOT) -> KILL_SHOT (lvl 35+)
  - tier 6: aura.active(SELF, FIRE_BUFF) -> AIMED_SHOT_INSTANT
  - tier 7: focus >= 50 AND off_cd(CHIMERA_SHOT) -> CHIMERA_SHOT (lvl 51+)
  - tier 8: stationary AND focus >= 65 -> AIMED_SHOT
  - tier 9: focus >= 25 -> ARCANE_SHOT
  - tier 10: STEADY_SHOT filler
notes: leveling — pet pulls/tanks; bot stays at range.
```

### Travel & out-of-combat

```yaml
travel:
  cheetah:
    use_when:
      - not in_combat
      - distance_to_destination > 30 AND distance_to_destination < 80
      - solo
    cast: ASPECT_OF_THE_CHEETAH

  pack:
    use_when:
      - not in_combat
      - in_party_with_master
      - distance_to_destination > 30 AND distance_to_destination < 80
    cast: ASPECT_OF_THE_PACK
    notes: party +30% move speed; daze risk in combat.

  mount:
    use_when:
      - not in_combat
      - distance_to_destination >= 80
      - mount_zone_allowed

ooc_maintenance:
  - check: aura.missing(SELF, ASPECT_OF_THE_HAWK) AND aspect_mode == HAWK -> ASPECT_OF_THE_HAWK
  - check: aura.missing(SELF, TRUESHOT_AURA) AND has_talent -> TRUESHOT_AURA
  - check: pet.dead AND not_in_combat -> CALL_PET or REVIVE_PET
  - check: SELF.hp_pct < 95 AND no_enemies_within(40) -> eat
```

### Idle

```yaml
idle:
  primary_action: follow master
  secondary_actions:
    - maintain ASPECT_OF_THE_HAWK
    - maintain TRUESHOT_AURA
    - maintain pet
  do_not:
    - do not cast offensive spells while idle
    - do not Multi-Shot neutral mobs
```

### World boss / elite scaling

```yaml
world_boss_or_elite:
  rotation_override:
    if_in_party:
      use: encounter rotation
      thresholds:
        deterrence_threshold: 50
        feign_death_threshold: 35
    if_solo:
      use: solo_offensive_rotation
      thresholds:
        do_not_engage_unless_can_solo: true
        keep_pet_alive_priority: high
```

### Disengage / emergency utility

```yaml
disengage:
  feign_death:
    use_when:
      - SELF.threat_pct > 90 AND NOT main_threat_intended
    cast: FEIGN_DEATH

  deterrence:
    use_when:
      - SELF.hp_pct < 35
      - no_other_save
    cast: DETERRENCE

  disengage:
    use_when:
      - melee_count_within_8y >= 2
    cast: DISENGAGE
```

---

## Leveling rotation

Pure Marksmanship from level 10 (spec choice).

### Spec recommendation

```yaml
spec_choice:
  for_leveling_dungeon_dps_bot:
    spec: Marksmanship
    talent_split: 7 BM / 31 MM / 3 SV
    talent_picks_bm: Go for the Throat 2/2 + One With Nature 3/3 + filler
    talent_picks_mm: Careful Aim 2/2 + Improved Steady Shot 2/2 + Piercing Shots 3/3 + Master Marksman 3/3 + Posthaste 2/2 + Trueshot Aura 1/1 + Silencing Shot 1/1 + Rapid Recuperation 2/2 + Chimera Shot (capstone)
    talent_picks_sv: Hawk Eye 2/2 + Improved Serpent Sting 1/1
    rationale: |
      Per survey: 7/31/3 consensus across G11/G13/G14. 31 MM core unlocks
      Chimera Shot capstone. 7 BM dip captures pet-focus generation +
      passive stat gains. 3 SV captures +6y range + Serpent Sting damage.
```

### Spell unlock table

```yaml
spell_unlocks:
  AUTO_SHOT:             {level: 1,  spell_id: 75}
  CALL_PET:              {level: 1,  spell_id: 883}
  TAME_BEAST:            {level: 10, spell_id: 1515}
  REVIVE_PET:            {level: 10, spell_id: 982}
  SERPENT_STING:         {level: 4,  spell_id: 1978}
  ARCANE_SHOT:           {level: 6,  spell_id: 3044}
  STEADY_SHOT:           {level: 7,  spell_id: 56641}
  CONCUSSIVE_SHOT:       {level: 8,  spell_id: 5116}
  AIMED_SHOT:            {level: 10, spell_id: 19434}    # MM-only baseline
  ASPECT_OF_THE_HAWK:    {level: 12, spell_id: 13165}
  HUNTERS_MARK:          {level: 14, spell_id: 1130}
  ASPECT_OF_THE_PACK:    {level: 14, spell_id: 13159}
  ASPECT_OF_THE_CHEETAH: {level: 16, spell_id: 5118}
  MULTI_SHOT:            {level: 18, spell_id: 2643}
  DETERRENCE:            {level: 20, spell_id: 19263}
  ICE_TRAP:              {level: 20, spell_id: 13809}
  RAPID_FIRE:            {level: 26, spell_id: 3045}
  FROST_TRAP:            {level: 28, spell_id: 13809}
  FEIGN_DEATH:           {level: 30, spell_id: 5384}
  DISENGAGE:             {level: 32, spell_id: 781}
  KILL_SHOT:             {level: 35, spell_id: 53351}
  TRANQUILIZING_SHOT:    {level: 38, spell_id: 19801}
  MISDIRECTION:          {level: 38, spell_id: 34477}
  EXPLOSIVE_TRAP:        {level: 40, spell_id: 13813}
  SNAKE_TRAP:            {level: 50, spell_id: 34600}
  CHIMERA_SHOT:          {level: 51, spell_id: 53209}    # MM talent
  READINESS:             {level: 60, spell_id: 23989}
  ASPECT_OF_THE_FOX:     {level: 64, spell_id: 82661}
  TRAP_LAUNCHER:         {level: 75, spell_id: 77769}
  COBRA_SHOT:            {level: 81, spell_id: 77767}    # NEVER USE for MM
  # Talent-gated:
  CALL_OF_THE_WILD:      {requires_pet_talent: true, pet_spec: ferocity}
  TRUESHOT_AURA:         {requires_talent: true, tree: marksmanship}
  SILENCING_SHOT:        {requires_talent: true, tree: marksmanship}
  CAREFUL_AIM:           {requires_talent: true, tree: marksmanship}
  IMPROVED_STEADY_SHOT:  {requires_talent: true, tree: marksmanship}
  PIERCING_SHOTS:        {requires_talent: true, tree: marksmanship}
  MASTER_MARKSMAN:       {requires_talent: true, tree: marksmanship}
  POSTHASTE:             {requires_talent: true, tree: marksmanship}
  HAWK_EYE:              {requires_talent: true, tree: survival}
  IMPROVED_SERPENT_STING:{requires_talent: true, tree: survival}
  WILD_QUIVER:           {passive_mastery: true, base_pct: 16.8, per_point: 2.1}
```

### Leveling brackets

```yaml
bracket_1_5:
  available: AUTO_SHOT, CALL_PET (starter), SERPENT_STING (4)
  rotation: SS apply -> AUTO_SHOT spam; pet on offense

bracket_6_9:
  available: + ARCANE_SHOT (6), + STEADY_SHOT (7), + CONCUSSIVE_SHOT (8)
  rotation: SS apply -> AS focus dump -> Steady filler; CS on caster trash

bracket_10_13:
  available: + TAME_BEAST (10), + AIMED_SHOT (10 MM-only)
  pet_change: tame Cat or Wolf at 10
  rotation_addition: Aimed Shot when stationary AND focus ≥ 65

bracket_14_25:
  available: + ASPECT_OF_THE_HAWK (12), + HUNTERS_MARK (14), + MULTI_SHOT (18), + DETERRENCE (20)
  rotation_addition: Hunter's Mark on priority; Multi-Shot for 3+ adds

bracket_26_31:
  available: + RAPID_FIRE (26), + FROST_TRAP (28), + FEIGN_DEATH (30)
  cooldown_addition: Rapid Fire on CD

bracket_32_50:
  available: + DISENGAGE (32), + KILL_SHOT (35), + TRANQUILIZING_SHOT (38), + MISDIRECTION (38), + EXPLOSIVE_TRAP (40)
  rotation_change: Kill Shot at ≤20% HP; Misdirection on tank

bracket_51_59:
  available: + CHIMERA_SHOT (51 talent)
  rotation_change: Chimera Shot becomes Tier 5 priority

bracket_60_74:
  available: + READINESS (60), + ASPECT_OF_THE_FOX (64)
  cooldown_addition: Readiness reset
  movement_addition: Aspect of the Fox swap

bracket_75_84:
  available: + TRAP_LAUNCHER (75)
  rotation_addition: Trap Launcher pre-place for stationary AoE

bracket_85:
  available: + all talents fully allocated, + Cobra Shot baseline (NOT used for MM)
  rotation: full encounter rotation per §"Rotation"
```

**Hunter is FOCUS-only since 4.0.1** — no mana brackets.

---

## UNVERIFIED items

| # | Item | Spec value | Verification |
|---|---|---|---|
| 1 | Master Marksman proc rate per Steady at 3/3 | 60% | Talent.dbc |
| 2 | "Fire!" buff damage modifier | +50% claimed | Spell.dbc 82926 / 82928 |
| 3 | "Ready, Set, Aim..." stack duration | 30s | Spell.dbc |
| 4 | Improved Steady Shot 8s buff / 15% haste | yes | Talent.dbc |
| 5 | Wild Quiver formula | 16.8% + 2.1%/pt | Mastery DBC |
| 6 | Aimed Shot 132% weapon damage | yes | Spell.dbc 19434 |
| 7 | Steady Shot focus generation | 9 | Spell.dbc 56641 |
| 8 | Kill Shot ≤20% threshold | yes | Spell.dbc 53351 |
| 9 | Glyph of Kill Shot 6s second-cast | yes | Glyph.dbc |
| 10 | Chimera Shot 5% max HP heal | yes | Spell.dbc 53209 |
| 11 | Rapid Fire 40% haste | yes | Spell.dbc 3045 |
| 12 | Readiness full-reset confirm | yes | Spell.dbc 23989 |
| 13 | Trap Launcher level | 75 (vs 48 wiki) | DBC |
| 14 | Disengage 25s/21s/16s with talent + glyph | yes | Spell.dbc + Talent.dbc |
| 15 | Misdirection 4s redirect / 30s fade | yes | Spell.dbc 34477 |
| 16 | Hunter's Mark 0 cost in 4.0.1+ | yes | Spell.dbc 1130 |
| 17 | Careful Aim >90% threshold | yes | Talent.dbc |
| 18 | Piercing Shots 30% bleed 8s refresh-or-stack | refresh | Talent.dbc |
| 19 | Aspect of the Hawk RAP value | TBD | Spell.dbc |
| 20 | Aspect of the Fox party-wide vs self regen | TBD | Spell.dbc |
| 21 | Trueshot Aura 10% RAP | yes | Spell.dbc + Talent.dbc |
| 22 | Ferocity Call of the Wild 10% AP/20s/5min | yes | pet ability spell |
| 23 | Pet hit rating coverage from Hunter | yes | server-side mechanic |
| 24 | Volley REMOVED in 4.0.1 confirm | yes | Spell.dbc absence |
| 25 | Multi-Shot 40 focus / 8y / 85% bonus weapon | yes | Spell.dbc 2643 |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kMMHunter_Spells[]` |
| Rotation tiers | `static const RotationTier kMMHunter_Rotation[]` |
| AoE rotation | `static const RotationTier kMMHunter_RotationAoE[]` |
| Cooldown Profile | `static const CooldownTrigger kMMHunter_Cooldowns[]` |
| Focus Management | `FocusState` enum + threshold helpers (NOT mana) |
| Leveling rotation | `LevelBracketRotation kMMHunter_Leveling[]` |
| Master Marksman state | `uint8 mm_stacks` (0-5) + Fire! buff active |
| Improved Steady Shot tracking | `uint8 steady_consecutive_count` + ISS buff timer |
| Aspect mode | `AspectMode` enum (Hawk / Fox / Cheetah / Pack) |
| Pet state | `PetState` (alive/dead, focus, position) |

**Key new helper APIs:**
- `Focus(bot)` — read 0-100.
- `MasterMarksmanStacks(bot)` — read 0-5.
- `FireBuffActive(bot)` — proc state.
- `ImprovedSteadyShotActive(bot)` — buff state + remaining ms.
- `IsCarefulAimWindow(target)` — `target.hp_pct > 90`.
- `SteadyShotConsecutiveCount(bot)` — for ISS trigger tracking.
- `IsInMovementMode(bot)` — for Aimed vs Arcane Shot decision.

Cross-reference to `docs/roles/ranged-dps.md` and the prior ranged-DPS specs (`frost-mage.md`, `affliction-warlock.md`).
