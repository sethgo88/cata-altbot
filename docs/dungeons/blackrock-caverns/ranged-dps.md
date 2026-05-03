# Blackrock Caverns — Ranged DPS

**Ranged-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how ranged DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content ranged DPS patterns (positioning, spread/stack rules, threat ceiling) live in `docs/roles/ranged-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (frost-mage, affliction-warlock, mm-hunter, shadow-priest, balance-druid, elemental-shaman)
- This doc layers BRC-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_ranged_dps_overview:
  total_bosses: 4 (+ 1 optional)
  estimated_full_clear_minutes: 25-45
  positioning_profile:
    rom_ogg: STACKED_AT_MELEE             # OVERRIDE — Skullcracker damage-share requires range to stack with melee
    corla: beam_intercept_assigned        # stand in assigned beam (intercept role)
    karsh: max_range_outside_cone         # standard ranged behavior
    obsidius: max_range_spread             # standard
  interrupt_load:
    rom_ogg: high                         # Quake every 10-15s; ranged participates if class has kick
    corla: zero                           # no kicks
    karsh: zero
    obsidius: zero
  key_responsibilities:
    - rom_ogg_stack_override               # ranged stacks with melee — exception to default
    - beam_intercept_assigned (corla)
    - cone_avoidance (karsh Cinderbreath)
    - target_swap_on_clone_rotation (obsidius)
```

---

## Trash — ranged DPS notes

```yaml
trash_ranged:
  twilight_humanoids:
    cc_priorities: |
      Ranged contributes opener-CC:
      - Frost Mage: Polymorph (Sheep) — primary multi-CC
      - Affliction Warlock: Fear (PvE-disabled in 5-man trash usually; UNVERIFIED) / Banish for Twilight Drakonids
      - MM Hunter: Freezing Trap (icetrap) on caster
      - Shadow Priest: Mind Control (rare 5-man use); Shackle Undead
      - Balance Druid: Cyclone (8s)
      - Elemental Shaman: Hex (frog 60s, breaks on damage)
  caster_packs:
    interrupt_role: |
      - Frost Mage: Counterspell (24s) / Frost Nova root for kiting
      - Affliction Warlock: Spell Lock via Felhunter pet
      - MM Hunter: Silencing Shot (24s)
      - Shadow Priest: Silence (45s)
      - Balance Druid: Solar Beam (60s; 8s AoE silence)
      - Elemental Shaman: Wind Shear (6s; off-GCD; 25y range)

  conflagrations:
    notes: environmental fire; ranged easily avoids — no movement penalty to staying clear
```

---

## Boss 1 — Rom'ogg Bonecrusher

```yaml
boss_ranged:
  boss: rom_ogg_bonecrusher
  difficulty_grade: easy (positional override)
  positioning:
    OVERRIDE: |
      STACK WITH MELEE under boss for Skullcracker damage-share. THIS IS AN
      EXPLICIT OVERRIDE of default ranged "stay at max range" behavior.
      Default rule does not apply on this fight.
    facing: any
    movement: |
      Chains of Woe (75539) pulls all players to fixed point regardless;
      no need to pre-position before. Stay stacked after.
  interrupt_role:
    quake: PARTICIPATE if class has kick (Counterspell / Wind Shear / Silencing Shot etc.)
  rotation_modifications:
    skullcracker_channel: continue DPS during channel — full-group stacked DPS is the strat
    add_wave_at_50: |
      AoE rotation on add cluster:
      - Frost Mage: Blizzard / Frostfire Bolt cleave; Frost Nova for root
      - Affliction Warlock: Seed of Corruption + Hellfire if no DoT spread tools
      - MM Hunter: Multi-Shot / Trap Launcher Explosive Trap
      - Shadow Priest: Mind Sear if 4+ targets (poor); Vampiric Touch + Shadow Word: Pain spread
      - Balance Druid: Hurricane (channel) / Wild Mushroom detonation
      - Elemental Shaman: Thunderstorm (knockback + dmg) — POSITIONAL HAZARD; do NOT cast Thunderstorm during stack
  cd_use:
    save_for_real_bosses: minimal CD use; rotational only
  heroic_delta: |
    - Skullcracker damage higher; stack-share more critical
    - Add wave possibly larger
  bot_anti_patterns:
    - DO NOT stay at max range (Skullcracker damage-share fails)
    - DO NOT use AoE-knockback during stack (Thunderstorm Shaman, Typhoon Druid)
    - DO NOT interrupt Chains of Woe
```

