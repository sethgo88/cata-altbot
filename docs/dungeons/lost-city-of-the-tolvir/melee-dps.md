# Lost City of the Tol'vir — Melee DPS

**Melee-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how melee DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content melee DPS patterns (positioning, interrupt rotation, threat ceiling) live in `docs/roles/melee-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (combat-rogue, fury-warrior, ret-paladin, frost-dk, enh-shaman, feral-cat)
- This doc layers Lost City of the Tol'vir-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_melee_dps_overview:
  total_bosses: 4
  estimated_full_clear_minutes: 30-45
  positioning_profile:
    husam:    behind_boss_follow_kite     # follow tank's circle path
    lockmaw:  SIDE_NEVER_BEHIND            # rear cone — special override of normal "behind boss" rule
    barim:    behind_boss_spread           # standard + spread for Plague jump limit
    siamat:   behind_active_unit_stay_center  # target swap; platform knockoff awareness
  interrupt_load:
    husam:   none
    lockmaw: none
    barim:   none
    siamat:  HIGH_IF_IN_RANGE              # Minion Chain Lightning if Minion within melee
  key_responsibilities:
    - kite_path_follow (husam — stay behind boss as tank kites)
    - lockmaw_side_positioning (UNIQUE — never behind boss; rear cone hazard)
    - soul_fragment_intercept (barim P2 — melee with stun/slow if no ranged handles)
    - servant_kill_priority (siamat P1)
    - minion_chain_lightning_kick_if_in_melee (siamat — opportunistic)
```

**Critical override on Lockmaw**: standard melee rule is "stay behind boss" — Lockmaw FLIPS this. Behind Lockmaw = Dust Flail rear cone hazard. Melee bots must SIDE-position on Lockmaw, not back-position. This is an explicit fight-specific override.

---

## Trash — melee DPS notes

```yaml
trash_melee:
  tolvir_stoneshapers:
    cc_priorities: |
      Melee opener-CC contribution:
      - Combat Rogue: Sap (humanoid only); Kick (10s) on caster
      - Fury Warrior: Pummel (10s; off-GCD) interrupt
      - Ret Pally: Repentance (60s; humanoid CC); Rebuke (15s) interrupt
      - Frost DK: Mind Freeze (10s) interrupt; Strangulate (2min silence)
      - Enh Shaman: Hex (60s; safest CC for ambiguous creature_type); Wind Shear (6s) interrupt
      - Feral Cat: Skull Bash (60s; gap-close + interrupt); Cyclone (8s humanoid)
    interrupt_role: |
      Stoneshaper hardcasts are MUST_INTERRUPT — melee primary kick rotation.
      Wind Shear (Enh) and Pummel (Fury) shortest CDs.

  fear_packs:
    notes: |
      Tol'vir / Neferset trash with fear-flavor casts. Melee MUST kick:
      - Combat Rogue: Kick
      - Fury Warrior: Pummel
      - Ret Pally: Rebuke
      - Frost DK: Mind Freeze
      - Enh Shaman: Wind Shear
      - Feral Cat: Skull Bash

  oathblade_axemaster_packs:
    positioning: stand SIDE/REAR on cleave-mobs; standard tank-and-spank rules

  pre_siamat_trash:
    notes: |
      Wind-flavor adds; melee in standard behind-target except where
      otherwise indicated.
```

---

## Boss 1 — General Husam

