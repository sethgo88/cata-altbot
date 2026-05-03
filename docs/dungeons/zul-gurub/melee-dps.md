# Zul'Gurub — Melee DPS

**Melee-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how melee DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content melee DPS patterns (positioning, interrupt rotation, threat ceiling) live in `docs/roles/melee-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (combat-rogue, fury-warrior, ret-paladin, frost-dk, enh-shaman, feral-cat)
- This doc layers ZG-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_melee_dps_overview:
  total_bosses: 5 (+ 1 optional Cache of Madness, default skip)
  estimated_full_clear_minutes: 45-60 (launch-tier) / 25-35 (geared)
  difficulty_tier: HEROIC_ONLY
  positioning_profile:
    venoxis: behind_boss_mid_range_phase_1 / cone_dodge_phase_2 / kite_tendrils_phase_3
    mandokir: behind_boss / swap_to_ohgan_on_spawn
    kilnara: cluster_panther_aoe / behind_boss_phase_2
    zanzil: behind_boss_off_zanzili_fire_line / cauldron_drinking
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
    - target_swap_to_ohgan (mandokir) — CP/disease reset
    - aoe_panther_clear (kilnara pre-engage)
    - cauldron_drink_frostburn_or_burning_blood (zanzil)
    - deadzone_in_out_timing (jindo phase 1)
    - chain_burst_phase_2 (jindo)
```

---

## Trash — melee DPS notes

```yaml
trash_melee:
  voodoo_acolytes:
    cc_priorities: |
      Sap (Rogue) and Hammer of Justice (Pally) for opener-CC. Primary role
      is kill-ordered DPS on skull marker.
    interrupt_role: |
      - Combat Rogue: Kick (10s CD; off-GCD)
      - Fury Warrior: Pummel (10s CD; off-GCD)
      - Ret Pally: Rebuke (15s CD; off-GCD)
      - Frost DK: Mind Freeze (10s CD; off-GCD)
      - Enh Shaman: Wind Shear (6s CD; off-GCD; 25y)
      - Feral Cat: Skull Bash (60s CD; gap-close + interrupt)

  razzashi_raptors:
    notes: pack-pulls; AoE rotation cleave
  bethekk_acolytes:
    cc_priorities: same as Voodoo Acolytes
  zanzili_pre_packs:
    notes: |
      First pack: tank Burning Blood AoE; melee in melee for cleave.
      Second pack (Rabid Gurubashi fixate): kite if fixated; otherwise
      burst with Frostburn cauldron pre-strike.
```

---

## Boss 1 — High Priest Venoxis

```yaml
boss_melee:
  boss: high_priest_venoxis
  difficulty_grade: hard (movement + interrupt)

  positioning:
    phase_1: |
      Behind boss; navigate poison maze. SPREAD if Toxic Linked with another
      melee — walk OUT of melee range to break (significant DPS loss but
      mandatory).
    phase_2: |
      Behind boss for Breath of Hethiss cone (15y forward). Boss has
      +50% physical damage taken in this phase — full burst window.
    phase_3: |
      Mobile — kite/burst tendrils. Cluster for Venom Withdrawal burst
      window.
    facing: behind boss
    movement: high

  interrupt_role:
    whispers_of_hethiss: |
      PARTICIPATE — primary kick target. Class kicks:
      - Combat Rogue: Kick (10s CD; off-GCD)
      - Fury Warrior: Pummel (10s CD; off-GCD)
      - Ret Pally: Rebuke (15s CD; off-GCD)
      - Frost DK: Mind Freeze (10s CD; off-GCD)
      - Enh Shaman: Wind Shear (6s CD; off-GCD; 25y)
      - Feral Cat: Skull Bash (60s CD; gap-close + interrupt)

  rotation_modifications:
    phase_1: |
      Standard ST rotation. Stop on Toxic Link spread movement.
    phase_2: |
      ST rotation; +50% physical taken from melee — BURST WINDOW for
      physical melee classes especially:
      - Combat Rogue: Adrenaline Rush + Killing Spree if available
      - Fury Warrior: Recklessness + Death Wish + Bloodbath
      - Ret Pally: Avenging Wrath + Zealotry + Guardian of Ancient Kings
      - Frost DK: Pillar of Frost + Raise Dead (ghoul) — Killing Machine procs golden
      - Enh Shaman: Feral Spirit + Shamanistic Rage + Spirit Walker's Grace for cast-while-move
      - Feral Cat: Tiger's Fury + Berserk + Glyph of Berserk for energy regen
    phase_3: |
      Tendril swap if assigned; otherwise burst Venoxis on Venom Withdrawal:
      - All melee: stack offensive CDs with Bloodlust if not used earlier

  cd_use:
    phase_2_burst: |
      Use offensive CDs on Phase 2 entry — +50% physical damage on boss is
      free DPS.
    phase_3_venom_withdrawal: |
      Stack remaining CDs + group Bloodlust on Venom Withdrawal stun
      window. Boss takes +100% damage stunned.
    defensive_for_whispers: |
      If Whispers lands on you and kick missed, pop defensive:
      - Combat Rogue: Cloak of Shadows (90s CD; -100% magic 5s)
      - Fury Warrior: Spell Reflection (10s CD; reflects single nature DoT? UNVERIFIED — channel may not reflect)
      - Ret Pally: Divine Protection (-20% magic 10s)
      - Frost DK: Anti-Magic Shell (75% magic absorb 5s) — STRONG
      - Enh Shaman: Shamanistic Rage + Stoneclaw Totem
      - Feral Cat: Survival Instincts (-50% damage 12s; cat form)

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT stand in front of boss in Phase 2 (Breath of Hethiss cone)
    - DO NOT stand in poison maze (Phase 1) or acid pool (Phase 2)
    - DO NOT bunker-melee through Toxic Link (move out if linked)
    - DO NOT spend major CDs before Phase 2 (boss has +50% physical taken there)
    - DO NOT stand on altar in Phase 3 (poison floor)
```

