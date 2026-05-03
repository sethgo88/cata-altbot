# Frost Death Knight — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide.

**Strategy content** reconciled across 8 community sources. See `docs/research/frost-death-knight-guide-survey.md`. Cross-reference: this is the **first DK spec doc** in the project — Runes, Runic Power, diseases (Frost Fever / Blood Plague), Death Runes, and Presences are all encoded fresh here.

**SPEC-CHOICE RATIONALE.** DK has 3 specs: Blood (tank, deferred per role-ordering), Frost (melee DPS), Unholy (melee DPS). Both DPS specs clear Dragon Soul at competitive numbers (~5% spread fight-by-fight). **Frost (2H, Might of the Frozen Wastes)** is selected over Unholy on three grounds: (1) flatter priority list (no Festering-Strike pandemic juggling, no Sudden Doom timing, no Dark Transformation alignment), (2) no permanent pet (Frost's Raise Dead is a 1-min temporary off-GCD ghoul, vs Unholy's permanent ghoul that requires pet-AI plumbing), (3) 2H weapon scaling on Dragon Soul-tier loot. Within Frost, **2H over DW** is the consensus build (~6/8 sources hard-pick 2H; DW shares ~90% of priority but adds Threat-of-Thassarian off-hand-copy bookkeeping). This is the **fourth melee-DPS spec** in the project.

---

## Identity

```yaml
spec: frost_death_knight
class: death_knight
role: MELEE_DPS
resource_model: runes (6: 2 Blood + 2 Frost + 2 Unholy) + runic_power (0-100)
defining_mechanic: Obliterate (1F+1U) primary builder + Frost Strike (32 RP) primary spender + Killing Machine + Rime procs + diseases applied via Outbreak (1min CD) or Howling Blast (Frost) / Plague Strike (BP filler)
defining_buffs:
  - Frost_Presence              # default DPS presence; +5% phys dmg on melee crit, +10% Stam, threat reduction
  - Pillar_of_Frost             # 1min CD, 20s, +20% Strength
  - Empower_Rune_Weapon         # 5min CD, instantly refresh all 6 runes + 25 RP
  - Horn_of_Winter              # raid +Str/Agi (no AP buff in Cata; mostly RP-gen + buff parity if no Kings)
  - Frost_Fever                 # disease applied by HB/Outbreak; spec damage source
  - Blood_Plague                # disease applied by Plague Strike/Outbreak; spec damage source
defining_procs:
  - Killing_Machine             # auto-attack proc; next Frost Strike OR Obliterate auto-crits
  - Rime                        # Obliterate proc; next Howling Blast costs 0 runes (free)
  - Mastery: Frozen_Heart       # +% damage to all Frost-school attacks (Frost Strike, Howling Blast, Frost Fever, Obliterate Frost portion)
mastery: Frozen_Heart
key_cooldowns:
  - Pillar_of_Frost             # 1min, 20s, Strength burst
  - Empower_Rune_Weapon         # 5min, instant rune refresh + 25 RP
  - Raise_Dead                  # 3min, 1-min temp ghoul (off-GCD)
  - Anti_Magic_Shell            # 45s, 5s magic absorb + RP gen
  - Icebound_Fortitude          # 3min, 12s, -20% damage taken + immune to stun
  - Lichborne                   # 2min talent (cross-tree, Blood) — self-CC-break + heal-from-undead
  - Death_Pact                  # 2min, sacrifice ghoul → heal self (only if Raise Dead up)
  - Mind_Freeze                 # 10s CD melee interrupt (no rune cost)
  - Strangulate                 # 2min, ranged silence/interrupt (1 Blood rune)
  - Death_Grip                  # 35s, 30y forced-pull (forced-taunt for tanks; utility for DPS)
melee_range: 5
notes: |
  Frost DK in 4.3.4 plays as a 2H plate melee in Frost Presence with diseases
  always-on (Outbreak 1min CD applies both; Howling Blast applies Frost Fever
  filler; Plague Strike applies Blood Plague filler). Heartbeat: maintain
  diseases (Outbreak on CD; HB applies FF; PS applies BP if Outbreak on CD
  AND Blood Plague < 3s); Obliterate at 1F+1U rune availability is primary
  builder; Frost Strike at >= 32 RP primary RP dump (and overcap-prevention
  spender at RP >= 88); Killing Machine procs trigger Frost Strike OR
  Obliterate auto-crit; Rime procs trigger free Howling Blast; Howling Blast
  no-Rime as filler when no other rune-positive action available; Pillar of
  Frost on CD (~1min) for +20% Strength; Empower Rune Weapon stacked into
  pull then "panic button" later.

  Burst window: Pillar of Frost + Empower Rune Weapon + Raise Dead +
  Golemblood pot stacked at first pull; Pillar drifts on its 1-min CD; ERW
  re-uses around 4 minutes in. T13 4P-set-bonus extends Pillar duration by
  5s (UNVERIFIED — flag for set-bonus DBC).

  Pre-Outbreak (under L81): use Plague Strike + Howling Blast for diseases
  (HB applies FF; PS applies BP). Pre-Pillar of Frost (Frost-talent unlock):
  no Strength burst; rotation is plain priority. Pre-Frost Strike (Frost
  talent unlock): Death Coil is RP dump.

  DK starts at L55 (hero class); no L1-L54 progression. All DKs run starter
  zone in Blood Presence; Frost Presence + spec is taken at L58 trainer.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  CURRENT_DPS_TARGET:
  ENEMY_CASTING_INTERRUPTIBLE:    # Mind Freeze range: 10y
  ENEMY_CASTING_RANGED:           # Strangulate range: 30y (2min CD)
  AOE_CLUSTER:                    # ≥3 enemies within 10y (Howling Blast splash + Pestilence)
  PESTILENCE_TARGET:              # primary diseased enemy near additional unit (10y spread)
  DEATH_GRIP_TARGET:              # forced-pull utility (off-tank / interrupt-pull)
  DEAD_ALLY:                      # Raise Ally (combat res, talent)
  ANTI_MAGIC_ZONE_GROUND:         # talent-only AoE shield placement
```

---

## Spell Catalog