```yaml
boss_melee:
  boss: general_husam
  difficulty_grade: medium (movement-heavy)
  positioning:
    default: |
      BEHIND boss but FOLLOW kite path. Tank kites Husam in slow circle;
      melee bot stays behind boss as it rotates.
    facing: boss back
    movement: |
      CONSTANT MOVEMENT. Melee bot must:
      - Move off Shockwave X-pattern lines
      - Move off arming Mystic Trap zones
      - Re-engage boss after each movement (gap close)
      Mobility tools:
      - Combat Rogue: Sprint (3min); Step / Shadowstep (UNVERIFIED Combat access)
      - Fury Warrior: Charge / Heroic Leap; Intervene (talent)
      - Ret Pally: Pursuit of Justice (talent — UNVERIFIED Cata)
      - Frost DK: Death's Advance (UNVERIFIED — talent name); Death Grip if needed (HOLD per Karsh-style rule — don't pull boss off tank)
      - Enh Shaman: Ghost Wolf for transit (cancel-cast); Spiritwalker's Grace
      - Feral Cat: Dash (Cat form); Stampeding Roar raid speed
  interrupt_role:
    none: no kick-required casts
  rotation_modifications:
    standard: |
      Standard ST rotation modulated by movement. Re-engage boss after each
      Shockwave / trap dodge.
      - Combat Rogue: SnD maint, RvS refresh, Eviscerate at 5 CP, SS builder
      - Fury Warrior: Slam Bloodsurge proc, CS, BT, RB Enraged, WW, Execute
      - Ret Pally: Inq refresh, TV at 3 HP, CS on CD, HoW conditional, Exo on AoW, Judge, Holy Wrath
      - Frost DK: 2H Frost Obliterate-priority + diseases via Outbreak
      - Enh Shaman: Stormstrike + Lava Lash + MW-stack ST/AoE swap
      - Feral Cat: Mangle + Rake + Rip + SR + Ferocious Bite + Tiger's Fury
    detonate_traps_heroic: |
      During 5s countdown: STOP DPS, MOVE to clean ground; resume DPS post-countdown.
  cd_use:
    standard_offensive: rotational; align Tiger's Fury / Berserk / Avenging Wrath / Pillar of Frost / etc on engagement
  heroic_delta: |
    - More movement (more traps, Detonate Traps event)
    - DPS uptime reduced — accept the loss; don't trade survival for damage
  bot_anti_patterns:
    - DO NOT stand still through Shockwave (line damage = significant spike)
    - DO NOT path through arming Mystic Traps (visible markers)
    - DO NOT pull boss off tank with Death Grip / Charge (disrupts kite)
    - DO NOT stand in front of boss (unlike Lockmaw, Husam has no front cone — but melee still preferred behind)
```

---

## Boss 2 — Lockmaw (and Augh)

