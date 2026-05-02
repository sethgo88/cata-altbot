# Throne of the Tides — Melee DPS

**Melee-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how a melee DPS reacts. Class-agnostic — when per-class spec docs exist (rogue, fury/arms warrior, ret paladin, frost/unholy DK, enhancement shaman, feral cat druid), they can reference this doc and add class-specific bindings.

**Layering**:
- Cross-content melee-DPS patterns (rear-arc discipline, gap-closer pacing, interrupt as primary duty, cleave thresholds, anti-patterns) live in `docs/roles/melee-dps.md`
- Per-spec rotation / CD / resource model lives in `docs/specs/{spec}.md` (TBD per class)
- This doc layers ToTT-specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_melee_overview:
  total_bosses: 4
  estimated_full_clear_minutes: 20-30
  primary_responsibilities:
    - interrupt_rotation     # PRIMARY interrupter on Shock Blast (Naz'jar) and Lava Bolt (Erunak)
    - target_switching       # Enslaved player burn on Ghur'sha P2
    - mobility               # Ulthok kite, Geyser dodging, fissure avoidance
    - cleave_aoe             # Naz'jar add phases, Ozumat P1 murloc waves
  positioning_constraints:
    - behind_boss             # Magma Splash cone (Erunak), Arc Slash cone (Honor Guard)
    - out_of_telegraph        # Geyser, Earth Shards, Dark Fissure, Blight pools
    - mobile_with_tank        # Ulthok kite, Ozumat P2 Blight Beast kite
  interrupt_repertoire:
    # Class interrupts likely available to a melee DPS:
    kick: rogue
    pummel: warrior_(arms_or_fury)
    rebuke: paladin_(retribution)
    mind_freeze: death_knight_(any_spec)
    skull_bash: druid_(feral)
    wind_shear: shaman_(enhancement)   # interrupt only — Resto Shaman has it too
    counterspell: NOT_APPLICABLE_to_melee_specs
```

---

## Trash — melee DPS notes

```yaml
trash_melee:
  naga_gauntlet:
    target_priority:
      - Naz'jar Spiritmender (kill priority; backup interrupt on heals)
      - Naz'jar Sentinel (high HP, melee-vulnerable)
      - Naz'jar Honor Guard (face away; cleave when safe)
      - Naz'jar Invader (low priority — primary melee target since they're melee-only)
    interrupts:
      - Spiritmender Healing Wave (MUST_INTERRUPT, melee primary duty)
      - Tempest Witch Lightning Bolt (MUST_INTERRUPT — secondary if positioned)
    positioning: behind Honor Guard (Arc Slash cone); behind Sentinels (Shellbreaker target priority)

  faceless_wing:
    target_priority:
      - Faceless Seer (interrupt Mind Flay, melee-friendly target)
      - Minion of Ghur'sha (stun-lock priority)
      - Faceless Watcher (high HP DPS race vs tank Crush)
    interrupts:
      - Faceless Seer Mind Flay (MUST_INTERRUPT)
    positioning: avoid Psionic Pulse cone from Minion; standard rear flanking

  corruption_hallway:
    target_priority:
      - Tainted Sentry (interrupt Swell, melee primary duty)
      - Unstable Corruption (note: dies → Wall of Corruption disease applies; healer dispels between pulls)
    interrupts:
      - Tainted Sentry Swell (MUST_INTERRUPT)
    positioning: standard
