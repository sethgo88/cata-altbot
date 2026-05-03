# Discipline Priest — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide. The YAML blocks below are designed to lift cleanly into `static const` tables in a future `AltbotCombat.cpp`.

**Numerical data** (spell IDs, mana %, cast times, cooldowns, durations, item IDs) verified against Wowhead's Cata 4.3.4 archive and Warcraft Wiki. Items the AI will need but that public archives don't pin down with confidence are tagged `UNVERIFIED`.

**Strategy content** reconciled across 13 community guides. See `docs/research/disc-priest-guide-survey.md` for the comparison table and which thresholds are consensus vs author-judgment.

**SPECIAL CASE — Atonement Smite IS healing.** Discipline is the only healer where damage-dealing is *primary healing*, not Tier-9 support DPS. Atonement converts Smite/Holy Fire damage into healing on the lowest-HP ally within 15y of the enemy. This spec encodes Atonement Smite at **Tier 3** (between Penance/PW:Shield and direct heals), gated on cluster reachability and tank-priority abandon. Cross-link: `docs/roles/healer.md` Atonement-as-primary-healing exception.

---

## Identity

```yaml
spec: disc_priest
class: priest
role: HEALER
resource_model: mana
defining_mechanic: Atonement (Smite/HolyFire damage → 100% ally heal within 15y; 50% self)
defining_buffs:
  - Inner_Fire             # +SP +armor
  - Power_Word_Fortitude
  - Borrowed_Time          # +14% haste on next cast after PW:Shield
  - Grace                  # 9% healing-received stack on tank (3 stacks max)
  - Weakened_Soul          # debuff after PW:Shield, 15s lockout per target
  - Evangelism             # 5-stack damage/cost buff from Smite/HF/Penance
  - Archangel              # consume Evangelism stacks for +18s healing buff + mana
defining_procs:
  - Divine_Aegis           # crit-heal absorb shield (30% of healing)
  - Rapture                # mana return on PW:Shield expiry, 12s ICD
mastery: Shield_Discipline   # +% absorb on PW:Shield + Divine Aegis
key_cooldowns:
  - Pain_Suppression       # 3 min — tank emergency 40% damage reduction
  - Power_Word_Barrier     # 3 min — raid AoE 25% damage-reduction zone
  - Inner_Focus            # 45s — next big heal is free + 25% crit
  - Power_Infusion         # 2 min — +20% haste / -20% cost (self by default)
  - Hymn_of_Hope           # 6 min — channeled mana CD
  - Divine_Hymn            # 8 min — channeled raid heal
  - Shadowfiend            # 5 min — pet melee returns mana
  - Desperate_Prayer       # 2 min — instant 30% self-HP
ranged: 40
notes: |
  Discipline Priest in 4.3.4 plays as either a tank-focused shield healer
  (Shield-spec) or an Atonement-throughput healer (Atonement-spec). Bot
  defaults to Atonement-spec — Smite contributes raid damage AND heals via
  Atonement, with PW:Shield + Penance + PoM maintaining the tank.

  Rotation heartbeat in Atonement-spec: PW:Shield(tank) on Weakened-Soul-clear,
  Penance on CD (heal or Smite-assist), PoM on CD, Holy Fire on CD, Smite as
  filler with Atonement healing the cluster, Archangel at 5 Evangelism stacks.

  Shield-spec preset is encoded as a runtime swap for tank-heal-assignment
  encounters where Atonement reliability is too low (tank is the only injured
  target). Spec swap is a respec, not runtime.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  TANK:
  PARTY_LOWEST_HP:
  PARTY_LOWEST_MISSING_HP:
  ATONEMENT_CLUSTER_TARGET:    # enemy with ≥1 injured friendly within 15y
  POH_PARTY_TARGET:            # raid party where avg HP < 80
  ENEMY_TARGET:
  HOLY_FIRE_TARGET:            # enemy without active Holy Fire DoT (or refreshable)
```

---

## Spell Catalog

