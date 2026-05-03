# Affliction Warlock — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide. The YAML blocks below are designed to lift cleanly into `static const` tables in a future `AltbotCombat.cpp`.

**Numerical data** verified against Wowhead's Cata 4.3.4 archive and Warcraft Wiki. Items the AI will need but that public archives don't pin down with confidence are tagged `UNVERIFIED`.

**Strategy content** reconciled across 22 community sources. See `docs/research/affliction-warlock-guide-survey.md` for the comparison table.

**SPEC-CHOICE RATIONALE.** Demonology is **rank #1-2 raid DPS** in 4.3.4 Dragon Soul; Affliction is **A-tier (rank #12-18)**; Destruction is C-tier. Project picks Affliction on rotation-engine simplicity + multi-target dominance + leveling viability (drain-tanking). Future option: a Demonology spec doc as a sibling for max-level raid optimization (mirrors the planned Fire-Mage sibling). This is the **second ranged-DPS spec** in the project.

---

## Identity

```yaml
spec: affliction_warlock
class: warlock
role: RANGED_DPS
resource_model: mana + soul_shards (max 3 in combat, no in-combat regen except Drain Soul kills)
defining_mechanic: DoT priority list + Haunt buff/debuff + Soul Swap target spread
defining_buffs:
  - Fel_Armor                # default armor
  - Demon_Armor              # tank/melee-heavy fallback
  - Shadow_Embrace           # +5% per stack (max 3) shadow DoT debuff on target
  - Haunt                    # +20% all shadow DoT damage to target (12s buff, 8s CD)
  - Dark_Intent              # +3% haste raid buff (assigned to ally)
defining_procs:
  - Eradication              # 6%/Corruption tick — +20% spellcasting haste 10s
  - Nightfall                # Shadow Bolt instant proc
mastery: Potent_Afflictions  # 13% baseline + 1.63%/point bonus periodic damage
key_cooldowns:
  - Demon_Soul               # 2 min, 20s; Felhunter: +20% periodic shadow
  - Soulburn                 # 45s, 15s buff; consumes 1 shard for empowered next cast
  - Doomguard                # 10 min, 45s pet (single-target boss)
  - Infernal                 # 10 min, 45s pet (AoE-mode)
  - Soulshatter              # 3 min, threat dump
  - Howl_of_Terror           # AoE fear, defensive
  - Death_Coil               # 2 min, self-heal + 3s horror
  - Demonic_Circle_Teleport  # mobility/escape
ranged: 30
notes: |
  Affliction Warlock in 4.3.4 plays as a DoT-priority caster with strong
  multi-target via Soul Swap (talent) and Soulburn-empowered Seed of
  Corruption. Single-target heartbeat: maintain Haunt + Bane of Doom +
  Corruption + Unstable Affliction; refresh handled passively via
  Everlasting Affliction (Shadow Bolt/Haunt/Drain Soul) and Pandemic
  (Drain Soul refreshes UA in execute). Shadow Bolt as filler maintains
  Shadow Embrace 3-stack debuff. At target HP ≤ 25%, swap to Drain Soul
  channel for ×2 damage + shard refund on kill.

  Pet: Felhunter (Spell Lock interrupt + Devour Magic dispel + best pet
  damage). Permanent; resummon on death. Demon Soul: Felhunter for the
  +20% periodic shadow damage buff aligned to opener.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  CURRENT_DPS_TARGET:
  ENEMY_CASTING_INTERRUPTIBLE:    # nearest enemy <30y casting interruptible — Felhunter Spell Lock
  CC_MARKER_BANISH:                # raid-target-icon: banish (warlock CC marker)
  CC_MARKER_FEAR:                  # raid-target-icon: fear
  AOE_CLUSTER:                     # ≥2 enemies within 15y of CURRENT_DPS_TARGET
  SOUL_SWAP_PRIMARY:               # current DoT-loaded target (source for swap)
  SOUL_SWAP_SECONDARY:             # next target (destination for spread)
  DARK_INTENT_TARGET:              # priority: Aff Lock > Boomkin > Shadow Priest > Mage > self
  MELEE_THREAT:                    # ≥1 enemy in 8y of bot
```

---

## Spell Catalog