```yaml
spells:
  # ─────── Rune-cost builders ───────

  OBLITERATE:
    id: 49020
    rune_cost: {frost: 1, unholy: 1}
    runic_power_gain: 20            # +Threat of Thassarian additional gain (UNVERIFIED scaling)
    cast_time_ms: 0
    cooldown_ms: 0                  # rune-gated, not CD-gated
    on_gcd: true
    range: 5
    target_type: enemy_melee
    notes: |
      Primary Frost-builder. With Annihilation talent (3/3) ignores disease
      bonus while Killing Machine NOT active (so KM consumption on Obliterate
      is Strength-scaled, not disease-scaled). Spend on rune availability.

  HOWLING_BLAST:
    id: 49184
    rune_cost: {frost: 1}             # 0 with Rime proc
    runic_power_gain: 10              # 0 if Rime-free (UNVERIFIED)
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: ground_at_target     # 10y splash radius
    radius: 10
    applies: FROST_FEVER              # primary FF applier in Frost
    notes: |
      Primary Frost AoE spell. Single-target use: Rime-proc free cast is
      mandatory consumption (no rune cost). No-Rime HB is filler if no
      other rune-positive action available. AoE: spam on Rime + Frost-rune
      availability.

  PLAGUE_STRIKE:
    id: 45462
    rune_cost: {unholy: 1}
    runic_power_gain: 10
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    applies: BLOOD_PLAGUE
    notes: |
      Frost rarely uses outside Outbreak-CD windows. Disease-filler only:
      cast IF Outbreak on CD AND Blood Plague missing OR < 3s remaining.

  # ─────── Runic-Power spenders ───────

  FROST_STRIKE:
    id: 49143
    rune_cost: none
    runic_power_cost: 32              # 40 base, reduced by Frost spec
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    requires_spec: frost
    notes: |
      Primary RP dump. Frost-school damage scales with Frozen Heart mastery.
      Cast at >= 32 RP. Overcap-prevention: cast BEFORE Obliterate when
      RP >= 88 (Obliterate gain would overflow). KM-proc consumption priority
      target alongside Obliterate.

  DEATH_COIL:
    id: 47541
    rune_cost: none
    runic_power_cost: 32
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: enemy
    notes: |
      Unholy-spec RP dump; Frost SKIPS in favor of Frost Strike (FS scales
      with Mastery, DC does not). Only used if forced-ranged AND no Frost
      target in melee.

  # ─────── Disease application / spread ───────

  OUTBREAK:
    id: 77575
    rune_cost: none
    runic_power_cost: 0
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: true
    range: 30
    target_type: enemy
    applies: [FROST_FEVER, BLOOD_PLAGUE]
    requires_level: 81
    notes: |
      L81 Cata-new. Instant cast applies BOTH diseases without rune cost.
      On CD always — primary disease maintenance once unlocked.

  PESTILENCE:
    id: 50842
    rune_cost: {blood: 1}
    runic_power_gain: 10
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_with_disease
    radius: 10                        # spread radius from primary target
    notes: |
      Spreads diseases from primary target to all enemies within 10y AND
      refreshes diseases on primary target to full duration. AoE-only on
      disease maintenance for cleave; not ST-rotation.

  BLOOD_BOIL:
    id: 48721
    rune_cost: {blood: 1}
    runic_power_gain: 10
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: pbaoe
    radius: 10
    notes: |
      AoE Blood-rune dump. Bonus damage to diseased enemies. Used only when
      Blood runes (or converted Death Runes) are surplus AND AoE active.

  # ─────── Death Strike (heal / emergency) ───────

  DEATH_STRIKE:
    id: 49998
    rune_cost: {frost: 1, unholy: 1}
    runic_power_gain: 20
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    notes: |
      Cata: heals based on damage-taken in last 5s (min 7% max HP). Frost
      uses ONLY at HP < 35% emergency — never rotational (Obliterate scales
      better with Frost mastery + KM procs).

  # ─────── Cooldowns ───────

  PILLAR_OF_FROST:
    id: 51271
    rune_cost: {frost: 1}             # UNVERIFIED — some sources say 0; consensus 1F
    runic_power_gain: 0
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: false                     # UNVERIFIED off-GCD
    range: 0
    target_type: self
    duration_ms: 20000
    requires_spec: frost
    notes: +20% Strength 20s. On CD always. Stack first-pull with ERW + Raise Dead.

  EMPOWER_RUNE_WEAPON:
    id: 47568
    rune_cost: none
    runic_power_gain: 25
    cast_time_ms: 0
    cooldown_ms: 300000
    on_gcd: false                     # UNVERIFIED off-GCD
    range: 0
    target_type: self
    notes: |
      Instantly refresh all 6 runes + grant 25 RP. 5min CD. First use stacked
      with pull (Pillar + ERW + RD). Second use as "rune panic" if rune
      starvation observed mid-fight.

  RAISE_DEAD:
    id: 46584
    rune_cost: none
    runic_power_gain: 0
    cast_time_ms: 0
    cooldown_ms: 180000
    on_gcd: false
    range: 0
    target_type: self
    summon: GHOUL_TEMP
    summon_duration_ms: 60000
    notes: |
      3min CD; 1-minute temporary ghoul. Off-GCD. Used as DPS cooldown +
      gives access to Death Pact (sacrifice ghoul → heal self). Stack with
      Pillar/ERW first pull.

  HORN_OF_WINTER:
    id: 57330
    rune_cost: none
    runic_power_gain: 10              # generates RP
    cast_time_ms: 0
    cooldown_ms: 20000
    on_gcd: true
    range: 0
    target_type: party_buff
    duration_ms: 120000
    notes: |
      +Str/+Agi raid buff (replaced by Kings/Mark in Cata raid; mostly RP-
      generation rotational filler). Cast pre-pull AND on CD if RP-starved.

  # ─────── Defensive cooldowns ───────

  ANTI_MAGIC_SHELL:
    id: 48707
    rune_cost: none
    runic_power_gain: 0
    cast_time_ms: 0
    cooldown_ms: 45000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 5000
    notes: |
      Absorb 75% of magic damage for 5s (cap 50% max HP). Generates RP from
      absorbed damage (UNVERIFIED conversion rate). Use pre-emptively on
      known incoming magic. Defensive + RP gen.

  ICEBOUND_FORTITUDE:
    id: 48792
    rune_cost: none
    runic_power_gain: 0
    cast_time_ms: 0
    cooldown_ms: 180000               # UNVERIFIED 120s vs 180s on Cata
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 12000
    notes: -20% damage taken + immune to stun. 3min CD emergency defensive.

  LICHBORNE:
    id: 49039
    rune_cost: none
    runic_power_cost: 0
    cast_time_ms: 0
    cooldown_ms: 120000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 10000
    requires_talent: true             # Blood-tree cross-pick (rare 31/5/5 splits)
    notes: |
      Self → Undead-type for 10s; can self-CC-break + Death Coil heals self.
      Talent-gated; default 31/7/3 build does NOT take. Encounter-tunable.

  DEATH_PACT:
    id: 48743
    rune_cost: none
    runic_power_cost: 40
    cast_time_ms: 0
    cooldown_ms: 120000
    on_gcd: false
    range: 0
    target_type: self
    notes: |
      Sacrifice ghoul → heal self 50% HP (UNVERIFIED %). Requires Raise Dead
      ghoul active. Emergency defensive when HP < 25%.

  # ─────── Interrupts / utility ───────

  MIND_FREEZE:
    id: 47528
    rune_cost: none
    runic_power_cost: 0               # Cata-new: free
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: false
    range: 10
    target_type: enemy
    notes: Primary interrupt. Off-GCD. Free cast (Cata change).

  STRANGULATE:
    id: 47476
    rune_cost: {blood: 1}
    runic_power_gain: 10
    cast_time_ms: 0
    cooldown_ms: 120000
    on_gcd: true
    range: 30
    target_type: enemy
    duration_ms: 5000
    notes: Ranged silence/interrupt (5s). Backup when Mind Freeze on CD or out-of-range.

  DEATH_GRIP:
    id: 49576
    rune_cost: none
    runic_power_gain: 0               # UNVERIFIED — some sources +20 RP, some 0
    cast_time_ms: 0
    cooldown_ms: 35000
    on_gcd: true
    range: 30
    target_type: enemy
    notes: |
      Forced-pull 30y. For DPS: gap-close + add-pull utility (NOT a sustained
      threat tool). Bot uses on manual command OR when target out-of-melee
      AND no other valid action.

  PATH_OF_FROST:
    id: 3714
    rune_cost: {frost: 1}
    runic_power_gain: 10
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 600000
    notes: Water-walking buff. Travel utility; never combat-cast.
```

