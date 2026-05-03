# Well of Eternity — Ranged DPS

**Ranged-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how ranged DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content ranged DPS patterns (positioning, spread/stack rules, threat ceiling) live in `docs/roles/ranged-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (frost-mage, affliction-warlock, mm-hunter, shadow-priest, balance-druid, elemental-shaman)
- This doc layers WoE-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_ranged_dps_overview:
  total_bosses: 3 (heroic-only)
  estimated_full_clear_minutes: 25-40
  positioning_profile:
    perotharn:
      phase_1: spread_max_range
      phase_2: HIDE_BEHIND_PILLAR             # OVERRIDE — no DPS during stealth
      phase_3: spread_max_range
    queen_azshara: max_range_spread            # standard
    mannoroth_phase_1: spread_8_to_10y         # Magistrike arc mitigation
    mannoroth_phase_2: stack_near_illidan      # Hand of Flame mitigation
    mannoroth_phase_3: stack_tight_for_gift_of_sargeras
  interrupt_load:
    perotharn: zero                            # Drain Essence is Illidan's job
    queen_azshara: HIGH                        # Total Obedience MUST INTERRUPT + Fire Magus Fireball
    mannoroth: medium                          # Doomguard shadow_bolt + Varo'then Magistrike
  key_responsibilities:
    - phase_2_hide_state_perotharn             # do NOT cast during stealth
    - dps_blacklist_azshara                    # do NOT target Azshara herself
    - magi_kill_order_arcane_frost_fire        # encoded DPS priority
    - puppet_strings_dps_switch_on_mc          # MC event interrupt
    - total_obedience_interrupt_priority       # WIPE if missed
    - sword_pickup_handler                     # ranged is typical sword handler (travel speed)
    - npc_ally_blacklist                       # do NOT damage Illidan/Tyrande/Malfurion
    - dreadlord_debilitator_kill_priority      # rescue Tyrande
    - moonbeam_stack_on_area                   # damage-vs-demons buff
```

---

## Trash — ranged DPS notes

```yaml
trash_ranged:
  illidan_stealth_section:
    notes: |
      Bot follows Illidan in stealth. DO NOT CAST during Shadowcloak escort
      (breaks party stealth and pulls trash). Hold rotation; resume on
      first mob engagement.
  crystal_packs:
    cc_priorities: |
      Bot ranged contributes opener-CC on Highborne / satyr humanoids:
      - Frost Mage: Polymorph (Sheep) — primary multi-CC
      - Affliction Warlock: Banish (demon types — Felguard etc.) / Fear (humanoids)
      - MM Hunter: Freezing Trap (icetrap) on caster
      - Shadow Priest: Shackle Undead (rare here); Mind Control (rare)
      - Balance Druid: Hibernate (Beast/Dragonkin only — UNVERIFIED applicability); Cyclone (8s)
      - Elemental Shaman: Hex (frog 60s; breaks on damage)
  highborne_caster_packs:
    interrupt_role: |
      Highborne Astromancer Frostbolt hardcasts:
      - Frost Mage: Counterspell (24s)
      - Affliction Warlock: Spell Lock via Felhunter pet
      - MM Hunter: Silencing Shot (24s)
      - Shadow Priest: Silence (45s)
      - Balance Druid: Solar Beam (60s; 8s AoE silence)
      - Elemental Shaman: Wind Shear (6s; off-GCD; 25y)
  shores_demon_trash:
    notes: |
      Felhound / Felguard / Infernal preview of final-boss waves. AoE rotation
      on clusters; Banish for Warlock if pulling demon types.
  ally_npcs:
    BLACKLIST: |
      Bot DPS targeting must NEVER include Illidan / Tyrande / Malfurion.
      Default "tab-target enemy" should filter friendly NPCs, but explicit
      blacklist required if any auto-AoE catches them.
```

---

## Boss 1 — Peroth'arn

