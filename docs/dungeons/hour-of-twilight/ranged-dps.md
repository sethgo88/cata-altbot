# Hour of Twilight — Ranged DPS

**Ranged-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how ranged DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content ranged DPS patterns (positioning, spread/stack rules, threat ceiling) live in `docs/roles/ranged-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (frost-mage, affliction-warlock, mm-hunter, shadow-priest, balance-druid, elemental-shaman)
- This doc layers Hour of Twilight–specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_ranged_dps_overview:
  total_bosses: 3
  estimated_full_clear_minutes: 15-25
  difficulty_mode: heroic_only
  positioning_profile:
    arcurion: max_range_avoid_ledges          # spread for Boulder telegraphs
    asira: behind_body_blocker_if_marked      # Mark of Silence repositioning
    benedictus: max_range_spread_10y          # Shear AoE radius is 10y; stay out of cleave
  interrupt_load:
    arcurion: high                            # Hand of Frost — participate in kick rotation if class has kick
    asira: zero
    benedictus: high                          # Smite (P1) / Twilight Blast (P2)
  key_responsibilities:
    - icy_tomb_target_switch (arcurion)        # CRITICAL — switch DPS to tomb to free Thrall
    - mark_of_silence_self_handling (asira)    # caster bot must read self-debuff and stop casting
    - shear_dispel_support_if_priest (benedictus) # Mass Dispel from Holy/Disc Priest is the off-spec dispel
    - phase_2_transform_detection (benedictus) # re-acquire on Twilight Epiphany
    - p2_wave_gap_run (benedictus)             # no Thrall Water Shell in P2
```

---

## Trash — ranged DPS notes

```yaml
trash_ranged:
  faceless_voidstalkers:
    interrupt_role: |
      Shadow Bolt hardcaster — primary kick target. Ranged classes with
      kick should rotate:
      - Frost Mage: Counterspell (24s) / Frost Nova root for kiting
      - Affliction Warlock: Spell Lock via Felhunter pet (24s)
      - MM Hunter: Silencing Shot (24s)
      - Shadow Priest: Silence (45s)
      - Balance Druid: Solar Beam (60s; 8s AoE silence)
      - Elemental Shaman: Wind Shear (6s; off-GCD; 25y range)

  twilight_humanoid_packs:
    cc_priorities: |
      Ranged contributes opener-CC:
      - Frost Mage: Polymorph (Sheep) — primary multi-CC
      - Affliction Warlock: Banish (humanoid? no — Banish is demon/elemental in Cata; Fear PvE-disabled in trash usually)
      - MM Hunter: Freezing Trap (icetrap) on caster
      - Shadow Priest: Shackle Undead (if undead); Mind Control rare
      - Balance Druid: Cyclone (8s); Hibernate (beast/dragonkin only — N/A here)
      - Elemental Shaman: Hex (frog 60s, breaks on damage)

  crystalline_elementals:
    notes: AoE-burn pulls; spread for any Frost-bolt-style ranged casts; melee classes pull threat off ranged-cast openers

  twilight_assassin_engagements:
    notes: |
      Stealth-engage humanoids appear from invisibility. Ranged caster
      may take the first hit if tank is slow to taunt. Hunter Disengage
      / Mage Blink to re-position; resume rotation.

  thrall_escort:
    notes: |
      Ranged bot follow-tank tracks Thrall most of the time. NO direct
      contribution to Thrall escort beyond not pulling extra packs and
      not running ahead of him.
```

---

## Boss 1 — Arcurion

