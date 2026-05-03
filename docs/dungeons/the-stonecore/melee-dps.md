# The Stonecore — Melee DPS

**Melee-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how melee DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content melee DPS patterns (positioning, interrupt rotation, threat ceiling) live in `docs/roles/melee-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (combat-rogue, fury-warrior, ret-paladin, frost-dk, enh-shaman, feral-cat)
- This doc layers Stonecore-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_melee_dps_overview:
  total_bosses: 4 (none optional)
  estimated_full_clear_minutes: 25-45
  positioning_profile:
    corborus: behind_boss_spread_5y         # spread for Crystal Barrage; behind for cleave avoidance
    slabhide: behind_boss_dodge_lava_fissure # standard behind, dodge ground fire
    ozruk: behind_boss_run_through_on_slam  # run through on Ground Slam; ≥ 5y for Shatter
    azil: behind_boss_spread_for_spike      # behind in Stage 1; spread Stage 2
  interrupt_load:
    corborus: zero                          # no kicks on boss
    slabhide: zero
    ozruk: zero                             # Ground Slam non-kickable
    azil: high                              # Force Grip every cycle — primary kicker (off-GCD melee kicks)
  key_responsibilities:
    - corborus_spread_5y                    # opposite of BRC Rom'ogg stack
    - lava_fissure_dodge (slabhide)
    - bulwark_NA_to_melee (ozruk, but Spike Shield IS relevant)
    - spike_shield_stop_attacking (ozruk normal) OR allow_1_2_stacks (ozruk heroic)
    - run_through_ground_slam (ozruk)
    - paralyze_break_via_active_dot_or_bleed (ozruk heroic)
    - force_grip_kick_rotation (azil) — PRIMARY KICKER (off-GCD)
    - aoe_devout_followers (azil)
    - millhouse_event_kick_earthshaper (trash)
```

---

## Trash — melee DPS notes

```yaml
trash_melee:
  millhouse_event:
    interrupt_role: |
      THE primary interrupt rotation contributor on this pull. Melee
      kicks are off-GCD on most specs:
      - Combat Rogue: Kick (10s CD)
      - Fury Warrior: Pummel (10s CD; off-GCD)
      - Ret Pally: Rebuke (15s CD; off-GCD)
      - Frost DK: Mind Freeze (10s CD; off-GCD)
      - Enh Shaman: Wind Shear (6s CD; off-GCD; 25y range — basically
        ranged kick, melee-class)
      - Feral Cat: Skull Bash (60s CD; gap-close + interrupt — long CD)
      Earthshaper Force of Earth (81459) is the priority kick (5s cast =
      group wipe).
    cc_priorities: |
      Melee CC contribution:
      - Combat Rogue: Sap (humanoid, opener) on Earthshaper or Berserker
      - Ret Pally: Repentance (humanoid) on Earthshaper
      - Frost DK: NO standard CC (Hungering Cold AoE freeze is talent)
      - Fury Warrior / Enh Shaman / Feral Cat: NO standard CC; rely on
        teammate openers
    notes: |
      Melee-positioning tip: Spinning Slash (Berserker) is a 4s 5y bleed
      AoE. KITE OUT of 5y when Berserker channels — break melee briefly,
      back in after channel ends.

  earthshaper_packs:
    interrupt_role: |
      Force of Earth (81459, 5s cast) is the priority kick. Earthshaper
      also has Lava Burst (81576, 2s cast) which is a softer interrupt
      target — kick Force of Earth FIRST.

  berserker_spinning_slash:
    notes: |
      Channeled bleed AoE; melee bot must KITE OUT of 5y radius for the
      4-second channel. Stand at ranged-position briefly (or move
      perpendicular). After channel ends, re-engage.
      - Combat Rogue: Sprint to recover; CP attached to Berserker
        unaffected by movement
      - Fury Warrior: Heroic Leap to re-engage
      - Ret Pally: Hand of Freedom for self-cleanse if rooted (rare on
        Spinning Slash)
      - Frost DK: Death Grip is overkill; just move
      - Enh Shaman: ranged Lava Burst / Lightning Bolt (talented — Maelstrom
        Weapon procs) during gap
      - Feral Cat: Stampeding Roar for raid speed; or just move

  magmalord_packs:
    notes: |
      Standard melee tank-and-spank. Magma Eruption ground patches —
      sidestep ground fire when targeted.

  flayer_packs:
    notes: |
      Flay AoE; positioning side/rear minimizes damage.
```