```yaml
boss_ranged:
  boss: perotharn
  difficulty_grade: medium (positional + phase-2 unique)

  positioning:
    phase_1: max_range; SPREAD from other ranged (Fel Flames cluster avoidance)
    phase_2: |
      HIDE BEHIND PILLAR. DO NOT CAST. Casting during stealth phase MAY
      break Shadowcloak (UNVERIFIED — conservative default = silent).
      Pathfind to pillar OUT of Eye line-of-sight; stay still 40s.
    phase_3: max_range; SPREAD; resume DPS rotation
    facing: boss (cast direction); irrelevant in phase 2

  interrupt_role:
    drain_essence: NO — Illidan handles. Do NOT waste kick CD.
    fel_flames: instant cast; no kick
    fel_decay: instant cast; no kick

  rotation_modifications:
    phase_1:
      standard: |
        Standard ST rotation on Peroth'arn:
        - Frost Mage: Frostbolt / Frostfire Bolt; Ice Lance on Fingers proc
        - Affliction Warlock: maintain Corruption + UA + Bane of Doom + Haunt
        - MM Hunter: Steady Shot / Aimed Shot; Chimera Shot
        - Shadow Priest: VT + SW:P + Mind Blast + Mind Flay; Shadow Orbs
        - Balance Druid: Wrath/Starfire (Eclipse-driven); Moonfire/Sunfire DoTs
        - Elemental Shaman: LB + LvB + ES Fulmination + FS DoT
      movement: |
        Fel Flames places ground patches under random players. Move out
        ASAP. Cast-while-moving classes have advantage (Lava Surge LvB,
        Steady Shot, instant DoTs).
    phase_2:
      cast_inhibit: true                        # do NOT cast
      movement: pathfind_to_pillar
      duration: 40s
    phase_3:
      enfeebled_path: |
        Boss takes +25% damage. Burn rotation; pop offensive CDs:
        - Frost Mage: Icy Veins + Mirror Image
        - Affliction Warlock: Soulburn Haunt + DoT-spread; Demon Soul
        - MM Hunter: Rapid Fire + Readiness
        - Shadow Priest: Shadowfiend + Mind Bender if available
        - Balance Druid: Starfall + Force of Nature
        - Elemental Shaman: Elemental Mastery
      endless_frenzy_path: |
        Boss does +25% damage. Burn-but-survive; defensive CDs as needed:
        - Frost Mage: Iceblock if HP critical
        - Aff Warlock: Healthstone + Drain Soul if execute
        - MM Hunter: Disengage + Master's Call
        - Shadow Priest: Dispersion (-90% damage 6s)
        - Balance Druid: Barkskin (-20% damage 12s)
        - Ele Shaman: Stoneclaw Totem (talented)

  cd_use:
    standard_offensive: rotational on engagement
    save_for_phase_3: hold major offensive CDs for Enfeebled phase 3

  heroic_delta: |
    Heroic-only encounter; numbers above ARE heroic.

  bot_anti_patterns:
    - DO NOT cast during phase 2 (breaks hide; risks Eye detection)
    - DO NOT target / DPS Eyes of Peroth'arn (scripted; despawn on phase end)
    - DO NOT stand in Fel Flames patches
    - DO NOT use kick on Drain Essence (Illidan's job)
    - DO NOT cluster with melee (Fel Flames hits multiple)
```

---

## Boss 2 — Queen Azshara

```yaml
boss_ranged:
  boss: queen_azshara
  difficulty_grade: hard (target priority + interrupt scheduling)

  positioning:
    default: max range from active magus; SPREAD ~8y from other ranged
    facing: active magus
    movement: |
      Move out of Coldflame line (Frost Magus); Firebomb / Blast Wave
      (Fire Magus); Arcane Bomb ground patch.

  interrupt_role:
    total_obedience: |
      HIGHEST PRIORITY in dungeon. Bot interrupt rotation MUST reserve a
      kick for it. 8s cast = multiple attempts available; missing all = wipe.
      - Frost Mage: Counterspell (24s; school-lockout 8s)
      - Affliction Warlock: Spell Lock via Felhunter pet
      - MM Hunter: Silencing Shot (24s)
      - Shadow Priest: Silence (45s)
      - Balance Druid: Solar Beam (60s; 8s AoE silence — covers everything)
      - Elemental Shaman: Wind Shear (6s; off-GCD; BEST in dungeon for guaranteed coverage)
    fireball_fire_magus: |
      Secondary interrupt for healer mana. Rotate kicks if available.

  rotation_modifications:
    target_priority: |
      Default DPS target on Azshara fight (highest first):
        1. Puppet strings (if MC event active)
        2. Active Arcane Magus (kill priority 1)
        3. Active Frost Magus (kill priority 2)
        4. Active Fire Magus (kill priority 3)
        4. (NEVER target Azshara — Shroud of Luminosity = invuln)
    dot_classes_target_swap: |
      Magi switch is target swap for DoT classes:
      - Affliction Warlock: DoTs RESET on swap. Re-apply Corruption + UA +
        BoD + Haunt on each new active magus. Significant DPS loss; bot
        continues DoT-spam rotation on new target.
      - Shadow Priest: VT + SW:P RESET on swap. Re-apply on new magus.
        Mind Blast / Mind Flay continue.
      - Balance Druid: Moonfire / Sunfire RESET on swap. Re-apply.
      - Elemental Shaman: Flame Shock RESETS on swap. Re-apply.
    mc_event: |
      Switch to puppet strings IMMEDIATELY on Hand of the Queen MC. Strings
      die fast; resume magus DPS after free.
    aoe_window: |
      Magi don't form AoE clusters (1 active at a time + tank-pulled magus).
      No major AoE rotation; pure ST priority.

  cd_use:
    save_for_arcane: |
      First magus is typically Arcane (kill priority 1). Pop major offensive
      CDs on first Arcane magus to maximize damage during high-priority kill.
    bloodlust_alignment: per group call; typically saved for execute or pre-pull

  heroic_delta: |
    Heroic-only encounter; numbers above ARE heroic.

  bot_anti_patterns:
    - DO NOT target Azshara (Shroud of Luminosity = invuln; wasted DPS)
    - DO NOT skip Total Obedience interrupt (wipe if missed)
    - DO NOT continue magus DPS during MC event (kill puppet strings first)
    - DO NOT stand in magus AoEs (Coldflame / Firebomb / Arcane Bomb)
    - DO NOT cluster ranged (Coldflame line / Charge multi-hit)
```