```yaml
spells:
  # ─────── Heal-side core ───────

  POWER_WORD_SHIELD:
    id: 17
    mana_pct: 25                # UNVERIFIED — see DBC queue
    cast_time_ms: 0
    cooldown_ms: 0              # 15s Weakened Soul lockout per target
    on_gcd: true
    range: 40
    target_type: ally
    duration_ms: 30000
    notes: |
      Tier 2 maintenance. With Rapture talent, returns 7% max mana on shield
      expiry (12s ICD). Borrowed Time talent grants +14% haste on next cast.
      Mastery: Shield Discipline scales the absorb amount.

  PENANCE:
    id: 47540
    mana_pct: 16
    cast_time_ms: 2000          # 3-tick channel; haste shortens channel only
    cooldown_ms: 8000           # 6s with Glyph of Penance
    on_gcd: true
    range: 40
    target_type: ally_or_enemy
    notes: |
      Smart heal/damage channel. Heals ally OR damages enemy (3 ticks).
      Penance on enemy stacks Evangelism + feeds Atonement healing.
      UNVERIFIED tick count fixed at 3 in 4.3.4 — confirm via DBC.

  HEAL:
    id: 2050
    mana_pct: 14
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    notes: cheap mana-efficient default heal (4.0 triage); Tier 9 filler

  GREATER_HEAL:
    id: 2060
    mana_pct: 32
    cast_time_ms: 3000
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    notes: heavy single-target heal; Empowered Healing talent +20%

  FLASH_HEAL:
    id: 2061
    mana_pct: 28
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    notes: emergency-only

  PRAYER_OF_HEALING:
    id: 596
    mana_pct: 38
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: party_of_target
    radius: 30
    notes: |
      Crit creates Divine Aegis absorb shield (30% of healing) on each target.
      Tier 6 — stacked-party damage trigger.

  PRAYER_OF_MENDING:
    id: 33076
    mana_pct: 9
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: true
    range: 40
    target_type: ally
    duration_ms: 30000
    charges: 5
    notes: bouncing HoT; on cooldown

  POWER_WORD_BARRIER:
    id: 62618
    mana_pct: 24
    cast_time_ms: 0
    cooldown_ms: 180000         # 3 min
    on_gcd: false
    range: 40
    target_type: ground_targeted
    duration_ms: 10000
    radius: 6                   # UNVERIFIED — confirm 6y or 8y
    notes: |
      25% damage-reduction zone on ground. Post-4.0.3a, NOT an absorb shield;
      Mastery does NOT scale this.

  PAIN_SUPPRESSION:
    id: 33206
    mana_pct: 8                 # Cata Classic value, UNVERIFIED
    cast_time_ms: 0
    cooldown_ms: 180000         # 3 min
    on_gcd: false
    range: 40
    target_type: ally
    duration_ms: 8000
    notes: 40% damage reduction on target. Tank-emergency CD.

  INNER_FOCUS:
    id: 14751
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 45000          # Cata 45s; reduced 5s by Train of Thought per Greater Heal cast
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 60000
    notes: next FlashHeal/GreaterHeal/BindingHeal/PoH = free + 25% crit

  POWER_INFUSION:
    id: 10060
    mana_pct: 16                # UNVERIFIED — Cata value
    cast_time_ms: 0
    cooldown_ms: 120000
    on_gcd: false
    range: 30
    target_type: ally           # bot self-targets by default
    duration_ms: 15000
    notes: +20% haste, -20% mana cost on spells

  ARCHANGEL:
    id: 87151
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 18000
    notes: |
      Consumes Evangelism stacks. +1% mana per stack restored, +3% healing
      done per stack for 18s. Fire at 5 stacks.

  EVANGELISM:
    talent_passive: true
    triggered_by: SMITE / HOLY_FIRE / PENANCE cast
    stacks: 5
    notes: builds via Smite/HolyFire/Penance; consumed by Archangel

  DESPERATE_PRAYER:
    id: 19236
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 120000
    on_gcd: false
    range: 0
    target_type: self
    notes: instant 30% max HP self-heal

  HYMN_OF_HOPE:
    id: 64901
    mana_pct: 0
    cast_time_ms: 8000
    cooldown_ms: 360000
    on_gcd: true
    range: 0
    target_type: self_channel

  DIVINE_HYMN:
    id: 64843
    mana_pct: 36
    cast_time_ms: 8000
    cooldown_ms: 480000
    on_gcd: true
    range: 0
    target_type: self_channel
    targets: 5

  SHADOWFIEND:
    id: 34433
    mana_pct: 6
    cast_time_ms: 0
    cooldown_ms: 300000
    on_gcd: false
    range: 30
    target_type: enemy
    duration_ms: 12000

  POWER_WORD_FORTITUDE:
    id: 21562
    mana_pct: 28
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: party_or_raid
    duration_ms: 3600000

  INNER_FIRE:
    id: 588
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000

  INNER_WILL:
    id: 73413
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    notes: mutually exclusive with Inner Fire; manual override only

  MASS_DISPEL:
    id: 32375
    mana_pct: 27
    cast_time_ms: 1500
    cooldown_ms: 15000
    on_gcd: true
    range: 30
    target_type: ground_targeted

  RESURRECTION:
    id: 2006

  # ─────── Damage spells (primary healing via Atonement) ───────

  SMITE:
    id: 585
    mana_pct: 11
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: holy
    target_type: enemy
    notes: |
      Atonement primary builder. With Glyph of Smite, +20% damage on
      Holy-Fire-debuffed targets — feeds Atonement healing throughput.
      Stacks Evangelism.

  HOLY_FIRE:
    id: 14914
    mana_pct: 13
    cast_time_ms: 2000
    cooldown_ms: 10000
    on_gcd: true
    range: 30
    school: holy
    target_type: enemy
    duration_ms: 7000           # DoT — UNVERIFIED, see DBC
    notes: applies the DoT that buffs Smite (Glyph of Smite). On cooldown.

  ATONEMENT:
    talent_passive: true
    spell_id: 81749             # passive talent — UNVERIFIED
    triggered_by: SMITE / HOLY_FIRE damage on enemy
    effect: |
      100% of damage dealt heals lowest-HP ally within 15y of the enemy
      (50% if heal target is the priest).
    notes: |
      4.3.4 live values per survey: 100% ally / 50% self. UNVERIFIED whether
      talent has 1 or 2 ranks at 50%/100% in 4.3.4 DBC — flagged.
```