---

## Boss 2 — Bloodlord Mandokir

```yaml
boss_melee:
  boss: bloodlord_mandokir
  difficulty_grade: medium-hard (target swap + immunity timing)

  positioning:
    default: behind boss; SPREAD slightly (Decapitate is random)
    facing: behind boss
    movement: lateral dodges on Devastating Slam

  interrupt_role:
    none: Devastating Slam non-interruptible (UNVERIFIED)

  rotation_modifications:
    standard: |
      Standard ST rotation; periodic Ohgan target swap.
    ohgan_swap: |
      OHGAN KILL PRIORITY ABSOLUTE on raptor spawn / Reanimate event.
      Buffs that are TARGET-BOUND reset on swap:
      - Combat Rogue: CP attached to target — RESET on target swap (lose all
        CP). Build new CP on Ohgan; fast-kill (Ohgan low HP); switch back to
        Mandokir; rebuild CP again. SnD self-buff persists (apply pre-pull).
      - Fury Warrior: most buffs self (Enrage / Death Wish); rage carries.
        Just swap targets and continue.
      - Ret Pally: Inquisition self-buff persists; Holy Power resets target.
        Re-apply DoTs on Ohgan if rotation calls.
      - Frost DK: Diseases on target — re-Outbreak on Ohgan; old target
        diseases don't matter (Ohgan dies fast).
      - Enh Shaman: Lightning Shield self-buff; Maelstrom self-resource.
        Easy swap.
      - Feral Cat: CP on target — RESET. Re-apply Mangle + Rake + Rip on
        Ohgan if HP > 30%; else just slash. Savage Roar self-buff persists.
    decapitate_on_self: |
      If self-targeted, pop class immunity:
      - Combat Rogue: Cloak of Shadows — UNVERIFIED if blocks physical
        Decapitate (Cloak is magic-only). PROBABLY DOES NOT BLOCK.
      - Fury Warrior: Shield Wall (talent? UNVERIFIED Fury) / -no immunity
      - Ret Pally: Divine Shield (Bubble) — IMMUNE; 5min CD
      - Frost DK: Lichborne (10s) — UNVERIFIED if blocks physical
      - Enh Shaman: NO direct immunity
      - Feral Cat: NO direct immunity — accept revive

  cd_use:
    standard_offensive: rotational
    frenzy_phase_burst: stack remaining offensive CDs at 20% HP for burn-down
    defensive_for_decapitate: see above (Bubble for Pally)

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT continue Mandokir DPS while Ohgan up (Ohgan kills spirits)
    - DO NOT stand in Devastating Slam furrow (laterally dodge)
    - DO NOT spend big offensive CDs on Ohgan (Ohgan dies fast — wasted)
    - DO NOT pre-burn offensive CDs before Frenzy phase (save for execute)
```

---

## Boss 3 — High Priestess Kilnara

