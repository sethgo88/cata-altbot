# Hour of Twilight — Melee DPS

**Melee-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how melee DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content melee DPS patterns (positioning, interrupt rotation, threat ceiling) live in `docs/roles/melee-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (combat-rogue, fury-warrior, ret-paladin, frost-dk, enh-shaman, feral-cat)
- This doc layers Hour of Twilight–specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_melee_dps_overview:
  total_bosses: 3
  estimated_full_clear_minutes: 15-25
  difficulty_mode: heroic_only
  positioning_profile:
    arcurion: behind_boss_avoid_ledges        # standard rear-arc + Boulder telegraph dodge
    asira: body_block_or_behind                # body-block knives for marked casters
    benedictus: behind_boss_spread_10y         # Shear AoE 10y — even melee spreads from clustered allies
  interrupt_load:
    arcurion: high                            # Hand of Frost — primary kick rotation participant
    asira: zero
    benedictus: high                          # Smite (P1) / Twilight Blast (P2) — primary kick rotation
  key_responsibilities:
    - quake_hand_of_frost_kick_rotation (arcurion)
    - icy_tomb_target_switch (arcurion)
    - body_block_throw_knife (asira)
    - blade_barrier_big_hit_priority (asira)
    - shear_dispel_support_if_paladin (benedictus)
    - phase_2_transform_detection (benedictus)
    - p2_wave_gap_run_or_immunity (benedictus)
```

---

## Trash — melee DPS notes

```yaml
trash_melee:
  faceless_voidstalkers:
    interrupt_role: |
      Shadow Bolt hardcaster — primary kick target. Melee classes rotate:
      - Combat Rogue: Kick (10s CD)
      - Fury Warrior: Pummel (10s CD; off-GCD)
      - Ret Pally: Rebuke (15s CD; off-GCD)
      - Frost DK: Mind Freeze (10s CD; off-GCD)
      - Enh Shaman: Wind Shear (6s CD; off-GCD; 25y range)
      - Feral Cat: Skull Bash (60s CD; gap-close + interrupt)

  twilight_humanoid_packs:
    cc_priorities: |
      Melee bot contributes Sap (Rogue) or Hammer of Justice (Pally) to
      opener-CC; primary role is kill-ordered DPS on skull marker.

  twilight_assassin_engagements:
    notes: |
      Stealth-engage humanoids appear from invisibility. Melee bot is
      typically already in melee range; can taunt-flag-equivalent (Hand
      of Reckoning if Pally) to pull off ranged-first-engagement, or
      just kill quickly.

  corrupted_slime_packs:
    notes: don't stand in acid patches; melee can re-position to side of slime

  thrall_escort:
    notes: |
      Melee bot follow-tank tracks Thrall most of the time. NO direct
      contribution to escort beyond not pulling extra packs.
```

---

## Boss 1 — Arcurion

