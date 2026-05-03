# The Stonecore — Ranged DPS

**Ranged-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how ranged DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content ranged DPS patterns (positioning, spread/stack rules, threat ceiling) live in `docs/roles/ranged-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (frost-mage, affliction-warlock, mm-hunter, shadow-priest, balance-druid, elemental-shaman)
- This doc layers Stonecore-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_ranged_dps_overview:
  total_bosses: 4 (none optional)
  estimated_full_clear_minutes: 25-45
  positioning_profile:
    corborus: spread_5y_min                 # Crystal Barrage 5y radius — must spread (UNLIKE Rom'ogg/BRC stack)
    slabhide: max_range_outside_cone        # standard ranged behavior
    ozruk: max_range_step_out_for_shatter   # 5y step-out at 9s of Spike Shield; suspend hardcasts during Bulwark
    azil: max_range_spread_stage_2          # spread for Seismic Shard; SPREAD throughout for Gravity Well baiting
  interrupt_load:
    corborus: zero                          # no kicks on boss
    slabhide: zero
    ozruk: zero                             # Ground Slam is non-kickable
    azil: high                              # Force Grip every cycle — ranged participates if class has kick
  dispel_load:
    corborus: zero (healer-only role)
    slabhide: zero
    ozruk: zero
    azil: high_for_curse_dispellers         # Mages, Boomkins, Druids, Shamans (talented) cleanse Curse of Blood
  key_responsibilities:
    - corborus_spread_5y                    # opposite of BRC Rom'ogg stack
    - crystal_shard_aoe_heroic (corborus)
    - crystal_storm_los_break_HEROIC (slabhide)
    - bulwark_stop_hardcasts (ozruk)
    - paralyze_break_via_active_dot (ozruk heroic)
    - force_grip_kick_rotation (azil)
    - curse_of_blood_dispel_for_curse_classes (azil)
```

---

## Trash — ranged DPS notes

```yaml
trash_ranged:
  millhouse_event:
    interrupt_role: |
      Ranged contributes interrupt rotation:
      - Frost Mage: Counterspell (24s) on Earthshaper Force of Earth
      - Affliction Warlock: Spell Lock via Felhunter pet
      - MM Hunter: Silencing Shot (24s)
      - Shadow Priest: Silence (45s)
      - Balance Druid: Solar Beam (60s; 8s AoE silence — covers entire pull)
      - Elemental Shaman: Wind Shear (6s; off-GCD; 25y range — best per-cast cooldown)
      EARTHSHAPER FORCE OF EARTH IS HIGHEST PRIORITY KICK in this pull.
      5s cast = group wipe if missed.
    cc_priorities: |
      Ranged contributes opener-CC:
      - Frost Mage: Polymorph (Sheep) on Earthshaper — primary CC target
      - Affliction Warlock: Banish (UNVERIFIED on Earthshaper elemental
        form post-transformation); Fear pre-transformation (humanoid?)
      - MM Hunter: Freezing Trap on Berserker
      - Shadow Priest: Shackle Undead (NOT applicable; Stonecore mobs
        are humanoid/elemental, not undead) — fall back to Mind Control
        (rare) or Silence rotation
      - Balance Druid: Cyclone (8s)
      - Elemental Shaman: Hex (frog 60s; humanoid CC on Berserker)
    notes: |
      THE HARDEST PULL OF THE DUNGEON. Multi-CC required; ranged kick
      rotation needed for Earthshaper Force of Earth.

  earthshaper_packs:
    interrupt_role: |
      Force of Earth (81459) is a 5s cast — ALL ranged with kick MUST
      participate in the rotation. Failure = group wipe.

  caster_packs_general:
    interrupt_role: |
      Earthshaper Lava Burst (81576, 2s cast) — interruptible
      Magmalord Ignite (UNVERIFIED) — interruptible likely
      Twilight cultists hardcasts — interruptible
    notes: |
      Standard interrupt rotation; not as critical as Earthshaper Force
      of Earth.

  berserker_spinning_slash:
    notes: |
      4s channel; ranged is naturally clear of the 5y radius. Continue
      DPS from outside.

  positioning:
    notes: |
      Ranged stays at max range (30y+) on most pulls. The pre-Slabhide
      ramp is narrow — be careful of mob aggro pulling additional packs.
```