```yaml
boss_melee:
  boss: high_priestess_kilnara
  difficulty_grade: medium-hard (interrupt + AoE)

  positioning:
    panther_clear: in melee on cluster; tank stacks panthers for cleave
    boss_phase_1: behind boss
    boss_phase_2: behind boss; mobile for leap dodging
    facing: behind boss
    movement: moderate (cone dodge + leap dodge in Phase 2)

  interrupt_role:
    tears_of_blood: PRIMARY — participate in kick rotation
    shadow_bolt: SECONDARY — kick if rotation has slack

  rotation_modifications:
    panther_clear: |
      AoE rotation on each cluster:
      - Combat Rogue: Blade Flurry on; Fan of Knives spam at 3+ targets
      - Fury Warrior: Whirlwind / Bladestorm on cluster
      - Ret Pally: Divine Storm at 3 HP; Hammer of the Righteous opener
      - Frost DK: Howling Blast spam (cleave); Death and Decay placement
      - Enh Shaman: Magma Totem + Chain Lightning at 5 MW + Stormstrike for cleave
      - Feral Cat: Swipe (Cat) on cluster; maintain Rip on primary target
    boss_phase_1: ST rotation; kick on Tears of Blood
    boss_phase_2: |
      ST rotation; mobility-aware:
      - Combat Rogue: Sprint for re-engage; Vanish for HP threat reset (rare)
      - Fury Warrior: Charge / Heroic Leap for re-engage
      - Ret Pally: Hand of Freedom self for movement immunity
      - Frost DK: Death's Advance + Death Grip pulls
      - Enh Shaman: Spirit Walker's Grace (cast while moving)
      - Feral Cat: Stampeding Roar (raid speed); Dash

  cd_use:
    panther_clear_aoe_cd: stack AoE CDs on largest cluster
    save_for_phase_2: hold one major CD for Phase 2 (panther leap chains)

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT cleave-hit sleeping panthers during Kilnara DPS
    - DO NOT push Kilnara to 50% with panthers asleep
    - DO NOT stand in front of Kilnara (Wave of Agony cone)
    - DO NOT stand within 12y of Kilnara during Tears of Blood (melee will be in radius — kick is the answer)
```

---

## Boss 4 — Zanzil

```yaml
boss_melee:
  boss: zanzil
  difficulty_grade: medium (cauldron mechanic + interrupt)

  positioning:
    default: behind boss; OFF Zanzili Fire ground line
    facing: behind boss
    movement: minimal (line dodge + cauldron drinks)

  interrupt_role:
    voodoo_bolt: PRIMARY — participate in kick rotation

  rotation_modifications:
    standard: |
      Standard ST rotation interleaved with cauldron drinks.
    blue_elixir_berserker_with_frostburn: |
      DRINK FROSTBURN CAULDRON, then strike Berserker with next attack.
      15s stun + 65% max HP dmg = burst-kill.
      - Combat Rogue: SS or Ambush opener post-Frostburn
      - Fury Warrior: Bloodthirst or Heroic Strike post-Frostburn
      - Ret Pally: Crusader Strike post-Frostburn
      - Frost DK: Obliterate post-Frostburn
      - Enh Shaman: Stormstrike post-Frostburn
      - Feral Cat: Mangle (Cat) post-Frostburn
    red_elixir_zombies: |
      DRINK BURNING BLOOD CAULDRON; cluster zombies in melee for AoE pulse.
      AoE rotation on cluster (same as Kilnara panther clear).
    green_elixir_graveyard_gas: |
      DRINK TOXIC TORMENT CAULDRON. Continue rotation in mitigated gas.

  cd_use:
    standard_offensive: rotational
    align_with_burst_windows: stack CDs on Berserker burst (Frostburn) or AoE phases (Burning Blood)

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT drink Frostburn and then attack Zanzil first (waste of buff, possibly unsafe)
    - DO NOT stand in Zanzili Fire line
    - DO NOT skip Toxic Torment on Green elixir
    - DO NOT continue boss DPS during Berserker fixate on group (kill Berserker first)

  bot_implementation_flag: |
    Cauldron is a clickable GameObject. Melee bot must:
      - Pathfind to the right cauldron based on elixir color
      - Use the cauldron object (interact action)
      - Resume rotation after acquiring buff (with the right next-attack target)
    Without this, Zanzil is un-completable autonomously.
```

---

## Boss 5 — Jin'do the Godbreaker

