# Hour of Twilight — Healer

**Healer-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the healer reacts. Class-agnostic where possible.

**Layering**:
- Cross-content healer patterns (triage hierarchy, dispel taxonomy, mana pacing, anti-patterns) live in `docs/roles/healer.md`
- Per-spec rotation / CD / mana model lives in the active healer's `docs/specs/` doc (resto-shaman, holy-paladin, holy-priest, disc-priest, resto-druid)
- This doc layers Hour of Twilight–specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_healer_overview:
  total_bosses: 3
  estimated_full_clear_minutes: 15-25 (heroic-only, short instance)
  difficulty_mode: heroic_only
  ilvl_gate: 353
  mana_profile:
    arcurion: moderate                       # tank Hand-of-Frost spikes + Torrent execute group AoE
    asira: low                                # clean positioning = minimal damage
    benedictus: HIGH                          # dispel-every-shear cycle + waves + transform spike
  drink_windows:
    - after Arcurion
    - after Asira                             # MOST IMPORTANT — go into Benedictus full mana
  key_responsibilities:
    - dispel_every_shear_on_benedictus        # CRITICAL — load-bearing healer pattern
    - mana_pacing_for_benedictus_p2           # no Thrall support after 60% — heal-throughput on group
    - tank_burst_on_arcurion_torrent          # 30%-HP execute is constant group AoE
    - thrall_hp_monitoring                    # fail-condition flag (not directly healable)
    - rotation_anti_patterns                  # do NOT auto-dispel Mark of Silence (physical) or Chains of Frost (waste)
```

---

## Trash — healer notes

```yaml
trash_healer:
  thrall_escort:
    notes: |
      Thrall walks/runs with party between encounters. Bot follow-tank
      behavior incidentally tracks Thrall most of the time. Healer is NOT
      responsible for healing Thrall directly (he's hotfix-immune to
      trash damage). Bot does monitor his HP as a fail-condition flag.

  twilight_humanoid_packs:
    dispels: []                                # no significant dispel calls on standard trash
    cooldown_use: none required; reactive only

  faceless_voidstalker_packs:
    notes: |
      Shadow Bolt hardcasters (interrupt role for DPS); if interrupts lag,
      group AoE damage rises. Healer reactive AoE heal as needed.

  corrupted_slime_packs:
    notes: ground acid patches; no healer concern, but pre-shield melee who must walk through

  twilight_assassin_pulls:
    notes: |
      Stealth-engage humanoids. Tank may be slow to taunt off ranged
      first-engagement — healer reactive heal on whoever gets jumped.