```yaml
spells:
  # ─────── DoT-priority core ───────

  CORRUPTION:
    id: 172
    mana_pct: 6
    cast_time_ms: 0                # instant in Cata
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: shadow
    target_type: enemy
    duration_ms: 18000             # base; Improved Corruption talent extends
    notes: |
      Maintained on target. Refreshed automatically via Everlasting Affliction
      (Shadow Bolt / Haunt / Drain Soul). Bot only manually re-casts on falloff.

  HAUNT:
    id: 48181
    mana_pct: 0                    # consumes 1 Soul Shard
    cast_time_ms: 1500
    cooldown_ms: 8000
    on_gcd: true
    range: 30
    school: shadow
    target_type: enemy
    soul_shard_cost: 1
    duration_ms: 12000             # debuff: +20% shadow DoT damage
    requires_talent: true          # 31-Aff CAPSTONE
    notes: Centerpiece talent. Buff (heal-on-damage) + debuff (+20%). Refresh on CD.

  UNSTABLE_AFFLICTION:
    id: 30108
    mana_pct: 15
    cast_time_ms: 1500             # haste-affected
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: shadow
    target_type: enemy
    duration_ms: 15000             # base
    notes: |
      Maintained on target. Pandemic talent: Drain Soul refreshes UA at
      target HP ≤ 25%. Otherwise bot manually refreshes when remaining < 3s.

  BANE_OF_DOOM:
    id: 603
    mana_pct: 15
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: shadow
    target_type: enemy
    duration_ms: 60000             # 1 min, ticks every 15s
    bane_slot: true                # occupies Bane slot
    notes: |
      Default Bane. 20% chance per tick to summon Ebon Imp guardian.
      Swap to Bane of Agony when target time-to-live < 30s.

  BANE_OF_AGONY:
    id: 980
    mana_pct: 10
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: shadow
    target_type: enemy
    duration_ms: 24000
    bane_slot: true
    notes: short-fight Bane swap when ttl < 30s.

  CURSE_OF_THE_ELEMENTS:
    id: 1490
    mana_pct: 10
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: enemy
    duration_ms: 300000            # 5 min
    curse_slot: true               # occupies Curse slot (separate from Bane)
    notes: |
      8% magic damage taken raid debuff. Skip if Ebon Plaguebringer (Unholy DK),
      Earth and Moon (Balance Druid), or Master Poisoner (Assassination Rogue)
      is in group + in melee range of target.

  # ─────── Filler & resource generation ───────

  SHADOW_BOLT:
    id: 686
    mana_pct: 10
    cast_time_ms: 1700             # haste-affected; Nightfall proc instant
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: shadow
    target_type: enemy
    notes: |
      Default filler. Refreshes Corruption (Everlasting Affliction). Stacks
      Shadow Embrace +5% per (max 3). Glyph of Shadow Bolt: +5% damage.

  DRAIN_LIFE:
    id: 689
    mana_pct: 15
    cast_time_ms: 3000              # 3s channel
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: shadow
    target_type: enemy
    notes: leveling-only at max level. Soulburn-empowered: instant version. Self-heal channel.

  DRAIN_SOUL:
    id: 1120
    mana_pct: 14
    cast_time_ms: 15000             # 15s channel, 5 ticks (3s each)
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: shadow
    target_type: enemy
    notes: |
      Execute filler at target HP ≤ 25%. Damage ×2 in execute. Refunds 3 Soul
      Shards on XP/honor-yielding kill. Pandemic: refreshes UA. Everlasting
      Affliction: refreshes Corruption.

  FEL_FLAME:
    id: 77799
    mana_pct: 6
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: shadow_or_fire
    target_type: enemy
    notes: |
      Movement filler. Extends Immolate / UA on target by ~6s (UNVERIFIED
      exact extension). Used during forced movement only.

  # ─────── Multi-target ───────

  SOUL_SWAP:
    id: 86121
    mana_pct: 18
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: enemy
    soul_shard_cost: 1
    requires_talent: true          # Tier 4 Aff in Cata (NOT baseline; baseline in MoP)
    notes: |
      Removes shadow DoTs from current target → 20s window → "Soul Swap: Exhale"
      applies them to next target. With Glyph of Soul Swap: COPIES instead of
      removes. Mandatory glyph for raid-spread Aff.

  SEED_OF_CORRUPTION:
    id: 27243
    mana_pct: 34
    cast_time_ms: 2000
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: enemy
    duration_ms: 18000             # detonates on damage threshold OR expiry
    notes: |
      Detonates as AoE Shadow damage. Soulburn-empowered: applies Corruption
      to all hit. 3-5 target opener.

  RAIN_OF_FIRE:
    id: 5740
    mana_pct: 53
    cast_time_ms: 2000
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: ground_targeted
    radius: 8
    duration_ms: 8000
    notes: AoE Fire channel. Used in 6+ target sustained AoE.

  SHADOWFLAME:
    id: 47897
    mana_pct: 13
    cast_time_ms: 0
    cooldown_ms: 12000
    on_gcd: true
    range: 0
    target_type: cone_8y_frontal
    duration_ms: 8000              # DoT
    notes: melee-range cone DPS. Fire when in 8y of target.

  HELLFIRE:
    id: 1949
    mana_pct: 64
    cast_time_ms: 15000             # 15s channel
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: pbaoe_channel
    radius: 10
    notes: PBAoE channel. Self-damaging (~50% HP over channel). Niche/leveling.

  # ─────── Cooldowns ───────

  SOULBURN:
    id: 74434
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 45000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 15000
    soul_shard_cost: 1
    notes: |
      Empowers next cast: Seed of Corruption (applies Corruption to all hit),
      Soul Fire (instant), Drain Life (instant), demon summon (instant),
      Healthstone (+20% HP regen 8s).

  DEMON_SOUL:
    id: 77801
    mana_pct: 15
    cast_time_ms: 0
    cooldown_ms: 120000            # 2 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 20000
    requires_pet: true
    notes: |
      Effect varies by pet. Felhunter (Aff default): +20% periodic shadow
      damage. Imp: +30% crit on cast-time Destruction spells (NOT useful for Aff).

  SUMMON_DOOMGUARD:
    id: 18540
    mana_pct: 80                   # heavy
    cast_time_ms: 1500
    cooldown_ms: 600000            # 10 min
    on_gcd: true
    range: 0
    target_type: self_pet
    duration_ms: 45000
    notes: |
      Single-target boss CD. Snapshots SP — fire during opener with Bloodlust
      + Demon Soul + Volcanic Potion stacked.

  SUMMON_INFERNAL:
    id: 1122
    mana_pct: 80
    cast_time_ms: 1500
    cooldown_ms: 600000
    on_gcd: true
    range: 30
    target_type: ground_targeted
    duration_ms: 45000
    notes: AoE-mode swap of Doomguard. Same snapshot rule.

  DARK_INTENT:
    id: 85767
    mana_pct: 6
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: ally
    duration_ms: 1800000            # 30 min
    notes: |
      +3% haste on target ally; warlock gets +3% damage stacks per ally crit.
      Cast on highest-DPS DoT/HoT class ally at fight start.

  SUMMON_FELHUNTER:
    id: 691
    mana_pct: 80                   # uncategorized; via Soul Shard for instant
    cast_time_ms: 6000
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self_pet
    notes: standing pet. Soulburn-empowered: instant.

  SOULSHATTER:
    id: 29858
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 180000            # 3 min
    on_gcd: false
    range: 0
    target_type: self
    soul_shard_cost: 1
    notes: -50% threat to all enemies attacking caster. Threat dump.

  # ─────── Defensives & utility ───────

  SOULSTONE:
    id: 20707
    mana_pct: 35
    cast_time_ms: 3000
    cooldown_ms: 900000            # 15 min
    on_gcd: true
    range: 30
    target_type: ally_or_self
    soul_shard_cost: 1
    notes: |
      Combat-rez analog. Pre-applied to ally — they auto-rez at 30% HP/mana on
      death. Bot reserves 1 shard at all times for healer Soulstone.

  HEALTHSTONE:
    item_id: 36892
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: false
    range: 0
    target_type: self
    notes: instant self-heal. Created via CREATE_HEALTHSTONE.

  DEATH_COIL:
    id: 6789
    mana_pct: 23
    cast_time_ms: 0
    cooldown_ms: 120000
    on_gcd: true
    range: 30
    school: shadow
    target_type: enemy
    duration_ms: 3000               # 3s horror
    self_heal: true
    notes: damage + 3s horror + heals caster. Humanoid/Undead targets only.

  HOWL_OF_TERROR:
    id: 5484
    mana_pct: 13
    cast_time_ms: 1500
    cooldown_ms: 40000
    on_gcd: true
    range: 0
    target_type: pbaoe
    radius: 10
    duration_ms: 8000               # AoE fear
    notes: emergency AoE fear when ≥2 melee within 10y AND self.hp<60.

  FEAR:
    id: 5782
    mana_pct: 12
    cast_time_ms: 1700
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: enemy
    duration_ms: 20000              # PVE; 8s PvP
    notes: single-target fear. Manual command in 5-man; DISABLED in raid.

  BANISH:
    id: 710
    mana_pct: 8
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: enemy
    duration_ms: 30000              # PVE
    notes: demon/elemental CC. Auto on cc-marker == "banish" target.

  DEMONIC_CIRCLE_SUMMON:
    id: 48018
    mana_pct: 14
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 360000              # 6 min
    notes: places teleport anchor. Bot places pre-pull at safe spot.

  DEMONIC_CIRCLE_TELEPORT:
    id: 48020
    mana_pct: 8
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: false
    range: 40                        # 40y from circle (60y with glyph)
    target_type: self
    notes: teleport to anchor. Mobility/escape.

  LIFE_TAP:
    id: 1454
    mana_pct: 0                     # negative mana: spends HP, gains mana
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: false
    range: 0
    target_type: self
    notes: trade HP for mana. Auto when mana<50 AND hp>70 AND no_priority_cast.

  # ─────── Buffs & maintenance ───────

  FEL_ARMOR:
    id: 28176
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    notes: default armor. +SP + healing taken increased.

  CREATE_HEALTHSTONE:
    id: 6201
    mana_pct: 5
    cast_time_ms: 3000
    cooldown_ms: 60000
    on_gcd: true
    range: 0
    target_type: self
    notes: pre-pull conjure.

  CREATE_SOULSTONE:
    id: 693
    mana_pct: 35
    cast_time_ms: 3000
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    soul_shard_cost: 1
    notes: pre-pull conjure.

  SOUL_HARVEST:
    id: 79268                       # UNVERIFIED
    mana_pct: 0
    cast_time_ms: 9000              # 9s channel
    cooldown_ms: 45000
    on_gcd: true
    range: 0
    target_type: self_channel
    notes: out-of-combat shard regen — 1 shard per 3s for 9s.
```