---

## Maintenance pass

```yaml
maintenance:
  - id: PRESENCE_FROST
    target: SELF
    aura: FROST_PRESENCE
    refresh_when: missing
    cast: FROST_PRESENCE
    notes: Default presence. Set on entering combat AND post-revive.

  - id: DISEASES_OUTBREAK
    target: CURRENT_DPS_TARGET
    aura: [FROST_FEVER, BLOOD_PLAGUE]
    refresh_when:
      - any_missing OR (any.expires_in_ms < 3000)
      - spell.off_cooldown(OUTBREAK)
      - level >= 81
    cast: OUTBREAK on CURRENT_DPS_TARGET
    notes: L81+ primary disease application. 1min CD.

  - id: DISEASES_HB_FF
    target: CURRENT_DPS_TARGET
    aura: FROST_FEVER
    refresh_when:
      - missing OR expires_in_ms < 3000
      - spell.on_cooldown(OUTBREAK) OR level < 81
      - rune.available(FROST) >= 1 OR aura.active(SELF, RIME_PROC)
    cast: HOWLING_BLAST on CURRENT_DPS_TARGET
    notes: HB applies FF + does damage; preferred over PS for Frost.

  - id: DISEASES_PS_BP
    target: CURRENT_DPS_TARGET
    aura: BLOOD_PLAGUE
    refresh_when:
      - missing OR expires_in_ms < 3000
      - spell.on_cooldown(OUTBREAK) OR level < 81
      - rune.available(UNHOLY) >= 1
    cast: PLAGUE_STRIKE on CURRENT_DPS_TARGET
    notes: BP application when Outbreak on CD. Rare in Frost.

  - id: HORN_OF_WINTER
    target: SELF
    aura: HORN_OF_WINTER_BUFF
    refresh_when: aura.expires_in_ms(SELF, HORN_OF_WINTER_BUFF) < 30000
    cast: HORN_OF_WINTER
    notes: Pre-pull cast + on CD if RP-starved.
```

---

## Rotation — Priority List (Single-Target)

