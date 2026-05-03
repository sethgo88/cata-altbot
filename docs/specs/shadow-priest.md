# Shadow Priest — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide.

**Numerical data** verified against Wowhead's Cata 4.3.4 archive and Warcraft Wiki. Items tagged `UNVERIFIED` need DBC verification.

**Strategy content** reconciled across 12 community sources. See `docs/research/shadow-priest-guide-survey.md`.

**SPEC-CHOICE RATIONALE.** Shadow is the only PVE-DPS-viable Priest spec in 4.3.4. Holy and Discipline are healers (covered separately). Shadow is **B-tier (rank #14)** in Dragon Soul — mid-tier sustained DPS with strong utility (Vampiric Embrace raid heal, Dispersion mana CD, Hymn of Hope, Mass Dispel). This is the **fourth ranged-DPS spec** in the project.

---

## Identity

```yaml
spec: shadow_priest
class: priest
role: RANGED_DPS
resource_model: mana + shadow_orbs (max 3, generated 10%/tick on SW:P + Mind Flay; consumed by Mind Blast)
defining_mechanic: DoT priority + Shadow Orb → Mind Blast → Empowered Shadow snapshot + Dark Evangelism → Archangel
defining_buffs:
  - Shadowform                  # +15% Shadow damage, -15% damage taken
  - Inner_Fire                  # +SP flat
  - Empowered_Shadow            # +20-22% periodic damage 15s after orbs consumed
  - Dark_Evangelism             # +2% per stack (5 stacks max) damage to MF/MS/MB/SW:D
  - Vampiric_Embrace            # 6% caster + 3% raid heal from Shadow damage (toggle)
defining_procs:
  - Shadow_Orbs                 # mastery resource; consumed by Mind Blast
  - Shadowy_Apparition          # 4/8/12% per SW:P tick — chasing visuals
  - Pain_and_Suffering          # 60%/Mind Flay tick refreshes SW:P
mastery: Shadow_Orb_Power       # +1.3%/pt to Empowered Shadow's DoT modifier
key_cooldowns:
  - Mind_Blast                  # 6.5s CD with Improved Mind Blast 3/3
  - Shadowfiend                 # 5 min, 15s pet, mana CD + DPS
  - Dispersion                  # 2 min, 6s, +6%/sec mana, -90% damage
  - Hymn_of_Hope                # 6 min, raid mana CD
  - Archangel                   # 1.5 min, consumes 5 Dark Evangelism stacks
  - Shadow_Word_Death           # 10s CD execute below 25%
  - Power_Word_Shield           # self-shield, Weakened Soul
  - Fade                        # threat dump
ranged: 40
notes: |
  Shadow Priest in 4.3.4 plays as a DoT-priority caster with a stacking proc
  resource (Shadow Orbs). Heartbeat: maintain SW:Pain + Vampiric Touch +
  Devouring Plague; Mind Blast on CD consumes orbs to apply Empowered Shadow
  (+20-22% DoT damage 15s); refresh DoTs in the snapshot window. Mind Flay
  filler builds Dark Evangelism stacks; at 5 stacks fire Archangel for
  +20% damage / +5% mana.

  Mind Spike is NOT cast in standard rotation — removes self DoTs.

  Pre-Shadowform leveling (under L40): Smite + SW:P only.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  CURRENT_DPS_TARGET:
  ENEMY_CASTING_INTERRUPTIBLE:
  AOE_CLUSTER:                  # ≥3 enemies within 10y of target (Mind Sear)
  SECONDARY_DOT_TARGET:         # priority adds for multi-DoT spread
```

---

## Spell Catalog

```yaml
spells:
  # ─────── DoT priority core ───────

  SHADOW_WORD_PAIN:
    id: 589
    mana_pct: 22
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: shadow
    target_type: enemy
    duration_ms: 18000
    tick_ms: 3000
    notes: Pain and Suffering 60%/MF tick auto-refreshes. Apply once per target.

  VAMPIRIC_TOUCH:
    id: 34914
    mana_pct: 16
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: shadow
    target_type: enemy
    duration_ms: 15000
    tick_ms: 3000
    requires_talent: true
    notes: Mind Blast on VT-affected target → Replenishment (1% mana/10s raid).

  DEVOURING_PLAGUE:
    id: 2944
    mana_pct: 25
    cast_time_ms: 0                # INSTANT
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: shadow
    target_type: enemy
    duration_ms: 24000
    tick_ms: 3000
    notes: Improved DP: up to 30% upfront damage on cast.

  # ─────── Damage casts ───────

  MIND_BLAST:
    id: 8092
    mana_pct: 17
    cast_time_ms: 1500
    cooldown_ms: 6500              # 8s baseline → 6.5s with Improved MB 3/3
    on_gcd: true
    range: 40
    school: shadow
    target_type: enemy
    notes: Consumes ALL Shadow Orbs; +10% damage per orb. Applies Empowered Shadow (15s, +20-22% DoT damage).

  MIND_FLAY:
    id: 15407
    mana_pct: 8
    cast_time_ms: 3000              # 3s channel, 1s tick (3 ticks)
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: shadow
    target_type: enemy
    notes: |
      Filler. Each tick: 60% Pain and Suffering (refresh SW:P), 10% Shadow
      Orb generation, 1 Dark Evangelism stack. 50% slow.

  MIND_SEAR:
    id: 48045
    mana_pct: 28
    cast_time_ms: 5000              # 5s channel
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: shadow
    target_type: enemy_aoe          # channeled on unit; AoE damage in 10y
    radius: 10
    notes: AoE filler. Used at 3+ targets.

  SHADOW_WORD_DEATH:
    id: 32379
    mana_pct: 12
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: true
    range: 40
    school: shadow
    target_type: enemy
    notes: |
      Below 25% HP: +200% damage (3x). Self-damage if target survives.
      T13 2P: +55% damage + no backlash. Glyph: 10s reset on miss/non-kill.
      Pre-execute use: mana_pct < 30 → Masochism feedback.

  MIND_SPIKE:
    id: 73510
    mana_pct: 12
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: shadowfrost
    target_type: enemy
    notes: |
      Removes caster's Shadow DoTs from target. NOT in DoT rotation.
      Reserved for pre-pull stationary cast OR no-DoTs target.

  # ─────── Cooldowns ───────

  SHADOWFIEND:
    id: 34433
    mana_pct: 6
    cast_time_ms: 0
    cooldown_ms: 300000              # 5 min (reduced by Veiled Shadows + Sin and Punishment)
    on_gcd: false
    range: 30
    target_type: enemy
    duration_ms: 15000
    notes: pet melee returns 3% mana/hit. Sin and Punishment: -10s per Mind Flay crit.

  DISPERSION:
    id: 47585
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 120000              # 2 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 6000
    requires_talent: true
    notes: +6% mana/sec, -90% damage taken. Mana emergency + raid mechanic soak.

  HYMN_OF_HOPE:
    id: 64901
    mana_pct: 0
    cast_time_ms: 8000               # 8s channel
    cooldown_ms: 360000
    on_gcd: true
    range: 0
    target_type: self_channel
    notes: 2% mana/2s to 3 lowest-mana raid + 15% max mana.

  ARCHANGEL:
    id: 87151                        # Dark Archangel for Shadow
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 90000               # 1.5 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 18000
    requires_talent: true
    consumes: Dark_Evangelism_stacks
    notes: +5% mana, +4%/stack damage to MF/MS/MB/SW:D. Cast at 5 stacks.

  SHADOWFORM:
    id: 15473
    mana_pct: 13
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000              # toggle until cancelled
    requires_talent: true
    requires_level: 40
    notes: +15% Shadow damage, -15% damage taken (all schools). Always-on.

  VAMPIRIC_EMBRACE:
    id: 15286
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    requires_talent: true
    notes: 6% caster + 3% raid heal from Shadow damage. Toggle ON in raid.

  POWER_WORD_SHIELD:
    id: 17
    mana_pct: 34
    cast_time_ms: 0
    cooldown_ms: 3000                # global PW:S CD; Weakened Soul 15s lockout
    on_gcd: true
    range: 40
    target_type: ally
    duration_ms: 15000
    notes: Self-shield on incoming damage; not part of DPS rotation.

  POWER_WORD_FORTITUDE:
    id: 1243
    mana_pct: 16
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: party_or_raid
    duration_ms: 3600000
    notes: stamina raid buff — pre-pull only.

  INNER_FIRE:
    id: 588
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    notes: +60% armor + 532 SP flat at L85. Default damage posture.

  INNER_WILL:
    id: 73413
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    notes: +10% movement, -15% instant cost. Mutually exclusive with Inner Fire.

  FADE:
    id: 586
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 10000
    notes: -90% threat for 10s. Glyph: -10% damage taken 4s after.

  PSYCHIC_SCREAM:
    id: 8122
    mana_pct: 15
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: true
    range: 0
    target_type: pbaoe
    radius: 8
    duration_ms: 8000
    notes: 8s AoE fear. Defensive.

  MASS_DISPEL:
    id: 32375
    mana_pct: 27
    cast_time_ms: 1500
    cooldown_ms: 15000
    on_gcd: true
    range: 30
    target_type: ground_targeted
    radius: 15
    notes: removes magic effect from up to 10 targets in area.

  MIND_SOOTHE:
    id: 453
    mana_pct: 8
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: enemy
    duration_ms: 15000
    notes: reduces target's aggro range. Manual command only.

  RESURRECTION:
    id: 2006
    mana_pct: 60
    cast_time_ms: 10000
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: corpse
    notes: out-of-combat resurrect.

  SMITE:
    id: 585
    mana_pct: 11
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 30
    school: holy
    target_type: enemy
    notes: pre-Shadowform leveling damage. NOT cast post-Shadowform (no Holy spells in form).
```

---

## Maintenance pass

```yaml
maintenance:
  - name: Shadowform
    when:
      - level >= 40
      - has_talent(SHADOWFORM)
      - aura.missing(SELF, SHADOWFORM)
    cast: SHADOWFORM

  - name: Inner Fire
    when:
      - aura.missing(SELF, INNER_FIRE)
      - NOT runtime_flag.inner_will_mode
    cast: INNER_FIRE

  - name: Power Word: Fortitude (pre-combat)
    when:
      - NOT in_combat
      - aura.missing(SELF, POWER_WORD_FORTITUDE)
    cast: POWER_WORD_FORTITUDE on SELF

  - name: Vampiric Embrace toggle
    when:
      - aura.missing(SELF, VAMPIRIC_EMBRACE)
      - has_talent(VAMPIRIC_EMBRACE)
      - in_combat OR boss_pull_imminent
    cast: VAMPIRIC_EMBRACE
```

---

## Rotation — Priority List (Single-Target)

```yaml
rotation:
  # ───────── Tier 1: SW:Pain maintenance ─────────
  - tier: 1
    name: SW:Pain — apply once per target
    when:
      - aura.missing(CURRENT_DPS_TARGET, SHADOW_WORD_PAIN)
      - target_in_range(40)
      - SELF.mana_pct > 25
    cast: SHADOW_WORD_PAIN on CURRENT_DPS_TARGET
    notes: |
      Pain and Suffering 60%/Mind Flay tick auto-refreshes. Bot only manually
      re-casts on falloff (target swap, dispel).

  # ───────── Tier 2: Vampiric Touch maintenance ─────────
  - tier: 2
    name: Vampiric Touch
    when:
      - has_talent(VAMPIRIC_TOUCH)
      - target_in_range(40)
      - any of:
          - aura.missing(CURRENT_DPS_TARGET, VAMPIRIC_TOUCH)
          - aura.expires_in_ms(CURRENT_DPS_TARGET, VAMPIRIC_TOUCH) < 2000
      - SELF.mana_pct > 25
    cast: VAMPIRIC_TOUCH on CURRENT_DPS_TARGET

  # ───────── Tier 3: Devouring Plague maintenance ─────────
  - tier: 3
    name: Devouring Plague
    when:
      - target_in_range(40)
      - any of:
          - aura.missing(CURRENT_DPS_TARGET, DEVOURING_PLAGUE)
          - aura.expires_in_ms(CURRENT_DPS_TARGET, DEVOURING_PLAGUE) < 3000
      - SELF.mana_pct > 25
    cast: DEVOURING_PLAGUE on CURRENT_DPS_TARGET
    notes: instant; Improved DP gives upfront damage on cast.

  # ───────── Tier 4: Mind Blast on CD ─────────
  - tier: 4
    name: Mind Blast — Shadow Orb consumer
    when:
      - spell.off_cooldown(MIND_BLAST)
      - target_in_range(40)
      - SELF.mana_pct > 25
    cast: MIND_BLAST on CURRENT_DPS_TARGET
    notes: |
      Consumes all Shadow Orbs (+10%/orb damage). Applies Empowered Shadow
      (+20-22% DoT damage 15s) — DoT refreshes within this window snapshot
      the boost.

  # ───────── Tier 5: SW:Death execute ─────────
  - tier: 5
    name: Shadow Word: Death — execute
    when:
      - CURRENT_DPS_TARGET.hp_pct <= 25
      - spell.off_cooldown(SHADOW_WORD_DEATH)
      - target_in_range(40)
    cast: SHADOW_WORD_DEATH on CURRENT_DPS_TARGET
    notes: +200% damage on ≤25% targets. T13 2P removes self-damage.

  # ───────── Tier 5b: SW:Death mana-emergency ─────────
  - tier: 5b
    name: Shadow Word: Death — mana emergency
    when:
      - SELF.mana_pct < 30
      - spell.on_cooldown(SHADOWFIEND)
      - shadowfiend_remaining_cd_ms > 30000
      - spell.off_cooldown(SHADOW_WORD_DEATH)
      - target_in_range(40)
    cast: SHADOW_WORD_DEATH on CURRENT_DPS_TARGET
    notes: Masochism mana feedback (4% per ≥10% HP self-damage).

  # ───────── Tier 6: Mind Flay filler ─────────
  - tier: 6
    name: Mind Flay — channel filler
    when:
      - target_in_range(30)
      - SELF.mana_pct > 25
    cast: MIND_FLAY on CURRENT_DPS_TARGET
    notes: |
      Generates Shadow Orbs (10%/tick), refreshes SW:P (Pain and Suffering
      60%/tick), builds Dark Evangelism stacks (1/tick).
```

---

## AoE Mode

Activated by `enemies_within_10y_of(CURRENT_DPS_TARGET) >= 3`. Hysteresis: exit `<3 sustained for 5s`.

```yaml
rotation_aoe:
  # 2 targets — DoT primary only; secondary only DoT'd if lifetime >= 10s
  # 3-4 targets — multi-DoT primary + Mind Sear filler
  # 5+ targets — pure Mind Sear spam, only DoT primary

  - tier: 1: Mind Blast on CD (still highest single-target hit)
  - tier: 2: SW:Pain maintenance on primary (Pain and Suffering refreshes via Mind Sear?)
  - tier: 3: VT + DP on primary
  - tier: 4: spread SW:P to 2-3 secondary if avg_target_lifetime >= 10s
  - tier: 5: Mind Sear channel filler on primary target
notes: |
  Mind Sear has 10y AoE around the channeled target; primary target itself is
  hit (Cata 4.1+ change). Pain and Suffering refresh from Mind Sear is
  UNVERIFIED — flagged.
```

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: SHADOWFIEND
    when:
      - in_combat
      - SELF.mana_pct < 60
      - fight_remaining_estimate > 15000
      - spell.off_cooldown(SHADOWFIEND)
    rationale: 5min CD; mana sustain + DPS contribution.

  - spell: ARCHANGEL
    when:
      - dark_evangelism_stacks == 5
      - spell.off_cooldown(ARCHANGEL)
      - in_combat
      - mind_blast_imminent_in_2000ms
    rationale: 1.5min CD; consume 5 stacks for +20% damage / +5% mana 18s.

  - spell: DISPERSION
    when:
      - any of:
          - SELF.mana_pct < 20
          - SELF.hp_pct < 30
          - raid_AoE_imminent
      - spell.off_cooldown(DISPERSION)
    rationale: 2min CD defensive + mana emergency.

  - spell: HYMN_OF_HOPE
    when:
      - in_combat
      - raid.mana_avg_pct < 35
      - combat_duration_ms > 60000
      - no_active_emergency
      - spell.off_cooldown(HYMN_OF_HOPE)
    rationale: 6min raid mana CD; gate on extended fight.

  - spell: FADE
    when:
      - SELF.threat_pct > 90
      - tank_alive
      - spell.off_cooldown(FADE)
    rationale: threat dump.

  - spell: PSYCHIC_SCREAM
    when:
      - melee_count_within_8y >= 2
      - SELF.hp_pct < 60
      - spell.off_cooldown(PSYCHIC_SCREAM)
    rationale: 8s AoE fear escape.

  - spell: MASS_DISPEL
    when:
      - dispellable_buff_count_in_raid_range >= 3
      - spell.off_cooldown(MASS_DISPEL)
    rationale: utility purge.
```

---

## Proc Reactions

```yaml
procs:
  SHADOW_ORBS:
    mastery_resource: true
    triggered_by: SW:Pain tick (10%) OR Mind Flay tick (10%)
    max_charges: 3
    consumed_by: Mind Blast (all stacks)
    ai_behavior: passive — Mind Blast on CD consumes; bot doesn't gate.

  EMPOWERED_SHADOW:
    talent_passive: true
    triggered_by: Shadow Orb consumption via Mind Blast
    duration_ms: 15000
    effect: +20% (+22% with Harnessed Shadows 2/2) periodic damage
    ai_behavior: |
      Bot prioritizes refreshing DoTs within Empowered Shadow window for
      snapshot benefit. Tier 1-3 maintenance refresh thresholds factor in
      buff state.

  PAIN_AND_SUFFERING:
    talent_passive: true
    triggered_by: Mind Flay tick
    proc_chance_pct: 60               # at 2/2
    effect: refreshes SW:Pain on target
    ai_behavior: passive — bot doesn't manually refresh SW:P.

  SHADOWY_APPARITION:
    talent_passive: true
    triggered_by: SW:Pain tick
    proc_chance_pct: 12               # at 3/3
    max_active: 4
    ai_behavior: passive (chasing visuals deal damage on contact).

  DARK_EVANGELISM:
    talent_passive: true
    triggered_by: Mind Flay tick (1 stack/tick)
    max_stacks: 5
    duration_ms: 20000
    effect_per_stack: +2% damage to MF/MS/MB/SW:D
    consumed_by: Archangel
    ai_behavior: bot fires Archangel at 5 stacks.

  REPLENISHMENT:
    triggered_by: Mind Blast on Vampiric-Touch-affected target
    effect: 1% mana / 10s to up to 10 nearby raid members
    ai_behavior: passive — emergent benefit.

  MASTERY_SHADOW_ORB_POWER:
    mastery_passive: true
    base_pct_per_point: 1.3
    effect: scales Empowered Shadow's DoT damage modifier
    ai_behavior: passive — informs reforge.
```

---

## Mana Management

```yaml
mana:
  thresholds:
    drink_floor_pct: 30
    drink_resume_pct: 95
    shadowfiend_floor_pct: 60
    dispersion_floor_pct: 20
    sw_death_emergency_floor_pct: 30
    hymn_raid_avg_floor_pct: 35
    conserve_mode_pct: 50
    crisis_mode_pct: 25

  conserve_behavior:
    when: SELF.mana_pct < 50
    rules:
      - skip Tier 6 Mind Flay filler unless Empowered Shadow active
      - keep DoT maintenance (Tiers 1-3)
      - prefer Mind Blast on CD (Tier 4) over filler

  crisis_behavior:
    when: SELF.mana_pct < 25
    rules:
      - skip Tier 6 entirely
      - fire Shadowfiend / Dispersion / Hymn of Hope cascade
      - SW:Death pre-execute (Tier 5b) for Masochism mana feedback

  out_of_combat:
    drink_when:
      - not in_combat
      - SELF.mana_pct < drink_floor_pct
      - no enemies within 40y for 5000ms
    eat_when: out_of_combat AND SELF.hp_pct < 80

shadow_orbs:
  max_charges: 3
  generation_chance_per_tick_pct: 10   # SW:P + Mind Flay both
  consumed_by: Mind Blast (all stacks)
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
      - boss_pull_imminent_in_2s
      - OR (in_combat AND fight_elapsed_ms ~ 60000)

  mana_potion:
    item: Mythical_Mana_Potion
    item_id: 57192

  healthstone:
    item_id: 36892
```

---

## Non-encounter content

### Solo combat

```yaml
solo_offensive_rotation:
  - tier 1: SELF.hp_pct < 35 AND aura.missing(SELF, POWER_WORD_SHIELD) -> PW:Shield self
  - tier 2: SELF.hp_pct < 25 -> Dispersion if off_cd
  - tier 3: aura.missing(target, SHADOW_WORD_PAIN) -> SW:P
  - tier 4: has_talent(VT) AND aura.missing(target, VT) -> VT
  - tier 5: level>=28 AND aura.missing(target, DP) -> DP
  - tier 6: target.hp_pct <= 25 AND off_cd(SW:D) -> SW:DEATH
  - tier 7: off_cd(MIND_BLAST) -> MIND_BLAST
  - tier 8: SELF.mana_pct > 25 -> MIND_FLAY (if level >= 10) else SMITE
notes: pre-Shadowform (under L40) uses Smite + SW:P only. Post-Shadowform (40+), full DoT rotation.
```

### Travel & out-of-combat

```yaml
travel:
  mount:
    use_when:
      - not in_combat
      - distance_to_destination >= 80
      - mount_zone_allowed

ooc_maintenance:
  - check: aura.missing(SELF, SHADOWFORM) AND level>=40 AND has_talent -> SHADOWFORM
  - check: aura.missing(SELF, INNER_FIRE) -> INNER_FIRE
  - check: aura.missing(SELF, POWER_WORD_FORTITUDE) -> POWER_WORD_FORTITUDE
  - check: SELF.mana_pct < 80 AND no_enemies_within(40) -> drink
  - check: SELF.hp_pct < 95 AND no_enemies_within(40) -> eat
```

### Idle

```yaml
idle:
  primary_action: follow master
  secondary_actions:
    - maintain Shadowform / Inner Fire / Vampiric Embrace
    - maintain raid Power Word: Fortitude
  do_not:
    - do not cast offensive spells while idle
    - do not Mind Soothe neutral mobs unprompted
```

### World boss / elite scaling

```yaml
world_boss_or_elite:
  rotation_override:
    if_in_party:
      use: encounter rotation
      thresholds:
        dispersion_threshold: 35
        crisis_mode_pct: 30
    if_solo:
      use: solo_offensive_rotation
      thresholds:
        pw_shield_self_threshold: 70
        do_not_engage_unless_can_solo: true
```

### Disengage / emergency utility

```yaml
disengage:
  dispersion:
    use_when:
      - SELF.hp_pct < 30 OR SELF.mana_pct < 20
    cast: DISPERSION

  psychic_scream:
    use_when:
      - melee_count_within_8y >= 2
      - SELF.hp_pct < 60
    cast: PSYCHIC_SCREAM

  fade:
    use_when:
      - SELF.threat_pct > 90
    cast: FADE
```

---

## Leveling rotation

Pure Shadow from level 10. Pre-Shadowform (under L40) uses Smite + SW:P; post-Shadowform full rotation.

### Spec recommendation

```yaml
spec_choice:
  for_leveling_dungeon_dps_bot:
    spec: Shadow
    talent_split: 31 Shadow / 10 Discipline / 0 Holy
    talent_picks_shadow: Darkness 3/3 + Improved SW:P 2/2 + Veiled Shadows 2/2 + Improved Mind Blast 3/3 + Improved Devouring Plague 2/2 + Twisted Faith 2/2 + Shadowform 1/1 + Harnessed Shadows 2/2 + Vampiric Embrace 1/1 + Masochism 2/2 + Mind Melt 2/2 + Pain and Suffering 2/2 + Vampiric Touch 1/1 + Sin and Punishment 2/2 + Shadowy Apparition 3/3 + Dispersion 1/1
    talent_picks_disc: Twin Disciplines 3/3 + Mental Agility 2/3 + Evangelism 2/2 + Archangel 1/1 + Inner Sanctum 2/3 (or Phantasm 2/2)
    rationale: |
      Per survey: gotwarcraft G6 explicit 31/10/0 build. Discipline secondary
      provides Twin Disciplines (instant cast +damage), Mental Agility (instant
      cost reduction), and Evangelism/Archangel cooldown burst.
```

### Spell unlock table

```yaml
spell_unlocks:
  POWER_WORD_FORTITUDE:    {level: 1,  spell_id: 1243}
  SMITE:                   {level: 1,  spell_id: 585}
  SHADOW_WORD_PAIN:        {level: 4,  spell_id: 589}
  POWER_WORD_SHIELD:       {level: 6,  spell_id: 17}
  MIND_BLAST:              {level: 9,  spell_id: 8092}
  MIND_FLAY:               {level: 10, spell_id: 15407}
  FADE:                    {level: 12, spell_id: 586}
  PSYCHIC_SCREAM:          {level: 14, spell_id: 8122}
  INNER_FIRE:              {level: 16, spell_id: 588}
  LEVITATE:                {level: 22, spell_id: 1706}
  DISPEL_MAGIC:            {level: 26, spell_id: 527}
  DEVOURING_PLAGUE:        {level: 28, spell_id: 2944}
  MIND_SOOTHE:             {level: 30, spell_id: 453}
  SHADOW_WORD_DEATH:       {level: 32, spell_id: 32379}
  SHADOWFORM:              {level: 40, spell_id: 15473}
  PRAYER_OF_FORTITUDE:     {level: 48, spell_id: 21562}
  HYMN_OF_HOPE:            {level: 64, spell_id: 64901}
  SHADOWFIEND:             {level: 66, spell_id: 34433}
  MASS_DISPEL:             {level: 68, spell_id: 32375}
  MIND_SEAR:               {level: 75, spell_id: 48045}
  MIND_SPIKE:              {level: 81, spell_id: 73510}
  INNER_WILL:              {level: 83, spell_id: 73413}
  # Talent-gated:
  VAMPIRIC_TOUCH:           {requires_talent: true, spell_id: 34914}
  VAMPIRIC_EMBRACE:         {requires_talent: true, spell_id: 15286}
  IMPROVED_DEVOURING_PLAGUE:{requires_talent: true}
  PAIN_AND_SUFFERING:       {requires_talent: true}
  SIN_AND_PUNISHMENT:       {requires_talent: true}
  TWISTED_FAITH:            {requires_talent: true}
  HARNESSED_SHADOWS:        {requires_talent: true}
  MASOCHISM:                {requires_talent: true}
  IMPROVED_MIND_BLAST:      {requires_talent: true}
  VEILED_SHADOWS:           {requires_talent: true}
  IMPROVED_SHADOW_WORD_PAIN:{requires_talent: true}
  DARKNESS:                 {requires_talent: true}
  SHADOWY_APPARITION:       {requires_talent: true}
  DISPERSION:               {requires_talent: true, spell_id: 47585}
  EVANGELISM:               {requires_talent: true, tree: discipline}
  ARCHANGEL:                {requires_talent: true, tree: discipline, spell_id: 87151}
  TWIN_DISCIPLINES:         {requires_talent: true, tree: discipline}
  MENTAL_AGILITY:           {requires_talent: true, tree: discipline}
  INNER_SANCTUM:            {requires_talent: true, tree: discipline}
  EMPOWERED_SHADOW:         {passive_buff: true, base_pct: 20, harnessed_shadows_pct: 2}
  MASTERY_SHADOW_ORB_POWER: {passive_mastery: true, base_pct_per_point: 1.3}
```

### Leveling brackets

```yaml
bracket_1_3:
  available: SMITE, POWER_WORD_FORTITUDE
  rotation: SMITE spam

bracket_4_5:
  available: + SHADOW_WORD_PAIN
  rotation:
    - tier 1: aura.missing(target, SW:P) -> SW:P
    - tier 2: SMITE filler

bracket_6_8:
  available: + POWER_WORD_SHIELD
  rotation_addition: PW:Shield self when meleed

bracket_9:
  available: + MIND_BLAST
  rotation_change: MB on CD becomes Tier 1; SW:P maintenance Tier 2

bracket_10_15:
  available: + MIND_FLAY (lvl 10)
  rotation_change: Mind Flay becomes filler Tier 6

bracket_16_27:
  available: + INNER_FIRE (16)
  maintenance: Inner Fire always on

bracket_28_31:
  available: + DEVOURING_PLAGUE (28)
  rotation_addition: DP becomes Tier 3

bracket_32_39:
  available: + SHADOW_WORD_DEATH (32)
  rotation_addition: SW:Death execute below 25%

bracket_40_63:
  available: + SHADOWFORM (40)
  maintenance: Shadowform always on
  rotation_change: full DoT priority rotation activates
  notes: pre-VT-talent (~level 30 talent path), VT not yet in rotation. Once talent allocated, VT becomes Tier 2.

bracket_64_65:
  available: + HYMN_OF_HOPE (64)
  cooldown_addition: HoH on raid mana <35%

bracket_66_74:
  available: + SHADOWFIEND (66)
  cooldown_addition: Shadowfiend on mana <60%

bracket_75_80:
  available: + MIND_SEAR (75)
  rotation_addition: AoE-mode at 3+ enemies

bracket_81_84:
  available: + MIND_SPIKE (81)
  notes: Mind Spike NOT cast in DoT rotation (removes self DoTs).

bracket_85:
  available: + INNER_WILL (83) + all talents fully allocated
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

| # | Item | Spec value | Verification |
|---|---|---|---|
| 1 | Shadow Orb generation rate per tick | 10% (vs 18% claim) | Talent.dbc / Spell.dbc |
| 2 | Empowered Shadow baseline | 20% + 1%/+2% per Harnessed Shadows | Talent.dbc + Spell.dbc |
| 3 | Shadowy Apparition movement-bonus | NO movement bonus assumed (60% claim flagged) | Talent.dbc |
| 4 | Pain and Suffering proc rate at 2/2 | 60% per MF tick | Talent.dbc 47581 |
| 5 | Dark Evangelism stacks | 1/Mind Flay tick | Talent.dbc |
| 6 | Improved Mind Blast 3/3 | -1.5s CD (8s → 6.5s) | Talent.dbc |
| 7 | Improved DP upfront formula | 5%/10%/30% across 3 ticks | Talent.dbc |
| 8 | Sin and Punishment Shadowfiend CD reduction | -10s/MF crit at 2/2 | Talent.dbc |
| 9 | T13 2P SW:Death no-backlash | yes | Item set DBC |
| 10 | T13 4P Shadow Orbs from pet/apparitions | yes | Item set DBC |
| 11 | Mastery Shadow Orb Power formula | 1.3% per point | Mastery DBC |
| 12 | Replenishment uptime | 1% mana/10s to 10 nearby | Spell.dbc |
| 13 | Vampiric Embrace heal % | 6% caster + 3% raid | Talent.dbc 15286 |
| 14 | Veiled Shadows Shadowfiend CD | -30s/-60s per rank | Talent.dbc |
| 15 | Twisted Faith 100% Spirit→Hit | yes | Talent.dbc 47577 |
| 16 | Glyph of SW:Death 10s reset on miss | yes | Glyph.dbc |
| 17 | Glyph of Mind Flay +10% | yes | Glyph.dbc |
| 18 | Glyph of SW:Pain +10% crit | yes | Glyph.dbc |
| 19 | Mind Sear AoE radius | 10y | Spell.dbc 48045 |
| 20 | Volcanic Potion +1200 Int | yes | Item DBC |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kShadowPriest_Spells[]` |
| Rotation tiers | `static const RotationTier kShadowPriest_Rotation[]` |
| AoE rotation | `static const RotationTier kShadowPriest_RotationAoE[]` |
| Cooldown Profile | `static const CooldownTrigger kShadowPriest_Cooldowns[]` |
| Mana Management | `ManaState` enum + threshold helpers |
| Shadow Orb tracking | `uint8 shadow_orbs` (0-3) |
| Empowered Shadow buff tracking | aura listener |
| Dark Evangelism stack tracking | `uint8 dark_evangelism_stacks` (0-5) |
| Shadowform state | `bool shadowform_active` |
| Inner Fire vs Will | `bool inner_will_mode` runtime flag |
| Leveling rotation | `LevelBracketRotation kShadowPriest_Leveling[]` |

**Key new helper APIs:**
- `ShadowOrbs(bot)` — read 0-3.
- `EmpoweredShadowActive(bot)` + `EmpoweredShadowRemainingMs(bot)`.
- `DarkEvangelismStacks(bot)` — read 0-5.
- `IsShadowformActive(bot)`.
- `IsInExecutePhase(target)` — `target.hp_pct <= 25`.
- `IsBoltDoTSnapshotWindow(bot)` — `EmpoweredShadowActive(bot)` for DoT-refresh prioritization.

Cross-reference to `docs/roles/ranged-dps.md` and prior ranged-DPS specs.
