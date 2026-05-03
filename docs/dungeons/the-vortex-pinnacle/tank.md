# The Vortex Pinnacle — Tank

**Tank-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the tank reacts. Class-agnostic where possible.

**Layering**:
- Cross-content tank patterns (threat priority, defensive CD pacing, taunt rules) live in `docs/roles/tank.md` (deferred to tank phase).
- Per-spec rotation / CD / threat model lives in the corresponding `docs/specs/` tank doc (all tank specs deferred to tank phase).
- This doc layers Vortex-Pinnacle-specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_tank_overview:
  total_bosses: 3
  estimated_full_clear_minutes: 25-45
  threat_profile:
    ertan: low                              # standard tank-and-spank; group stacks in ring
    altairus: low                           # standard with reposition on wind shifts
    asaad: low                              # tank holds boss center; mostly stand-still
  defensive_cd_profile:
    ertan: light                            # only Lightning Bolt is tank-relevant; cyclone retract is positional
    altairus: light                         # standard melee; Chilling Breath if tank ever targeted (rare — cones random)
    asaad: light                            # standard melee
  key_responsibilities:
    - threat_priority_per_fight
    - position_inside_ring (Ertan)
    - position_upwind_for_buff (Altairus)
    - reposition_on_wind_shift (Altairus)
    - position_in_triangle_on_grounding_field (Asaad)
    - face_away_from_group (Empyrean Assassin / Cloud Prince trash; bosses)
    - lurking_tempest_face_management(heroic_ertan)  # camera management OR target-suppression
```

---

## Trash — tank notes

```yaml
trash_tank:
  cyclone_summit_packs:
    pull_pattern: |
      Caster trash (Wild Vortex, Gust Soldier) is CC-able. Default pull
      pattern: skull on lead caster (kill priority), CC on second (cross/X
      markers).
    facing: away from group (frontal cleaves on Cloud Prince)
    cloud_prince: |
      MINI-BOSS-style. CANNOT be CC'd. Tank pulls to a wall (group spread
      backs); takes Turbulence AoE-on-cast hit; pop short defensive on
      Turbulence telegraph.

  bridge_howling_gales:
    pull_pattern: |
      Tank engages first Howling Gale at edge of bridge AoE; group DPSes
      to disable; tank advances; engages second Gale similarly. CRITICAL:
      do NOT push past the gale into knock-off range.
    facing: |
      Position central on the bridge — knock-off can come from either side
      depending on Gale animation orientation.
    edge_avoidance: |
      ABSOLUTE — falling = teleport to dungeon entrance for tank, party
      wipes shortly after.

  nimbus_rise_packs:
    turbulent_squall: |
      Caster — DPS interrupts; tank holds position. Healer purges if class
      has it.
    empyrean_assassin: |
      Stealth-style melee. Tank Sap-vulnerability on initial pull or
      acquire on Vapor Form interrupt by DPS. Lethargic Poison stacks on
      tank — healer dispels.
    young_storm_dragon: |
      MOST DANGEROUS trash mob in the dungeon. Tank pulls to wall; faces
      away from group (frontal breath cone). Pop major defensive on first
      AoE damage tick. Healer mana check.

  temple_of_asaad_packs:
    servant_of_asaad: tank-and-spank; no special handling
    temple_adept: |
      Healer trash — DPS interrupts Greater Heal. Tank holds position with
      group ready to focus-fire Adept on skull marker. Mind Control by
      Priest is canonical play.
    executor_of_the_caliph: |
      Large melee; AoE damage on uninterruptible defensive cast. Pop short
      defensive during cast. Tank-and-spank otherwise.
    minister_of_air: caster — position near Grounding Field environmental object if accessible
    skyfall_star_trash: |
      Low HP; ranged DPS handles. Tank doesn't need to engage unless they
      come into melee range.
