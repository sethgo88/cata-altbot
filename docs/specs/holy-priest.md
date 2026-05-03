# Holy Priest — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide. The YAML blocks below are designed to lift cleanly into `static const` tables in a future `AltbotCombat.cpp`.

**Numerical data** (spell IDs, mana %, cast times, cooldowns, durations, item IDs) verified against Wowhead's Cata 4.3.4 archive and Warcraft Wiki. Items the AI will need but that public archives don't pin down with confidence are tagged `UNVERIFIED`.

**Strategy content** reconciled across 9 community guides. See `docs/research/holy-priest-guide-survey.md` for the comparison table and which thresholds are consensus vs author-judgment.

---

## Identity

```yaml
spec: holy_priest
class: priest
role: HEALER
resource_model: mana
defining_mechanic: Chakra states (Sanctuary / Serenity / Chastise) — modal stance buffs that transform Holy Word
defining_buffs:
  - Inner_Fire             # +SP +armor (default healing posture)
  - Power_Word_Fortitude   # raid stamina buff
  - Chakra_Sanctuary       # default raid-heal mode (+15% AoE, summons HW: Sanctuary)
  - Chakra_Serenity        # tank-heal mode (+25% direct-heal crit, summons HW: Serenity)
defining_procs:
  - Surge_of_Light         # 3%/6% chance, next Flash Heal is free + instant
  - Test_of_Faith          # +12% healing on targets ≤50% HP
  - Inspiration            # 10% physical damage reduction proc on crit-heal
mastery: Echo_of_Light       # HoT-style follow-up, 10% of direct-heal over 6s
key_cooldowns:
  - Guardian_Spirit        # 3 min — tank panic CD: +60% healing received OR death-prevent
  - Divine_Hymn            # 8 min — channeled raid heal + buff
  - Hymn_of_Hope           # 6 min — channeled mana CD
  - Shadowfiend            # 5 min — pet melee returns mana
  - Lightwell              # 3 min — placeable raid-cooldown reservoir
  - Desperate_Prayer       # 2 min — instant 30% self-HP
ranged: 40
notes: |
  Holy Priest in 4.3.4 plays as the pre-eminent raid/AOE healer. Default mode
  is Chakra: Sanctuary (raid-heal) — entered by casting Prayer of Mending,
  it boosts AoE healing 15% and summons Holy Word: Sanctuary, an 18-second
  ground-AoE puddle that smart-heals up to 6 targets. Serenity is the
  alternate tank-heal mode, runtime-switchable.

  Rotation heartbeat in Sanctuary mode: PoM on cooldown, Circle of Healing
  on cooldown, Holy Word: Sanctuary on cooldown when raid clusters, Prayer
  of Healing on stacked-party damage, Heal as filler.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  TANK:
  PARTY_LOWEST_HP:
  PARTY_LOWEST_MISSING_HP:
  COH_ANCHOR:                  # ally with most injured allies within 30y
  POH_PARTY_TARGET:            # raid party where avg HP < 80
  HWSANC_CENTROID:             # raid centroid for Holy Word: Sanctuary placement
  RENEW_TANK_TARGET:           # tank as default Renew recipient
  ENEMY_TARGET:
```

---

## Spell Catalog