---

## Maintenance pass

```yaml
maintenance:
  - name: Inner Fire
    when:
      - aura.missing(SELF, INNER_FIRE)
      - NOT runtime_flag.inner_will_mode
    cast: INNER_FIRE

  - name: Power Word: Fortitude
    when:
      - NOT in_combat
      - aura.missing(SELF, POWER_WORD_FORTITUDE)
    cast: POWER_WORD_FORTITUDE on SELF

  - name: Power Word: Shield on tank (Weakened-Soul-clear)
    when:
      - target_resolves(TANK)
      - in_combat
      - aura.missing(TANK, WEAKENED_SOUL)
      - aura.missing(TANK, POWER_WORD_SHIELD)
      - SELF.mana_pct > 25
    cast: POWER_WORD_SHIELD on TANK
    notes: 15s Weakened-Soul lockout means PW:Shield re-cycle is ~15s. With Rapture, mana-positive.

  - name: Pre-pull PW:Shield
    when:
      - NOT in_combat
      - boss_pull_imminent
      - target_resolves(TANK)
    cast: POWER_WORD_SHIELD on TANK
    notes: |
      Borrowed Time chains into the opener Penance / Greater Heal at +14% haste.
```

---

## Rotation — Priority List (Atonement-spec default)

```yaml
rotation_atonement:
  # ───────── Tier 1: emergency CDs ─────────
  - tier: 1
    name: Pain Suppression — tank emergency
    when:
      - target_resolves(TANK) AND TANK.hp_pct < 40
      - tank_taking_predictable_burst
      - spell.off_cooldown(PAIN_SUPPRESSION)
    cast: PAIN_SUPPRESSION on TANK

  - tier: 1b
    name: Desperate Prayer self-emergency
    when:
      - SELF.hp_pct < 35
      - spell.off_cooldown(DESPERATE_PRAYER)
    cast: DESPERATE_PRAYER

  - tier: 1c
    name: Power Word: Barrier raid emergency
    when:
      - in_combat
      - raid.injured_count >= 4
      - raid.avg_hp_pct < 60
      - spell.off_cooldown(POWER_WORD_BARRIER)
    cast: POWER_WORD_BARRIER at raid_centroid

  # ───────── Tier 2a: Archangel at 5 Evangelism stacks ─────────
  - tier: 2a
    name: Archangel — consume Evangelism stacks
    when:
      - evangelism_stacks == 5
      - spell.off_cooldown(ARCHANGEL)
    cast: ARCHANGEL
    notes: priority over rotation continuation; +18s healing buff is too valuable to delay

  # ───────── Tier 2b: PW:Shield maintenance ─────────
  - tier: 2b
    name: PW:Shield on tank (Weakened-Soul-clear)
    when:
      - target_resolves(TANK)
      - aura.missing(TANK, WEAKENED_SOUL)
      - aura.missing(TANK, POWER_WORD_SHIELD)
      - SELF.mana_pct > 25
    cast: POWER_WORD_SHIELD on TANK
    notes: covered in maintenance, but rotation re-checks each tick.

  # ───────── Tier 2c: Penance on CD ─────────
  - tier: 2c
    name: Penance on injured ally
    when:
      - spell.off_cooldown(PENANCE)
      - any_injured_ally
    target_resolution:
      - prefer: TANK if TANK.hp_pct < 75
      - fallback: PARTY_LOWEST_HP
    cast: PENANCE on resolved_target

  # ───────── Tier 2d: PoM on CD ─────────
  - tier: 2d
    name: Prayer of Mending
    when:
      - spell.off_cooldown(PRAYER_OF_MENDING)
      - in_combat
    cast: PRAYER_OF_MENDING on TANK

  # ───────── Tier 3: Atonement Smite (PRIMARY HEALING) ─────────
  - tier: 3
    name: Atonement-Smite or Holy Fire on cluster-injured target
    when:
      - target_resolves(ATONEMENT_CLUSTER_TARGET)  # enemy w/ ≥1 injured friendly within 15y
      - any_friendly_in_15y_of_atonement_target with hp_pct < 95
      - tank.hp_pct >= 75                          # if tank below 75%, abandon Atonement (Tier 4+)
      - SELF.mana_pct > 25
      - NOT currently_casting

    cast_priority:
      - condition: spell.off_cooldown(HOLY_FIRE)
        cast: HOLY_FIRE on ATONEMENT_CLUSTER_TARGET
      - condition: glyph.active(SMITE) AND aura.active(ATONEMENT_CLUSTER_TARGET, HOLY_FIRE)
        cast: SMITE on ATONEMENT_CLUSTER_TARGET    # +20% damage from glyph
      - condition: default
        cast: SMITE on ATONEMENT_CLUSTER_TARGET

    cancel_conditions:
      - cancel_if: TANK.hp_pct drops below 75
      - cancel_if: any of tier 1 becomes true
      - cancel_if: party.injured_count(radius=30, hp_pct_lt=70) >= 3   # raid heal needed

    notes: |
      Atonement is Tier 3 healing — NOT a Tier 9 support DPS. Survey item:
      this priority placement is the bot's Atonement-spec defining behavior.

  # ───────── Tier 4: Greater Heal — heavy single-target ─────────
  - tier: 4
    name: Greater Heal on tank/heavy-damage target
    when:
      - target_resolves(PARTY_LOWEST_HP)
      - any of:
          - PARTY_LOWEST_HP.hp_pct < 60
          - PARTY_LOWEST_HP == TANK AND PARTY_LOWEST_HP.hp_pct < 75
      - SELF.mana_pct > 40
    cast: GREATER_HEAL on PARTY_LOWEST_HP
    notes: Inner Focus auto-pair if IF.cd == 0 (free + crit).

  # ───────── Tier 5: Prayer of Healing for stacked party ─────────
  - tier: 5
    name: Prayer of Healing on stacked party
    when:
      - target_resolves(POH_PARTY_TARGET)
      - party_of_target_avg_hp_pct < 80
      - injured_in_party >= 3
      - SELF.mana_pct > 40
    cast: PRAYER_OF_HEALING on POH_PARTY_TARGET
    notes: Divine Aegis crit-shield amortizes; Inner Focus auto-pair candidate.

  # ───────── Tier 6: Flash Heal emergency ─────────
  - tier: 6
    name: Flash Heal emergency
    when:
      - target_resolves(PARTY_LOWEST_HP)
      - PARTY_LOWEST_HP.hp_pct < 35
      - SELF.mana_pct > 25
    cast: FLASH_HEAL on PARTY_LOWEST_HP

  # ───────── Tier 7: Heal filler ─────────
  - tier: 7
    name: Heal — efficient default
    when:
      - target_resolves(PARTY_LOWEST_HP)
      - PARTY_LOWEST_HP.hp_pct < 90
      - SELF.mana_pct > 25
    cast: HEAL on PARTY_LOWEST_HP

  # ───────── Tier 8: Smite filler (no Atonement target) ─────────
  - tier: 8
    name: Smite filler — no atonement target
    when:
      - target_resolves(ENEMY_TARGET)
      - NOT atonement_cluster_target_available
      - SELF.mana_pct >= 60                      # higher floor — pure damage, no healing return
      - party.lowest_hp_pct >= 95
      - not currently_casting
    cast: SMITE on ENEMY_TARGET
    notes: |
      Truly support-DPS Smite — no Atonement healing because no friendly is in
      range of the enemy. Higher mana floor than Tier 3 because the cast is
      purely offensive throughput.
```

