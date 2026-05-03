# Grim Batol — Healer

**Healer-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the healer reacts. Class-agnostic where possible.

**Layering**:
- Cross-content healer patterns (triage hierarchy, dispel taxonomy, mana pacing, anti-patterns) live in `docs/roles/healer.md`
- Per-spec rotation / CD / mana model lives in the active healer's `docs/specs/` doc (resto-shaman, holy-paladin, holy-priest, disc-priest, resto-druid)
- This doc layers Grim Batol-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_healer_overview:
  total_bosses: 4
  estimated_full_clear_minutes: 30-45
  mana_profile:
    umbriss: moderate                          # tank Bleeding Wound + Blitz reactive
    throngus: high                             # Sword stance is heavy steady tank damage; Mace Impaling Slam is reactive
    drahga: moderate_to_high                   # P2 Valiona descent has predictable damage spikes (Devouring Flames + Seeping Twilight + Shredding Swipe)
    erudax: high                               # Shadow Gale group AoE every cycle + Feeble Body tank pressure + Corruptor add wave
  drink_windows:
    - after Umbriss
    - after Throngus                           # important — go into Drahga full mana
    - after Drahga                             # MOST IMPORTANT — Erudax is the OOM-test boss
  key_responsibilities:
    - dispel_handling                          # Shield of Nightmares on Erudax Corruptors (AUTO_DISPEL); Binding Shadows REACTIVE_ONLY
    - mana_pacing_for_erudax_dps_check
    - cooldown_pacing                          # Mana Tide / Spirit Link / Hymn of Hope / Innervate placement
    - anti_pattern_awareness                   # do NOT auto-dispel Binding Shadows; do NOT step out of Shadow Gale safe zone
    - shadow_gale_aoe_heal_alignment
```

---

## Trash — healer notes

```yaml
trash_healer:
  twilight_humanoid_casters:
    dispels: []                                  # no significant dispel calls on standard trash
    cooldown_use: none required; reactive only
    notes: |
      Watch for Twilight Cultist / Beguiler / Faceless trash hardcasts —
      DPS interrupts; healer triages reactive damage if kicks miss.

  trogg_packs:
    notes: |
      Skardyn / Trogg packs apply melee bleeds on tank + melee. Bleeds NOT
      dispellable; HoT-roll the tank. If a melee DPS gets stack, they're a
      soft tank — apply HoT.

  drakonid_packs:
    notes: tank takes frontal_cleave damage; standard tank-heal flow

  rookery_approach:
    notes: |
      Faceless trash + Mature Twilight Drake heavy hits; tank takes both
      frontal cleave AND tail swipe on the Drake. Standard tank focus.

  bombing_run_sequence:
    notes: |
      If group uses drake-bombing skip, healer is mounted on a drake too;
      no healing during the skip. Otherwise standard trash healing.
