# The Vortex Pinnacle — Ranged DPS

**Ranged-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how ranged DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content ranged DPS patterns (positioning, spread/stack rules, threat ceiling) live in `docs/roles/ranged-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (frost-mage, affliction-warlock, mm-hunter, shadow-priest, balance-druid, elemental-shaman)
- This doc layers Vortex-Pinnacle-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_ranged_dps_overview:
  total_bosses: 3
  estimated_full_clear_minutes: 25-45
  positioning_profile:
    ertan: STACKED_INSIDE_RING              # OVERRIDE — Storm's Edge punishes max-range; group stays inside cyclone ring
    altairus: UPWIND_OF_BOSS                # group repositions on Call the Wind direction shifts (~30s)
    asaad: SPREAD_15Y_BASELINE → STACK_IN_TRIANGLE_ON_GROUNDING_FIELD
  interrupt_load:
    ertan: medium                           # Lightning Bolt every ~10s; ranged participates if class has kick
    altairus: zero                          # Chilling Breath UNVERIFIED interruptibility; not in rotation
    asaad: zero                             # no kick-mandatory casts
  key_responsibilities:
    - ertan_stay_inside_ring                # ranged stacks at melee — exception to default
    - altairus_wind_position_track          # continuous spatial query
    - asaad_skyfall_star_kill_priority      # PRIMARY ranged duty on Asaad — continuous swap
    - asaad_chain_lightning_spread          # 15y+ from all other players
    - asaad_triangle_stack_on_grounding_field
    - lurking_tempest_target_suppression(heroic_ertan)
```

---

## Trash — ranged DPS notes

```yaml
trash_ranged:
  cyclone_summit_packs:
    cc_priorities: |
      Ranged contributes opener-CC:
      - Frost Mage: Polymorph (Sheep) — primary multi-CC
      - Affliction Warlock: Banish on elementals (Wild Vortex, Mistrals); Fear if available
      - MM Hunter: Freezing Trap (icetrap) on caster
      - Shadow Priest: Shackle Undead — limited use here (mobs not undead)
      - Balance Druid: Cyclone (8s)
      - Elemental Shaman: Hex (60s, breaks on damage)
    cloud_prince: |
      IMMUNE TO CC. AoE-burn priority before Turbulence completes:
      - Frost Mage: Blizzard / Cone of Cold
      - Aff Warlock: Seed of Corruption
      - MM Hunter: Multi-Shot / Trap Launcher Explosive Trap
      - Shadow Priest: Mind Sear (poor — only 4+ targets); Vampiric Touch + SW:P
      - Balance Druid: Hurricane / Wild Mushroom detonation
      - Elemental Shaman: Chain Lightning / Fire Nova on Magma Totem

  caster_packs:
    interrupt_role: |
      - Frost Mage: Counterspell (24s) on Wild Vortex / Turbulent Squall / Temple Adept
      - Affliction Warlock: Spell Lock via Felhunter pet
      - MM Hunter: Silencing Shot (24s)
      - Shadow Priest: Silence (45s)
      - Balance Druid: Solar Beam (60s; 8s AoE silence — STRONG on Adept packs)
      - Elemental Shaman: Wind Shear (6s; off-GCD; 25y range)
    temple_adept_priority: |
      Greater Heal MUST be interrupted. Highest interrupt priority in the
      dungeon's trash. CC Adept first if multi-Adept pull.

  bridge_howling_gales:
    notes: |
      RANGED IS BEST CLASS for this — kill them from outside the AoE.
      Standard ST rotation; do NOT stand on the bridge edges (knock-off
      hazard).

  empyrean_assassin:
    interrupt_role: |
      Vapor Form is the priority interrupt — re-stealth/heal mid-fight.
      Dispel Lethargic Poison from tank/melee if class has Poison cleanse
      (Mage/Warlock/SPriest/Druid/Shaman — most have it; Hunter does not).

  skyfall_star_trash:
    notes: |
      Trash version of boss-summoned Skyfall Star. Low HP — ranged DPS
      priority kill. First pack near Asaad can be skipped via right-wall.
```

---

## Boss 1 — Grand Vizier Ertan