```

---

## Boss 1 — Arcurion

```yaml
boss_healer:
  boss: arcurion
  difficulty_grade: medium
  mana_intensity: moderate

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 60   # raised — Hand of Frost spike adds to ramp risk
    tier_3_thrall_critical: thrall.hp_pct < 50 # fail-condition flag; alert party (not healable directly in most strats)

  predictable_spike_events:
    - hand_of_frost_on_tank:
        warning: 2s cast bar
        damage: ~45-55k Frost on tank
        response: |
          Pre-cast tank heal during the 2s cast window:
          - Resto Shaman: Riptide pre-applied; Greater Healing Wave during cast
          - Holy Paladin: Beacon on tank; Holy Light during cast
          - Holy Priest: Renew on tank; Greater Heal during cast
          - Disc Priest: PW:S pre-cast; Penance during; Atonement-Smite if tank stable
          - Resto Druid: Lifebloom + Rejuv on tank; Healing Touch during cast
        anti_pattern: |
          DO NOT panic-flash if interrupt is rolling — kick rotation
          should land most casts. Conserve mana.

    - icy_tomb_thrall_event:
        warning: scripted (visible animation on Thrall)
        damage: |
          NOT a healing event for the party. Bot DPS must switch target
          to the tomb to free Thrall. Healer continues tank focus.
        response: |
          Continue normal tank healing. If tank takes Hand of Frost during
          tomb event, prioritize tank heal over reacting to Thrall (DPS
          handles the tomb).

    - torrent_of_frost_30pct:
        cadence: persistent_channel from 30% → 0%
        damage: ~13-16k Frost / sec to all party + 50% slow
        response: |
          AoE heal cooldown rotation. Pre-mana before 30% HP.
          - Resto Shaman: Healing Stream Totem + Healing Rain pre-drop;
            Mana Tide Totem at boss 30% HP entry
          - Holy Paladin: Holy Radiance + Light of Dawn rotation; Avenging
            Wrath if rotational
          - Holy Priest: Hymn of Hope at ~40% mana; Circle of Healing on
            CD; Divine Hymn if group HP collapses
          - Disc Priest: PW:Barrier ground-place; PoM bounces; Atonement-
            Smite OFF (full reactive mode)
          - Resto Druid: Wild Growth on CD; Tranquility if group HP at risk
        notes: |
          Thrall casts Bloodlust at this trigger; healer offensive CDs
          (Holy Pally Avenging Wrath, etc.) align with the Bloodlust
          window if not already used.

  cooldown_use:
    rotational: reactive HoT on tank (Riptide / Beacon / Renew / PW:S / Lifebloom)
    save_for_torrent: |
      Mana Tide / Hymn of Hope / Innervate / Spirit Link Totem deployed at
      30% HP transition. This is the canonical mana-CD save for this fight.
    no_unique_cd_burn_pre_30pct: |
      Don't burn Mana Tide / Innervate before 30% — Torrent of Frost is
      where the mana bleed lives.

  dispel_priority: []                          # no dispels — Chains of Frost is technically Magic but do_not_auto_dispel

  interrupt_role:
    hand_of_frost: PARTICIPATE in interrupt rotation if class has kick (Wind Shear, Rebuke, Mind Freeze N/A for healer specs)

  heroic_delta: |
    N/A — heroic-only fight.

  bot_anti_patterns:
    - DO NOT auto-dispel Chains of Frost (102582) — wasted GCDs on group-wide immobilize
    - DO NOT burn Mana Tide / equivalent before 30% HP entry
    - DO NOT attempt to heal Thrall directly during Icy Tomb (he's not party-healable in most strats)
    - DO NOT chase strays during Boulder telegraphs — let DPS move themselves
```

---

## Boss 2 — Asira Dawnslayer

```yaml
boss_healer:
  boss: asira_dawnslayer
  difficulty_grade: easy
  mana_intensity: low

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50   # standard — minimal pressure with clean positioning

  predictable_spike_events:
    - throw_knife_on_caster:
        warning: marked-target casts → knife fires
        damage: ~10k physical on first player hit (often a body-blocker)
        response: |
          Reactive heal on whoever takes the knife (usually a tank/melee
          body-blocker or the marked caster if no body-block).
    - smoke_bomb_caught_player:
        warning: cloud appears on ground
        damage: 5k Nature / 0.5-1s tick on anyone inside
        response: |
          Should be near-zero with proper positioning (tank drags out).
          If a melee/healer is caught, reactive heal + alert via raid
          markers.
    - blade_barrier_execute:
        cadence: from 30% → 0%
        damage: minimal — Asira's auto-attack rate doesn't ramp
        response: |
          Long fight tail if shield isn't broken quickly. Healer mana
          drain is the risk, not burst. Conservative reactive healing
          throughout.

  cooldown_use:
    rotational: reactive only
    save_for_benedictus: hold Mana Tide / Innervate / Hymn of Hope / Spirit Link Totem for Benedictus phase 2

  dispel_priority: []                          # no dispels — Mark of Silence is physical, Smoke Bomb is environmental

  interrupt_role:
    none: no kick-required casts

  heroic_delta: |
    N/A — heroic-only fight.

  bot_anti_patterns:
    - DO NOT attempt to dispel Mark of Silence (102726) — physical, not dispellable
    - DO NOT cleanse Smoke Bomb (103790) — environmental, not on a player
    - DO NOT cast hardcasts if YOU are the Mark-of-Silence target (knife procs on cast):
        - For Holy Priest / Disc Priest / Holy Paladin / Resto Shaman / Resto Druid: stop hardcasting and use instants/HoTs only for the 20s mark duration
        - Body-block by positioning behind a melee
    - DO NOT burn major mana CDs on this fight — save for Benedictus
```

---

## Boss 3 — Archbishop Benedictus

```yaml
boss_healer:
  boss: archbishop_benedictus
  difficulty_grade: HARD
  mana_intensity: HIGH
  notes: |
    PRIMARY DISPEL FIGHT for the entire dungeon. Righteous Shear (P1) and
    Twilight Shear (P2) demand dispel-every-application discipline —
    higher dispel-priority than any Cata 5-man healer pattern documented
    so far. Phase 2 transition removes Thrall's heal/totem/Water-Shell
    support; healer must throughput-heal the rest of the fight unaided.

  threshold_overrides:
    tier_1_dispel_priority: |
      raised to absolute priority — Shear dispel comes BEFORE tank
      emergency heals (the AoE damage from un-dispelled Shear stacks
      will kill the group faster than the tank loss kills the tank).
    tier_2_tank_emergency: tank.hp_pct < 50
    tier_2b_self_emergency_p2: |
      In P2, healer must also handle waves alone — if you are about to
      eat a Wave of Twilight, gap-run / Iceblock / Divine Shield equivalent
      takes priority over a heal cast.

  predictable_spike_events:
    - smite_p1_on_tank:
        warning: cast bar
        damage: heavy holy nuke on tank
        response: |
          Pre-cast tank heal during cast window if interrupt rotation
          fails to land. Standard tank heal flow.

    - twilight_blast_p2_on_tank:
        warning: cast bar
        damage: heavy shadow nuke on tank
        response: same as Smite — pre-cast heal during cast window

    - righteous_shear_applied_p1:
        warning: instant application; no cast bar
        damage: |
          Stacking debuff; ticks Holy AoE (~7500) to allies within 10y
          of affected player. Stacks compound rapidly.
        response: |
          DISPEL ASAP. Class dispel:
          - Resto Shaman: Cleanse Spirit (Curse only in Cata 4.3.4 —
            DOES NOT WORK on Magic). Resto Shaman has NO Magic dispel.
            **CRITICAL HOLE** — see Disagreement / cross-class note below.
          - Holy Paladin: Cleanse (51886) — dispels Magic + Disease + Poison
          - Holy Priest: Dispel Magic (528) — Magic only on friendlies in 4.3.4
          - Disc Priest: Dispel Magic (528) / Mass Dispel (32375)
          - Resto Druid: Remove Corruption (2782) — dispels Curse + Poison ONLY in 4.3.4 (NOT Magic)
            **CRITICAL HOLE** — Resto Druid has NO Magic dispel either.
        notes: |
          ALERT — Resto Shaman and Resto Druid CANNOT dispel Magic in
          4.3.4. If the bot healer is Shaman or Druid, the dispel role
          falls to a hybrid DPS (Holy Pally Cleanse, Mage Remove Curse?
          — Mage doesn't dispel Magic either). With pure-non-Magic-dispel
          group, the Shear dispel falls to the off-class hybrids. UNVERIFIED
          whether Hour of Twilight has alternate group composition gating.

    - twilight_shear_applied_p2:
        warning: instant application
        damage: stacking; ticks Shadow AoE (~7500) to allies within 10y
        response: same as Righteous Shear — Magic dispel asap

    - purifying_light_orb_p1:
        warning: scripted summon visual
        damage: 80k Holy on impact (lethal if hit)
        response: |
          Thrall destroys 2 of 3. Movement-track party away from path of
          the surviving orb. Healer-bot heal-while-moving (instants only)
          if needed to dodge.

    - corrupting_twilight_orb_p2:
        warning: scripted summon visual
        damage: 80k Shadow on impact
        response: |
          NO THRALL HELP — all 3 orbs must be dodged. Healer dodges +
          heals during movement (HoTs / Riptide / Beacon-of-Light / Renew).

    - wave_of_virtue_p1:
        warning: scripted cast
        damage: ~97-100k Holy + knockback (lethal contact)
        response: |
          STACK INTO THRALL'S WATER SHELL. Healer is also inside. If
          missed, eat the wave + bres after fight. Heal-thru is not
          viable.

    - wave_of_twilight_p2:
        warning: scripted cast
        damage: ~97-100k Shadow + knockback
        response: |
          NO WATER SHELL. Healer survives by:
          - Resto Shaman: Stoneclaw Totem (talented shield) — WEAK
          - Holy Paladin: Divine Shield (immune all damage)
          - Holy Priest: Dispersion N/A (Shadow only); use Iceblock-equivalent? — none.
          - Disc Priest: PW:Barrier ground-place; Pain Suppression on self
          - Resto Druid: Barkskin (-20% damage); Survival Instincts (cat/bear; not in tree form)
          Or gap-run the wave (it has openings). Bot defaults to gap-run
          + emergency CD if hit.

  cooldown_use:
    p1_rotational: reactive healing + dispel priority
    p2_save_major_cds: |
      Mana Tide Totem / Innervate / Hymn of Hope / Spirit Link Totem at
      phase 2 entry — Thrall heal/totem support is gone. This is THE
      mana CD save for the dungeon.
    avenging_wrath_holy_pally: align with phase 2 entry for damage
    tranquility: emergency raid heal if group HP collapses (P2 wave hits)
    lay_on_hands: emergency tank/self heal if Wave hit lands

  dispel_priority:
    P1:
      - { spell_id: 103161, name: Righteous Shear, dispel: magic, priority: ABSOLUTE }
    P2:
      - { spell_id: 103526, name: Twilight Shear, dispel: magic, priority: ABSOLUTE }
    notes: |
      These are the load-bearing dispels of the entire dungeon. Bot
      healer evaluates dispel queue every GCD; Shear dispel pre-empts
      almost all other healing decisions short of tank-imminent-death.

  interrupt_role:
    smite_p1: PARTICIPATE if class has kick (rare for healers)
    twilight_blast_p2: PARTICIPATE if class has kick

  heroic_delta: |
    N/A — heroic-only fight.

  bot_anti_patterns:
    - DO NOT skip Shear dispels — un-dispelled stacks kill the group
    - DO NOT spam-dispel a single application twice (debuff is removed on first dispel)
    - DO NOT continue healing rotation through Wave of Twilight (P2) — gap-run first
    - DO NOT expect Thrall support in P2 — bot must detect Twilight Epiphany and adjust expectations
    - DO NOT overheal the marked Shear target before dispelling — dispel removes the AoE source faster than heal-throughput
    - For Resto Shaman / Resto Druid: KNOWN LIMITATION — bot cannot dispel Shears. Group composition must include a Magic-dispeller (Holy Pally Cleanse, Disc/Holy Priest Dispel Magic / Mass Dispel)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  instance_wide:
    - encounter_state_thrall_alive(true)        → continue normally
    - encounter_detect_thrall_hp_low             → fail-condition alert (no direct heal action)

  arcurion:
    - encounter_detect_hand_of_frost_cast        → pre_cast_tank_heal
    - encounter_state_boss_hp_pct <= 30          → torrent_execute_phase
    - encounter_detect_thrall_bloodlust          → align_offensive_cd_if_holy_pally
    - encounter_state_torrent_active             → aoe_heal_cd_rotation; reactive_aoe_heal

  asira_dawnslayer:
    - encounter_detect_mark_of_silence(self)     → stop_hardcasting; instants/HoTs only for 20s
    - encounter_detect_smoke_bomb_caught(ally)   → reactive_heal_caught_target
    - encounter_state_blade_barrier_active       → conservative_mana_use; long_fight_pacing
    - encounter_state_active                     → save_major_cd_for_benedictus

  archbishop_benedictus:
    - encounter_detect_righteous_shear_applied(target) → DISPEL_ASAP (P1)
    - encounter_detect_twilight_shear_applied(target)  → DISPEL_ASAP (P2)
    - encounter_detect_smite_cast                → pre_cast_tank_heal
    - encounter_detect_twilight_blast_cast       → pre_cast_tank_heal
    - encounter_detect_purifying_light_orb_p1    → movement_track_away_from_orb_path
    - encounter_detect_corrupting_twilight_orb_p2 → dodge_all_3_orbs
    - encounter_detect_wave_of_virtue_p1         → stack_into_water_shell
    - encounter_detect_wave_of_twilight_p2       → gap_run_OR_immunity_cd
    - encounter_state_boss_hp_pct == 60          → twilight_epiphany_imminent; pre_mana_cd
    - encounter_detect_twilight_epiphany_cast    → phase_2_transition_detect; deploy_major_cd
    - encounter_detect_thrall_twilight_prison    → discontinue_thrall_support_expectations

dispel_blacklist: []

mana_cd_save_priority:
  arcurion:    [Mana Tide / Hymn of Hope / Innervate at 30% HP entry (Torrent execute)]
  asira:       [no_save]
  benedictus:  [primary CD at phase 2 entry (60% HP); secondary CD if needed in execute]

class_specific_dispel_capability_p3_alert:
  resto_shaman: NO_MAGIC_DISPEL                 # Cleanse Spirit is curse/poison only in 4.3.4
  resto_druid:  NO_MAGIC_DISPEL                 # Remove Corruption is curse/poison only in 4.3.4
  holy_paladin: YES (Cleanse 4987/51886)
  holy_priest:  YES (Dispel Magic 528, Mass Dispel 32375)
  disc_priest:  YES (same as Holy)
  notes: |
    If healer bot is Resto Shaman or Resto Druid, the Shear-dispel role
    must fall to a hybrid DPS (Holy Pally if in DPS slot — but Holy Pally
    isn't a DPS spec in the project's spec roster; Ret Pally Cleanse
    51886 is Magic-dispel for friendlies; Disc/Holy Priest off-spec
    Mass Dispel; Mage Remove Curse is curse-only — does NOT dispel Magic).
    **In a pure Resto-Shaman/Druid healer + non-Pally/Priest DPS group,
    Shear dispels are unavailable.** This is a group-composition
    limitation rather than a bot bug.
```