```yaml
boss_melee:
  boss: lockmaw
  difficulty_grade: HARD (positioning override)
  positioning:
    default: |
      *** SIDE POSITION — NEVER BEHIND ***
      This is an EXPLICIT OVERRIDE of the default melee "stay behind boss"
      rule. Lockmaw's tail is the back; standing behind = Dust Flail rear
      cone (high damage + 50% accuracy debuff).
      Stand at 90 degrees from boss facing — flank position.
      Tank usually pulls Lockmaw to a wall facing IN; melee bot stands at
      side angle.
    facing: boss flank
    movement: |
      Move out of Viscous Poison ground patches (5y radius).
      Spread from other melee (Scent of Blood adds path to bleed target;
      cluster catches collateral).
      During Augh phase: back off Whirlwind 5y radius.
  interrupt_role:
    none: no kick-required casts on bosses
  rotation_modifications:
    add_handling_scent_of_blood: |
      AoE rotation when Frenzied Crocolisks spawn (2-4 adds typical):
      - Combat Rogue: Blade Flurry on; Fan of Knives if 3+ adds
      - Fury Warrior: Bladestorm (talent — UNVERIFIED Fury access); Whirlwind on add cluster
      - Ret Pally: Divine Storm at 3 HP; Consecration on cluster
      - Frost DK: Howling Blast spam; Death and Decay
      - Enh Shaman: Magma Totem at boss feet; Chain Lightning at 5 MW; Fire Nova on FS-target adds (UNVERIFIED)
      - Feral Cat: Swipe (Cat form) on cluster; maintain Rip on Lockmaw primary
    augh_phase: |
      Standard ST rotation on Augh. KEY RULE:
      - Step out of Whirlwind 5y radius when active
      - Smoke Bomb stuns nearby — melee in 5y range gets stunned (3s)
      - DPS may need to TAUNT during Smoke Bomb (Ret Pally Hand of Reckoning,
        DK Dark Command, Druid Growl in Bear, Shaman no taunt) — bot logic:
        if tank stunned AND boss attacking healer/ranged, melee with taunt
        ability uses it.
    venomous_rage_30_pct: |
      Lockmaw +25% damage; align offensive CD here:
      - Combat Rogue: Shadow Blades / Vendetta (UNVERIFIED Combat; Combat doesn't have Vendetta)
      - Fury Warrior: Recklessness + Death Wish; align with execute
      - Ret Pally: Avenging Wrath + Zealotry + GoAK
      - Frost DK: Pillar of Frost + Empower Rune Weapon
      - Enh Shaman: Feral Spirit + Berserker Rage / Stormblast
      - Feral Cat: Tiger's Fury + Berserk
  cd_use:
    survival_cooldowns: |
      Pop major defensive if Viscous Poison lands AND HP drops below 60%:
      - Combat Rogue: Cloak of Shadows (90s; Magic immune; Poison still ticks but next dispel comes faster) — UNVERIFIED if CoS removes Poison
      - Fury Warrior: Spell Reflection (questionable for Poison — likely no); Berserker Rage immune to fear
      - Ret Pally: Divine Protection (-20% magic/all 10s); Lay on Hands emergency
      - Frost DK: Anti-Magic Shell (75% magic absorb 5s) — DOES cover Nature-school Poison if Poison-school Nature
      - Enh Shaman: Shamanistic Rage; Stoneclaw Totem (talented shield)
      - Feral Cat: Survival Instincts (-50% damage 12s); Barkskin (-20% damage 12s)
  heroic_delta: |
    - Augh full second-phase fight
    - Viscous Poison damage 2x — defensive CDs more important
    - More crocolisk adds; AoE rotation more important
    - Venomous Rage 30% spike higher
  bot_anti_patterns:
    - DO NOT stand behind Lockmaw — RULE INVERSION from standard melee positioning
    - DO NOT stand in Viscous Poison ground patches
    - DO NOT cluster melee together (Scent of Blood collateral; Smoke Bomb stuns multiple)
    - DO NOT continue auto-attack during Smoke Bomb stun (you're stunned; auto-attack swing skipped anyway)
    - DO NOT engage Augh while Lockmaw alive (Augh is immune until Lockmaw dies)
```

---

## Boss 3 — High Prophet Barim