---

## Boss 1 — Corborus

```yaml
boss_ranged:
  boss: corborus
  difficulty_grade: easy (positional)
  positioning:
    default: |
      SPREAD ≥ 5 yards from all other players. Crystal Barrage radius
      is 5y; spread prevents multi-player hits. THIS IS OPPOSITE of
      Rom'ogg (BRC) where ranged stacked with melee.
    facing: boss (cast direction)
    movement: |
      Ground phase: minimal — spread, then DPS. Burrow phase: watch
      dust-cloud telegraphs and move PERPENDICULAR to charge axis.
      Crystal Barrage targeted on you: move out of the 5y splash
      ground.
  interrupt_role:
    none: no kick-required casts on boss
  rotation_modifications:
    crystal_barrage_target_on_self: |
      Move out of the 5y splash; continue DPS while moving:
      - Frost Mage: Ice Lance instant; Frostbolt hardcast (cancel only
        if needed for movement); Frostfire Bolt during Brain Freeze
      - Affliction Warlock: DoTs (Corruption / Bane of Agony / Unstable
        Affliction / Haunt) tick during movement; channeled Drain Soul
        breaks
      - MM Hunter: Steady Shot (hardcast — cancel if moving) / Aimed
        Shot (hardcast); Auto Shot continues; Cobra Shot N/A (MM)
      - Shadow Priest: DoTs continue; Mind Flay channels are mobility-
        friendly; Mind Blast hardcast (cancel briefly)
      - Balance Druid: Wrath/Starfire hardcast (cancel briefly);
        Moonfire / Sunfire instant DoT continues
      - Elemental Shaman: Lightning Bolt / Lava Burst hardcast (cancel
        briefly); Flame Shock instant DoT continues
    burrow_phase: |
      AoE rotation on Rock Borers + Crystal Shards (heroic). Switch
      back to single-target Corborus on resurface.
      - Frost Mage: Blizzard / Frostfire Orb on Rock Borer cluster
      - Aff Warlock: Seed of Corruption + Hellfire if no DoT-spread
        tools available
      - MM Hunter: Multi-Shot + Trap Launcher Explosive Trap
      - Shadow Priest: Mind Sear (poor below 4 targets); Vampiric
        Touch + SW:P spread
      - Balance Druid: Hurricane (channel) / Wild Mushroom
      - Ele Shaman: Magma Totem at boss feet (NOT in the dust telegraph
        though); Chain Lightning at 5+ MW (UNVERIFIED Cata enhancement
        of Chain Lightning); Thunderstorm — POSITIONAL HAZARD, do NOT
        cast Thunderstorm in Burrow phase (knockback could push players
        into Thrashing Charge path)
    crystal_shards_HEROIC: |
      Heroic adds: 7.5k HP each, spawn every 0.5s during Crystal
      Barrage. Ranged AoE clears them before they reach players.
      Priority: Crystal Shards > Rock Borers > Corborus DPS during
      Burrow.
  cd_use:
    save_for_real_bosses: minimal CD use on Corborus
    standard_offensive: rotational
  heroic_delta: |
    - Crystal Shards add AoE-burn pressure
    - Crystal Barrage damage scales hard; spread is non-optional
  bot_anti_patterns:
    - DO NOT stack with other ranged (Crystal Barrage 5y splash)
    - DO NOT use AoE-knockback during Burrow phase (Thunderstorm pushes targets into Thrashing Charge)
    - DO NOT continue DPS on Corborus during Burrow (he's unattackable)
    - DO NOT melee-range Crystal Shards (they explode on contact — AoE from outside)
```

