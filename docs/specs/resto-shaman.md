# Resto Shaman — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide. The YAML blocks below are designed to lift cleanly into `static const` tables in a future `AltbotCombat.cpp`. Prose explains the reasoning so thresholds can be tuned without re-deriving them.

**Numerical data** (spell IDs, mana %, cast times, cooldowns, durations, item IDs) verified against Wowhead's Cata 4.3.4 archive (`wowhead.com/cata/spell=<id>`). Items the AI will need but that public archives don't pin down with confidence are tagged `UNVERIFIED` — those should be confirmed against the server's `Spell.dbc` before code commits to the value.

**Strategy content** (rotation priority, cooldown triggers, mana thresholds) reconciled across 5 community guides. See `docs/research/resto-shaman-guide-survey.md` for the comparison table and which thresholds are consensus vs author-judgment.

---

## Identity

```yaml
spec: resto_shaman
class: shaman
role: HEALER
mana_model: spirit_regen + Resurgence_proc + Water_Shield_orb
weapon_imbue: Earthliving_Weapon
defining_procs:
  - Tidal_Waves            # 2 charges granted by Riptide / Chain Heal
  - Resurgence             # mana return on direct-heal crit (requires Water Shield)
  - Earthliving_HoT        # 20% chance HoT proc on direct heal targets
key_cooldowns:
  - Mana_Tide_Totem        # 3 min — group/raid mana regen
  - Spirit_Link_Totem      # 3 min — emergency HP equalize (signature 4.3 cooldown)
  - Spiritwalker_Grace     # 2 min — cast-while-moving
  - Natures_Swiftness      # 2 min — instant + amplified next cast
  - Earth_Elemental_Totem  # 10 min — emergency tank/threat backup
ranged: 40
notes: |
  Resto Shaman in 4.3.4 plays as a reactive healer with a strong AoE smart-heal (Chain Heal),
  a tank-maintenance HoT (Riptide → Tidal Waves), a tank-attached damage-reactive HoT
  (Earth Shield), and a signature emergency cooldown (Spirit Link Totem) that equalizes party
  HP percentage in a 10y radius. Throughput per cast is high but mana is finite — the AI
  should treat mana as a budget and prefer Healing Wave over Healing Surge when tank stable.
```

---

## Target taxonomy

The AI references these target selectors throughout. Implementations should resolve these against group state once per tick and reuse the resolved unit pointers across the rotation pass.

```yaml
target_selectors:
  SELF:                      # the bot itself
  TANK:                      # group member with TANK role flag; fallback = master
  PARTY_LOWEST_HP:           # ally member with lowest hp_pct, in 40y, alive
  PARTY_LOWEST_MISSING_HP:   # ally with greatest absolute missing HP (favors tanks late-fight)
  CHAIN_HEAL_PRIMARY:        # ally hp_pct < 90 with 2+ injured allies within 12y of them
  AOE_CLUSTER_3:             # 3+ allies within 10y of each other, all hp_pct < 75
  ENEMY_TARGET:              # bot's current attack target (rare for resto — Wind Shear / utility)
  ENEMY_CASTING_INTERRUPTIBLE: # nearest enemy within 25y currently casting an interruptible spell
  GROUND_AT_SELF:            # totem placement at bot's feet
  WEAPON_MAINHAND:           # weapon imbue target
```

---

## Spell Catalog