---

## Maintenance pass

```yaml
maintenance:
  - name: Fel Armor
    when:
      - aura.missing(SELF, FEL_ARMOR)
    cast: FEL_ARMOR

  - name: Felhunter pet
    when:
      - pet.dead OR pet.absent
      - any of:
          - NOT in_combat
          - in_combat AND fight_remaining_estimate > 30000

    cast: SUMMON_FELHUNTER

  - name: Healthstone create
    when:
      - NOT in_combat
      - aura.missing_item(SELF, HEALTHSTONE)
    cast: CREATE_HEALTHSTONE

  - name: Soulstone create + apply
    when:
      - NOT in_combat
      - boss_pull_imminent_in_60s
      - soul_shards >= 2                # reserve 1 for active rotation
      - aura.missing_buff(designated_healer, SOULSTONE_RESURRECTION)
    cast: SOULSTONE on designated_healer

  - name: Demonic Circle pre-pull
    when:
      - NOT in_combat
      - boss_pull_imminent_in_60s
      - circle.expired OR circle.position_outdated
    cast: DEMONIC_CIRCLE_SUMMON at safe_position_near_pull

  - name: Curse of the Elements (skip if other class provides)
    when:
      - in_combat
      - target_resolves(CURRENT_DPS_TARGET)
      - aura.missing(CURRENT_DPS_TARGET, CURSE_OF_THE_ELEMENTS)
      - NOT (group_has_class_with_8pct_magic_debuff AND that_class_in_melee_range_of_target)
    cast: CURSE_OF_THE_ELEMENTS on CURRENT_DPS_TARGET
    notes: skip-if-redundant prevents double-application.

  - name: Dark Intent (raid buff handoff)
    when:
      - in_combat
      - level >= 83
      - dark_intent_target_resolves
      - aura.missing(DARK_INTENT_TARGET, DARK_INTENT)
    cast: DARK_INTENT on DARK_INTENT_TARGET
```