```yaml
boss_ranged:
  boss: arcurion
  difficulty_grade: medium
  positioning:
    default: |
      Max range from boss; AVOID standing under canyon ledges (Frozen
      Servitor Boulder reticles overlap there). Spread ~5y between
      players for Chains of Frost wave AND Boulder telegraph movement
      flexibility.
    facing: boss
    movement: |
      Move out of Boulder ground reticles (~3s warning per reticle).
      Chains of Frost wave is AoE — no positional escape, just eat the
      damage and the immobilize.

  interrupt_role:
    hand_of_frost: PARTICIPATE if class has kick
    chains_of_frost: UNVERIFIED interruptibility — try if kick is off-CD

  rotation_modifications:
    icy_tomb_spawn: |
      CRITICAL TARGET SWITCH. When Icy Tomb (103252) appears on Thrall:
      - Frost Mage: stop Frostbolt cast on boss; switch target to tomb;
        Frostbolt / Ice Lance on tomb until destroyed
      - Aff Warlock: do NOT re-apply DoTs to tomb (no benefit; tomb is
        a destructible object). Drain Soul / Shadow Bolt on tomb directly
      - MM Hunter: Steady Shot / Aimed Shot on tomb (no Hunter's Mark
        application; tomb is short-lived)
      - Shadow Priest: do NOT re-apply DoTs to tomb. Mind Blast / Mind
        Flay direct damage on tomb
      - Balance Druid: Wrath direct on tomb; do NOT re-apply DoTs
      - Ele Shaman: Lightning Bolt / Lava Burst on tomb; do NOT drop
        Searing Totem at tomb (tomb dies fast)
      Return to boss when tomb destroyed.

    torrent_execute_30pct: |
      Phase 2 — persistent group AoE; Thrall casts Bloodlust. Align
      offensive CDs:
      - Frost Mage: Icy Veins on Bloodlust; Mirror Image earlier in fight
      - Aff Warlock: Demon Soul (Felhunter) on BL; Soulburn-Curse setup
        if 31/0/10 build allows; full DoT spread for execute
      - MM Hunter: Rapid Fire on BL; Bestial Wrath N/A (BM only); Trinket
      - Shadow Priest: Shadowfiend on BL; trinket
      - Balance Druid: Force of Nature + Starfall on BL
      - Ele Shaman: Elemental Mastery + Fire Elemental Totem on BL

    chains_of_frost_immobilize: |
      You're rooted for 10s. Continue cast-in-place rotation. No movement
      anyway during the immobilize.

  cd_use:
    standard_offensive: rotational; align with Bloodlust at 30% HP
    survivability_during_torrent: |
      - Frost Mage: Iceblock if HP critical (5min CD — emergency)
      - Aff Warlock: Healthstone in combat
      - MM Hunter: Master's Call (talent — UNVERIFIED Cata)
      - Shadow Priest: Dispersion (-90% damage 6s) — strong on Torrent
      - Balance Druid: Barkskin (-20% damage 12s)
      - Ele Shaman: Stoneclaw Totem (talented shield)

  heroic_delta: |
    N/A — heroic-only fight.

  bot_anti_patterns:
    - DO NOT continue casting on the boss when Icy Tomb is up — switch target
    - DO NOT stand under canyon ledges (Boulder reticle overlap zone)
    - DO NOT cast AoE rotation on boss alone (no adds to cleave; ST rotation only)
    - DO NOT use Thunderstorm (Ele Shaman) — knockback may shove tank or move mobs poorly
```

---

## Boss 2 — Asira Dawnslayer