---

## Rotation — Shield-Spec Preset (tank-heal mode)

When `runtime_flag.tank_healing_mode == true`, the spec swaps to Shield-spec:
- Atonement talent NOT picked (Tier 3 disabled)
- Strength of Soul replaces Train of Thought
- Glyph of Pain Suppression replaces Glyph of Smite
- Tier 2 (PW:Shield + Penance + PoM) becomes the rotation core; Tier 3 Atonement-Smite is removed; rotation skips to Tier 4 (Greater Heal) on PW:Shield-cycle gaps.

This is a respec, not a runtime tier-table swap.

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: INNER_FOCUS
    when:
      - spell.off_cooldown(INNER_FOCUS)
      - about_to_cast(GREATER_HEAL OR PRAYER_OF_HEALING)
    rationale: |
      Free + 25% crit on next big heal. With Train of Thought, every Greater
      Heal cast reduces IF cooldown by 5s — so IF effectively pairs with
      Greater Heal during heavy tank damage.

  - spell: POWER_INFUSION
    when:
      - spell.off_cooldown(POWER_INFUSION)
      - any of:
          - SELF.mana_pct > 50 AND any_injured_ally AND TANK.hp_pct < 80
          - atonement_burst_phase
    cast_target: SELF
    rationale: |
      Self-target by default (Disc PvE convention). +20% haste / -20% cost
      for 15s. Spec uses self-throughput-window predicate; future encounter
      profiles can override to buff a DPS.

  - spell: SHADOWFIEND
    when:
      - in_combat
      - SELF.mana_pct < 50
      - has_enemy_target
      - spell.off_cooldown(SHADOWFIEND)
    rationale: cheaper resource than HoH; fire first when mana drops.

  - spell: HYMN_OF_HOPE
    when:
      - in_combat
      - SELF.mana_pct < 30
      - no_active_emergency
      - spell.off_cooldown(HYMN_OF_HOPE)
    rationale: 8s channel; gates harder on emergency clause.

  - spell: DIVINE_HYMN
    when:
      - in_combat
      - raid.injured_count >= 5
      - raid.avg_hp_pct < 50
      - spell.off_cooldown(DIVINE_HYMN)
    rationale: 8s channel; lower priority than PWB. Cast 30s after PWB if raid still hurts.