```yaml
spells:
  RENEW:
    id: 139
    mana_pct: 17               # heavy in 4.0 redesign
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: holy
    target_type: ally
    duration_ms: 12000         # 4 base ticks (5 with 12.5%+ haste)
    notes: HoT — primarily on tank; raid-Renew only with Surge of Light proc free use

  HEAL:
    id: 2050
    mana_pct: 14               # cheap mana-efficient default heal
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    notes: 4.0 "triage" filler — replaces Greater Heal as default

  GREATER_HEAL:
    id: 2060
    mana_pct: 32
    cast_time_ms: 3000
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    notes: heavy single-target heal; Serendipity 2-stack reduces cast/cost

  FLASH_HEAL:
    id: 2061
    mana_pct: 28               # mana-expensive
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    notes: emergency-only; Surge of Light proc makes free + instant

  PRAYER_OF_HEALING:
    id: 596
    mana_pct: 38
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: party_of_target
    radius: 30
    notes: party-wide heal (party of target, 30y); Serendipity 2-stack reduces cost

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
    notes: bouncing HoT; activates Chakra: Sanctuary on cast

  CIRCLE_OF_HEALING:
    id: 34861
    mana_pct: 21
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: true
    range: 40
    target_type: ally_anchor
    radius: 30
    targets: 5                 # 6 with Glyph of CoH
    notes: 5-target smart heal; Sanctuary Chakra reduces CD by 2s

  HOLY_WORD_SANCTUARY:
    id: 88685                  # UNVERIFIED — confirm via DBC
    mana_pct: 23
    cast_time_ms: 0
    cooldown_ms: 40000         # 28s with Tome of Light 2/2
    on_gcd: true
    range: 40
    target_type: ground_targeted
    duration_ms: 18000
    radius: 10                 # UNVERIFIED — guides give 10y, Wowhead may show different
    target_count: 6
    requires_chakra: SANCTUARY
    notes: ground-AoE puddle smart-heals up to 6 targets; only available in Sanctuary

  HOLY_WORD_SERENITY:
    id: 88684                  # UNVERIFIED
    mana_pct: 6
    cast_time_ms: 0
    cooldown_ms: 10000         # 7s with Tome of Light 2/2
    on_gcd: true
    range: 40
    target_type: ally
    requires_chakra: SERENITY
    notes: instant heal + +25% crit on next 8s heals on target; only in Serenity

  HOLY_WORD_CHASTISE:
    id: 88625                  # UNVERIFIED
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: true
    range: 30
    target_type: enemy
    requires_chakra: CHASTISE
    notes: damage variant — bot does NOT enter Chastise Chakra (DPS-spec only)

  CHAKRA:
    id: 14751
    mana_pct: 6
    cast_time_ms: 0
    cooldown_ms: 30000         # State of Mind talent reduces this
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 60000         # UNVERIFIED — see DBC verification queue
    notes: |
      Modal stance. Triggers on next-cast school: PoM/PoH activates Sanctuary,
      Heal/FlashHeal/GreaterHeal/BindingHeal activates Serenity, Smite/HolyFire
      activates Chastise. Bot defaults to Sanctuary via pre-pull PoM.

  LIGHTWELL:
    id: 724                    # talent-gated; check DBC
    mana_pct: 18
    cast_time_ms: 0
    cooldown_ms: 180000        # 3 min
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 180000
    charges: 10                # 15 with Glyph of Lightwell
    requires_talent: true
    notes: placeable healing reservoir; allies right-click to consume a charge

  GUARDIAN_SPIRIT:
    id: 47788
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 180000        # 3 min
    on_gcd: false
    range: 40
    target_type: ally
    duration_ms: 10000
    notes: +60% healing received OR death-prevent for 50% max HP. Tank panic CD.

  DIVINE_HYMN:
    id: 64843
    mana_pct: 36
    cast_time_ms: 8000         # channel
    cooldown_ms: 480000        # 8 min in Cata
    on_gcd: true
    range: 0
    target_type: self_channel
    targets: 5
    notes: heals 5 lowest-HP allies + 10% healing-received buff

  HYMN_OF_HOPE:
    id: 64901
    mana_pct: 0
    cast_time_ms: 8000         # channel
    cooldown_ms: 360000        # 6 min
    on_gcd: true
    range: 0
    target_type: self_channel
    notes: 8s channel; +15% max mana + 2% mana per tick to 3 lowest-mana allies

  SHADOWFIEND:
    id: 34433
    mana_pct: 6
    cast_time_ms: 0
    cooldown_ms: 300000        # 5 min
    on_gcd: false
    range: 30
    target_type: enemy
    duration_ms: 12000
    notes: pet melee; returns ~3% mana per hit

  DESPERATE_PRAYER:
    id: 19236                  # talent-gated tier 1
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 120000        # 2 min
    on_gcd: false
    range: 0
    target_type: self
    notes: instant 30% max HP self-heal

  POWER_WORD_SHIELD:
    id: 17
    mana_pct: 25
    cast_time_ms: 0
    cooldown_ms: 0             # 4s Weakened Soul lockout per target
    on_gcd: true
    range: 40
    target_type: ally
    duration_ms: 30000
    notes: |
      Holy doesn't get Disc's Rapture/Borrowed Time/Mastery synergy — small
      absorb only. Reactive use when Heal cast won't land in time.

  POWER_WORD_FORTITUDE:
    id: 21562
    mana_pct: 28
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: party_or_raid
    duration_ms: 3600000
    notes: raid stamina buff — pre-pull only

  INNER_FIRE:
    id: 588
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000       # 30 min
    notes: +SP +armor; default healing posture

  INNER_WILL:
    id: 73413
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    notes: |
      Mutually exclusive with Inner Fire. +10% movement, -15% mana cost on instants.
      Bot does NOT auto-swap; manual command override only.

  MASS_DISPEL:
    id: 32375
    mana_pct: 27
    cast_time_ms: 1500
    cooldown_ms: 15000
    on_gcd: true
    range: 30
    target_type: ground_targeted
    radius: 15
    notes: removes 1 magic effect from up to 10 friendly targets in area

  RESURRECTION:
    id: 2006
    mana_pct: 60
    cast_time_ms: 10000
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: corpse
    notes: out-of-combat resurrect

  # ─────────────────────────────────────────────────
  # Damage spells (solo questing + Tier 9 support DPS)
  # ─────────────────────────────────────────────────

  SMITE:
    id: 585
    mana_pct: 11
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: holy
    target_type: enemy
    notes: primary solo damage filler

  HOLY_FIRE:
    id: 14914
    mana_pct: 13
    cast_time_ms: 2000
    cooldown_ms: 10000
    on_gcd: true
    range: 30
    school: holy
    target_type: enemy
    duration_ms: 7000          # DoT
    notes: damage + DoT; on cooldown in solo / Tier 9

  SHADOW_WORD_PAIN:
    id: 589
    mana_pct: 22
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: shadow
    target_type: enemy
    duration_ms: 18000
    notes: instant DoT; maintain on enemy in solo

  MIND_BLAST:
    id: 8092
    mana_pct: 17
    cast_time_ms: 1500
    cooldown_ms: 8000
    on_gcd: true
    range: 30
    school: shadow
    target_type: enemy
    notes: solo nuke
```