```yaml
# All spell IDs verified at wowhead.com/cata/spell=<id>
# mana costs are % of base mana (Cata standard)
# cast_time / cooldown / duration in ms
spells:
  HEALING_WAVE:
    id: 331
    mana_pct: 9
    cast_time_ms: 3000
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: nature
    target_type: ally
    notes: efficient base heal; Tidal Waves reduces cast_time_ms by 30%

  GREATER_HEALING_WAVE:
    id: 77472
    mana_pct: 33
    cast_time_ms: 3000
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: nature
    target_type: ally
    notes: heavy single-target heal; Tidal Waves reduces cast_time_ms by 30%

  HEALING_SURGE:
    id: 8004
    mana_pct: 27
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: nature
    target_type: ally
    notes: fast emergency heal; Tidal Waves grants +30% crit on this cast

  RIPTIDE:
    id: 61295
    mana_pct: 10
    cast_time_ms: 0
    cooldown_ms: 6000
    on_gcd: true
    range: 40
    school: nature
    target_type: ally
    hot_duration_ms: 15000
    notes: instant heal + 15s HoT; grants 2 charges of Tidal Waves; consumed by Chain Heal on the riptide-target for +25% effect

  CHAIN_HEAL:
    id: 1064
    mana_pct: 20
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: nature
    target_type: ally
    jumps: 3
    jump_falloff_pct: 30      # each jump heals 30% less than previous
    jump_radius: 12
    notes: smart-jumps to lowest-HP allies within 12y; consumes Riptide HoT on primary target for +25%

  EARTH_SHIELD:
    id: 974
    mana_pct: 19
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    duration_ms: 600000        # 10 min buff
    charges: 9
    target_type: ally          # typically the tank
    notes: damage-reactive HoT — heals target when struck; consumes 1 charge per proc; only one shaman's ES can be on a target

  HEALING_STREAM_TOTEM:
    id: 5394
    buff_id: 52042
    mana_pct: 3
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    gcd_ms: 1000               # totems use the 1s totem GCD
    range: 0
    target_type: ground_at_self
    duration_ms: 60000         # 1 min totem (Cata standard)
    notes: passive party HoT pulse; keep up always when in combat

  MANA_TIDE_TOTEM:
    id: 16190
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 180000
    on_gcd: true
    gcd_ms: 1000
    range: 0
    target_type: ground_at_self
    duration_ms: 12000
    notes: 12s; +400% Spirit-derived mana regen to party while totem alive

  SPIRIT_LINK_TOTEM:
    id: 98008
    linked_effect_id: 98021
    mana_pct: 11
    cast_time_ms: 0
    cooldown_ms: 180000
    on_gcd: true
    gcd_ms: 1000
    range: 0
    target_type: ground_at_self
    duration_ms: 6000
    radius: 10
    notes: every 1s for 6s, equalizes hp_pct of allies within 10y of totem; also -10% damage taken; signature emergency CD

  SPIRITWALKER_GRACE:
    id: 79206
    mana_pct: 12
    cast_time_ms: 0
    cooldown_ms: 120000
    on_gcd: false              # off-GCD
    range: 0
    target_type: self
    duration_ms: 15000
    notes: cast-while-moving for 15s; use proactively before scripted boss movement phases

  EARTHLIVING_WEAPON:
    id: 51730
    proc_buff_id: 51945
    mana_pct: 9
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: weapon_mainhand
    duration_ms: 1800000       # 30 min imbue
    proc_chance_pct: 20
    notes: 20% chance on direct heal to apply a 12s HoT (4 ticks) on the heal target

  WATER_SHIELD:
    id: 52127
    mana_pct: 4
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 600000        # 10 min
    orb_count: 3
    notes: passive mana regen; each orb consumed on hit returns mana; UNVERIFIED 4.3.4 mana-per-orb value — confirm via DBC

  EARTH_ELEMENTAL_TOTEM:
    id: 2062
    mana_pct: 24
    cast_time_ms: 0
    cooldown_ms: 600000        # 10 min
    on_gcd: true
    gcd_ms: 1000
    range: 0
    target_type: ground_at_self
    duration_ms: 120000
    notes: pet elemental tank; emergency threat / off-tank backup

  CLEANSE_SPIRIT:
    id: 51886
    mana_pct: 14
    cast_time_ms: 0
    cooldown_ms: 0                # Cata removed CD when Cleansing Waters talented
    on_gcd: true
    range: 40
    target_type: ally
    notes: removes Curse (and Magic with Improved Cleanse Spirit talent); with Cleansing Waters talent, dispel triggers a small heal on target

  NATURES_SWIFTNESS:
    id: 16188
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 120000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 8000
    notes: next nature spell <8s becomes instant + +25% healing; pair with GREATER_HEALING_WAVE on tank

  TREMOR_TOTEM:
    id: 8143
    mana_pct: 2
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: true
    gcd_ms: 1000
    range: 0
    target_type: ground_at_self
    duration_ms: 6000
    notes: pulses to break Fear/Charm/Sleep on party within radius; reactive utility

  WIND_SHEAR:
    id: 57994
    mana_pct: 8
    cast_time_ms: 0
    cooldown_ms: 6000          # UNVERIFIED — Cata-Classic re-tuning shows 15s; original 4.3.4 widely cited as 6s. Confirm via DBC.
    on_gcd: false
    range: 25
    target_type: enemy
    notes: interrupt + 2s school lockout; primary interrupt

  # ─────────────────────────────────────────────────
  # Non-encounter spells (solo combat / travel / utility)
  # ─────────────────────────────────────────────────

  LIGHTNING_BOLT:
    id: 403
    mana_pct: 6
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: nature
    target_type: enemy
    notes: primary single-target offensive for solo content; low DPS for Resto (no Elemental cast-time/crit talents)

  FLAME_SHOCK:
    id: 8050
    mana_pct: 17
    cast_time_ms: 0
    cooldown_ms: 6000          # shared shock CD with EARTH_SHOCK / FROST_SHOCK
    on_gcd: true
    range: 25
    school: fire
    target_type: enemy
    duration_ms: 18000          # base DoT duration; extended by Glyph of Flame Shock
    notes: best DPS-per-mana for Resto solo; DoT continues while bot moves / Ghost-Wolfs

  EARTH_SHOCK:
    id: 8042
    mana_pct: 18
    cast_time_ms: 0
    cooldown_ms: 6000          # shared shock CD
    on_gcd: true
    range: 25
    school: nature
    target_type: enemy
    notes: instant Nature damage; uses shared shock CD

  FROST_SHOCK:
    id: 8056
    mana_pct: 18
    cast_time_ms: 0
    cooldown_ms: 6000          # shared shock CD
    on_gcd: true
    range: 25
    school: frost
    target_type: enemy
    slow_duration_ms: 8000
    slow_pct: 50
    notes: 50% slow 8s; primary kite tool in solo

  CHAIN_LIGHTNING:
    id: 421
    mana_pct: 26
    cast_time_ms: 2000
    cooldown_ms: 3000
    on_gcd: true
    range: 30
    school: nature
    target_type: enemy
    jumps: 3
    notes: AoE chain; only worth casting on 3+ targets; mana-expensive for Resto

  EARTHQUAKE:
    id: 61882
    mana_pct: 60
    cast_time_ms: 2500
    cooldown_ms: 10000
    on_gcd: true
    range: 35
    school: nature
    target_type: ground_targeted
    duration_ms: 10000
    tick_interval_ms: 1000
    notes: persistent ground AoE; NOT channeled — bot can move/cast after the 2.5s hard-cast lands; mana-prohibitive for Resto solo (only use on dense add packs)

  GHOST_WOLF:
    id: 2645
    mana_pct: 6
    cast_time_ms: 2000
    cooldown_ms: 0
    on_gcd: true
    range: 0
    school: nature
    target_type: self
    speed_pct: 30
    notes: out-of-combat travel form; +30% run speed; breaks on melee hits taken

  LIGHTNING_SHIELD:
    id: 324
    mana_pct: 0                 # 4.3.4: cost is zero (changed from earlier expansions)
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    school: nature
    target_type: self
    duration_ms: 600000         # 10 min
    charges: 3
    notes: alternative to WATER_SHIELD; deals damage on hit. Resto default is WATER_SHIELD (mana regen via orbs is more valuable). Lightning Shield only when explicitly damage-prioritized in solo.

  ASTRAL_RECALL:
    id: 556
    mana_pct: 5
    cast_time_ms: 10000
    cooldown_ms: 900000         # 15 minutes (Wowhead Cata 4.3.4 verified — NOT the 60-min Vanilla/TBC value)
    on_gcd: true
    range: 0
    school: nature
    target_type: self
    notes: teleport to bind point; cannot cast in combat or while shapeshifted (incl. GHOST_WOLF); emergency disengage

  REINCARNATION:
    id: 20608
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 1800000        # 30 min
    on_gcd: false               # 0s GCD — chains with immediate post-rez action
    range: 0
    school: nature
    target_type: self
    requires_dead: true
    consumes_reagent: ankh
    notes: self-rez; cast while dead; off-GCD
```

---

## Maintenance pass

This pass runs **before** the priority rotation each tick. None of it is reactive throughput — it's the always-on substrate. Skip an entry if its predicate is false.

```yaml
maintenance:
  - name: Earthliving Weapon imbue
    when:
      - aura.missing(WEAPON_MAINHAND, EARTHLIVING_WEAPON)
      - not in_combat
    cast: EARTHLIVING_WEAPON

  - name: Water Shield self-buff
    when:
      - aura.missing(SELF, WATER_SHIELD)
    cast: WATER_SHIELD

  - name: Earth Shield on tank
    when:
      - in_combat OR boss_pull_imminent
      - target_resolves(TANK)
      - aura.missing(TANK, EARTH_SHIELD) OR aura.charges(TANK, EARTH_SHIELD) <= 3
    cast: EARTH_SHIELD on TANK

  - name: Healing Stream Totem
    when:
      - in_combat
      - totem.water_slot.empty OR totem.water_slot.expires_in_ms < 5000
    cast: HEALING_STREAM_TOTEM
    cooldown_self: 1000        # don't recast within 1s of placement
```

---

## Rotation — Priority List

Tiers are evaluated top-down. The first tier whose predicate evaluates true selects the cast. Only one cast per tick (the AI loop is 1000ms in `AltbotAI::Update`; spells finish on their own cast timer asynchronously). If the bot is currently casting a spell, the tick is a no-op unless tier 1 (self-preservation) interrupts.