---

## Rotation — Priority List (Single-Target)

```yaml
rotation:
  # ───────── Tier 1: Haunt on CD ─────────
  - tier: 1
    name: Haunt — refresh on CD
    when:
      - has_talent(HAUNT)
      - spell.off_cooldown(HAUNT)
      - soul_shards >= 1                # reserve 1 for Soulstone
      - target_in_range(30)
      - any of:
          - aura.missing(CURRENT_DPS_TARGET, HAUNT_DEBUFF)
          - aura.expires_in_ms(CURRENT_DPS_TARGET, HAUNT_DEBUFF) < 2000
    cast: HAUNT on CURRENT_DPS_TARGET
    notes: centerpiece. Never let it fall off.

  # ───────── Tier 2: Bane of Doom ─────────
  - tier: 2
    name: Bane of Doom maintenance
    when:
      - target_in_range(30)
      - any of:
          - aura.missing(CURRENT_DPS_TARGET, BANE_OF_DOOM)
          - aura.expires_in_ms(CURRENT_DPS_TARGET, BANE_OF_DOOM) < 3000
      - target_ttl_estimate >= 30000     # else swap to Bane of Agony
    cast: BANE_OF_DOOM on CURRENT_DPS_TARGET

  # ───────── Tier 2b: Bane of Agony (short fights) ─────────
  - tier: 2b
    name: Bane of Agony — short-fight swap
    when:
      - target_in_range(30)
      - target_ttl_estimate < 30000
      - aura.missing(CURRENT_DPS_TARGET, BANE_OF_AGONY)
    cast: BANE_OF_AGONY on CURRENT_DPS_TARGET

  # ───────── Tier 3: Corruption ─────────
  - tier: 3
    name: Corruption maintenance (rare manual refresh)
    when:
      - target_in_range(30)
      - aura.missing(CURRENT_DPS_TARGET, CORRUPTION)
        # Everlasting Affliction normally refreshes via Shadow Bolt/Haunt/Drain Soul
    cast: CORRUPTION on CURRENT_DPS_TARGET

  # ───────── Tier 4: Unstable Affliction ─────────
  - tier: 4
    name: Unstable Affliction maintenance
    when:
      - target_in_range(30)
      - any of:
          - aura.missing(CURRENT_DPS_TARGET, UNSTABLE_AFFLICTION)
          - aura.expires_in_ms(CURRENT_DPS_TARGET, UNSTABLE_AFFLICTION) < 3000
      - NOT (CURRENT_DPS_TARGET.hp_pct <= 25 AND has_talent(PANDEMIC))
        # Pandemic refreshes UA via Drain Soul in execute
    cast: UNSTABLE_AFFLICTION on CURRENT_DPS_TARGET

  # ───────── Tier 5: Shadowflame (in melee range) ─────────
  - tier: 5
    name: Shadowflame on CD when in melee
    when:
      - distance_to(CURRENT_DPS_TARGET) <= 8
      - spell.off_cooldown(SHADOWFLAME)
    cast: SHADOWFLAME at CURRENT_DPS_TARGET
    notes: cone-AoE; only fires when geometry permits.

  # ───────── Tier 6: Drain Soul (execute) ─────────
  - tier: 6
    name: Drain Soul — execute filler at <=25% HP
    when:
      - CURRENT_DPS_TARGET.hp_pct <= 25
      - target_in_range(30)
      - SELF.mana_pct > 25
    cast: DRAIN_SOUL on CURRENT_DPS_TARGET
    notes: |
      ×2 damage. Refreshes UA (Pandemic) + Corruption (Everlasting Affliction).
      Refunds 3 shards on XP/honor-yielding kill.

  # ───────── Tier 7: Shadow Bolt filler ─────────
  - tier: 7
    name: Shadow Bolt — default filler
    when:
      - target_in_range(30)
      - SELF.mana_pct > 25
      - CURRENT_DPS_TARGET.hp_pct > 25    # else Tier 6 takes over
    cast: SHADOW_BOLT on CURRENT_DPS_TARGET
    notes: refreshes Corruption + stacks Shadow Embrace +5% per (max 3).
```