---

## Maintenance pass

```yaml
maintenance:
  - name: Inner Fire self-buff
    when:
      - aura.missing(SELF, INNER_FIRE)
      - NOT runtime_flag.inner_will_mode
    cast: INNER_FIRE

  - name: Power Word: Fortitude raid buff (pre-combat)
    when:
      - NOT in_combat
      - aura.missing(SELF, POWER_WORD_FORTITUDE)
    cast: POWER_WORD_FORTITUDE on SELF

  - name: Renew on tank
    when:
      - target_resolves(TANK)
      - in_combat
      - aura.missing(TANK, RENEW) OR aura.expires_in_ms(TANK, RENEW) < 2000
    cast: RENEW on TANK

  - name: Chakra: Sanctuary entry (pre-pull)
    when:
      - NOT chakra_active
      - boss_pull_imminent OR in_combat
      - NOT runtime_flag.tank_healing_mode
    cast: PRAYER_OF_MENDING on TANK     # PoM activates Sanctuary
    notes: bot enters Sanctuary by casting PoM. SerenityChakra is opt-in.

  - name: Chakra: Sanctuary refresh
    when:
      - chakra_state == SANCTUARY
      - chakra_buff_remaining_ms < 10000
    cast: PRAYER_OF_MENDING on TANK     # refreshes Chakra duration

  - name: Lightwell pre-pull placement
    when:
      - NOT in_combat
      - boss_pull_imminent
      - spell.off_cooldown(LIGHTWELL)
      - lightwell_charges_remaining < 3
      - has_talent(LIGHTWELL)
    cast: LIGHTWELL at HWSANC_CENTROID  # raid centroid at engage time

  - name: Lightwell mid-fight recast
    when:
      - in_combat
      - spell.off_cooldown(LIGHTWELL)
      - lightwell_charges_remaining <= 2
      - has_talent(LIGHTWELL)
    cast: LIGHTWELL at HWSANC_CENTROID
```

---

## Rotation — Priority List (Sanctuary Chakra default)