```yaml
boss_melee:
  boss: high_prophet_barim
  difficulty_grade: medium (phase transition + add intercept)
  positioning:
    default: |
      P1: BEHIND Barim, spread from other melee (Plague of Ages bounce
      limit ~8y).
      P2: BEHIND Harbinger of Darkness (target swap from Barim).
      P1 resume: behind Barim again.
    facing: active target back
    movement: |
      P1: move off Heaven's Fury zones; maintain spread.
      Repentance transition: BE READY to move (group stunned 6s, then
      step out of Hallowed Ground).
      P2: position behind Harbinger; intercept Soul Fragments if no ranged
      handles them.
  interrupt_role:
    none: no kick-required casts
  rotation_modifications:
    phase_1_standard: |
      Standard ST rotation on Barim. DoT/bleed maintenance via target swap
      tolerance (Barim → Harbinger → Barim) means accepting reset on transition:
      - Combat Rogue: SnD/RvS continue (self-buffs)
      - Fury Warrior: standard rotation
      - Ret Pally: standard rotation
      - Frost DK: diseases via Outbreak — refresh on each phase
      - Enh Shaman: Lightning Shield + FS maint — FS resets on target swap, refresh
      - Feral Cat: Rip + Rake + SR — bleeds reset on target swap (significant DPS loss but unavoidable)
    repentance_transition: |
      6s group stun at 50% HP. NO OUTPUT during stun. As stun breaks:
      - Move out of Hallowed Ground zone (13y radius)
      - Re-target Harbinger of Darkness
      - DoT/bleed classes: re-apply on Harbinger (or accept reset)
    phase_2_soul_fragment_intercept: |
      Soul Fragments spawn from Soul Sever (every ~4s) and move toward
      Harbinger. RANGED IS PRIMARY KILL ROLE; melee contributes intercept
      stuns/slows if Fragment passes through melee zone:
      - Combat Rogue: Kick (interrupts cast — but Soul Fragments may not cast); Cheap Shot stun (4s); Kidney Shot stun
      - Fury Warrior: Charge stun; Intercept stun; Heroic Leap stun (talented — UNVERIFIED)
      - Ret Pally: Hammer of Justice (60s; 6s stun); Hand of Reckoning (taunt; 30s)
      - Frost DK: Death Grip pull; Strangulate silence
      - Enh Shaman: Hex (60s frog) — UNVERIFIED if works on Fragment; Earthbind Totem slow
      - Feral Cat: Skull Bash (60s gap-close + interrupt); Maim (5 CP stun)
      Once Fragment intercepted/dead, return to Harbinger DPS.
    phase_2_harbinger_burn: |
      Standard ST on Harbinger. Continue rotation.
    phase_1_resume: |
      Harbinger dies → Barim re-engageable. Re-target Barim; resume
      P1 rotation. Reset DoTs/bleeds.
  cd_use:
    bloodlust_or_heroism: |
      Save for Siamat P2 — Barim is medium-pace; not the burn moment.
    survival_cooldowns: |
      Pop defensive if Plague of Ages on self AND HP drops below 60%:
      - Combat Rogue: Cloak of Shadows
      - Fury Warrior: Berserker Rage
      - Ret Pally: Divine Protection
      - Frost DK: Anti-Magic Shell (covers Nature school)
      - Enh Shaman: Shamanistic Rage
      - Feral Cat: Survival Instincts; Barkskin
  heroic_delta: |
    - Damage scales (Plague of Ages 2x)
    - Blaze of the Heavens phoenix add (heroic P1) — RANGED handles (melee continues Barim)
    - Soul Fragments more numerous / faster — melee intercept role tighter
  bot_anti_patterns:
    - DO NOT cluster melee together (Plague of Ages bounce)
    - DO NOT stand in Heaven's Fury zones for DPS uptime (move first)
    - DO NOT continue DPS on Barim during Repentance stun (you're stunned anyway)
    - DO NOT keep DPS on Barim after he becomes immune in P2 (switch to Harbinger)
    - DO NOT spend Combo Points on the wrong target after target swap (CP attached to target)
```

---

## Boss 4 — Siamat