---

## Boss 1 — Corborus

```yaml
boss_melee:
  boss: corborus
  difficulty_grade: easy (positional)
  positioning:
    default: |
      Behind boss; SPREAD ≥ 5y from other melee. Crystal Barrage 5y
      radius means even melee must spread (different from default
      melee-stack-with-tank rule).
    facing: boss back
    movement: |
      Ground phase: behind boss; spread. Burrow phase: dodge dust-cloud
      Thrashing Charge telegraphs (move PERPENDICULAR to charge axis).
      Crystal Barrage on you: move 5y+ off the splash ground.
  interrupt_role:
    none: no kick-required casts
  rotation_modifications:
    crystal_barrage_target_self: |
      Move out; continue rotation while moving:
      - Combat Rogue: SnD / RvS instant; CP build via SS continues; CP
        attached to boss unaffected
      - Fury Warrior: Heroic Leap to relocate; rage continues building
      - Ret Pally: Holy Power continues; CS / TV instant; HoW execute
        if HP < 20%
      - Frost DK: Diseases continue ticking; Obliterate / Howling Blast
        / Frost Strike all instant
      - Enh Shaman: Stormstrike / Lava Lash instant; MW procs continue
      - Feral Cat: Energy / CP continue; Bear-form-shift not needed
    burrow_phase: |
      Boss unattackable; switch to AoE on Rock Borers + Crystal Shards
      (heroic):
      - Combat Rogue: Blade Flurry on; Fan of Knives at 3+ targets
      - Fury Warrior: Whirlwind / Bladestorm; Cleave on add cluster
      - Ret Pally: Divine Storm at 3 HP; Consecration / HoW filler
      - Frost DK: Howling Blast spam (Frost spec); Pestilence to spread
        diseases
      - Enh Shaman: Magma Totem + Chain Lightning at 5 MW; Fire Nova
        through Searing Totem
      - Feral Cat: Swipe (Cat); maintain Rip on whichever Rock Borer
        the bot picks as primary
    thrashing_charge_dodge: |
      Watch dust-cloud telegraph; if a charge is going to clip you,
      gap-close back to boss after the eruption resolves.
  cd_use:
    standard_offensive: rotational; align bursts with ground phase if possible
    save_for_real_bosses: minimal CD save needed
  heroic_delta: |
    - Crystal Shards spawn during Crystal Barrage — AoE rotation more important
    - Thrashing Charge damage scales hard; direct hit one-shot
    - Crystal Barrage damage radius unchanged but tick damage scales
  bot_anti_patterns:
    - DO NOT stack with other melee (Crystal Barrage 5y splash)
    - DO NOT chase boss into Burrow ground (he's underground)
    - DO NOT melee Crystal Shards on heroic (they explode on contact — let ranged AoE)
    - DO NOT follow Crystal Barrage target into the splash zone
```

---

## Boss 2 — Slabhide