```yaml
rotation:
  # ───────── Tier 1: Disease maintenance via Outbreak ─────────
  - tier: 1
    name: Outbreak — diseases missing or expiring
    when:
      - level >= 81
      - any_of:
          - aura.missing(CURRENT_DPS_TARGET, FROST_FEVER)
          - aura.missing(CURRENT_DPS_TARGET, BLOOD_PLAGUE)
          - aura.expires_in_ms(CURRENT_DPS_TARGET, FROST_FEVER) < 3000
          - aura.expires_in_ms(CURRENT_DPS_TARGET, BLOOD_PLAGUE) < 3000
      - spell.off_cooldown(OUTBREAK)
      - target_in_range(30)
    cast: OUTBREAK on CURRENT_DPS_TARGET

  # ───────── Tier 2: Killing Machine consumption ─────────
  - tier: 2
    name: Frost Strike — Killing Machine + RP available
    when:
      - aura.active(SELF, KILLING_MACHINE_PROC)
      - runic_power >= 32
      - target_in_range(5)
    cast: FROST_STRIKE on CURRENT_DPS_TARGET
    notes: |
      KM consumption preference: Frost Strike over Obliterate when RP
      available — FS Frost-school benefits from Mastery; Obliterate
      Frost-school component also benefits but FS scales harder per cast.

  - tier: 2
    name: Obliterate — Killing Machine + 1F+1U runes (no RP for FS)
    when:
      - aura.active(SELF, KILLING_MACHINE_PROC)
      - runic_power < 32
      - rune.available(FROST) >= 1
      - rune.available(UNHOLY) >= 1
      - target_in_range(5)
    cast: OBLITERATE on CURRENT_DPS_TARGET

  # ───────── Tier 3: Rime consumption ─────────
  - tier: 3
    name: Howling Blast — Rime free cast
    when:
      - aura.active(SELF, RIME_PROC)
      - target_in_range(30)
    cast: HOWLING_BLAST on CURRENT_DPS_TARGET
    notes: Free cast; immediate consumption — Rime is single-stack.

  # ───────── Tier 4: Disease filler (no Outbreak available) ─────────
  - tier: 4
    name: Howling Blast — apply Frost Fever (Outbreak on CD or pre-L81)
    when:
      - aura.missing(CURRENT_DPS_TARGET, FROST_FEVER)
      - any_of:
          - spell.on_cooldown(OUTBREAK)
          - level < 81
      - rune.available(FROST) >= 1
      - target_in_range(30)
    cast: HOWLING_BLAST on CURRENT_DPS_TARGET

  - tier: 4
    name: Plague Strike — apply Blood Plague (Outbreak on CD or pre-L81)
    when:
      - aura.missing(CURRENT_DPS_TARGET, BLOOD_PLAGUE)
      - any_of:
          - spell.on_cooldown(OUTBREAK)
          - level < 81
      - rune.available(UNHOLY) >= 1
      - target_in_range(5)
    cast: PLAGUE_STRIKE on CURRENT_DPS_TARGET

  # ───────── Tier 5: Obliterate (primary builder) ─────────
  - tier: 5
    name: Obliterate — 1F+1U runes available
    when:
      - rune.available(FROST) >= 1
      - rune.available(UNHOLY) >= 1
      - target_in_range(5)
      - runic_power < 88
    cast: OBLITERATE on CURRENT_DPS_TARGET
    notes: Primary builder. Skip if RP >= 88 (overcap risk on next gain).

  # ───────── Tier 6: Frost Strike — overcap-prevention dump ─────────
  - tier: 6
    name: Frost Strike — RP dump at 88+ (prevent overcap)
    when:
      - runic_power >= 88
      - target_in_range(5)
    cast: FROST_STRIKE on CURRENT_DPS_TARGET

  - tier: 6
    name: Frost Strike — high RP general dump
    when:
      - runic_power >= 80
      - target_in_range(5)
    cast: FROST_STRIKE on CURRENT_DPS_TARGET
    notes: Tier-6.5 — alternates with Obliterate when RP nearing cap.

  # ───────── Tier 7: Howling Blast no-Rime filler ─────────
  - tier: 7
    name: Howling Blast — no-Rime filler (Frost rune surplus)
    when:
      - rune.available(FROST) >= 1
      - rune.available(UNHOLY) == 0
      - aura.expires_in_ms(CURRENT_DPS_TARGET, FROST_FEVER) >= 6000
      - target_in_range(30)
    cast: HOWLING_BLAST on CURRENT_DPS_TARGET
    notes: Frost-rune dump when no Obliterate possible (UH on CD).

  # ───────── Tier 8: Frost Strike — standard RP dump ─────────
  - tier: 8
    name: Frost Strike — RP dump at >=32 (standard)
    when:
      - runic_power >= 32
      - target_in_range(5)
    cast: FROST_STRIKE on CURRENT_DPS_TARGET
    notes: Final-tier RP spend; lower priority than Obliterate but ensures uptime.

  # ───────── Tier 9: Horn of Winter — RP-starvation filler ─────────
  - tier: 9
    name: Horn of Winter — RP filler when no rune action available
    when:
      - runic_power < 32
      - rune.available(FROST) == 0
      - rune.available(UNHOLY) == 0
      - rune.available(BLOOD) == 0
      - spell.off_cooldown(HORN_OF_WINTER)
    cast: HORN_OF_WINTER on SELF
    notes: GCD-filler for total rune starvation. Generates 10 RP.
```

---

## AoE Mode

Activated by `enemies_within_10y >= 3`. Hysteresis: exit `<2 sustained for 5s`.

```yaml
rotation_aoe:
  - tier: 1
    name: Disease seed via Outbreak (priority target)
    when:
      - level >= 81
      - aura.missing(PRIMARY_AOE_TARGET, [FROST_FEVER, BLOOD_PLAGUE])
      - spell.off_cooldown(OUTBREAK)
    cast: OUTBREAK on PRIMARY_AOE_TARGET

  - tier: 2
    name: Pestilence — spread diseases 10y from seeded target
    when:
      - aura.active(PRIMARY_AOE_TARGET, [FROST_FEVER, BLOOD_PLAGUE])
      - any_target_within_10y_of(PRIMARY_AOE_TARGET).aura.missing(FROST_FEVER)
      - rune.available(BLOOD) >= 1
    cast: PESTILENCE on PRIMARY_AOE_TARGET

  - tier: 3
    name: Howling Blast — Rime free cast (AoE)
    when:
      - aura.active(SELF, RIME_PROC)
    cast: HOWLING_BLAST at PRIMARY_AOE_TARGET

  - tier: 4
    name: Howling Blast — Frost rune dump (AoE primary)
    when:
      - rune.available(FROST) >= 1
    cast: HOWLING_BLAST at PRIMARY_AOE_TARGET

  - tier: 5
    name: Blood Boil — Blood/Death rune AoE dump
    when:
      - rune.available(BLOOD) >= 1
      - enemies_within_10y_of_self >= 3
    cast: BLOOD_BOIL

  - tier: 6
    name: Frost Strike — single-target overcap-prevention even in AoE
    when:
      - runic_power >= 88
    cast: FROST_STRIKE on CURRENT_DPS_TARGET

  - tier: 7
    name: Obliterate — UH+F rune availability (single-target damage in AoE)
    when:
      - rune.available(FROST) >= 1
      - rune.available(UNHOLY) >= 1
    cast: OBLITERATE on CURRENT_DPS_TARGET

  - tier: 8
    name: Frost Strike — standard RP dump in AoE
    when:
      - runic_power >= 32
    cast: FROST_STRIKE on CURRENT_DPS_TARGET

notes: |
  AoE swap also enables Glyph of Howling Blast crit (UNVERIFIED — some
  sources say diseased-target bonus, not crit). Bot logic does not depend
  on glyph state — same priority either way.
```

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: PILLAR_OF_FROST
    when:
      - in_combat
      - spell.off_cooldown(PILLAR_OF_FROST)
      - any_of:
          - boss_engagement OR fight_just_started
          - cooldown_window_aligned (ERW + Raise Dead)
    rationale: 1min CD; sync first w/ ERW + RD + Golemblood pot.

  - spell: EMPOWER_RUNE_WEAPON
    when:
      - in_combat
      - spell.off_cooldown(EMPOWER_RUNE_WEAPON)
      - any_of:
          - boss_engagement (first use)
          - rune.available_total <= 1 AND aura.active(SELF, PILLAR_OF_FROST)
    rationale: 5min CD; first stack with pull, second as "rune panic".

  - spell: RAISE_DEAD
    when:
      - in_combat
      - spell.off_cooldown(RAISE_DEAD)
      - any_of:
          - boss_engagement
          - cooldown_window_aligned (Pillar of Frost)
    rationale: 3min CD; 1min ghoul DPS + Death Pact enabler.

  - spell: ANTI_MAGIC_SHELL
    when:
      - any_of:
          - SELF.taking_magic_damage
          - SELF.hp_pct < 80 AND last_incoming_school == magic
      - spell.off_cooldown(ANTI_MAGIC_SHELL)
    rationale: 45s CD; defensive + RP gen. Pre-emptive on known magic.

  - spell: ICEBOUND_FORTITUDE
    when:
      - SELF.hp_pct < 40
      - no_other_save_ready
      - spell.off_cooldown(ICEBOUND_FORTITUDE)
    rationale: 3min CD emergency defensive + stun-immune.

  - spell: DEATH_PACT
    when:
      - SELF.hp_pct < 25
      - aura.active(SELF, RAISE_DEAD_GHOUL)
      - no_other_heal_ready
      - spell.off_cooldown(DEATH_PACT)
    rationale: 2min emergency self-heal (sacrifices ghoul).

  - spell: MIND_FREEZE
    when:
      - target.casting AND target.cast_remaining_ms > 300
      - target.cast_interruptible
      - target_in_range(10)
      - spell.off_cooldown(MIND_FREEZE)
    rationale: 10s CD off-GCD interrupt. Free.

  - spell: STRANGULATE
    when:
      - target.casting AND target.cast_remaining_ms > 300
      - target.cast_interruptible
      - target_in_range(30)
      - spell.on_cooldown(MIND_FREEZE) OR not_in_range(10)
      - rune.available(BLOOD) >= 1
      - spell.off_cooldown(STRANGULATE)
    rationale: 2min ranged backup interrupt.

  - spell: DEATH_GRIP
    when:
      - manual_command OR (target.out_of_melee AND boss_target AND no_other_valid_action)
      - spell.off_cooldown(DEATH_GRIP)
    rationale: 35s utility CD; gap-close + forced-pull. NOT primary threat.
