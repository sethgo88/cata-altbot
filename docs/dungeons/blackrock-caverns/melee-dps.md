# Blackrock Caverns — Melee DPS

**Melee-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how melee DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content melee DPS patterns (positioning, interrupt rotation, threat ceiling) live in `docs/roles/melee-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (combat-rogue, fury-warrior, ret-paladin, frost-dk, enh-shaman, feral-cat)
- This doc layers BRC-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_melee_dps_overview:
  total_bosses: 4 (+ 1 optional)
  estimated_full_clear_minutes: 25-45
  positioning_profile:
    rom_ogg: stacked_at_melee             # group-stack for Skullcracker share
    corla: beam_intercept_melee           # melee one drakonid while intercepting beam
    karsh: behind_outside_plume           # standard behind-target, but DON'T chase tank into plume
    obsidius: behind_active_unit          # target-swap aware
  interrupt_load:
    rom_ogg: high                         # Quake every 10-15s
    corla: zero                           # no kicks
    karsh: zero                           # no kicks
    obsidius: zero                        # no confirmed kicks
  key_responsibilities:
    - quake_interrupt_rotation (rom_ogg)
    - beam_intercept_assigned (corla)
    - position_outside_plume (karsh)
    - target_swap_on_clone_rotation (obsidius)
```

---

## Trash — melee DPS notes

```yaml
trash_melee:
  twilight_humanoids:
    cc_priorities: |
      Polymorph / Sap available targets first if multi-CC team. Melee bot
      contributes Sap (Rogue) or Hammer of Justice (Pally) to opener-CC; primary
      role is kill-ordered DPS on skull marker.
  fear_packs:
    notes: |
      Twilight Sadists fear-cast. Melee in range MUST kick on rotation:
      - Combat Rogue: Kick (10s CD)
      - Fury Warrior: Pummel (10s CD; off-GCD)
      - Ret Pally: Rebuke (15s CD; off-GCD)
      - Frost DK: Mind Freeze (10s CD; off-GCD)
      - Enh Shaman: Wind Shear (6s CD; off-GCD; 25y range)
      - Feral Cat: Skull Bash (60s CD; gap-close + interrupt)
  quicksilver_tank:
    positioning: stand SIDE/REAR — frontal cleave hazard
```

---

## Boss 1 — Rom'ogg Bonecrusher

```yaml
boss_melee:
  boss: rom_ogg_bonecrusher
  difficulty_grade: easy
  positioning:
    default: stacked_at_melee under boss (default for melee anyway)
    facing: any (no frontal cone)
    movement: stay-still — Chains of Woe pulls everyone in regardless
  interrupt_role:
    quake: PRIMARY — melee DPS interrupt rotation (Quake on ~10-15s; rotate kicks)
    chains_of_woe: DO NOT INTERRUPT (mechanic-intended)
    skullcracker: DO NOT INTERRUPT (channel; not interruptible anyway)
  rotation_modifications:
    skullcracker_channel: |
      Continue DPS during channel — damage-share fight is best done by full
      stacked group dealing damage simultaneously.
    add_wave_at_50: |
      Switch to AoE rotation on add spawn:
      - Combat Rogue: Blade Flurry on; Fan of Knives if 3+ adds
      - Fury Warrior: Bladestorm or Whirlwind on add cluster
      - Ret Pally: Divine Storm at 3 HP
      - Frost DK: Howling Blast spam
      - Enh Shaman: Magma Totem + Chain Lightning at 5 MW
      - Feral Cat: Swipe (Cat) + maintain primary target Rip
  cd_use:
    standard_offensive: standard rotational on engagement; no special hold
  heroic_delta: |
    - Quake interrupt mandatory; missed kick spikes healer mana
    - Add wave possibly larger
  bot_anti_patterns:
    - DO NOT dodge Skullcracker (mechanic intended; stack)
    - DO NOT interrupt Chains of Woe (intended pull)
```

---

## Boss 2 — Corla, Herald of Twilight