```yaml
boss_melee:
  boss: slabhide
  difficulty_grade: medium
  positioning:
    default: |
      Behind boss; OUTSIDE 60° forward Sand Blast cone. During AIR
      PHASE, dodge stalactite ground textures; pre-position near a
      pillar pair for heroic Crystal Storm cover.
    facing: boss back
    movement: |
      Ground phase: behind boss; dodge Lava Fissure. Air phase: dodge
      Stalactite ground textures; pre-position. Crystal Storm channel
      (heroic): full LoS-break behind a stalactite pillar.
  interrupt_role:
    none: no kick-required casts
  rotation_modifications:
    standard_ground_phase: |
      Standard ST rotation. Sand Blast cone from boss FRONT — melee is
      already behind, so cone-safe.
      - Combat Rogue: SS builder + Eviscerate at 5 CP; SnD maint
      - Fury Warrior: BT/CS/RB rotation; Slam on Bloodsurge proc
      - Ret Pally: Inq maint; CS on CD; TV at 3 HP; HoW execute
      - Frost DK: Pillar of Frost on CD; Obliterate / Frost Strike
      - Enh Shaman: Stormstrike / Lava Lash; Maelstrom Weapon procs
      - Feral Cat: Tiger's Fury; Rake/Rip maint; Mangle on cooldown
    lava_fissure_dodge: |
      5s normal / 3s heroic ground crack telegraph on a player. If on
      YOU, move 5y+ off the crack. If on TEAMMATE, position around them
      (don't follow them off-boss unnecessarily).
    air_phase: |
      Boss is in the air; melee bot has NO target (boss is targetable
      but unreachable for melee — flying out of melee range).
      - Combat Rogue: thrown weapon + Killing Spree gap-close (UNVERIFIED
        if Killing Spree gap-closes vs flying target)
      - Fury Warrior: Heroic Throw (single ranged attack); Charge does
        not work vs flying
      - Ret Pally: Exorcism ranged; Hammer of Wrath if execute window
      - Frost DK: Death Coil (RP dump); Howling Blast at range; no
        melee while flying
      - Enh Shaman: Lava Burst hardcast (Maelstrom Weapon if proc-stacked);
        Lightning Bolt
      - Feral Cat: NO ranged attack; wait for boss to land. Keep CP
        on bear-form Rip if applicable (Cat doesn't have ranged).
      Movement during air phase: dodge Stalactite ground textures.
    crystal_storm_HEROIC: |
      6s LoS-break behind stalactite pillar. NO DPS during channel.
      Survival is the ONLY priority. Resume melee after channel ends.
  cd_use:
    standard_offensive: rotational
  heroic_delta: |
    - Crystal Storm 6s no-DPS window per air-phase cycle
    - Lava Pool 30s persistence — ground hazard accumulates
    - Lava Fissure 3s telegraph (vs 5s) — less reaction time
  bot_anti_patterns:
    - DO NOT stand in Sand Blast cone (60° forward arc)
    - DO NOT stand in Lava Fissure crack
    - DO NOT chase boss into the air (you can't melee it anyway)
    - DO NOT skip the Crystal Storm LoS-break on heroic
    - DO NOT stand in Lava Pool (persistent fire ground)
```

---

## Boss 3 — Ozruk

```yaml
boss_melee:
  boss: ozruk
  difficulty_grade: hard (heroic); medium (normal)
  positioning:
    default: |
      Behind boss; RUN THROUGH BOSS during Ground Slam (DO NOT just
      sidestep — the AoE radius is 4y in front, and "behind" is
      literally behind the boss model, not 4y back); STEP TO ≥ 5y from
      boss at the 9-second mark of Spike Shield (1s before Shatter).
    facing: boss back
    movement: |
      Continuous awareness of the Spike Shield + Bulwark cycle. Default
      melee positioning (behind) is correct; the deviations are:
      - Ground Slam 3s cast → run through boss
      - Spike Shield expires (≈10s mark) → step to ≥ 5y
      - Bulwark — no melee deviation (Bulwark is spell-reflect; melee unaffected)
  interrupt_role:
    none: Ground Slam non-kickable
  rotation_modifications:
    spike_shield_phase_NORMAL: |
      STOP MELEE for the 10s duration. The bleed (300/sec) compounds
      pointlessly; pause melee.
      - Combat Rogue: Stop SS; CP attached to boss preserved; SnD
        timer ticking — refresh BEFORE shield expires
      - Fury Warrior: Stop BT/CS/RB; rage banking
      - Ret Pally: Stop CS/TV; Inquisition timer ticking — refresh
        BEFORE shield expires
      - Frost DK: Stop Obliterate/Frost Strike; runes regenerating
      - Enh Shaman: Stop Stormstrike/Lava Lash; MW procs hold
      - Feral Cat: Stop SS/FB; Energy capping — Tiger's Fury still
        usable for Energy regen during pause
      RANGED INSTANTS allowed (none of the listed melee specs have
      ranged-DPS spell rotations though, so this is mostly N/A).
    spike_shield_phase_HEROIC: |
      ALLOW 1-2 BLEED STACKS DELIBERATELY so the bleed DoT ticks
      during Paralyze (8s stun applied at Shatter). This breaks
      Paralyze on first tick.
      - Combat Rogue: 1-2 SS hits = 1-2 stacks; then pause melee
      - Fury Warrior: 1-2 melee hits; then pause
      - Ret Pally: 1-2 CS = 1-2 stacks; then pause (Censure DoT also
        ticks — Ret has built-in passive Paralyze break)
      - Frost DK: 1-2 Obliterate hits OR rely on diseases (Frost
        Fever, Blood Plague tick during Paralyze — pre-applied
        diseases ARE the break)
      - Enh Shaman: 1-2 Stormstrike hits; then pause
      - Feral Cat: 1-2 Mangle/Shred; Rip and Rake DoTs tick during
        Paralyze — pre-applied Rip IS the break
    bulwark_phase: |
      NOT A MELEE CONCERN. Bulwark is spell-reflect; melee attacks are
      not reflected. Continue full melee rotation.
    shatter_step_out: |
      At Spike Shield 9s mark, step to ≥ 5y. Move 5+ yards from boss;
      gap-close back after Shatter resolves.
      - Combat Rogue: Sprint to gap-close back
      - Fury Warrior: Charge to gap-close back
      - Ret Pally: walk back (no gap-close — plan distance)
      - Frost DK: Death's Advance (talented — UNVERIFIED Cata) speed buff
      - Enh Shaman: Spiritwalker's Grace (talented — UNVERIFIED Cata)
      - Feral Cat: Travel Form / Stampeding Roar for movement
    ground_slam_response: |
      3s cast = 3s window to run THROUGH boss. Use whichever movement
      tool is available:
      - Combat Rogue: Sprint or just walk (3s is plenty)
      - Fury Warrior: Charge through (Heroic Leap also works if it
        targets behind boss)
      - Ret Pally: walk through
      - Frost DK: Death's Advance speed if specced
      - Enh Shaman: Ghost Wolf
      - Feral Cat: Stampeding Roar (raid-wide)
  cd_use:
    save_for_clean_dps_window: |
      Major CDs (Bloodlust, AW, Berserk, Pillar of Frost, etc.) align
      to Bulwark-up, Spike-Shield-down windows on heroic — caster
      damage drops, your relative contribution rises.
  heroic_delta: |
    - Spike Shield bleed 600/sec (vs 300/sec) — even 1-2 stacks more
      damaging
    - Paralyze break is DoT-tick driven; melee bleeds + class DoTs
      contribute
    - Rupture (post-Ground Slam) — knockback up; sidestep front arc
    - Bulwark 100% reflect doesn't affect melee but caster damage
      drops
  bot_anti_patterns:
    - DO NOT continue full melee during Spike Shield (NORMAL) — bleed pile-up wastes HP
    - DO NOT stop melee fully during Spike Shield (HEROIC) — need 1-2 stacks for Paralyze break OR rely on class DoT
    - DO NOT stand in 4y front arc during Ground Slam (run through)
    - DO NOT stand within 5y of boss when Spike Shield expires (Shatter AoE)
    - DO NOT use Death Grip on Ozruk (interferes with tank kite/wall positioning)
```

