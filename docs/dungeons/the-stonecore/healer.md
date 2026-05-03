# The Stonecore — Healer

**Healer-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the healer reacts. Class-agnostic where possible.

**Layering**:
- Cross-content healer patterns (triage hierarchy, dispel taxonomy, mana pacing, anti-patterns) live in `docs/roles/healer.md`
- Per-spec rotation / CD / mana model lives in the active healer's `docs/specs/` doc (resto-shaman, holy-paladin, holy-priest, disc-priest, resto-druid)
- This doc layers Stonecore-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_healer_overview:
  total_bosses: 4 (none optional)
  estimated_full_clear_minutes: 25-45
  mana_profile:
    millhouse_event: high                   # 3-5 mob trash pull is harder than first 2 bosses (per Wiki)
    corborus: moderate                      # Dampening Wave dispel cycle + tank Rock Bore stacks
    slabhide: moderate                      # ground-fire spread damage; heroic Crystal Storm LoS-break healing window
    ozruk: high (heroic)                    # Paralyze damage-break HoT timing; Shatter resolution
    azil: high                              # Force Grip slam damage if kick missed + Curse dispel cycle + Stage 2 transitions
  drink_windows:
    - after Millhouse event
    - after Corborus
    - after Slabhide
    - after Ozruk                          # MOST IMPORTANT — go into Azil full mana
  key_responsibilities:
    - high_priority_dispel_per_fight        # Magic on Corborus, Curse on Azil — two distinct dispels in this dungeon
    - paralyze_damage_break_aware (ozruk)   # do NOT auto-dispel; let DoTs handle
    - mana_pacing_for_azil_force_grip
    - cooldown_pacing                       # Mana Tide / Spirit Link / Hymn / Innervate placement
    - anti_pattern_awareness               # NEVER block DoT damage-break on Ozruk Paralyze
```

---

## Trash — healer notes

```yaml
trash_healer:
  millhouse_event:
    notes: |
      Hardest pull of the dungeon. 3-5 mobs + Millhouse. Healer pre-HoTs
      tank; spot-heals fear/charge/Spinning-Slash victims; mana-cycle
      after kill (drink before Corborus pull).
    dispels:
      - none required on standard trash
    cooldown_use: |
      Major CD (Mana Tide / Hymn / Innervate / etc.) NOT needed if group
      handles CC properly. Save for boss fights.

  earthshaper_packs:
    interrupt_dependency: |
      Earthshaper Force of Earth (81459) is a 5-SECOND CAST. If healer
      sees the cast bar AND the group has not interrupted by 4s, healer
      may need to dispel the cast (UNVERIFIED whether dispellable; cast
      type is Nature). Otherwise rely on melee/ranged kick rotation.
    dispels: []
    notes: |
      If Force of Earth fires (transformation), group damage spikes
      hard. Pop defensive CDs immediately; tank may need Pain
      Suppression / Guardian Spirit equivalents.

  berserker_spinning_slash:
    notes: |
      4s channel; players within 5y take stacking bleed damage. Healer
      does NOT need to dispel (bleed = not dispellable). Reactive heal
      players who tank a tick or two before kiting out.

  magmalord_packs:
    notes: |
      Ignite + Magma Eruption ground patches. Pre-HoT random fire-DoT
      victims. Dispel only if Ignite is Magic-school (UNVERIFIED) AND
      the group cannot interrupt the cast.
