# The Vortex Pinnacle — Melee DPS

**Melee-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how melee DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content melee DPS patterns (positioning, interrupt rotation, threat ceiling) live in `docs/roles/melee-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (combat-rogue, fury-warrior, ret-paladin, frost-dk, enh-shaman, feral-cat)
- This doc layers Vortex-Pinnacle-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_melee_dps_overview:
  total_bosses: 3
  estimated_full_clear_minutes: 25-45
  positioning_profile:
    ertan: melee_inside_ring                # default melee-on-boss; ring positioning automatic
    altairus: behind_boss + upwind_of_boss  # layered: behind for cleave avoidance, upwind for buff
    asaad: behind_boss + 15y_from_others    # spread-from-other-players even at melee range; STACK in triangle on Grounding Field
  interrupt_load:
    ertan: medium                           # Lightning Bolt every ~10s; melee in range, primary kicker
    altairus: zero                          # Chilling Breath UNVERIFIED interruptibility
    asaad: zero                             # no kick-mandatory casts
  key_responsibilities:
    - ertan_lightning_bolt_interrupt_rotation
    - altairus_wind_position_track          # unusual for melee — boss-relative position matters
    - altairus_chilling_breath_cone_avoid   # if targeted, reposition
    - asaad_chain_lightning_spread          # melee usually clusters; this fight wants spread
    - asaad_triangle_stack_on_grounding_field
    - lurking_tempest_avoid_facing(heroic_ertan)  # camera management OR target-suppression
```

---

## Trash — melee DPS notes

```yaml
trash_melee:
  cyclone_summit_packs:
    cc_priorities: |
      Melee bot contributes opener-CC:
      - Combat Rogue: Sap on humanoid (some packs); Blind on emergency
      - Fury Warrior: Hammer of Justice (none — Pally only); Intimidating Shout fear (group, situational)
      - Ret Pally: Hammer of Justice (6s stun) on initial cast; Repentance for humanoid CC
      - Frost DK: Strangulate (5s silence)
      - Enh Shaman: Hex (60s frog) — STRONG on humanoids; Earthbind Totem for kiting
      - Feral Cat: Maim (5 CP for stun) — limited use; Bash from bear form (cooldown swap)

  cloud_prince:
    notes: |
      IMMUNE TO CC. Melee-burn fast before Turbulence cast completes:
      - Combat Rogue: SnD up + Eviscerate on 5 CP burst
      - Fury Warrior: Bloodthirst + Raging Blow + Heroic Strike rage dump
      - Ret Pally: TV at 3 HP + CS on CD
      - Frost DK: Obliterate burst on rune-ready
      - Enh Shaman: Stormstrike + Lava Lash + maelstrom-stack burst
      - Feral Cat: Mangle + Shred burst (no time for full Rip rotation)

  bridge_howling_gales:
    notes: |
      Melee bot stays AT TANK'S POSITION; do NOT walk into gale AoE
      footprint (knock-off = teleport to entrance). Standard melee on
      tank's target; the gales themselves are best DPSed by ranged from
      outside.

  fear_packs (none in this dungeon):
    notes: |
      No fear-cast trash mobs identified in Vortex Pinnacle. Standard melee
      flow.

  empyrean_assassin:
    interrupt_role: |
      Vapor Form is the priority interrupt — assassin re-stealths to heal
      otherwise. Melee in range MUST kick on rotation:
      - Combat Rogue: Kick (10s CD)
      - Fury Warrior: Pummel (10s CD; off-GCD)
      - Ret Pally: Rebuke (15s CD; off-GCD)
      - Frost DK: Mind Freeze (10s CD; off-GCD)
      - Enh Shaman: Wind Shear (6s CD; off-GCD; 25y range)
      - Feral Cat: Skull Bash (60s CD; gap-close + interrupt)
    poison_dispel: |
      Lethargic Poison stacks on tank/melee. Class-specific cleanse:
      - Ret Pally: Cleanse (Poison + Disease)
      - Enh Shaman: Cleanse Spirit (NOT Poison — Resto only) — UNVERIFIED
        Enh has Cleanse Spirit; mostly tied to spec
      - Feral Cat: Remove Corruption (Curse + Poison) — NEW in Cata; UNVERIFIED Feral
      - others: no native poison cleanse
      Healer is primary poison-dispeller.

  temple_adept:
    notes: |
      MELEE DPS PRIMARY KICKER on Greater Heal cast. Highest-value interrupt
      in the dungeon's trash. CC Adept first if multi-Adept pull.

  servant_of_asaad:
    notes: physical melee charges; tank-and-spank; standard melee flow

  executor_of_the_caliph:
    notes: |
      Defensive buff cast UNINTERRUPTIBLE — wait it out, no kick available.
      Standard melee on tank's target.
```

---