```yaml
rotation_sanctuary:
  # ───────── Tier 1: Guardian Spirit (tank panic) ─────────
  - tier: 1
    name: Guardian Spirit on tank panic
    when:
      - target_resolves(TANK) AND TANK.hp_pct < 25
      - spell.off_cooldown(GUARDIAN_SPIRIT)
      - no_other_save_in_next_1500ms
    cast: GUARDIAN_SPIRIT on TANK
    notes: 3min CD panic button. Author-judgment HP gate.

  # ───────── Tier 1b: self-emergency Desperate Prayer ─────────
  - tier: 1b
    name: Desperate Prayer self-emergency
    when:
      - SELF.hp_pct < 35
      - spell.off_cooldown(DESPERATE_PRAYER)
    cast: DESPERATE_PRAYER

  # ───────── Tier 2: Holy Word: Sanctuary on CD ─────────
  - tier: 2
    name: Holy Word: Sanctuary
    when:
      - chakra_state == SANCTUARY
      - has_talent(REVELATIONS)
      - spell.off_cooldown(HOLY_WORD_SANCTUARY)
      - injured_allies_within_10y_of(HWSANC_CENTROID) >= 3
      - average_cluster_hp_pct < 80
    cast: HOLY_WORD_SANCTUARY at HWSANC_CENTROID

  # ───────── Tier 3: Prayer of Mending on CD ─────────
  - tier: 3
    name: Prayer of Mending on CD
    when:
      - spell.off_cooldown(PRAYER_OF_MENDING)
      - in_combat
    cast: PRAYER_OF_MENDING on TANK

  # ───────── Tier 4: Circle of Healing on CD ─────────
  - tier: 4
    name: Circle of Healing on cluster
    when:
      - spell.off_cooldown(CIRCLE_OF_HEALING)
      - injured_allies_within_30y_of_anchor(COH_ANCHOR) >= 3
      - average_cluster_hp_pct < 85
    cast: CIRCLE_OF_HEALING on COH_ANCHOR

  # ───────── Tier 5: Surge of Light proc consumption ─────────
  - tier: 5
    name: Free Flash Heal via Surge of Light
    when:
      - aura.active(SELF, SURGE_OF_LIGHT)
      - target_resolves(PARTY_LOWEST_HP)
      - PARTY_LOWEST_HP.hp_pct < 90
    cast: FLASH_HEAL on PARTY_LOWEST_HP
    notes: SoL proc has ~10s expiry — consume aggressively.

  # ───────── Tier 6: Prayer of Healing for stacked party ─────────
  - tier: 6
    name: Prayer of Healing on stacked party
    when:
      - target_resolves(POH_PARTY_TARGET)
      - party_of_target_avg_hp_pct < 80
      - injured_in_party >= 3
      - SELF.mana_pct > 40
    cast: PRAYER_OF_HEALING on POH_PARTY_TARGET

  # ───────── Tier 7: Greater Heal heavy single-target ─────────
  - tier: 7
    name: Greater Heal on heavy-damage target
    when:
      - target_resolves(PARTY_LOWEST_HP)
      - any of:
          - PARTY_LOWEST_HP.hp_pct < 60
          - serendipity_stacks == 2
          - PARTY_LOWEST_HP == TANK AND PARTY_LOWEST_HP.hp_pct < 70
      - SELF.mana_pct > 40
    cast: GREATER_HEAL on PARTY_LOWEST_HP

  # ───────── Tier 8: Flash Heal emergency ─────────
  - tier: 8
    name: Flash Heal emergency
    when:
      - target_resolves(PARTY_LOWEST_HP)
      - PARTY_LOWEST_HP.hp_pct < 35
      - SELF.mana_pct > 25
    cast: FLASH_HEAL on PARTY_LOWEST_HP

  # ───────── Tier 9: Heal filler ─────────
  - tier: 9
    name: Heal — efficient default
    when:
      - target_resolves(PARTY_LOWEST_HP)
      - PARTY_LOWEST_HP.hp_pct < 90
      - SELF.mana_pct > 25
    cast: HEAL on PARTY_LOWEST_HP

  # ───────── Tier 10: support DPS via Smite ─────────
  - tier: 10
    name: Support DPS — Smite/Holy Fire
    when:
      - target_resolves(ENEMY_TARGET)
      - SELF.mana_pct >= 75
      - party.lowest_hp_pct >= 95
      - chakra_buff_remaining_ms > 15000   # don't break Chakra
      - not currently_casting
      - no_predicted_damage_in_next_2s

    cast_priority:
      - condition: spell.off_cooldown(HOLY_FIRE)
        cast: HOLY_FIRE on ENEMY_TARGET
      - condition: aura.missing(ENEMY_TARGET, SHADOW_WORD_PAIN)
        cast: SHADOW_WORD_PAIN on ENEMY_TARGET
      - condition: SELF.mana_pct >= 80
        cast: SMITE on ENEMY_TARGET

    cancel_conditions:
      - cancel_if: any of tiers 1-3 becomes true
      - cancel_if: SELF.mana_pct drops below 60 mid-cast
```

---

## Rotation — Tank-Heal Mode (Serenity Chakra)