```yaml
boss_melee:
  boss: corla_herald_of_twilight
  difficulty_grade: medium (positional)
  positioning:
    default: |
      Take CLOSEST drakonid platform; intercept that drakonid's beam by
      standing between drakonid and Corla. Melee the drakonid while in beam.
    facing: drakonid (in beam range)
    movement: |
      Step out of beam at stack threshold:
      - Normal: stack 4 (out for ~10s, swap back if needed)
      - Heroic: stack 3 (mandatory two-player rotation per beam)
  interrupt_role:
    none: no kick-required casts
  rotation_modifications:
    in_beam: |
      Continue DPS on assigned drakonid. Survival cooldowns when health drops:
      - Combat Rogue: Cloak of Shadows (90s) for shadow-school resist on beam damage
      - Fury Warrior: Spell Reflection if facing drakonid (questionable; UNVERIFIED beam reflectability)
      - Ret Pally: Divine Protection (-20% magic 10s)
      - Frost DK: Anti-Magic Shell (75% magic absorb 5s) — SIGNIFICANT mitigation
      - Enh Shaman: Shamanistic Rage + Stoneclaw Totem
      - Feral Cat: Survival Instincts (-50% damage 12s)
    out_of_beam: |
      Continue DPS on drakonid from melee range; don't run far. Stack drops
      at ~1/sec.
    after_drakonids: standard ST rotation on Corla
  cd_use:
    save_major_for_intercept: pop major defensive when in beam if HP drops below 60%
  heroic_delta: |
    - Two-DPS-per-beam rotation; melee bot must coordinate swap-in
    - Beam damage higher
  bot_anti_patterns:
    - DO NOT stand in TWO beams simultaneously (overlap = 2x stack rate)
    - DO NOT kill Corla before drakonids
    - DO NOT melee Corla while drakonids alive (positioned near beam, not Corla)
```

---

## Boss 3 — Karsh Steelbender

```yaml
boss_melee:
  boss: karsh_steelbender
  difficulty_grade: medium (positional)
  positioning:
    default: BEHIND boss, OUTSIDE central plume
    facing: boss back; rotate when tank rotates boss
    movement: |
      KEY RULE — DO NOT FOLLOW TANK INTO PLUME during kite-pass.
      Default melee "stay behind boss" is the correct rule with one
      exception: when tank kites boss through plume, melee bot waits
      outside the plume and re-engages when boss exits.
  interrupt_role:
    cinderbreath: GENERALLY NON-INTERRUPTIBLE; one source disagrees (UNVERIFIED) — try if your kick is off CD
  rotation_modifications:
    standard: |
      Standard ST rotation; kite-pass causes ~3-5s no-melee window per
      cadence. Use that gap for:
      - Combat Rogue: Sprint to re-engage faster; refresh Slice and Dice via 1-CP
      - Fury Warrior: Charge / Heroic Leap to re-engage; rage will be high coming back
      - Ret Pally: Hammer of Wrath if HP-execute window; otherwise wait for boss exit
      - Frost DK: Death Grip to re-pull boss to original position (UNVERIFIED — may interfere with tank kite)
      - Enh Shaman: ranged maelstrom-LB during gap; Ghost Wolf if needed for re-engage
      - Feral Cat: Stampeding Roar (raid speed) if mid-gap movement; Skull Bash to gap-close
  cd_use:
    standard_offensive: rotational; align with Tiger's Fury / Berserk / Avenging Wrath / etc on engagement
  heroic_delta: |
    - Faster plume cadence (~8-10s) = more frequent re-engage windows
    - Cinderbreath hits harder if cone-positioning sloppy
  bot_anti_patterns:
    - DO NOT chase tank through plume (you are NOT the tank)
    - DO NOT stand in front of boss (Cinderbreath cone)
    - DO NOT pull boss off tank (Death Grip-style abilities should be held)
```

---

## Boss 4 (optional) — Beauty