---

## Boss 4 — High Priestess Azil

```yaml
boss_melee:
  boss: high_priestess_azil
  difficulty_grade: hard
  positioning:
    default: |
      Behind boss in Stage 1; SPREAD widely in Stage 2. Stage 1
      melee-on-boss; Stage 2 transition to AoE on Devout Followers (boss
      is 75% mitigated, melee on her is wasted).
    facing: boss back (Stage 1) / Devout Followers (Stage 2)
    movement: |
      Stage 1: minimal; respond to Gravity Well by moving OUT. Stage 2:
      continuous Seismic Shard sidestep + Devout Follower AoE.
  interrupt_role:
    force_grip: PRIMARY KICKER — melee kicks are off-GCD and short CD; kick rotation lead
    rotation_partners: [tank, melee_dps_HEAVY, ranged_dps_with_kick, healer_with_kick]
  rotation_modifications:
    stage_1_force_grip: |
      EVERY CAST KICKED. Force Grip cycles every ~6s; melee bot kicks
      first when off CD.
      - Combat Rogue: Kick (10s) — alternate with another kicker
      - Fury Warrior: Pummel (10s) — alternate
      - Ret Pally: Rebuke (15s) — slower CD; second-string kicker
      - Frost DK: Mind Freeze (10s) — alternate
      - Enh Shaman: Wind Shear (6s; range) — every cast solo if needed
      - Feral Cat: Skull Bash (60s) — emergency kick only; long CD
    devout_follower_aoe: |
      Add waves spawn 1-3 times per Stage 1; 9-12 cultists per wave.
      AoE rotation:
      - Combat Rogue: Blade Flurry on; Fan of Knives spam (3+ targets)
      - Fury Warrior: Whirlwind / Bladestorm; Cleave through adds
      - Ret Pally: Divine Storm at 3 HP; Consecration on add-spawn
        location; HoW on adds in execute
      - Frost DK: Howling Blast spam; Pestilence diseases on adds
      - Enh Shaman: Magma Totem; Fire Nova through Searing Totem;
        Chain Lightning at 5 MW
      - Feral Cat: Swipe (Cat); maintain Rip on a single follower as
        primary (poor for AoE — Cat is single-target-leaning)
    stage_2_response: |
      Boss flies to altar; Energy Shield (75% mitigation). Melee bot
      transitions:
      - DPS Devout Followers (boss is wasted DPS at 75% mitigation)
      - Sidestep Seismic Shards (3.5y radius; visible telegraph)
      - Avoid altar area (Energy Shield knockback on creation hits 5y)
      - On heroic, FUNNEL Devout Followers through Gravity Wells
        (each kill shrinks well 25%)
    gravity_well_dodge: |
      10y pull radius; move OUT of pull (push outward against pull
      direction). Gap-closer back to boss after escape:
      - Combat Rogue: Sprint
      - Fury Warrior: Charge / Heroic Leap
      - Ret Pally: walk (no gap-close)
      - Frost DK: Death's Advance (UNVERIFIED) or just walk
      - Enh Shaman: Ghost Wolf
      - Feral Cat: Travel Form
    curse_of_blood_on_self: |
      No melee class can cleanse Curse (except Ret Pally Cleanse —
      but Cleanse is Holy Pally; Ret has Cleanse as well per spec? UNVERIFIED).
      Default: rely on healer or curse-dispel teammate. Continue
      rotation.
  cd_use:
    standard_offensive: rotational; align with Stage 1 (clean DPS uptime); avoid Stage 2 (75% mitigation wastes burst)
  heroic_delta: |
    - More Devout Followers per wave — AoE rotation more important
    - Gravity Wells require kills to shrink — bait adds through
    - Seismic Shard direct hit wipe-tier — extra sidestep margin
    - Force Grip more punishing if kick missed — kick rotation tight
  bot_anti_patterns:
    - DO NOT skip Force Grip kick rotation (tank takes 70k+ slam per tick)
    - DO NOT stand in Gravity Well (pull mechanic; ramp damage)
    - DO NOT cluster with other melee (Seismic Shard cross-hit + Devout cleave)
    - DO NOT use major DPS CD in Stage 2 (75% mitigation wastes)
    - DO NOT continue boss DPS in Stage 2 (focus Devout Followers)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  corborus:
    - encounter_state_active → spread_5y_behind_boss
    - encounter_detect_crystal_barrage_target_self → move_out_of_5y_splash
    - encounter_detect_burrow_phase → swap_to_aoe_rotation
    - encounter_detect_thrashing_charge_telegraph → move_perpendicular_to_charge_axis

  slabhide:
    - encounter_state_active → behind_boss_outside_cone
    - encounter_detect_lava_fissure_target_self → move_5y+_off_crack
    - encounter_detect_air_phase → wait_for_boss_landing (no melee target available)
    - encounter_detect_crystal_storm_cast_HEROIC → move_to_full_pillar_los_break

  ozruk:
    - encounter_detect_spike_shield_aura_NORMAL → stop_melee
    - encounter_detect_spike_shield_aura_HEROIC → allow_1_2_stacks_then_pause
    - encounter_detect_spike_shield_9s → step_to_5y
    - encounter_detect_ground_slam_cast → run_through_boss
    - encounter_detect_paralyze_applied_self → ensure_active_DoT_or_bleed_for_break
    - encounter_state_active → maintain_rear_arc_position (default melee behavior)

  azil:
    - encounter_detect_force_grip_cast → KICK (melee kicks PRIMARY rotation)
    - encounter_detect_devout_follower_spawn → swap_to_aoe_rotation
    - encounter_detect_gravity_well_target → move_out_of_pull
    - encounter_detect_stage_2_transition → switch_target_to_devout_followers
    - encounter_detect_seismic_shard_cast → sidestep_telegraph

dispel_priority_per_fight:
  corborus:    []
  slabhide:    []
  ozruk:       []
  azil:        []                               # melee classes lack curse-cleanse generally

interrupt_priority_per_fight:
  corborus:    []
  slabhide:    []
  ozruk:       []                               # Ground Slam non-kickable
  azil:        [{ spell_id: 79351, name: Force Grip, urgency: HIGHEST, melee_role: PRIMARY_KICKER }]

dispel_blacklist:
  - { spell_id: 92426, name: Paralyze (Ozruk heroic), reason: prefer DoT-damage-break over dispel; melee bleeds + class DoTs ARE the break mechanism, scope: ozruk_fight }
  # melee bots typically don't dispel; included for completeness on Ret Pally Cleanse and Enh Shaman Cleanse Spirit (talented)
```