```yaml
boss_melee:
  boss: jin_do_the_godbreaker
  difficulty_grade: hard (Deadzone timing + parallel zone)

  positioning:
    phase_1_default: behind Jin'do; OUT of Deadzone normally
    phase_1_during_shadows_of_hakkar: |
      MOVE INTO active Deadzone before 3s cast resolves.
      ENCOUNTER OVERRIDE — normally avoid ground AoE.
    phase_2_default: |
      Spirit Realm. Melee on chains when barrier removed; AoE on Twisted
      Spirits between barrier windows.
    phase_2_chain_burst: |
      When Brittle Barrier removed (Sunder Rift on chain), full burst.
      Window is short.

  interrupt_role:
    none: Shadows of Hakkar non-interruptible

  rotation_modifications:
    phase_1_normal: |
      Standard ST rotation behind boss.
    phase_1_in_deadzone_during_shadows: |
      INSTANT spells / melee strikes only:
      - Combat Rogue: SS / Eviscerate / Slice and Dice (instant CP-spend)
      - Fury Warrior: Bloodthirst / Raging Blow / Whirlwind (instant)
      - Ret Pally: CS / Templar's Verdict / Inquisition (instant)
      - Frost DK: Obliterate / Frost Strike / Howling Blast (instant from
        rune resources)
      - Enh Shaman: Stormstrike / Lava Lash / Earth Shock (mostly instant);
        Lightning Bolt at 5 MW (cast-while-move via SWG)
      - Feral Cat: all abilities instant (Mangle / Shred / Rip / Rake / FB)
    phase_2_chain_burst_window: |
      Full burst on chain — instant access since melee is on the chain
      already:
      - Combat Rogue: Adrenaline Rush + Killing Spree on chain
      - Fury Warrior: Recklessness + Death Wish on chain
      - Ret Pally: Avenging Wrath + Zealotry + GoAK on chain
      - Frost DK: Pillar of Frost + Killing Machine procs
      - Enh Shaman: Feral Spirit + Shamanistic Rage + Searing Totem
      - Feral Cat: Tiger's Fury + Berserk on chain
    phase_2_twisted_spirits_aoe: |
      Cleave AoE on wave between chain windows.

  cd_use:
    phase_2_entry: |
      Major offensive CDs on Spirit Realm entry — fast Twisted Spirit clear
      + first chain burst.
    phase_2_per_chain: rotate medium CDs on each chain window
    bloodlust_timing: group call; usually Phase 2 entry or final phase

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT stand outside Deadzone during Shadows of Hakkar
    - DO NOT stand inside Deadzone outside Shadows of Hakkar (rotation is fine since melee instants don't care, but you take other ambient mechanics ineffectively healed)
    - DO NOT continue ST on Jin'do during chain-break window in Phase 2
    - DO NOT ignore Twisted Spirits (soft enrage)

  bot_implementation_flag_phase2: |
    Phase 2 parallel-zone teleport. Melee bot must:
      - Maintain target reference across zone change
      - Re-acquire chains, spirits, Twisted Spirits on Spirit Realm side
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
boss_melee:
  boss: cache_of_madness
  optional: true
  default_skip: true

  positioning_per_variant:
    hazza_rah: melee on illusion adds at 66/33%; ST on boss between
    renataki: side/rear (Thousand Blades cone); kite during Vanish
    wushoolay: SPREAD (Forked Lightning chain); avoid Lightning Cloud
    gri_lek: melee in melee; dodge Rupture Line (ground waves)

  bot_anti_patterns:
    - DO NOT engage if Archaeology not unlocked
```

---

## Bot AI hooks

```yaml
ai_hooks:
  venoxis:
    - encounter_detect_whispers_cast → kick_rotation_participate
    - encounter_state_self_toxic_linked → break_to_25y (move out of melee if linked with another melee)
    - encounter_state_phase_2_active → BURST_OFFENSIVE_CDS (+50% physical taken)
    - encounter_state_phase_3_active → save_remaining_cds_for_venom_withdrawal
    - encounter_detect_venom_withdrawal_active → BURST_ALL_CDS

  mandokir:
    - encounter_detect_ohgan_active → SWAP_TARGET_TO_OHGAN
    - encounter_detect_ohgan_dead → SWAP_BACK_TO_MANDOKIR
    - encounter_detect_decapitate_telegraph_self → POP_CLASS_IMMUNITY (Bubble for Pally; accept death otherwise)
    - encounter_state_frenzy_phase → burst_offensive_cds

  kilnara:
    - encounter_detect_tears_of_blood_cast → kick_rotation_participate
    - encounter_state_panther_clear → swap_to_aoe_rotation (Blade Flurry / WW / Divine Storm / etc.)
    - encounter_state_kilnara_phase_2 → swap_back_to_st + cone_dodge

  zanzil:
    - encounter_detect_voodoo_bolt_cast → kick_rotation_participate
    - encounter_detect_blue_elixir → DRINK_FROSTBURN AND target_berserker (bot_implementation_flag)
    - encounter_detect_red_elixir → DRINK_BURNING_BLOOD AND swap_to_aoe_rotation
    - encounter_detect_green_elixir → DRINK_TOXIC_TORMENT
    - encounter_detect_zanzili_fire_line → step_off_line

  jindo:
    - encounter_detect_shadows_of_hakkar_cast → MOVE_INTO_DEADZONE (encounter override)
    - encounter_detect_shadows_of_hakkar_resolved → MOVE_OUT_OF_DEADZONE
    - encounter_detect_phase_2_transition → handle_zone_change (bot_implementation_flag)
    - encounter_detect_brittle_barrier_removed(chain) → BURST_CHAIN_DPS
    - encounter_detect_twisted_spirit_wave → swap_to_aoe_rotation

dispel_blacklist: []                           # melee bots typically don't dispel; included for completeness on hybrid melee (Ret Pally Cleanse, Enh Shaman Cleanse Spirit)
```