```yaml
boss_melee:
  boss: arcurion
  difficulty_grade: medium
  positioning:
    default: |
      Behind boss (standard rear-arc); avoid standing under canyon ledges
      (Frozen Servitor Boulder reticles overlap there). When boss moves,
      reposition behind.
    facing: boss back
    movement: |
      Move out of Boulder ground reticles (~3s warning). Chains of Frost
      wave is AoE — no positional escape, but melee is in range anyway.

  interrupt_role:
    hand_of_frost: PRIMARY — melee DPS interrupt rotation (Hand of Frost on ~30s; rotate kicks across DPS/tank)

  rotation_modifications:
    icy_tomb_spawn: |
      CRITICAL TARGET SWITCH. When Icy Tomb (103252) appears on Thrall:
      - Combat Rogue: stop SnD-maintenance auto-attack on boss; CP RESET
        on target switch; Shiv (instant) / Sinister Strike on tomb to
        rebuild CP, OR if you're CP-loaded already, Eviscerate the tomb
        for big damage. Save SnD refresh for after tomb (2-CP enough).
      - Fury Warrior: Charge to tomb if out of melee; Heroic Strike /
        Slam on tomb. Rage carries over.
      - Ret Pally: Crusader Strike / Templar's Verdict on tomb. Holy
        Power generated on tomb spends fine.
      - Frost DK: Plague Strike / Festering Strike on tomb (no diseases
        applied; tomb dies before they tick). Obliterate / Frost Strike
        as runes/RP allow.
      - Enh Shaman: Stormstrike / Lava Lash on tomb if in range; Maelstrom
        Weapon may stack while attacking tomb (carry stacks back to boss).
      - Feral Cat: Mangle / Shred on tomb (no Rip/Rake — tomb dies fast).
        CP RESET on target switch — full CP reset; build new on tomb or
        save for boss return.
      Return to boss when tomb destroyed.

    torrent_execute_30pct: |
      Phase 2 — persistent group AoE; Thrall casts Bloodlust. Align
      offensive CDs:
      - Combat Rogue: Adrenaline Rush + Killing Spree on BL
      - Fury Warrior: Recklessness + Death Wish (talent — UNVERIFIED) on BL
      - Ret Pally: Avenging Wrath + Zealotry + Guardian of Ancient Kings on BL
      - Frost DK: Pillar of Frost + trinket on BL
      - Enh Shaman: Feral Spirits (Spirit Wolves) + Elemental Mastery (talent — UNVERIFIED) on BL
      - Feral Cat: Tiger's Fury + Berserk on BL

    chains_of_frost_immobilize: |
      You're rooted for 10s. Continue rotation in place. No movement
      anyway during the immobilize.

  cd_use:
    standard_offensive: rotational; align with Bloodlust at 30% HP
    survivability_during_torrent: |
      - Combat Rogue: Cloak of Shadows (90s) for the Frost AoE — strong
      - Fury Warrior: Spell Reflection? — Spell Reflect is shield-only,
        Fury 2H spec doesn't carry shield (UNVERIFIED Titan's Grip mode);
        Berserker Rage (immune fear/disorient, not damage)
      - Ret Pally: Divine Protection (-20% magic 10s); Lay on Hands
        emergency
      - Frost DK: Anti-Magic Shell (75% magic absorb 5s) — STRONG on Frost
      - Enh Shaman: Shamanistic Rage; Stoneclaw Totem (talented shield)
      - Feral Cat: Survival Instincts (-50% damage 12s)

  heroic_delta: |
    N/A — heroic-only fight.

  bot_anti_patterns:
    - DO NOT continue DPS on boss when Icy Tomb is up — switch target
    - DO NOT stand under canyon ledges (Boulder reticle overlap)
    - DO NOT spend CP/HP on a target you're about to leave (Combat Rogue,
      Feral Cat, Ret Pally — pre-tomb-spawn, finish your finisher then switch)
    - DO NOT use AoE rotation on boss alone (no adds to cleave)
```

---

## Boss 2 — Asira Dawnslayer

```yaml
boss_melee:
  boss: asira_dawnslayer
  difficulty_grade: medium (positional + body-block role)
  positioning:
    default: |
      BEHIND boss for rear-arc damage. CRITICAL — when a ranged caster
      teammate is the Mark of Silence target, position YOURSELF between
      Asira and the marked caster (body-block the knife). The melee bot
      is the natural body-blocker because melee is already adjacent to
      Asira.
    facing: boss back
    movement: |
      AVOID Choking Smoke Bomb cloud. When tank drags Asira out, follow
      to the new position behind her.

  interrupt_role:
    none: no kick-required casts

  rotation_modifications:
    body_block_throw_knife: |
      When ranged caster has Mark of Silence (102726) and is about to
      cast (or is hardcasting now), MELEE BOT MUST BE in the line
      between Asira and the marked caster. Knife travels straight; hits
      first player in line. Body-block redirects damage to melee
      (~10k physical — easily healed).
      Bot logic: detect "ally has Mark of Silence" → check positioning
      → if not in line, reposition between Asira and that ally.

    smoke_bomb_avoidance: |
      Stay out of cloud. When cloud is placed, tank drags Asira out;
      melee follows tank's lead. DO NOT chase Asira into the cloud
      (5k Nature / sec; un-targetable from outside the cloud).

    blade_barrier_execute_30pct: |
      Big-hit cooldowns to break the shield (~30-40k threshold UNVERIFIED):
      - Combat Rogue: Killing Spree (combat ability, ~50k+ per attack
        burst) — primary breaker; Eviscerate at 5 CP (~30-50k crit) —
        secondary breaker; Sinister Strike under threshold individually
      - Fury Warrior: Heroic Strike (rage dump, ~30-40k) — primary
        breaker if rage-fed; Bloodthirst / Raging Blow under threshold
        individually
      - Ret Pally: Templar's Verdict at 3 HP (~50k+ crit) — primary
        breaker; Crusader Strike under threshold individually
      - Frost DK: Obliterate (~40-60k crit on diseased target) — primary
        breaker; Howling Blast / Frost Strike under threshold
      - Enh Shaman: Stormstrike (~30-50k crit) — primary breaker; Lava
        Lash under threshold; Lightning Bolt at 5 MW Maelstrom Weapon
        stacks (instant cast hard hit) — secondary breaker
      - Feral Cat: Ferocious Bite at 5 CP with full Energy (~50k+ crit) —
        primary breaker; Shred / Mangle under threshold
      Stand in Thrall's Rising Fire Totem buff during this phase.

  cd_use:
    save_for_blade_barrier: |
      Big-hit offensive CDs queued for 30% HP execute window:
      - Combat Rogue: Adrenaline Rush + Killing Spree at 30%
      - Fury Warrior: Recklessness at 30%
      - Ret Pally: Avenging Wrath + Zealotry at 30%
      - Frost DK: Pillar of Frost + trinket at 30%
      - Enh Shaman: Feral Spirits + Maelstrom Weapon spam (5MW Lightning Bolt)
      - Feral Cat: Tiger's Fury + Berserk at 30%

  heroic_delta: |
    N/A — heroic-only fight.

  bot_anti_patterns:
    - DO NOT chase Asira into Smoke Bomb (target-loss + 5k Nature / sec)
    - DO NOT spend small finishers during Blade Barrier (1 damage per hit)
    - DO NOT ignore body-block opportunity when caster ally is marked
    - DO NOT stand IN the cloud (5k Nature / sec)
```