```yaml
boss_ranged:
  boss: asira_dawnslayer
  difficulty_grade: medium (caster-punish mechanic)
  positioning:
    default: |
      Stay at max range; position so a non-marked melee/tank is BETWEEN
      you and Asira (body-block knife if you become marked). Stay near
      Thrall's Rising Fire Totem for the buff during Blade Barrier execute.
    facing: boss
    movement: |
      AVOID Choking Smoke Bomb cloud. Move out before bomb lands or
      reposition once it's placed. Tank drags Asira out; ranged stays
      well clear.

  interrupt_role:
    none: no kick-required casts

  rotation_modifications:
    mark_of_silence_on_self: |
      CRITICAL self-debuff handling. Mark of Silence (102726) lasts 20s.
      For the duration:
      - Frost Mage: STOP casting Frostbolt / Frostfire Bolt. Cast only
        instants: Ice Lance on Fingers proc, Frost Nova (no effect on
        Asira but proc-able), Cold Snap CD reset. Rotation stalls; eat
        the DPS loss.
      - Aff Warlock: existing DoT TICKS on Asira don't proc the knife
        (UNVERIFIED — see survey Disagreement #6). DON'T re-cast UA /
        Corruption / Bane / Haunt during the mark. Drain Soul channel:
        STOP. Shadow Bolt: STOP.
      - MM Hunter: Steady Shot / Aimed Shot are casts → STOP. Auto-shot
        is technically a "cast" — UNVERIFIED. Use Arcane Shot (instant),
        Multi-Shot (instant), Kill Shot (instant if available). Wing
        Clip / trap-launcher instant abilities OK.
      - Shadow Priest: Mind Blast / Mind Spike / Vampiric Touch (cast) /
        Shadow Word: Pain (instant). Mind Flay = channel → STOP.
        Continue instants only. DPS halts.
      - Balance Druid: Wrath / Starfire = cast → STOP. Moonfire / Sunfire
        / Starsurge (instant) → OK. Eclipse bar will stall.
      - Ele Shaman: Lightning Bolt / Lava Burst / Earth Shock / Flame
        Shock = casts (some instant, some hardcast). UNVERIFIED whether
        Shock-school casts trigger the mark. Conservative default: stop
        ALL casts; auto-attack and Lightning Shield ticks only. Eat 20s
        DPS gap.
      Body-block alternative: if a melee/tank is positioned between you
      and Asira, the knife hits the body-blocker first; you can continue
      casting. Bot logic: detect body-blocker presence; if present,
      continue rotation. If absent, gate casts.

    smoke_bomb_avoidance: |
      Reposition out of the cloud OR away from Asira's standing position
      so she walks out of the cloud toward you (tank's job, but ranged
      facilitates by not crowding tank's drag direction).

    blade_barrier_execute_30pct: |
      Big-hit cooldowns to break the shield:
      - Frost Mage: Frostfire Bolt (~2.5s cast, big hit) — primary breaker;
        Deep Freeze + Ice Lance on Shatter (Frostbolt → DF → IL combo)
      - Aff Warlock: Soul Fire (cast) — biggest single Aff hit; Drain
        Soul ticks during execute (sub-threshold per tick — useless to
        break shield, useful for damage post-break)
      - MM Hunter: Aimed Shot (Master Marksman proc instant) — primary
        breaker; Chimera Shot ~50k; Kill Shot if execute window
      - Shadow Priest: Mind Blast (instant ~30-40k) — primary breaker;
        Shadow Word: Death (Glyphed for self-damage immunity)
      - Balance Druid: Starsurge (~60k crit) — primary breaker; Wrath /
        Starfire below threshold individually
      - Ele Shaman: Lava Burst (always crits on Flame Shock target) —
        primary breaker (~50-70k crit); Earth Shock if Lightning Shield
        is at 9 stacks (Fulmination)
      Stand in Thrall's Rising Fire Totem buff during this phase.

  cd_use:
    save_for_blade_barrier: |
      Big-hit offensive CDs queued for 30% HP execute window:
      - Frost Mage: Icy Veins + Mirror Image at 30%
      - Aff Warlock: Soul Fire + Demon Soul at 30%
      - MM Hunter: Rapid Fire + Master Marksman + trinket at 30%
      - Shadow Priest: Shadowfiend + Power Infusion (talent — UNVERIFIED)
      - Balance Druid: Force of Nature + Starfall (Starfall proc-cast Starsurge)
      - Ele Shaman: Elemental Mastery + Fire Elemental Totem

  heroic_delta: |
    N/A — heroic-only fight.

  bot_anti_patterns:
    - DO NOT continue hardcasts while Mark of Silence is on you
    - DO NOT chase Asira into the Smoke Bomb cloud (target-loss; bot waits)
    - DO NOT use small filler hits during Blade Barrier execute (all reduce to 1)
    - DO NOT stand in Smoke Bomb (5k Nature / sec)
```

---

## Boss 3 — Archbishop Benedictus

