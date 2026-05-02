# Throne of the Tides — Ranged DPS

**Ranged-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how a ranged DPS reacts. Class-agnostic — when per-class spec docs exist (mage, warlock, hunter, balance druid, shadow priest, ele shaman), they can reference this doc and add class-specific CD bindings.

**Layering**:
- Cross-content ranged-DPS patterns (range management, target priority, AoE thresholds, mobility, anti-patterns) live in `docs/roles/ranged-dps.md`
- Per-spec rotation / CD model lives in `docs/specs/{spec}.md` (TBD per class)
- This doc layers ToTT-specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_ranged_overview:
  total_bosses: 4
  estimated_full_clear_minutes: 20-30
  primary_responsibilities:
    - interrupt_rotation     # Brain Spike P1 Ozumat is THE single-most-critical interrupt in the dungeon
    - cc_duty                # Tempest Witches on Lady Naz'jar add phases
    - target_switching       # Sappers on Ozumat P2; Enslaved player on Ghur'sha P2
    - dispel_offensive       # Spellsteal/purge Absorb Magic on Ghur'sha (class-dependent)
    - aoe_throughput         # Naz'jar add phases; Ozumat P1 murloc waves
  cc_repertoire:
    # Working CCs on the relevant Tempest Witch (humanoid naga):
    polymorph: mage
    hex: shaman_(any_spec)
    repentance: paladin_(ret_or_holy)
    freezing_trap: hunter
    # Banish does NOT work (Tempest Witch is not Demon/Elemental).
    # Hibernate does NOT work (not Beast/Dragonkin).
  spellsteal_purge_repertoire:
    spellsteal: mage
    tranq_shot: hunter           # purges magic / enrages
    devour_magic: warlock_(felhunter_pet)
    dispel_magic_offensive: priest_(if_specced)
    purge: shaman_(elemental_or_enhancement)   # NOTE: Resto Shaman cannot purge in 4.3.4
```

---

## Trash — ranged DPS notes

```yaml
trash_ranged:
  naga_gauntlet:
    target_priority:
      - Naz'jar Spiritmender (priority kill; their heals are large)
      - Naz'jar Tempest Witch (trash version)
      - Naz'jar Sentinel
      - Naz'jar Invader (melee — leave for melee party members)
    interrupts:
      - Spiritmender Healing Wave (MUST_INTERRUPT — backup if melee misses)
      - Spiritmender Wrath (secondary)
      - Tempest Witch Lightning Bolt (MUST_INTERRUPT)
    cc_use: Polymorph/Trap/Repentance second Spiritmender if pulls double up
    positioning: max range from current target; spread to avoid Lightning Surge AoE

  faceless_wing:
    target_priority:
      - Faceless Seer (interrupt-priority on Mind Flay)
      - Faceless Watcher (high HP; DPS race vs Crush on tank)
      - Minion of Ghur'sha (stun-locked when possible)
      - Gilgoblin Aquamage (interrupt Water Bolts)
    interrupts:
      - Faceless Seer Mind Flay (MUST_INTERRUPT)
      - Gilgoblin Aquamage Water Bolt (interrupt as bandwidth allows)
    positioning: stay outside melee range; avoid Psionic Pulse cone if possible

  corruption_hallway:
    target_priority:
      - Tainted Sentry (interrupt Swell cast OR purge resulting buff)
      - Unstable Corruption (kill order matters for disease dispel between pulls)
    interrupts:
      - Tainted Sentry Swell (MUST_INTERRUPT, alternative is post-cast purge)
    positioning: standard ranged spread