---

## AoE Mode

Activated by `enemies_within_15y_of(CURRENT_DPS_TARGET) >= 2`. Hysteresis: exit after `<2 sustained for 5s`.

```yaml
rotation_aoe:
  - tier: 1: 2 targets -> Soul Swap (Glyph: copy DoTs to second target). Maintain DoTs on both.
  - tier: 2: 3-5 targets -> Soulburn AND off_CD -> Seed of Corruption (applies Corruption to all hit). Then Soul Swap to spread UA/BoD on 1-2 priority targets.
  - tier: 3: 6+ targets -> Seed of Corruption spam (no Soulburn after first; shards depleted)
  - tier: 4: in_melee_range -> Shadowflame on CD
  - tier: 5: 6+ sustained -> Rain of Fire channel
notes: Bane of Havoc is NOT in Aff loadout (Destruction Tier-6 talent). Aff multi-target = Soul Swap + Soulburn-Seed.
```

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: DEMON_SOUL
    when:
      - in_combat
      - level >= 85
      - pet.alive
      - spell.off_cooldown(DEMON_SOUL)
      - any of:
          - boss_engagement OR fight_just_started
          - cooldown_window_aligned (Bloodlust + trinkets)
    rationale: 2-min CD. Felhunter: +20% periodic shadow damage. Opener priority.

  - spell: SUMMON_DOOMGUARD
    when:
      - in_combat
      - target.is_boss
      - target_ttl_estimate > 60000
      - spell.off_cooldown(SUMMON_DOOMGUARD)
      - aura.active(SELF, DEMON_SOUL) OR aura.active(SELF, BLOODLUST_EFFECT)
    rationale: 10-min CD; snapshots SP. Fire with all CDs aligned.

  - spell: SUMMON_INFERNAL
    when:
      - aoe_mode == true
      - enemies_in_aoe_cluster >= 4
      - aoe_phase_estimated_duration > 20000
      - spell.off_cooldown(SUMMON_INFERNAL)
    rationale: AoE-mode swap of Doomguard.

  - spell: SOULBURN
    when:
      - any of:
          - aoe_mode_entry AND about_to_cast(SEED_OF_CORRUPTION)    # AoE opener
          - pet.dead AND combat AND fight_remaining > 30000          # instant pet revive
          - SELF.hp_pct < 35 AND no_other_save AND has_healthstone   # +20% HP regen
      - soul_shards >= 1
      - spell.off_cooldown(SOULBURN)
    rationale: 45s CD shard consumer. Multiple use cases.

  - spell: SOULSHATTER
    when:
      - SELF.threat_pct > 90
      - NOT main_threat_intended
      - soul_shards >= 1
      - spell.off_cooldown(SOULSHATTER)
    rationale: 3-min CD threat dump.

  - spell: DEATH_COIL
    when:
      - SELF.hp_pct < 50
      - target.creature_type in (humanoid, undead)
      - spell.off_cooldown(DEATH_COIL)
    rationale: self-heal + 3s horror.

  - spell: HOWL_OF_TERROR
    when:
      - melee_count_within_10y >= 2
      - SELF.hp_pct < 60
      - spell.off_cooldown(HOWL_OF_TERROR)
    rationale: emergency AoE fear break.

  - spell: DEMONIC_CIRCLE_TELEPORT
    when:
      - circle.placed AND circle.alive
      - any of:
          - melee_count_within_8y >= 2
          - SELF.hp_pct < 25
      - spell.off_cooldown(DEMONIC_CIRCLE_TELEPORT)
    rationale: mobility/escape.

  - spell: SOUL_HARVEST
    when:
      - NOT in_combat
      - soul_shards < 3
      - spell.off_cooldown(SOUL_HARVEST)
      - boss_pull_imminent_in_30s OR after_combat_cleanup
    rationale: pre-pull / post-combat shard regen.