---

## Boss 2 — Slabhide

```yaml
boss_ranged:
  boss: slabhide
  difficulty_grade: medium (heroic; easy normal)
  positioning:
    default: |
      Max range (30y+); OUT of Sand Blast 60° cone arc; OFF Lava
      Fissure ground patches. During AIR PHASE, position near a
      stalactite pillar that will be your LoS-cover for heroic Crystal
      Storm.
    facing: boss
    movement: |
      Ground phase: minimal — DPS from max range. Air phase: dodge
      Stalactite ground textures; pre-position near a pillar pair.
      Lava Fissure on you: 5s normal / 3s heroic; move 5y+ off the
      crack.
  interrupt_role:
    none: no kick-required casts (Sand Blast / Lava Fissure are instant; Crystal Storm is mitigated by LoS not interrupt)
  rotation_modifications:
    standard: |
      Standard ST rotation from max range. Sand Blast cone determines
      safe arc: stay BEHIND-ish (60° forward arc means anything > 30°
      from boss-front is safe).
    air_phase: |
      Continue ranged DPS through air phase if line-of-sight
      maintained. Slabhide is targetable in the air.
      - Frost Mage: continue Frostbolt rotation
      - Aff Warlock: DoTs continue; Drain Soul interruption normal
      - MM Hunter: Aimed Shot / Steady Shot (boss is in range)
      - Shadow Priest: DoT/Mind Flay continue
      - Balance Druid: Eclipse rotation continues
      - Ele Shaman: LB/LvB continue; Flame Shock refresh
    crystal_storm_HEROIC: |
      6s LoS-break during channel = 6s of zero DPS-output if you
      fully break LoS (cast bar can't resolve through pillar). Two
      strats:
      - Strict LoS-break: stand fully behind pillar; survive fully;
        DPS resumes at channel end. SAFE option, recommended for bot.
      - Risky DPS-through: hover at pillar edge; eat partial ticks for
        DPS time. NOT recommended for bot logic.
      Bot strategy: SAFE option — full LoS-break for the 6 seconds.
  cd_use:
    save_major_for_phase_2: |
      DPS cooldowns (Bloodlust, Combustion, Berserk, etc.) align to
      ground phase if possible (boss takes more damage uninterrupted
      than during stalactite-spawn air phase).
  heroic_delta: |
    - Crystal Storm 6s LoS-break is mandatory survival mechanic
    - Lava Pool persistence (30s vs 10s) — ground hazards accumulate
    - Sand Blast cone damage scales hard
  bot_anti_patterns:
    - DO NOT stand in Sand Blast cone (60° forward, 15y range)
    - DO NOT stand in Lava Fissure crack
    - DO NOT cast through stalactite LoS (your hardcast hits pillar = wasted GCD)
    - DO NOT skip the Crystal Storm LoS-break on heroic — full pillar cover for 6s
    - DO NOT cluster with other ranged (Lava Fissure splash + Stalactite cluster damage)
```

---

## Boss 3 — Ozruk

