# Throne of the Tides — Tank

**Tank-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how a tank reacts. Class-agnostic — when per-class spec docs exist (Prot Warrior, Prot Paladin, Blood DK, Guardian Druid), they can reference this doc and add class-specific bindings.

**Layering**:
- Cross-content tank patterns (active mitigation cycle, threat-on-pull, cone-facing, defensive CD pacing, kite path planning, anti-patterns) live in `docs/roles/tank.md`
- Per-spec rotation / mitigation cycle / threat model lives in `docs/specs/{spec}.md` (TBD per class)
- This doc layers ToTT-specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_tank_overview:
  total_bosses: 4
  estimated_full_clear_minutes: 20-30
  primary_responsibilities:
    - threat_management      # snap aggro on adds; hold bosses through phase transitions
    - cone_facing            # face Honor Guard / Erunak / Behemoth AWAY from group
    - kiting                 # Ulthok perimeter kite; Ozumat P2 Blight Beast circular kite
    - defensive_cd_pacing    # absorb Shock Blast misses, Aura of Dread overlap, P3 Blight stacks
    - pre_pull_setup         # threat-on-the-pull, prepull buffs (Vigilance, Last Stand pre-application, etc.)
  positioning_themes:
    - face_away_from_group   # cones (Arc Slash, Magma Splash, Blight Spray)
    - perimeter_kite          # Ulthok heroic, Ozumat P2 Beasts
    - stack_central          # Naz'jar Phase 1/3/5; Ozumat P3
  defensive_repertoire:
    # 4.3.4 tank cooldowns by class:
    shield_wall_class: warrior
    last_stand_class: warrior
    ardent_defender: paladin_(protection)
    divine_protection: paladin
    icebound_fortitude: death_knight
    vampiric_blood: death_knight_(blood)
    rune_tap: death_knight_(blood)
    survival_instincts: druid_(feral_or_guardian)
    barkskin: druid
    frenzied_regeneration: druid_(feral_or_guardian)