---

## Boss 3 — Mannoroth and Varo'then

```yaml
boss_ranged:
  boss: mannoroth_and_varothen
  difficulty_grade: hard (multi-phase + sword pickup + NPC tracking)

  positioning:
    phase_1: |
      Max range; SPREAD 8-10y for Magistrike arc mitigation. Constantly
      relocate out of Fel Firestorm patches.
    phase_2: |
      Stack near Illidan for Hand of Flame mitigation. Stack on Tyrande's
      Moonbeam when it appears (damage-vs-demons buff). Demon waves take
      AoE rotation slot.
    phase_3: |
      Stack tight near party for Gift of Sargeras AoE pulse + heal aura.
    facing: active target (Varo'then phase 1; demon waves phase 2; Mannoroth phase 3)
    movement: HIGH — Fel Firestorm patches require continuous relocation entire fight

  interrupt_role:
    doomguard_shadow_bolt_phase_2: PARTICIPATE if class has kick
    varothen_magistrike_phase_1: ATTEMPT — UNVERIFIED interruptibility
    mannoroth_shadow_bolt_volley: UNVERIFIED existence; opportunistic

  rotation_modifications:
    phase_1: |
      Standard ST on Varo'then:
      - Frost Mage: Frostbolt / Frostfire; Ice Lance on Fingers
      - Aff Warlock: Corruption + UA + BoD + Haunt; Drain Soul on execute
      - MM Hunter: Steady Shot / Aimed Shot rotation
      - Shadow Priest: VT + SW:P + Mind Blast + Mind Flay
      - Balance Druid: Eclipse-driven Wrath/Starfire
      - Elemental Shaman: LB + LvB + ES + FS
      DO NOT DPS Mannoroth phase 1 — Illidan tanks him; player damage trivial
      until sword embedded.
    phase_2: |
      Sword embedded — Mannoroth takes massive Magistrike Arcs proc damage.
      Player rotation contributes to total damage:
      - DoT classes: re-apply on Mannoroth (target swap)
      - ST classes: full rotation on Mannoroth
      Demon waves AoE:
        Felhounds (75%): light AoE; melee handles
        Felguards (60%): medium AoE; ranged contributes
        Doomguards (45%): interrupt + ranged DPS
        Infernals (30%): heavy AoE + Tyrande overwhelmed; rescue priority
      Dreadlord Debilitators: KILL PRIORITY ABSOLUTE; rescue Tyrande
    phase_3: |
      Gift of Sargeras execute. Burn Mannoroth. Stack near party for heal+AoE.

  sword_pickup_handler:
    designation: typically a ranged DPS (travel speed)
    notes: |
      ON VARO'THEN DEATH:
        1. Detect Varo'then death event
        2. Path to sword location (object position on ground)
        3. Right-click / use object (pickup)
        4. Acquire Mannoroth as target
        5. Activate sword-throw player buff (new ability bar item)
        6. Resume normal DPS rotation
      Bot needs explicit handler — default DPS rotation does NOT cover this.
      MM Hunter (mobility), Frost Mage (Blink), or Balance Druid (Travel
      Form) are best candidates.

  cd_use:
    phase_1_offensive: standard rotational on Varo'then engagement
    phase_2_offensive: |
      Bloodlust on phase 2 entry (typical group call). Stack offensive
      CDs on demon-wave AoE windows.
    phase_3_offensive: full burn — execute mode

  npc_ally_handling:
    illidan_stormrage: DO NOT TARGET (damage blacklist)
    tyrande_whisperwind: DO NOT TARGET (damage blacklist); track HP for Dreadlord rescue
    malfurion_stormrage: DO NOT TARGET (damage blacklist); detect phase 3 trigger

  moonbeam_handling:
    on_moonbeam_appear: |
      Stack on Tyrande's Moonbeam ground area. Provides Blessing of Elune
      (damage-vs-demons buff + light HP regen). Override default spread
      positioning during Moonbeam window.

  heroic_delta: |
    Heroic-only encounter; numbers above ARE heroic.

  bot_anti_patterns:
    - DO NOT target Mannoroth in phase 1 (Illidan tanks; player damage trivial)
    - DO NOT skip Dreadlord Debilitator kill (Tyrande rescue priority)
    - DO NOT stand in Fel Firestorm patches (continuous movement)
    - DO NOT damage Illidan / Tyrande / Malfurion
    - DO NOT cluster on Magistrike-arc target in phase 1 (3 max arc targets)
    - DO NOT skip Moonbeam stack (free buff + heal)
    - DO NOT continue DoTs on Varo'then after death (target swap to Mannoroth)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  perotharn:
    - encounter_state_phase_1 → standard_ranged_rotation_spread
    - encounter_detect_fel_flames_landing(self) → move_out_of_patch
    - encounter_detect_drain_essence_cast → suspend_dps_pre_phase_2
    - encounter_state_phase_2_active → ENTER_HIDE_STATE + suspend_casts + pillar_pathfind
    - encounter_state_phase_3_enfeebled → pop_offensive_cds_burn
    - encounter_state_phase_3_endless_frenzy → defensive_cd_burn

  queen_azshara:
    - encounter_state_active → target_resolution: kill_order_arcane_frost_fire
    - encounter_detect_total_obedience_cast → kick_rotation_participate (PRIORITY: HIGHEST)
    - encounter_detect_fireball_fire_magus → kick_rotation_participate (PRIORITY: high)
    - encounter_detect_hand_of_queen_mc(target=other) → switch_target_to_puppet_strings
    - encounter_detect_arcane_bomb_landing → move_out
    - encounter_detect_coldflame_line → sidestep
    - encounter_state_self_targeting_azshara → BLOCK + force_retarget_active_magus

  mannoroth:
    - encounter_state_phase_1 → target=varothen + spread_8_to_10y
    - encounter_detect_fel_firestorm_patch_under_self → relocate
    - encounter_detect_varothen_death → DESIGNATE_SWORD_HANDLER (if self) → execute_sword_pickup_handler
    - encounter_state_phase_2 → target_resolution: dreadlord_debilitator > doomguard > infernal > felguard > felhound > mannoroth
    - encounter_detect_tyrande_moonbeam_appear → stack_on_moonbeam
    - encounter_detect_dreadlord_debilitator_spawn → switch_target_to_dreadlord
    - encounter_state_phase_3 → target=mannoroth + stack_party

dispel_blacklist:
  - { spell_id: 105544, name: Fel Decay (Peroth'arn), reason: heal-vs-decay decision }
  # Ranged hybrids (Druid Remove Corruption, Shaman Cleanse Spirit) included for completeness

dps_target_blacklist:
  - { npc_id: 54853, name: Queen Azshara, reason: Shroud of Luminosity invuln }
  - { npc_id: UNVERIFIED, name: Eye of Peroth'arn, reason: scripted; despawn on phase end }
  - { npc_id: UNVERIFIED, name: Illidan Stormrage, reason: ally NPC }
  - { npc_id: UNVERIFIED, name: Tyrande Whisperwind, reason: ally NPC }
  - { npc_id: UNVERIFIED, name: Malfurion Stormrage, reason: ally NPC }

interrupt_priority_per_fight:
  perotharn:        []
  queen_azshara:    [Total Obedience (HIGHEST), Fireball (high), other magus casts (opportunistic)]
  mannoroth:        [Doomguard shadow_bolt, Varo'then Magistrike (if interruptible)]

phase_specific_states:
  perotharn_phase_2_hide:
    cast_inhibit: true
    movement: pillar_pathfind_los_clear
    duration: 40s OR phase_3_signal
  azshara_mc_event:
    target_switch: puppet_strings
    duration: until_strings_die
  mannoroth_phase_2_sword_handler:
    designation: typically_ranged_dps
    handler: execute_sword_pickup_sequence (object click + use on Mannoroth)
```
