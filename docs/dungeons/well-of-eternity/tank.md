# Well of Eternity — Tank

**Tank-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the tank reacts. Class-agnostic where possible.

**Layering**:
- Cross-content tank patterns (threat priority, defensive CD pacing, taunt rules) live in `docs/roles/tank.md` (deferred to tank phase).
- Per-spec rotation / CD / threat model lives in the corresponding `docs/specs/` tank doc (all tank specs deferred to tank phase).
- This doc layers WoE-specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_tank_overview:
  total_bosses: 3 (heroic-only)
  estimated_full_clear_minutes: 25-40
  threat_profile:
    perotharn: moderate                       # Corrupting Touch tank ramp
    queen_azshara: moderate                   # magi-rotation pickup
    mannoroth: high                           # NPC-tank-Mannoroth + add-tank phase 2
  defensive_cd_profile:
    perotharn: medium                         # Corrupting Touch stack ramp
    queen_azshara: light                      # Magi melee is soft on tank
    mannoroth: heavy                          # demon waves + Magistrike Arcs proc damage on tank
  key_responsibilities:
    - threat_priority_per_fight
    - phase_2_hide_state_perotharn            # tank also hides; no boss to tank
    - mannoroth_npc_tank_awareness            # bot tank does NOT taunt Mannoroth (Illidan's job)
    - magi_rotation_pickup_azshara            # acquire newly-active magus on each kill
    - demon_wave_pickup_mannoroth             # phase 2 add tank
    - dreadlord_debilitator_help_kill         # rescue Tyrande priority
    - npc_ally_blacklist                      # do NOT taunt allied NPCs
    - face_away_from_group                    # standard tank facing on cleave-capable bosses
```

---

## Trash — tank notes

```yaml
trash_tank:
  illidan_stealth_section:
    pull_pattern: |
      DO NOT ENGAGE during Shadowcloak escort. Tank holds threat tools;
      follows Illidan in stealth. Resume tanking on first mob engagement
      (typically the intro Legion demon fight).
  intro_legion_demon:
    notes: |
      Mandatory pull at start of dungeon BEFORE Illidan applies Shadowcloak.
      Standard tank-and-spank. Threat-on-pull required.
  crystal_packs:
    pull_pattern: |
      3 packs, each guarding a crystal. Standard pull with skull (kill priority)
      + cross/X (CC). Tank picks up melee mobs; CC-team handles casters.
    facing: away from group (frontal cleaves on Felguards / Satyrs)
  highborne_caster_packs:
    notes: |
      Astromancer Frostbolts; Apprentice melee/ranged hybrid. Tank pulls;
      DPS interrupts and kills.
  shores_demon_trash:
    notes: |
      Felguard / Felhound / Infernal preview. Standard tank-pull-and-cleave.
      Pre-defensive on Infernal damage aura if tank in range.
  ally_npcs:
    BLACKLIST: |
      Bot tank logic must NEVER taunt Illidan / Tyrande / Malfurion. Default
      taunt-to-acquire-threat should filter friendly NPCs.
  fear_packs:
    notes: |
      Fear-cast Highborne / satyr — tank stays in center while DPS interrupts.
      If fear lands, healer can't respond — tank does NOT chase strays.