```

---

## Proc Reactions

```yaml
procs:
  BORROWED_TIME:
    talent_passive: true
    triggered_by: PW:Shield cast
    effect: +14% spell haste on next cast for 6s
    ai_behavior: |
      Auto-consumed by next cast. Bot's PW:Shield → Penance opener gets the
      haste benefit naturally. No explicit tracking.

  RAPTURE:
    talent_passive: true
    triggered_by: PW:Shield expiry (full or absorbed) — 12s ICD
    effect: 7% max mana to caster
    ai_behavior: |
      Mana model — bot treats PW:Shield as net-positive when ≥12s elapsed.
      No explicit predicate.

  DIVINE_AEGIS:
    talent_passive: true
    triggered_by: crit on direct heal
    effect: 30% of healing done as absorb shield on target
    ai_behavior: |
      Passive. Biases target selection toward Prayer of Healing in cluster
      damage windows (shields amortize). No active reaction.

  GRACE:
    talent_passive: true
    triggered_by: Penance / Heal / Flash Heal on target
    stacks: 3
    effect: +9% healing received (3% per stack)
    ai_behavior: |
      Bot biases toward keeping heals on the same target (tank) in sustained
      damage phases — Grace amortizes. Implicit in target_resolution.

  EVANGELISM:
    talent_passive: true
    triggered_by: Smite / Holy Fire / Penance cast
    stacks: 5
    effect: -2% mana cost / +2% damage per stack
    ai_behavior: |
      Tracked via aura listener. Tier 2a fires Archangel at 5 stacks.

  STRENGTH_OF_SOUL:
    talent_passive: true        # Shield-spec only, NOT Atonement-spec
    triggered_by: Heal / GreaterHeal / FlashHeal on target with Weakened Soul
    effect: -4s on target's Weakened Soul
    ai_behavior: Shield-spec preset only; spec doc encodes for the alternate build.