```

---

## Boss 1 — Lady Naz'jar

```yaml
boss_ranged:
  boss: lady_nazjar
  difficulty_grade: moderate

  pre_pull:
    - mark_targets: agree on which Tempest Witch is the CC target before pull (preserves CC across phase 2/4 spawns)

  per_phase:
    phase_1_3_5:
      target: lady_nazjar
      interrupts:
        - Shock Blast (76008) — BACKUP interrupt rotation behind melee. Every cast is lethal-to-tank on heroic.
      cooldown_plan: |
        Use offensive CDs on boss windows; nothing fight-defining about timing for ranged here.
      positioning: max range from boss; >=5y from other allies (Fungal Spores splash, Geyser knockback)

    waterspout_phases_2_4:
      target_priority:
        - Naz'jar Tempest Witch (CC ONE, kill the OTHER first)
        - Naz'jar Honor Guard (engage after first Witch dies, OR concurrently if party comp lacks CC)
      interrupts:
        - Tempest Witch Chain Lightning (103637) — MUST_INTERRUPT (hits 3 allies in chain)
        - Tempest Witch Lightning Bolt — MUST_INTERRUPT (the casting Witch, not CC'd one)
      cc_duty:
        - apply CC on the marked Tempest Witch IMMEDIATELY at spawn
        - re-apply if CC breaks (CC'd targets take damage from cleave / DoTs — control DoT placement)
      dispel_assist:
        - Lightning Surge (75992 — MAGIC, timed detonation): if class can dispel magic, help the healer
        - Honor Guard Enrage (22428): purge if class can (Tranq Shot / Devour Magic / etc.)
      positioning: |
        Stay >=8y from other allies. Honor Guard Arc Slash is a frontal cone — never stand
        in front of him.

  anti_patterns:
    - do NOT break CC by cleaving onto the CC'd Tempest Witch
    - do NOT chase the boss during Waterspout — boss is immune; focus add survival
    - do NOT stand in Geyser telegraph circles (~5s detonation window)

  cooldown_summary:
    offensive_cds: free use; no specific phase to save them for
    defensive_cds: reactive on Geyser knockback or Lightning Surge if not dispelled

  reference_mechanics:
    - encounter.md: Shock Blast (76008), Chain Lightning (103637), Lightning Surge (75992),
      Geyser (75722), Fungal Spores (76001), Arc Slash (75907), Enrage (22428)
```

---

## Boss 2 — Commander Ulthok

```yaml
boss_ranged:
  boss: commander_ulthok
  difficulty_grade: moderate

  per_phase:
    single_phase:
      target: commander_ulthok (tunnel)
      interrupts: []   # no interruptible boss casts
      cooldown_plan: |
        Free CD usage early — fight is short-ish, no specific save windows. Tunnel boss.
        Save mobility cooldowns (Blink, Disengage, etc.) for fissure-spawn moments.
      positioning: |
        - Stay max range from boss along the tank's kite path.
        - Move CONTINUOUSLY — Dark Fissure (76047) spawns telegraph at boss position; on heroic
          they expand and persist.
        - When Cursed (76094 — Curse of Fatigue), pre-position toward open ground; you cannot
          outrun fissures while slowed. Trust the healer to dispel within 1 GCD.

  anti_patterns:
    - do NOT stand still mid-cast — fissure spawns will eat you
    - do NOT cluster with other ranged — fissure splash radius (6y) hits multiple if grouped

  cooldown_summary:
    offensive_cds: open with them; fight is short
    defensive_cds: reactive on Squeeze (76026) target — if you're squeezed, use any "while-stunned" defensives (Cloak of Shadows for rogue is melee, so doesn't apply; Iceblock for mage; Dispersion for shadow priest; etc.)

  reference_mechanics:
    - encounter.md: Squeeze (76026), Dark Fissure (76047), Curse of Fatigue (76094)
```

---

## Boss 3 — Mindbender Ghur'sha

```yaml
boss_ranged:
  boss: mindbender_ghursha
  difficulty_grade: high

  per_phase:
    phase_1_erunak:
      target: erunak_stonespeaker (until 50%)
      interrupts:
        - Lava Bolt (76171) — MUST_INTERRUPT (high priority; backup to melee)
      cooldown_plan: |
        Burn Erunak fast — phase 1 to 50% is the only window before P2 complications.
      positioning: |
        Max range, behind boss (Magma Splash 76170 is a 90° frontal cone, 20y).
        Avoid Earth Shards (84931) ground telegraphs.

    phase_2_ghursha:
      target_priority:
        - ENSLAVED PLAYER (when Enslave 76207 lands) — HIGHEST priority target switch.
          Burn to 50% HP to break free. Heroic: 60s timeout = death.
        - mindbender_ghursha (default)
      interrupts:
        - Absorb Magic (76308) cast — interrupt if catchable; PURGE preferred (see dispel below)
      dispel_offensive:
        - Absorb Magic (76308) on Ghur'sha — HIGHEST PRIORITY in instance.
          Spellsteal (mage) / Tranq Shot (hunter) / Devour Magic (warlock pet) ASAP.
          The buff heals Ghur'sha for 3x absorbed; every GCD of delay is huge.
      cooldown_plan: |
        Save burst CDs for Enslave windows — burning enslaved player to 50% as fast as
        possible reduces party-damage exposure.
      positioning: |
        Spread to avoid Mind Fog (76230) overlap. Move OUT of Mind Fog clouds immediately
        — silence/pacify is fight-losing for casters.
        LoS the Unrelenting Agony (76339) channel if geometry permits (per Icy Veins).

  anti_patterns:
    - do NOT continue casting when Absorb Magic is up if you can't purge it — you're healing the boss
    - do NOT stand in Mind Fog clouds — silence ruins ranged DPS uptime
    - do NOT fail to switch to the Enslaved player — they die in 60s on heroic

  cooldown_summary:
    offensive_cds: |
      Open Erunak P1 with CDs to make 50% transition fast.
      Save second-rotation CDs for Enslave windows in P2.
    defensive_cds: reactive on Mind Fog overlap or Unrelenting Agony cluster

  reference_mechanics:
    - encounter.md: Magma Splash (76170), Lava Bolt (76171), Earth Shards (84931),
      Enslave (76207), Mind Fog (76230), Absorb Magic (76308), Unrelenting Agony (76339)
```

---

## Boss 4 — Ozumat

```yaml
boss_ranged:
  boss: ozumat
  difficulty_grade: extreme           # P3 specifically — DPS race
  primary_phase: P2 (Sapper killer) and P3 (Ozumat burn)

  per_phase:
    phase_1_filthy_tide:
      target_priority:
        - Vicious Mindlasher (3) — TOP priority; their Brain Spike is fight-defining
        - Unyielding Behemoth (1, ~15s in) — secondary, after Mindlashers down
        - Deep Murloc Invader packs (3 packs of 6) — cleave/AoE while engaging Mindlashers
      interrupts:
        - Brain Spike (Mindlasher) — MUST_INTERRUPT, EVERY cast.
          Party-wide Shadow damage + 1k mana drain. This is THE most critical
          interrupt in the dungeon. Set up an interrupt rotation BEFORE pulling.
        - Mindlasher Shadow Bolt — secondary interrupt
        - Veil of Shadow (heroic only — Mindlasher Curse cast) — MUST_INTERRUPT or dispel result
      cooldown_plan: |
        Burst CDs at pull — Mindlashers must die fast OR a lot of Brain Spikes will land.
        AoE on murloc packs as they approach Neptulon.
      positioning: |
        Stay near Neptulon's pedestal. Do NOT chase adds — let them come to you.

    phase_2_sappers:
      target_priority:
        - Faceless Sapper (3, ceiling) — RANGED PRIMARY responsibility. Kill all 3 to end phase.
        - Blight Beast (periodic ground spawn) — secondary; help only if directed
      interrupts: []
      cooldown_plan: |
        Mid-fight CD usage — open with whatever's off CD on Sappers.
        Save burst for P3 Ozumat burn.
      positioning: |
        Tank kites Blight Beasts; stay >=10y from beast pack to avoid Aura of Dread
        cluster damage. Dodge Blight of Ozumat (83561) ground pools.

    phase_3_tidal_surge:
      target: ozumat (all-in)
      interrupts: []
      cooldown_plan: |
        POP EVERY OFFENSIVE CD. Tidal Surge buff gives +2000% damage; this is the
        only window where DPS matters and the only way the party survives is by
        killing Ozumat before stacking Blight DoT becomes unhealable.
      positioning: stack on Ozumat with party; move only to dodge fresh Blight pools

  anti_patterns:
    - do NOT prioritize murlocs over Mindlashers in P1 — Brain Spike >> murloc cleave
    - do NOT tunnel Beasts in P2 if Sappers are still up — phase ends only on Sappers
    - do NOT save CDs in P3 — there is no later phase

  cooldown_summary:
    offensive_cds: open at pull; refresh on P3 start
    defensive_cds: reactive on Behemoth Shadow Blast knockback / Aura of Dread overlap

  reference_mechanics:
    - encounter.md: Mindlasher abilities, Behemoth abilities, Faceless Sapper, Blight Beast
      Aura of Dread, Blight of Ozumat (83561), Tidal Surge (Neptulon party buff)
```

---

## Cross-fight ranged-DPS budget (full clear)

```yaml
cross_fight_budget:
  interrupt_count_required:
    naz_jar_shock_blast: ~10-15 across fight (every 10-15s in non-waterspout phases)
    naz_jar_chain_lightning: ~3-5 per add phase (twice per fight = ~6-10 total)
    erunak_lava_bolt: ~5-8 across P1
    mindlasher_brain_spike: ~3-6 across P1 of Ozumat (depends on Mindlasher uptime)
    total_interrupt_uses: ~25-40 dungeon-wide
    minimum_interrupt_cd_required: <=15s (most class interrupts qualify)

  cc_uses:
    naz_jar_tempest_witch: 2 (one per add phase)
    naz_jar_spiritmender: 1-2 (gauntlet trash, situational)

  spellsteal_purge_uses:
    ghursha_absorb_magic: ~3-5 across fight
    nazjar_honor_guard_enrage: 0-2 (only if hits 35% before death)

  target_switches:
    enslaved_player_burn: ~2-4 across Ghursha P2
    sapper_priority: 3 (one per Sapper kill)
    mindlasher_priority: 3 (initial P1 burst)

offensive_cd_alignment:
  open_at_pull:
    - lady_nazjar
    - commander_ulthok
    - erunak_phase_1
    - ozumat_phase_1   # to burst Mindlashers
  refresh_for:
    - ghursha_p2_first_enslave
    - ozumat_phase_3   # MANDATORY — full burn
```

---

## Anti-pattern summary

```yaml
anti_patterns:
  - id: missed_brain_spike_interrupt
    fight: ozumat_phase_1
    rule: |
      Brain Spike from Vicious Mindlasher is the single most damaging non-tank
      ability in the dungeon — party-wide Shadow + mana drain. Set an interrupt
      rotation BEFORE pulling Ozumat. Every missed Brain Spike = potential wipe.
    bot_implementation: |
      In Ozumat P1, target_priority must place Mindlashers at top, with interrupt
      gating: if Mindlasher.casting(BrainSpike) AND interrupt.off_cooldown,
      override current cast with interrupt.

  - id: cc_break_via_cleave
    fight: lady_nazjar_waterspout
    rule: do not cleave/DoT/AoE the CC'd Tempest Witch — breaking CC = unmarked add free-castin
    bot_implementation: |
      Maintain a "cc_protected" flag on a target ID. Skip targets with this flag
      from any AoE / cleave / DoT-spread targeting. Lift the flag when CC aura
      drops or expires.

  - id: sapper_skip
    fight: ozumat_phase_2
    rule: phase 2 ends only when all 3 Sappers die — kill any other target = wasted DPS
    bot_implementation: |
      In Ozumat P2, target_priority is hard-coded to Sappers. Blight Beasts only
      qualify if the tank explicitly assigns them (otherwise tank kites; party
      ignores Beasts).

  - id: cd_hoard_p3
    fight: ozumat_phase_3
    rule: pop every offensive CD at P3 start; saving them is a wipe
    bot_implementation: |
      Override default offensive-CD gates in Ozumat P3 to fire on phase entry,
      regardless of usual conserve / opener-only logic.

  - id: continue_casting_through_absorb_magic
    fight: mindbender_ghursha_phase_2
    rule: |
      Absorb Magic on Ghur'sha makes every spell hit you cast into a heal for the boss
      (3x absorbed). Stop offensive casts until purged.
    bot_implementation: |
      In Ghursha P2, gate offensive casts on `not aura.active(boss, ABSORB_MAGIC)`.
      If aura is active and bot can purge, fire purge first; if bot cannot purge,
      idle (or shift to non-damage actions like CC refresh) until purge or expire.
```

---

## How this layers on future class spec docs

Per-class ranged DPS specs (when authored — Balance Druid, Frost/Fire Mage, Marks/BM/Survival Hunter, Shadow Priest, Affliction/Demonology/Destruction Warlock, Elemental Shaman) will reference this doc and add:

| What this doc defines | What class specs add |
|---|---|
| Interrupt priority list per fight | Class-specific interrupt spell IDs (Counterspell, Silence, Wind Shear, Spell Lock, Solar Beam, etc.) and cooldowns |
| CC duty on Tempest Witches | Class-specific CC choice (Polymorph variant, Hex, Repentance, Freezing Trap) |
| Spellsteal/purge on Absorb Magic | Class-specific tool: Spellsteal vs Tranq Shot vs Devour Magic vs Purge |
| "Open at pull / refresh for P3" cooldown alignment | Class-specific offensive CDs to align (Trinket, Bloodlust/Heroism, Combustion, etc.) |
| Defensive CD reactive use | Class-specific defensives (Iceblock, Dispersion, Deterrence, Soul Link, etc.) |
| Target switching rules | Class-specific switch behavior — DoT classes (Affliction, Balance) lose value on rapid switches; burst classes (Frost Mage, Marksman) thrive |

When a class spec doc is authored, it should layer on top of this role doc the same way `healer.md` layers on `docs/specs/resto-shaman.md` — only document the deviations and class bindings, not duplicate the role-level logic.
