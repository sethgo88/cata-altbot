# Well of Eternity — Melee DPS

**Melee-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how melee DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content melee DPS patterns (positioning, interrupt rotation, threat ceiling) live in `docs/roles/melee-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (combat-rogue, fury-warrior, ret-paladin, frost-dk, enh-shaman, feral-cat)
- This doc layers WoE-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_melee_dps_overview:
  total_bosses: 3 (heroic-only)
  estimated_full_clear_minutes: 25-40
  positioning_profile:
    perotharn:
      phase_1: behind_boss
      phase_2: HIDE_BEHIND_PILLAR             # OVERRIDE — no melee target during stealth
      phase_3: behind_boss
    queen_azshara: behind_active_magus         # rear-arc on each new active magus
    mannoroth_phase_1: behind_varothen + spread_8_to_10y for Magistrike arc
    mannoroth_phase_2: in_range_of_demons; cleave demon waves
    mannoroth_phase_3: stack_tight_for_gift_of_sargeras
  interrupt_load:
    perotharn: zero                            # Drain Essence is Illidan's job
    queen_azshara: HIGH                        # Total Obedience MUST INTERRUPT + Fire Magus Fireball
    mannoroth: medium                          # Doomguard shadow_bolt + Varo'then Magistrike
  key_responsibilities:
    - phase_2_hide_state_perotharn             # do NOT engage; pathfind to pillar
    - dps_blacklist_azshara                    # do NOT target Azshara herself
    - magi_kill_order_arcane_frost_fire        # encoded DPS priority
    - puppet_strings_dps_switch_on_mc          # MC event interrupt
    - total_obedience_interrupt_priority       # WIPE if missed
    - npc_ally_blacklist                       # do NOT damage Illidan/Tyrande/Malfurion
    - mannoroth_npc_tank_awareness             # bot does NOT tank Mannoroth phase 1
    - dreadlord_debilitator_kill_priority      # rescue Tyrande phase 2
```

---

## Trash — melee DPS notes

```yaml
trash_melee:
  illidan_stealth_section:
    notes: |
      Bot follows Illidan in stealth. DO NOT engage during Shadowcloak escort
      (breaks party stealth and aggros trash). Hold abilities.
  crystal_packs:
    cc_priorities: |
      Polymorph / Sap available targets first if multi-CC team. Melee bot
      contributes:
      - Combat Rogue: Sap on humanoid Highborne / satyr — opener-CC
      - Ret Paladin: Hammer of Justice (stun)
      - Other melee: kick + DPS on skull marker
    fear_packs:
      notes: |
        Highborne / satyr fear-cast — interrupt on rotation:
        - Combat Rogue: Kick (10s CD)
        - Fury Warrior: Pummel (10s CD; off-GCD)
        - Ret Pally: Rebuke (15s CD; off-GCD)
        - Frost DK: Mind Freeze (10s CD; off-GCD)
        - Enh Shaman: Wind Shear (6s CD; off-GCD)
        - Feral Cat: Skull Bash (60s CD; gap-close + interrupt)
  highborne_caster_packs:
    interrupt_role: same kick rotation; rotate kicks across casters
    positioning: standard side/rear melee on caster
  shores_demon_trash:
    notes: |
      Felguard / Felhound / Infernal preview. Cleave + AoE rotation.
      Ret Pally / Fury Warrior / Frost DK favor cleave packs; Combat Rogue
      uses Blade Flurry; Enh Shaman drops Magma Totem.
  ally_npcs:
    BLACKLIST: |
      Bot targeting must NEVER include Illidan / Tyrande / Malfurion. Default
      tab-target should filter friendlies; explicit blacklist for any AoE
      auto-cleave that catches them.
```

---

## Boss 1 — Peroth'arn