```yaml
boss_melee:
  boss: beauty
  optional: true
  default_skip: true
  difficulty_grade: easy
  positioning:
    default: behind boss; spread from other players (charge multi-hit)
  interrupt_role:
    terrifying_roar: ATTEMPT — UNVERIFIED interruptibility
    pup_magma_spit: PROBABLY interruptible; try
  rotation_modifications:
    fear_landed: |
      Fear breaks attempts to cast. Use racial / trinket / fear-counter:
      - Combat Rogue: PvP trinket break
      - Fury Warrior: Berserker Rage (immune to fear)
      - Ret Pally: Hand of Freedom + PvP trinket
      - Frost DK: Lichborne (10s; talent — usually not in default Frost spec)
      - Enh Shaman: Tremor Totem pre-cast by self/Shaman teammate
      - Feral Cat: Berserk talent doesn't break fear; PvP trinket only
  bot_anti_patterns:
    - DO NOT stack with other players (charge multi-hit)
```

---

## Boss 5 — Ascendant Lord Obsidius

```yaml
boss_melee:
  boss: ascendant_lord_obsidius
  difficulty_grade: medium (target-swap)
  positioning:
    default: behind ACTIVE boss/clone unit
    facing: behind active unit
    movement: |
      Target-swap awareness — when clone-rotation event fires, the boss
      becomes a clone and a different Shadow of Obsidius becomes the active
      boss. Melee bot must:
      1. Detect target swap (active-unit change)
      2. Disengage from old unit (which is now a passive clone)
      3. Re-engage on new active unit at proper position (behind)
  interrupt_role:
    none: no confirmed kicks
  rotation_modifications:
    crepuscular_veil_on_self: |
      If you become Veil target (75476), MOVE OUT of group to avoid cleave.
      Continue DPS from outside-group position; healer will reactive-heal.
    target_swap: |
      On clone-rotation event, ST rotation continues on new target. Buffs
      that are target-bound transfer:
      - Combat Rogue: CP attached to target — RESET on target swap (lose
        all CP). Build new CP on new target. Apply Slice and Dice via 1 CP
        from new target.
      - Frost DK: diseases on target — re-Outbreak on new target; old
        target still has FF/BP but doesn't matter (it's a clone now)
      - Feral Cat: CP attached to target — same as Rogue, full CP reset.
        Re-apply Mangle + Rake + Rip on new target.
      - Fury Warrior / Ret Pally / Enh Shaman: most buffs are self-buffs
        (Inquisition, Savage Roar, Lightning Shield) — unaffected by target
        swap.
  cd_use:
    standard_offensive: rotational; align with Tiger's Fury / Berserk / Avenging Wrath / Pillar of Frost / etc
  heroic_delta: |
    - Faster clone-pull cycle = more frequent CP/disease resets
    - More damage from clones if target-swap is sloppy
  bot_anti_patterns:
    - DO NOT continue DPS on previous active unit after clone-rotation
    - DO NOT spend Combo Points on the wrong target
    - DO NOT stand in shadow puddles (Disagreement #8 unresolved; default avoid)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  rom_ogg:
    - encounter_detect_quake_cast → kick_rotation_participate
    - encounter_state_skullcracker_channel → maintain_stack_position
    - encounter_detect_add_spawn → swap_to_aoe_rotation

  corla:
    - encounter_state_drakonids_alive AND assigned_beam(self) → position_in_assigned_beam
    - encounter_state_self_evolution_stacks(target=self) → check_swap_threshold
    - encounter_state_drakonids_dead → return_to_corla_melee

  karsh:
    - encounter_state_tank_in_plume → wait_outside_plume
    - encounter_state_boss_exited_plume → re_engage_behind

  obsidius:
    - encounter_detect_clone_rotation_event → swap_target_to_new_active_unit
    - encounter_detect_self_crepuscular_veil → move_out_of_group
    - encounter_state_self_veil_active AND positioned_clear → continue_dps

dispel_blacklist:
  - { spell_id: 75476, name: Crepuscular Veil, reason: Obsidius swap mechanic }
  # melee bots typically don't dispel; included for completeness on hybrid melee (Ret Pally Cleanse, Enh Shaman Cleanse Spirit)
```