```

---

## Boss 1 — Grand Vizier Ertan

```yaml
boss_tank:
  boss: grand_vizier_ertan
  difficulty_grade: easy
  threat_intensity: low

  positioning:
    pull: tank pulls Ertan to platform CENTER; cyclones spawn around the
      tank position (boss's position becomes ring-center)
    facing: |
      Standard tank face-away. HEROIC: face TOWARD platform center (where
      Ertan is) — tank does not face platform edges where Lurking Tempests
      spawn.
    movement: minimal; do NOT kite. Cyclone retract events: stationary
      central position is stable.
    cyclone_retract_response: |
      No action change for stationary tank. Cyclones retract toward boss
      and back; tank is at boss position so cyclones pass around tank, not
      into tank.

  threat_management:
    standard: |
      Standard tank threat rotation:
      - Prot Warrior: Shield Slam + Revenge + Devastate; Thunder Clap on AoE
      - Prot Pally: Crusader Strike + Holy Wrath + Hammer of Wrath; Avenger's
        Shield on cooldown for ranged threat
      - Blood DK: Death Strike + Heart Strike + Rune Strike; Death and Decay
        for AoE
      - Feral Druid (Bear): Mangle + Lacerate stack; Swipe (Bear) + Thrash
        for AoE
    lightning_bolt_threat: |
      Ertan auto-casts Lightning Bolt at current target (tank). Standard
      threat lead is sufficient.

  defensive_cd_use:
    lightning_bolt_unintercepted: |
      If Lightning Bolt is NOT interrupted in time, tank takes ~10k Nature
      damage. Pop short defensive (Shield Block / Barkskin / Bone Shield
      proc — passive — / Pain Suppression equivalent) if multiple
      consecutive bolts land.
    cyclone_retract: standard tanking — no CD needed if positioned at center
    long_fight: rotate Shield Wall / Survival Instincts / Icebound Fortitude
      / Divine Protection on natural cooldown for healer mana relief

  interrupt_role:
    ertan_lightning_bolt: PRIMARY interrupt rotation participant — tank's
      kick counts in the rotation; rotate with melee + ranged kicks
    rotation_partners: [tank, melee_dps, ranged_dps_with_kick]

  target_suppression:
    lurking_tempest_heroic: |
      DO NOT acquire Lurking Tempest as target. Tank stays on Ertan. If a
      tempest activates (someone faced it), DPS handles kill — tank does
      NOT move to engage tempest.

  heroic_delta: |
    - Lurking Tempest target-suppression rule
    - Lightning Bolt damage higher; interrupt mandatory rather than optional
    - Cyclone retract damage higher (positional — affects DPS more than tank)

  bot_anti_patterns:
    - DO NOT kite Ertan (mechanic-intended stationary tank position)
    - DO NOT face platform edges on heroic (tempest activation hazard for ranged)
    - DO NOT chase Lurking Tempest (target-suppression)
    - DO NOT use AoE knockback (no ability common to tanks does this — but Druid Typhoon if Feral-bear-with-talent: DO NOT)
```

---

## Boss 2 — Altairus

```yaml
boss_tank:
  boss: altairus
  difficulty_grade: medium (positional)
  threat_intensity: low

  positioning:
    pull: tank pulls Altairus near platform CENTER
    facing: |
      AWAY from group AND oriented so behind-boss is the UPWIND side.
      That is: turn boss so the upwind direction is behind boss. Group
      stacks behind boss (upwind); tank is in front of boss but laterally
      centered.

      Wait — tank can't be in front (Chilling Breath cone hits front).
      Correct: tank is at boss's tail; faces boss away from group. Group
      stacks on tank's side (= upwind side). Boss faces away into open
      sky / empty side.
    movement: |
      Reposition every ~30s on Call the Wind direction shift. Tank rotates
      boss so behind-boss is the new upwind side. Group follows tank.

      Tank also wants Upwind buff like everyone else.

      HEROIC: dodge Twisting Winds patrolling tornadoes. Continuous evasion;
      tank stays central — knockup at platform edge = fall hazard for tank.
    chilling_breath_response: |
      If tank is the Chilling Breath target (rare — typically random non-
      tank), reorient boss so cone clears group. Otherwise, no tank action
      needed for Chilling Breath cast.

  threat_management:
    standard: standard tank threat — Altairus melees the tank; standard
      reactive threat rotation
    on_reposition: |
      Wind shift = boss rotation. Threat is unaffected by rotation; tank
      maintains threat through reposition.

  defensive_cd_use:
    standard: rotational cooldown use
    twisting_winds_clip: pop short defensive if you take a Twisting Winds
      hit (~12k Nature + knockup)
    chilling_breath_clip: |
      Tank is rarely the Chilling Breath target. If tank takes the cone
      (positional bug), pop major defensive — frost damage 25-35k normal,
      47-52k heroic.

  interrupt_role:
    chilling_breath: SKIP — UNVERIFIED interruptibility; standard strat does not kick

  heroic_delta: |
    - Twisting Winds (NEW MECHANIC) — tank moves with platform
    - Chilling Breath damage roughly doubles
    - Wind shift cadence same; reposition window tighter

  bot_anti_patterns:
    - DO NOT push boss to platform edge (knockup → fall)
    - DO NOT chase boss into Twisting Winds (let them pass)
    - DO NOT stand still on heroic (Twisting Winds clip)
    - DO NOT face boss INTO group (Chilling Breath cone hits group)
    - DO NOT attempt to dispel Downwind of Altairus on self (positional, NOT dispellable)
```

---

## Boss 3 — Asaad, Caliph of Zephyrs

```yaml
boss_tank:
  boss: asaad_caliph_of_zephyrs
  difficulty_grade: easy (tank — most pressure on healer/DPS)
  threat_intensity: low

  positioning:
    pull: tank pulls Asaad to platform CENTER and HOLDS HIM THERE
    facing: away from group (standard); Asaad is a caster boss, frontal
      cleave is minor / non-existent
    movement: |
      MINIMAL. Tank holds boss centrally so:
      - Skyfall Stars can be picked off by ranged from any angle
      - Chain Lightning targeting is consistent
      - Grounding Field triangle is reachable from any direction

      ON GROUNDING FIELD SPAWN: tank moves INTO triangle (random per cast).
      Tank may need to drag Asaad along — Asaad CAN be dragged into the
      triangle area, which makes melee DPS easier during Supremacy of the
      Storm channel.

      Some sources (UNVERIFIED) say Asaad has knockback. If so, tank pulls
      back to center after each knockback.

  threat_management:
    standard: |
      Standard tank threat. Asaad is a caster — mostly Nature damage
      auto-casts; tank threat lead generally easy to maintain.
    skyfall_star_threat: |
      Tank does NOT pick up Skyfall Stars. They cast on random ranged;
      tank threat doesn't matter. Stay on Asaad.
    chain_lightning_threat: |
      Chain Lightning targets randomly. Tank position is one of the targets
      among many; threat unaffected.

  defensive_cd_use:
    standard: rotational
    static_cling_on_self (heroic — rare): |
      Static Cling typically targets ranged. If tank gets Static Cling
      (rare):
      - Prot Warrior: PvP trinket; otherwise eat root + dispel
      - Prot Pally: Hand of Freedom self-cast OR Cleanse self
      - Blood DK: Anti-Magic Shell (75% absorb); Lichborne (talented;
        breaks roots) — UNVERIFIED Blood DK
      - Feral (Bear) Druid: Powershift to remove root
    supremacy_of_the_storm: |
      Inside triangle, channel ticks are survivable. Pop short defensive
      if tank HP drops below 60% during channel (Shield Block / Bone
      Shield-up / Barkskin / Survival Instincts).

  interrupt_role:
    none: no kick-mandatory casts on this fight

  triangle_response: |
    Grounding Field spawn → move INTO triangle. Drag Asaad along if
    possible (improves melee DPS during Supremacy channel). Triangle is
    random per cast — bot AI uses spatial query.

  heroic_delta: |
    - Static Cling (unlikely to target tank, but possible)
    - Skyfall Star damage higher (tank ignores; ranged handles)
    - Chain Lightning damage higher
    - Supremacy of the Storm tick may scale (UNVERIFIED)

  bot_anti_patterns:
    - DO NOT chase Skyfall Stars (ranged DPS responsibility)
    - DO NOT taunt Skyfall Stars off ranged (they don't aggro from threat — they cast on random)
    - DO NOT stand outside Grounding Field triangle during Supremacy of the Storm (ONE-SHOT, even for tank)
    - DO NOT attempt JUMP-avoidance for Static Cling (sub-second timing)
    - DO NOT hold boss far from triangle on Grounding Field spawn (drag boss into triangle for melee DPS)
    - DO NOT attempt to dispel anything (tank's role; healer handles dispels)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  ertan:
    - encounter_state_active → standard_tank_position_at_platform_center
    - encounter_detect_lightning_bolt_cast → kick_rotation_participate (PRIMARY rotation member)
    - encounter_state_lurking_tempest_present(heroic) → target_suppress (do NOT engage)
    - encounter_detect_cyclone_retract_telegraph → no_action_change

  altairus:
    - encounter_state_active → tank_position_central_with_boss_facing_away
    - encounter_detect_wind_direction_shift → rotate_boss_so_behind_is_upwind (group reposition leader)
    - encounter_detect_chilling_breath_cast(target=self) → reposition_cone_away_from_group
    - encounter_state_twisting_winds_active(heroic) → continuous_evasion_pathing_at_center
    - DISPEL_BLACKLIST: Downwind of Altairus

  asaad:
    - encounter_state_active → hold_boss_at_platform_center
    - encounter_detect_skyfall_star_spawn → no_action_change (ranged DPS handles)
    - encounter_detect_chain_lightning_cast → maintain_position
    - encounter_detect_static_cling_applied(target=self)(heroic) → self_immobilize_break_if_class_has_one ELSE await_magic_dispel
    - encounter_detect_unstable_grounding_field_spawn → move_into_triangle (drag boss along)
    - encounter_detect_supremacy_of_the_storm_channel → continue_threat_rotation_inside_triangle
    - encounter_state_supremacy_of_the_storm_active → defensive_cd_short_if_below_60pct

dispel_blacklist:
  - { spell_id: UNVERIFIED, name: Downwind of Altairus, scope: altairus_fight, force: true }

target_suppression:
  - { npc_name: Lurking Tempest, scope: ertan_fight_heroic, fallback: kill_if_active }

defensive_cd_priority_per_fight:
  ertan:    [reactive_short_CD on Lightning Bolt clip; long_CD rotation for healer mana relief]
  altairus: [reactive_short_CD on Twisting Winds clip; reactive on Chilling Breath self-target]
  asaad:    [reactive_short_CD inside triangle if Supremacy ticks drop tank HP; major CD on Static Cling self-root if no Magic dispeller]

position_overrides:
  ertan:    PLATFORM_CENTER (boss is held centrally; cyclones orbit around)
  altairus: PLATFORM_CENTER + ROTATE_BOSS_ON_WIND_SHIFT
  asaad:    PLATFORM_CENTER → DRAG_INTO_TRIANGLE_ON_GROUNDING_FIELD

key_environmental_hazards_per_fight:
  ertan:
    - cyclone_retract: stationary central tank is stable
    - lurking_tempest_edge_spawn: target_suppress (do not engage)
  altairus:
    - twisting_winds_patrol(heroic): central tank evasion
    - platform_edge_fall: AVOID approach — knockup hazard
  asaad:
    - chain_lightning_bounce: 15y spread (tank usually has natural separation)
    - supremacy_of_the_storm_outside_triangle: ONE-SHOT — must be inside triangle
```