```

---

## Boss 1 — Peroth'arn

```yaml
boss_tank:
  boss: perotharn
  difficulty_grade: medium (Corrupting Touch ramp + phase 2 hide)
  threat_intensity: low (single boss, no adds in phase 1)

  positioning:
    pull: |
      Tank pulls Peroth'arn to one side of the Courtyard chamber. Position
      so DPS / healer can spread on the open side of the room.
    facing: away from group (standard); irrelevant in phase 2
    movement: minimal in phase 1 / 3
    phase_2_hide: |
      Tank ALSO HIDES BEHIND PILLAR. There is no boss to tank during the
      stealth phase. Pathfind to pillar OUT of Eye line-of-sight; stay
      still 40s.

  threat_management:
    standard: |
      Phase 1: standard threat rotation; maintain threat lead.
      Phase 2: NO TARGET; threat irrelevant.
      Phase 3: re-acquire boss on phase transition; threat rotation.
    no_adds: phase 1 and 3 have NO adds; phase 2 Eyes are NOT killable

  defensive_cd_use:
    corrupting_touch_stack_ramp: |
      Pop short defensive (Shield Block / Barkskin / Bone Shield etc.) at
      ~3-4 stacks. Pop major defensive (Shield Wall / Survival Instincts /
      Icebound Fortitude / Vampiric Blood) at ~6+ stacks if heal pressure
      compounds.
    fel_decay_on_self: |
      Tank can be Fel Decay target — defensive CD if HP drops below 50%.
      Healer will likely NOT direct-heal (heal-decision); tank's HoT auto-heal
      is the recovery.
    fel_flames_on_self: reactive defensive if tank takes Fel Flames hit
    drain_essence: NO defensive needed (Illidan interrupts; brief stun only)
    phase_3_endless_frenzy: pop major defensive on Endless Frenzy (failure path)
    phase_3_enfeebled: standard tanking; boss is weakened (success path)

  interrupt_role:
    drain_essence: NO — Illidan handles. Do NOT waste kick CD.
    fel_flames: instant cast; no kick
    fel_decay: instant cast; no kick

  heroic_delta: |
    Heroic-only encounter; numbers above ARE heroic.

  bot_anti_patterns:
    - DO NOT continue tanking during phase 2 (boss is vanished)
    - DO NOT attempt to taunt Eyes of Peroth'arn (scripted; despawn on phase end)
    - DO NOT use kick on Drain Essence (Illidan's job)
    - DO NOT stand in Fel Flames patches
    - DO NOT skip the pillar-hide (tank is bot in phase 2 just like DPS)
```

---

## Boss 2 — Queen Azshara

```yaml
boss_tank:
  boss: queen_azshara
  difficulty_grade: medium (target rotation pickup)
  threat_intensity: low_to_moderate (single magus at a time + tank-pulled-pair)

  positioning:
    pull: |
      Pull active magi to ONE SIDE of palace, away from group. Newly-activated
      magus acquired on each kill — taunt or run-pickup as the previous
      target dies.
    facing: away from group (standard)
    movement: minimal — hold magi in one corner

  threat_management:
    magi_rotation_pickup: |
      CRITICAL behavior. As each active magus dies, a NEW magus activates
      from the pre-spawned 6. Tank must:
        1. Detect "magus active" event (HP threshold 100% on previously-inactive)
        2. Acquire threat: taunt OR move-to-engage and AoE-threat tool
           (Thunder Clap / Swipe / Death and Decay / Consecration / Avenger's
           Shield)
        3. Position new magus near previous-magus location to consolidate
           DPS area
    azshara_targeting: |
      DO NOT taunt or attempt threat on Azshara herself (Shroud of Luminosity
      invuln). She does not threat-cycle.
    puppet_strings: |
      Tank does NOT pick up puppet strings (DPS handles). Tank holds active
      magus.

  defensive_cd_use:
    standard: rotational defensives on magi melee swings
    arcane_bomb_proximity: pre-defensive if tank standing in Arcane Bomb path
    long_fight: |
      Long fight (~3 min). Rotate major CDs across the fight rather than
      saving for execute (no execute phase in this fight).

  interrupt_role:
    total_obedience: |
      HIGHEST PRIORITY in dungeon. Tank PARTICIPATES in interrupt rotation:
      - Prot Warrior: Pummel (10s CD) — usable in defensive stance via Glyph
      - Prot Paladin: Rebuke (15s CD; off-GCD)
      - Blood DK: Mind Freeze (10s CD; off-GCD)
      - Feral Tank Druid: Skull Bash (60s CD; gap-close + interrupt)
      Wipe if missed.
    fireball_fire_magus: secondary interrupt; rotate kicks if available
    other_magus_casts: opportunistic interrupts

  heroic_delta: |
    Heroic-only encounter; numbers above ARE heroic.

  bot_anti_patterns:
    - DO NOT taunt Azshara (Shroud of Luminosity = invuln)
    - DO NOT skip Total Obedience interrupt (wipe if missed)
    - DO NOT pick up puppet strings (DPS-killable; tank holds magus)
    - DO NOT chase strays (magi rotate; pre-positioning is enough)
    - DO NOT stand in magus AoEs (Arcane Bomb / Coldflame / Firebomb / Blast Wave)
```

---

## Boss 3 — Mannoroth and Varo'then

```yaml
boss_tank:
  boss: mannoroth_and_varothen
  difficulty_grade: hard (NPC-tank awareness + multi-phase + add-tank)
  threat_intensity: moderate (Varo'then phase 1 + demon waves phase 2)

  positioning:
    phase_1: |
      Pull Varo'then; tank to ONE SIDE, away from Mannoroth (Illidan's tanking
      Mannoroth — do NOT pull them together). DO NOT TAUNT MANNOROTH.
    phase_2: |
      Tank picks up demon waves as they spawn. Position waves between Illidan
      and party; cleave-tank if multiple demons up. Stay near Illidan for
      Hand of Flame mitigation aura.
    phase_3: |
      Stack tight near party for Gift of Sargeras AoE pulse + heal aura.
      Mannoroth still has aggro on Illidan (or transferring); player tank
      auto-attacks for cleanup.
    facing: away from group (standard); rotate per phase
    movement: |
      HIGH — Fel Firestorm patches require continuous relocation entire fight.
      Tank kites Varo'then around the arena to keep group on safe ground.

  threat_management:
    phase_1_varothen_only: |
      Bot tank's phase-1 target is Varo'then ONLY. Mannoroth is on the
      encounter list but TANKED BY ILLIDAN (NPC ally). Bot tank logic must
      distinguish "boss tanked by ally NPC" from "boss tanked by player."
      Bot does NOT taunt Mannoroth. Bot does NOT engage Mannoroth.
    phase_2_demon_wave_pickup: |
      Each wave triggers at Mannoroth HP threshold:
        75%: Felhounds (melee) — taunt + AoE threat
        60%: Felguards (melee + cleave) — taunt + AoE threat; defensive on cleave
        45%: Doomguards (caster + ranged) — gap-close + interrupt-coordination
              with DPS
        30%: Infernals (high AoE) — taunt + major defensive CD
      AoE threat tools (Thunder Clap / Swipe / Death and Decay / Consecration /
      Avenger's Shield) on each wave.
    phase_2_dreadlord_help: |
      Help kill Dreadlord Debilitators during rescue event. Pick up if they
      attack tank; otherwise, DPS-burn alongside.
    phase_3_mannoroth_minor: |
      Phase 3 — Illidan still tanking-equivalent (or aggro transferring).
      Bot tank attacks Mannoroth for cleanup damage; threat is not primary
      concern (boss dies fast).

  defensive_cd_use:
    phase_1: rotational on Varo'then melee swings + Magistrike arc on tank
    phase_2: |
      Demon wave defensives:
        Felhounds (75%): light defensive
        Felguards (60%): medium defensive on cleave
        Doomguards (45%): light defensive (caster damage)
        Infernals (30%): MAJOR defensive (Shield Wall / Survival Instincts /
                          Icebound Fortitude / Vampiric Blood) — heaviest AoE
                          pulse
      Magistrike Arcs proc damage on tank from sword: rotational defensives
      throughout phase 2.
    phase_3: |
      Gift of Sargeras heals — defensive less critical. Standard tank-flow.

  interrupt_role:
    doomguard_shadow_bolt_phase_2: PARTICIPATE; rotate kicks
    varothen_magistrike_phase_1: ATTEMPT — UNVERIFIED interruptibility
    other: opportunistic only

  npc_ally_handling:
    illidan_stormrage: DO NOT TAUNT (he's tanking Mannoroth phase 1; ally damage blacklist)
    tyrande_whisperwind: DO NOT TAUNT; track HP for Dreadlord Debilitator rescue
    malfurion_stormrage: DO NOT TAUNT; phase 3 trigger detection
    mannoroth_phase_1: DO NOT TAUNT (Illidan's job)

  moonbeam_handling:
    on_moonbeam_appear: |
      Stack on Tyrande's Moonbeam ground area. Provides Blessing of Elune
      (damage-vs-demons buff + light HP regen). Override default
      "anchor demons in one spot" positioning briefly to claim the buff.

  sword_pickup_handler:
    primary_designation: ranged DPS (travel speed)
    backup: tank if no melee/ranged available — but rare; tank's engagement
            with Varo'then makes the swap awkward
    notes: |
      Tank is LAST-RESORT sword handler. If forced (4-bot DPS-light group),
      tank uses gap-close (Charge / Heroic Leap / Death's Advance) AFTER
      transitioning to demon-wave duty.

  heroic_delta: |
    Heroic-only encounter; numbers above ARE heroic.

  bot_anti_patterns:
    - DO NOT taunt Mannoroth phase 1 (Illidan's job)
    - DO NOT pull Mannoroth and Varo'then to same spot (split tank coverage; Illidan handles Mannoroth elsewhere)
    - DO NOT skip demon wave pickup (Tyrande overwhelmed if waves leak to ranged)
    - DO NOT damage Illidan / Tyrande / Malfurion
    - DO NOT stand in Fel Firestorm patches (continuous movement)
    - DO NOT skip Moonbeam stack (free buff + heal)
    - DO NOT skip defensive CD on Infernal wave (heaviest AoE pulse)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  perotharn:
    - encounter_state_phase_1 → standard_threat_rotation_on_perotharn
    - encounter_detect_corrupting_touch_stacks(self, stacks>=4) → pop_short_defensive_cd
    - encounter_detect_corrupting_touch_stacks(self, stacks>=6) → pop_major_defensive_cd
    - encounter_detect_drain_essence_cast → suspend_tanking_pre_phase_2
    - encounter_state_phase_2_active → ENTER_HIDE_STATE + suspend_threat + pillar_pathfind
    - encounter_state_phase_3_endless_frenzy → pop_major_defensive_cd
    - encounter_state_phase_3_enfeebled → standard_tank_burn

  queen_azshara:
    - encounter_state_active → target_resolution: active_magus (closest if multiple)
    - encounter_detect_magus_activation_event → acquire_threat_on_new_magus (taunt OR run-pickup + AoE_threat)
    - encounter_detect_total_obedience_cast → kick_rotation_participate (PRIORITY: HIGHEST)
    - encounter_detect_fireball_fire_magus → kick_rotation_participate (PRIORITY: high)
    - encounter_state_self_targeting_azshara → BLOCK + force_retarget_active_magus
    - encounter_detect_arcane_bomb_landing → relocate

  mannoroth:
    - encounter_state_phase_1 → target=varothen + tank_position_away_from_illidan_mannoroth
    - encounter_state_self_target_mannoroth_phase_1 → BLOCK + retarget_varothen
    - encounter_detect_fel_firestorm_patch_under_self → relocate
    - encounter_detect_demon_wave_spawn(wave) → acquire_threat + AoE_threat_tools + defensive_cd_per_wave
    - encounter_detect_dreadlord_debilitator_spawn → help_kill
    - encounter_detect_tyrande_moonbeam_appear → stack_on_moonbeam
    - encounter_state_phase_3 → target=mannoroth + stack_party

dispel_blacklist:
  - { spell_id: 105544, name: Fel Decay (Peroth'arn), reason: heal-vs-decay decision }
  # tank bots typically don't dispel; included for completeness

threat_target_blacklist:
  - { npc_id: 54853, name: Queen Azshara, reason: Shroud of Luminosity invuln }
  - { npc_id: UNVERIFIED, name: Eye of Peroth'arn, reason: scripted; despawn on phase end }
  - { npc_id: UNVERIFIED, name: Illidan Stormrage, reason: ally NPC }
  - { npc_id: UNVERIFIED, name: Tyrande Whisperwind, reason: ally NPC }
  - { npc_id: UNVERIFIED, name: Malfurion Stormrage, reason: ally NPC }
  - { npc_id: 54969, name: Mannoroth, scope: phase_1, reason: Illidan tanks Mannoroth phase 1; do NOT taunt }
  # NOTE: Mannoroth blacklist is PHASE-SPECIFIC — bot CAN engage Mannoroth in phase 2-3 (sword embedded)

interrupt_priority_per_fight:
  perotharn:        []
  queen_azshara:    [Total Obedience (HIGHEST), Fireball (high), other magus casts (opportunistic)]
  mannoroth:        [Doomguard shadow_bolt, Varo'then Magistrike (if interruptible)]

defensive_cd_priority_per_fight:
  perotharn:           [reactive on Corrupting Touch stacks 4+/6+; major on Endless Frenzy phase 3]
  queen_azshara:       [rotational across magi melee; no execute phase]
  mannoroth:           [phase 2 demon waves staggered (Infernal MAJOR); Magistrike Arcs rotational]

phase_specific_states:
  perotharn_phase_2_hide:
    threat_inhibit: true
    movement: pillar_pathfind_los_clear
    duration: 40s OR phase_3_signal
    do_not_attack: [Eye of Peroth'arn]
  azshara_magi_rotation:
    threat_acquisition: on_each_magus_activation_event
    target_lock: active_magus
    target_blacklist: queen_azshara
  mannoroth_phase_1_target_lock:
    locked_target: varothen
    blacklist_target: mannoroth                # Illidan tanking; bot does NOT taunt
  mannoroth_phase_2_add_tank:
    target_priority:
      - dreadlord_debilitator
      - infernal
      - doomguard
      - felguard
      - felhound
    defensive_cd_per_wave: true
```