```

---

## Proc Reactions

```yaml
procs:
  KILLING_MACHINE_PROC:
    id: 51124                         # UNVERIFIED — proc-aura ID
    triggered_by: melee auto-attack OR Howling Blast
    proc_chance_per_minute: ~10        # UNVERIFIED — talent-rate scaled
    effect: next Frost Strike OR Obliterate guaranteed crit
    duration_ms: 30000                # UNVERIFIED 15s vs 30s
    ai_behavior: |
      Tier-2 priority — consume on next cast. Preference order:
      1. Frost Strike if RP >= 32 (Mastery-scaled crit on FS)
      2. Obliterate if RP < 32 AND 1F+1U runes available
      3. Hold if neither possible until rune/RP refilled (rare)

  RIME_PROC:
    id: 59052                         # UNVERIFIED — proc-aura ID
    triggered_by: Obliterate cast
    proc_chance: ~15%                 # UNVERIFIED — Rime talent rank scales
    effect: next Howling Blast costs 0 runes (free) + bonus damage (UNVERIFIED)
    duration_ms: 15000
    single_stack: true
    ai_behavior: |
      Tier-3 priority — immediate consumption. Single-cast aura; cast-on-proc.

  FROZEN_HEART_MASTERY:
    mastery_passive: true
    triggered_by: Frost-school damage (FS, HB, FF tick, OB Frost portion)
    effect: +% damage scaling with Mastery rating
    base_pct: 16.0                    # UNVERIFIED Frost-mastery base
    per_point_pct: 2.0                # UNVERIFIED per-point
    ai_behavior: Passive — informs reforge priority + KM consumption.

  THREAT_OF_THASSARIAN:
    talent_id: 65661
    talent_rank_max: 3
    triggered_by: Death Strike, Frost Strike, Plague Strike, Obliterate, Rune Strike
    effect: |
      DW build: chance for off-hand copy of strike. 2H build: 100%
      passive +RP generation on Frost Strike (UNVERIFIED scaling).
    ai_behavior: 2H Frost — passive; no AI gating. DW Frost — passive; no gating.

  ANNIHILATION:
    talent_id: 51468
    talent_rank_max: 3
    effect: |
      Obliterate ignores diseases for damage when KM NOT active (so KM
      consumption is Strength-scaled, not disease-bonus-scaled). 3/3
      mandatory in 31/7/3 build.
    ai_behavior: Passive — informs KM-consumption preference order.
```

---

## Resource Management

```yaml
runes:
  pool: 6                             # 2 Blood + 2 Frost + 2 Unholy
  recharge_per_rune_ms: 10000         # haste-modifiable; Runic Focus passive
  death_runes:                        # converted runes consumable as any color
    sources:
      - Death_Strike: blood→death
      - Blood_of_the_North_talent: blood→death (permanent, Frost spec)
      - Reaping_talent: blood→death (Unholy spec; not Frost)
  notes: |
    Frost spec auto-converts both Blood runes to Death runes via Blood of
    the North (3/3 talent, default-talented). So Frost has access to:
    2 Frost + 2 Unholy + 2 Death (consumable as anything).

    Bot rune state must track: per-rune {color, ready_at_ms, is_death}.
    Action gating: rune.available(Frost) checks for any Frost rune ready
    OR any Death rune ready.

runic_power:
  pool: 100
  decay_when_out_of_combat: true       # ~3 RP/sec out-of-combat decay
  generators:
    - Obliterate: 20 RP gain
    - Howling_Blast: 10 RP gain
    - Plague_Strike: 10 RP gain
    - Pestilence: 10 RP gain
    - Blood_Boil: 10 RP gain
    - Death_Strike: 20 RP gain
    - Horn_of_Winter: 10 RP gain
    - Anti_Magic_Shell: variable (absorbed damage → RP; UNVERIFIED rate)
    - Empower_Rune_Weapon: instant 25 RP grant
  spenders:
    - Frost_Strike: 32 RP (Frost-talent reduction from 40)
    - Death_Coil: 32 RP (Frost skips)
    - Death_Pact: 40 RP (emergency only)
  overcap_threshold: 88                # cast Frost Strike before next OB gain

action_eligibility:
  obliterate: rune.available(FROST) >= 1 AND rune.available(UNHOLY) >= 1
  howling_blast_paid: rune.available(FROST) >= 1
  howling_blast_free: aura.active(SELF, RIME_PROC)
  plague_strike: rune.available(UNHOLY) >= 1
  pestilence: rune.available(BLOOD) >= 1
  blood_boil: rune.available(BLOOD) >= 1
  frost_strike: runic_power >= 32
  death_coil: runic_power >= 32
  outbreak: spell.off_cooldown(OUTBREAK) AND level >= 81