```

---

## Proc Reactions

```yaml
procs:
  ERADICATION:
    talent_id: 47195             # UNVERIFIED
    triggered_by: Corruption tick (6% chance)
    effect: +20% spellcasting haste 10s
    ai_behavior: |
      Bot delays DoT refresh ≤1.5s when Eradication proc is imminent (refresh
      window). DoTs snapshot on cast — refreshing during Eradication captures
      the haste for the new application.

  NIGHTFALL:
    talent_id: 18094             # UNVERIFIED
    triggered_by: Corruption tick (4%) OR Drain Life tick (4%)
    effect: next Shadow Bolt is instant + free
    ai_behavior: bot consumes proc immediately (instant cast = pure throughput add).

  EBON_IMP_SUMMON:
    triggered_by: Bane of Doom tick (20%)
    effect: temporary Ebon Imp guardian
    ai_behavior: passive — pet AI handles.

  SHADOW_EMBRACE:
    talent_passive: true
    triggered_by: Shadow Bolt / Haunt cast
    effect: +5% per stack (max 3) shadow DoT debuff on target
    ai_behavior: |
      Bot prioritizes Shadow Bolt over Drain Life when SE stacks < 3.
      Haunt cast counts toward stack; Drain Soul does NOT.

  EVERLASTING_AFFLICTION:
    talent_passive: true
    triggered_by: Shadow Bolt / Haunt / Drain Soul on target with Corruption
    effect: refreshes Corruption duration to full
    ai_behavior: passive — bot doesn't manually refresh Corruption normally.

  PANDEMIC:
    talent_passive: true
    triggered_by: Drain Soul on target with UA (target HP ≤25%)
    effect: refreshes UA duration
    ai_behavior: passive — bot skips manual UA refresh in execute.

  POTENT_AFFLICTIONS_MASTERY:
    mastery_passive: true
    effect: 13% baseline + 1.63%/point bonus periodic damage
    ai_behavior: passive.