```yaml
boss_ranged:
  boss: grand_vizier_ertan
  difficulty_grade: easy (positional override)
  positioning:
    OVERRIDE: |
      STACK INSIDE the cyclone ring at near-melee range. THIS IS AN
      EXPLICIT OVERRIDE of default ranged "stay at max range" behavior.
      Storm's Edge (86309) punishes anyone outside the ring with
      9,599-11,732 Nature DoT/sec.
    facing: any
    movement: |
      Minimal. Stand in a dead zone between cyclones; periodic retract
      events will pull cyclones inward but most stationary spots are stable.
      Active dodging not required for most positions (see survey D#7).

      HEROIC CAMERA RULE: face Ertan (boss is at platform center). Do NOT
      face platform edges where Lurking Tempests spawn.

  target_suppression:
    lurking_tempest_heroic: |
      DO NOT acquire Lurking Tempest as target. Stays harmlessly idle if
      not faced/targeted. Bot AI: encounter-level target-suppression list.
      FALLBACK: if a tempest IS active (somehow engaged), kill it as
      secondary priority.

  interrupt_role:
    ertan_lightning_bolt: PARTICIPATE if class has kick (Counterspell / Wind Shear / Silencing Shot etc.) — MEDIUM priority

  rotation_modifications:
    standard: |
      Single-target ST rotation on Ertan. Ranged inside ring = effectively
      melee-range cast positioning. Cast-while-moving classes have no
      advantage here (no movement required).

      - Frost Mage: Frostbolt / Frostfire Bolt; Ice Lance procs
      - Affliction Warlock: Corruption + Bane of Agony + UA + Haunt; DoT-
        roll. Drain Soul on execute.
      - MM Hunter: Steady Shot / Aimed Shot rotation; Master Marksman procs
      - Shadow Priest: VT + SW:P + Mind Blast + Mind Flay; DoT cycle
      - Balance Druid: Eclipse-driven Wrath/Starfire; instant DoT app
      - Elemental Shaman: LB + LvB on proc; Flame Shock maintain; Fulmination
        ES on charge cap

    cyclone_retract_dodge:
      action: minimal — stationary positions are stable in most cases
      if_clipped: take 23-26k Nature + 60% slow debuff for 12s; healer
        compensates, continue casting

  cd_use:
    save_for_real_bosses: |
      Asaad is the high-pressure boss. On Ertan, only rotational CDs.
      Save Bloodlust / Heroism / Time Warp for whoever the group calls
      it on (typically Altairus or Asaad).

  heroic_delta: |
    - Lurking Tempest target-suppression rule active
    - Cyclone retract damage higher; less forgiving on bad positions
    - Lightning Bolt damage higher; interrupt rotation more meaningful

  bot_anti_patterns:
    - DO NOT stay at max range (Storm's Edge punishes)
    - DO NOT auto-target Lurking Tempest on heroic (target-suppression)
    - DO NOT use AoE-knockback (Thunderstorm Shaman, Typhoon Druid) — could push allies out of ring
    - DO NOT interrupt cyclone retract event (it's not an interruptible cast)
```

---

## Boss 2 — Altairus

```yaml
boss_ranged:
  boss: altairus
  difficulty_grade: medium (positional)
  positioning:
    default: |
      UPWIND of Altairus. Boss should be downwind of you. Wind direction
      shifts every ~30s (Call the Wind event); ALL players reposition on
      shift.

      Spread for Chilling Breath cone safety: stay ~10y from other players
      so cone clipping is single-target only.

      HEROIC: Twisting Winds patrolling tornadoes — continuous evasion;
      do not stand at platform edges (knockup carries fall risk).
    facing: boss
    movement: |
      Reposition every ~30s on wind shift.
      If targeted by Chilling Breath: turn boss away from group OR move so
      cone clears group.
      Heroic: continuous Twisting Winds dodging.

  interrupt_role:
    chilling_breath: SKIP — UNVERIFIED interruptibility; standard strat does not kick
    none_required: no other interrupt-mandatory casts

  rotation_modifications:
    standard_with_movement: |
      Cast-while-moving classes have an advantage on heroic (Twisting Winds
      dodging):
      - Frost Mage: Frostbolt hardcast — must stop on Twisting Winds dodge;
        Ice Lance instant for procs
      - Affliction Warlock: DoTs are instant — keep up DoT-roll; Drain Soul
        channel breaks on movement
      - MM Hunter: Steady Shot hardcast; Aimed Shot on proc; Aspect of the
        Fox (talented) for cast-on-move — UNVERIFIED Cata
      - Shadow Priest: Mind Flay channels are mobility-friendly; DoTs instant
      - Balance Druid: Wrath/Starfire hardcast; Moonfire/Sunfire instant;
        Lunar Shower talent allows DoT-on-move DPS
      - Elemental Shaman: LB / LvB hardcast — must stop; FS instant; Lava
        Surge proc gives instant LvB

    upwind_buff_maintenance: |
      The Upwind buff (UNVERIFIED magnitude — likely +30% haste) is
      significant DPS boost. Re-acquire on EVERY wind shift.

    chilling_breath_cone_avoidance: |
      If you are the cone target: move to point cone away from group.
      If you are NOT the target: spread; move only to clear cone arc.

  cd_use:
    standard_offensive: rotational
    save_for_asaad: hold Bloodlust / Heroism / Time Warp if group calls it for Asaad

  heroic_delta: |
    - Twisting Winds (NEW MECHANIC) — continuous evasion penalty for hardcast classes
    - Chilling Breath damage roughly doubles — cone-target is healer-priority
    - Wind shift cadence same; reposition window tighter

  bot_anti_patterns:
    - DO NOT cluster ranged (Chilling Breath cone clip)
    - DO NOT stand still on heroic (Twisting Winds clip)
    - DO NOT stand at platform edges (knockup → fall → teleport to entrance)
    - DO NOT cast hardcast spells while in Twisting Winds path (interrupt)
    - DO NOT attempt to dispel Downwind of Altairus (positional debuff, NOT dispellable — see encounter.md DISPEL_BLACKLIST)
```