```yaml
boss_ranged:
  boss: ozruk
  difficulty_grade: hard (heroic); medium (normal)
  positioning:
    default: |
      Max range (30y+); RUN THROUGH BOSS during Ground Slam (or sidestep
      to ≥ 4y outside front arc); STEP TO ≥ 5y from boss at the 9-second
      mark of Spike Shield (1s before Shatter resolves).
    facing: boss (cast direction during DPS)
    movement: |
      Continuous awareness of the Spike Shield + Bulwark cycle:
      - Spike Shield up → continue ranged DPS (NO bleed for ranged in
        Cata? UNVERIFIED — sources are ambiguous. Caster spell hits MIGHT
        proc the bleed. Treat conservatively: DoTs already on boss are
        safe; new spell hits MAY proc bleed. Default = continue DPS but
        prefer DoT maintenance over new spell casts.)
      - Bulwark up → STOP HARDCASTS (100% spell reflect on heroic)
      - Spike Shield 9s → STEP OUT to ≥ 5y
      - Ground Slam 3s cast → run through boss / sidestep front arc
  interrupt_role:
    none: Ground Slam non-interruptible
  rotation_modifications:
    spike_shield_phase: |
      Caster classes — DoT maintenance only. Don't queue new hardcast
      spells; the existing DoT damage continues to tick safely
      regardless of shield. Hunter — auto-shot continues; main
      bottleneck is Steady Shot/Aimed Shot generation.
      - Frost Mage: cancel queued Frostbolt; Ice Lance instant safe;
        DoTs (Living Bomb if Fire — not Frost spec) continue
      - Affliction Warlock: DoTs CONTINUE TICKING (this is the spec's
        ideal scenario); avoid new direct-damage casts (Shadow Bolt,
        Drain Soul). Bane of Agony, Corruption, Unstable Affliction,
        Haunt all tick safely. UNVERIFIED whether re-casting an
        expiring DoT during Spike Shield triggers the bleed proc.
      - MM Hunter: ranged non-spell; Spike Shield bleed proc
        UNVERIFIED for hunter ranged auto-shot. Stop Aimed/Steady to
        be safe; let Auto Shot continue.
      - Shadow Priest: VT, SW:P, DP DoTs continue; stop Mind Flay
        (channel = repeated hits) and Mind Blast (hardcast)
      - Balance Druid: Moonfire/Sunfire DoTs continue; stop Wrath /
        Starfire hardcasts
      - Ele Shaman: Flame Shock DoT continues; stop Lightning Bolt /
        Lava Burst hardcasts
    bulwark_phase: |
      Caster classes — STOP HARDCASTS. 100% spell reflect on heroic =
      self-damage. Continue ONLY existing DoT ticks (which are not
      reflected — only new casts are).
      - Frost Mage: Ice Lance instant — UNVERIFIED whether instants
        bypass reflect. Conservative: stop ALL spell casts.
      - Affliction Warlock: DoTs continue; stop new applications
      - MM Hunter: NOT AFFECTED by spell reflect (physical attacks);
        continue rotation
      - Shadow Priest: DoTs continue; stop Mind Blast, Mind Flay
      - Balance Druid: DoTs continue; stop Wrath / Starfire
      - Ele Shaman: stop ALL nature/lightning casts; auto-attack only
    paralyze_break_HEROIC: |
      The pre-applied DoT IS the Paralyze break mechanism. Bot
      strategy: ENSURE DoT IS ACTIVE on the boss before Spike Shield
      9-second mark.
      - Frost Mage: NO native DoT in Frost spec. Bot relies on healer
        HoT for own Paralyze break; doesn't contribute to boss-DoT
        for self-break.
      - Affliction Warlock: BEST CLASS for this fight — Corruption +
        Bane of Agony + Unstable Affliction + Haunt all tick during
        Paralyze; first tick breaks. ALWAYS CHECK DoT uptime before
        Spike Shield expires.
      - MM Hunter: Serpent Sting — apply BEFORE Spike Shield. Hunter
        is well-suited.
      - Shadow Priest: VT + SW:P always active; first tick breaks.
        STRONG SUPPORT.
      - Balance Druid: Moonfire + Sunfire — apply both before Spike
        Shield expires; one ticks during Paralyze.
      - Ele Shaman: Flame Shock — apply before Spike Shield. Less DoT
        density than Aff/Spriest; relies on Flame Shock alone.
    shatter_step_out: |
      At 9s of Spike Shield (1s before Shatter), step to ≥ 5y. Most
      ranged are already at 30y+ from boss; this is automatic. Confirm
      no positioning drift.
  cd_use:
    save_for_optimal_window: |
      Bloodlust / Pre-Pot / Personal CDs ideally pre-pull or align to
      the first non-Bulwark window (clean DPS uptime).
  heroic_delta: |
    - Paralyze adds 8s stun — DoT-active discipline is mandatory
    - Bulwark 100% reflect (vs 20%) — caster damage drops; class with
      DoT-heavy rotation (Aff Warlock, Shadow Priest) maintains throughput
    - Rupture (post-Ground Slam) — knockback up; sidestep front arc
  bot_anti_patterns:
    - DO NOT cast hardcasts during Bulwark (100% reflect = self-damage on heroic)
    - DO NOT stand in 4y front arc during Ground Slam (run through or sidestep)
    - DO NOT stand within 5y of boss when Spike Shield expires (Shatter AoE)
    - DO NOT let DoTs expire before Spike Shield 9-second mark on heroic (Paralyze break depends on DoT tick)
    - DO NOT use Thunderstorm (Ele Shaman) — no benefit on this fight; positional hazard
```