```

---

## Trash — tank notes

```yaml
trash_tank:
  naga_gauntlet:
    threat_management: |
      Spiritmenders heal — pull them with the rest, they'll start casting and DPS will
      kick. Use AoE threat (Thunder Clap, Avenger's Shield, Death and Decay, Swipe) on
      grouped naga packs.
    positioning: |
      Face Honor Guard packs AWAY from group (Arc Slash cone). Face Sentinels into
      walls if possible (Shellbreaker has no positioning constraint, but rear-attack
      threat is generally cheaper).
    defensive_cd_use: reactive — use minor CDs (Shield Block, Death Strike, etc.) on heavy pulls
    mob_specific:
      - Naz'jar Sentinel: Crushing Depths stacks on tank — non-dispellable; rotate
        defensives between heavy stacks
      - Vicious Snap Dragon: Razor Jaws bleeds tank — request stun reset (Kidney Shot,
        Hammer of Justice) from party
      - Naz'jar Honor Guard (trash): Arc Slash cone — face away

  faceless_wing:
    threat_management: |
      Faceless Watcher Crush is constant — survive via Death Strike / Word of Glory /
      Frenzied Regen self-heals. Pick up Minions and Seers behind Watcher.
    positioning: |
      Face Watchers away from group (no specific cone, but threat positioning matters).
      Avoid Psionic Pulse cone from Minion of Ghur'sha if standing too close.
    defensive_cd_use: |
      Crush is hard-hitting; rotate minor defensives between hits. Save majors for
      Faceless Watcher pulls if doubled up.

  corruption_hallway:
    threat_management: |
      Tainted Sentry has Swell — interrupting is melee/ranged duty, but the resulting
      enrage buff (if not interrupted) makes tank damage spike — be ready with defensive.
    positioning: standard
    defensive_cd_use: reactive on un-interrupted Swell enrage
```

---

## Boss 1 — Lady Naz'jar

```yaml
boss_tank:
  boss: lady_nazjar
  difficulty_grade: moderate

  pre_pull:
    - apply_pre_pull_buffs: class-specific (Last Stand pre-application, Bone Shield pre-cast, etc.)
    - position: pull boss to a clear area away from walls — geyser knockback can bounce off walls oddly

  per_phase:
    phase_1_3_5:
      target: lady_nazjar (sole target)
      threat_priority: maintain max threat on boss; no add concerns
      defensive_cd_plan: |
        Shock Blast (76008) is interrupted by melee/ranged. If interrupt LAPSES, expect
        ~70-79k Nature damage on heroic. Pre-emptively pop a defensive (Shield Wall,
        Ardent Defender, Icebound Fortitude, Survival Instincts) when:
          - boss.casting(ShockBlast) AND no_interrupt_available_in_party
        OR have a "rotation defensive" (minor CD) up every 2-3 Shock Blast windows as
        insurance against missed interrupts.
      positioning: |
        Stack with melee (boss has no cone). Stay >=5y from healer/ranged so Geyser
        (75722) telegraphs don't catch the cluster.

    waterspout_phases_2_4:
      threat_priority:
        1. Naz'jar Honor Guard (immediate aggro snap on spawn)
        2. Naz'jar Tempest Witches (NON-CC'd one) — taunt/swap if they target healer
        3. Boss is immune; don't bother
      threat_actions: |
        Honor Guard spawns at boss's position — be there. Use AoE threat opener
        (Thunder Clap, Avenger's Shield, Death and Decay, Swipe). Then snap individual
        targets with single-target taunt/threat.
      defensive_cd_plan: |
        Honor Guard hits HARD (~150% melee Arc Slash if you're in front — DON'T BE).
        Tempest Witch Lightning Bolt + Chain Lightning add ranged damage.
        Pop a minor defensive at start of add phase; major defensive only if Honor
        Guard reaches Enrage (35% HP, +50% attack speed).
      positioning: |
        Face Honor Guard AWAY from group. Pull him toward a wall so his cone faces
        empty space. Tempest Witches don't melee (or melee weakly) — positioning
        for them is about taunt range, not facing.

  anti_patterns:
    - do NOT face Honor Guard toward the group — Arc Slash bypasses all avoidance
    - do NOT use major defensive on Shock Blast if interrupt rotation is solid — save for misses
    - do NOT pull boss into the cluster of allies — Geyser knockback radius hits the cluster

  cooldown_summary:
    minor_defensive: rotate every 2-3 Shock Blast cycles
    major_defensive: save for missed-interrupt scenarios + Honor Guard Enrage
    threat_cds: open at pull (Shield Block, Heroic Strike rage dump, Vengeance ramp, etc.)

  reference_mechanics:
    - encounter.md: Shock Blast (76008), Geyser (75722), Fungal Spores (76001),
      Arc Slash (75907), Enrage (22428), Chain Lightning (103637), Lightning Surge (75992)
```

---

## Boss 2 — Commander Ulthok

```yaml
boss_tank:
  boss: commander_ulthok
  difficulty_grade: moderate_to_high   # most movement-intensive tank fight in dungeon
  primary_concern: kite_path_management

  per_phase:
    single_phase:
      target: commander_ulthok (sole target)
      threat_priority: maintain max threat — no adds, no swaps
      defensive_cd_plan: |
        No tank-targeted spike abilities. Damage is steady from boss melee + ambient
        AoE if you stand in fissures. Use minor defensives reactively.
      kite_strategy: |
        - Heroic: Dark Fissure (76047) GROWS and PERSISTS (~60s).
        - Pre-plan a perimeter route around the room. Each fissure spawn = move boss
          ~5-10y along the perimeter, leaving the fissure behind.
        - Group follows behind you (melee in rear arc, ranged at max distance).
        - DO NOT BACKTRACK into existing fissures.
        - Counterclockwise kite is conventional; either direction works.
      positioning: |
        - Boss faces away from party center (no cone, but threat-positioning convention).
        - Maintain melee uptime — don't over-kite. Move only when fissure spawns force it.

  anti_patterns:
    - do NOT kite without a route — running boss into existing fissures kills the party
    - do NOT stop moving when Cursed (76094 — Curse of Fatigue) — wait for healer dispel,
      but pre-position toward open ground
    - do NOT face boss toward the cluster — Squeeze stuns are random non-tank, but melee
      cleave damage from boss can clip the group if facing wrong

  cooldown_summary:
    minor_defensive: reactive
    major_defensive: rarely needed; save for back-to-back fissure overlap if it happens
    mobility_cds: useful for Curse of Fatigue moments (Berserker Charge, Wings of Liberty, etc.)

  reference_mechanics:
    - encounter.md: Squeeze (76026), Dark Fissure (76047), Curse of Fatigue (76094)
```

---

## Boss 3 — Mindbender Ghur'sha

```yaml
boss_tank:
  boss: mindbender_ghursha
  difficulty_grade: moderate            # tank pressure is in P1, not P2

  per_phase:
    phase_1_erunak:
      target: erunak_stonespeaker (until 50%)
      threat_priority: hold Erunak through to phase transition
      defensive_cd_plan: |
        Erunak melee is hard-hitting. Emberstrike (76165) stacks Fire damage on subsequent
        swings — the healer dispels, but if dispel lapses, ramping Fire ticks can spike.
        Rotate minor defensives. Save major defensive for back-to-back high-stack scenarios.
      positioning: |
        FACE ERUNAK AWAY FROM GROUP. Magma Splash (76170) is a 90° frontal cone, 20y range,
        ~20k Fire + 2k/s DoT. This is THE positioning rule of P1.
        Move to dodge Earth Shards (84931) ground telegraphs. Slight repositioning is OK
        — just keep cone pointed away.

    phase_2_ghursha:
      target: mindbender_ghursha
      threat_priority: |
        Ghur'sha is small (low HP), and threat is generally easy. The fight in P2 is
        about MECHANICS, not threat. Just hold the boss in place while DPS handles
        Enslave / Mind Fog / Absorb Magic.
      defensive_cd_plan: |
        Tank takes very low damage in P2. Use minor defensives on cooldown (free uptime
        on offensive abilities like Shield Block / Death Strike for self-heal).
      positioning: |
        Hold boss in a fixed central spot. P2 has no cone mechanic — face the boss
        whichever way avoids overlapping party with Mind Fog (76230) clouds (which
        spawn at random ground locations).

  anti_patterns:
    - do NOT face Erunak toward the group in P1 — Magma Splash will instantly down at
      least one party member
    - do NOT continue tanking the Enslaved player if they're tanking — they're mind-controlled,
      but rare for them to grab boss aggro; if they do, taunt the boss back
    - do NOT move boss into Mind Fog clouds — keeps the fight clean

  cooldown_summary:
    minor_defensive: rotate freely; tank pressure is low overall
    major_defensive: rarely needed in this fight
    threat_cds: open at pull (Erunak takes some time to push to 50%)

  reference_mechanics:
    - encounter.md: Emberstrike (76165), Magma Splash (76170), Lava Bolt (76171),
      Earth Shards (84931), Enslave (76207), Mind Fog (76230), Absorb Magic (76308),
      Unrelenting Agony (76339)
```

---

## Boss 4 — Ozumat

```yaml
boss_tank:
  boss: ozumat
  difficulty_grade: high                # P2 kite is the test; P3 is straightforward
  primary_concern: phase_2_blight_beast_kite

  per_phase:
    phase_1_filthy_tide:
      target_priority:
        1. Unyielding Behemoth (single highest-priority — biggest threat to Neptulon)
        2. Vicious Mindlasher (3) — ranged-targeted, but pull aggro to keep them out of melee
        3. Deep Murloc Invader packs (3 packs of 6) — AoE pickup as they pass
      threat_actions: |
        Pull Behemoth on spawn (~15s in). AoE threat on murloc waves with Thunder Clap /
        Avenger's Shield / Death and Decay / Swipe. Use taunt to claim Mindlashers if they
        path to a healer/ranged.
      defensive_cd_plan: |
        Behemoth Shadow Blast (~24k Shadow + knockback) hits hard. Pop minor defensive
        on cooldown.
      positioning: |
        Face Behemoth AWAY from group AND away from Neptulon. Blight Spray (90° frontal
        cone, 25y, 4s channel) will obliterate Neptulon if cone clips the pedestal.

    phase_2_sappers:
      target_priority:
        1. Blight Beast (each spawn) — KITE, do not stand and tank
      threat_actions: |
        Pick up Blight Beasts on spawn. Beasts have Aura of Dread (7y radius, +5%
        Shadow vulnerability per stack) — clustering is fight-losing.
      defensive_cd_plan: |
        Tank takes constant Beast melee + Aura of Dread self-tick. Rotate minor and
        major defensives across the phase.
      kite_strategy: |
        - Circular path around the chamber (clockwise or counterclockwise — pick one).
        - Stay >=10y ahead of party at all times.
        - When a new Beast spawns, taunt it to you and CONTINUE MOVING.
        - Drag old Beasts with you; do NOT leave them behind in spawn locations.
      positioning: keep cone outward (no boss cone, but Beasts have melee swings — face them away from party center where possible)

    phase_3_tidal_surge:
      target: ozumat (sole target)
      threat_priority: maintain Ozumat threat through Tidal Surge buff window
      threat_actions: |
        Taunt at pull (boss spawns when phase enters). Use Vengeance / Tidal Surge buff
        to inflate threat — DPS's +2000% damage will rapidly out-threat you without
        active threat-CD usage.
      defensive_cd_plan: |
        Stacking Blight DoT hits everyone — including you. Tidal Surge gives +500% HP,
        which buys time, but at high stacks (>50) raid damage is severe. Rotate defensives.
        SAVE MAJOR DEFENSIVE for the final burn moment (last 20% of Ozumat) — that's where
        stacks peak.
      positioning: |
        Hold Ozumat in a clean spot. Move only to avoid fresh Blight pools. Stack-on-boss
        is the prescribed pattern; party will come to you.

  anti_patterns:
    - do NOT cluster Blight Beasts in P2 — Aura of Dread vulnerability stacks compound
    - do NOT face Behemoth toward Neptulon in P1 — Blight Spray cone wipes the encounter
    - do NOT stand still in P2 — Beast spawns will eventually catch up if you're stationary
    - do NOT save defensives in P3 — there's no later phase

  cooldown_summary:
    minor_defensive: rotate constantly across P1/P2/P3
    major_defensive:
      - P2 mid-phase if Beast count grows
      - P3 final 20% (stacks peak)
    threat_cds: open at P1 pull (Behemoth pickup); refresh at P3 entry (Ozumat taunt)

  reference_mechanics:
    - encounter.md: Mindlasher abilities, Behemoth abilities (Shadow Blast, Blight Spray),
      Faceless Sapper, Blight Beast (Aura of Dread), Blight of Ozumat (83561),
      Tidal Surge (Neptulon party buff)
```

---

## Cross-fight tank budget (full clear)

```yaml
cross_fight_budget:
  major_defensive_uses:
    naz_jar: 0-1                # only on missed interrupt or Honor Guard Enrage
    ulthok: 0                   # rarely needed
    ghursha: 0                  # P2 tank pressure is low
    ozumat: 1-2                 # P2 Beast cluster + P3 final burn
    full_dungeon_total: 1-3

  minor_defensive_uses:
    naz_jar: ~3-5 across full fight
    ulthok: ~3-5 across full fight
    ghursha: ~2-4 across full fight
    ozumat: ~6-10 across all 3 phases
    full_dungeon_total: ~15-25

  taunt_uses:
    nazjar_waterspout_adds: 2-4 per add phase (×2 phases = 4-8 total)
    ozumat_p1_mindlashers: 1-3 (if Mindlashers path off)
    ozumat_p1_behemoth: 1 (initial pickup)
    ozumat_p2_beasts: 3-6 (per spawn)
    full_dungeon_total: ~10-20

  threat_cd_uses:
    open_at_pull: 4 (one per boss)
    refresh_for_phase_transitions: 2 (Ghursha P2 small boss; Ozumat P3 spawn)
    full_dungeon_total: ~6
```

---

## Anti-pattern summary

```yaml
anti_patterns:
  - id: cone_facing_group
    fight: lady_nazjar_waterspout, mindbender_ghursha_phase_1, ozumat_phase_1
    rule: |
      Honor Guard (Arc Slash 75907), Erunak (Magma Splash 76170), and Behemoth (Blight
      Spray) all have frontal cones that bypass standard avoidance OR oneshot non-tanks.
      Tank MUST face these targets away from the group AND away from Neptulon.
    bot_implementation: |
      Add a `face_away_from_group` flag to specific encounter mobs. Tank's threat
      positioning logic must compute "away from party centroid" target heading and
      enforce it via movement before threat actions.

  - id: kite_into_existing_fissure
    fight: commander_ulthok
    rule: |
      Dark Fissure (76047) persists ~60s on heroic and grows over time. Backtracking
      into existing fissures kills the party. Plan a one-way perimeter kite.
    bot_implementation: |
      Maintain a list of active fissure positions (timestamp + center + radius). Tank
      pathing must avoid this set when computing kite direction. Direction picked at
      first fissure spawn and not reversed.

  - id: blight_beast_cluster
    fight: ozumat_phase_2
    rule: |
      Aura of Dread (Blight Beast) stacks +5% Shadow vulnerability per stack within 7y.
      Clustering Beasts (or letting party cluster around tank) compounds the debuff
      to fight-losing levels.
    bot_implementation: |
      In Ozumat P2, tank must maintain >=10y distance from party. Movement controller
      enforces lead distance ahead of party centroid. Beasts dragged WITH tank, not
      left behind.

  - id: blight_spray_at_neptulon
    fight: ozumat_phase_1
    rule: Behemoth Blight Spray cone facing Neptulon = encounter wipe (Neptulon dies)
    bot_implementation: |
      Behemoth threat positioning must compute "away from Neptulon AND away from party"
      heading. If both can't be satisfied (e.g., narrow chamber), prioritize Neptulon
      first.

  - id: defensive_hoarding_p3
    fight: ozumat_phase_3
    rule: |
      Save major defensive for final 20% Blight peak. Hoarding past that = wasted CD
      because either boss dies or party wipes.
    bot_implementation: |
      In Ozumat P3, gate major defensive on:
      `ozumat.hp_pct < 25 OR raid.average_blight_stacks > 50`. Fire on first true.
```

---

## How this layers on future class spec docs

Per-class tank specs (when authored — Protection Warrior, Protection Paladin, Blood Death Knight, Guardian Druid) will reference this doc and add:

| What this doc defines | What class specs add |
|---|---|
| Major defensive CD usage windows | Class-specific names (Shield Wall, Ardent Defender, Icebound Fortitude, Survival Instincts) and cooldowns |
| Minor defensive rotation | Class-specific minor CDs (Shield Block, Word of Glory self-heal, Death Strike rune cycle, Frenzied Regeneration) |
| Active mitigation model | Cata-era classes have different active-mitigation paradigms — Prot Warrior Shield Block + Heroic Strike rage dump, Prot Pally Holy Power → Word of Glory / Shield of the Righteous, Blood DK Death Strike for self-heal, Guardian Druid Savage Defense procs |
| AoE threat opener | Class-specific (Thunder Clap, Avenger's Shield, Death and Decay, Swipe) |
| Taunt | Class-specific (Taunt — Warrior, Hand of Reckoning — Paladin, Dark Command — DK, Growl — Druid) |
| Threat-buff CDs | Class-specific (Vengeance ramp, Inquisition for ret/prot pally) |
| Kiting tools | Class-specific mobility (Heroic Leap, Speed of Light, Death's Advance, Stampeding Roar) |
| Self-healing during damage windows | Class-specific (Death Strike rune cycle, Word of Glory, Frenzied Regen, Enraged Regen) |

When a class spec doc is authored, it should layer on top of this role doc the same way `healer.md` layers on `docs/specs/resto-shaman.md` — only document the deviations and class bindings, not duplicate the role-level logic.