```

---

## Boss 1 — Lady Naz'jar

```yaml
boss_melee:
  boss: lady_nazjar
  difficulty_grade: moderate

  pre_pull:
    - assign_interrupt_rotation: melee is PRIMARY on Shock Blast; coordinate timing if multiple melee
    - mark_targets: confirm CC target (Tempest Witch) so cleave doesn't break it during add phases

  per_phase:
    phase_1_3_5:
      target: lady_nazjar
      interrupts:
        - Shock Blast (76008) — MUST_INTERRUPT, EVERY cast.
          PRIMARY interrupt duty (melee is closest, most-reliable). On heroic, missed
          Shock Blast = tank dies.
      cooldown_plan: |
        Open with offensive CDs at pull. Sustain through phase transitions.
      positioning: |
        Behind boss (no rear-arc mechanic, but standard melee positioning).
        Watch ground for Geyser (75722) telegraph circles — move out 5s before detonation.
        Stay >=5y from other allies to avoid Fungal Spores (76001) splash.

    waterspout_phases_2_4:
      target_priority:
        - Naz'jar Honor Guard (PRIMARY melee target — tank holds him; high HP melee-vulnerable)
        - Naz'jar Tempest Witch (cleave only on the NON-CC'd one; help focus when Honor Guard dies)
      interrupts:
        - Honor Guard has no interruptible casts (Arc Slash is melee swing, not cast)
        - Tempest Witch Chain Lightning / Lightning Bolt — secondary backup if positioned in range (25y kick range typical)
      cooldown_plan: |
        Burn Honor Guard fast — he hits the tank hardest. Save burst for Honor Guard phase
        if you have a long-CD CD (e.g., Recklessness, Berserk).
      positioning: |
        BEHIND Honor Guard — Arc Slash (75907) is undodgeable/unblockable/unparriable
        frontal cone. Tank should face away from group; you stand on Honor Guard's back.
        Avoid Geyser telegraphs. Heroic: water vortex eddies stun + tick — dodge.

  anti_patterns:
    - do NOT stand in front of Honor Guard — Arc Slash bypasses all defensive avoidance
    - do NOT cleave the CC'd Tempest Witch — breaking CC = unmarked add free-casting
    - do NOT chase the boss during Waterspout — boss is immune

  cooldown_summary:
    offensive_cds:
      open_at_pull: yes
      save_for_honor_guard: defensible — burst window aligns well
    defensive_cds: reactive on Geyser knockback or if you eat Lightning Surge

  reference_mechanics:
    - encounter.md: Shock Blast (76008), Geyser (75722), Fungal Spores (76001),
      Arc Slash (75907), Chain Lightning (103637)
```

---

## Boss 2 — Commander Ulthok

```yaml
boss_melee:
  boss: commander_ulthok
  difficulty_grade: moderate_to_high   # most punishing fight for melee due to mobility
  primary_concern: mobility

  per_phase:
    single_phase:
      target: commander_ulthok (tunnel)
      interrupts: []
      cooldown_plan: |
        Fight is short; open with everything. Save mobility / gap-closer cooldowns
        (Charge, Heroic Leap, Shadowstep, Death Grip pull-self) for fissure repositioning.
      positioning: |
        - BEHIND boss as tank kites along the perimeter (heroic).
        - Move CONTINUOUSLY — Dark Fissure (76047) telegraphs spawn under boss, then
          expand. On heroic, fissures persist ~60s and grow.
        - When Cursed (76094 — Curse of Fatigue), trust the healer to dispel within
          1 GCD. If still slowed when a fissure spawns, use any movement-extending CD
          (Sprint, Wings of Liberty, etc.).

  anti_patterns:
    - do NOT tunnel boss without watching ground — fissure damage is severe and persists
    - do NOT stay in Ulthok's old position after kite — boss has moved, fissure is where you are
    - do NOT use gap-closer to chase tank if it brings you THROUGH a fissure — go around

  cooldown_summary:
    offensive_cds: open at pull; fight is short
    mobility_cds: reserved for fissure-spawn moments
    defensive_cds: reactive on Squeeze (76026) target — use damage-mitigation defensives during the 6s stun

  reference_mechanics:
    - encounter.md: Squeeze (76026), Dark Fissure (76047), Curse of Fatigue (76094)
