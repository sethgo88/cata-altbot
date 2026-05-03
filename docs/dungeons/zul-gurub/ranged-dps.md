# Zul'Gurub — Ranged DPS

**Ranged-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how ranged DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content ranged DPS patterns (positioning, spread/stack rules, threat ceiling) live in `docs/roles/ranged-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (frost-mage, affliction-warlock, mm-hunter, shadow-priest, balance-druid, elemental-shaman)
- This doc layers ZG-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_ranged_dps_overview:
  total_bosses: 5 (+ 1 optional Cache of Madness, default skip)
  estimated_full_clear_minutes: 45-60 (launch-tier) / 25-35 (geared)
  difficulty_tier: HEROIC_ONLY
  positioning_profile:
    venoxis: spread_25y_phase_1 / mid_range_phase_2 / cluster_phase_3
    mandokir: spread_moderate                  # standard ranged spread
    kilnara: 12y_outside_tears_radius          # cluster panthers for AoE during clear
    zanzil: max_range_avoid_zanzili_fire       # standard
    jindo: in_or_out_of_deadzone_phase_1 / chain_dps_phase_2
  interrupt_load:
    venoxis: HIGH                              # Whispers of Hethiss every cast
    mandokir: zero
    kilnara: HIGH                              # Tears of Blood + Shadow Bolt
    zanzil: HIGH                               # Voodoo Bolt
    jindo: zero
  key_responsibilities:
    - whispers_kick_rotation (venoxis)
    - tears_of_blood_kick_rotation (kilnara)
    - voodoo_bolt_kick_rotation (zanzil)
    - target_swap_to_ohgan (mandokir)
    - aoe_panther_clear (kilnara pre-engage)
    - cauldron_drink_on_green (zanzil)
    - deadzone_in_out_timing (jindo phase 1)
    - chain_burst_phase_2 (jindo)
```

---

## Trash — ranged DPS notes

```yaml
trash_ranged:
  voodoo_acolytes_caster_packs:
    cc_priorities: |
      Ranged contributes opener-CC:
      - Frost Mage: Polymorph (Sheep) — primary multi-CC
      - Affliction Warlock: Fear / Banish (no demons in ZG); Howl of Terror situational
      - MM Hunter: Freezing Trap on caster
      - Shadow Priest: Shackle Undead (no undead pre-Jin'do, limited use); Mind Control rare
      - Balance Druid: Cyclone (8s); Hibernate on cobras/raptors (beasts)
      - Elemental Shaman: Hex (frog 60s, breaks on damage)
    interrupt_role: |
      - Frost Mage: Counterspell (24s; 40y) on Shadow Bolts
      - Aff Warlock: Spell Lock (Felhunter pet)
      - MM Hunter: Silencing Shot (24s)
      - Shadow Priest: Silence (45s)
      - Balance Druid: Solar Beam (60s; 8s AoE silence)
      - Ele Shaman: Wind Shear (6s; off-GCD; 25y)

  razzashi_raptors:
    notes: pack-pulls; AoE rotation on cluster (Blizzard / Seed of Corruption / Multi-Shot / Mind Sear / Hurricane / Chain Lightning at 5 MW)

  bethekk_acolytes:
    cc_priorities: same as Voodoo Acolytes; Polymorph/Hex primary

  zanzili_pre_packs:
    notes: |
      First pack: tank-led Burning Blood AoE; ranged stays at range and
      cleaves. Second pack (Rabid Gurubashi): ranged kites if fixated.
```

---

## Boss 1 — High Priest Venoxis