```yaml
rotation:
  # ───────── Tier 1: self-preservation ─────────
  - tier: 1
    name: Self-save (Healing Surge SELF, instant via NS if needed)
    when:
      - SELF.hp_pct < 35
      - not currently_casting OR currently_casting.spell_id != HEALING_SURGE
    use_cooldown_first:
      - condition: spell.off_cooldown(NATURES_SWIFTNESS) AND SELF.hp_pct < 25
        cast: NATURES_SWIFTNESS
    cast: HEALING_SURGE on SELF

  # ───────── Tier 2: tank emergency ─────────
  - tier: 2
    name: Tank emergency burst
    when:
      - target_resolves(TANK)
      - TANK.hp_pct < 40
    use_cooldown_first:
      - condition: spell.off_cooldown(NATURES_SWIFTNESS) AND TANK.hp_pct < 30
        cast: NATURES_SWIFTNESS
        next_cast: GREATER_HEALING_WAVE on TANK
    cast: HEALING_SURGE on TANK
    notes: prefer Healing Surge here — Tidal Waves grants +30% crit which feeds Resurgence mana

  # ───────── Tier 3: Riptide on cooldown (always) ─────────
  - tier: 3
    name: Riptide on CD — tank preferred, fall back to lowest-HP injured
    when:
      - spell.off_cooldown(RIPTIDE)
      - any of:
          - target_resolves(TANK) AND (aura.missing(TANK, RIPTIDE) OR aura.expires_in_ms(TANK, RIPTIDE) < 3000)
          - target_resolves(PARTY_LOWEST_MISSING_HP) AND PARTY_LOWEST_MISSING_HP.hp_pct < 90
    target_resolution:
      - prefer: TANK if aura.missing(TANK, RIPTIDE) OR aura.expires_in_ms(TANK, RIPTIDE) < 3000
      - fallback: PARTY_LOWEST_MISSING_HP
    cast: RIPTIDE on resolved_target
    notes: |
      Consensus across 5 surveyed guides: keep Riptide always on cooldown — it generates
      2 Tidal Waves charges, applies a 15s HoT, and amplifies a subsequent Chain Heal on
      the same target by +25%. Tank target is a starting bias, not a hard gate. If the
      tank's HoT is healthy, fire on the most-injured ally to keep the engine running.

  # ───────── Tier 4: emergency raid CD ─────────
  - tier: 4
    name: Spirit Link Totem (emergency equalize)
    when:
      - spell.off_cooldown(SPIRIT_LINK_TOTEM)
      - party.count_within(10, hp_pct_lt=40) >= 3
      - party.spread_within(10) >= 3       # at least 3 allies are within 10y of each other
    cast: SPIRIT_LINK_TOTEM at GROUND_AT_SELF
    notes: signature 4.3 emergency cooldown — only fires when party is clustered AND multiple low

  # ───────── Tier 5: group AoE smart heal ─────────
  - tier: 5
    name: Chain Heal on AoE cluster
    when:
      - target_resolves(CHAIN_HEAL_PRIMARY)
      - party.injured_count(radius=12, hp_pct_lt=85) >= 3
    cast: CHAIN_HEAL on CHAIN_HEAL_PRIMARY
    notes: prefer a Riptide-tagged target as primary for the +25% bonus; CHAIN_HEAL_PRIMARY selector should prefer aura.active(target, RIPTIDE)

  # ───────── Tier 6: heavy single-target heal ─────────
  - tier: 6
    name: Greater Healing Wave on critical ally
    when:
      - target_resolves(PARTY_LOWEST_HP)
      - PARTY_LOWEST_HP.hp_pct < 60
      - aura.active(SELF, TIDAL_WAVES)     # only when Tidal Waves makes it cheap-cast
      - SELF.mana_pct > 50                 # never cast GHW under conserve threshold
    cast: GREATER_HEALING_WAVE on PARTY_LOWEST_HP

  # ───────── Tier 7: non-tank ally emergency ─────────
  - tier: 7
    name: Healing Surge emergency on non-tank ally
    when:
      - target_resolves(PARTY_LOWEST_HP)
      - PARTY_LOWEST_HP.hp_pct < 50
      - SELF.mana_pct > 35
    cast: HEALING_SURGE on PARTY_LOWEST_HP
    notes: |
      Consensus across 4 of 5 surveyed guides: Healing Surge is reserved for emergencies,
      NOT used as a routine Tidal-Waves consumer (mana efficiency too poor). Tier 1/2 cover
      self and tank emergencies; this tier covers non-tank allies in serious trouble.
      Healing Wave (tier 8) is the default Tidal-Waves consumer for spot heals.

  # ───────── Tier 8: efficient filler / default TW consumer ─────────
  - tier: 8
    name: Healing Wave filler — default Tidal Waves consumer
    when:
      - target_resolves(PARTY_LOWEST_HP)
      - PARTY_LOWEST_HP.hp_pct < 90
    cast: HEALING_WAVE on PARTY_LOWEST_HP
    notes: |
      Mana-efficient default heal. This is the spell that should consume Tidal Waves charges
      in normal play (Tidal Waves -30% cast time makes it ~2.1s with the buff). HSurge in
      tier 7 only fires for actual emergencies, regardless of TW state.

  # ───────── Tier 9: support DPS (only when nothing needs healing AND mana comfortable) ─────────
  - tier: 9
    name: Support DPS — Flame Shock / Earth Shock / Lightning Bolt
    when:
      # ALL conditions must hold — every tier above this must have failed to fire
      - target_resolves(ENEMY_TARGET)              # bot has an enemy target (boss / focus)
      - SELF.mana_pct >= 75                         # below this, conserve for healing
      - party.lowest_hp_pct >= 95                   # nobody is even slightly hurt
      - aura.active(TANK, EARTH_SHIELD)             # tank's main maintenance buff is up
      - HEALING_STREAM_TOTEM is active or on_cooldown_recent < 5s
      - not currently_casting
      - no_predicted_damage_in_next_2s              # if encounter flags scripted damage incoming, suppress

    cast_priority:
      # Within this tier, prefer instants so the bot can react to incoming damage mid-cast
      - condition: aura.missing(ENEMY_TARGET, FLAME_SHOCK) OR aura.expires_in_ms(ENEMY_TARGET, FLAME_SHOCK) < 3000
        cast: FLAME_SHOCK on ENEMY_TARGET           # instant DoT — apply/refresh
      - condition: spell.off_cooldown(SHOCK_SHARED) AND aura.active(ENEMY_TARGET, FLAME_SHOCK)
        cast: EARTH_SHOCK on ENEMY_TARGET           # instant nuke — uses shared shock CD
      - condition: SELF.mana_pct >= 85 AND no_predicted_damage_in_next_3s
        cast: LIGHTNING_BOLT on ENEMY_TARGET        # 2.5s hard-cast — only when truly safe

    cancel_conditions:
      # Mid-cast cancellation rules — applies to LIGHTNING_BOLT specifically
      - cancel_if: any of tiers 1-3 becomes true (self / tank / non-tank emergency)
      - cancel_if: time-critical dispel becomes pending
      - cancel_if: SELF.mana_pct drops below 70 mid-cast

    notes: |
      Healing remains priority 1 — this tier ONLY fires when every higher tier
      resolves to nothing.

      Telluric Currents (Resto tree, tier 5, 2 ranks — 20%/40% of Lightning Bolt
      damage returned as mana) makes LIGHTNING_BOLT mana-positive against damaged
      enemies. With 2/2 Telluric Currents talented, the support-DPS tier is mana-
      neutral or slightly mana-positive at typical dungeon damage levels — turning
      idle GCDs into mana sustain rather than mana drain.

      Without Telluric Currents talented: still defensible for low-mana-cost
      instants (Flame Shock + Earth Shock), but skip Lightning Bolt — its 2.5s
      cast time + 6% base mana cost without return makes it a net-negative
      mana drain.
```

**Determinism property** the implementation must preserve: given a fixed snapshot of `(self.hp, self.mana, party HP, active auras, active CDs, in_combat)`, exactly one tier resolves to a cast (or zero tiers, in which case the bot idles). No two tiers should overlap on the same input snapshot. If this is ever violated by a tuning change, treat it as a bug.

---

## Cooldown Profile

Major cooldowns the rotation does not fire automatically. These are evaluated in their own pass between maintenance and the priority rotation; if any fires, the rotation tick is consumed by it.

