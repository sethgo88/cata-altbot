# Restoration Druid — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide. The YAML blocks below are designed to lift cleanly into `static const` tables in a future `AltbotCombat.cpp`.

**Numerical data** (spell IDs, mana %, cast times, cooldowns, durations, item IDs) verified against Wowhead's Cata 4.3.4 archive and Warcraft Wiki. Items the AI will need but that public archives don't pin down with confidence are tagged `UNVERIFIED`.

**Strategy content** reconciled across 9 community guides. See `docs/research/resto-druid-guide-survey.md` for the comparison table and which thresholds are consensus vs author-judgment.

**DESIGN DEPARTURE — HoT-rolling rotation tier shape.** Resto Druid is the only healer spec in this project where the top rotation tiers are HoT MAINTENANCE, not HP-gated direct heals. Tiers 1-3 maintain Lifebloom 3-stack, Rejuv on damaged raid, Wild Growth on cluster damage. Tier 5 fires direct heals primarily to refresh **Mastery: Harmony** (10s buff after any direct heal grants +10% baseline HoT/heal scaling). HP-gated emergencies start at Tier 6, not Tier 1. Cross-class healer-role doc references this spec as the HoT-rolling exemplar.

---

## Identity

```yaml
spec: resto_druid
class: druid
role: HEALER
resource_model: mana
defining_mechanic: HoT-rolling + Mastery: Harmony refresh cadence
defining_buffs:
  - Mark_of_the_Wild        # raid stat buff
  - Harmony                 # +10% HoT bonus, refreshed by direct heals
  - Tree_of_Life_Form       # 25s shapeshift CD; +15% healing, instant Regrowth, etc.
defining_procs:
  - Omen_of_Clarity         # free + instant next nature spell (typically Regrowth)
  - Living_Seed             # absorb-then-heal seed on critically-healed targets
mastery: Harmony            # +% on direct heals AND HoTs after recent direct heal
key_cooldowns:
  - Tree_of_Life_Form       # 3 min — 25s shapeshift CD
  - Tranquility             # 3 min (with Malfurion's Gift 2/2; baseline 8 min)
  - Innervate               # 3 min — free, no-GCD mana CD
  - Natures_Swiftness       # 3 min — instant + amplified next nature spell
  - Barkskin                # 1 min — -20% damage taken, no GCD
  - Rebirth                 # 10 min — combat-rez (out-of-combat too)
ranged: 40
notes: |
  Resto Druid in 4.3.4 plays as a HoT-rolling raid+tank hybrid healer.
  Outside of Tree of Life, the rotation is: maintain Lifebloom 3-stack on
  tank, spread Rejuvenation on damaged raid, Wild Growth on cluster damage,
  Swiftmend on cooldown to seed Efflorescence ground-AoE, refresh Harmony
  with Nourish every ~10s. Tree of Life is a 3-min CD that grants instant
  Regrowth + Lifebloom on unlimited targets + Wild Growth +2 targets for
  25 seconds — fire when raid is hurting.

  This spec departs from the other healer specs in tier shape: Tiers 1-3
  are HoT maintenance, NOT HP-gated emergency heals. Emergency direct heals
  start at Tier 6.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  TANK:                        # primary tank with Lifebloom 3-stack maintained
  PARTY_LOWEST_HP:
  PARTY_LOWEST_MISSING_HP:
  REJUV_REFRESH_TARGET:        # ally with Rejuv expiring < 3s OR ally without Rejuv at hp_pct < 80
  WILD_GROWTH_ANCHOR:          # ally with most injured allies clustered within 30y
  SWIFTMEND_TARGET:             # ally with active Rejuv/Regrowth AND lowest HP among them
  EFFLORESCENCE_CLUSTER:        # cluster of 3+ injured within 8y of Swiftmend target
  HARMONY_REFRESH_TARGET:       # any injured ally; prefer Lifebloom-stacked tank for Empowered Touch refresh
  CLEARCASTING_TARGET:          # lowest-HP injured ally if Clearcasting proc active
  ENEMY_TARGET:
```

---

## Spell Catalog