---

## Boss 2 — Corla, Herald of Twilight

```yaml
boss_ranged:
  boss: corla_herald_of_twilight
  difficulty_grade: medium (positional)
  positioning:
    default: |
      Take ASSIGNED beam (DPS bots split 3 platforms among themselves;
      ranged usually takes the back-most platform for easiest line-of-sight
      to drakonid + Corla).
    facing: drakonid
    movement: |
      Step out of beam at stack threshold:
      - Normal: stack 4
      - Heroic: stack 3 (mandatory two-DPS rotation per beam)
  interrupt_role:
    none: no kick-required casts
  rotation_modifications:
    in_beam: |
      Continue DPS on assigned drakonid. Cast-while-moving classes preferred:
      - Frost Mage: Frostbolt is hardcast; Ice Lance instant on Fingers proc
        — bot stops casting on beam-step transitions; resumes when stable
      - Affliction Warlock: DoTs are instant; Drain Soul channel breaks on
        movement — refresh DoTs and channel during stable beam-stand
      - MM Hunter: Steady Shot is hardcast; Aimed Shot on proc
        — Hunter is BEST class for this fight (cast-on-move via Wyvern Sting talent? UNVERIFIED Cata)
      - Shadow Priest: Mind Flay channels are mobility-friendly; DoTs instant
      - Balance Druid: Wrath/Starfire hardcast; Moonfire/Sunfire instant; Eclipse is
        mostly maintained via instant DoT applications
      - Elemental Shaman: Lightning Bolt hardcast; Lava Burst hardcast; Flame Shock
        instant — same step-out cast-pause pattern as Frost Mage
    out_of_beam: |
      Continue DPS on drakonid from ranged. Stacks decay ~1/s.
    after_drakonids: |
      Standard ST rotation on Corla from max range.
  cd_use:
    save_major_for_intercept: |
      Pop major defensive when in beam if HP drops below 60%:
      - Frost Mage: Iceblock (5min CD) — CRITICAL save
      - Aff Warlock: Soulstone self pre-fight; Healthstone in combat
      - MM Hunter: Disengage (no CD-defensive but mobility); Master's Call (talent — UNVERIFIED)
      - Shadow Priest: Dispersion (-90% damage 6s)
      - Balance Druid: Barkskin (-20% damage 12s)
      - Ele Shaman: Stoneclaw Totem (talented — shield self)
  heroic_delta: |
    - Two-DPS-per-beam rotation; ranged bot must coordinate swap-in
  bot_anti_patterns:
    - DO NOT stand in TWO beams (overlap = 2x stack)
    - DO NOT stop casting while in stable beam (out-of-beam transitions only)
    - DO NOT kill Corla before drakonids
```

---

## Boss 3 — Karsh Steelbender

```yaml
boss_ranged:
  boss: karsh_steelbender
  difficulty_grade: easy (positional)
  positioning:
    default: max range from boss; OUT of Cinderbreath cone arc; OFF central plume
    facing: boss (cast direction)
    movement: minimal — boss kited around plume, but ranged stays at perimeter
  interrupt_role:
    cinderbreath: GENERALLY NON-INTERRUPTIBLE; try if your kick is off CD
  rotation_modifications:
    standard: |
      Standard ST rotation. Cinderbreath arc determines safe-cast positioning:
      - Frost Mage: position at max range; Frostbolt / Frostfire Bolt rotation
      - Affliction Warlock: max range; DoT ramp + Drain Soul if execute
      - MM Hunter: 35y+ for full damage; Aimed Shot / Steady Shot rotation
      - Shadow Priest: max range; Vampiric Touch + Shadow Word: Pain + Mind Blast + Mind Flay
      - Balance Druid: Eclipse-driven rotation; max range
      - Ele Shaman: max range; LB/LvB rotation; Searing Totem at boss feet (drop NEAR plume but NOT IN — totem doesn't take fire damage but range)
  cd_use:
    standard_offensive: rotational
  heroic_delta: |
    - Cinderbreath splash damage higher if cone clipping occurs
  bot_anti_patterns:
    - DO NOT stand in plume (you are NOT the tank)
    - DO NOT cast IN front of boss arc (Cinderbreath cone)
    - DO NOT cast Knockback abilities (none of standard ranged DPS specs have these in BRC; Thunderstorm Shaman is Ele-CD that knocks back — DO NOT use here)
```

---

## Boss 4 (optional) — Beauty