```yaml
boss_ranged:
  boss: high_priest_venoxis
  difficulty_grade: hard (movement + interrupt)

  positioning:
    phase_1: |
      SPREAD to >25y from other ranged for Toxic Link safety. Navigate
      around poison maze patches (Venomous Effusion).
    phase_2: |
      MID-RANGE (~25-30y) — NOT max range. Pool of Acrid Tears spawns at
      furthest player; if you're at extreme range, you become the pool
      target. Pool placement should be at platform edge.
    phase_3: |
      Move to platform edge to avoid altar floor poison. Cluster for burst
      window (Venom Withdrawal); kite tendrils with positioning.
    facing: boss
    movement: high — multi-phase repositioning required

  interrupt_role:
    whispers_of_hethiss: |
      PARTICIPATE — primary kick target. Class kicks:
      - Frost Mage: Counterspell (24s CD; 40y; SILENCES)
      - Aff Warlock: Spell Lock via Felhunter (24s CD; 30y)
      - MM Hunter: Silencing Shot (24s CD; 40y; off-GCD via Marksman tree)
      - Shadow Priest: Silence (45s CD; 30y; 5s lockout)
      - Balance Druid: Solar Beam (60s CD; 8s AoE silence)
      - Ele Shaman: Wind Shear (6s CD; 25y; off-GCD) — RANGE LIMITED

  rotation_modifications:
    phase_1: |
      Standard ST rotation. Stop-cast on Toxic Link breakaway movement.
      Cast-while-moving classes excel here:
      - Frost Mage: Frostbolt hardcast; Ice Lance instant on Fingers; Fire Blast instant
      - Aff Warlock: DoTs instant; Drain Soul channel breaks on movement
      - MM Hunter: Steady Shot hardcast; Aimed Shot proc; cast-while-moving via Aspect of the Fox? UNVERIFIED Cata
      - Shadow Priest: Mind Flay channels mobile-friendly; DoTs instant
      - Balance Druid: Wrath/Starfire hardcast; Moonfire/Sunfire instant; Lunar Shower talent allows cast-while-moving
      - Ele Shaman: LB/LvB hardcast; Flame Shock instant
    phase_2: |
      ST rotation with cone-aware positioning. Pool of Acrid Tears placement
      depends on YOUR distance — drop pools at platform edge by positioning
      mid-range, not max.
    phase_3: |
      Tendril AoE target swap if assigned a tendril. Burst CDs on Venom
      Withdrawal window:
      - Frost Mage: Icy Veins + Mirror Image
      - Aff Warlock: Soulburn-Haunt + Demon Soul
      - MM Hunter: Rapid Fire + Readiness + Bestial Wrath (BM only)
      - Shadow Priest: Shadowfiend + Archangel
      - Balance Druid: Starfall + Force of Nature trees
      - Ele Shaman: Bloodlust/Heroism + Elemental Mastery + Fire Elemental

  cd_use:
    save_for_phase_3: |
      Hold ALL major offensive CDs for Venom Withdrawal in Phase 3.
      Boss takes +100% damage and is stunned — biggest burst window of the
      fight.
    defensive_for_whispers: |
      If Whispers lands on you and kick missed, pop defensive:
      - Frost Mage: Iceblock (5min CD) — interrupts Whispers BUFF on you
      - Aff Warlock: Demonic Circle for re-position (light defense)
      - MM Hunter: Deterrence (-100% direct dmg 5s)
      - Shadow Priest: Dispersion (-90% damage 6s)
      - Balance Druid: Barkskin (-20% damage 12s)
      - Ele Shaman: Stoneclaw Totem (talented shield) / Shamanistic Rage (spec lock — Enh only)

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT cluster ranged in Phase 1 (Toxic Link explosion hazard)
    - DO NOT stand at max range in Phase 2 (acid pool target placement)
    - DO NOT spend major CDs before Phase 3
    - DO NOT cast hardcast spells while feared / linked-running (movement breaks cast)
    - DO NOT stand on altar in Phase 3 (poison floor)
```

---

## Boss 2 — Bloodlord Mandokir