```yaml
cooldowns:
  - spell: MANA_TIDE_TOTEM
    when:
      - in_combat
      - SELF.mana_pct < 60
      - totem.water_slot is HEALING_STREAM_TOTEM       # only swap stream → tide if stream up; manage-water-slot logic
      - not aura.party_has(MANA_TIDE_TOTEM_BUFF)       # don't double up if another shaman fired theirs
    rationale: |
      12s window of +400% Spirit regen for the party. Surveyed guides describe the trigger
      as "fire when the raid will benefit" rather than a numeric threshold; Icy Veins
      specifically suggests party-mana<80% as a marker. The bot uses self.mana<60% as a
      single-actor proxy for "the encounter has been going long enough that mana matters."
      Tunable; not a derived consensus value.

  - spell: SPIRITWALKER_GRACE
    when:
      - moving.self == true
      - currently_casting AND currently_casting.cast_time_ms > 1500
    rationale: |
      Surveyed guides describe SWG reactively ("if you need to move while casting"). The
      spec uses the same trigger. A future enhancement could fire SWG proactively when a
      movement event is predicted; that's bot-specific upgrade, not guide consensus.

  - spell: NATURES_SWIFTNESS
    when:
      - bound to tier 1 / tier 2 (see use_cooldown_first blocks)
    rationale: |
      Never fire NS without an immediate consumer. Spec pairs NS with empowered GHW on
      tank emergency (matches Icy Veins). An alternate pairing — NS + Chain Heal for an
      AoE-cluster panic moment — is also defensible (wowtbc.gg recommends this in 5-man
      / clustered-heal contexts). Currently spec uses NS+GHW only; consider adding
      NS+CHAIN_HEAL as a tunable second pairing if cluster-emergency events become
      common enough to warrant it.

  - spell: EARTH_ELEMENTAL_TOTEM
    when:
      - context == raid
      - TANK.hp_pct < 25
      - spell.on_cooldown(SPIRIT_LINK_TOTEM)
    rationale: last-ditch threat soak when Spirit Link is unavailable; not a 5-man cooldown
```

---

## Proc Reactions

```yaml
procs:
  TIDAL_WAVES:
    buff_id: 53390
    triggered_by: [RIPTIDE_cast, CHAIN_HEAL_cast]
    charges: 2
    duration_ms: 15000
    consumed_by: [HEALING_WAVE, GREATER_HEALING_WAVE, HEALING_SURGE]
    bonus_for_HW_GHW: cast_time_pct -30
    bonus_for_HEALING_SURGE: crit_pct +30
    ai_behavior: |
      The rotation already prefers Tidal Waves consumers in tiers 6/7 — no extra
      reaction logic needed. Pure book-keeping: track aura state, decrement charges
      on consumer cast.

  RESURGENCE:
    talent_id: 101033
    requires: aura.active(SELF, WATER_SHIELD)
    triggered_by: crit on direct heal (HW, GHW, HSurge, Riptide initial, Chain Heal)
    effect: returns mana to caster
    ai_behavior: |
      No active reaction. Use as input to mana_pct floor calculation —
      under heavy crit the bot can sustain a slightly more aggressive throughput
      profile. UNVERIFIED 4.3.4 per-spell return values — confirm via DBC row 101033.

  EARTHLIVING_HOT:
    proc_buff_id: 51945
    triggered_by: direct heals on a target while EARTHLIVING_WEAPON imbue active
    proc_chance_pct: 20
    duration_ms: 12000
    ai_behavior: |
      No active reaction; passive throughput. Bot does NOT need to track this on
      individual targets unless a future tuning pass uses HoT presence as a tier
      condition (currently unused).
```

---

## Mana Management

```yaml
mana:
  thresholds:
    drink_floor_pct: 30          # below this, force drinking when out of combat
    drink_resume_pct: 95         # stop drinking once at/above this
    mana_tide_floor_pct: 60      # fire Mana Tide when self mana drops below this
    conserve_mode_pct: 50        # below this, downshift the rotation
    crisis_mode_pct: 25          # below this, only tier 1-3 + filler HW; pop mana_potion

  conserve_behavior:
    when: SELF.mana_pct < 50
    rules:
      - skip tier 6 (GREATER_HEALING_WAVE) entirely
      - HEALING_SURGE remains restricted to tier 1/2/7 emergency tiers only; do NOT use it as a routine Tidal Waves consumer regardless of mana state
      - tier 8 HEALING_WAVE is the default TW consumer in conserve mode (and in normal play — surveyed guides agree HSurge is emergency-only on mana grounds)

  crisis_behavior:
    when: SELF.mana_pct < 25
    rules:
      - skip tier 5 (CHAIN_HEAL) unless party.injured_count >= 4
      - skip tier 6 entirely
      - tier 7 forced to HEALING_WAVE
      - fire MYTHICAL_MANA_POTION if off cooldown
      - fire MANA_TIDE_TOTEM regardless of party-stack rules

  out_of_combat:
    drink_when:
      - not in_combat
      - SELF.mana_pct < drink_floor_pct
      - no enemies within 40y for 5000ms
    eat_when: out_of_combat AND SELF.hp_pct < 80
```

---

## Consumables

```yaml
consumables:
  flask:
    item: Flask_of_the_Draconic_Mind
    item_id: 58086
    apply: pre_combat AND not aura.active(SELF, FLASK_OF_THE_DRACONIC_MIND)
    rationale: +300 intellect — best-in-slot for Resto throughput

  food:
    primary:
      item: Seafood_Magnifique_Feast
      item_id: 62290
      buff_stat: +90 to chosen primary stat (intellect for Resto) at the feast level
    fallback:
      item: Severed_Sagefish_Head
      item_id: 62669
      buff_stat: +90 intellect / +90 spirit
    apply: pre_combat AND not aura.active(SELF, well_fed_intellect)

  mana_potion:
    item: Mythical_Mana_Potion
    item_id: 57192
    use_when:
      - in_combat
      - SELF.mana_pct < 25
      - spell.on_cooldown(MANA_TIDE_TOTEM)
      - cooldown_shared_potion.off_cooldown   # potion CD is per-combat in 4.3.4

  healthstone:
    item: Healthstone
    item_id: 36892
    use_when:
      - SELF.hp_pct < 30
      - not currently_casting OR currently_casting.spell_id != HEALING_SURGE
      - silenced OR cant_cast_for_ms > 1500
```

---

## Non-encounter content

This section covers behavior when the bot is NOT in a dungeon/raid encounter — questing, world combat, travel, OOC maintenance, idle. Encounter overrides (per-dungeon, per-boss) live in `docs/dungeons/{dungeon}/healer.md`; this is the baseline for any context where no specific encounter profile is loaded. Cross-class role-level patterns (kite logic, AoE thresholds, etc.) live in `docs/roles/healer.md`.

### Solo combat — offensive rotation

When the bot has an active enemy target and no friendly heal target requiring attention (questing, killing world mobs, soloing rares), it shifts to an offensive role with self-heal-as-needed.