```yaml
boss_ranged:
  boss: archbishop_benedictus
  difficulty_grade: HARD (mechanically dense)
  positioning:
    default: |
      Max range from boss; SPREAD ≥10y from other players (Shear AoE
      radius is 10y — clustered ranged take cleave damage when Shear
      lands on a teammate).
    facing: boss
    movement: |
      Constant — orb dodge in P2 (3 orbs unaided), wave gap-run in P2.
      P1 is more stationary (stack into Water Shell on wave).

  interrupt_role:
    smite_p1: PARTICIPATE — primary kick target on P1
    twilight_blast_p2: PARTICIPATE — primary kick target on P2

  rotation_modifications:
    p1_holy_phase: |
      Standard ST rotation on boss. Maintain spread. Move out of orb path
      when the surviving Purifying Light orb (1 of 3 — Thrall destroys
      2/3) approaches your position.
      Ranged cast-while-moving classes preferred for orb-dodge:
      - Frost Mage: Frostbolt = hardcast (interrupted by movement); Ice
        Lance instant; Cone of Cold instant
      - Aff Warlock: DoTs are instant; Drain Soul channels break on move
      - MM Hunter: Steady / Aimed are hardcasts (Aspect of the Fox in 5.0+
        allows cast-on-move — UNVERIFIED Cata); Arcane Shot instant
      - Shadow Priest: Mind Flay channel breaks on move; DoTs instant
      - Balance Druid: Wrath / Starfire hardcast; Moonfire / Sunfire
        instant; Starsurge instant on proc
      - Ele Shaman: Lightning Bolt / Lava Burst hardcast; Flame Shock instant

    twilight_epiphany_60pct: |
      Phase transition detection — bot must:
      1. Detect transform event (boss model swap, aura set change)
      2. Re-acquire target (likely same NPC ID; visual change)
      3. Adjust school detection (Holy → Shadow incoming damage)
      4. Discontinue waiting for Thrall support (no Water Shell, no
         Chain Lightning)
      DPS rotation continues uninterrupted (single target on boss).

    p2_twilight_phase: |
      Same ST rotation; school of school-buff-eligible buffs may matter:
      - Frost Mage: Mage Armor +Magic resist? — N/A in Cata 4.3.4 build
      - Shadow Priest: Vampiric Touch / Shadow Word: Pain — Shadow school
        on the boss who's now Shadow-themed; no special interaction
      - All ranged: continue rotation

    purifying_light_orb_p1: |
      ~1 surviving orb to dodge per cast. Move sideways out of orb path.
      Continue cast on-move when class permits.

    corrupting_twilight_orb_p2: |
      ALL 3 orbs must be dodged unaided. Movement-heavy phase. Frost
      Mage / Aff Warlock / Shadow Priest favor instants; MM Hunter
      Steady/Aimed hardcasts may need stop-cast windows.

    wave_of_virtue_p1: |
      STACK INTO THRALL'S WATER SHELL bubble. Damage immunity + 100%
      damage buff during the bubble. Hardcast spells (Pyroblast, Aimed
      Shot, etc.) launched during the buff window benefit from the buff.

    wave_of_twilight_p2: |
      NO WATER SHELL. Survive by:
      - Frost Mage: ICEBLOCK (immune all damage 10s, 5min CD) — CRITICAL save
      - Aff Warlock: Soulstone self pre-fight; Healthstone; gap-run
      - MM Hunter: Disengage to gap; Master's Call (talent — UNVERIFIED)
      - Shadow Priest: Dispersion (-90% damage 6s); gap-run
      - Balance Druid: Barkskin; gap-run
      - Ele Shaman: Stoneclaw Totem (talented shield); gap-run
      Default: gap-run the wave (it has openings).

    shear_dispel_support: |
      Ranged with Magic-dispel:
      - Holy Priest off-spec: Mass Dispel (32375) — 60s CD, 15y AoE
      - Disc Priest off-spec: Dispel Magic (528) on individual; Mass Dispel
      - Shadow Priest: Dispel Magic (528) — UNVERIFIED if Shadow specs
        retain friendly Magic dispel in 4.3.4
      - Mage: Remove Curse (475) — Curse only, NOT Magic
      - Druid: Remove Corruption (2782) — Curse + Poison only, NOT Magic
      - Warlock: Singe Magic (Imp pet) — UNVERIFIED Cata; bot rarely
        switches to Imp
      - Hunter / Shaman ranged: NO Magic dispel
      If healer is Resto Shaman / Resto Druid (both lack Magic dispel),
      the Shear-dispel role MUST fall to a hybrid like Shadow Priest off-
      spec. Bot AI hooks must encode "if healer cannot dispel, ranged-
      with-dispel takes the role."

  cd_use:
    standard_offensive: rotational; bloodlust pre-pull or per group call
    big_burst_align: phase 2 entry (no more Thrall help; faster kill = less risk)

  heroic_delta: |
    N/A — heroic-only fight.

  bot_anti_patterns:
    - DO NOT cluster ranged (Shear AoE 10y radius cleaves clustered ranged)
    - DO NOT continue hardcast through orb impact (eat 80k = dead)
    - DO NOT stand outside Water Shell during Wave of Virtue (P1)
    - DO NOT skip phase 2 transform detection (target reference may stale)
    - DO NOT auto-dispel anything other than Shears in this fight (no other dispel-needs)
    - DO NOT continue casting Vampiric Touch / Drain Soul / Mind Flay channels through wave gap-run
```