```yaml
rotation_serenity:
  # Activated when runtime_flag.tank_healing_mode == true.
  # Bot enters Serenity by casting Heal (which activates Serenity Chakra).
  # Same Tier 1 emergencies as Sanctuary mode.

  - tier: 2_serenity
    name: Holy Word: Serenity on CD on tank
    when:
      - chakra_state == SERENITY
      - has_talent(REVELATIONS)
      - spell.off_cooldown(HOLY_WORD_SERENITY)
      - target_resolves(TANK) AND TANK.hp_pct < 90
    cast: HOLY_WORD_SERENITY on TANK

  # Tiers 3-9: same as Sanctuary, but PARTY_LOWEST_HP biases toward tank
  # and Renew is maintained more aggressively (Serenity refreshes Renew on direct heals).
```

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: DIVINE_HYMN
    when:
      - in_combat
      - raid.injured_count >= 4
      - raid.avg_hp_pct < 60
      - spell.off_cooldown(DIVINE_HYMN)
    rationale: 8s channel; heaviest raid-heal CD. Author-judgment trigger.

  - spell: HYMN_OF_HOPE
    when:
      - in_combat
      - SELF.mana_pct < 40
      - no_active_emergency
      - spell.off_cooldown(HYMN_OF_HOPE)
    rationale: 8s channel mana CD. Fire when mana stress dictates.

  - spell: SHADOWFIEND
    when:
      - in_combat
      - SELF.mana_pct < 70
      - has_enemy_target
      - spell.off_cooldown(SHADOWFIEND)
    rationale: shorter CD than HoH; fire first when mana drops.
```

---

## Proc Reactions

```yaml
procs:
  SURGE_OF_LIGHT:
    talent_id: 33149            # UNVERIFIED
    triggered_by: Smite/Heal/FlashHeal/BindingHeal/GreaterHeal cast
    effect: next Flash Heal is free + instant + can crit (post-4.0.6)
    ai_behavior: Tier 5 consumes aggressively. ~10s expiry window.

  TEST_OF_FAITH:
    talent_id: 9381             # UNVERIFIED
    triggered_by: passive
    effect: +12% healing on targets ≤50% HP
    ai_behavior: passive throughput; no rotation logic needed.

  SERENDIPITY:
    talent_id: 47755            # UNVERIFIED
    triggered_by: Flash Heal / Binding Heal cast
    effect: stacks (max 2); -10% cast time + cost on next Greater Heal / PoH
    ai_behavior: |
      Tier 7 (Greater Heal) gates check for serendipity_stacks == 2 as one of
      the activation conditions; consume aggressively.

  INSPIRATION:
    talent_id: 14893            # UNVERIFIED
    triggered_by: crit-heal on ally
    effect: 10% physical damage reduction on target for 15s
    ai_behavior: passive; no rotation logic.

  SPIRIT_OF_REDEMPTION:
    talent_id: 20711            # UNVERIFIED
    triggered_by: priest dies
    effect: 15s ghost form, free heals
    ai_behavior: bot in spirit form continues healing rotation (no mana cost during form).

  ECHO_OF_LIGHT:
    mastery_passive: true
    triggered_by: direct heal cast
    effect: 10% (+1.25% per mastery point) of healing as HoT over 6s
    ai_behavior: |
      No special rotation logic. Direct-heal cadence unchanged. UNVERIFIED whether
      successive heals overwrite or add to the reservoir — see DBC verification.
```

---

## Mana Management

```yaml
mana:
  thresholds:
    drink_floor_pct: 30
    drink_resume_pct: 95
    hymn_of_hope_floor_pct: 40
    shadowfiend_floor_pct: 70
    conserve_mode_pct: 50
    crisis_mode_pct: 25

  conserve_behavior:
    when: SELF.mana_pct < 50
    rules:
      - skip Tier 6 (PRAYER_OF_HEALING) unless party.injured_in_party >= 4
      - skip Tier 7 (GREATER_HEAL) unless tank.hp_pct < 50
      - prefer Tier 9 (HEAL) as default cast
      - skip Tier 10 support DPS entirely

  crisis_behavior:
    when: SELF.mana_pct < 25
    rules:
      - skip Tiers 6-8
      - Tier 9 forced to HEAL on TANK only
      - fire MYTHICAL_MANA_POTION if off cooldown
      - fire HYMN_OF_HOPE regardless of emergency rule

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

  food:
    primary:
      item: Seafood_Magnifique_Feast
      item_id: 62290
    fallback:
      item: Severed_Sagefish_Head
      item_id: 62669
    apply: pre_combat AND not aura.active(SELF, well_fed_intellect)

  mana_potion:
    item: Mythical_Mana_Potion
    item_id: 57192
    use_when:
      - in_combat
      - SELF.mana_pct < 25
      - cooldown_shared_potion.off_cooldown

  healthstone:
    item: Healthstone
    item_id: 36892
    use_when:
      - SELF.hp_pct < 30
      - silenced OR cant_cast_for_ms > 1500