```yaml
solo_offensive_rotation:
  # Tiers evaluated top-down. First true tier fires.

  - tier: 1
    name: Self-preservation (break offensive cast)
    when:
      - SELF.hp_pct < 35
    cast: HEALING_SURGE on SELF
    notes: same as encounter tier 1; interrupts current cast if any

  - tier: 2
    name: Self-heal mid-fight
    when:
      - SELF.hp_pct < 60
      - aura.missing(SELF, RIPTIDE) OR aura.expires_in_ms(SELF, RIPTIDE) < 3000
      - spell.off_cooldown(RIPTIDE)
    cast: RIPTIDE on SELF
    notes: instant; HoT continues while bot resumes Lightning Bolt

  - tier: 3
    name: Maintain Flame Shock DoT on target
    when:
      - target.hp_pct > 25                     # not worth applying on near-dead targets
      - aura.missing(target, FLAME_SHOCK) OR aura.expires_in_ms(target, FLAME_SHOCK) < 3000
      - spell.off_cooldown(SHOCK_SHARED)       # shared shock CD
    cast: FLAME_SHOCK on target

  - tier: 4
    name: AoE on dense pulls
    when:
      - enemies_in_radius(target, 10) >= 3
      - SELF.mana_pct > 50                     # CL/Earthquake are expensive
    cast: CHAIN_LIGHTNING on target
    notes: prefer CHAIN_LIGHTNING (cheaper than EARTHQUAKE); fall back to EARTHQUAKE only if 5+ targets and mana > 70%

  - tier: 5
    name: Shock filler (Earth Shock)
    when:
      - spell.off_cooldown(SHOCK_SHARED)
      - aura.active(target, FLAME_SHOCK)       # don't blow shock CD if FS still needs refresh
    cast: EARTH_SHOCK on target

  - tier: 6
    name: Frost Shock for kite
    when:
      - target.hp_pct > 0
      - target.distance_from_self < 8 AND target.is_melee
      - SELF.hp_pct < 70
      - spell.off_cooldown(SHOCK_SHARED)
    cast: FROST_SHOCK on target
    notes: 50% slow 8s; chain with Ghost Wolf disengage

  - tier: 7
    name: Lightning Bolt filler
    when:
      - target.hp_pct > 0
      - SELF.mana_pct > 20                     # crisis floor
    cast: LIGHTNING_BOLT on target
    notes: primary filler in solo; low DPS but cheap and reliable

  - tier: 8
    name: Out-of-mana wand attack / melee
    when:
      - SELF.mana_pct <= 20
    cast: WAND_AUTO_ATTACK or AUTO_ATTACK
    notes: bot fallback when mana-starved; let regen catch up

solo_self_heal:
  thresholds:
    riptide_self_threshold: 60                 # cast RIPTIDE on self
    healing_surge_self_threshold: 35           # emergency self-burst
    natures_swiftness_self_threshold: 25       # NS + Healing Surge if HP critical and NS off CD

  earth_shield_self:
    apply_when: in_combat AND SELF.hp_pct < 80 AND aura.missing(SELF, EARTH_SHIELD)
    notes: Earth Shield can be self-cast in solo; absorbs damage proactively
```

### Travel & out-of-combat

```yaml
travel:
  ghost_wolf:
    use_when:
      - not in_combat
      - distance_to_destination > 15
      - distance_to_destination < 80           # for longer trips, prefer mount
      - aura.missing(SELF, GHOST_WOLF)
    cast: GHOST_WOLF on SELF
    notes: bot's primary travel buff; +30% run speed; breaks on melee hit but unaffected by ranged casts

  mount:
    use_when:
      - not in_combat
      - distance_to_destination >= 80
      - mount_zone_allowed                     # not in instances, BGs, or no-mount zones
    notes: mount details out-of-scope for this doc; AltbotMount logic handles it

  dismount_priority:
    - immediate dismount if combat starts (game-side automatic)
    - manual dismount when within 15y of destination

ooc_maintenance:
  # Runs on AltbotAI tick when not in_combat
  - check: aura.missing(WEAPON_MAINHAND, EARTHLIVING_WEAPON)
    action: cast EARTHLIVING_WEAPON
  - check: aura.missing(SELF, WATER_SHIELD)
    action: cast WATER_SHIELD
    notes: Resto default — DO NOT swap to LIGHTNING_SHIELD even in solo (mana regen via orbs > on-hit damage for Resto throughput goals)
  - check: SELF.mana_pct < 80 AND not enemies_within(40)
    action: drink (consumable rules in Consumables section)
  - check: SELF.hp_pct < 95 AND not enemies_within(40)
    action: eat
```

### Idle

```yaml
idle:
  # When the bot has no active target, no heal target, no travel destination
  primary_action: follow master (handled by AltbotFollow.cpp)
  secondary_actions:
    - maintain WATER_SHIELD aura
    - maintain EARTHLIVING_WEAPON imbue
    - drink/eat to full if below maintenance thresholds
    - face master if in_party (cosmetic, but matches expected idle behavior)
  do_not:
    - do not cast offensive spells while idle (no aggro pulls)
    - do not engage neutral mobs unprompted
    - do not toggle GHOST_WOLF while idle (drain mana for nothing)
```

### World boss / elite scaling

```yaml
world_boss_or_elite:
  # When the bot is engaged with an enemy that's significantly above its level
  # (boss-flagged target, elite mob with HP > 5x normal mob, world boss zone)
  detection_signals:
    - target.classification == elite OR target.classification == world_boss
    - target.hp_max > 5 * SELF.hp_max
    - zone.is_world_boss_area

  rotation_override:
    if_in_party_with_tank_or_healer:
      use: encounter rotation (priority list above)
      thresholds:
        tier_2_tank_emergency: tank.hp_pct < 50    # raid-like cushion
        chain_heal_trigger: party.injured_count(radius=12, hp_pct_lt=85) >= 2
        crisis_mode_pct: 30
    if_solo:
      use: solo_offensive_rotation
      thresholds:
        riptide_self_threshold: 70                 # raised — elite damage is higher
        healing_surge_self_threshold: 50           # raised — burst earlier
        do_not_engage_unless_can_solo: true        # bot retreats if elite > 8 levels above

  cooldown_use:
    in_party: full encounter cooldown profile (Mana Tide, Spirit Link conditional)
    solo: NATURES_SWIFTNESS reactive on HP <30%; do not waste Mana Tide solo (no party benefit)
```

### Disengage / emergency utility

```yaml
disengage:
  # Bot escape behaviors when overwhelmed in solo content
  reincarnation:
    use_when:
      - SELF.is_dead
      - not in_party_combat                       # don't rez mid-wipe
      - safe_distance_from_recent_attacker        # avoid rez-then-die loop
      - reagent.ankh.available
    cast: REINCARNATION
    notes: off-GCD; can chain with immediate post-rez Healing Surge self

  astral_recall:
    use_when:
      - SELF.hp_pct < 30
      - not in_combat                              # cannot cast in combat
      - distance_to_pursuer > 20                   # need a clean window
      - bind_point.zone != current.zone OR bind_point.distance > 200
    cast: ASTRAL_RECALL
    notes: 10s cast — vulnerable during cast; only fire when truly clear of pursuit

  ghost_wolf_break:
    use_when:
      - SELF.hp_pct < 50
      - in_combat
      - target.is_melee AND target.hp_pct > SELF.hp_pct
    cast: GHOST_WOLF on SELF
    notes: combat-castable but melee hits will break form; chain with FROST_SHOCK for slow
```

### Spec doc template note

Future spec docs (`holy-paladin.md`, `holy-priest.md`, etc.) follow the same shape as this doc. Required sections in order:

1. Identity
2. Target taxonomy
3. Spell Catalog (encounter spells + non-encounter spells)
4. Maintenance pass
5. Rotation — Priority List (encounter)
6. Cooldown Profile
7. Proc Reactions
8. Mana Management
9. Consumables
10. **Non-encounter content** (this section — solo rotation, travel, idle, world-boss scaling, disengage)
11. **Leveling rotation** (next section — per-bracket rotations bracketed at spell-unlock breakpoints)
12. UNVERIFIED items
13. Open hooks for `AltbotCombat.cpp`

Role-level patterns (triage, kite logic, AoE thresholds) live in `docs/roles/{role}.md` and are referenced from the spec doc when applicable. Cross-class leveling-dungeon patterns (era-by-era differences, per-role progression by level, common mechanics) live in `docs/dungeons/leveling.md`.