```yaml
spells:
  # ─────── HoT core ───────

  REJUVENATION:
    id: 774
    mana_pct: 16              # post-Jan-2011 hotfix
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    duration_ms: 12000        # 4 base ticks; 5 with 12.5%+ haste
    notes: raid HoT — selectively spread, not blanket-spammed

  REGROWTH:
    id: 8936
    mana_pct: 35              # most expensive direct heal
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    duration_ms: 6000
    notes: |
      Direct heal + 6s HoT. Mana-prohibitive without Clearcasting. Tree of
      Life makes it instant. Refreshes Lifebloom on target via Empowered Touch.

  LIFEBLOOM:
    id: 33763
    mana_pct: 9
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    duration_ms: 10000
    max_stacks: 3
    notes: |
      Tank 3-stack maintained near 100% uptime. Single-target outside Tree
      of Life (multiple targets in TOL form). Refreshes via direct heal on
      same target via Empowered Touch (100% at rank 2/2).

  WILD_GROWTH:
    id: 48438
    mana_pct: 27
    cast_time_ms: 0
    cooldown_ms: 8000
    on_gcd: true
    range: 40
    target_type: ally_anchor
    radius: 30
    targets: 5                # 6 with Glyph of Wild Growth; 7 in TOL; 8 with both
    duration_ms: 7000
    notes: smart-heal HoT cluster; front-loaded healing curve

  SWIFTMEND:
    id: 18562
    mana_pct: 14
    cast_time_ms: 0
    cooldown_ms: 15000
    on_gcd: true
    range: 40
    target_type: ally_with_rejuv_or_regrowth
    notes: |
      Instant heal. Default consumes Rejuv/Regrowth on target; Glyph of
      Swiftmend prevents consumption. Spawns Efflorescence ground-AoE if
      Efflorescence talent picked.

  EFFLORESCENCE:
    talent_passive: true
    spawned_by: SWIFTMEND
    radius: 8                 # NOT 30y — confirmed 8y per G5 wiki
    duration_ms: 7000
    targets: 3                # 3 most-injured within radius
    notes: ground HoT; 12% of Swiftmend healing per tick (rank 3/3)

  HEALING_TOUCH:
    id: 5185
    mana_pct: 30
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    notes: |
      Slow, mana-expensive direct heal. Reserved for Nature's Swiftness
      pairing (NS makes it instant + 50% amplified, post-4.1).

  NOURISH:
    id: 50464
    mana_pct: 10
    cast_time_ms: 3000        # haste-affected
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    notes: |
      Cheap direct heal. +20% bonus per HoT on target. Default Harmony
      refresher. Refreshes Lifebloom via Empowered Touch.

  TRANQUILITY:
    id: 740
    mana_pct: 32
    cast_time_ms: 8000        # 8s channel
    cooldown_ms: 180000       # 3 min with Malfurion's Gift 2/2 (baseline 8 min)
    on_gcd: true
    range: 0
    target_type: self_channel
    targets: 5
    notes: smart-heal channel; pulses 5 most-injured raid-wide

  TREE_OF_LIFE:
    id: 33891
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 180000       # 3 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 25000        # 25s, NOT 30s (4.0.6 nerf)
    notes: |
      Shapeshift CD. +15% all healing, +120% armor, instant Regrowth, Lifebloom
      unlimited targets, Wild Growth +2 targets, instant Wrath, instant
      Entangling Roots.

  INNERVATE:
    id: 29166
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 180000       # 3 min
    on_gcd: false
    range: 30
    target_type: self_or_ally  # bot uses self only (5% on ally, 20% self post-4.2)
    duration_ms: 10000
    notes: free + no-GCD mana cooldown

  NATURES_SWIFTNESS:
    id: 17116
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 180000       # 3 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 8000
    notes: next nature spell instant + 50% amplified (post-4.1)

  BARKSKIN:
    id: 22812
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 60000        # 1 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 12000
    notes: -20% damage taken; no mana, no GCD; use frequently

  REBIRTH:
    id: 20484
    mana_pct: 60
    cast_time_ms: 2000
    cooldown_ms: 600000       # 10 min (Glyph of Rebirth: target rezzes at 100% HP)
    on_gcd: true
    range: 30
    target_type: corpse
    notes: combat-rez

  REVIVE:
    id: 50769
    mana_pct: 60
    cast_time_ms: 10000
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: corpse
    notes: out-of-combat resurrect

  MARK_OF_THE_WILD:
    id: 1126
    mana_pct: 39
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: party_or_raid
    duration_ms: 3600000      # 1 hour
    notes: stat buff — pre-pull only

  REMOVE_CORRUPTION:
    id: 2782
    mana_pct: 13
    cast_time_ms: 0
    cooldown_ms: 8000
    on_gcd: true
    range: 40
    target_type: ally
    notes: removes 1 poison + 1 curse from ally

  THORNS:
    id: 467
    mana_pct: 8
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: true
    range: 40
    target_type: ally
    duration_ms: 8000
    notes: reflective shield 8s; rarely valuable in PvE healer rotation

  # ─────── Damage spells (solo + Tier 9 idle) ───────

  WRATH:
    id: 5176
    mana_pct: 9
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: nature
    target_type: enemy

  MOONFIRE:
    id: 8921
    mana_pct: 9
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: arcane
    target_type: enemy
    duration_ms: 12000        # DoT
```