```yaml
boss_melee:
  boss: perotharn
  difficulty_grade: medium (positional + phase-2 unique)

  positioning:
    phase_1: behind boss (rear arc); standard melee
    phase_2: |
      HIDE BEHIND PILLAR. NO MELEE TARGET (boss is vanished). Pathfind
      to pillar OUT of Eye line-of-sight; stay still 40s.
      DO NOT engage Eyes of Peroth'arn (scripted; despawn on phase end).
    phase_3: behind boss; resume rotation
    facing: boss back
    movement: minimal in phase 1/3; stationary in phase 2

  interrupt_role:
    drain_essence: NO — Illidan handles. Do NOT waste kick CD.
    fel_flames: instant cast; no kick
    fel_decay: instant cast; no kick

  rotation_modifications:
    phase_1:
      standard: |
        Standard ST melee rotation:
        - Combat Rogue: Sinister Strike → Eviscerate at 5 CP; SnD maint
        - Fury Warrior: Bloodthirst → Raging Blow → Slam-Bloodsurge proc
        - Ret Pally: Crusader Strike → Templar's Verdict at 3 HP; Inquisition maint
        - Frost DK: Obliterate → Frost Strike; Howling Blast on Rime
        - Enh Shaman: Stormstrike → Lava Lash → Maelstrom Weapon procs
        - Feral Cat: Mangle → Shred → Rip + Rake maint
      movement: |
        Fel Flames places ground patches on random players. Bot melee
        moves out of patch ASAP — gap-closer back to boss after.
    phase_2:
      cast_inhibit: true                        # implicit; no boss to attack
      movement: pathfind_to_pillar
      duration: 40s
      DO_NOT_ATTACK_EYES: true
    phase_3:
      enfeebled_path: |
        Boss takes +25% damage. Burn rotation; pop offensive CDs:
        - Combat Rogue: Adrenaline Rush + Killing Spree
        - Fury Warrior: Recklessness + Death Wish
        - Ret Pally: Avenging Wrath + Zealotry + Guardian of Ancient Kings
        - Frost DK: Pillar of Frost + Empower Rune Weapon
        - Enh Shaman: Feral Spirit + Elemental Mastery
        - Feral Cat: Berserk + Tiger's Fury
      endless_frenzy_path: |
        Boss does +25% damage. Burn-but-survive; defensive CDs as needed:
        - Combat Rogue: Cloak of Shadows for any magic spike
        - Fury Warrior: Shield Wall (defensive stance) — UNVERIFIED Fury access
        - Ret Pally: Divine Protection (-20% magic 10s)
        - Frost DK: Anti-Magic Shell (75% magic absorb 5s)
        - Enh Shaman: Shamanistic Rage + Stoneclaw
        - Feral Cat: Survival Instincts (-50% damage 12s)

  cd_use:
    standard_offensive: rotational on engagement
    save_for_phase_3: hold major offensive CDs for Enfeebled phase 3

  heroic_delta: |
    Heroic-only encounter; numbers above ARE heroic.

  bot_anti_patterns:
    - DO NOT engage during phase 2 (boss is vanished)
    - DO NOT attack Eyes of Peroth'arn (scripted)
    - DO NOT stand in Fel Flames patches
    - DO NOT use kick on Drain Essence (Illidan's job)
    - DO NOT continue rotation during Drain Essence stun
```

---

## Boss 2 — Queen Azshara

```yaml
boss_melee:
  boss: queen_azshara
  difficulty_grade: hard (target priority + interrupt scheduling + DoT/CP reset on swap)

  positioning:
    default: behind ACTIVE magus (rear arc); spread from other melee for Coldflame
    facing: active magus back
    movement: |
      Move out of Coldflame line (Frost Magus charge); Firebomb / Blast Wave
      (Fire Magus pbaoe); Arcane Bomb ground patch.

  interrupt_role:
    total_obedience: |
      HIGHEST PRIORITY in dungeon. Bot interrupt rotation MUST reserve a kick:
      - Combat Rogue: Kick (10s CD; off-GCD)
      - Fury Warrior: Pummel (10s CD; off-GCD)
      - Ret Pally: Rebuke (15s CD; off-GCD)
      - Frost DK: Mind Freeze (10s CD; off-GCD)
      - Enh Shaman: Wind Shear (6s CD; off-GCD; 25y range — BEST)
      - Feral Cat: Skull Bash (60s CD; gap-close + interrupt)
      Missing all attempts = wipe.
    fireball_fire_magus: |
      Secondary interrupt for healer mana. Rotate kicks if available.
    other_magus_casts: opportunistic interrupts on rotation

  rotation_modifications:
    target_priority: |
      Default DPS target on Azshara fight (highest first):
        1. Puppet strings (if MC event active)
        2. Active Arcane Magus (kill priority 1)
        3. Active Frost Magus (kill priority 2)
        4. Active Fire Magus (kill priority 3)
        5. (NEVER target Azshara — Shroud of Luminosity = invuln)
    cp_dot_classes_target_swap: |
      Magi switch is target swap for CP/DoT-based melee:
      - Combat Rogue: CP attached to target — RESET on magus swap. Build CP
        on new active magus. Apply Slice and Dice via 1 CP from new target.
      - Frost DK: Diseases on target — re-Outbreak on each new magus.
      - Feral Cat: CP attached to target — same as Rogue, full CP reset.
        Re-apply Mangle + Rake + Rip on new target.
      - Fury Warrior / Ret Pally / Enh Shaman: most buffs are self-buffs
        (Inquisition, Savage Roar, Lightning Shield) — unaffected by target
        swap. Standard rotation continues.
    mc_event: |
      Switch to puppet strings IMMEDIATELY on Hand of the Queen MC. Strings
      die fast; resume magus DPS after free.
    aoe_window: |
      Magi don't AoE-cluster (1 active at a time + tank-pulled magus).
      No major AoE rotation; pure ST priority.
      EXCEPTION: if another magus auto-aggros from cleave, Combat Rogue Blade
      Flurry / Ret Pally Divine Storm / Fury Whirlwind catches both — accept
      the bonus damage.

  cd_use:
    save_for_arcane: |
      First magus is Arcane (kill priority 1). Pop major offensive CDs on
      first Arcane magus to maximize damage during high-priority kill.
    bloodlust_alignment: per group call; typically saved for execute or pre-pull

  heroic_delta: |
    Heroic-only encounter; numbers above ARE heroic.

  bot_anti_patterns:
    - DO NOT target Azshara (Shroud of Luminosity = invuln; wasted DPS)
    - DO NOT skip Total Obedience interrupt (wipe if missed)
    - DO NOT continue magus DPS during MC event (kill puppet strings first)
    - DO NOT stand in magus AoEs (Coldflame / Firebomb / Arcane Bomb / Blast Wave)
    - DO NOT cluster melee (Coldflame line + Charge multi-hit)
    - DO NOT spend CP on dying magus (waste; build new on next active)
```