```

---

## Boss 3 — Mindbender Ghur'sha

```yaml
boss_melee:
  boss: mindbender_ghursha
  difficulty_grade: high

  per_phase:
    phase_1_erunak:
      target: erunak_stonespeaker (until 50%)
      interrupts:
        - Lava Bolt (76171) — MUST_INTERRUPT, PRIMARY melee duty.
          Backup ranged interrupts cover misses, but melee is closest and fastest.
      cooldown_plan: |
        Open with offensive CDs to push Erunak to 50% fast.
      positioning: |
        BEHIND Erunak — Magma Splash (76170) is a 90° frontal cone, 20y, hits ~20k Fire +
        DoT. Tank faces Erunak away from party; you stand on his back.
        Avoid Earth Shards (84931) ground telegraphs (5y radius spike fields).

    phase_2_ghursha:
      target_priority:
        - ENSLAVED PLAYER (when Enslave 76207 lands) — HIGHEST priority target switch.
          Burn to 50% HP. Melee with high single-target burst (Frost DK Obliterate, Fury
          Bloodthirst, Rogue Eviscerate stack) excel here. Heroic: 60s = death.
        - mindbender_ghursha (default)
      interrupts:
        - Absorb Magic (76308) cast — interrupt the cast if catchable.
          Note: melee classes generally cannot purge in 4.3.4 (Rogue can't, Warrior can't,
          DK can't, Feral can't, Ret can't, Enhancement Shaman CAN purge).
      cooldown_plan: |
        Save burst CDs for Enslave windows in P2 — burning enslaved player to 50% as fast
        as possible reduces party damage exposure.
      positioning: |
        Stay in melee range of boss between Enslave events.
        Move OUT of Mind Fog (76230) clouds immediately.

  anti_patterns:
    - do NOT stand in front of Erunak — Magma Splash cone is huge
    - do NOT skip the Enslaved player target switch — they die on heroic if not freed
    - do NOT continue auto-attacking the Enslaved player past 50% — they break free at 50%, additional damage is wasted (and on heroic might kill them)

  cooldown_summary:
    offensive_cds:
      open_at_p1: yes
      save_for_p2_enslave: defensible (timing varies)
    defensive_cds: reactive on Mind Fog overlap or Unrelenting Agony cluster

  reference_mechanics:
    - encounter.md: Magma Splash (76170), Lava Bolt (76171), Earth Shards (84931),
      Enslave (76207), Mind Fog (76230), Absorb Magic (76308), Unrelenting Agony (76339)
```

---

## Boss 4 — Ozumat

```yaml
boss_melee:
  boss: ozumat
  difficulty_grade: extreme           # P3 specifically — DPS race

  per_phase:
    phase_1_filthy_tide:
      target_priority:
        - Vicious Mindlasher (3) — TOP priority. Brain Spike is fight-defining.
        - Unyielding Behemoth (1, ~15s in) — secondary
        - Deep Murloc Invader packs — cleave only; do not switch off Mindlashers to single-target
      interrupts:
        - Brain Spike (Mindlasher) — MUST_INTERRUPT, EVERY cast. Coordinate with ranged.
        - Mindlasher Shadow Bolt — secondary
      cooldown_plan: |
        Burst Mindlashers down with offensive CDs at pull. Cleave murlocs as they pass.
      positioning: |
        Stay behind Behemoth when he spawns — Blight Spray is a 90° frontal cone, 25y range,
        4s channel.
        Stay close to Neptulon's pedestal — don't chase adds beyond engage range.

    phase_2_sappers:
      target_priority:
        - Faceless Sapper (3, CEILING) — RANGED PRIMARY responsibility.
          Melee involvement is class-dependent: jumping/leaping abilities (Heroic Leap,
          Death Grip-self via terrain, Disengage backwards into wall) MAY reach. Default
          assumption: melee CANNOT reach Sappers. UNCERTAIN — check encounter geometry.
        - Blight Beast (ground spawn) — DEFAULT melee target during P2 if Sappers unreachable
      interrupts: []
      cooldown_plan: |
        Sustain — save burst for P3.
        If Beasts assigned to melee: focus single-target Beast kill to relieve tank kite.
      positioning: |
        Stay >=10y from beast pack to avoid Aura of Dread cluster damage.
        Dodge Blight of Ozumat (83561) ground pools.

    phase_3_tidal_surge:
      target: ozumat (all-in)
      interrupts: []
      cooldown_plan: |
        POP EVERY OFFENSIVE CD at P3 start. Tidal Surge buff (+2000% damage) makes this
        the only window where DPS matters. There is no later phase to save for.
      positioning: |
        Stay in melee range of Ozumat with party. Move only to dodge fresh Blight pools.
        On melee: this is the easiest positioning of the fight — Ozumat is stationary
        and stack-on-boss is the prescribed pattern.

  anti_patterns:
    - do NOT prioritize murlocs over Mindlashers in P1 — Brain Spike >> murloc cleave
    - do NOT attempt Sappers in P2 if you can't reach them — wasted GCDs on attempts
    - do NOT save CDs in P3 — there is no later phase

  cooldown_summary:
    offensive_cds: open at pull (P1 Mindlasher burst); refresh for P3 entry
    defensive_cds: reactive on Behemoth Shadow Blast knockback / Blight pool overlap

  reference_mechanics:
    - encounter.md: Mindlasher abilities, Behemoth abilities, Faceless Sapper, Blight Beast
      Aura of Dread, Blight of Ozumat (83561), Tidal Surge (Neptulon party buff)
```

---

## Cross-fight melee-DPS budget (full clear)

```yaml
cross_fight_budget:
  interrupt_count_required:
    nazjar_shock_blast: ~10-15 (melee primary; ranged backup)
    erunak_lava_bolt: ~5-8 (melee primary)
    mindlasher_brain_spike: ~3-6 (shared with ranged)
    spiritmender_healing_wave: 5-8 (gauntlet trash, melee primary)
    total_interrupt_uses: ~25-40 dungeon-wide
    minimum_interrupt_cd_required: <=15s

  target_switches:
    enslaved_player_burn: ~2-4 across Ghursha P2
    mindlasher_priority: 3 (initial P1 burst)

  mobility_cd_uses:
    ulthok_fissure_dodge: ~3-6 (depends on fissure spawn timing)
    nazjar_geyser_dodge: ~4-8 across full fight
    ozumat_blight_pool_dodge: ~5-10 across P2/P3

offensive_cd_alignment:
  open_at_pull:
    - lady_nazjar (sustain through fight)
    - commander_ulthok (short fight)
    - erunak_phase_1 (burst to 50%)
    - ozumat_phase_1 (Mindlasher burst)
  refresh_for:
    - ozumat_phase_3 (MANDATORY full burn)
```

---

## Anti-pattern summary

```yaml
anti_patterns:
  - id: missed_shock_blast_interrupt
    fight: lady_nazjar
    rule: |
      Melee is the PRIMARY interrupter on Shock Blast (76008). Every missed cast on
      heroic = potential tank death. Coordinate rotation if multiple melee.
    bot_implementation: |
      In Naz'jar Phase 1/3/5, gate offensive ability casts on:
      `not (boss.casting(ShockBlast) AND interrupt.off_cooldown)` — i.e., reserve a
      GCD for interrupt when needed.

  - id: front_arc_violation
    fight: lady_nazjar_waterspout, mindbender_ghursha_phase_1
    rule: |
      Honor Guard Arc Slash (75907) and Erunak Magma Splash (76170) are frontal
      cones that ignore all standard avoidance. Melee MUST stay behind these targets.
    bot_implementation: |
      Add a `position_behind_target` flag to specific encounter mechanics. Bot
      pathing must enforce rear-arc positioning when this flag is set.

  - id: fissure_tunnel
    fight: commander_ulthok
    rule: do not stop moving on Ulthok — Dark Fissure persists and expands on heroic
    bot_implementation: |
      In Ulthok encounter, augment melee positioning with a "no-stand zone" updated
      every fissure spawn event. Bot must reposition off any zone within 3s of spawn.

  - id: enslaved_overkill
    fight: mindbender_ghursha_phase_2
    rule: |
      Stop attacking the Enslaved player at 50% HP. They break free at 50%; additional
      damage on heroic could kill them outright (40% buffer is thin).
    bot_implementation: |
      In Ghursha P2, gate damage on enslaved target:
      `enslaved_target.hp_pct > 50` — release target the moment HP drops below 50%.

  - id: sapper_attempt_loop
    fight: ozumat_phase_2
    rule: do not waste GCDs trying to reach unreachable Sappers
    bot_implementation: |
      In Ozumat P2, melee target_priority defaults to Blight Beast (or stays on Ozumat
      ineffectually if no Beast). Attempting Sapper reach requires explicit class-spec
      capability flag (Heroic Leap range, Death Grip range, etc.).
```

---

## How this layers on future class spec docs

Per-class melee DPS specs (when authored — Combat/Assassination/Subtlety Rogue, Arms/Fury Warrior, Retribution Paladin, Frost/Unholy Death Knight, Enhancement Shaman, Feral Cat Druid) will reference this doc and add:

| What this doc defines | What class specs add |
|---|---|
| Interrupt priority list per fight | Class-specific interrupt spell IDs (Kick, Pummel, Rebuke, Mind Freeze, Skull Bash, Wind Shear) and cooldowns |
| Mobility cooldown allocation | Class-specific gap-closers (Charge, Heroic Leap, Shadowstep, Death's Advance, Feral Charge, Burning Rush) |
| "Open at pull / refresh for P3" cooldown alignment | Class-specific offensive CDs (Recklessness, Berserk, Adrenaline Rush, Pillar of Frost, Avenging Wrath, Bloodlust/Heroism for Enhance) |
| Defensive CD reactive use | Class-specific defensives (Cloak of Shadows, Shield Wall, Anti-Magic Shell, Survival Instincts, Divine Protection, Shamanistic Rage) |
| Position-behind-target enforcement | Class-specific rear-arc abilities (Backstab requirement, Rake/Shred, etc. — some abilities REQUIRE rear positioning, increasing the cost of front-arc violation) |
| Target-switch behavior | Class-specific switch cost — single-target classes (Frost DK, Fury Warrior) switch cheaply; combo-point / DoT classes (Sub Rogue, Feral) lose value on switches |

When a class spec doc is authored, it should layer on top of this role doc the same way `healer.md` layers on `docs/specs/resto-shaman.md` — only document the deviations and class bindings.