---

## Boss 4 — High Priestess Azil

```yaml
boss_ranged:
  boss: high_priestess_azil
  difficulty_grade: hard
  positioning:
    default: |
      Max range; SPREAD throughout (Gravity Well-bait + Seismic Shard
      Stage 2 + Devout Follower cleave). Stage 1 max range; Stage 2
      spread widely (3 Seismic Shards target separate ground spots).
    facing: active boss/clone unit
    movement: |
      Stage 1 — minimal; spread; respond to Gravity Well by moving OUT
      of pull. Stage 2 — spread WIDE; sidestep individual Seismic Shard
      tombstones; avoid altar area (Energy Shield knockback on creation).
  interrupt_role:
    force_grip: PARTICIPATE if class has kick (HIGHEST priority kick of dungeon)
    rotation_partners: [tank, melee_dps, ranged_dps_with_kick, healer_with_kick]
  rotation_modifications:
    stage_1: |
      Standard ST rotation on Azil. Switch to AoE on Devout Follower
      add waves (9-12 cultists per wave; 1-3 waves per Stage 1).
      - Frost Mage: Blizzard on add cluster; Frost Nova for root + AoE
      - Aff Warlock: Seed of Corruption (best AoE with humanoid adds);
        DoT-spam if bot doesn't have Seed-AoE primer
      - MM Hunter: Multi-Shot + Explosive Trap; Aimed Shot / Steady on
        boss between add waves
      - Shadow Priest: VT + SW:P spread; Mind Sear if 4+ adds
      - Balance Druid: Hurricane / Wild Mushroom on adds; Moonfire spam
        if Eclipse-bar friendly
      - Ele Shaman: Magma Totem + Chain Lightning at 5+ MW; LB on boss
    force_grip_kick: |
      EVERY CAST must be kicked. Bot kick rotation:
      - Frost Mage: Counterspell (24s)
      - Aff Warlock: Spell Lock (Felhunter pet, 24s)
      - MM Hunter: Silencing Shot (24s)
      - Shadow Priest: Silence (45s)
      - Balance Druid: Solar Beam (60s; AoE silence)
      - Ele Shaman: Wind Shear (6s — best per-cast cooldown; primary
        kicker on this fight)
      Force Grip cycles ~6s; bot rotation must alternate kicks.
    curse_of_blood_dispel: |
      For curse-cleansing classes:
      - Frost Mage: Remove Curse — DISPEL ON DETECTION
      - Balance Druid: Remove Corruption (talented; UNVERIFIED Cata talent
        path) — DISPEL ON DETECTION
      - Ele Shaman: Cleanse Spirit (talented Improved Cleanse Spirit)
        — DISPEL ON DETECTION if specced
      Holy/Disc Priest, Aff Warlock, MM Hunter, Shadow Priest: NO
      curse cleanse — defer to teammate.
    stage_2: |
      Boss is 75% mitigated; Devout Followers spawn; Seismic Shards
      target ground.
      - Continue AoE on Devout Followers (boss DPS is wasted vs 75%
        mitigation)
      - Sidestep Seismic Shard tombstones (3.5y radius; visible
        telegraph)
      - On heroic, FUNNEL Devout Followers through Gravity Wells if
        wells are still alive — each kill shrinks well 25%
      - Energy Shield creation hits 5y radius — be ≥ 5y from altar
        when boss begins lifting
  cd_use:
    standard_offensive: rotational; align with stage 1 for clean Azil DPS
    save_for_phase_1: |
      Major DPS CDs (Bloodlust, Combustion, Berserk, etc.) ideally
      ground-phase only (Stage 1) — Stage 2's 75% mitigation wastes
      offensive bursts.
  heroic_delta: |
    - Gravity Wells shrink only by killing units — adjust positioning
      to bait Devout Followers through wells
    - More Devout Followers per wave — AoE-rotation more important
    - Seismic Shard direct hit wipe-tier — sidestep with extra margin
    - Force Grip more punishing if missed — kick rotation tight
    - Curse of Blood applied more frequently — dispel cooldown
      management for curse classes
  bot_anti_patterns:
    - DO NOT skip Force Grip kick rotation (group wipe risk)
    - DO NOT stand in Gravity Well center (ramp damage; pull mechanic)
    - DO NOT cluster (Seismic Shard cross-hit + Devout Follower cleave)
    - DO NOT stand ≤ 5y from altar during Stage 2 transition (Energy Shield knockback + arcane damage)
    - DO NOT continue DPS on boss in Stage 2 (75% mitigated — focus adds)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  corborus:
    - encounter_state_active → maintain_spread_5y
    - encounter_detect_crystal_barrage_target_self → move_out_of_5y_splash
    - encounter_detect_burrow_phase → swap_to_aoe_rotation_for_rock_borers
    - encounter_detect_crystal_shard_spawn_HEROIC → aoe_priority_target

  slabhide:
    - encounter_state_active → max_range_outside_cone
    - encounter_detect_lava_fissure_target_self → move_5y+_off_crack
    - encounter_detect_air_phase → pre_position_near_stalactite_pillar
    - encounter_detect_crystal_storm_cast_HEROIC → move_to_full_pillar_los_break

  ozruk:
    - encounter_detect_spike_shield_aura_on_boss → continue_dot_maintenance_only_no_new_casts
    - encounter_detect_bulwark_aura_on_boss → suspend_ALL_hardcasts
    - encounter_detect_spike_shield_9s → step_to_5y_or_more
    - encounter_detect_ground_slam_cast → run_through_boss_or_sidestep_front_arc
    - encounter_state_active_HEROIC → maintain_dot_uptime_for_paralyze_break

  azil:
    - encounter_detect_force_grip_cast → kick_if_class_has_kick
    - encounter_detect_curse_of_blood_application → dispel_if_class_has_remove_curse
    - encounter_detect_devout_follower_spawn → swap_to_aoe_rotation
    - encounter_detect_gravity_well_target → move_out_of_pull_radius
    - encounter_detect_stage_2_transition → spread_wide_avoid_altar
    - encounter_detect_seismic_shard_cast → sidestep_telegraph

dispel_priority_per_fight:
  corborus:    []                               # healer-only dispel role
  slabhide:    []
  ozruk:       []
  azil:        [{ spell_id: 16098, name: Curse of Blood, type: curse, urgency: HIGH, only_if_class_has_curse_cleanse: true }]

interrupt_priority_per_fight:
  corborus:    []
  slabhide:    []
  ozruk:       []                               # Ground Slam non-kickable
  azil:        [{ spell_id: 79351, name: Force Grip, urgency: HIGHEST }]

dispel_blacklist:
  - { spell_id: 92426, name: Paralyze (Ozruk heroic), reason: prefer DoT-damage-break over dispel; ranged DPS DoTs ARE the break mechanism, scope: ozruk_fight }
```