```

---

## Non-encounter content

### Solo combat — offensive rotation (Holy spec)

```yaml
solo_offensive_rotation:
  - tier: 1
    name: Self-preservation
    when:
      - SELF.hp_pct < 35
    cast: DESPERATE_PRAYER if off_cd else FLASH_HEAL on SELF

  - tier: 2
    name: Maintain SW:Pain DoT
    when:
      - aura.missing(target, SHADOW_WORD_PAIN) OR aura.expires_in_ms(target, SHADOW_WORD_PAIN) < 3000
    cast: SHADOW_WORD_PAIN on target

  - tier: 3
    name: Holy Fire on CD
    when:
      - spell.off_cooldown(HOLY_FIRE)
    cast: HOLY_FIRE on target

  - tier: 4
    name: Mind Blast on CD
    when:
      - spell.off_cooldown(MIND_BLAST)
      - SELF.mana_pct > 30
    cast: MIND_BLAST on target

  - tier: 5
    name: Shadowfiend mana sustain
    when:
      - spell.off_cooldown(SHADOWFIEND)
      - SELF.mana_pct < 60
    cast: SHADOWFIEND on target

  - tier: 6
    name: Smite filler
    when:
      - target.hp_pct > 0
      - SELF.mana_pct > 20
    cast: SMITE on target

solo_self_heal:
  thresholds:
    flash_heal_self_threshold: 50
    desperate_prayer_threshold: 35
```

### Travel & out-of-combat

```yaml
travel:
  mount:
    use_when:
      - not in_combat
      - distance_to_destination >= 80
      - mount_zone_allowed
    notes: AltbotMount module handles mount selection.

ooc_maintenance:
  - check: aura.missing(SELF, INNER_FIRE)
    action: cast INNER_FIRE
  - check: aura.missing(SELF, POWER_WORD_FORTITUDE)
    action: cast POWER_WORD_FORTITUDE
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
    - maintain INNER_FIRE
    - maintain POWER_WORD_FORTITUDE
  do_not:
    - do not cast offensive spells while idle
    - do not engage neutral mobs unprompted
```

### World boss / elite scaling

```yaml
world_boss_or_elite:
  rotation_override:
    if_in_party:
      use: encounter rotation (Sanctuary mode)
      thresholds:
        guardian_spirit_tank_hp: 35      # raised from 25 — elites hit harder
        crisis_mode_pct: 30
    if_solo:
      use: solo_offensive_rotation
      thresholds:
        flash_heal_self_threshold: 60
        desperate_prayer_threshold: 50
        do_not_engage_unless_can_solo: true
```

### Disengage / emergency utility

```yaml
disengage:
  desperate_prayer:
    use_when:
      - SELF.hp_pct < 35
      - spell.off_cooldown(DESPERATE_PRAYER)
    cast: DESPERATE_PRAYER

  fade:
    use_when:
      - SELF.has_aggro
      - SELF.hp_pct < 60
    cast: FADE on SELF
    notes: temporary threat-drop; bot escapes pull
```

---

## Leveling rotation

Pure Holy from level 10 (spec choice). Bot's mandate is dungeon healing.

### Spec recommendation

```yaml
spec_choice:
  for_dungeon_healer_bot:
    spec: Holy
    secondary_tree_split: 51 Holy + 5 (3 Shadow Darkness + 2 Disc Twin Disciplines)
    rationale: |
      Per survey: Darkness 3/3 (3% haste) crosses Renew breakpoint; Twin Disciplines
      2/2 (+4% spell damage/healing). Mental Agility (Disc) skipped — rotation mana
      is dominated by Greater Heal/PoH/HW:Sanctuary, none of which MA affects.