```

---

## Mana Management

```yaml
mana:
  thresholds:
    drink_floor_pct: 30
    drink_resume_pct: 95
    hymn_of_hope_floor_pct: 30
    shadowfiend_floor_pct: 50
    conserve_mode_pct: 50
    crisis_mode_pct: 25

  conserve_behavior:
    when: SELF.mana_pct < 50
    rules:
      - skip Tier 5 (PRAYER_OF_HEALING) unless party.injured_in_party >= 4
      - skip Tier 4 (GREATER_HEAL) unless TANK.hp_pct < 50
      - skip Tier 8 (Smite filler) entirely
      - keep Tier 3 (Atonement) — it's healing throughput per damage
      - prefer Tier 7 (HEAL) as default

  crisis_behavior:
    when: SELF.mana_pct < 25
    rules:
      - skip Tiers 4-5
      - Tier 7 forced to HEAL on TANK only
      - fire MYTHICAL_MANA_POTION
      - fire HYMN_OF_HOPE regardless of emergency rule
      - Tier 3 Atonement only if Holy Fire is off CD (cheap cast)

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

  food:
    primary:
      item: Seafood_Magnifique_Feast
      item_id: 62290
    fallback:
      item: Severed_Sagefish_Head
      item_id: 62669

  mana_potion:
    item: Mythical_Mana_Potion
    item_id: 57192

  healthstone:
    item: Healthstone
    item_id: 36892
```

---

## Non-encounter content

### Solo combat

```yaml
solo_offensive_rotation:
  - tier: 1: SELF.hp_pct < 35 -> DESPERATE_PRAYER if off_cd else FLASH_HEAL self
  - tier: 2: aura.missing(SELF, POWER_WORD_SHIELD) AND SELF.hp_pct < 80 -> PW:Shield self
  - tier: 3: spell.off_cooldown(PENANCE) -> PENANCE on enemy
  - tier: 4: spell.off_cooldown(HOLY_FIRE) -> HOLY_FIRE on enemy
  - tier: 5: spell.off_cooldown(SHADOWFIEND) AND SELF.mana_pct < 60 -> SHADOWFIEND
  - tier: 6: SELF.mana_pct > 30 -> SMITE filler
  - tier: 7: SELF.mana_pct < 30 -> wand auto-attack
notes: Disc has self-Atonement healing via Smite-on-self... no, Atonement self-heals at 50%. Bot relies on PW:Shield + Penance for survivability while damaging.
```

### Travel & out-of-combat

Mirrors holy-priest spec.

### Idle

Mirrors holy-priest spec.

### World boss / elite scaling

```yaml
world_boss_or_elite:
  rotation_override:
    if_in_party:
      use: encounter rotation (Atonement-spec)
      thresholds:
        pain_suppression_tank_hp: 50          # raised — elites hit harder
        atonement_abandon_threshold: 80       # tighter — preserve tank harder
        crisis_mode_pct: 30
    if_solo:
      use: solo_offensive_rotation
      thresholds:
        pw_shield_self_threshold: 90
        flash_heal_self_threshold: 60
```

---

## Leveling rotation

Bot defaults to Disc spec from level 10 (spec choice unlock).

### Spec recommendation

```yaml
spec_choice:
  for_dungeon_healer_bot:
    spec: Discipline
    secondary_tree_split: 31 Disc + 7 Holy + 3 Shadow
    rationale: |
      Per survey: Atonement-build is bot default. Disc 31 picks include
      Atonement, Train of Thought, Borrowed Time, Rapture, Pain Suppression,
      Power Word: Barrier, Power Infusion, Archangel, Evangelism. Holy 7 =
      Divine Fury 3/3 + Empowered Healing 2/2 + Holy Concentration 2/2.
      Shadow 3 = Darkness 3/3.
    secondary_preset:
      name: Shield-spec
      use_when: tank_heal_assignment_encounter
      changes: Strength of Soul replaces Train of Thought; Atonement skipped