```yaml
boss_melee:
  boss: siamat
  difficulty_grade: medium (target swap + interrupt + positioning)
  positioning:
    default: |
      P1: BEHIND active Servant. Stay >10y from OTHER (non-target) Servants
      to avoid Lightning Nova spillover.
      P2: BEHIND Siamat. STAY CENTER OF PLATFORM (Wailing Winds knockoff).
    facing: active target back
    movement: |
      P1: target-swap among 3 Servants in spawn order; stay behind active.
      P2 entry (Wailing Winds): brace; stay center.
      P2: move to clear Tempest Storms from Siamat path if positioned to.
  interrupt_role:
    minion_chain_lightning: |
      OPPORTUNISTIC kick — if Minion is within melee range AND Chain
      Lightning casting AND ranged DPS hasn't kicked yet:
      - Combat Rogue: Kick (10s)
      - Fury Warrior: Pummel (10s)
      - Ret Pally: Rebuke (15s)
      - Frost DK: Mind Freeze (10s)
      - Enh Shaman: Wind Shear (6s; 25y range — actually ranged-grade)
      - Feral Cat: Skull Bash (60s gap-close)
      Minions are at distance (cast at range); melee gap-close cost may
      not be worth it. Default: let ranged handle.
  rotation_modifications:
    phase_1_servant_priority: |
      KILL ORDER: Servants of Siamat in spawn order. Tank picks up; melee
      DPS contributes. DPS GATED on Servant kills — Siamat takes 90% reduced
      damage in P1 (Deflecting Winds).
      DO NOT WASTE DPS ON SIAMAT IN P1.
      Position behind active Servant; stay >10y from other Servants.
    minion_handling: |
      DO NOT KILL Minions. They self-deplete. Melee DPS does NOT cleave
      Minions even with Blade Flurry / Sweeping Strikes / Whirlwind:
      - Combat Rogue: Blade Flurry OFF if Minion adjacent to Servant
      - Fury Warrior: ST rotation; avoid Whirlwind in mixed-add zones
      - Frost DK: avoid Death and Decay in mixed-add zones
      Cleaving Minions wastes damage on self-dying targets and may trigger
      early Tempest Storm at sub-optimal positions.
    phase_2_siamat_burn: |
      Wailing Winds 6s entry → Siamat attackable. Standard ST behind boss:
      - Combat Rogue: full 4-tier rotation (SnD / RvS / Eviscerate / SS)
      - Fury Warrior: Slam-Bloodsurge → CS → BT → RB Enraged → WW → Execute → HS rage dump
      - Ret Pally: Inq refresh → TV at 3 HP → CS on CD → HoW execute → Exo on AoW → Judge → Holy Wrath
      - Frost DK: 2H Obliterate-priority rotation
      - Enh Shaman: Stormstrike + Lava Lash + MW-stack ST
      - Feral Cat: Mangle + Rake + Rip + SR + FB + Tiger's Fury
      Watch for Cloud Burst spawn under self → move out.
      Watch for Tempest Storm clouds → kite away from Siamat (do not let
      reach).
    gathered_storms_p2_heroic: |
      Stacks on group; heroic = all players. Continue rotation; rely on
      healer AoE heal. Pop defensive if HP drops.
  cd_use:
    bloodlust_or_heroism: |
      P2 ENTRY is canonical Lust moment. Align offensive CDs:
      - Combat Rogue: Adrenaline Rush + Killing Spree + Shadow Blades
      - Fury Warrior: Recklessness + Death Wish + Berserker Rage
      - Ret Pally: Avenging Wrath + Zealotry + GoAK
      - Frost DK: Pillar of Frost + Empower Rune Weapon + Army of the Dead (pre-pull)
      - Enh Shaman: Feral Spirit + Berserker Rage + Stormblast
      - Feral Cat: Tiger's Fury + Berserk + Stampede (UNVERIFIED Cata)
    survival_cooldowns: |
      Pop defensive on heroic Gathered Storms if all-target spike:
      - Combat Rogue: Cloak of Shadows
      - Fury Warrior: Berserker Rage / Spell Reflection (questionable on Nature)
      - Ret Pally: Divine Protection (-20%)
      - Frost DK: Anti-Magic Shell (covers Nature)
      - Enh Shaman: Shamanistic Rage
      - Feral Cat: Survival Instincts; Barkskin
  heroic_delta: |
    - Lightning Charge stacks on group (achievement-relevant; ignore for survival)
    - Gathered Storms hits ALL players — major P2 spike
    - Tighter mechanic windows
    - DPS race in P2 — sustained output critical
  bot_anti_patterns:
    - DO NOT DPS Siamat in P1 (Deflecting Winds = wasted GCDs)
    - DO NOT cleave Minions of Siamat (self-deplete; cleave wastes damage and may trigger early Tempest Storm)
    - DO NOT step off platform during Wailing Winds (knockoff = wipe)
    - DO NOT stand within 10y of active Servant unless that's your target
    - DO NOT stand in Cloud Burst clouds
    - DO NOT spend Combo Points on Servant when Siamat is the new target (P2 transition CP reset for Rogue / Cat)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  husam:
    - encounter_state_active → behind_boss_follow_kite
    - encounter_detect_shockwave_telegraph → move_off_x_pattern_lines
    - encounter_detect_mystic_trap_landed_near_self → move_clear
    - encounter_detect_detonate_traps_countdown_heroic → move_to_clean_ground

  lockmaw:
    - encounter_state_active → side_position_never_behind  # OVERRIDE default melee rule
    - encounter_detect_viscous_poison_under_self → move_out
    - encounter_detect_scent_of_blood_adds_spawned → swap_to_aoe_rotation
    - encounter_detect_smoke_bomb_telegraph → move_outside_5y_or_brace_for_stun
    - encounter_state_lockmaw_hp_pct < 35 → align_offensive_cooldowns  # Venomous Rage burn
    - encounter_state_augh_phase_active → standard_st_rotation_on_augh
    - encounter_detect_augh_whirlwind_active → step_out_5y

  barim:
    - encounter_state_phase_1_active → behind_barim_spread
    - encounter_state_barim_hp_pct < 55 → brace_for_repentance_stun
    - encounter_detect_repentance_stun_active → no_action
    - encounter_state_phase_2_active → swap_target_to_harbinger
    - encounter_detect_soul_fragment_in_melee_range → intercept_with_stun_or_slow
    - encounter_detect_harbinger_dead → swap_target_back_to_barim

  siamat:
    - encounter_state_phase_1_active → swap_target_to_active_servant_in_order
    - encounter_state_servant_active → behind_active_servant_distance_other_servants
    - encounter_state_minion_active AND minion_in_melee_range AND chain_lightning_casting → opportunistic_kick
    - encounter_state_third_servant_dead → brace_for_wailing_winds_stay_center
    - encounter_detect_wailing_winds_active → minimal_movement_stay_center
    - encounter_state_phase_2_active → swap_target_to_siamat_align_lust_cooldowns
    - encounter_detect_cloud_burst_under_self → move_out_of_radius
    - encounter_detect_tempest_storm_path_to_siamat → kite_or_avoid

dispel_blacklist: []                                # no debuffs to BLACKLIST in this dungeon
                                                    # (melee hybrid healers' Cleanse is not invoked here on bosses)

interrupt_priority_per_fight:
  husam:    [no_kicks]
  lockmaw:  [no_kicks]
  barim:    [no_kicks]
  siamat:   [Minion Chain Lightning (103637) — OPPORTUNISTIC if in melee range; ranged primary]
```