```

### Spell unlock table

```yaml
spell_unlocks:
  RENEW:                {level: 4,  spell_id: 139}
  POWER_WORD_SHIELD:    {level: 5,  spell_id: 17}
  POWER_WORD_FORTITUDE: {level: 6,  spell_id: 21562}
  INNER_FIRE:           {level: 7,  spell_id: 588}
  FLASH_HEAL:           {level: 3,  spell_id: 2061}
  HEAL:                 {level: 16, spell_id: 2050}
  HOLY_FIRE:            {level: 18, spell_id: 14914}
  RESURRECTION:         {level: 18, spell_id: 2006}
  SHADOW_WORD_PAIN:     {level: 3,  spell_id: 589}
  MIND_BLAST:           {level: 10, spell_id: 8092}
  PRAYER_OF_HEALING:    {level: 44, spell_id: 596}
  GREATER_HEAL:         {level: 38, spell_id: 2060}
  PRAYER_OF_MENDING:    {level: 68, spell_id: 33076}
  BINDING_HEAL:         {level: 64, spell_id: 32546}
  SHADOWFIEND:          {level: 68, spell_id: 34433}
  HYMN_OF_HOPE:         {level: 70, spell_id: 64901}
  MASS_DISPEL:          {level: 72, spell_id: 32375}
  LEAP_OF_FAITH:        {level: 85, spell_id: 73325}
  CHAKRA:               {level: 10, spell_id: 14751}        # Holy spec
  CIRCLE_OF_HEALING:    {level: 39, spell_id: 34861}        # Holy spec
  GUARDIAN_SPIRIT:      {level: 70, spell_id: 47788}        # Holy spec
  DIVINE_HYMN:          {level: 78, spell_id: 64843}        # Holy spec
  # Talented:
  LIGHTWELL:            {requires_talent: true, talent_tier: 3}
  REVELATIONS:          {requires_talent: true, talent_tier: 5}
  TOME_OF_LIGHT:        {requires_talent: true, talent_tier: 3}
  SURGE_OF_LIGHT:       {requires_talent: true, talent_tier: 4}
  TEST_OF_FAITH:        {requires_talent: true, talent_tier: 6}
  BODY_AND_SOUL:        {requires_talent: true, talent_tier: 5}
  INSPIRATION:          {requires_talent: true, talent_tier: 2}
  SPIRIT_OF_REDEMPTION: {requires_talent: true, talent_tier: 4}
  SERENDIPITY:          {requires_talent: true, talent_tier: 4}
  DESPERATE_PRAYER:     {requires_talent: true, talent_tier: 1}
  TWIN_DISCIPLINES:     {requires_talent: true, tree: discipline}
  DARKNESS:             {requires_talent: true, tree: shadow}
```

### Bracket 1 — Levels 1–3 (pre-Renew)

```yaml
bracket_1_3:
  available: SMITE, POWER_WORD_FORTITUDE, [some baseline heal — Lesser Heal pre-Cata, varies]
  rotation:
    - tier: 1: SELF.hp_pct < 25 -> Lesser Heal self
    - tier: 2: PARTY_LOWEST_HP.hp_pct < 70 -> Lesser Heal on PARTY_LOWEST_HP
    - tier: 3: idle -> follow tank
```

### Bracket 2 — Levels 4–9 (Renew unlocks)

```yaml
bracket_4_9:
  available: + RENEW (4), + POWER_WORD_SHIELD (5), + INNER_FIRE (7)
  maintenance:
    - INNER_FIRE always on (lvl 7+)
    - RENEW on tank
  rotation:
    - tier: 1: SELF.hp_pct < 25 -> Renew self + Lesser Heal
    - tier: 2: PARTY_LOWEST_HP.hp_pct < 50 -> Lesser Heal
    - tier: 3: tank Renew refresh
    - tier: 4: idle
```

### Bracket 3 — Levels 10–17 (Holy spec + Chakra + Mind Blast)

```yaml
bracket_10_17:
  available: + CHAKRA, + MIND_BLAST (lvl 10)
  maintenance:
    - INNER_FIRE
    - Chakra entry (PoM unlocks at 68 — pre-PoM, Chakra activates via Heal/PoH casts)
    - Pre-68: just cast Heal repeatedly to keep Serenity Chakra up
  notes: |
    Pre-PoM, Chakra is hard to maintain in Sanctuary mode. Bot defaults to
    Serenity (tank-heal) Chakra during early leveling — activated by direct
    heal casts. Switches to Sanctuary when PoM unlocks at level 68.
```

### Bracket 4 — Levels 18–37 (Holy Fire / Resurrection)

```yaml
bracket_18_37:
  available: + HOLY_FIRE, + RESURRECTION
  rotation_change: solo_offensive uses Holy Fire on CD
  maintenance: same as Bracket 3
```

### Bracket 5 — Levels 38–43 (Greater Heal)

```yaml
bracket_38_43:
  available: + GREATER_HEAL
  rotation_change: GREATER_HEAL becomes Tier 7 (heavy single-target heal)