---

## Bot AI hooks

```yaml
ai_hooks:
  arcurion:
    - encounter_detect_hand_of_frost_cast        → kick_rotation_participate (if class has kick)
    - encounter_detect_icy_tomb_spawn(thrall)    → switch_target_to_tomb
    - encounter_detect_icy_tomb_destroyed        → switch_target_back_to_boss
    - encounter_detect_icy_boulder_telegraph     → move_out_of_reticle
    - encounter_state_boss_hp_pct <= 30          → torrent_execute_phase
    - encounter_detect_thrall_bloodlust          → align_offensive_cd

  asira_dawnslayer:
    - encounter_detect_mark_of_silence(self)     → STOP_ALL_CASTS_FOR_20S OR position_behind_body_blocker
    - encounter_detect_smoke_bomb_ground_place   → reposition_clear; do_not_chase_into_cloud
    - encounter_state_boss_in_cloud              → wait (target-loss; do NOT chase)
    - encounter_state_boss_hp_pct <= 30          → blade_barrier_phase
    - encounter_state_blade_barrier_active       → big_hit_cooldown_priority
    - encounter_detect_thrall_rising_fire_totem  → stand_in_totem_buff

  archbishop_benedictus:
    - encounter_detect_smite_cast(P1)            → kick_rotation_participate
    - encounter_detect_twilight_blast_cast(P2)   → kick_rotation_participate
    - encounter_detect_purifying_light_orb_p1    → dodge_surviving_orb (Thrall destroys 2/3)
    - encounter_detect_corrupting_twilight_orb_p2 → dodge_all_3_orbs
    - encounter_detect_wave_of_virtue_p1         → stack_into_water_shell
    - encounter_detect_wave_of_twilight_p2       → gap_run_OR_immunity_cd (Iceblock / Dispersion / Barkskin)
    - encounter_state_boss_hp_pct == 60          → twilight_epiphany_imminent
    - encounter_detect_twilight_epiphany_cast    → phase_2_transition_detect; re_acquire_target
    - encounter_detect_thrall_twilight_prison    → discontinue_thrall_support_expectations
    - encounter_state_self_dispel_capable        → magic_dispel_shear_if_healer_cannot

dispel_blacklist: []                             # no dispel-blacklist entries

target_switch_rules:
  arcurion:
    - icy_tomb_spawn → switch to tomb
    - icy_tomb_destroyed → switch back to boss
  asira_dawnslayer: []
  benedictus:
    - twilight_epiphany → re-acquire (likely same NPC ID, visual swap)

class_specific_dispel_capability_p3:
  frost_mage:        NO_MAGIC_DISPEL              # Remove Curse is curse-only
  affliction_warlock: NO_MAGIC_DISPEL             # Singe Magic Imp pet swap UNVERIFIED Cata
  mm_hunter:         NO_MAGIC_DISPEL
  shadow_priest:     YES (Dispel Magic 528) — UNVERIFIED if Shadow retains in 4.3.4
  balance_druid:     NO_MAGIC_DISPEL              # Remove Corruption is curse/poison only
  elemental_shaman:  NO_MAGIC_DISPEL              # Cleanse Spirit is curse only
  notes: |
    In a healer=Resto-Shaman/Druid + ranged=non-Priest group, NO bot
    has Magic dispel for Shears. This is a group-composition gap.
```