```yaml
boss_ranged:
  boss: bloodlord_mandokir
  difficulty_grade: medium (target-swap)

  positioning:
    default: max range; SPREAD moderately (Decapitate is random target)
    facing: boss / Ohgan during swap
    movement: minimal — Devastating Slam dodge laterally

  interrupt_role:
    none: Devastating Slam non-interruptible (UNVERIFIED)

  rotation_modifications:
    standard: |
      Standard ST rotation on Mandokir interleaved with Ohgan target swaps.
    ohgan_swap: |
      OHGAN KILL PRIORITY ABSOLUTE on raptor spawn / Reanimate event.
      DPS swap:
      - Frost Mage: drop boss; full rotation on Ohgan; Frostbolt + Ice Lance
        spam; Pet (Water Elemental) auto-target updates
      - Aff Warlock: DoTs RESET on Ohgan (target swap loss). Only re-DoT if
        Ohgan HP > 30%; otherwise nuke with Shadow Bolt + Drain Soul
      - MM Hunter: Hunter's Mark on Ohgan; Steady Shot + Aimed Shot rotation
      - Shadow Priest: DoTs RESET on Ohgan; same heuristic as Warlock
      - Balance Druid: Moonfire + Sunfire RESET; Wrath/Starfire spam
      - Ele Shaman: Flame Shock + LvB combo for fast burst on Ohgan
    decapitate_on_self: |
      If self-targeted by Decapitate (telegraph signal), pop class immunity:
      - Frost Mage: Iceblock — IMMUNE to Decapitate (5min CD)
      - Aff Warlock: NO direct immunity — Soulstone self-revive instead
      - MM Hunter: Deterrence — UNVERIFIED if blocks Decapitate (physical
        damage; Deterrence has 100% deflect chance; should work)
      - Shadow Priest: Dispersion -90% but NOT immunity — likely still kills
      - Balance Druid: NO direct immunity — accept death + revive
      - Ele Shaman: NO direct immunity — accept death + revive

  cd_use:
    standard_offensive: rotational; stack CDs on Frenzy phase (20% HP) for burst-kill
    defensive_for_decapitate: see above (Iceblock primary)

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT continue DPS on Mandokir while Ohgan is up (Ohgan kills spirits)
    - DO NOT stand in Devastating Slam furrow
    - DO NOT spend big offensive CDs on Ohgan (waste — Ohgan dies fast)
    - DO NOT panic on Decapitate — pop Iceblock (Mage) or accept revive
```

---

## Boss 3 — High Priestess Kilnara

```yaml
boss_ranged:
  boss: high_priestess_kilnara
  difficulty_grade: medium-hard (interrupt + AoE)

  positioning:
    panther_clear: at range from cluster; AoE-cleave panthers
    boss_phase_1: >12y from Kilnara (outside Tears of Blood radius)
    boss_phase_2: max range; mobile to dodge leap targeting + Wave of Agony cone
    facing: boss
    movement: moderate (cone dodging)

  interrupt_role:
    tears_of_blood: PRIMARY — participate in kick rotation (group AoE if missed)
    shadow_bolt: SECONDARY — kick if rotation has slack

  rotation_modifications:
    panther_clear: |
      AoE rotation on each cluster:
      - Frost Mage: Blizzard (channel) + Frostfire Bolt cleave + Frost Nova
        for kiting if cluster fixates
      - Aff Warlock: Seed of Corruption multi-spread + Bane of Havoc on
        secondary target for cleave
      - MM Hunter: Multi-Shot + Trap Launcher Explosive Trap; pet AoE-tank
      - Shadow Priest: Mind Sear if 4+ targets (poor in 4.x, but available);
        Shadow Word: Pain spread + Vampiric Touch on primary
      - Balance Druid: Hurricane channel + Wild Mushroom detonation
      - Ele Shaman: Magma Totem + Earthquake at 5+ targets; Chain Lightning
    boss_phase_1: |
      ST rotation; kick rotation participation for Tears of Blood.
    boss_phase_2: |
      ST rotation with leap-attack dodge mobility. Ranged classes that
      cast-while-moving have advantage here.

  cd_use:
    save_offensive_for_phase_2: hold one major CD for Phase 2 (panther leap chains)
    aoe_cd_for_panther_clusters: |
      Use AoE CDs strategically — Mage Mirror Image NOT useful (single
      target); Druid Force of Nature (DPS trees) NOT in 4.x; Hunter
      Readiness reset for trap reuse

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT stand within 12y of Kilnara during Tears of Blood radius
    - DO NOT hit sleeping panthers with cleave/AoE during Kilnara DPS
    - DO NOT push Kilnara to 50% with panthers asleep (un-healable Phase 2)
    - DO NOT spend Bloodlust pre-pull (save for panther clear or Phase 2)
```

---

## Boss 4 — Zanzil