## Boss 1 — Grand Vizier Ertan

```yaml
boss_melee:
  boss: grand_vizier_ertan
  difficulty_grade: easy
  positioning:
    default: |
      Melee on Ertan — automatically inside cyclone ring (boss is at center;
      melee at her feet is well inside ring).
    facing: |
      Standard melee positioning (behind/side of boss). HEROIC: do NOT face
      platform edges (Lurking Tempest spawn locations).
    movement: |
      Stay-still — Ertan is pinned by tank. Cyclone retract events: most
      stationary positions are stable; if you see a cyclone path coming
      directly at you, sidestep ~3-5y (rare).
  interrupt_role:
    ertan_lightning_bolt: PRIMARY — melee DPS interrupt rotation (Lightning Bolt on ~10-15s; rotate kicks with tank + melee)
    cyclone_retract: DO NOT INTERRUPT (passive event, not interruptible)

  target_suppression:
    lurking_tempest_heroic: |
      DO NOT acquire as target. Stay on Ertan. If melee bot gets caught
      with tempest activated as target (unlikely — melee is on boss), drop
      tempest and resume Ertan rotation.

  rotation_modifications:
    standard: |
      Standard ST rotation on Ertan:
      - Combat Rogue: SnD maint + Eviscerate at 5 CP + Sinister Strike CP build + RvS refresh; Adrenaline Rush + Killing Spree on CD
      - Fury Warrior: BT + RB Enraged + Slam Bloodsurge proc + WW + HS dump; Death Wish + Recklessness on CD
      - Ret Pally: Inq refresh + TV at 3 HP + CS on CD + Judge + Exo on AoW; AW + Zealotry on CD
      - Frost DK: Obliterate priority + Howling Blast on Rime proc + FS rune dump + KM-FS proc spike; Pillar of Frost + Empower Rune Weapon on CD
      - Enh Shaman: SS + LL on CD + FS maintain + LB dump on 5MW; Feral Spirits + Shamanistic Rage on CD
      - Feral Cat: Mangle + Rake/Rip pandemic + SR maint + Shred filler + FB on >25% energy + Tiger's Fury on procs; Berserk + INC on CD

    cyclone_retract_response: |
      No action change for stationary melee position; cyclones retract
      THROUGH boss center, not along melee positions.

  cd_use:
    standard_offensive: rotational; pop on engagement
    save_for_real_bosses: |
      Asaad is the high-pressure boss. Save Bloodlust (Enh Shaman) /
      Heroism / Time Warp for Asaad if group calls it there. Otherwise
      pop on Ertan opener.

  heroic_delta: |
    - Lurking Tempest target-suppression rule active
    - Lightning Bolt damage higher; interrupt rotation more meaningful
    - Cyclone retract damage higher

  bot_anti_patterns:
    - DO NOT face platform edges on heroic (Lurking Tempest activation)
    - DO NOT auto-target Lurking Tempest (target-suppression)
    - DO NOT use AoE knockback (Death Knight Death Grip pull is fine; Warrior Charge / Heroic Leap — fine; Shaman Thunderstorm — DO NOT, would push allies out of ring)
```

---

## Boss 2 — Altairus

```yaml
boss_melee:
  boss: altairus
  difficulty_grade: medium (positional)
  positioning:
    default: |
      BEHIND boss + UPWIND of boss. Layered positional:
      1. Behind boss = standard melee positioning (avoid Chilling Breath
         cone if boss faces toward you AND you happen to be the target)
      2. Upwind of boss = Upwind buff side (haste/movement)

      Tank should orient boss so behind-boss IS the upwind side. If wind
      shifts mid-fight, tank reorients; melee moves with boss.
    facing: boss back
    movement: |
      Reposition every ~30s on wind shift. Tank moves boss to a new facing;
      melee rotates with the tank.

      If targeted by Chilling Breath: move to point cone away from group.
      Cone is 120° from Altairus's facing — if you are BEHIND boss, you are
      not in cone (the cone is frontal). Most cone hits land on whoever is
      currently in front of boss.

      HEROIC: dodge Twisting Winds patrolling tornadoes. Continuous evasion;
      do NOT stand at platform edges (knockup → fall hazard).

  interrupt_role:
    chilling_breath: SKIP — UNVERIFIED interruptibility; standard strat does not kick

  rotation_modifications:
    standard_with_movement: |
      Heroic Twisting Winds = movement-friendly classes have advantage:
      - Combat Rogue: Sprint to dodge; mobility-rich
      - Fury Warrior: Charge / Heroic Leap to re-engage post-dodge
      - Ret Pally: standard melee with limited mobility — accept ~5% DPS
        loss to movement on heroic
      - Frost DK: Death's Advance (talented — UNVERIFIED) for movement
      - Enh Shaman: Ghost Wolf for movement; Spiritwalker's Grace not Enh
      - Feral Cat: Stampeding Roar for raid mobility; Dash self

    upwind_buff_maintenance: |
      Re-acquire Upwind buff on every wind shift. The buff is significant
      DPS (haste — UNVERIFIED magnitude). If you are stuck downwind:
      - Drop rotation priority temporarily
      - Move to upwind side (behind boss, standard)
      - Resume rotation

    chilling_breath_response: |
      If you are NOT the target: stay in position; cone is from boss's face
      to a point in front, you are behind so you're safe.
      If you ARE the target: move so boss-cone clears group; standard
      "tank-target-self" reposition.

  cd_use:
    standard_offensive: rotational; align with Tiger's Fury / Berserk / Avenging Wrath / Pillar of Frost on engagement
    bloodlust_window: if group calls it here, align CDs

  heroic_delta: |
    - Twisting Winds (NEW MECHANIC) — continuous movement penalty
    - Chilling Breath damage roughly doubles
    - Wind shift reposition window tighter

  bot_anti_patterns:
    - DO NOT stand still on heroic (Twisting Winds clip)
    - DO NOT follow boss off platform if tank pushes too far (some tanks try edge-positioning — fall risk)
    - DO NOT continue front-of-boss DPS (face is Chilling Breath cone)
    - DO NOT attempt to dispel Downwind of Altairus (positional, NOT dispellable; melee bot rarely has dispel anyway)
    - DO NOT use Death Grip-equivalent abilities to pull boss off tank (Frost DK Death Grip — DON'T)
```