```

### Spell unlock table

```yaml
spell_unlocks:
  POWER_WORD_FORTITUDE: {level: 1,  spell_id: 21562}
  SMITE:                {level: 1,  spell_id: 585}
  SHADOW_WORD_PAIN:     {level: 3,  spell_id: 589}
  POWER_WORD_SHIELD:    {level: 4,  spell_id: 17}
  INNER_FIRE:           {level: 7,  spell_id: 588}
  FLASH_HEAL:           {level: 3,  spell_id: 2061}
  HOLY_FIRE:            {level: 18, spell_id: 14914}
  PENANCE:              {level: 10, spell_id: 47540}     # Disc spec
  HEAL:                 {level: 16, spell_id: 2050}
  GREATER_HEAL:         {level: 38, spell_id: 2060}
  PRAYER_OF_HEALING:    {level: 44, spell_id: 596}
  PRAYER_OF_MENDING:    {level: 68, spell_id: 33076}
  HYMN_OF_HOPE:         {level: 64, spell_id: 64901}
  SHADOWFIEND:          {level: 68, spell_id: 34433}
  DIVINE_HYMN:          {level: 78, spell_id: 64843}
  POWER_WORD_BARRIER:   {requires_talent: true, talent_tier: 7}   # 31-point gate
  PAIN_SUPPRESSION:     {requires_talent: true, talent_tier: 5}
  POWER_INFUSION:       {requires_talent: true, talent_tier: 3}
  ATONEMENT:            {requires_talent: true, talent_tier: 5}
  EVANGELISM:           {requires_talent: true, talent_tier: 4}
  ARCHANGEL:            {requires_talent: true, talent_tier: 6}
  BORROWED_TIME:        {requires_talent: true, talent_tier: 4}
  RAPTURE:              {requires_talent: true, talent_tier: 4}
  DIVINE_AEGIS:         {requires_talent: true, talent_tier: 4}
  GRACE:                {requires_talent: true, talent_tier: 5}
  TRAIN_OF_THOUGHT:     {requires_talent: true, talent_tier: 4}
  STRENGTH_OF_SOUL:     {requires_talent: true, talent_tier: 5}   # Shield-spec only
  INNER_FOCUS:          {requires_talent: true, talent_tier: 1}
  DESPERATE_PRAYER:     {requires_talent: true, talent_tier: 1}
```

### Bracket 1 — Levels 1–9 (pre-Penance)

```yaml
bracket_1_9:
  available: SMITE, PW:SHIELD (4), PW:FORTITUDE (1), FLASH_HEAL (3), INNER_FIRE (7)
  rotation:
    - tier 1: SELF.hp_pct < 25 -> Lesser Heal / Flash Heal self
    - tier 2: PARTY_LOWEST_HP.hp_pct < 50 -> Lesser Heal / Flash Heal
    - tier 3: tank PW:Shield maintenance
    - tier 4: idle -> follow tank
  notes: Bot is just a basic healer pre-Penance. No Atonement (talent-gated).
```

### Bracket 2 — Levels 10–17 (Penance unlocks)

```yaml
bracket_10_17:
  available: + PENANCE (Disc spec at 10)
  rotation_change:
    - PENANCE becomes Tier 2 — heal on CD on injured ally
  notes: Disc spec choice at 10 unlocks Penance. Bot enters Disc identity.
```

### Bracket 3 — Levels 18–37 (Holy Fire)

```yaml
bracket_18_37:
  available: + HOLY_FIRE (18)
  rotation_addition:
    - HOLY_FIRE on cooldown in solo / Tier 3 Atonement (when talent unlocks)
```

### Bracket 4 — Levels 38–43 (Greater Heal)

```yaml
bracket_38_43:
  available: + GREATER_HEAL
  rotation_change:
    - GREATER_HEAL becomes Tier 4 (heavy single-target heal)
```

### Bracket 5 — Levels 44–67 (Prayer of Healing)

```yaml
bracket_44_67:
  available: + PRAYER_OF_HEALING
  rotation_addition:
    - POH becomes Tier 5 (stacked-party heal)
  talent_progression:
    - Atonement (talent) is reachable in this bracket — typically post-50
    - Once Atonement is allocated, Tier 3 Atonement-Smite enters the rotation
```

### Bracket 6 — Levels 68–77 (Prayer of Mending + Shadowfiend)

```yaml
bracket_68_77:
  available: + PRAYER_OF_MENDING (68), + SHADOWFIEND (68), + HYMN_OF_HOPE (70)
  rotation_addition:
    - PoM becomes Tier 2d
    - Shadowfiend cooldown entry
    - Hymn of Hope cooldown entry