---

## Boss 3 — Asaad, Caliph of Zephyrs

```yaml
boss_ranged:
  boss: asaad_caliph_of_zephyrs
  difficulty_grade: hard (heroic) / medium (normal)
  positioning:
    default: |
      Spread 15y+ from ALL other players for Chain Lightning bounce safety
      (40y range, 50% reduced per bounce — 15y spread reliably breaks chain).

      Tank holds Asaad center; ranged spreads at perimeter (but inside
      platform, not at edge — fall risk).

      ON GROUNDING FIELD SPAWN: stack inside triangle (random per cast).
      Triangle position is NOT memorized — bot AI needs spatial query.
    facing: boss / Skyfall Star (current target)
    movement: |
      - Skyfall Star spawn → swap target, move to LoS the star
      - Chain Lightning cast → spread maintenance (already spread, no
        action needed)
      - Static Cling cast (heroic) → either eat root (default) or jump
        (NOT bot-reliable — defaults to eat-and-dispel)
      - Grounding Field spawn → move INTO triangle; stack until Supremacy
        of the Storm channel ends (~6s); resume spread after

  primary_responsibility:
    skyfall_star_kill_priority: |
      ABSOLUTE — ranged DPS swaps to whatever Skyfall Star is currently up.
      Stars die fast (low HP) but spawn continuously throughout fight.
      DPS loss from target-swap is acceptable; ignored stars compound
      damage on healer.

      Class behavior on swap:
      - Frost Mage: Frostbolt → Ice Lance combo on star; no DoT loss
      - Aff Warlock: DoTs RESET on swap (significant DPS loss for Aff);
        may swap to Conflagrate-style instant nukes on the star, then
        re-DoT Asaad. CONSIDER soul-swapping DoTs on/off star (talent).
      - MM Hunter: Hunter's Mark resets; Steady/Aimed continues unchanged
      - Shadow Priest: DoTs RESET on swap (same as Warlock); Mind Blast +
        Mind Spike on star
      - Balance Druid: Moonfire/Sunfire RESET; Eclipse-bar self preserved;
        Starfire/Wrath rotation continues
      - Elemental Shaman: Flame Shock RESET; LB chain on star; LvB on proc

  interrupt_role:
    none: no kick-mandatory casts; standard strat does not interrupt

  rotation_modifications:
    skyfall_star_swap: |
      On Skyfall Star spawn → swap target; full ST burst on star until
      dead (~3-5s); return to Asaad. Continuous throughout fight.

    chain_lightning_response: |
      If properly spread (15y+) → no action; continue rotation.
      If NOT spread → reposition to break the chain; brief DPS interruption.

    static_cling_response (heroic only): |
      Bot default = EAT THE ROOT, dispel by healer.
      Class-specific exceptions:
      - Frost Mage: Iceblock breaks root (5min CD; SAVE for emergency)
      - Aff Warlock: Demonic Circle teleport (talented) — UNVERIFIED Cata
      - MM Hunter: Disengage breaks root (UNVERIFIED — likely no, but
        Master's Call self-cast does)
      - Shadow Priest: Fade (talented Phantasm) breaks roots — UNVERIFIED
      - Balance Druid: Powershift to Travel/Cat — root-break
      - Elemental Shaman: no native root-break; rely on dispel

      JUMP-AVOIDANCE is NOT recommended for bot AI (sub-second timing).

    grounding_field_response: |
      MOVE INTO TRIANGLE. Triangle position is random per cast — bot AI
      uses spatial query "point in triangle" + "move to nearest interior
      point" pathing override.

      INSIDE triangle, continue DPS rotation on Asaad. Hardcast classes
      can resume; Twisting Winds-equivalent hazard does NOT apply on this
      fight.

      Skyfall Stars MAY spawn during Supremacy of the Storm — if star is
      OUTSIDE the triangle, DO NOT chase it (death). Wait for storm end.

  cd_use:
    bloodlust_heroism_time_warp: align with pull OR with major boss damage phase (pull-time most common)
    survival_cd_save: |
      Pop major defensive when in Chain Lightning chain or low HP from
      Skyfall Star Arcane Barrage:
      - Frost Mage: Iceblock (5min CD) — save for Static Cling root if
        Magic-dispeller is dead
      - Aff Warlock: Healthstone in combat; Soulstone self pre-fight
      - MM Hunter: Deterrence (-100% damage 5s)
      - Shadow Priest: Dispersion (-90% damage 6s) — SAVE for Supremacy
        of the Storm channel as backup if healer struggles
      - Balance Druid: Barkskin (-20% damage 12s)
      - Ele Shaman: Stoneclaw Totem (talented — shield self), Astral Recall
        not relevant in dungeon

  heroic_delta: |
    - Static Cling (NEW MECHANIC) — root threat; bot eats and waits for dispel
    - Skyfall Star damage higher; spawn cadence slightly faster
    - Chain Lightning damage higher
    - Supremacy of the Storm tick may scale (UNVERIFIED) — universally one-shots outside triangle

  bot_anti_patterns:
    - DO NOT cluster within 15y of another player (Chain Lightning bounce hazard)
    - DO NOT stand at platform edges (knockback hazards may exist; UNVERIFIED Asaad has knockback)
    - DO NOT chase Skyfall Stars OUTSIDE the triangle during Supremacy of the Storm channel (death)
    - DO NOT continue DPS on Asaad while Skyfall Star is alive (kill priority)
    - DO NOT attempt JUMP-avoidance for Static Cling (sub-second timing unreliable)
    - DO NOT auto-dispel anything during this fight EXCEPT Static Cling (heroic only) — there is no dispel-blacklist on this boss specifically
```