```

---

## Mana Management

```yaml
mana:
  thresholds:
    drink_floor_pct: 30
    drink_resume_pct: 95
    life_tap_floor_pct: 50
    life_tap_hp_floor_pct: 70
    conserve_mode_pct: 50
    crisis_mode_pct: 25

  conserve_behavior:
    when: SELF.mana_pct < 50
    rules:
      - skip Tier 5 Shadowflame (12% CD spend on AoE that's not in priority)
      - skip Rain of Fire / Hellfire even in AoE-mode
      - prefer Drain Life over Shadow Bolt when self.hp<60 (sustains both HP and tempo)

  crisis_behavior:
    when: SELF.mana_pct < 25
    rules:
      - prioritize Life Tap when no priority cast
      - skip Curse of the Elements maintenance (don't refresh)
      - fire MYTHICAL_MANA_POTION
      - keep DoTs up; let Shadow Bolt filler run dry

  out_of_combat:
    drink_when:
      - not in_combat
      - SELF.mana_pct < drink_floor_pct
      - no enemies within 40y for 5000ms
    eat_when: out_of_combat AND SELF.hp_pct < 80

soul_shards:
  max_in_combat: 3
  no_in_combat_regen: true       # except Drain Soul XP-yielding kills
  reservation: 1                 # always reserve 1 for Soulstone
  consumers:
    - HAUNT (1 shard, 8s CD)
    - SOUL_SWAP (1 shard)
    - SOULBURN (1 shard, 45s CD)
    - SOULSTONE (1 shard, 15-min CD)
    - SOULSHATTER (1 shard, 3-min CD)
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
      item: Severed_Sagefish_Head
      item_id: 62669
    fallback:
      item: Seafood_Magnifique_Feast
      item_id: 62290

  dps_potion:
    item: Volcanic_Potion
    item_id: 58091
    use_when:
      - boss_pull_imminent_in_2s            # pre-pot
      - OR (in_combat AND fight_elapsed_ms ~ 60000 AND demon_soul_off_cd AND doomguard_off_cd)

  mana_potion:
    item: Mythical_Mana_Potion
    item_id: 57192
    use_when:
      - in_combat
      - SELF.mana_pct < 25

  healthstone:
    item_id: 36892
    use_when:
      - SELF.hp_pct < 30
      - silenced OR cant_cast_for_ms > 1500
```

---

## Non-encounter content

### Solo combat / Drain-tanking (leveling viability)

Affliction is the strongest leveling spec for warlocks via drain-tanking.

```yaml
solo_offensive_rotation:
  - tier: 1: SELF.hp_pct < 20 -> SOULBURN-Healthstone if shards else FEAR + retreat
  - tier: 2: pet.alive AND pet.hp<40 -> HEALTH_FUNNEL self
  - tier: 3: aura.missing(target, CORRUPTION) -> CORRUPTION
  - tier: 4: aura.missing(target, BANE_OF_AGONY) -> BANE_OF_AGONY (or BoD if level>=30)
  - tier: 5: aura.missing(target, UNSTABLE_AFFLICTION) AND has_talent(UA) -> UA
  - tier: 6: SELF.hp_pct < 60 AND target.hp>30 -> DRAIN_LIFE channel
  - tier: 7: target.hp_pct <= 25 -> DRAIN_SOUL (kill + shard refund)
  - tier: 8: SELF.mana > 25 -> SHADOW_BOLT
  - tier: 9: SELF.mana <= 25 -> LIFE_TAP if hp>60 else wand auto-attack
notes: Pet — Voidwalker until ~30 (tank); switch to Felhunter from 30+.
```

### Travel & out-of-combat

```yaml
travel:
  mount:
    use_when:
      - not in_combat
      - distance_to_destination >= 80
      - mount_zone_allowed
    notes: AltbotMount module.

ooc_maintenance:
  - check: aura.missing(SELF, FEL_ARMOR) -> cast FEL_ARMOR
  - check: pet.dead AND not_in_combat -> SUMMON_FELHUNTER
  - check: aura.missing_item(SELF, HEALTHSTONE) -> CREATE_HEALTHSTONE
  - check: SELF.mana_pct < 80 AND no_enemies_within(40) -> drink
  - check: SELF.hp_pct < 95 AND no_enemies_within(40) -> eat
```

### Idle

```yaml
idle:
  primary_action: follow master
  secondary_actions:
    - maintain FEL_ARMOR
    - maintain Felhunter pet
    - regen via Soul Harvest if shards<3 and pre-pull window
  do_not:
    - do not cast offensive spells while idle
    - do not Banish neutral mobs
```

### World boss / elite scaling

```yaml
world_boss_or_elite:
  rotation_override:
    if_in_party:
      use: encounter rotation
      thresholds:
        soulburn_healthstone_threshold: 50      # raised — elites hit harder
        crisis_mode_pct: 30
    if_solo:
      use: solo_offensive_rotation
      thresholds:
        drain_life_self_threshold: 70           # drain-tank more aggressively
        do_not_engage_unless_can_solo: true
```

### Disengage / emergency utility

```yaml
disengage:
  demonic_circle_teleport:
    use_when:
      - circle.placed
      - melee_count_within_8y >= 2 OR self.hp<25
    cast: DEMONIC_CIRCLE_TELEPORT

  howl_of_terror:
    use_when:
      - melee_count_within_10y >= 2
      - self.hp_pct < 60
      - off_cd
    cast: HOWL_OF_TERROR
```

---

## Leveling rotation

Pure Affliction from level 10 (spec choice). Drain-tank from low levels.

### Spec recommendation

```yaml
spec_choice:
  for_leveling_dungeon_dps_bot:
    spec: Affliction
    talent_split: 31 Affliction / 0 Demonology / 10 Destruction
    talent_picks_aff: Doom and Gloom + Improved Life Tap + Improved Corruption + Jinx + Soul Siphon + Siphon Life + Eradication + Soul Swap + Shadow Embrace + Death's Embrace + Nightfall + Soulburn-Seed + Everlasting Affliction + Pandemic + Haunt
    talent_picks_destro: Bane (Bane CD reduction) + Shadow and Flame (+12% Shadow Bolt) + Improved Soul Fire OR Emberstorm
    rationale: |
      Per survey: 31/0/10 (gotwarcraft G13 explicit build). 31 Aff capstone
      = Haunt. 10 Destruction unlocks Shadow and Flame which buffs primary
      filler (Shadow Bolt) by 12% — highest-DPS use of off-spec points.
```

### Leveling brackets

```yaml
bracket_1_3:
  available: SHADOW_BOLT, IMP, FEL_ARMOR (lvl 1)
  rotation: SHADOW_BOLT spam; pet on offense
  notes: pre-Corruption; bot is single-cast caster

bracket_4_7:
  available: + CORRUPTION (4), + LIFE_TAP (6), + CURSE_OF_WEAKNESS (6)
  rotation:
    - tier 1: aura.missing(target, CORRUPTION) -> CORRUPTION
    - tier 2: SHADOW_BOLT filler
  notes: DoT-priority enters; bot establishes Aff identity

bracket_8_9:
  available: + SUMMON_VOIDWALKER (8), + FEAR (8), + DRAIN_LIFE (9)
  pet_change: switch from Imp to Voidwalker (tank)
  rotation_addition: DRAIN_LIFE when self.hp<60

bracket_10_13:
  available: + DRAIN_SOUL (10), + BANE_OF_AGONY (10), + UNSTABLE_AFFLICTION (10 if Aff-spec), + HEALTHSTONE
  rotation_change:
    - DoT priority: Corruption + Bane of Agony + UA (talent-gated)
    - Drain Soul to finish kills (shard refund)

bracket_12_17:
  available: + CURSE_OF_THE_ELEMENTS (12), + HOWL_OF_TERROR (14), + CURSE_OF_TONGUES (16)
  rotation_addition: CotE on tougher targets

bracket_18_29:
  available: + BANISH (18), + SOULSTONE (18), + DEMON_ARMOR (20), + SUMMON_SUCCUBUS (20), + DEATH_COIL (24)
  rotation_addition: Banish on cc-marker; Death Coil emergency self-heal

bracket_28_29:
  available: + SOULBURN (28)
  rotation_addition: Soulburn-Drain-Life for instant heavy heal

bracket_30_49:
  available: + BANE_OF_DOOM (30), + SUMMON_FELHUNTER (30)
  pet_change: switch from Voidwalker to Felhunter (highest pet DPS)
  rotation_change:
    - swap Bane of Agony for Bane of Doom on long-fight targets

bracket_38_55:
  available: + RAIN_OF_FIRE (38), + HELLFIRE (40)
  rotation_addition: AoE-mode entry on ≥3 enemies

bracket_50_59:
  available: + FEL_ARMOR (50), + SUMMON_INFERNAL (50), + SOULSHATTER (50), + SEED_OF_CORRUPTION (50)
  maintenance_change: Fel Armor replaces Demon Armor as default

bracket_56_59:
  available: + SHADOWFLAME (56)
  rotation_addition: Shadowflame in melee range

bracket_60_80:
  available: + SUMMON_DOOMGUARD (60), + DEMONIC_CIRCLE_SUMMON/TELEPORT (60)
  cooldown_addition: Doomguard for boss; Demonic Circle for mobility

bracket_81_84:
  available: + FEL_FLAME (81), + DARK_INTENT (83)
  rotation_addition: Fel Flame for movement; Dark Intent on raid buff target

bracket_85:
  available: + DEMON_SOUL (85), + all talents fully allocated
  rotation: full encounter rotation per §"Rotation"
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
| 1 | Mastery Potent Afflictions formula | 13% + 1.63%/pt | Mastery passive | Spell.dbc / chrclasses.dbc |
| 2 | Haunt damage + heal coefficient | TBD | Haunt catalog | Spell.dbc 48181 |
| 3 | Haunt 20% debuff stack rule | additive vs SE? | Throughput math | Spell.dbc + aura interaction |
| 4 | Drain Soul ×2 damage at ≤25% | yes | Tier 6 throughput | Spell.dbc 1120 |
| 5 | Drain Soul shard refund predicate | XP/honor yielding | Shard model | Server XP eligibility |
| 6 | Pandemic refresh predicate | UA at ≤25% | Tier 4 skip | Talent.dbc |
| 7 | Everlasting Affliction rank scaling | 33%/66%/100% | Corruption refresh logic | Talent.dbc |
| 8 | Bane of Doom Ebon Imp rate | 20% per tick | Catalog | Spell.dbc 603 |
| 9 | Eradication proc rate | 6% per Corruption tick | Proc reactions | Talent.dbc |
| 10 | Soul Swap Tier 4 confirmed | yes | Talent gating | Talent.dbc |
| 11 | Soul Swap Glyph effect | copy not transfer | Multi-target rotation | Glyph.dbc |
| 12 | Soulburn 45s/1 shard/15s | yes | Catalog | Spell.dbc 74434 |
| 13 | Demon Soul Felhunter +20% periodic | yes | Cooldown profile | Spell.dbc 77801 |
| 14 | Doomguard / Infernal 10 min CD + 45s pet | yes | Catalog | Spell.dbc |
| 15 | CotE 8% magic vs other class debuffs | overlap rule | Maintenance skip | Spell.dbc + aura mod |
| 16 | CotE vs Bane slot independence | yes | Catalog | Aura category check |
| 17 | Shadow Embrace 5%/stack max 3 | yes | Tier 7 priority | Talent.dbc |
| 18 | Shadow and Flame +12% at 3/3 | yes (build justification) | 31/0/10 build | Talent.dbc |
| 19 | Glyph effects (Haunt, Corruption, UA, Life Tap, Shadow Bolt, Soul Swap) | various | Glyph picks | Glyph.dbc |
| 20 | Drain Soul 3-shard refund on XP kill | yes | Shard model | Spell.dbc + server logic |
| 21 | Soul Harvest 9s / 1 shard per 3s / 45s CD | yes | OOC catalog | Spell.dbc |
| 22 | Bane of Havoc Destruction Tier-6 (NOT Aff) | confirmed | Spec exclusion | Talent.dbc |
| 23 | Soulshatter cooldown (3 min assumed) | TBD | Threat dump | Spell.dbc |
| 24 | Fel Flame DoT extension on UA | +6s assumed | Movement filler | Spell.dbc effect mod |
| 25 | Howl of Terror 8s fear / 10y AoE / 40s CD | TBD | Defensive | Spell.dbc 5484 |
| 26 | Banish PVE duration (30s assumed) | TBD | CC | Spell.dbc 710 |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kAffWarlock_Spells[]` |
| Rotation tiers | `static const RotationTier kAffWarlock_Rotation[]` |
| AoE rotation | `static const RotationTier kAffWarlock_RotationAoE[]` (separate table) |
| Cooldown Profile | `static const CooldownTrigger kAffWarlock_Cooldowns[]` |
| Mana Management | `ManaState` enum + threshold helpers |
| Soul Shard tracking | `uint8 soul_shards` on `AltbotAI` (0-3) + reservation rule |
| Leveling rotation | `LevelBracketRotation kAffWarlock_Leveling[]` |
| Pet state | `PetState` (Felhunter alive/dead, Spell Lock CD) |
| Bane vs Curse slot tracking | independent slot trackers per target |
| Shadow Embrace stack tracking | aura listener for SE stacks (0-3) |
| Eradication / Nightfall proc tracking | aura listeners |
| Dark Intent target selector | priority chain (Aff Lock > Boomkin > etc.) |

**Key new helper APIs:**
- `SoulShards(bot)` — read 0-3.
- `BaneSlot(target)` — read which Bane is active.
- `CurseSlot(target)` — read which Curse is active.
- `ShadowEmbraceStacks(target)` — read 0-3.
- `HasGroupClassWith8PctMagicDebuff()` — for CotE skip predicate.
- `DarkIntentTarget(group)` — priority-chain selector.
- `IsExecutePhase(target)` — `target.hp_pct <= 25`.

Cross-reference to `docs/roles/ranged-dps.md` for range management / target priority / interrupt rotation patterns. Cross-reference to `docs/specs/frost-mage.md` for the prior ranged-DPS spec doc shape.