---

## Leveling rotation

Per-bracket dungeon-healing rotations from level 1 → 85, bracketed at **spell-unlock breakpoints**. Bot's current rotation tier list is the bracket whose level range covers `pPlayer->getLevel()`.

Spell-unlock levels verified against Wowhead Cata Classic (`wowhead.com/cata/spell=<id>` → Required Level field). Strategy reconciled across 7 leveling-rotation guides — see `docs/research/resto-shaman-guide-survey.md` (Leveling section) for the audit trail.

### Spec recommendation

```yaml
spec_choice:
  for_dungeon_healer_bot: |
    Pure Restoration from level 10 → 85. The Cata Classic guide consensus is
    "Elemental for solo + Resto for dungeons (dual-spec)" — but our bot's mandate
    is dungeon healing, so solo-questing efficiency doesn't matter. Pure Resto
    keeps the rotation simple, the talent investment focused, and Riptide+Earth
    Shield available from level 10.
  for_solo_or_world_bot:
    consideration: |
      If the bot will quest extensively while leveling, dual-spec Elemental is
      more efficient for solo combat. Out-of-scope for the dungeon-first use case
      this doc was built for.
```

### Spell unlock table (Wowhead Cata Classic verified)

```yaml
spell_unlocks:
  # required_level fields per Wowhead Cata Classic tooltips, 2026-05 verification
  - LIGHTNING_BOLT (403): 1
  - EARTH_SHOCK (8042): 5
  - HEALING_WAVE (331): 7
  - LIGHTNING_SHIELD (324): 8
  - EARTH_SHIELD (974): 10        # spec ability, granted with Restoration spec at 10
  - RIPTIDE (61295): 10           # spec ability, granted with Restoration spec at 10
  - FLAME_SHOCK (8050): 14
  - GHOST_WOLF (2645): 15
  - WIND_SHEAR (57994): 16
  - CLEANSE_SPIRIT (51886): 18
  - HEALING_SURGE (8004): 20
  - HEALING_STREAM_TOTEM (5394): 20
  - WATER_SHIELD (52127): 20
  - FROST_SHOCK (8056): 22
  - CHAIN_LIGHTNING (421): 28
  - CHAIN_HEAL (1064): 40
  - TREMOR_TOTEM (8143): 52
  - EARTHLIVING_WEAPON (51730): 54     # NOT 30 — Cata pushed this up from WotLK
  - EARTH_ELEMENTAL_TOTEM (2062): 56
  - GREATER_HEALING_WAVE (77472): 68   # Wowhead Cata Classic — some legacy guides say 60 (likely MoP confusion); UNCERTAIN, treat 68 as authoritative
  - MASTERY_DEEP_HEALING (77226): 80   # Mastery as a stat activates at 80; gear Mastery rating is inert below this
  - SPIRITWALKERS_GRACE (79206): 85
  - EARTHQUAKE (61882): UNCERTAIN_LEVEL # Wowhead doesn't tooltip a clean required-level for this AoE
  # Talent-gated abilities (Resto-tree tier requirement; bot reaches tier N at character level 9 + 5*N if pure Resto)
  - NATURES_SWIFTNESS (16188): tier_3_talent (~level 19, pure Resto) — UNCERTAIN exact level
  - MANA_TIDE_TOTEM (16190): tier_5_talent (~level 29, pure Resto) — UNCERTAIN exact level
  - SPIRIT_LINK_TOTEM (98008): tier_6_talent (~level 34, pure Resto) — UNCERTAIN; may be baseline 85 ability instead — confirm via DBC
```

### Per-bracket rotation