---

## Bot-implementation notes

- **Lockmaw side-position override**: this is the ONLY fight in the dungeon where the standard "melee stays behind boss" rule is INVERTED. Bot melee positioning logic must accept fight-specific positioning overrides — Lockmaw flags `melee_position: side` (90 degrees from boss facing). On all other fights, default `melee_position: rear` applies.
- **Siamat Minion-cleave avoidance**: bot melee with cleave abilities (Blade Flurry, Sweeping Strikes, Whirlwind, Death and Decay) must DISABLE cleave when targeting a Servant if Minions are adjacent. Cleaving Minions wastes damage and may trigger Tempest Storm at sub-optimal positions. Combat Rogue: turn off Blade Flurry. Fury Warrior: avoid Whirlwind. Frost DK: hold Death and Decay.
- **Combo-Point / target-bound resource resets**: Combat Rogue and Feral Cat lose ALL Combo Points on target swap. Significant on Barim (P1 → P2 → P1 cycles) and Siamat (Servant 1 → Servant 2 → Servant 3 → Siamat = 4 target swaps minimum). Bot CP-spend logic must accept these resets and rebuild on each new target — do NOT hold CP across target swap (waste).
- **Augh trigger detection**: same as healer — bot uses `target.is_attackable(Augh) AND target.lockmaw.is_dead`, not HP-threshold.
- **Frost DK Death Grip on bosses**: HOLD Death Grip on Husam (don't pull off kite path), Lockmaw (face management is tank's job), Barim (no need), Siamat Servants (tank pulls them anyway). Death Grip on Soul Fragments in Barim P2 is VALID if Fragment escaped melee gap-close range and ranged hasn't intercepted yet.