```

### Bracket 7 — Levels 78–84 (Divine Hymn)

```yaml
bracket_78_84:
  available: + DIVINE_HYMN
  cooldown_addition:
    - DH at raid emergency
```

### Bracket 8 — Level 85 (Power Word: Barrier + full talents)

```yaml
bracket_85:
  available: + POWER_WORD_BARRIER (31-point Disc talent), + all talents
  rotation: full encounter rotation per §"Rotation Atonement"
```

### Mana floors by era

```yaml
mana_floors_by_level:
  1_60:    { conserve: 0,  crisis: 0  }
  60_70:   { conserve: 55, crisis: 30 }
  70_80:   { conserve: 45, crisis: 25 }
  80_85:   { conserve: 50, crisis: 25 }
```

---

## UNVERIFIED items

| # | Item | Spec value | Where used | Verification path |
|---|---|---|---|---|
| 1 | Atonement talent rank values (4.3.4) | 100% ally / 50% self | Tier 3 healing throughput | DBC SpellEffect for spell IDs 81749 / 81751 |
| 2 | Atonement radius | 15y from enemy | Cluster predicate | DBC EffectRadiusIndex |
| 3 | Penance tick count | 3 (no haste-extra-tick) | Penance spell | DBC SpellDuration for 47540 |
| 4 | Glyph of Penance CD reduction | 8s → 6s | Penance maintenance | DBC GlyphProperties |
| 5 | Borrowed Time haste % | 14% (rank 2/2) | Maintenance opener | Talent.dbc |
| 6 | Rapture mana return | 7% max mana (rank 3/3) | Mana model | Talent.dbc |
| 7 | PW:Shield mana cost | 25% base mana | Spell catalog | DBC PowerCost for 17 |
| 8 | Pain Suppression mana cost | 8% base mana | Spell catalog | DBC PowerCost for 33206 |
| 9 | Hymn of Hope mana per tick | 2% per tick | Mana CD | DBC SpellEffect |
| 10 | Divine Hymn target count | 5 | Spell catalog | DBC SpellEffect |
| 11 | Body and Soul movement-speed value | 60% / 4s | passive notes | Talent.dbc |
| 12 | Power Infusion mana cost | 16% base mana | Cooldown profile | DBC PowerCost |
| 13 | Inner Focus 4.3.4 spell list | excl. Penance? | Inner Focus pairing | DBC + survey gap |
| 14 | Train of Thought interaction with Smite | -0.5s Penance CD per Smite | Atonement rotation tempo | Talent.dbc |
| 15 | Glyph of Smite Holy Fire DoT requirement | active DoT | Smite priority | DBC GlyphProperties |
| 16 | Holy Word: Sanctuary radius (priest baseline pre-talent) | n/a — Disc doesn't have this | n/a | n/a |
| 17 | Power Word: Barrier radius | 6y assumed | Spell catalog | DBC EffectRadiusIndex |
| 18 | Holy Fire DoT duration | 7s assumed | Spell catalog | DBC |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kDiscPriest_Spells[]` |
| Rotation tiers | `static const RotationTier kDiscPriest_Rotation[]` |
| Cooldown Profile | `static const CooldownTrigger kDiscPriest_Cooldowns[]` |
| Mana Management | `ManaState` enum + threshold helpers |
| Leveling rotation | `LevelBracketRotation kDiscPriest_Leveling[]` |
| Atonement target resolution | `Unit* AtonementClusterTarget(bot)` — enemy with ≥1 injured friendly within 15y |
| Evangelism stack tracker | `uint8 evangelism_stacks` on `AltbotAI`, updated via aura listener |
| Spec preset (Atonement vs Shield) | `DiscSpec` enum on `AltbotAI` (Atonement / Shield); affects rotation table selection |
| Borrowed Time chain | implicit — bot doesn't track buff, just chains casts |
| Grace stack tracker | not modeled directly; bot's tank-priority bias amortizes Grace naturally |

**Key new helper APIs:**
- `EvangelismStacks(bot)` — read current stack count (0–5).
- `AtonementClusterTarget(bot, master)` — returns enemy with injured friendly in 15y, prefers tank-cluster.
- `IsAtonementSpec(bot)` — runtime flag for spec preset.
- `WeakenedSoulRemainingMs(target)` — for PW:Shield-cycle gate.

Cross-reference to `docs/roles/healer.md` for the Atonement-as-primary-healing exception. The exception is encoded in the Tier 3 of this spec; the role doc is the project-level explainer.