---

## Bot AI hooks

```yaml
ai_hooks:
  ertan:
    - encounter_state_active → maintain_position_inside_cyclone_ring (override default ranged-spread)
    - encounter_detect_lightning_bolt_cast → kick_rotation_participate (if class has kick)
    - encounter_state_lurking_tempest_present(heroic) → target_suppress (do NOT acquire as target)
    - encounter_state_lurking_tempest_active(heroic) → fallback_kill_priority (only if accidentally engaged)
    - encounter_detect_cyclone_retract_telegraph → continue_rotation (no action change for stable positions)

  altairus:
    - encounter_state_active → maintain_position_upwind_of_boss
    - encounter_detect_wind_direction_shift → reposition_to_new_upwind
    - encounter_detect_chilling_breath_cast(target=self) → reposition_cone_away_from_group
    - encounter_detect_chilling_breath_cast(target=other) → spread_maintenance
    - encounter_state_twisting_winds_active(heroic) → continuous_evasion_pathing
    - DISPEL_BLACKLIST: Downwind of Altairus

  asaad:
    - encounter_detect_skyfall_star_spawn → swap_target_to_skyfall_star (KILL_PRIORITY_ABSOLUTE)
    - encounter_state_skyfall_star_dead → return_to_asaad
    - encounter_detect_chain_lightning_cast → maintain_15y_spread
    - encounter_detect_static_cling_applied(target=self)(heroic) → self_immobilize_break_if_class_has_one ELSE await_magic_dispel
    - encounter_detect_unstable_grounding_field_spawn → move_into_triangle (spatial query pathing override)
    - encounter_detect_supremacy_of_the_storm_channel → continue_rotation_inside_triangle
    - encounter_state_supremacy_of_the_storm_active AND skyfall_star_outside_triangle → suppress_kill_priority_until_storm_ends

dispel_blacklist:
  - { spell_id: UNVERIFIED, name: Downwind of Altairus, scope: altairus_fight, force: true }
  # ranged hybrids that can dispel (Mage Remove Curse, Druid Remove Corruption, etc.) typically don't have Magic-dispel access on these schools

target_suppression:
  - { npc_name: Lurking Tempest, scope: ertan_fight_heroic, fallback: kill_if_active }

position_overrides:
  ertan:    INSIDE_CYCLONE_RING (override max-range default)
  altairus: UPWIND_OF_BOSS (continuous re-position on direction shifts)
  asaad:    SPREAD_15Y_BASELINE → STACK_IN_TRIANGLE_ON_GROUNDING_FIELD

cd_priority_per_fight:
  ertan:    [reactive_only]
  altairus: [reactive_only — bloodlust possible if group calls it here]
  asaad:    [bloodlust_pull_or_grounding_field; major defensive on Static Cling root if no Magic dispeller]
```