---

## Boss 3 — Archbishop Benedictus

```yaml
boss_melee:
  boss: archbishop_benedictus
  difficulty_grade: HARD (mechanically dense)
  positioning:
    default: |
      Behind boss for rear-arc damage; SPREAD ≥10y from other melee/
      ranged where possible. Shear AoE radius is 10y — even melee bots
      should not stack with each other within Shear range.
      Note: in a single-tank single-melee comp this is automatic; with
      2+ melee, the second melee positions to the OPPOSITE side of boss
      (still rear-arc, but 10y from first melee).
    facing: boss back
    movement: |
      Constant — orb dodge in P2, wave gap-run in P2. P1 is more
      stationary (stack into Water Shell on wave).

  interrupt_role:
    smite_p1: PARTICIPATE — primary kick rotation
    twilight_blast_p2: PARTICIPATE — primary kick rotation

  rotation_modifications:
    p1_holy_phase: |
      Standard ST rotation on boss. Maintain spread. Move out of orb
      path when the surviving Purifying Light orb (1 of 3 — Thrall
      destroys 2/3) approaches your position.
      Melee mobility tools for orb-dodge:
      - Combat Rogue: Sprint (1min CD); already mobile via Step
      - Fury Warrior: Charge (15s CD); Heroic Leap (45s)
      - Ret Pally: Hand of Freedom (1min CD self-cast for movement-
        immunity; or cast on someone else)
      - Frost DK: Death's Advance (talent — UNVERIFIED); Death Grip on
        boss to drag toward you (DO NOT — interferes with tank position)
      - Enh Shaman: Ghost Wolf for travel (no combat speed buff)
      - Feral Cat: Cat Form base 30% speed; Stampeding Roar (raid speed
        burst)

    twilight_epiphany_60pct: |
      Phase transition — bot must:
      1. Detect transform (boss model swap)
      2. Re-acquire target
      3. CP / DOTs / disease state reset on target swap (in this case,
         the target is the same NPC just transformed — UNVERIFIED whether
         Cata 4.3.4 treats transform as "new target" for CP/DoT pandemic)
         - Combat Rogue: SnD self-buff persists (off target). CP attached
           to target — UNVERIFIED if reset on transform. Conservative
           default: assume CP transfers if NPC ID is unchanged.
         - Frost DK: diseases on target — UNVERIFIED if reset on transform.
           Re-Outbreak as a safety.
         - Feral Cat: CP attached to target — UNVERIFIED if reset.
           Re-apply Rake / Rip / Mangle as safety.

    p2_twilight_phase: |
      Same ST rotation. School swap on boss damage doesn't affect melee
      attacks. Continue rotation.

    purifying_light_orb_p1: |
      ~1 surviving orb to dodge. Melee re-positions sideways behind
      boss; resume rotation when orb is clear.

    corrupting_twilight_orb_p2: |
      ALL 3 orbs must be dodged unaided. Melee uses mobility tools (above)
      and/or ranged-out-of-orb-path. Lose ~1-2s of rotation per dodge.

    wave_of_virtue_p1: |
      STACK INTO THRALL'S WATER SHELL bubble. Melee just runs to bubble
      with the rest of the group. Damage immunity + 100% damage buff
      applies to melee strikes during bubble.

    wave_of_twilight_p2: |
      NO WATER SHELL. Survive by:
      - Combat Rogue: Cloak of Shadows (90s; magic immunity 5s) — STRONG
      - Fury Warrior: gap-run; no immunity available
      - Ret Pally: Divine Shield (immune all damage 8s, 5min CD) —
        CRITICAL save
      - Frost DK: Anti-Magic Shell (75% magic absorb 5s) — strong on Shadow
      - Enh Shaman: Stoneclaw Totem (talented shield); gap-run primary
      - Feral Cat: Survival Instincts (-50% damage 12s); gap-run primary
      Default: gap-run the wave (it has openings).

    shear_dispel_support: |
      Melee with Magic-dispel:
      - Ret Pally: Cleanse (51886) — dispels Magic (after Cleansing Light
        talent or baseline at level 22) — YES Magic-dispel for Ret in 4.3.4 (UNVERIFIED if baseline or talented)
      - Combat Rogue: NO Magic dispel
      - Fury Warrior: NO dispel (Spell Reflect reflects, not dispels)
      - Frost DK: NO friendly dispel
      - Enh Shaman: Cleanse Spirit — Curse only, NOT Magic
      - Feral Cat: NO dispel
      If melee bot is Ret Pally AND healer is Resto Shaman/Druid (no Magic
      dispel), Ret Pally takes the Shear-dispel role. UNVERIFIED whether
      Ret Cleanse is Magic-capable in 4.3.4 PvE — flag for verification.

  cd_use:
    standard_offensive: rotational; bloodlust pre-pull or per group call
    big_burst_align: phase 2 entry (no Thrall help; faster kill = less risk)

  heroic_delta: |
    N/A — heroic-only fight.

  bot_anti_patterns:
    - DO NOT stack with other melee within 10y (Shear AoE cleave)
    - DO NOT continue rotation through orb impact path (eat 80k = dead)
    - DO NOT stand outside Water Shell during Wave of Virtue (P1)
    - DO NOT skip phase 2 transform detection (rotation may continue on
      stale target reference)
    - DO NOT spend finishers right before phase transition (CP/HP may
      reset on transform — UNVERIFIED)
    - DO NOT auto-dispel anything other than Shears (no other dispel-needs)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  arcurion:
    - encounter_detect_hand_of_frost_cast        → kick_rotation_participate (PRIMARY)
    - encounter_detect_icy_tomb_spawn(thrall)    → switch_target_to_tomb
    - encounter_detect_icy_tomb_destroyed        → switch_target_back_to_boss
    - encounter_detect_icy_boulder_telegraph     → move_out_of_reticle
    - encounter_state_boss_hp_pct <= 30          → torrent_execute_phase
    - encounter_detect_thrall_bloodlust          → align_offensive_cd

  asira_dawnslayer:
    - encounter_detect_mark_of_silence(ally)     → reposition_to_body_block_line(asira, ally)
    - encounter_detect_smoke_bomb_ground_place   → exit_cloud_or_avoid
    - encounter_state_boss_in_cloud              → wait (do NOT chase)
    - encounter_state_boss_hp_pct <= 30          → blade_barrier_phase
    - encounter_state_blade_barrier_active       → big_hit_cooldown_priority
    - encounter_detect_thrall_rising_fire_totem  → stand_in_totem_buff

  archbishop_benedictus:
    - encounter_detect_smite_cast(P1)            → kick_rotation_participate (PRIMARY)
    - encounter_detect_twilight_blast_cast(P2)   → kick_rotation_participate (PRIMARY)
    - encounter_detect_purifying_light_orb_p1    → dodge_surviving_orb
    - encounter_detect_corrupting_twilight_orb_p2 → dodge_all_3_orbs
    - encounter_detect_wave_of_virtue_p1         → stack_into_water_shell
    - encounter_detect_wave_of_twilight_p2       → gap_run_OR_immunity_cd (Cloak / Divine Shield / AMS)
    - encounter_state_boss_hp_pct == 60          → twilight_epiphany_imminent
    - encounter_detect_twilight_epiphany_cast    → phase_2_transition_detect; re_acquire_target; check_cp_dot_state
    - encounter_detect_thrall_twilight_prison    → discontinue_thrall_support_expectations
    - encounter_state_self_dispel_capable_magic  → magic_dispel_shear_if_healer_cannot (Ret Pally only — UNVERIFIED)

dispel_blacklist: []                             # no dispel-blacklist entries

target_switch_rules:
  arcurion:
    - icy_tomb_spawn → switch to tomb (CP/disease/buff state may reset depending on class)
    - icy_tomb_destroyed → switch back to boss (re-apply CP/DoT/disease as needed)
  asira_dawnslayer: []
  benedictus:
    - twilight_epiphany → re-acquire (likely same NPC ID; UNVERIFIED if CP/DoT/disease state preserves through transform)

class_specific_dispel_capability_p3:
  combat_rogue:      NO_MAGIC_DISPEL
  fury_warrior:      NO_MAGIC_DISPEL
  ret_paladin:       UNVERIFIED — Cleanse 51886 dispel-Magic flag in 4.3.4 PvE
  frost_dk:          NO_FRIENDLY_DISPEL
  enh_shaman:        NO_MAGIC_DISPEL              # Cleanse Spirit is curse only
  feral_cat:         NO_DISPEL
  notes: |
    If melee bot is Ret Pally and healer is Resto Shaman/Druid (no Magic
    dispel), Ret Pally MAY take the Shear-dispel role. Verify Ret
    Cleanse Magic-capability against 4.3.4 talent/glyph state at DBC
    pass time.
```