```yaml
boss_ranged:
  boss: beauty
  optional: true
  default_skip: true
  difficulty_grade: easy
  positioning:
    default: max range; SPREAD from other ranged (charge multi-hit)
  interrupt_role:
    terrifying_roar: ATTEMPT — UNVERIFIED interruptibility
  rotation_modifications:
    fear_landed: |
      Fear breaks casting. Counter:
      - Frost Mage: Iceblock (immune to fear)
      - Aff Warlock: Demonic Circle (talent) for re-position
      - MM Hunter: PvP trinket; Disengage post-fear
      - Shadow Priest: Fear Ward (self-cast at L70+; UNVERIFIED Cata)
      - Balance Druid: PvP trinket; Travel Form for movement
      - Ele Shaman: Tremor Totem (pre-cast); racial Will of Forsaken (Undead)
    standard_dps: rotational
  bot_anti_patterns:
    - DO NOT cluster ranged together (charge stuns multi-hit possible)
    - DO NOT cast hardcast spells while feared (waste of GCD)
```

---

## Boss 5 — Ascendant Lord Obsidius

```yaml
boss_ranged:
  boss: ascendant_lord_obsidius
  difficulty_grade: medium (target-swap + positioning)
  positioning:
    default: max range; SPREAD to avoid clone cleave
    facing: active boss/clone unit
    movement: |
      Target-swap awareness — clone-rotation event changes active unit.
      Movement is minimal; just re-target. Stay clear of corners (shadow
      puddles concentrate there per Disagreement #8 default = avoid).
  interrupt_role:
    none
  rotation_modifications:
    crepuscular_veil_on_self: |
      If you become Veil target (75476), MOVE OUT of group to avoid cleave.
      Continue ranged DPS from outside-group position.
    target_swap: |
      On clone-rotation event, ST rotation continues on new target. Most
      ranged buffs are self-buffs unaffected by target swap:
      - Frost Mage: Frostbolt / Frostfire continue; no target-bound buffs
      - Aff Warlock: DOTS RESET on target swap — must re-apply Corruption,
        Bane of Agony, Unstable Affliction, Haunt, Curse of the Elements/
        Curse of Tongues. Significant DPS loss; bot continues DoT-spam
        rotation on new target.
      - MM Hunter: Hunter's Mark resets; re-apply on new target. Steady Shot
        / Aimed Shot rotation continues unchanged.
      - Shadow Priest: DOTS RESET on target swap (same as Warlock) — Vampiric
        Touch + Shadow Word: Pain re-application. Mind Blast / Mind Flay
        continue.
      - Balance Druid: Moonfire / Sunfire RESET on target swap. Eclipse bar
        is self; preserved.
      - Ele Shaman: Flame Shock RESET on target swap. Lightning Shield self;
        preserved.
  cd_use:
    standard_offensive: rotational; bloodlust pre-pull or saved per group call
  heroic_delta: |
    - Faster clone-pull cycle = more frequent DoT resets (DPS loss for DoT classes)
    - More cleave damage if Veil target stays in group
  bot_anti_patterns:
    - DO NOT continue cast on previous active unit after clone-rotation event
    - DO NOT cluster ranged (cleave hazard)
    - DO NOT stand in shadow puddles (default avoid)
    - DO NOT auto-dispel Crepuscular Veil (DISPEL_BLACKLIST entry)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  rom_ogg:
    - encounter_state_active → STACK_WITH_MELEE (override default ranged spread)
    - encounter_detect_quake_cast → kick_rotation_participate (if class has kick)
    - encounter_detect_add_spawn → swap_to_aoe_rotation
    - encounter_detect_skullcracker_channel → continue_dps_in_stack

  corla:
    - encounter_state_drakonids_alive AND assigned_beam(self) → position_in_assigned_beam
    - encounter_state_self_evolution_stacks(target=self) → check_swap_threshold
    - encounter_state_drakonids_dead → max_range_corla_dps

  karsh:
    - encounter_state_active → max_range_outside_cone
    - encounter_detect_cinderbreath_cast → verify_self_outside_arc

  obsidius:
    - encounter_detect_clone_rotation_event → swap_target_to_new_active_unit
    - encounter_detect_self_crepuscular_veil → move_out_of_group
    - encounter_state_active → maintain_spread

dispel_blacklist:
  - { spell_id: 75476, name: Crepuscular Veil, reason: Obsidius swap mechanic }
  # ranged bots typically only dispel via Mage Remove Curse / Druid Remove Corruption / etc.;
  # included for completeness because some ranged hybrids can dispel Magic.
```