```

### Bracket 6 — Levels 44–67 (Prayer of Healing + Circle of Healing at 39)

```yaml
bracket_44_67:
  available: + CIRCLE_OF_HEALING (39 — Holy spec), + PRAYER_OF_HEALING (44)
  rotation_change:
    - CoH becomes Tier 4 (cluster heal)
    - PoH becomes Tier 6 (stacked-party heal)
```

### Bracket 7 — Levels 68–69 (Prayer of Mending)

```yaml
bracket_68_69:
  available: + PRAYER_OF_MENDING (68)
  rotation_change:
    - PoM becomes Tier 3 (always on CD)
    - Bot now defaults to Sanctuary Chakra (PoM is the Sanctuary trigger)
```

### Bracket 8 — Levels 70–77 (Guardian Spirit + Hymn of Hope)

```yaml
bracket_70_77:
  available: + GUARDIAN_SPIRIT (70 — Holy), + HYMN_OF_HOPE (70)
  cooldown_addition:
    - GS at tank.hp_pct < 25
    - HoH at self.mana_pct < 40
```

### Bracket 9 — Levels 78–84 (Divine Hymn)

```yaml
bracket_78_84:
  available: + DIVINE_HYMN (78 — Holy)
  cooldown_addition:
    - DH at raid.injured_count >= 4
```

### Bracket 10 — Level 85 (Leap of Faith + full talent tree)

```yaml
bracket_85:
  available: + LEAP_OF_FAITH (85), + all talents fully allocated
  rotation: full encounter rotation per §"Rotation Sanctuary"
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
| 1 | Holy Word: Sanctuary spell ID | 88685 | Spell catalog | DBC lookup |
| 2 | Holy Word: Serenity spell ID | 88684 | Spell catalog | DBC lookup |
| 3 | Holy Word: Sanctuary radius | 10y | Cluster trigger | DBC EffectRadiusIndex |
| 4 | Echo of Light reservoir mechanic | overwrite (assumed) | mastery section | DBC + in-combat test |
| 5 | Chakra duration | 60s (assumed) | Chakra refresh threshold | DBC + Tome of Light interaction |
| 6 | Tome of Light extends Chakra duration | yes (assumed) | Chakra refresh logic | DBC + survey gap |
| 7 | Lightwell baseline charges | 10 (15 glyphed) | Lightwell maintenance | DBC |
| 8 | Renew tick count | 4 base, 5 with 12.5% haste | Mana / haste model | DBC |
| 9 | Inner Fire spell power scaling | 532 SP at 85 | Inner Fire spell | DBC + level scaling |
| 10 | Surge of Light talent ID | 33149 | Proc reactions | Talent.dbc |
| 11 | Test of Faith talent ID | 9381 | Proc reactions | Talent.dbc |
| 12 | Serendipity talent ID | 47755 | Proc reactions | Talent.dbc |
| 13 | Inspiration talent ID | 14893 | Proc reactions | Talent.dbc |
| 14 | Spirit of Redemption talent ID | 20711 | Proc reactions | Talent.dbc |
| 15 | Prayer of Mending unlock level | 68 | Leveling Bracket 7 | Wowhead Cata Classic |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kHolyPriest_Spells[]` |
| Rotation tiers | `static const RotationTier kHolyPriest_Rotation[]` |
| Cooldown Profile | `static const CooldownTrigger kHolyPriest_Cooldowns[]` |
| Mana Management | `ManaState` enum + threshold helpers |
| Leveling rotation | `LevelBracketRotation kHolyPriest_Leveling[]` |
| Chakra state tracking | `ChakraState` enum on `AltbotAI` (None / Sanctuary / Serenity / Chastise) |
| Lightwell anchor | `Position lightwell_anchor` on `AltbotAI`, default = raid centroid |
| Surge of Light proc | tracked via `aura.active(SELF, SURGE_OF_LIGHT)` aura listener |
| Serendipity stacks | `uint8 serendipity_stacks` on `AltbotAI` (0-2), updated via aura listener |

**Key new helper APIs:**
- `ChakraState(bot)` — read current Chakra mode.
- `SerendipityStacks(bot)` — read 0/1/2 stack count.
- `RaidCentroidWithin40y(bot)` — for Holy Word: Sanctuary placement and Lightwell anchor.
- `IsTankHealingMode(bot)` — runtime flag for Serenity-vs-Sanctuary.

Cross-reference to `docs/roles/healer.md` for triage hierarchy / dispel taxonomy.