```yaml
leveling_brackets:
  # Each bracket lists: available healing tools, single-target priority,
  # group-heal approach, mana mode, key milestones unlocked at the bracket entry.

  - bracket: 1-6
    name: pre_healing
    available: LIGHTNING_BOLT only (no Healing Wave yet)
    rotation: |
      Bot CANNOT function as a dungeon healer below level 7. Auto-attack +
      Lightning Bolt only. LFD bracket gating (level 15 minimum for any LFD
      dungeon) means this bracket is bot-irrelevant in practice.
    mana: N/A — no heals
    notes: keep this bracket only for solo/world combat; bots queueing for LFD start at 15

  - bracket: 7-9
    name: healing_wave_only
    available: HEALING_WAVE
    single_target: spam HEALING_WAVE on lowest-HP target
    group_heal: HEALING_WAVE on whoever is lowest; no AoE option
    mana: comfortable — low mob damage, downtime regens fully

  - bracket: 10-14
    name: spec_chosen
    available: + RIPTIDE + EARTH_SHIELD + LIGHTNING_SHIELD (still — Water Shield not until 20)
    single_target: |
      Tier 1: EARTH_SHIELD on tank (refresh on charges-out OR pre-pull if missing)
      Tier 2: RIPTIDE on tank if missing OR HoT < 3s remaining
      Tier 3: HEALING_WAVE filler on PARTY_LOWEST_HP
    group_heal: RIPTIDE on injured target → HEALING_WAVE chase; no smart-AoE yet
    mana: comfortable; RIPTIDE is most mana-efficient heal
    notes: |
      LFD bracket opens at 15 — bot can start dungeon-healing in this bracket.
      Talent allocation begins at 10 (1 pt / level).

  - bracket: 15-19
    name: utility_unlocks
    available: + GHOST_WOLF (15) + WIND_SHEAR (16) + CLEANSE_SPIRIT (18)
    single_target: same as 10-14
    group_heal: same; CLEANSE_SPIRIT now usable for Magic dispels (and Curse with Cleansing Waters talent if allocated)
    mana: comfortable
    new_responsibilities:
      - WIND_SHEAR is the bot's interrupt — backup melee on caster pulls
      - CLEANSE_SPIRIT for Magic / Curse dispels per role healer dispel taxonomy
      - GHOST_WOLF for travel between pulls (out-of-combat)

  - bracket: 20-27
    name: water_shield_and_surge
    available: + HEALING_SURGE (20) + HEALING_STREAM_TOTEM (20) + WATER_SHIELD (20) + FROST_SHOCK (22)
    single_target: |
      Tier 1: EARTH_SHIELD on tank
      Tier 2: HEALING_STREAM_TOTEM placed at start of combat
      Tier 3: RIPTIDE on tank on CD
      Tier 4: HEALING_SURGE if tank.hp_pct < 35   # emergency only
      Tier 5: HEALING_WAVE filler
    group_heal: HST + RIPTIDE + HW spam on lowest
    mana: |
      SWITCH FROM LIGHTNING_SHIELD TO WATER_SHIELD AT 20.
      Major mana-regen boost via Water Shield orb consumption. Conserve floor
      remains low — mana not yet a concern except in long fights.
    new_responsibilities:
      - drop HST on every pull entry; refresh when expiring
      - HEALING_SURGE strictly emergency (consensus: same rule applies as max-level)

  - bracket: 28-29
    name: chain_lightning_unlock
    available: + CHAIN_LIGHTNING (28)
    single_target: same as 20-27
    group_heal: same — still no Chain Heal
    mana: comfortable
    notes: CHAIN_LIGHTNING is offensive, not healing — useful for solo, ignored in dungeon healing rotation

  - bracket: 30-39
    name: mana_tide_talent_window
    available: |
      + MANA_TIDE_TOTEM (~tier-5 Resto talent, ~level 29-30 if pure Resto) — UNCERTAIN exact level
      + NATURES_SWIFTNESS (~tier-3 Resto talent, ~level 19-20) — already available since 19-20 if Resto-leveled, sometimes counted in this bracket
    single_target: same as 20-27
    cooldowns:
      - NATURES_SWIFTNESS reactive on tank emergency (paired with HEALING_WAVE if no GHW yet)
      - MANA_TIDE_TOTEM if available; cast on cooldown if any party caster is low mana
    mana: comfortable; MTT solves any group mana issue when available

  - bracket: 40-53
    name: chain_heal_window
    available: + CHAIN_HEAL (40)
    single_target: |
      Tier 1: EARTH_SHIELD on tank
      Tier 2: HEALING_STREAM_TOTEM at pull
      Tier 3: RIPTIDE on tank on CD
      Tier 4: HEALING_SURGE emergency tank (hp < 35) — paired with NS if off CD
      Tier 5: CHAIN_HEAL if 3+ allies <80% HP within 12y
      Tier 6: HEALING_WAVE filler
    group_heal: |
      CHAIN_HEAL becomes the AoE staple. Apply RIPTIDE to the chain-heal target
      first — Riptide+Chain Heal synergy via Tidal Waves (talented) and the
      Resto mastery pattern (active from 80).
    mana: |
      Comfortable but not free. Chain Heal is mana-heavy — don't spam on 1-2
      injured. Stick to single-target spam unless 3+ qualify.

  - bracket: 54-55
    name: earthliving_imbue_unlock
    available: + EARTHLIVING_WEAPON (54)
    single_target: same as 40-53
    new_maintenance:
      - EARTHLIVING_WEAPON imbue on main-hand pre-pull and refresh as needed
    notes: |
      Until 54, the bot SHOULD NOT have an Earthliving imbue (it doesn't exist yet).
      Pre-54 fallback imbue: ROCKBITER_WEAPON (early threat-assist) OR FLAMETONGUE
      (small spell-power buff for offensive casts) — bot-author judgment, no guide
      pins this down.

  - bracket: 56-59
    name: earth_elemental_unlock
    available: + EARTH_ELEMENTAL_TOTEM (56)
    single_target: same as 54-55
    cooldowns:
      - EET as panic tank-summon (5-man emergency, very rare use)
    notes: low impact at this bracket; matters more at 80-85 raid scaling

  - bracket: 60-67
    name: pre_ghw
    available: same kit
    single_target: same priority list — HEALING_WAVE remains the spam target heal until GHW unlocks at 68
    mana: |
      Mana tightening starts ~60+. BC-era leveling dungeons (Hellfire Ramparts,
      Slave Pens, etc.) are the bot's first real mana stress test. See
      docs/dungeons/leveling.md healer_progression block for the BC-era rules
      (Five Second Rule enforced, drink between pulls).

  - bracket: 68-79
    name: ghw_unlock
    available: + GREATER_HEALING_WAVE (68)
    single_target: |
      Tier 1: EARTH_SHIELD on tank
      Tier 2: HST at pull
      Tier 3: RIPTIDE on CD
      Tier 4: GREATER_HEALING_WAVE on tank when big hits incoming AND mana > 50%
      Tier 5: HEALING_SURGE emergency (hp < 35)
      Tier 6: HEALING_WAVE filler when comfortable
    group_heal: RIPTIDE on chain-heal-target → CHAIN_HEAL on cluster → HW filler
    mana: |
      Mana matters. Use HW as filler, GHW only when needed, HSurge only emergency.
      Drop MTT on cooldown when group benefits.

  - bracket: 80-84
    name: mastery_active
    available: + MASTERY: DEEP_HEALING (passive at 80)
    single_target: same as 68-79 priority
    new_property: |
      Mastery Deep Healing now ACTIVE — heals are stronger on lower-HP targets.
      Practical effect: favor letting tank dip slightly before casting GHW for
      the mastery boost. Don't pre-cast on full-HP targets.
    mana: matches max-level discipline — see Mana Management section above
    notes: |
      Cata leveling dungeons (Throne of the Tides, Blackrock Caverns, etc.)
      start at 80. Use the standard rotation from this doc's Rotation section
      with the threshold overrides documented in docs/dungeons/{dungeon}/healer.md.

  - bracket: 85
    name: full_kit
    available: + SPIRITWALKERS_GRACE (85) + SPIRIT_LINK_TOTEM (talent capstone — verify if baseline-85 instead via DBC)
    rotation: full max-level encounter rotation as defined in this doc's Rotation section
    notes: |
      Bot is at full power. All rotation tiers, cooldowns, proc reactions, and
      consumables apply per the main spec doc. Encounter overrides per-dungeon
      apply on top.
```

### Talent allocation order (Restoration tree, level 10 → 70)

```yaml
talent_path_resto:
  # Cata talent rules: 1 talent point per level starting at 10, 41 total points
  # by level 85. Tier N talent unlocks at (N-1)*5 points spent in tree.
  # Pure Restoration leveling reaches 31 pts in Resto by level 40.

  level_10_through_14:    # tier 1
    spend:
      - Spark of Life (3/3)
      - Tidal Focus (2/3)              # finish at 15
    rationale: 6% healing + mana cost reduction — broad foundational throughput

  level_15_through_19:    # tier 2
    spend:
      - Tidal Focus (3/3)
      - Resurgence (3/3)               # crit-on-direct-heal mana return
      - Totemic Focus (2/2)            # totem mana cost reduction
    rationale: mana-economy talents — Resurgence is THE Resto mana sustain mechanic

  level_20_through_24:    # tier 3
    spend:
      - Nature's Swiftness (1/1)       # instant cast — emergency tool
      - Focused Insight (3/3) OR Improved Cleansing Totem
    rationale: NS unlocks. Filler talents per personal preference.

  level_25_through_29:    # tier 4-5 prep
    spend:
      - Ancestral Healing (3/3)        # tank-heal damage reduction proc
      - Mana Tide Totem prerequisite path
    rationale: tank-survival talents become available

  level_30_through_34:    # tier 5-6
    spend:
      - Telluric Currents (2/2)        # Lightning Bolt returns 20%/40% damage as mana — enables tier 9 support DPS
      - Mana Tide Totem (or its prereq)
      - Soothing Rains (3/3)             # buffs HST + Healing Rain
    rationale: |
      AoE-heal + mana-CD foundation. Telluric Currents added to this tier (was
      previously erroneously flagged as "skip"): turns Lightning Bolt into a
      mana-positive cast against damaged enemies, enabling the tier 9 support-DPS
      rotation without compromising healing throughput. Mandatory for dungeon-
      running bots; only optional for raid-progression specs where every GCD is a heal.

  level_35_through_44:    # tier 6-7
    spend:
      - Tidal Waves (2/2)              # HEART OF THE SPEC — Riptide → 2 charges of HW/GHW cast-time reduction / HSurge crit
      - Blessing of the Eternals (2/2) # crit chance on Riptide-targeted heals
    rationale: Tidal Waves is the keystone Resto talent — DO NOT skip

  level_45_through_69:    # tier 8-10
    spend:
      - Ancestral Resolve (2/2)
      - Cleansing Waters (2/2)         # dispel triggers heal — Resto's Cleanse Spirit becomes a heal-dispel
      - Ancestral Awakening (3/3)      # crit heal triggers second smaller heal on lowest-HP ally
      - Riptide capstones
    rationale: late-tree polish; finishes at 31 pts in Resto

  level_70:
    spend:
      - Resto capstone (final tier 11 talent)
    rationale: 31st point reached; no further required Resto investment

  level_71_through_85:    # spend in second tree
    spend_in_elemental:
      - Acuity (3/3 crit)
      - Convection + Concussion (DPS talents — relevant only if bot off-DPS during downtime)
    rationale: 10 free points; Elemental tree is the safe second spend
```