```

---

## Boss 1 — General Umbriss

```yaml
boss_healer:
  boss: general_umbriss
  difficulty_grade: easy_to_medium
  mana_intensity: moderate

  threshold_overrides:
    tier_1_tank_emergency: tank.hp_pct < 40        # Bleeding Wound stack + Blitz path-clip can compound
    tier_2_tank_priority: tank.hp_pct < 80         # KEEP TANK >= 90% for Bleeding Wound auto-removal (UNVERIFIED mechanic)

  predictable_spike_events:
    - bleeding_wound_application:
        warning: melee swing on tank
        damage: 10s tank DoT
        response: |
          Maintain tank above 90% HP if community-source mechanic holds
          (auto-removes debuff at >=90%). HoT-roll tank as primary action.
          - Resto Shaman: Riptide + Earth Shield
          - Holy Paladin: Beacon of Light + Holy Light spam
          - Holy Priest: Renew + Greater Heal
          - Disc Priest: Atonement-Smite + PW:S maint
          - Resto Druid: Lifebloom 3-stack + Rejuv refresh
    - blitz_charge_target:
        warning: ~3s telegraph cast on Umbriss
        damage: massive physical hit + knockback
        response: |
          Pre-shield random non-tank if available (PW:S / Earth Shield).
          Reactive heal post-impact. Track which player Umbriss faces.
    - frenzy_at_30:
        damage: ramping tank melee (+50% attack speed, +50% damage)
        response: pop tank-cooldown-ally if available (Pain Suppression / Guardian Spirit / Spirit Link Totem); HoT-roll heavily

  cooldown_use:
    standard: reactive HoT on tank (Riptide / Beacon / Renew / PW:S / Lifebloom)
    frenzy_window: align mid-tier CD (Spirit Link Totem / Pain Suppression / Power Infusion) at 35% HP
    no_unique_cd_save: |
      Save Mana Tide / Innervate / Hymn for Throngus or Erudax — Umbriss
      shouldn't burn major mana CD.

  dispel_priority:
    - { spell: Frenzy (96800), dispel_type: UNVERIFIED, action: SOOTHE_IF_AVAILABLE_NOT_HEALER_DISPEL }
    notes: |
      Frenzy may be soothable by Hunter Tranq Shot / Druid Soothe / Rogue
      Shiv. Healer dispel kit does NOT remove enrage. Skip.

  interrupt_role: []                              # no kick-required casts

  heroic_delta: |
    - Blitz reportedly one-shots undergeared players — pre-shield is more valuable
    - Trogg adds hit harder; Modgud's Malady stacks more dangerous on melee
    - Frenzy at 30% more punishing — defensive CD on rotation

  bot_anti_patterns:
    - DO NOT chase Malignant Trogg position (DPS handles kiting; healer stays in healing range)
    - DO NOT auto-dispel anything (no relevant healer dispel calls)
    - DO NOT panic-flash-heal Bleeding Wound — it's a steady DoT, not a burst