```yaml
boss_ranged:
  boss: zanzil
  difficulty_grade: medium (cauldron-mechanic + interrupt)

  positioning:
    default: max range; AVOID Zanzili Fire ground line
    facing: boss
    movement: minimal — line dodge only

  interrupt_role:
    voodoo_bolt: PRIMARY — participate in kick rotation

  rotation_modifications:
    standard: |
      Standard ST rotation interleaved with cauldron drinks on elixir cycles.
    blue_elixir_berserker: |
      Berserker is melee-fixate. Ranged contributes ranged DPS / kiting:
      - Frost Mage: Frost Nova root + slow + cast on
      - Aff Warlock: Curse of Tongues + DoT rotation
      - MM Hunter: Concussive Shot slow + cast-while-moving on
      - Shadow Priest: standard ST DoT rotation
      - Balance Druid: Cyclone if fixated player is in trouble
      - Ele Shaman: Earthbind Totem + Flame Shock + LB
    red_elixir_zombies: |
      AoE rotation on zombie cluster — same heuristics as Kilnara panther clear
    green_elixir_graveyard_gas: |
      DRINK TOXIC TORMENT CAULDRON. Gas tick is mitigated -90% nature dmg.
      Continue rotation while in gas.

  cd_use:
    standard_offensive: rotational; align with red-elixir AoE cycles for waste-free CDs
    save_for_jindo: hold major CDs

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT stand in Zanzili Fire line
    - DO NOT skip Toxic Torment drink on Green elixir (gas kills you)
    - DO NOT drink Frostburn (boss-target unsafe; ranged isn't melee-cleave anchor)
    - DO NOT continue boss DPS during Berserker fixate on you (kite or burst)

  bot_implementation_flag: |
    Cauldron is a clickable GameObject. Ranged bot must be able to:
      - Pathfind to Toxic Torment cauldron on Green elixir cast
      - Use the cauldron object
      - Resume rotation after acquiring buff
```

---

## Boss 5 — Jin'do the Godbreaker

```yaml
boss_ranged:
  boss: jin_do_the_godbreaker
  difficulty_grade: hard (Deadzone timing + parallel zone)

  positioning:
    phase_1_default: max range from Jin'do; OUT of Deadzone (90% cast speed = no DPS)
    phase_1_during_shadows_of_hakkar: |
      MOVE INTO active Deadzone before 3s cast resolves. Inside = -90%
      magic dmg (survivable); outside = ~120k death.
      ENCOUNTER-SPECIFIC OVERRIDE — normally avoid ground AoE.
    phase_2_default: |
      Spirit Realm. Ranged advantage — easier Shadow Spike dodge than
      melee. Cleave on Twisted Spirits while ST'ing chains.
    phase_2_chain_burst: |
      When Brittle Barrier removed (Sunder Rift on chain), full burst on
      that chain. Window is short.

  interrupt_role:
    none: Shadows of Hakkar non-interruptible (mitigated, not stopped)

  rotation_modifications:
    phase_1_normal: |
      Max-range standard ST rotation. Stop-cast on Shadows of Hakkar
      cast-start; move into Deadzone; resume IF Deadzone allows (-90% cast
      speed makes hardcasts useless, but instants still fire).
    phase_1_in_deadzone_during_shadows: |
      INSTANT spells only (cast speed -90% renders hardcasts impractical):
      - Frost Mage: Ice Lance (instant), Fire Blast (instant)
      - Aff Warlock: DoTs (Corruption / Bane / UA / Curse)
      - MM Hunter: cast-while-moving Steady Shot? UNVERIFIED — may still fire
      - Shadow Priest: SW: Pain / VT / SW: Death (instant)
      - Balance Druid: Moonfire / Sunfire (instant)
      - Ele Shaman: Lava Burst (UNVERIFIED — 2.0s base cast affected by cast speed?)
    phase_2_chain_dps: |
      Chain has Brittle Barrier; un-attackable until Sunder Rift hits the
      chain location. When window opens, full burst:
      - Frost Mage: Icy Veins + Frostbolt + Frostfire Bolt
      - Aff Warlock: Drain Soul + Haunt + DoTs
      - MM Hunter: Rapid Fire + Aimed Shot
      - Shadow Priest: Devouring Plague + Mind Blast
      - Balance Druid: Eclipse-state nuke
      - Ele Shaman: Lava Burst + Lightning Bolt
    phase_2_twisted_spirits_aoe: |
      AoE rotation on wave; same heuristics as Kilnara panther clear

  cd_use:
    phase_1_burst: standard rotation; no save
    phase_2_entry: |
      Major offensive CDs on Spirit Realm entry — fast Twisted Spirit clear
      + first chain burst. Bloodlust on entry (UNVERIFIED — may save for
      final phase return)
    phase_2_per_chain: rotate medium CDs on each chain-break window

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT stand outside Deadzone during Shadows of Hakkar (~120k death)
    - DO NOT stand inside Deadzone outside Shadows of Hakkar (-90% cast speed)
    - DO NOT continue ST on Jin'do during chain burst window (chain is the priority)
    - DO NOT ignore Twisted Spirit waves (soft enrage)

  bot_implementation_flag_phase2: |
    Phase 2 parallel-zone teleport. Ranged DPS bot must:
      - Maintain target reference across zone change
      - Re-acquire chains, spirits, and Twisted Spirits on Spirit Realm side
      - Continue rotation logic on the other side
      - Detect return to physical realm
    Without this, Phase 2 is un-completable.

  bot_implementation_flag_phase1: |
    Deadzone in/out timing override. Bot must:
      - Detect Shadows of Hakkar cast start → MOVE INTO Deadzone
      - Detect cast end → MOVE OUT
```