---

## Maintenance pass

```yaml
maintenance:
  - name: Mark of the Wild on raid
    when:
      - NOT in_combat
      - any_raid_member_missing_buff(MARK_OF_THE_WILD)
    cast: MARK_OF_THE_WILD on missing_member

  - name: Pre-pull Lifebloom 3-stack on tank
    when:
      - NOT in_combat
      - boss_pull_imminent
      - target_resolves(TANK)
      - lifebloom_stacks_on(TANK) < 3
    cast: LIFEBLOOM on TANK
    notes: build 3-stack across T-7s/T-5s/T-3s

  - name: Pre-pull Rejuvenation on tank
    when:
      - NOT in_combat
      - boss_pull_imminent_in_2s
      - target_resolves(TANK)
      - aura.missing(TANK, REJUVENATION)
    cast: REJUVENATION on TANK
```

---

## Rotation — Priority List

```yaml
rotation:
  # ───────── Tier 1: Lifebloom 3-stack on tank ─────────
  - tier: 1
    name: Lifebloom — maintain 3-stack on tank
    when:
      - target_resolves(TANK)
      - any of:
          - lifebloom_stacks_on(TANK) < 3
          - lifebloom_stacks_on(TANK) == 3 AND lifebloom_remaining_ms < 4000
    cast: LIFEBLOOM on TANK
    notes: |
      Refresh BEFORE bloom (4s buffer). Stack-build 1→3 on first cycle, refresh-only
      after. Empowered Touch lets direct heals on the stacked target refresh — bot
      exploits this by routing direct heals through the tank.

  # ───────── Tier 2: Rejuvenation on damaged raid ─────────
  - tier: 2
    name: Rejuvenation — selective spread on damaged raid
    when:
      - target_resolves(REJUV_REFRESH_TARGET)
      - any of:
          - aura.missing(REJUV_REFRESH_TARGET, REJUVENATION) AND REJUV_REFRESH_TARGET.hp_pct < 90
          - aura.expires_in_ms(REJUV_REFRESH_TARGET, REJUVENATION) < 3000 AND REJUV_REFRESH_TARGET.hp_pct < 80
      - SELF.mana_pct > 40
    cast: REJUVENATION on REJUV_REFRESH_TARGET
    notes: |
      Selective — NOT blanket-spammed. Tank gets Rejuv refresh aggressively;
      raid members only when injured (avoid pure overheal mana waste).

  # ───────── Tier 3: Wild Growth on cluster ─────────
  - tier: 3
    name: Wild Growth on cluster damage
    when:
      - spell.off_cooldown(WILD_GROWTH)
      - target_resolves(WILD_GROWTH_ANCHOR)
      - injured_allies_within_30y_of(WILD_GROWTH_ANCHOR) >= 3
      - SELF.mana_pct > 30
    cast: WILD_GROWTH on WILD_GROWTH_ANCHOR

  # ───────── Tier 4: Swiftmend ─────────
  - tier: 4
    name: Swiftmend — instant heal + Efflorescence seed
    when:
      - spell.off_cooldown(SWIFTMEND)
      - target_resolves(SWIFTMEND_TARGET)
      - target_has_rejuv_or_regrowth(SWIFTMEND_TARGET)
      - SWIFTMEND_TARGET.hp_pct < 90
    cast: SWIFTMEND on SWIFTMEND_TARGET
    notes: |
      Doubles as Harmony refresher. Glyph of Swiftmend prevents HoT consumption
      (default ON in this spec). Efflorescence ground-AoE seeds at target's
      position; favor targets clustered with other injured allies (8y radius).

  # ───────── Tier 4.5: Clearcasting Regrowth ─────────
  - tier: 4.5
    name: Free Regrowth via Omen of Clarity proc
    when:
      - aura.active(SELF, CLEARCASTING)
      - target_resolves(CLEARCASTING_TARGET)
      - CLEARCASTING_TARGET.hp_pct < 95
    cast: REGROWTH on CLEARCASTING_TARGET
    notes: free + instant. Refreshes Lifebloom + Harmony + applies Regrowth HoT.

  # ───────── Tier 5: Harmony refresh (Nourish) ─────────
  - tier: 5
    name: Nourish — Mastery: Harmony refresh
    when:
      - harmony_buff_remaining_s < 4
      - target_resolves(HARMONY_REFRESH_TARGET)
      - HARMONY_REFRESH_TARGET.hp_pct < 90
      - SELF.mana_pct > 25
    cast: NOURISH on HARMONY_REFRESH_TARGET
    notes: |
      Default Harmony refresher. Cheaper than Healing Touch. Prefer Lifebloom-stacked
      tank for Empowered Touch refresh side-effect. Skipped pre-Mastery (level < 80).

  # ───────── Tier 6: Tank emergency direct heal ─────────
  - tier: 6
    name: Healing Touch — Nature's Swiftness emergency burst
    when:
      - aura.active(SELF, NATURES_SWIFTNESS)
      - target_resolves(TANK) AND TANK.hp_pct < 50
    cast: HEALING_TOUCH on TANK
    notes: NS-paired emergency only. Without NS, HT is too slow + mana-expensive.

  # ───────── Tier 7: Raid emergency direct heal ─────────
  - tier: 7
    name: Regrowth emergency on critical ally (no Clearcasting)
    when:
      - target_resolves(PARTY_LOWEST_HP)
      - PARTY_LOWEST_HP.hp_pct < 30
      - SELF.mana_pct > 40
      - NOT aura.active(SELF, CLEARCASTING)   # if clearcasting, Tier 4.5 handles
    cast: REGROWTH on PARTY_LOWEST_HP
    notes: emergency-only fallback when no Clearcasting + critical HP

  # ───────── Tier 8: support DPS / idle ─────────
  - tier: 8
    name: Support DPS — Wrath / Moonfire
    when:
      - target_resolves(ENEMY_TARGET)
      - SELF.mana_pct >= 75
      - party.lowest_hp_pct >= 95
      - lifebloom_stacks_on(TANK) == 3
      - aura.active(TANK, REJUVENATION)
      - not currently_casting
      - no_predicted_damage_in_next_2s

    cast_priority:
      - condition: aura.missing(ENEMY_TARGET, MOONFIRE)
        cast: MOONFIRE on ENEMY_TARGET
      - condition: SELF.mana_pct >= 80
        cast: WRATH on ENEMY_TARGET

    cancel_conditions:
      - cancel_if: any of tiers 1-7 becomes true
      - cancel_if: harmony_buff_remaining_s < 5
```