---

## Boss 3 — Asaad, Caliph of Zephyrs

```yaml
boss_melee:
  boss: asaad_caliph_of_zephyrs
  difficulty_grade: hard (heroic) / medium (normal)
  positioning:
    default: |
      Behind Asaad + 15y from EVERY OTHER PLAYER. The 15y spread is for
      Chain Lightning bounce safety — even melee must spread, which is
      unusual.

      Tank holds Asaad center; melee positions behind boss. Other melee
      bots and the tank are within 5y, but ranged is far. Chain Lightning
      bounces 40y between targets; 15y player-to-player spread is the
      bounce-break threshold.

      In a typical 3-DPS comp: 1-2 melee + 1-2 ranged. If 2+ melee, they
      cluster behind boss — Chain Lightning will bounce between them. This
      is acceptable on normal; on heroic, the 2nd melee should position
      ~5-10y off the boss to break direct-melee adjacency. (Bot heuristic:
      simpler is "all melee stack behind boss; healer compensates for chain
      damage on melee cluster.")
    facing: boss back / Skyfall Star (if engaged)
    movement: |
      - Skyfall Star spawn → if star paths through melee zone, dodge; do
        NOT chase to ranged zone (let ranged DPS handle the kill)
      - Chain Lightning cast → maintain spread (already behind boss; small
        adjustment if clustered with another melee)
      - Static Cling cast (heroic) → typically targets ranged, but melee
        can occasionally be hit. Eat root + healer dispels.
      - Grounding Field spawn → MOVE INTO TRIANGLE; stack until Supremacy
        of the Storm channel ends (~6s); resume behind-boss after

  interrupt_role:
    none: no kick-mandatory casts

  rotation_modifications:
    skyfall_star_handling: |
      MELEE DOES NOT swap to Skyfall Stars. Stars are ranged-DPS priority
      kill. Melee continues Asaad rotation. Exception: if a Skyfall Star
      lands directly in melee range, melee can cleave-kill it via:
      - Combat Rogue: Fan of Knives if 3+ targets (rare); otherwise Sinister
        Strike priority on whichever closest
      - Fury Warrior: Whirlwind sweeps the star + boss
      - Ret Pally: Divine Storm cleaves star + boss
      - Frost DK: standard rotation hits star via cleave (Howling Blast
        cleaves)
      - Enh Shaman: standard rotation has cleave via Magma Totem + Lightning
        Shield Static Shock / Earthen Power — limited
      - Feral Cat: Swipe (Cat) cleaves star
      DEFAULT: stay on Asaad; if star incidentally dies, it's a bonus.

    chain_lightning_response: |
      Already behind boss = some natural separation from ranged. Multi-melee
      clusters take chain damage; healer compensates.

    static_cling_response (heroic): |
      Bot default = EAT THE ROOT, healer dispels.
      Class-specific exceptions (rare for melee — Static Cling typically
      targets ranged):
      - Ret Pally: Hand of Freedom self-cast OR Cleanse self
      - Frost DK: Anti-Magic Shell (75% magic absorb 5s) — absorbs the
        Static Cling cast on every other application
      - Feral Cat: powershift to remove root (Travel form / Cat form swap)
      - Combat Rogue / Fury Warrior / Enh Shaman: no native root-break;
        rely on dispel

      JUMP-AVOIDANCE not bot-reliable.

    grounding_field_response: |
      Move INTO triangle. Triangle is random per cast; bot AI uses spatial
      query. Continue Asaad rotation INSIDE triangle (you can still hit boss
      from inside — boss is held in or near triangle by tank; if tank stays
      central, triangle near boss is fine).

      If tank is OUTSIDE triangle (rare — tank should also enter): brief
      DPS gap until storm ends. Do NOT chase tank outside triangle.

  cd_use:
    bloodlust_window: align with pull or Grounding Field if group calls it
    survival_cd_save: |
      Pop major defensive on Static Cling root (heroic) if Magic-dispeller
      is dead OR overwhelmed:
      - Combat Rogue: Cloak of Shadows (90s) — UNVERIFIED if it removes
        Magic Static Cling; testing required
      - Fury Warrior: Spell Reflection — questionable (UNVERIFIED if it
        reflects Static Cling)
      - Ret Pally: Divine Protection (-20% magic 10s); Hand of Freedom
        breaks root
      - Frost DK: Anti-Magic Shell (75% magic absorb) absorbs the cast
      - Enh Shaman: Shamanistic Rage; Stoneclaw Totem
      - Feral Cat: Survival Instincts (-50% damage 12s); powershift breaks
        root

  heroic_delta: |
    - Static Cling (NEW MECHANIC) — typically targets ranged but occasional melee hit
    - Skyfall Star damage higher (cleave benefit if in melee range)
    - Chain Lightning damage higher (multi-melee clusters more punishing)
    - Supremacy of the Storm tick may scale (UNVERIFIED) — universally one-shots outside triangle

  bot_anti_patterns:
    - DO NOT cluster all melee at exact same spot (Chain Lightning bounce; some natural spacing helps)
    - DO NOT chase Skyfall Stars to ranged zone (let ranged DPS handle)
    - DO NOT chase Skyfall Stars OUTSIDE triangle during Supremacy of the Storm (death)
    - DO NOT continue front-of-Asaad DPS (he is ranged caster — face matters less, but face the cleave risk if any)
    - DO NOT attempt JUMP-avoidance for Static Cling (sub-second timing unreliable)
    - DO NOT pull boss off tank with Death Grip-equivalent abilities (Frost DK)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  ertan:
    - encounter_state_active → standard_melee_position_behind_boss
    - encounter_detect_lightning_bolt_cast → kick_rotation_participate (PRIMARY for melee)
    - encounter_state_lurking_tempest_present(heroic) → target_suppress
    - encounter_state_lurking_tempest_active(heroic) → fallback_kill_priority

  altairus:
    - encounter_state_active → maintain_position_behind_boss + maintain_upwind_aura
    - encounter_detect_wind_direction_shift → reposition_with_tank
    - encounter_detect_chilling_breath_cast(target=self) → reposition_cone_away_from_group
    - encounter_state_twisting_winds_active(heroic) → continuous_evasion_pathing

  asaad:
    - encounter_state_active → maintain_position_behind_boss + 15y_from_other_players
    - encounter_detect_skyfall_star_spawn → continue_dps_on_asaad (cleave incidentally if star in range)
    - encounter_detect_chain_lightning_cast → maintain_spread (small adjustment if multi-melee cluster)
    - encounter_detect_static_cling_applied(target=self)(heroic) → self_immobilize_break_if_class_has_one ELSE await_magic_dispel
    - encounter_detect_unstable_grounding_field_spawn → move_into_triangle (spatial query pathing override)
    - encounter_detect_supremacy_of_the_storm_channel → continue_rotation_inside_triangle
    - encounter_state_supremacy_of_the_storm_active AND skyfall_star_outside_triangle → suppress_kill_priority_until_storm_ends

dispel_blacklist:
  - { spell_id: UNVERIFIED, name: Downwind of Altairus, scope: altairus_fight, force: true }
  # melee bots typically don't dispel; included for completeness on hybrid melee (Ret Pally Cleanse, Enh Shaman Cleanse Spirit talented)

target_suppression:
  - { npc_name: Lurking Tempest, scope: ertan_fight_heroic, fallback: kill_if_active }

position_overrides:
  ertan:    BEHIND_BOSS_INSIDE_RING (default melee on boss; ring positioning automatic)
  altairus: BEHIND_BOSS + UPWIND_OF_BOSS (continuous re-position with tank on wind shifts)
  asaad:    BEHIND_BOSS + 15Y_FROM_OTHERS → STACK_IN_TRIANGLE_ON_GROUNDING_FIELD

cd_priority_per_fight:
  ertan:    [reactive_only; bloodlust if group calls pull-time]
  altairus: [reactive_only]
  asaad:    [bloodlust_pull_or_grounding_field; major defensive on Static Cling root if no Magic dispeller]
```