### Glyph priorities (acquire in this order as bot levels)

```yaml
glyph_priorities:
  # Glyphs are level-gated by glyph SLOT, not by individual glyph level
  # In Cata Classic rules: glyph slots open at varying levels (25 / 50 / 75)
  # but the glyphs themselves can usually be installed once unlocked.

  prime_glyphs:
    # Prime glyph slot opens at level 25 (Cata Classic) — earlier than legacy
    1: Glyph of Riptide          # extends HoT — pick up first when prime slot opens
    2: Glyph of Earthliving Weapon  # passive heal-target HoT chance — pick up at 54+ when EW is learned
    3: Glyph of Earth Shield     # +20% Earth Shield healing

  major_glyphs:
    1: Glyph of Healing Stream Totem  # adds splash-heal — useful from 20+
    2: Glyph of Chain Heal       # extra jump — pick up at 40+ once Chain Heal unlocks
    3: Glyph of Stoneclaw Totem  # situational raid-utility absorb shield

  minor_glyphs:
    - Glyph of Water Walking
    - Glyph of Renewed Life
    - Glyph of Astral Recall
```

### Author-judgment defaults at low levels

These are bot-author defaults for thresholds no leveling guide pins down. Tunable per encounter / per bot config.

```yaml
leveling_author_defaults:
  - id: emergency_hp_threshold_pre_60
    value: SELF.hp_pct < 25 (was 35 at max level)
    rationale: low-level mob damage is sparse; lower threshold avoids reflex panic-heal

  - id: chain_heal_trigger_pre_60
    value: party.injured_count(radius=12, hp_pct_lt=80) >= 3
    rationale: same as 80% threshold default; tighter than max-level 85% because Chain Heal is mana-expensive at low gear levels

  - id: mana_floor_pre_60
    value: 0  # no conserve mode below level 60
    rationale: mana isn't tight pre-BC; conserve gates fire false-positives

  - id: mana_floor_60_to_70
    value: 55 (conserve) / 30 (crisis)
    rationale: BC-era mana stress is the highest in leveling; raise floors aggressively

  - id: mana_floor_70_to_80
    value: 45 (conserve) / 25 (crisis)
    rationale: WotLK regen overhauls relax the floor

  - id: mana_floor_80_plus
    value: matches max-level Mana Management section (50 / 25)
    rationale: Cata leveling = Cata max-level discipline

  - id: pre_earthliving_imbue_choice
    value: ROCKBITER_WEAPON
    rationale: |
      Until level 54 (Earthliving unlock), bot defaults to Rockbiter for the
      small threat-reduction proc. Flametongue (small spell-power) is a defensible
      alternate. No guide pins this down.

  - id: support_dps_during_leveling
    value: |
      level 7-29: weave LIGHTNING_BOLT freely between heals on slow tank-only pulls (mana not a concern; mob damage sparse)
      level 30+: support DPS gated on tier 9 conditions — only when no heal needed AND mana >= 75% AND tank Earth Shield up
      level 30+ (with Telluric Currents talented at level 30-34): LIGHTNING_BOLT becomes mana-positive at damaged enemies; full tier 9 active
      level 30+ (without TC talent): only FLAME_SHOCK + EARTH_SHOCK instants in tier 9; skip LIGHTNING_BOLT until TC
    rationale: |
      Earlier draft of this default said "stop weaving past level 30" — that was
      written before Telluric Currents was reincorporated into the talent path.
      With TC talented at 30-34, Lightning Bolt is mana-positive against damaged
      enemies, so the support-DPS tier becomes a mana-sustain mechanic rather
      than a mana drain. Bot's leveling flow: take TC at 30, enable tier 9 support
      DPS from that point onward.

  - id: water_shield_rebuff_threshold
    value: charges < 1 OR remaining_duration_ms < 60000
    rationale: keep Water Shield up during all combat

  - id: respec_during_leveling
    value: stay_resto_throughout
    rationale: |
      No guide recommends mid-leveling respecs for a dungeon-only Resto Shaman.
      Bot stays Resto from 10 to 85.

  - id: greater_healing_wave_required_level
    value: 68
    rationale: |
      Wowhead Cata Classic tooltip is authoritative. Some legacy guides say 60
      (probably MoP confusion). UNCERTAIN — confirm via Spell.dbc row 77472.
```

---

## UNVERIFIED items (confirm against TC `Spell.dbc`)

These values are best-effort from public archives but were inconsistent or absent in 4.3.4-specific sources. Pull the canonical values from the server's DBC before code references them as constants.

| Item | What's uncertain | Where to verify |
|---|---|---|
| `WIND_SHEAR.cooldown_ms` | Wowhead Cata page lists 15s (Cata-Classic tuning); original 4.3.4 widely cited as 6s | `Spell.dbc` row 57994 → `RecoveryTime` |
| `WATER_SHIELD.mana_per_orb` | 4.3.4 used a flat scaled value, not % base mana | `Spell.dbc` row 52127 effects |
| `RESURGENCE` per-spell mana return | Modern published values differ from original 4.3.4 | `Spell.dbc` row 101033 + `spell_proc_event` SQL |
| Healing Stream Totem `duration_ms` | Cata totems went to 60s but exact value should be DBC-confirmed | `Spell.dbc` row 5394 |
| `SPIRIT_LINK_TOTEM` baseline-vs-talent | Research agent flagged it as a tier-6 Resto talent (~level 35). Memory says it's a baseline level-85 ability granted with Resto spec at 85. Major rotation impact on whether SLT is in the 35-84 toolkit. | `Spell.dbc` row 98008 + spec-grant table OR talent table. Confirm before building level-gating logic. |
| `MANA_TIDE_TOTEM` baseline-vs-talent | Same as SLT — research agent says tier-5 Resto talent (~level 30); memory says baseline | Same approach as SLT verification |
| `NATURES_SWIFTNESS` baseline-vs-talent | Same — research agent says tier-3 Resto talent (~level 19-20); memory says baseline | Same approach |
| `GREATER_HEALING_WAVE.required_level` | Wowhead Cata Classic tooltip says 68; some legacy guides say 60 (likely MoP confusion). Treat 68 as authoritative pending DBC verification. | `Spell.dbc` row 77472 → `BaseLevel` / `SpellLevel` |
| `EARTHQUAKE.required_level` | Wowhead Cata Classic doesn't expose a clean required-level for this AoE (tooltip parser failed) | `Spell.dbc` row 61882 |

---

## Open hooks for `AltbotCombat.cpp`

When Phase 3 begins, the YAML blocks above map to concrete C++ structures:

| Doc block | Target structure |
|---|---|
| Spell Catalog | `static const SpellEntry kRestoShamanSpells[]` |
| Maintenance pass | `static const MaintenanceCheck kRestoShamanMaintenance[]` |
| Rotation priority list | `static const RotationTier kRestoShamanRotation[]` |
| Cooldown Profile | `static const CooldownTrigger kRestoShamanCooldowns[]` |
| Proc Reactions | book-keeping inside `AltbotCombat::OnAuraApplied/OnAuraRemoved` |
| Mana Management | `RestoShamanManaState` enum + threshold checks in tick entry |
| Consumables | shared `AltbotCombat::ConsumableUsageRule` table |

The condition predicates (`aura.active(...)`, `party.count_within(...)`, etc.) define the helper API the AI layer needs to expose. That API should be implemented once and reused across all spec specs.