---

## Tree of Life Form Rotation

When Tree of Life is active (form CD up):

```yaml
rotation_tree_of_life:
  # ───────── Tier 1: Lifebloom spread (TOL only) ─────────
  - tier: 1_tol
    name: Lifebloom spread — multiple injured raid members
    when:
      - tree_of_life_active
      - target_resolves(PARTY_LOWEST_HP)
      - aura.missing(PARTY_LOWEST_HP, LIFEBLOOM)
      - PARTY_LOWEST_HP.hp_pct < 90
      - SELF.mana_pct > 30
    cast: LIFEBLOOM on PARTY_LOWEST_HP
    notes: |
      Always prioritize tank's 3-stack first (Tier 0). Then spread Lifebloom
      to additional injured raid members. Each Lifebloom procs Clearcasting
      with Malfurion's Gift talent (4% chance per heal tick).

  # ───────── Tier 2: Wild Growth (TOL +2 targets = 7 baseline / 8 with glyph) ─────────
  # Same as non-TOL Tier 3 but with expanded target count

  # ───────── Tier 3: Instant Regrowth ─────────
  - tier: 3_tol
    name: Instant Regrowth — Tree of Life enhancement
    when:
      - tree_of_life_active
      - target_resolves(PARTY_LOWEST_HP)
      - PARTY_LOWEST_HP.hp_pct < 70
    cast: REGROWTH on PARTY_LOWEST_HP
    notes: TOL makes Regrowth instant; mana-expensive but instant heal+HoT combo.

  # All other rotation tiers continue as in non-TOL rotation.
```

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: TREE_OF_LIFE
    when:
      - in_combat
      - raid.injured_count >= 4
      - raid.avg_hp_pct < 70
      - spell.off_cooldown(TREE_OF_LIFE)
    rationale: |
      25s shapeshift; G7 advice "at least twice per fight." Bot fires more
      eagerly than a player would.

  - spell: TRANQUILITY
    when:
      - in_combat
      - raid.injured_count >= 5
      - raid.avg_hp_pct < 60
      - spell.off_cooldown(TRANQUILITY)
    rationale: |
      8s channel; smart-heals 5 most-injured raid-wide. Cancellable if
      tank-emergency requires reaction. Pair with TOL when both up
      (TOL first instant, Tranquility immediately after for +15% buff).

  - spell: INNERVATE
    when:
      - in_combat
      - SELF.mana_pct < 80
      - spell.off_cooldown(INNERVATE)
    rationale: |
      Free + no-GCD. G1 explicit threshold. Self-cast only (post-4.2 ally
      nerf to 5% makes ally-cast not worth GCD).

  - spell: NATURES_SWIFTNESS
    when:
      - any of:
          - target_resolves(TANK) AND TANK.hp_pct < 30
          - target_resolves(PARTY_LOWEST_HP) AND PARTY_LOWEST_HP.hp_pct < 25
      - spell.off_cooldown(NATURES_SWIFTNESS)
    next_cast: HEALING_TOUCH on resolved_target
    rationale: emergency burst CD; always paired with Healing Touch

  - spell: BARKSKIN
    when:
      - SELF.hp_pct < 70
      - in_combat
      - spell.off_cooldown(BARKSKIN)
    rationale: 1-min CD self-defensive; no mana, no GCD. Use aggressively per G7.

  - spell: REBIRTH
    when:
      - in_combat
      - any_dead_party_member
      - spell.off_cooldown(REBIRTH)
    target_priority: tank > healer > caster_dps > melee_dps
    rationale: combat-rez; cross-class generic priority order.