```

---

## Consumables

```yaml
consumables:
  - item: GOLEMBLOOD_POTION             # +Strength 25s
    when:
      - boss_engagement (pre-pot)
      - cooldown_window_aligned (Pillar + ERW + RD)
    notes: Pre-pot 1s before pull; combat-pot 1min mark when CDs realign.

  - item: FLASK_OF_TITANIC_STRENGTH     # +Strength flask
    when:
      - in_combat AND aura.missing(SELF, FLASK_BUFF)
    notes: Always-on flask.

  - item: SCROLL_OF_STRENGTH_VIII
    when:
      - aura.missing(SELF, [BLESSING_OF_KINGS, MARK_OF_THE_WILD, HORN_OF_WINTER, BATTLE_SHOUT])
    notes: Stat scroll fallback if no party Str/Agi buff.

  - item: STARFIRE_DIAMOND              # +AGI/STR meta gem (UNVERIFIED 4.3 default)
    when: passive
    notes: Helm meta-gem.

  - item: SHARPENING_STONE_PYRIUM       # +500 weapon damage 60min
    when:
      - in_combat AND aura.missing(SELF, WEAPON_STONE_BUFF)
    notes: Re-apply hourly.

  - item: FOOD_BUFF_BEER_BASTED_CROCOLISK  # +90 Strength
    when:
      - in_combat AND aura.missing(SELF, FOOD_BUFF)
    notes: Always-on food buff.
```

---

## Non-encounter content

### Solo combat

```yaml
solo:
  - condition: open_world AND level < 85
    rotation: |
      Outbreak (L81+) OR HB+PS (sub-81) → Obliterate spam → Frost Strike
      RP dumps → Howling Blast no-Rime filler. KM/Rime procs as encountered.
      No CD usage (Pillar/ERW/RD held for elite/group encounters).
  - condition: solo_elite_target
    rotation: standard rotation + Pillar of Frost on engagement; RD if HP < 50%; AMS pre-magic-cast.
  - condition: low_hp_self
    cast: DEATH_STRIKE (HP < 35%) OR DEATH_PACT (HP < 25% AND ghoul up) OR ICEBOUND_FORTITUDE (HP < 40%).
```

### Travel & out-of-combat

```yaml
travel:
  - condition: master_riding AND not_combat
    cast: PATH_OF_FROST IF on water surface
    cast: standard mount (handled by AltbotMount)
  - condition: out_of_combat AND aura.missing(SELF, HORN_OF_WINTER_BUFF)
    cast: HORN_OF_WINTER on SELF
    notes: Pre-buff HoW for raid Str/Agi parity if no Kings/MotW.
  - condition: out_of_combat AND aura.missing(SELF, FROST_PRESENCE)
    cast: FROST_PRESENCE
```

### Idle

```yaml
idle:
  - condition: out_of_combat AND no_active_target
    behavior: |
      No-op. Frost DK has no out-of-combat ability worth proactive cast
      (Death's Advance passive; Path of Frost only on water; HoW already
      handled in travel).
  - condition: out_of_combat AND HP < 50%
    cast: DEATH_STRIKE on dummy (UNVERIFIED — DS heal works on training-dummy hits only); fallback Bandage
    notes: Cata Death Strike heals based on damage taken last 5s; idle HP-low has no heal vector.
```

### Disengage

```yaml
disengage:
  - condition: master.fleeing AND in_combat
    behavior: Drop combat → switch to Frost Presence (default) → reset RP decay tick.
  - condition: bot.HP < 25% AND no_save_ready
    cast: ICEBOUND_FORTITUDE OR DEATH_PACT (if ghoul up).