```

---

## Boss 1 — Corborus

```yaml
boss_healer:
  boss: corborus
  difficulty_grade: medium
  mana_intensity: moderate
  notes: |
    THE primary dispel encounter. Dampening Wave (82415) is Magic-
    school heal-absorb (15k heroic). Healer must dispel off TANK FIRST
    every cast, then group as bandwidth allows.

  threshold_overrides:
    tier_1_dispel_dampening_wave: |
      ABSOLUTE PRIORITY. As soon as Dampening Wave (82415) is detected
      on the tank, healer dispel-step is mandatory and overrides
      reactive healing for the GCD it consumes. Untreated heal-absorb
      = healer cannot top tank → Rock Bore stack ramp kills tank.
    tier_2_tank_emergency: tank.hp_pct < 50  # raised — Rock Bore stacks compound

  predictable_spike_events:
    - dampening_wave_cycle:
        warning: cast detected on boss target
        damage: 10k shadow PBAoE + 15k heal-absorb on heroic
        response: |
          Pre-cast dispel: Cleanse / Dispel Magic / Purify / Cleanse
          Spirit (talented). Dispel TANK FIRST, then group.
    - crystal_barrage_target:
        warning: channel on random player; ticks every 0.5s for 4s
        damage: ~10k physical/tick at impact ground; targeted player AND splash
        response: |
          HoT the targeted player so they're rolling damage during the
          4s. Topping during the channel works if a HoT is already
          ticking (Riptide, Renew, Lifebloom, Beacon target).
    - rock_bore_stack_ramp_on_tank:
        warning: tank in Burrow phase taking Rock Borer melee + Rock Bore bleed
        damage: 4k/3s per stack (heroic), up to 10 stacks
        response: |
          AoE-style heals (Healing Rain / Holy Radiance / Wild Growth /
          PoH) hit tank+Rock Borers. Single-target tank if AoE not
          available.

  cooldown_use:
    standard: reactive HoT on tank (Riptide / Beacon / Renew / PW:S / Lifebloom)
    dispel_priority_dispel_macro: |
      Dispel Dampening Wave is the single most important dispel of the
      dungeon. NEVER let a Dampening Wave cast resolve without dispel
      attempt within 1.5s of detection.
    aoe_window: align AoE heal CD to Crystal Barrage spread damage moments
    save_for_real_boss: |
      Save Mana Tide / Innervate / Hymn for OZRUK or AZIL (the higher-
      mana fights). Corborus Dampening Wave dispel is not Mana-Tide-
      worthy.

  dispel_priority:
    - { spell_id: 82415, name: Dampening Wave, type: magic, target: tank_then_group, urgency: HIGHEST }

  interrupt_role:
    none: no kick-required casts (Burrow / Thrashing Charge are scripted, not interruptible)

  heroic_delta: |
    - Dampening Wave heal-absorb is 15k — dispel mandatory every cast
    - Crystal Shard adds (heroic) add reactive AoE-heal pressure if a
      shard reaches a player
    - Rock Bore bleed deals 4k/3s — Rock Borers must die fast or tank
      takes serious bleed damage

  bot_anti_patterns:
    - DO NOT skip the Dampening Wave dispel for the sake of a heal cast — heal-absorb makes the heal ineffective anyway
    - DO NOT dispel Rock Bore (NOT dispellable; it's a bleed)
    - DO NOT auto-cleanse the heal-absorb buff with a Holy Power healer macro — direct dispel only
```

---

## Boss 2 — Slabhide

```yaml
boss_healer:
  boss: slabhide
  difficulty_grade: medium (heroic; easy normal)
  mana_intensity: moderate

  threshold_overrides:
    tier_3_lava_fissure_victim: |
      Player who fails to move out of a Lava Fissure crack takes ~24k
      fire damage on eruption. Reactive HoT-roll the slow-mover; on
      heroic the lava pool persists 30s and can stack damage.

  predictable_spike_events:
    - sand_blast_cone:
        damage: ~47k nature damage to anyone in 60° forward arc
        response: |
          If a non-tank ate the cone (positioning fail), big single-
          target heal: Greater Healing Wave / Holy Light / Healing Touch
          / Greater Heal / etc.
    - lava_fissure_eruption:
        warning: 5s normal / 3s heroic ground crack telegraph on player
        damage: ~24k fire damage on direct hit + lava pool DoT if not moved
        response: |
          Pre-HoT the player whose feet have a fissure (visible in
          encounter state). Reactive top-up if they ate the eruption.
    - crystal_storm_channel_HEROIC:
        warning: 2.5s cast after air phase ends, then 6s channel
        damage: ticks every 0.1s in 8y radius — wipe-tier damage to anyone in LoS
        response: |
          HEROIC ONLY. The 6s channel is a HEALING DEAD ZONE — healer
          is also LoS-broken from group. PRE-CAST (before LoS break):
          - Resto Shaman: Healing Stream Totem + Healing Rain pre-drop
            on the post-channel reformation point
          - Holy Paladin: Holy Radiance pre-stack; Beacon target for
            tank uptime
          - Holy Priest: Prayer of Healing pre-cast; Renew on tank
          - Disc Priest: Power Word: Shield + Power Word: Barrier on
            the LoS-break point
          - Resto Druid: HoT-roll the entire party (Wild Growth +
            Rejuv); Lifebloom on tank
          The 6s channel is survived by HoT/shield uptime, NOT active
          casting.
    - stalactite_direct_hit:
        warning: ground texture telegraph during air phase
        damage: heavy physical + knockback; on heroic can one-shot
        response: top-up immediately; pre-shield knockback victims if predictable

  cooldown_use:
    crystal_storm_pre_channel_HEROIC: |
      Pop AoE-heal CD just BEFORE the 6s channel begins:
      - Resto Shaman: Spirit Link Totem on heroic if HP variance > 30%
      - Holy Pally: Aura Mastery + Devotion Aura
      - Holy Priest: Divine Hymn (channeled — risky if you're LoS-
        broken yourself; align before the LoS break)
      - Disc Priest: Power Word: Barrier on the regroup point
      - Resto Druid: Tranquility (channeled) — same LoS-break risk;
        pre-cast Wild Growth instead if Tranq is risky
    save_for_azil: still hold Mana Tide / Hymn for Azil if normal mode

  dispel_priority: []                       # no dispels required on Slabhide

  interrupt_role:
    none: no kick-required casts (all instant or environmental)

  heroic_delta: |
    - Crystal Storm 6s channel = LoS-break healing dead zone
    - Lava Fissure 3s telegraph (vs 5s) — less reaction time
    - Lava Pool 30s persistence — ground hazard accumulates
    - Stalactite direct hit potentially one-shot

  bot_anti_patterns:
    - DO NOT chase strays into stalactite ground textures
    - DO NOT cast through stalactite LoS — pillar blocks heals
    - DO NOT skip pre-channel HoT/shield setup on heroic Crystal Storm (group dies in LoS-break window)
```

---

## Boss 3 — Ozruk

```yaml
boss_healer:
  boss: ozruk
  difficulty_grade: hard (heroic); easy (normal)
  mana_intensity: high (heroic)
  notes: |
    THE Cata-launch heroic skill check. Healer's primary roles are:
    (1) keep tank alive through Spike Shield bleed ramp (heroic),
    (2) ensure HoT is active on group members so Paralyze breaks on
        first tick (heroic),
    (3) reactive heal Shatter resolution.

    DO NOT auto-dispel Paralyze by default — let DoTs/HoTs handle the
    break. Dispel is a fallback if no damage source is breaking it.

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 60  # raised — Spike Shield bleed + Ground Slam direct hit if positioning fails
    paralyze_break_target_threshold: |
      If a player is paralyzed (92426) AND HoT is NOT ticking on them
      AND duration_remaining_ms < 4000, healer dispels (Magic
      dispel-type). Otherwise let HoT/DoT damage break it.

  predictable_spike_events:
    - ground_slam_resolution:
        warning: 3s cast (visible cast bar)
        damage: ~94k physical to anyone in 4y front arc — lethal direct hit
        response: |
          Reactive heal anyone who ate the slam. If the tank ate it
          (failed to run through), big emergency single-target heal +
          defensive CD if available.
    - spike_shield_bleed_ramp_on_tank_HEROIC:
        warning: Spike Shield aura on boss + tank attacking
        damage: 600/sec per stack on heroic; 5 stacks max; 3000/sec at peak
        response: |
          Tank-focus heals during the 10s window. HoT-roll: Earth
          Shield + Riptide (Resto Shaman); Beacon + Word of Glory (Holy
          Pally); Renew + PoM (Priest); Lifebloom + Rejuv (Druid).
    - shatter_resolution:
        warning: Spike Shield expiring (≈ 9s mark)
        damage: ~84k+ frost damage in 5y radius
        response: |
          AoE heal CD aligned to the 9s mark. Anyone who failed to
          step out takes the hit; reactive top-up. If multiple players
          ate it, emergency raid CD: Spirit Link / Tranquility / Divine
          Hymn / Power Word: Barrier.
    - paralyze_application_HEROIC:
        warning: Shatter cast resolves WITH Paralyze on heroic
        damage: 8s stun; 47k arcane at expire if not broken
        response: |
          Verify HoTs are ticking on group members (pre-cast Wild
          Growth / Holy Radiance / Renew / etc. just before Shatter).
          The HoT tick during the stun breaks Paralyze on the first
          tick. If a player has no HoT AND no DoT-source, healer
          dispels Magic.

  cooldown_use:
    pre_shatter_heal_cd: align AoE heal cooldown to Spike Shield 9s mark (Shatter is 1s away)
    spirit_link_totem: deploy if HP variance > 30% (Resto Shaman)
    tranquility: emergency raid heal if multiple players ate Shatter (Resto Druid)
    pain_suppression: tank Spike Shield bleed peak (Disc Priest if specced)
    aura_mastery: Devotion Aura for raid AoE mitigation (Holy Pally)
    mana_tide_totem: ~50% boss HP — heroic mana check (Resto Shaman)
    innervate: self-cast at ~30% mana (Resto Druid)
    hymn_of_hope: ~40% mana (Holy Priest)

  dispel_priority:
    - { spell_id: 92426, name: Paralyze, type: magic, target: any_paralyzed_ally, urgency: BACKUP_ONLY }

  interrupt_role:
    none: Ground Slam is NOT interruptible

  heroic_delta: |
    - Paralyze (8s stun) added to Shatter cycle — DoT-break primary,
      dispel backup
    - Spike Shield bleed 600/sec (vs 300/sec) — tank healing pressure
    - Bulwark 100% reflect — caster damage drops; healer also drops
      offensive support (no Atonement Smite on Disc, no Telluric Currents
      on Resto Shaman if Bulwark is up)
    - Mana check intensifies — Mana Tide / Innervate are necessary

  bot_anti_patterns:
    - DO NOT auto-dispel Paralyze on first detection — let HoT ticks handle it (let damage-break play out 1-2s)
    - DO NOT cast offensive spells when Bulwark is up (heal output continues; offensive DPS-support tier is paused)
    - DO NOT panic-flash-heal if the tank is at 60% during Spike Shield (steady HoT-roll is more efficient)
    - DO NOT stand in 4y front arc during Ground Slam (you are NOT exempt — healer runs through too)
    - DO NOT skip the pre-Shatter AoE heal CD — naked Shatter resolution is a wipe condition
```

---

## Boss 4 — High Priestess Azil

```yaml
boss_healer:
  boss: high_priestess_azil
  difficulty_grade: hard
  mana_intensity: high
  notes: |
    THE second-most-mana-intensive fight. Force Grip slam damage if
    kick missed is brutal; Curse of Blood dispel cycle is constant;
    Stage 2 Energy Shield knockback + Seismic Shard tombstone misses
    add reactive-heal pressure.

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50  # Force Grip slam damage if kick missed
    tier_2_curse_of_blood_target: |
      Curse of Blood (16098) on a non-tank target stacks with Devout
      Follower melee for serious damage. Dispel within 2s of detection.

  predictable_spike_events:
    - force_grip_channel_unkicked:
        warning: Force Grip cast on tank; kick rotation should fire within 1.5s of cast start
        damage: 70k+ slam per tick if not interrupted; 5s channel = 280k+ tank damage
        response: |
          PRE-SHIELD/PRE-HOT THE TANK ON CAST DETECTION. If the kick
          rotation lands, channel ends early — saved damage. If it
          DOESN'T land, healer reactive heal + tank pops defensive CD.
          On heroic, this is the highest non-Shatter spike.
    - curse_of_blood_application:
        warning: 2s cast on random ally
        damage: not direct damage; +5000 phys damage taken
        response: DISPEL AS CURSE within 2s of application
    - gravity_well_pull:
        warning: 10y radius ground-target on player
        damage: ramping shadow damage as player approaches center
        response: |
          Trust the player to push out (movement is their responsibility);
          reactive HoT if they're slow.
    - stage_2_transition_knockback:
        warning: Energy Shield cast (2s) at altar
        damage: ~47k arcane + knockback to anyone within 5y
        response: |
          PRE-SHIELD anyone too close; reactive heal post-knockback. If
          tank stayed in melee on her too long, big single-target heal.
    - seismic_shard_direct_hit:
        warning: ground-target tombstone telegraph
        damage: ~942k-1057k physical (heroic) — wipe-tier on direct hit
        response: |
          PRE-SHIELD anyone slow to dodge (rare; should be 100% dodgeable).
          Direct hit usually kills outright; battle-rez the victim if
          available.

  cooldown_use:
    mana_tide_totem: ~50% boss HP if not used on Ozruk
    spirit_link_totem: deploy on Stage 2 transition (group HP variance from knockback + Seismic Shard misses)
    innervate: self-cast at ~30% mana
    hymn_of_hope: combat-mana-recovery at ~40% mana
    aura_mastery_devotion: Stage 2 raid-mitigation moment (Holy Pally)
    pain_suppression: tank Force Grip if kick missed (Disc Priest)
    tranquility: Stage 2 if multiple players took knockback + Seismic Shard
    barrier: Stage 2 regroup point (Disc Priest)
    avenging_wrath: rotational on Holy Pally

  dispel_priority:
    - { spell_id: 16098, name: Curse of Blood, type: curse, target: any_cursed_ally, urgency: HIGH }
    notes: |
      Curse-school dispel — only Resto Druid (Remove Corruption when
      talented), Mage (Remove Curse), Resto Shaman (Cleanse Spirit
      talented), and Boomkin (Remove Corruption) can cleanse Curse.
      Holy Pally / Holy Priest / Disc Priest CANNOT remove Curse —
      relies on a curse-cleansing party member or ranged DPS hybrid.

  interrupt_role:
    force_grip: PARTICIPATE in interrupt rotation if class has kick (Resto Shaman Wind Shear, Holy Pally Rebuke)
    notes: |
      Healer interrupt-rotation participation IS load-bearing on Azil.
      A 5-man interrupt rotation typically includes the healer (Wind
      Shear / Rebuke) because Force Grip cycles fast (~6s) and 3 DPS
      kicks may not cover it.

  heroic_delta: |
    - Gravity Wells shrink only by killing units — strategic play
    - More Devout Followers per wave — AoE-heal pressure (cleave)
    - Seismic Shard direct hit wipe-tier
    - Curse of Blood applied more frequently — dispel cooldown
      management
    - Force Grip more punishing if kick missed

  bot_anti_patterns:
    - DO NOT skip Curse of Blood dispel for a heal cast (heal is wasted on the +5000 damage taken)
    - DO NOT cast through a Gravity Well's pull (you'll be dragged in — break and reposition)
    - DO NOT skip the interrupt rotation contribution if class has a kick on this fight
    - DO NOT cluster with melee in Stage 2 (Seismic Shard cross-hit risk)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  corborus:
    - encounter_detect_dampening_wave_cast → dispel_tank_priority (highest dispel of dungeon)
    - encounter_detect_crystal_barrage_target(target) → apply_hot_tier_3
    - encounter_state_burrow_phase → standard_aoe_heal_on_rock_borer_cleave

  slabhide:
    - encounter_detect_lava_fissure_target(target) → pre_hot
    - encounter_detect_air_phase → maintain_los_through_stalactites
    - encounter_detect_crystal_storm_cast_HEROIC → pre_cast_aoe_heal_cd_align_to_los_break
    - encounter_detect_crystal_storm_channel_HEROIC → conserve_mana (group survives on pre-applied HoTs/shields)

  ozruk:
    - encounter_detect_spike_shield → tank_focus_hot_roll
    - encounter_detect_shatter_imminent (spike_shield 9s) → pre_cast_aoe_heal_cd
    - encounter_detect_paralyze_applied(target) → check_hot_active(target); if NO active hot AND duration < 4s → dispel_magic
    - encounter_detect_bulwark → suspend_offensive_casts (healer doesn't damage during Bulwark)
    - encounter_state_active_HEROIC → always_keep_hot_on_party (Paralyze break preparation)

  azil:
    - encounter_detect_force_grip_cast → kick_rotation_participate (if class has kick) AND pre_hot_tank
    - encounter_detect_curse_of_blood_application(target) → dispel_curse (HIGH priority)
    - encounter_detect_gravity_well_target(target) → no_action (movement is player responsibility)
    - encounter_detect_stage_2_transition → pre_shield_within_5y_players + spirit_link_totem
    - encounter_detect_seismic_shard_cast → no_action (movement is player responsibility); reactive heal on direct hit

dispel_priority_per_fight:
  corborus:    [{ spell_id: 82415, name: Dampening Wave, type: magic, urgency: HIGHEST }]
  slabhide:    []
  ozruk:       [{ spell_id: 92426, name: Paralyze, type: magic, urgency: BACKUP_ONLY, prefer: dot_break }]
  azil:        [{ spell_id: 16098, name: Curse of Blood, type: curse, urgency: HIGH }]

mana_cd_save_priority:
  millhouse_event: [no_save]
  corborus:        [no_save]
  slabhide:        [no_save]
  ozruk:           [Mana Tide / Innervate / Hymn of Hope at ~50% boss HP]
  azil:            [secondary CD if not used on Ozruk; Spirit Link Totem on Stage 2 transition]

interrupt_role_per_fight:
  corborus:    [no_kick]
  slabhide:    [no_kick]
  ozruk:       [no_kick]
  azil:        [participate_in_force_grip_kick_rotation_if_class_has_kick]
```