```

---

## Proc Reactions

```yaml
procs:
  OMEN_OF_CLARITY:
    spell_id: 16864
    triggered_by: |
      - swing/cast events with random chance per heal tick (Malfurion's Gift talent enables proc-from-healing)
    effect: next nature spell free + instant
    duration_ms: 15000          # 15s post-4.2
    ai_behavior: |
      Tier 4.5 fires Regrowth on lowest-HP injured ally. Aggressive consume
      because the proc has only 15s before expiry.

  HARMONY:
    mastery_passive: true
    triggered_by: direct heal cast (Nourish, Healing Touch, Regrowth, Swiftmend)
    effect: +10% (+1.25% per mastery point) on direct heals AND HoTs
    duration_ms: 10000
    ai_behavior: |
      Tier 5 fires Nourish to refresh when buff_remaining_s < 4. Bot does NOT
      cast direct heals during low-damage phases just to refresh (mana waste);
      Harmony lapses naturally if no incoming damage.

  LIVING_SEED:
    talent_passive: true
    triggered_by: crit-heal on ally
    effect: absorb-then-heal seed on target
    duration_ms: 15000
    ai_behavior: passive; no rotation logic.

  NATURES_GRACE:
    talent_passive: true
    triggered_by: Regrowth crit / various procs
    effect: +20% spell haste for 15s, 60s ICD
    ai_behavior: passive; biases cast cadence implicitly.
```

---

## Mana Management

```yaml
mana:
  thresholds:
    drink_floor_pct: 30
    drink_resume_pct: 95
    innervate_floor_pct: 80
    conserve_mode_pct: 50
    crisis_mode_pct: 25

  conserve_behavior:
    when: SELF.mana_pct < 50
    rules:
      - skip Tier 2 Rejuv on raid members above 80% HP (tank Rejuv still maintained)
      - skip Tier 3 Wild Growth unless injured_count >= 4
      - skip Tier 8 support DPS entirely
      - Tier 5 Nourish only when harmony_buff_remaining_s < 2

  crisis_behavior:
    when: SELF.mana_pct < 25
    rules:
      - skip Tier 2 Rejuv on non-tank raid members
      - skip Tier 3 Wild Growth unless raid.injured_count >= 5
      - skip Tier 5 Harmony refresh entirely (let buff lapse)
      - fire MYTHICAL_MANA_POTION
      - fire INNERVATE regardless of timing rules
      - Tier 7 emergency forced to NOURISH if Regrowth too expensive

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

Resto solo damage is weak; bot mandate is dungeon healing per project scope.

```yaml
solo_offensive_rotation:
  - tier: 1: SELF.hp_pct < 35 -> REGROWTH self if mana_pct > 40 else REJUVENATION self
  - tier: 2: aura.missing(SELF, REJUVENATION) AND SELF.hp_pct < 80 -> REJUVENATION self
  - tier: 3: aura.missing(target, MOONFIRE) -> MOONFIRE on target
  - tier: 4: target.hp_pct > 0 AND SELF.mana_pct > 20 -> WRATH on target
  - tier: 5: SELF.mana_pct <= 20 -> auto-attack
notes: bot is healer-mandate; solo combat is fallback only
```

### Travel & out-of-combat

```yaml
travel:
  travel_form:
    use_when:
      - not in_combat
      - distance_to_destination > 30
      - distance_to_destination < 80
    cast: TRAVEL_FORM
    notes: druid travel form (+40% run speed); dismounts trigger on combat

  mount:
    use_when:
      - not in_combat
      - distance_to_destination >= 80
      - mount_zone_allowed
    notes: AltbotMount module handles mount selection

ooc_maintenance:
  - check: aura.missing(SELF, MARK_OF_THE_WILD)
    action: cast MARK_OF_THE_WILD
  - check: SELF.mana_pct < 80 AND not enemies_within(40)
    action: drink
  - check: SELF.hp_pct < 95 AND not enemies_within(40)
    action: eat
```

### Idle

```yaml
idle:
  primary_action: follow master
  secondary_actions:
    - maintain MARK_OF_THE_WILD
  do_not:
    - do not cast offensive spells while idle
    - do not engage neutral mobs unprompted
```

### World boss / elite scaling

```yaml
world_boss_or_elite:
  rotation_override:
    if_in_party:
      use: encounter rotation
      thresholds:
        guardian_emergency_tank_hp: 35      # raised — elites hit harder
        ns_ht_threshold: 50                 # raised
        crisis_mode_pct: 30
    if_solo:
      use: solo_offensive_rotation
      thresholds:
        regrowth_self_threshold: 60
        do_not_engage_unless_can_solo: true
```

### Disengage / emergency utility

```yaml
disengage:
  barkskin:
    use_when:
      - SELF.hp_pct < 70
      - in_combat
    cast: BARKSKIN

  travel_form_break:
    use_when:
      - SELF.hp_pct < 50
      - in_combat
    cast: TRAVEL_FORM
    notes: travel form is combat-castable for movement burst
```

---

## Leveling rotation

Pure Resto from level 10 (spec choice). Bot's mandate is dungeon healing.

### Spec recommendation

```yaml
spec_choice:
  for_dungeon_healer_bot:
    spec: Restoration
    secondary_tree_split: 31 Resto + 5 Balance (Moonglow 3/3 + Nature's Majesty 2/2)
    rationale: |
      Per survey: Balance secondary wins over Feral. Moonglow 3/3 is pure
      throughput-via-mana-efficiency; Nature's Majesty 2/2 (+4% spell crit)
      benefits Regrowth + direct-heal Harmony procs. Feral utility (Stampeding
      Roar, Skull Bash) requires Cat Form mid-combat which the bot is not
      designed to use.
```

### Spell unlock table

```yaml
spell_unlocks:
  REJUVENATION:        {level: 3,  spell_id: 774}
  HEALING_TOUCH:       {level: 78, spell_id: 5185}    # raised in 4.0.6 from 3 to 78
  NOURISH:             {level: 8,  spell_id: 50464}   # lowered in 4.0.6 from 78 to 8
  REGROWTH:            {level: 12, spell_id: 8936}
  WILD_GROWTH:         {level: 60, spell_id: 48438}   # Resto spec
  LIFEBLOOM:           {level: 64, spell_id: 33763}
  TRANQUILITY:         {level: 68, spell_id: 740}
  INNERVATE:           {level: 28, spell_id: 29166}
  SWIFTMEND:           {level: 34, spell_id: 18562}   # Resto spec
  NATURES_SWIFTNESS:   {level: 32, spell_id: 17116}   # talent
  TREE_OF_LIFE:        {level: 81, spell_id: 33891}   # Resto capstone
  REBIRTH:             {level: 32, spell_id: 20484}
  MARK_OF_THE_WILD:    {level: 1,  spell_id: 1126}
  BARKSKIN:            {level: 32, spell_id: 22812}
  REVIVE:              {level: 12, spell_id: 50769}
  REMOVE_CORRUPTION:   {level: 14, spell_id: 2782}
  WRATH:               {level: 1,  spell_id: 5176}
  MOONFIRE:            {level: 4,  spell_id: 8921}
  # Talented:
  EFFLORESCENCE:       {requires_talent: true, talent_tier: 5}
  EMPOWERED_TOUCH:     {requires_talent: true, talent_tier: 4}
  NATURES_BOUNTY:      {requires_talent: true, talent_tier: 5}
  MALFURIONS_GIFT:     {requires_talent: true, talent_tier: 4}
  LIVING_SEED:         {requires_talent: true, talent_tier: 3}
  REVITALIZE:          {requires_talent: true, talent_tier: 5}
```

### Bracket 1 — Levels 1–7 (Rejuv + Wrath only)

```yaml
bracket_1_7:
  available: REJUVENATION (3), WRATH (1), MOONFIRE (4), MARK_OF_THE_WILD (1)
  rotation:
    - tier 1: SELF.hp_pct < 30 -> REJUVENATION self
    - tier 2: PARTY_LOWEST_HP.hp_pct < 60 -> REJUVENATION on PARTY_LOWEST_HP
    - tier 3: idle -> follow tank
  notes: pure HoT healing pre-direct-heal; bot is essentially a Rejuv distributor
```

### Bracket 2 — Levels 8–11 (Nourish unlocks)

```yaml
bracket_8_11:
  available: + NOURISH (8)
  rotation_change:
    - new tier (between Rejuv and idle): direct-heal injured ally with NOURISH
  notes: NOURISH replaces lack-of-direct-heal at level 8 (post-4.0.6 change)
```

### Bracket 3 — Levels 12–27 (Regrowth)

```yaml
bracket_12_27:
  available: + REGROWTH
  rotation_addition:
    - REGROWTH for emergency single-target heal
```

### Bracket 4 — Levels 28–31 (Innervate)

```yaml
bracket_28_31:
  available: + INNERVATE
  cooldown_addition:
    - INNERVATE on self at mana_pct < 80
```

### Bracket 5 — Levels 32–33 (NS + Barkskin + Rebirth)

```yaml
bracket_32_33:
  available: + NATURES_SWIFTNESS (talent), + BARKSKIN, + REBIRTH
  cooldown_addition:
    - NS+HT-equivalent (no HT until 78; NS pairs with REGROWTH at this bracket — instant + amplified)
    - BARKSKIN at self.hp_pct < 70
    - REBIRTH on dead ally
```

### Bracket 6 — Levels 34–59 (Swiftmend)

```yaml
bracket_34_59:
  available: + SWIFTMEND (Resto spec)
  rotation_addition:
    - SWIFTMEND becomes Tier 4 (instant heal on Rejuv/Regrowth target)
```

### Bracket 7 — Levels 60–63 (Wild Growth)

```yaml
bracket_60_63:
  available: + WILD_GROWTH (Resto spec)
  rotation_addition:
    - WILD_GROWTH becomes Tier 3 (cluster heal)
```

### Bracket 8 — Levels 64–67 (Lifebloom)

```yaml
bracket_64_67:
  available: + LIFEBLOOM
  rotation_addition:
    - LIFEBLOOM becomes Tier 1 (tank 3-stack maintenance)
  notes: bot now does proper HoT-rolling rotation; pre-64 it's just Rejuv + direct heals
```

### Bracket 9 — Levels 68–77 (Tranquility)

```yaml
bracket_68_77:
  available: + TRANQUILITY
  cooldown_addition:
    - TRANQUILITY at raid emergency
```

### Bracket 10 — Levels 78–80 (Healing Touch)

```yaml
bracket_78_80:
  available: + HEALING_TOUCH
  rotation_change:
    - NS+HT replaces NS+REGROWTH as emergency burst (HT amplified more)
```

### Bracket 11 — Level 81+ (Tree of Life + Mastery: Harmony)

```yaml
bracket_81_85:
  available: + TREE_OF_LIFE, + MASTERY: HARMONY (lvl 80)
  rotation_change:
    - TOL becomes 3-min cooldown
    - Tier 5 Harmony refresh (Nourish) enters rotation
    - Full encounter rotation per §"Rotation"
  notes: Mastery itself unlocks at 80; Harmony refresh logic only fires at 80+.
```

### Mana floors by era

```yaml
mana_floors_by_level:
  1_60:    { conserve: 0,  crisis: 0  }
  60_70:   { conserve: 55, crisis: 30 }
  70_80:   { conserve: 45, crisis: 25 }
  80_85:   { conserve: 50, crisis: 25 }
```

### Glyph priorities by level

```yaml
glyphs_by_level:
  pre_25:  skip
  25_50:   [Glyph_of_Rejuvenation]                       # prime — first prime slot
  50_75:   [+Glyph_of_Lifebloom (prime), +Glyph_of_Healing_Touch (major)]
  75_85:   [+Glyph_of_Swiftmend (prime), +Glyph_of_Wild_Growth (major), +Glyph_of_Rebirth (major)]
```

---

## UNVERIFIED items

| # | Item | Spec value | Where used | Verification path |
|---|---|---|---|---|
| 1 | Tree of Life duration | 25s (4.0.6 nerf) | TOL CD | DBC SpellDuration for 33891 |
| 2 | Tree of Life Wild Growth +2 targets | 5→7 in form | TOL Wild Growth | DBC ApplyAura/EffectMisc |
| 3 | Glyph of Wild Growth +1 target effect | 5→6 baseline | Wild Growth catalog | DBC GlyphProperties |
| 4 | Efflorescence radius | 8y (NOT 30y) | Swiftmend cluster | DBC EffectRadiusIndex for talent spell |
| 5 | Efflorescence rank scaling | 12% per tick (rank 3/3) | Talent picks | Talent.dbc |
| 6 | Tranquility CD with Malfurion's Gift 2/2 | 8 min - 5 min = 3 min | Cooldown profile | Talent.dbc + Spell.dbc cross-reference |
| 7 | Mastery: Harmony scaling | 10% baseline + 1.25% per mastery point | Mastery procs | DBC + level 85 mastery rating |
| 8 | Empowered Touch refresh chance at 2/2 | 100% | Lifebloom refresh logic | Talent.dbc |
| 9 | Omen of Clarity proc rate from heals | 4% per Lifebloom heal tick (Malfurion's Gift) | Proc rotation tier | Talent.dbc |
| 10 | Clearcasting proc duration | 15s post-4.2 | Tier 4.5 timing | Spell.dbc for OOC buff |
| 11 | Innervate self-cast mana return | 20% max mana over 10s | Cooldown profile | Spell.dbc |
| 12 | Innervate ally-cast mana return (post-4.2) | 5% ally max mana | Excluded from rotation | Spell.dbc |
| 13 | NS+HT 50% amplification (post-4.1) | 50% on next nature spell | NS cooldown | Spell.dbc |
| 14 | Healing Touch level requirement | 78 (post-4.0.6) | Bracket 10 | Spell.dbc |
| 15 | Nourish level requirement | 8 (post-4.0.6) | Bracket 2 | Spell.dbc |
| 16 | Living Seed proc rate | UNVERIFIED | Living Seed talent | Talent.dbc |
| 17 | Wild Growth front-loaded tick curve | UNVERIFIED, engine model TBD | Wild Growth catalog | DBC + tick-by-tick effect data |
| 18 | Tree of Life enhanced spell list | UNVERIFIED full list | TOL rotation | DBC SpellAura + form-conditional effects |
| 19 | Lifebloom bloom-on-expiry healing | UNVERIFIED amount | Lifebloom catalog | DBC SpellEffect on bloom |
| 20 | Mark of the Wild duration | 1 hour (assumed) | Maintenance | Spell.dbc |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kRestoDruid_Spells[]` |
| Rotation tiers | `static const RotationTier kRestoDruid_Rotation[]` (HoT-rolling tier shape) |
| Tree of Life rotation | `static const RotationTier kRestoDruid_RotationTOL[]` (separate table) |
| Cooldown Profile | `static const CooldownTrigger kRestoDruid_Cooldowns[]` |
| Mana Management | `ManaState` enum + threshold helpers |
| Leveling rotation | `LevelBracketRotation kRestoDruid_Leveling[]` |
| Lifebloom stack tracking | `LifebloomState` (target guid + stack count + remaining ms) on `AltbotAI` |
| Harmony buff tracking | aura listener for Mastery: Harmony buff |
| Clearcasting proc tracking | aura listener for Omen of Clarity buff |
| Tree of Life form state | `bool tree_of_life_active` on `AltbotAI` |

**Key new helper APIs:**
- `LifebloomStacks(target)` — read 0–3 stacks on a target.
- `LifebloomRemainingMs(target)` — read time-to-bloom.
- `HarmonyBuffRemainingMs(bot)` — read Harmony buff time remaining (0 if not active).
- `ClearcastingActive(bot)` — read Omen of Clarity proc state.
- `RejuvRefreshTarget(bot)` — selector returning ally with Rejuv expiring soon OR ally without Rejuv at low HP.
- `WildGrowthAnchor(bot)` — selector returning ally with most injured allies clustered within 30y.
- `SwiftmendTarget(bot)` — selector returning ally with active Rejuv/Regrowth AND lowest HP.
- `IsTreeOfLifeActive(bot)` — runtime state for rotation table selection.

Cross-reference to `docs/roles/healer.md` for triage hierarchy and the HoT-rolling pattern that this spec exemplifies.