---

## Boss 3 — Mannoroth and Varo'then

```yaml
boss_melee:
  boss: mannoroth_and_varothen
  difficulty_grade: hard (multi-phase + NPC awareness + sword pickup support)

  positioning:
    phase_1: |
      Behind Varo'then (rear arc). SPREAD 8-10y from other melee for
      Magistrike arc mitigation. Constantly relocate out of Fel Firestorm
      patches.
    phase_2: |
      In melee range of demon waves (Felhounds / Felguards / Doomguards /
      Infernals as they spawn). Maintain proximity to Illidan for Hand of
      Flame mitigation. Stack on Tyrande's Moonbeam when it appears.
    phase_3: |
      In melee range of Mannoroth (rear arc). Stack tight near party for
      Gift of Sargeras AoE pulse + heal aura.
    facing: boss/demon back
    movement: HIGH — Fel Firestorm patches require continuous relocation entire fight

  interrupt_role:
    doomguard_shadow_bolt_phase_2: PARTICIPATE; rotate kicks
    varothen_magistrike_phase_1: ATTEMPT — UNVERIFIED interruptibility
    mannoroth_shadow_bolt_volley: UNVERIFIED existence; opportunistic

  rotation_modifications:
    phase_1: |
      Standard ST on Varo'then:
      - Combat Rogue: SS → Evis at 5 CP; SnD maint
      - Fury Warrior: BT → RB → Slam-Bloodsurge
      - Ret Pally: CS → TV at 3 HP; Inquisition
      - Frost DK: Obliterate → Frost Strike
      - Enh Shaman: Stormstrike → Lava Lash → MW
      - Feral Cat: Mangle → Shred → Rip/Rake
      DO NOT engage Mannoroth phase 1 — Illidan tanks; player damage trivial
      until sword embedded. Bot stays on Varo'then.
    phase_2: |
      Sword embedded — Mannoroth takes massive Magistrike Arcs proc damage.
      Bot melee target priority:
        1. Dreadlord Debilitator (rescue Tyrande)
        2. Doomguard (interrupt + DPS)
        3. Mannoroth (sword proc damage)
        4. Other demons (Felhound / Felguard / Infernal as they spawn)
      Spec-specific demon-wave rotation:
      - Combat Rogue: Blade Flurry on; Fan of Knives on Felhound cluster
      - Fury Warrior: Whirlwind / Bladestorm on cluster
      - Ret Pally: Divine Storm at 3 HP
      - Frost DK: Howling Blast spam on cluster; Death and Decay
      - Enh Shaman: Magma Totem + Chain Lightning at 5 MW
      - Feral Cat: Swipe Cat-form on cluster; Rake on primary target
      DoT/CP-based melee target swap (same Azshara pattern):
      - Combat Rogue: CP RESETS per target
      - Frost DK: diseases per target
      - Feral Cat: CP RESETS per target
      - Fury Warrior / Ret Pally / Enh Shaman: self-buffs; unaffected
    phase_3: |
      Gift of Sargeras execute. Burn Mannoroth. Stack near party.

  sword_pickup_handler:
    primary_designation: typically a ranged DPS (travel speed)
    backup: melee_dps if no ranged available
    notes: |
      Default — ranged handles. Melee bot NOT first-pick for this. If forced
      (no ranged in group), melee bot uses gap-closer:
      - Fury Warrior: Charge / Heroic Leap to sword
      - Ret Paladin: gap-close via judgment glyph speed buff
      - Frost DK: Death's Advance + run
      - Enh Shaman: Ghost Wolf
      - Feral Cat: Travel Form (Cat-Cheetah)
      Pickup sequence is the same:
        1. Detect Varo'then death
        2. Path to sword
        3. Right-click / use object
        4. Acquire Mannoroth
        5. Activate sword-throw buff

  cd_use:
    phase_1_offensive: standard rotational on Varo'then engagement
    phase_2_offensive: |
      Bloodlust on phase 2 entry (typical group call). Stack offensive CDs
      on demon-wave AoE windows (Felguard 60% / Doomguard 45%).
    phase_3_offensive: full burn — execute mode
    defensive_save: |
      Phase 2 demon waves can stack damage. Defensive CD on Infernal wave
      (30%) — heaviest AoE pulse.

  npc_ally_handling:
    illidan_stormrage: DO NOT TARGET (damage blacklist; he's tanking Mannoroth)
    tyrande_whisperwind: DO NOT TARGET; track HP for Dreadlord rescue
    malfurion_stormrage: DO NOT TARGET; phase 3 trigger detection
    mannoroth_phase_1: DO NOT ENGAGE (Illidan's job; bot stays on Varo'then)

  moonbeam_handling:
    on_moonbeam_appear: |
      Stack on Tyrande's Moonbeam ground area. Provides Blessing of Elune
      (damage-vs-demons buff + light HP regen). Override default spread
      positioning during Moonbeam window.

  heroic_delta: |
    Heroic-only encounter; numbers above ARE heroic.

  bot_anti_patterns:
    - DO NOT engage Mannoroth phase 1 (Illidan tanks; bot stays on Varo'then)
    - DO NOT skip Dreadlord Debilitator kill (Tyrande rescue priority)
    - DO NOT stand in Fel Firestorm patches (continuous movement)
    - DO NOT damage Illidan / Tyrande / Malfurion
    - DO NOT cluster melee on Magistrike-arc target (3 max arc targets)
    - DO NOT skip Moonbeam stack (free buff + heal)
    - DO NOT spend CP / refresh diseases on Varo'then near death (target swap to Mannoroth phase 2)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  perotharn:
    - encounter_state_phase_1 → standard_melee_rotation_behind_boss
    - encounter_detect_fel_flames_landing(self) → move_out_of_patch
    - encounter_detect_drain_essence_cast → suspend_dps_pre_phase_2
    - encounter_state_phase_2_active → ENTER_HIDE_STATE + suspend_attacks + pillar_pathfind
    - encounter_state_phase_3_enfeebled → pop_offensive_cds_burn
    - encounter_state_phase_3_endless_frenzy → defensive_cd + burn

  queen_azshara:
    - encounter_state_active → target_resolution: kill_order_arcane_frost_fire
    - encounter_detect_total_obedience_cast → kick_rotation_participate (PRIORITY: HIGHEST)
    - encounter_detect_fireball_fire_magus → kick_rotation_participate (PRIORITY: high)
    - encounter_detect_hand_of_queen_mc(target=other) → switch_target_to_puppet_strings
    - encounter_detect_arcane_bomb_landing → move_out
    - encounter_detect_coldflame_line → sidestep
    - encounter_detect_fire_magus_blast_wave_telegraph → range_out_briefly
    - encounter_state_self_targeting_azshara → BLOCK + force_retarget_active_magus
    - encounter_detect_magus_swap → reset_cp_or_diseases_on_new_target

  mannoroth:
    - encounter_state_phase_1 → target=varothen + spread_8_to_10y + behind_boss
    - encounter_detect_fel_firestorm_patch_under_self → relocate
    - encounter_detect_varothen_death → ranged_priority_sword_pickup; if no ranged self_handle
    - encounter_state_phase_2 → target_resolution: dreadlord > doomguard > infernal > felguard > felhound > mannoroth
    - encounter_detect_tyrande_moonbeam_appear → stack_on_moonbeam
    - encounter_detect_dreadlord_debilitator_spawn → switch_target_to_dreadlord
    - encounter_state_phase_3 → target=mannoroth + stack_party

dispel_blacklist:
  - { spell_id: 105544, name: Fel Decay (Peroth'arn), reason: heal-vs-decay decision }
  # melee bots typically don't dispel; included for hybrid melee (Ret Pally Cleanse, Enh Shaman Cleanse Spirit)

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
    do_not_attack: [Eye of Peroth'arn]
  azshara_mc_event:
    target_switch: puppet_strings
    duration: until_strings_die
  mannoroth_phase_1_target_lock:
    locked_target: varothen
    blacklist_target: mannoroth                # Illidan tanking; player damage trivial
  mannoroth_phase_2_target_priority:
    priority_list:
      - dreadlord_debilitator
      - doomguard
      - infernal
      - felguard
      - felhound
      - mannoroth
```