```

---

## Leveling rotation

DK starts L55 (hero class). No L1-L54 content.

### Spec recommendation

```yaml
spec_choice:
  for_leveling_dungeon_dps_bot:
    spec: Frost
    talent_split: 31 Frost / 7 Blood / 3 Unholy
    talent_picks_frost: |
      Improved Icy Touch 3/3, Runic Power Mastery 2/2, Toughness 3/3,
      Icy Reach 2/2, Black Ice 5/5, Nerves of Cold Steel 2/3 (or
      0/3 for 2H, points into Annihilation), Annihilation 3/3,
      Killing Machine 5/5, Chill of the Grave 2/2, Pillar of Frost 1/1,
      Endless Winter 2/2, Threat of Thassarian 3/3, Blood of the North 3/3,
      Howling Blast 1/1
      # 31-pt Frost capstone: Howling Blast (technically learned at trainer; Frost talent unlocks usage in spec)
    talent_picks_blood: |
      Butchery 2/2, Scent of Blood 1/3 OR 0/3, Improved Blood Tap 2/2,
      Blade Barrier 3/3 (filler — defensive), or Bladed Armor 3/3
      (Strength on armor).
    talent_picks_unholy: |
      Virulence 3/3 (mandatory for disease damage in Frost rotation).
    rationale: Icy Veins 31/7/3 consensus 6/8 sources. 31/5/5 Lichborne split is encounter-tunable but not default.

  alternatives:
    DW_Frost:
      talent_split: 31 Frost / 5 Blood / 5 Unholy
      key_changes: Nerves of Cold Steel 3/3 (DW hit) + Threat of Thassarian 3/3 (off-hand copies)
      glyph_change: Glyph of Frost Strike (DW preferred)
      build_choice_at_level: 60+
    Lichborne_31_5_5:
      talent_split: 31 Frost / 5 Blood / 5 Unholy
      key_changes: 5pts Blood for Lichborne reach (self-CC-break)
      situational: encounter-tunable for fights with frequent fear/stun (e.g., Domo's lightning, Spine adds).
```

### Spell unlock table

```yaml
spell_unlocks:
  # ─────── L55 baseline (all DKs at character creation) ───────
  DEATH_STRIKE:           {level: 55, spell_id: 49998}
  DEATH_GRIP:             {level: 55, spell_id: 49576}
  ICY_TOUCH:              {level: 55, spell_id: 45477}
  PLAGUE_STRIKE:          {level: 55, spell_id: 45462}
  RAISE_DEAD:             {level: 55, spell_id: 46584}
  DEATH_COIL:             {level: 55, spell_id: 47541}
  ANTI_MAGIC_SHELL:       {level: 55, spell_id: 48707}
  BLOOD_PRESENCE:         {level: 55, spell_id: 48263}
  FROST_PRESENCE:         {level: 55, spell_id: 48266}
  UNHOLY_PRESENCE:        {level: 55, spell_id: 48265}
  RAISE_ALLY:             {level: 56, spell_id: 61999}
  HORN_OF_WINTER:         {level: 56, spell_id: 57330}
  PATH_OF_FROST:          {level: 56, spell_id: 3714}
  STRANGULATE:            {level: 56, spell_id: 47476}
  CHAINS_OF_ICE:          {level: 57, spell_id: 45524}
  MIND_FREEZE:            {level: 57, spell_id: 47528}
  RUNE_TAP:               {level: 57, spell_id: 48982}
  DEATH_PACT:             {level: 58, spell_id: 48743}
  EMPOWER_RUNE_WEAPON:    {level: 58, spell_id: 47568}
  ICEBOUND_FORTITUDE:     {level: 58, spell_id: 48792}
  PESTILENCE:             {level: 58, spell_id: 50842}
  BLOOD_BOIL:             {level: 58, spell_id: 48721}
  ARMY_OF_THE_DEAD:       {level: 58, spell_id: 42650}

  # ─────── L60+ (mid-leveling) ───────
  DARK_COMMAND:           {level: 60, spell_id: 56222}    # taunt; tank only
  DEATH_AND_DECAY:        {level: 60, spell_id: 43265}

  # ─────── L70+ talent / spec gates ───────
  HOWLING_BLAST:          {level: 70, spell_id: 49184, requires_talent: frost_31pt OR trainer_at_70}
  OBLITERATE:             {level: 70, spell_id: 49020, requires_spec: frost_or_unholy_specced}  # UNVERIFIED gate
  FROST_STRIKE:           {level: 70, spell_id: 49143, requires_spec: frost}
  PILLAR_OF_FROST:        {level: 81, spell_id: 51271, requires_spec: frost}

  # ─────── L81+ Cata-new abilities ───────
  OUTBREAK:               {level: 81, spell_id: 77575}
  DARK_SIMULACRUM:        {level: 85, spell_id: 77606}    # spell-copy utility
  NECROTIC_STRIKE:        {level: 81, spell_id: 73975}    # Unholy-spec strike

  # ─────── Talent-gated (in addition to spec gating) ───────
  THREAT_OF_THASSARIAN:   {requires_talent: true}         # passive
  KILLING_MACHINE:        {requires_talent: true}         # passive proc
  RIME:                   {requires_talent: true}         # passive proc
  ANNIHILATION:           {requires_talent: true}         # passive
  BLOOD_OF_THE_NORTH:     {requires_talent: true}         # blood→death rune conversion
  CHILL_OF_THE_GRAVE:     {requires_talent: true}         # +RP on FS/HB/HB
  ENDLESS_WINTER:         {requires_talent: true}         # +Strength passive
  IMPROVED_ICY_TOUCH:     {requires_talent: true}         # +IT damage
  ICY_REACH:              {requires_talent: true}         # +range on FS/HB
  BLACK_ICE:              {requires_talent: true}         # +Frost/Shadow damage
  TOUGHNESS:              {requires_talent: true}         # armor%
  RUNIC_POWER_MASTERY:    {requires_talent: true}         # max RP +15/30
  VIRULENCE:              {requires_talent: true}         # +disease damage
  BUTCHERY:               {requires_talent: true}         # passive RP gen on hit
  SCENT_OF_BLOOD:         {requires_talent: true}         # RP gen on damage taken

  # ─────── Mastery / passives ───────
  MASTERY_FROZEN_HEART:   {passive_mastery: true, base_pct: 16.0, per_point_pct: 2.0}  # UNVERIFIED
  PLATE_SPECIALIZATION:   {passive: true, str_pct: 5}     # UNVERIFIED — DKs may not have Plate Spec
  RUNIC_FOCUS:            {passive: true}                 # haste-scales rune recharge
  BLOOD_OF_THE_NORTH_PASSIVE: {passive: true}             # 100% blood→death conversion when 3/3 talent
```

### Leveling brackets

```yaml
bracket_55:
  available: |
    DEATH_STRIKE, DEATH_GRIP, ICY_TOUCH, PLAGUE_STRIKE, RAISE_DEAD,
    DEATH_COIL, ANTI_MAGIC_SHELL, all 3 presences (Blood default at start)
  rotation: |
    Diseases via IT + PS → Death Strike spam → Death Coil RP dump.
    No spec yet (Blood Presence default through starter zone).

bracket_56_57:
  available: + HoW, Path of Frost, Strangulate, Chains of Ice (56), + Mind Freeze, Rune Tap (57)
  rotation: same as 55 + Mind Freeze interrupt + HoW pre-pull buff

bracket_58_60:
  available: |
    + Death Pact, ERW, Icebound Fortitude, Pestilence, Blood Boil, Army of the Dead (58),
    + Dark Command, Death and Decay (60)
    L58 trainer: spec choice (Frost). Switch to Frost Presence + take Frost talents.
  rotation: |
    Frost Presence + Frost spec at 58. Diseases via IT + PS until Frost gets
    Howling Blast at 70. Death Strike spam → Frost Strike (Frost-spec
    available?) — UNVERIFIED FS gate. Death Coil RP dump.

bracket_60_70:
  available: + Death and Decay (60); first Frost talent points (Killing Machine, Rime, Toughness, Black Ice via 31-pt build progression)
  rotation: |
    KM+Rime procs become real once talented. Pre-Howling-Blast: Plague Strike
    + Icy Touch + Death Strike + Frost Strike (if FS unlocked at 60 vs 70 —
    UNVERIFIED). Death Coil as RP dump fallback.

bracket_70_80:
  available: + Howling Blast (70), + Obliterate (70 via Frost spec), + Frost Strike (70 via Frost spec)
  rotation: |
    Full Frost rotation activates. Outbreak still locked (L81+). Disease
    application via Howling Blast (FF) + Plague Strike (BP) until Outbreak
    unlocks. KM, Rime, BotN, ToT, Annihilation talents all online.

bracket_81_84:
  available: + Pillar of Frost (81), + Outbreak (81), + Necrotic Strike (81 — Unholy-only)
  rotation: |
    Outbreak replaces HB+PS as primary disease applier. Pillar of Frost on
    1-min CD becomes primary burst CD. Standard ST priority list active.

bracket_85:
  available: + Dark Simulacrum (85)
  rotation: full encounter rotation per §"Rotation"
  notes: |
    Dark Simulacrum is utility-only (boss spell-copy); not in standard rotation.
    All raid-tier rotational abilities online by L85.
```

### Mana floors by era

DKs do NOT use mana — Runes + Runic Power. No mana floor section.

### Rune/RP starvation thresholds

```yaml
resource_starvation:
  rune_starvation:
    detect: rune.available_total == 0 AND runic_power < 32
    response: HORN_OF_WINTER if off-CD; else fall through to auto-attack
  rp_overcap_pre_empt: runic_power >= 88 → cast Frost Strike before next Obliterate
  rp_idle_decay: runic_power decays out-of-combat at ~3 RP/sec; not relevant in combat
```

---

## UNVERIFIED items

| # | Item | Spec value | Verification |
|---|---|---|---|
| 1 | Killing Machine proc-aura ID | 51124 | Spell.dbc / aura ID lookup |
| 2 | KM proc rate (per-minute or PPM) | ~10 PPM | Talent.dbc + class scaling |
| 3 | KM duration | 30s | Spell.dbc 51124 |
| 4 | Rime proc-aura ID | 59052 | Spell.dbc / aura ID lookup |
| 5 | Rime proc chance per Obliterate | ~15% (3/3 talent) | Talent.dbc |
| 6 | Rime: free HB also no RP gain? | UNVERIFIED | Test in-game |
| 7 | Frozen Heart Mastery base + per-point | 16.0% + 2.0%/pt | Mastery DBC |
| 8 | Threat of Thassarian 2H RP-gain bonus | UNVERIFIED scaling | Talent.dbc 65661 |
| 9 | Blood of the North conversion permanence | 3/3 = permanent BL→D conversion | Talent.dbc |
| 10 | Pillar of Frost rune cost | 1F vs 0 | Spell.dbc 51271 |
| 11 | Pillar of Frost off-GCD | UNVERIFIED | Spell.dbc 51271 |
| 12 | ERW off-GCD | UNVERIFIED | Spell.dbc 47568 |
| 13 | Anti-Magic Shell RP-conversion rate from absorbed dmg | UNVERIFIED | Spell.dbc 48707 |
| 14 | Icebound Fortitude CD (120s vs 180s on Cata) | 180s | Spell.dbc 48792 |
| 15 | Death Pact heal % | 50% | Spell.dbc 48743 |
| 16 | Death Grip RP gain (0 or 20) | UNVERIFIED | Spell.dbc 49576 |
| 17 | Outbreak rune cost | 0 | Spell.dbc 77575 |
| 18 | Howling Blast splash radius | 10y | Spell.dbc 49184 |
| 19 | Pestilence spread radius | 10y | Spell.dbc 50842 |
| 20 | Frost Fever / Blood Plague tick rate | 3s tick / 21s duration | Spell.dbc |
| 21 | Disease snapshot Strength on apply | true | Test in-game |
| 22 | Plate Specialization for DK | UNVERIFIED — DKs may not have Plate Spec | Class passive table |
| 23 | Frost Strike base RP cost vs talent reduction | 40 base → 32 Frost-spec | Spell.dbc 49143 |
| 24 | Glyph of Howling Blast effect (crit vs disease bonus) | UNVERIFIED | Glyph.dbc |
| 25 | Glyph of Frost Strike (+%damage or +crit) | UNVERIFIED | Glyph.dbc |
| 26 | Glyph of Obliterate effect | UNVERIFIED | Glyph.dbc |
| 27 | T13 Frost 2P set bonus | TBD | Item set DBC |
| 28 | T13 Frost 4P set bonus (Pillar +5s?) | TBD | Item set DBC |
| 29 | Annihilation: KM-active vs KM-inactive disease scaling | UNVERIFIED | Talent.dbc 51468 |
| 30 | Howling Blast pre-Frost-spec availability | UNVERIFIED — Frost talent vs trainer | Talent / spell gate |
| 31 | Frost Strike availability gate (L60 vs L70) | UNVERIFIED | Spell trainer table |
| 32 | Obliterate availability outside Frost spec | UNVERIFIED | Spell trainer table |
| 33 | Icy Touch base damage formula | TBD | Spell.dbc 45477 |
| 34 | Frost Presence +5% phys-on-crit (Cata change) | true | Spell.dbc 48266 |
| 35 | Death Strike heal-from-damage formula (5s window) | min 7% max HP | Spell.dbc 49998 |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kFrostDK_Spells[]` |
| Maintenance pass | `static const MaintenanceTrigger kFrostDK_Maintenance[]` |
| Rotation tiers | `static const RotationTier kFrostDK_Rotation[]` |
| AoE rotation | `static const RotationTier kFrostDK_RotationAoE[]` |
| Cooldown Profile | `static const CooldownTrigger kFrostDK_Cooldowns[]` |
| Rune state | per-rune {color, ready_at_ms, is_death} (6-rune array) |
| Runic Power | 0-100 tracker w/ overcap-pre-empt logic |
| Killing Machine proc | aura listener + KM-consumption preference |
| Rime proc | aura listener + immediate-cast trigger |
| Disease state | per-target {FF, BP} with remaining_ms |
| Frost Presence | presence-mode tracker |
| Forbearance equivalent | n/a (DKs have no Forbearance — Hand-of buffs are paladin-only) |
| Leveling rotation | `LevelBracketRotation kFrostDK_Leveling[]` |

**Key new helper APIs:**
- `RuneAvailable(bot, RuneColor)` — accept Frost / Unholy / Blood + Death-rune fallback.
- `RuneAvailableTotal(bot)` — count of all 6 ready-state runes.
- `RunicPower(bot)` — read 0-100.
- `KillingMachineActive(bot)` — proc aura tracker.
- `RimeActive(bot)` — proc aura tracker.
- `DiseaseRemainingMs(target, FROST_FEVER)` / `DiseaseRemainingMs(target, BLOOD_PLAGUE)`.
- `IsObliterateUsable(bot)` — `RuneAvailable(F) >= 1 AND RuneAvailable(U) >= 1`.
- `IsFrostStrikeUsable(bot)` — `RunicPower(bot) >= 32`.
- `ShouldPreEmptOvercap(bot)` — `RunicPower(bot) >= 88 AND IsObliterateUsable(bot)`.
- `IsAoEMode(bot)` — `enemies_within_10y_self >= 3`.

**Death Knight resource model is NEW for this codebase.** No existing class shares the rune+RP shape. Implementation plan:
1. Extend `AltbotAI` with `RuneState _runes[6]` array (per-rune cooldown timer).
2. Extend `AltbotAI` with `uint8_t _runicPower` (0-100).
3. Hook `Player::SpellHitTarget` (or similar) to update rune state on cast — TC's rune-spending API may already handle this; verify.
4. Add aura listener for KM/Rime procs (re-uses existing aura-tracking infra).
5. Disease tracking: per-target aura listener for FROST_FEVER (id TBD) + BLOOD_PLAGUE (id TBD).

Cross-reference to `docs/roles/melee-dps.md` for melee positioning patterns. No prior class peer.