```

---

## Boss 2 — Forgemaster Throngus

```yaml
boss_healer:
  boss: forgemaster_throngus
  difficulty_grade: medium
  mana_intensity: high

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50        # Sword stance Dual Blades + Thrash burst
    melee_dps_emergency: dps_melee.hp_pct < 60     # Cave In patches + Mace Impaling Slam splash

  predictable_spike_events:
    - sword_stance_steady_drain:
        damage: heavy sustained tank damage (Dual Blades + Thrash)
        response: |
          HEAVIEST mana drain phase. Use efficient heals between spikes:
          - Resto Shaman: Healing Wave (cheap) > Greater Healing Wave (spike)
          - Holy Paladin: Holy Light efficient; Word of Glory at 3 HP
          - Holy Priest: Heal (efficient) > Greater Heal (spike); Chakra: Serenity
          - Disc Priest: PW:S + Atonement-Smite efficient
          - Resto Druid: Nourish efficient; Healing Touch with Nature's Grace proc
    - mace_stance_impaling_slam:
        warning: instant cast, no telegraph
        damage: random-player burst + 5s DoT
        response: reactive heal jump-target; do NOT pre-shield (random target)
    - shield_stance_flaming_arrow:
        warning: arrow patches on floor
        damage: predictable ground damage if positioning fails
        response: standard reactive; pre-shield melee who must reposition

    - cave_in_patches:
        warning: Mighty Stomp 2s cast; patches drop after
        damage: persistent ground patch ticks
        response: heal whoever's slow to move; reposition self if needed

    - disorienting_roar:
        damage: AoE -50% haste (debuff, not damage)
        response: |
          NOT a damage event but an OUTPUT event. Bot still outputs heals at
          normal cast speed (server-side haste; debuff hits players, slows
          our healer too).
          DO NOT attempt dispel by default (UNVERIFIED dispellability — see
          encounter.md Disagreement #2). Flag healer dispel set if DBC
          confirms Magic.

  cooldown_use:
    sword_stance: |
      Heaviest pressure phase. Mana Tide Totem / Hymn of Hope / Innervate
      candidate if in stance for 20+ seconds. Track stance-rotation timer.
    shield_stance: |
      Recovery window. Boss is rooted; group is repositioning behind. Heal
      pacing eases. Use this time to top off and regen mana (cast slightly
      slower / use efficient heals).
    mace_stance: |
      Reactive on Impaling Slam jumps. Tank kiting; healer follows at
      healing-range distance, NOT in kite path (lava trail UNVERIFIED).
    avenging_wrath_holy_pally: standard rotational on engagement
    spirit_link_totem: deploy if multiple players take Cave In + Impaling Slam simultaneously

  dispel_priority:
    - { spell: Disorienting Roar (74976), dispel_type: UNVERIFIED, default: NO_AUTO_DISPEL, notes: "flag for DBC; Wowhead n/a, Glowberry claims Magic" }

  interrupt_role:
    disorienting_roar: ATTEMPT — if class has kick (Holy Pally Rebuke, Disc/Holy Priest none, Resto Shaman Wind Shear, Resto Druid Skull Bash if cat form)

  heroic_delta: |
    - Mace stance damage especially severe; tank cannot stand-tank
    - Disorienting Roar may stack on heroic per GuiasWow
    - Flaming Arrow patches hit harder; precision matters
    - Lava trail on Mace path more pronounced (UNVERIFIED)

  bot_anti_patterns:
    - DO NOT stand in kite-path during Mace stance (lava trail UNVERIFIED but conservative avoid)
    - DO NOT stand IN front of boss during Shield stance (in cleave arc)
    - DO NOT auto-dispel Disorienting Roar (default — flag for DBC)
    - DO NOT burn Mana Tide / equivalent in Shield stance (recovery window)
```

---

## Boss 3 — Drahga Shadowburner

```yaml
boss_healer:
  boss: drahga_shadowburner
  difficulty_grade: medium
  mana_intensity: moderate_to_high

  threshold_overrides:
    tier_1_group_emergency: party.injured_count(radius=20, hp_pct_lt=40) >= 3   # P2 Devouring Flames clip
    tier_2_tank_emergency: tank.hp_pct < 50

  predictable_spike_events:
    - phase_1_burning_shadowbolt:
        damage: ranged single-target on tank or random
        response: standard reactive; minor tank-pressure
    - phase_1_invoked_flaming_spirit_supernova:
        warning: spirit spawned from Invocation of Flame portal; 10s window before fixate-impact
        damage: massive AoE / one-shot if reaches target
        response: |
          NOT primarily a healer mechanic — DPS kills spirit before impact.
          BUT: pre-shield FIXATED PLAYER if your kit has it (PW:S / Earth
          Shield). Reactive heal if Supernova lands.
    - phase_2_devouring_flames:
        warning: Valiona breath cast (~2-4s); telegraphs direction
        damage: heavy fire cone (potential one-shot)
        response: |
          PRIMARY counter is POSITIONING (group moves behind Valiona). Healer
          MOVES with group; do NOT cast through breath. Reactive heal post-
          breath if any clipping occurred.
    - phase_2_seeping_twilight:
        damage: shadow patches on random players (~6-9k + 50% slow)
        response: reactive heal patch-clip; do NOT cleanse slow (UNVERIFIED dispellability)
    - phase_2_shredding_swipe:
        damage: frontal cone melee on tank + cone-clip
        response: standard tank healing; pre-shield if cone clip likely

  cooldown_use:
    phase_1: |
      Light healing pressure. Save mana CDs for Phase 2 / Phase 3 execute.
    phase_2_descent: |
      HEAVIEST PHASE. Mana Tide / Hymn of Hope candidate at Phase-2 entry
      (Drahga HP == 25%). Spirit Link Totem if Devouring Flames clips
      multiple. Tranquility / Divine Hymn / Hymn of Mercy as emergency raid
      heal if 3+ players drop below 30%.
    phase_3_execute: |
      Boss returns to ground; Phase 1 mechanics. Recovery window for any
      remaining mana drain.
    barkskin_resto_druid: rotational; Tree of Life on Phase 2 if available
    avenging_wrath_holy_pally: align with Phase 2 entry

  dispel_priority: []                            # no confirmed dispels

  interrupt_role:
    burning_shadowbolt: ATTEMPT — if class has kick

  heroic_delta: |
    - Spirits hit MUCH harder; pre-shield fixated player is high value
    - Devouring Flames may only exist on heroic (Disagreement #6)
    - More frequent spirit spawns in Phase 2 — multiple in flight simultaneously possible

  bot_anti_patterns:
    - DO NOT keep casting through Devouring Flames cast (move with group)
    - DO NOT attempt to dispel Seeping Twilight slow (UNVERIFIED + low value)
    - DO NOT burn Mana Tide in Phase 1 (save for Phase 2 entry)
```

---

## Boss 4 — Erudax, the Duke of Below

```yaml
boss_healer:
  boss: erudax_duke_of_below
  difficulty_grade: hard
  mana_intensity: high

  threshold_overrides:
    tier_1_tank_emergency: tank.hp_pct < 50         # Feeble Body 200% damage taken
    tier_2_group_aoe: party.injured_count(radius=40, hp_pct_lt=60) >= 3   # Shadow Gale tick

  predictable_spike_events:
    - shadow_gale_telegraph:
        warning: Erudax cast (~5s telegraph per community guides; 1.25s on spell page)
        damage: whole-room AoE during cast
        response: |
          PRE-CAST AoE heal cooldown to land during the channel:
          - Resto Shaman: Healing Stream Totem + Healing Rain on safe-zone
          - Holy Paladin: Holy Radiance + Light of Dawn on stack
          - Holy Priest: Circle of Healing + PoH at center
          - Disc Priest: PoM bounces + Power Word: Barrier ground-place at safe zone
          - Resto Druid: Wild Growth on collapsed group + Tranquility on heroic
          MOVE TO SAFE ZONE FIRST, then cast — center is mandatory
          positioning, NOT optional.
    - enfeebling_blow_feeble_body:
        warning: tank knocked back ~400 distance units on Enfeebling Blow
        damage: 200% damage taken on tank for 5s
        response: |
          PRE-SHIELD tank if available (PW:S / Sacred Shield). Tank should
          WAIT OUT 5s before re-engaging — heal them through the wait.
          Earth Shield / Beacon / Lifebloom maintenance is critical here.
    - binding_shadows_on_player:
        warning: 1.5s cast telegraph
        damage: shadow damage tick + 8s root + heals Erudax 3x
        response: |
          PRIMARY counter is POSITIONING (player moves out of telegraph).
          REACTIVE HEAL the rooted player. DO NOT auto-dispel by default
          (root resolves on duration; dispel-mana-spam wastes resources).
          REACTIVE DISPEL only if player is rooted in a fatal position
          (e.g., outside Shadow Gale safe zone with cast incoming).
    - faceless_corruptor_spawn:
        warning: post-Shadow-Gale spawn at room entrance
        damage: |
          Corruptor not directly threatening healer; but if egg corruption
          completes, Twilight Hatchlings spawn = group AoE damage.
        response: |
          DISPEL Shield of Nightmares (75809) on Corruptor (Magic) — high
          priority. Removing the buff lets DPS burst safely.
    - twilight_hatchling_blast:
        warning: only if egg corruption completed
        damage: AoE shadow damage to group
        response: reactive AoE heal; pre-shield if hatchlings in range

  cooldown_use:
    shadow_gale: align AoE cooldown to channel ticks
    feeble_body: tank-CD-ally if available (Pain Suppression / Spirit Link / Guardian Spirit) on knockback
    corruptor_burst: dispel + assist via DPS-buff (Power Infusion / Tide of Light) where relevant
    mana_tide_totem: deploy ~50% boss HP (mana check window)
    innervate: self-cast at ~30% mana (Resto Druid)
    hymn_of_hope: combat-mana-tide (Holy Priest) ~40% mana
    spirit_link_totem: deploy on HP-distribution chaos (Shadow Gale clip + Corruptor cleave overlap)
    tranquility_or_divine_hymn: emergency on heroic if Hatchlings spawned + Shadow Gale clip

  dispel_priority:
    - { spell_id: 75809, name: Shield of Nightmares, target: faceless_corruptor, dispel_type: magic, action: AUTO_DISPEL_HIGH_PRIORITY }
    - { spell_id: 79466, name: Binding Shadows, target: ally_player, dispel_type: magic, action: REACTIVE_ONLY, notes: do NOT auto-dispel; let root resolve unless cripples positioning }
    notes: |
      The Shield of Nightmares dispel is the FIGHT'S PRIMARY healer dispel
      job. Without it, DPS reflect damage burns the group when attacking
      Corruptor. AUTO-DISPEL on detection of buff aura on Corruptor.

      Binding Shadows is the OPPOSITE — auto-dispel is an ANTI-PATTERN
      because root self-resolves and dispel-mana-spam during a hard DPS-
      check fight burns the healer's mana pool.

  interrupt_role:
    umbral_mending_75763: MUST_INTERRUPT — if class has kick (highest priority; corruptor heal cast)
    twilight_corruption_75520: ATTEMPT — if channel is interruptible (UNVERIFIED)
    binding_shadows_79466: SECONDARY — positioning is canonical counter

  heroic_delta: |
    - 2 Faceless Corruptors per Shadow Gale (vs 1 normal) — DPS-check tighter; healer dispel doubles
    - Shadow Gale damage higher; late entry to safe zone fatal
    - Enfeebling Blow knockback farther / more frequent
    - More mana drain overall — DRINK BEFORE PULL is mandatory

  bot_anti_patterns:
    - DO NOT step OUT of Shadow Gale safe zone (REVERSAL of normal "fire = avoid"; safe zone IS center)
    - DO NOT auto-dispel Binding Shadows (root resolves; mana-blow)
    - DO NOT skip dispelling Shield of Nightmares (DPS reflect damage)
    - DO NOT chase Corruptor (DPS handles; healer stays at center healing range)
    - DO NOT pre-cast through Shadow Gale telegraph WHILE moving (move first, then cast)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  umbriss:
    - encounter_state_active → maintain_tank_above_90pct (Bleeding Wound auto-removal — UNVERIFIED)
    - encounter_detect_blitz_telegraph → pre_shield_target_player_if_available
    - encounter_detect_frenzy_at_30 → pop_tank_ally_cd

  throngus:
    - encounter_state_stance_sword → heaviest_tank_healing_efficient_heals
    - encounter_state_stance_mace → reactive_only_jump_target_focus
    - encounter_state_stance_shield → mana_recovery_window
    - encounter_detect_cave_in_patch_dropped → reposition_self_if_in_patch
    - encounter_detect_disorienting_roar_landed → continue_normal_cast_pattern (do NOT auto-dispel)

  drahga:
    - encounter_state_phase_1 → standard_reactive_low_intensity
    - encounter_detect_invoked_flaming_spirit_spawn → pre_shield_fixated_player
    - encounter_state_phase_2 → high_intensity_pre_position_for_devouring_flames
    - encounter_detect_devouring_flames_cast → move_with_group_behind_valiona
    - encounter_state_phase_3 → standard_reactive_recovery

  erudax:
    - encounter_detect_shadow_gale_telegraph → pre_cast_aoe_heal_cd_in_safe_zone
    - encounter_detect_self_outside_safe_zone → MOVE_TO_CENTER_BEFORE_CAST
    - encounter_detect_enfeebling_blow_landed → tank_priority_heal_with_pre_shield
    - encounter_detect_faceless_corruptor_spawn → enable_dispel_shield_of_nightmares
    - encounter_detect_shield_of_nightmares_aura → AUTO_DISPEL_HIGH_PRIORITY
    - encounter_detect_binding_shadows_on_self → reactive_self_heal_no_dispel
    - encounter_detect_binding_shadows_on_ally → reactive_heal_only_NO_AUTO_DISPEL

dispel_blacklist:
  - { spell_id: 79466, name: Binding Shadows, scope: erudax_fight, force: false, notes: REACTIVE_ONLY behavior — see ai_hooks above }

dispel_active:
  - { spell_id: 75809, name: Shield of Nightmares, scope: erudax_fight_corruptor_target, action: AUTO_DISPEL }

mana_cd_save_priority:
  umbriss:    [no_save]
  throngus:   [Mana Tide / Hymn / Innervate during Sword stance ~20s+]
  drahga:     [Mana Tide at Phase 2 entry (Drahga HP 25%)]
  erudax:     [Mana Tide ~50% boss HP; Spirit Link on overlap chaos; emergency raid heal on heroic Hatchling spawn]
```