---

## Optional Boss — Cache of Madness

```yaml
boss_ranged:
  boss: cache_of_madness
  optional: true
  default_skip: true

  positioning_per_variant:
    hazza_rah: range; AoE on illusion adds at 66/33%
    renataki: max range to avoid Thousand Blades cone; kite during Vanish
    wushoolay: SPREAD (Forked Lightning chain); dodge Lightning Cloud zones
    gri_lek: range; dodge Rupture Line ground; dispel Entangling Roots if class has Nature dispel

  bot_anti_patterns:
    - DO NOT engage if Archaeology not unlocked
```

---

## Bot AI hooks

```yaml
ai_hooks:
  venoxis:
    - encounter_detect_whispers_cast → kick_rotation_participate
    - encounter_state_self_toxic_linked → break_to_25y
    - encounter_state_phase_2_active → mid_range_position (NOT max range)
    - encounter_state_phase_3_active → reserve_offensive_cds_for_venom_withdrawal
    - encounter_detect_venom_withdrawal_active → BURST_ALL_CDS

  mandokir:
    - encounter_detect_ohgan_active → SWAP_TARGET_TO_OHGAN
    - encounter_detect_decapitate_telegraph_self → POP_CLASS_IMMUNITY (Iceblock for Mage; accept death otherwise)
    - encounter_state_frenzy_phase → burst_offensive_cds

  kilnara:
    - encounter_detect_tears_of_blood_cast → kick_rotation_participate
    - encounter_state_panther_clear → swap_to_aoe_rotation
    - encounter_state_kilnara_phase_2 → swap_back_to_st + cone_dodge

  zanzil:
    - encounter_detect_voodoo_bolt_cast → kick_rotation_participate
    - encounter_detect_green_elixir → DRINK_TOXIC_TORMENT (bot_implementation_flag)
    - encounter_detect_red_elixir → swap_to_aoe_rotation_for_zombies
    - encounter_detect_zanzili_fire_line → step_off_line

  jindo:
    - encounter_detect_shadows_of_hakkar_cast → MOVE_INTO_DEADZONE (encounter override)
    - encounter_detect_shadows_of_hakkar_resolved → MOVE_OUT_OF_DEADZONE
    - encounter_detect_phase_2_transition → handle_zone_change (bot_implementation_flag)
    - encounter_detect_brittle_barrier_removed(chain) → BURST_CHAIN_DPS
    - encounter_detect_twisted_spirit_wave → swap_to_aoe_rotation

dispel_blacklist: []                           # ranged bots typically don't dispel; included for completeness
```
