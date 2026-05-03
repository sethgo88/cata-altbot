# Retribution Paladin — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide.

**Strategy content** reconciled across 10 community sources. See `docs/research/retribution-paladin-guide-survey.md`. Cross-reference: Holy Power mechanics + most baseline paladin abilities are documented in `docs/specs/holy-paladin.md`.

**SPEC-CHOICE RATIONALE.** Ret is the only Paladin DPS spec; Holy is healer (already done), Prot is tank (deferred per role-ordering). This is the **third melee-DPS spec** in the project.

---

## Identity

```yaml
spec: retribution_paladin
class: paladin
role: MELEE_DPS
resource_model: mana + holy_power (max 3 charges)
defining_mechanic: Crusader Strike HP generation + Templar's Verdict HP spender + Inquisition +30% Holy buff maintenance + Censure DoT via Seal of Truth
defining_buffs:
  - Inquisition                 # +30% Holy damage; duration scales with HP spent (4s × HP)
  - Avenging_Wrath              # 3min CD (1.5min Sanctified Wrath); +20% damage 20s
  - Zealotry                    # 31-pt capstone, 2min CD; max HP for 20s
  - Guardian_of_Ancient_Kings_Ret # 5min CD, 30s; summon damage CD
  - Judgements_of_the_Pure      # +9% haste 60s from Judgement
  - Seal_of_Truth               # primary seal; on-hit Censure stack
  - Censure                     # 5-stack DoT auto-applied by Seal of Truth
  - Blessing_of_Might           # raid AP + MP5
defining_procs:
  - The_Art_of_War              # melee crit → free instant Exorcism
  - Mastery: Hand of Light      # +% Holy on TV/DS/CS/HotR
mastery: Hand_of_Light
key_cooldowns:
  - Avenging_Wrath              # 1.5min talent / 3min base
  - Zealotry                    # 2min, 20s, capstone
  - Guardian_of_Ancient_Kings_Ret # 5min, 30s
  - Lay_on_Hands                # 10min, full HP
  - Divine_Shield               # 5min, 8s immunity, Forbearance
  - Divine_Protection           # 1min, 10s, -20% magic
  - Hand_of_Freedom             # 25s, snare break
  - Hand_of_Protection          # 5min, phys immunity, Forbearance
  - Hand_of_Salvation           # 2min, -20% threat
  - Rebuke                      # 15s, off-GCD interrupt
melee_range: 5
notes: |
  Ret Paladin in 4.3.4 plays as a 2H plate melee with Holy Power as a
  secondary resource. Heartbeat: Seal of Truth always-on (Censure DoT auto-
  applied); Crusader Strike on CD generates 1 HP; Templar's Verdict spends
  3 HP for primary single-target damage; Inquisition self-buff +30% Holy
  maintained 100% uptime (refresh <4s + 3 HP); Judgement on CD refreshes
  Judgements of the Pure haste; Hammer of Wrath when usable (<20% HP OR
  Avenging Wrath OR Zealotry active); Exorcism only on Art of War proc.

  Burst window: AW + Zealotry + Guardian of Ancient Kings (Ret) + Golemblood
  pot stacked on first pull; drift independently after. Sanctified Wrath
  talent (default-talented) halves AW CD AND enables HoW during AW.

  Pre-Templar's Verdict (under L30): auto + CS + Seal of Righteousness; no
  HP-spender. Pre-Avenging Wrath / Hammer of Wrath / Divine Storm / Zealotry
  (L30/L36/talent/L30 capstone): basic single-target only.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  CURRENT_DPS_TARGET:
  ENEMY_CASTING_INTERRUPTIBLE:    # Rebuke range: 5y melee
  AOE_CLUSTER:                    # ≥2 enemies within 8y of self (Divine Storm)
  CC_MARKER_REPENTANCE:
  ALLY_PROTECTION_TARGET:         # Hand of Protection / Salvation / Freedom
  TANK:                           # Hand of Reckoning utility
```

---

## Spell Catalog

```yaml
spells:
  # ─────── HP generators ───────

  CRUSADER_STRIKE:
    id: 35395
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 4500
    on_gcd: true
    range: 5
    target_type: enemy_melee
    holy_power_gain: 1
    notes: Primary HP generator. On CD always.

  HAMMER_OF_WRATH:
    id: 24275
    mana_pct: 12
    cast_time_ms: 0
    cooldown_ms: 6000
    on_gcd: true
    range: 30
    target_type: enemy
    requires_one_of:
      - target.hp_pct <= 20
      - aura.active(SELF, AVENGING_WRATH)
      - aura.active(SELF, ZEALOTRY)
    notes: Conditional ranged execute. Sanctified Wrath enables AW/Zealotry use.

  JUDGEMENT:
    id: 20271
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 8000
    on_gcd: true
    range: 30
    target_type: enemy
    notes: Refreshes JotP haste +9% / 60s. On CD.

  EXORCISM:
    id: 879
    mana_pct: 6
    cast_time_ms: 1500              # instant via Art of War proc
    cooldown_ms: 15000
    on_gcd: true
    range: 30
    target_type: enemy
    notes: Instant-only via Art of War proc. Hardcast skipped.

  HOLY_WRATH:
    id: 2812
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 15000
    on_gcd: true
    range: 0
    target_type: pbaoe
    radius: 10
    notes: AoE Holy damage; stuns Demons/Undead/Elemental/Dragonkin. Filler.

  CONSECRATION:
    id: 26573
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 10000
    notes: AoE ground DoT. AoE-only; mana-gated.

  # ─────── HP spenders ───────

  TEMPLARS_VERDICT:
    id: 85256
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    holy_power_cost: 3
    requires_spec: ret
    notes: Primary ST finisher. Spend at 3 HP.

  DIVINE_STORM:
    id: 53385
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: pbaoe
    radius: 8
    holy_power_cost: 3
    requires_spec: ret
    requires_talent: true
    notes: AoE finisher. 4+ enemies replaces TV.

  INQUISITION:
    id: 84963
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    holy_power_cost: 1                # 1/2/3 → 4/8/12s buff
    requires_spec: ret
    notes: |
      +30% Holy damage; duration scales 4s/HP. Refresh at <4s remaining
      with 3 HP. Always-on maintenance.

  WORD_OF_GLORY:
    id: 85673
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    holy_power_cost: 1
    notes: Defensive HP-spender heal. Self HP < 50% AND 3 HP AND Inq > 8s.

  # ─────── Cooldowns ───────

  AVENGING_WRATH:
    id: 31884
    mana_pct: 8
    cast_time_ms: 0
    cooldown_ms: 90000              # 1.5min with Sanctified Wrath
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 20000
    requires_spec: ret
    notes: +20% damage all sources. On CD.

  ZEALOTRY:
    id: 85696
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 120000              # 2 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 20000
    requires_spec: ret
    requires_talent: true
    notes: Max HP for duration; pair with AW first pull.

  GUARDIAN_OF_ANCIENT_KINGS_RET:
    id: 86698
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 300000              # 5 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 30000
    requires_spec: ret
    requires_level: 85
    notes: Summon Ancient Kings; periodic damage. Major DPS CD.

  LAY_ON_HANDS:
    id: 633
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 600000              # 10 min
    on_gcd: false
    range: 40
    target_type: ally_or_self
    forbearance_apply: true
    notes: Full HP heal. Emergency.

  DIVINE_SHIELD:
    id: 642
    mana_pct: 6
    cast_time_ms: 0
    cooldown_ms: 300000              # 5 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 8000
    forbearance_apply: true
    notes: Full immunity 8s. -50% damage done debuff during.

  DIVINE_PROTECTION:
    id: 498
    mana_pct: 3
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 10000
    notes: -20% magic damage 10s (Glyph removes magic-only).

  HAND_OF_FREEDOM:
    id: 1044
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 25000
    on_gcd: false
    range: 30
    target_type: ally
    duration_ms: 8000
    notes: Removes movement-impairing + immunity to them.

  HAND_OF_PROTECTION:
    id: 1022
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 300000              # 5 min
    on_gcd: false
    range: 30
    target_type: ally
    duration_ms: 10000
    forbearance_apply: true
    notes: Full physical immunity. Cannot use on tanks during melee.

  HAND_OF_SALVATION:
    id: 1038
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 120000
    on_gcd: false
    range: 40
    target_type: ally
    duration_ms: 10000
    notes: -20% threat reduction.

  HAND_OF_RECKONING:
    id: 62124
    mana_pct: 3
    cast_time_ms: 0
    cooldown_ms: 8000
    on_gcd: true
    range: 30
    target_type: enemy
    notes: Ranged taunt analog. Off-tank pickup.

  REBUKE:
    id: 96231
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 15000
    on_gcd: false
    range: 5
    target_type: enemy_casting
    duration_ms: 4000               # 4s lockout
    notes: Interrupt. Off-GCD.

  HAMMER_OF_JUSTICE:
    id: 853
    mana_pct: 4
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: true
    range: 10
    target_type: enemy
    duration_ms: 6000               # 6s stun
    notes: CC stun.

  REPENTANCE:
    id: 20066
    mana_pct: 9
    cast_time_ms: 1500
    cooldown_ms: 60000
    on_gcd: true
    range: 30
    target_type: enemy_humanoid_dragonkin_giant_undead_demon
    duration_ms: 6000
    requires_talent: true
    notes: 6s incapacitate; breaks on damage. CC.

  # ─────── Buffs / auras ───────

  SEAL_OF_TRUTH:
    id: 31801
    mana_pct: 14
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000             # 30 min
    requires_spec: ret
    notes: Primary Ret seal. On-hit Censure stack.

  SEAL_OF_RIGHTEOUSNESS:
    id: 20154
    mana_pct: 14
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    notes: AoE seal; 4+ targets.

  SEAL_OF_INSIGHT:
    id: 20165
    mana_pct: 14
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    notes: Solo / leveling mana sustain.

  CENSURE:
    spell_id: 31803                  # debuff
    triggered_by: Seal of Truth on-hit
    max_stacks: 5
    notes: Auto-applied DoT. Refreshes per swing at 5 stacks.

  BLESSING_OF_MIGHT:
    id: 19740
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: party_or_raid
    duration_ms: 300000              # 5 min Cata
    notes: AP + MP5 raid buff.

  BLESSING_OF_KINGS:
    id: 20217
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: party_or_raid
    duration_ms: 300000
    notes: +5% all stats. Conditional vs Mark of the Wild.

  RETRIBUTION_AURA:
    id: 7294
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self_aura
    duration_ms: 1800000
    notes: Holy damage to attackers. Default Ret aura.

  REDEMPTION:
    id: 7328
    mana_pct: 60
    cast_time_ms: 10000
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: corpse
    notes: Out-of-combat resurrect.
```

---

## Maintenance pass

```yaml
maintenance:
  - name: Seal of Truth (default)
    when:
      - aura.missing(SELF, SEAL_OF_TRUTH)
      - NOT runtime_flag.use_aoe_seal
    cast: SEAL_OF_TRUTH

  - name: Seal of Righteousness (AoE swap)
    when:
      - aura.missing(SELF, SEAL_OF_RIGHTEOUSNESS)
      - runtime_flag.use_aoe_seal
      - enemies_within_8y_of_self >= 4
    cast: SEAL_OF_RIGHTEOUSNESS

  - name: Retribution Aura
    when:
      - aura.missing(SELF, RETRIBUTION_AURA)
    cast: RETRIBUTION_AURA

  - name: Blessing of Might
    when:
      - aura.missing(SELF, BLESSING_OF_MIGHT_BUFF)
      - any_raid_member_missing_buff(BLESSING_OF_MIGHT_BUFF)
    cast: BLESSING_OF_MIGHT on raid

  - name: Inquisition pre-buff (1 HP from CS)
    when:
      - in_combat
      - holy_power >= 1
      - aura.missing(SELF, INQUISITION_BUFF)
    cast: INQUISITION on SELF
    notes: First Inquisition cast; only at HP=1 if no recent Inq.
```

---

## Rotation — Priority List (Single-Target)

```yaml
rotation:
  # ───────── Tier 1: Inquisition refresh ─────────
  - tier: 1
    name: Inquisition refresh — <4s remaining + 3 HP
    when:
      - aura.expires_in_ms(SELF, INQUISITION_BUFF) < 4000
      - holy_power == 3
    cast: INQUISITION on SELF

  # ───────── Tier 2: Templar's Verdict at 3 HP ─────────
  - tier: 2
    name: Templar's Verdict — 3 HP spender
    when:
      - holy_power == 3
      - aura.active(SELF, INQUISITION_BUFF) AND aura.expires_in_ms(SELF, INQUISITION_BUFF) >= 4000
      - target_in_range(5)
    cast: TEMPLARS_VERDICT on CURRENT_DPS_TARGET

  # ───────── Tier 3: Crusader Strike on CD ─────────
  - tier: 3
    name: Crusader Strike on CD
    when:
      - spell.off_cooldown(CRUSADER_STRIKE)
      - holy_power < 3
      - target_in_range(5)
    cast: CRUSADER_STRIKE on CURRENT_DPS_TARGET

  # ───────── Tier 4: Hammer of Wrath ─────────
  - tier: 4
    name: Hammer of Wrath — when usable
    when:
      - any of:
          - CURRENT_DPS_TARGET.hp_pct <= 20
          - aura.active(SELF, AVENGING_WRATH)
          - aura.active(SELF, ZEALOTRY)
      - spell.off_cooldown(HAMMER_OF_WRATH)
      - target_in_range(30)
    cast: HAMMER_OF_WRATH on CURRENT_DPS_TARGET

  # ───────── Tier 5: Exorcism on Art of War proc ─────────
  - tier: 5
    name: Exorcism — Art of War instant
    when:
      - aura.active(SELF, ART_OF_WAR_PROC)
      - target_in_range(30)
    cast: EXORCISM on CURRENT_DPS_TARGET

  # ───────── Tier 6: Judgement on CD ─────────
  - tier: 6
    name: Judgement on CD
    when:
      - spell.off_cooldown(JUDGEMENT)
      - target_in_range(30)
    cast: JUDGEMENT on CURRENT_DPS_TARGET
    notes: Refreshes Judgements of the Pure haste.

  # ───────── Tier 7: Holy Wrath filler ─────────
  - tier: 7
    name: Holy Wrath filler
    when:
      - spell.off_cooldown(HOLY_WRATH)
      - SELF.mana_pct > 30
      - target_in_range(10)
    cast: HOLY_WRATH

  # ───────── Tier 8: Consecration (AoE only) ─────────
  - tier: 8
    name: Consecration — AoE
    when:
      - enemies_within_8y_of_self >= 4
      - SELF.mana_pct > 50
      - spell.off_cooldown(CONSECRATION)
    cast: CONSECRATION at SELF
```

---

## AoE Mode

Activated by `enemies_within_8y_of_self >= 4`. Hysteresis: exit `<3 sustained for 5s`.

```yaml
rotation_aoe:
  - tier: 1: Inquisition refresh
  - tier: 2: Divine Storm at 3 HP (replaces TV at 4+ enemies)
  - tier: 3: Crusader Strike on CD
  - tier: 4: Hammer of Wrath when usable
  - tier: 5: Exorcism on AoW proc
  - tier: 6: Judgement on CD
  - tier: 7: Consecration on CD
  - tier: 8: Holy Wrath filler
notes: Swap to Seal of Righteousness on AoE entry; revert to Seal of Truth on exit.
```

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: AVENGING_WRATH
    when:
      - in_combat
      - spell.off_cooldown(AVENGING_WRATH)
      - any of:
          - boss_engagement OR fight_just_started
          - cooldown_window_aligned (Zealotry + GoAK)
    rationale: 1.5min CD with Sanctified Wrath; sync first w/ Zealotry + GoAK.

  - spell: ZEALOTRY
    when:
      - in_combat
      - spell.off_cooldown(ZEALOTRY)
      - any of:
          - boss_engagement
          - aura.active(SELF, AVENGING_WRATH)
    rationale: 2min CD; align first w/ AW.

  - spell: GUARDIAN_OF_ANCIENT_KINGS_RET
    when:
      - in_combat
      - level >= 85
      - spell.off_cooldown(GUARDIAN_OF_ANCIENT_KINGS_RET)
      - any of:
          - boss_engagement
          - aura.active(SELF, AVENGING_WRATH)
    rationale: 5min CD; align first w/ AW + Zealotry.

  - spell: LAY_ON_HANDS
    when:
      - SELF.hp_pct < 15
      - no_other_save_ready_in_1500ms
      - aura.missing(SELF, FORBEARANCE)
      - spell.off_cooldown(LAY_ON_HANDS)
    rationale: 10min CD emergency.

  - spell: DIVINE_SHIELD
    when:
      - SELF.hp_pct < 10
      - no_other_save_ready
      - aura.missing(SELF, FORBEARANCE)
      - spell.off_cooldown(DIVINE_SHIELD)
    rationale: 5min CD last-resort.

  - spell: DIVINE_PROTECTION
    when:
      - SELF.hp_pct < 60
      - SELF.taking_magic_damage
      - spell.off_cooldown(DIVINE_PROTECTION)
    rationale: 1min CD magic mitigation.

  - spell: REBUKE
    when:
      - target.casting AND target.cast_remaining_ms > 300
      - target_in_range(5)
      - spell.off_cooldown(REBUKE)
    rationale: 15s CD off-GCD interrupt.

  - spell: HAMMER_OF_JUSTICE
    when:
      - manual_command OR (target.casting AND rebuke.on_cd AND target_in_range(10))
      - spell.off_cooldown(HAMMER_OF_JUSTICE)
    rationale: Backup interrupt / CC.

  - spell: WORD_OF_GLORY
    when:
      - SELF.hp_pct < 50
      - holy_power == 3
      - aura.expires_in_ms(SELF, INQUISITION_BUFF) > 8000
    rationale: Defensive HP spend (rare; emergency only).
```

---

## Proc Reactions

```yaml
procs:
  ART_OF_WAR:
    talent_id: 87138             # UNVERIFIED
    triggered_by: melee crit
    effect: next Exorcism free + instant + 100% damage
    duration_ms: 15000           # UNVERIFIED
    ai_behavior: Tier 5 priority — consume immediately on proc.

  HAND_OF_LIGHT_MASTERY:
    mastery_passive: true
    triggered_by: TV/DS/CS/HotR cast
    effect: extra Holy damage (% scales with Mastery)
    base_pct: 16.8                # UNVERIFIED
    per_point_pct: 2.1
    ai_behavior: Passive — informs reforge.

  CENSURE:
    seal_passive: true
    triggered_by: Seal of Truth on-hit
    effect: stacking DoT (5 max); refreshes per swing at 5
    ai_behavior: Auto-applied; bot doesn't manage.

  JUDGEMENTS_OF_THE_PURE:
    talent_passive: true
    triggered_by: Judgement cast
    effect: +9% spell haste 60s
    ai_behavior: Tier 6 (Judgement on CD) maintains.

  PLATE_SPECIALIZATION:
    class_passive: true
    requires: all-plate equipped
    effect: +5% Strength
    ai_behavior: Passive — gear check.

  TWO_HANDED_SPECIALIZATION:
    talent_passive: true
    requires: 2H weapon equipped
    effect: +20% damage with 2H weapons
    ai_behavior: Passive — required for Ret.
```

---

## Resource Management

```yaml
mana:
  thresholds:
    drink_floor_pct: 30
    drink_resume_pct: 95
    conserve_mode_pct: 50
    crisis_mode_pct: 25

  conserve_behavior:
    when: SELF.mana_pct < 50
    rules:
      - skip Holy Wrath filler
      - skip Consecration even in AoE
      - keep priority core (Inq + TV + CS + Judge + HoW + Exo)

  crisis_behavior:
    when: SELF.mana_pct < 25
    rules:
      - swap to Seal of Insight (mana regen)
      - skip all fillers; CS + TV + Inq only
      - Word of Glory self-heal if HP < 30

  out_of_combat:
    drink_when:
      - not in_combat
      - SELF.mana_pct < drink_floor_pct
      - no enemies within 40y for 5000ms
    eat_when: out_of_combat AND SELF.hp_pct < 80

holy_power:
  max: 3
  generation:
    crusader_strike: +1 per cast
    hammer_of_wrath_crit: +1 (Sacred Duty / Tower of Light talent)
    zealotry: max HP for 20s (every generator grants 3 HP)
  consumers:
    - Templar's Verdict: 3 HP (single-target)
    - Divine Storm: 3 HP (AoE, talent-gated)
    - Inquisition: 1/2/3 HP (4s × HP buff duration)
    - Word of Glory: 1/2/3 HP (heal scales)
```

---

## Consumables

```yaml
consumables:
  flask:
    item: Flask_of_Titanic_Strength
    item_id: 58088                  # +300 Strength

  food:
    primary:
      item: Beer_Basted_Crocolisk
      item_id: 62669                # +90 Strength

  dps_potion:
    item: Golemblood_Potion
    item_id: 58146
    use_when:
      - boss_pull_imminent_in_2s    # pre-pot
      - OR (in_combat AND fight_elapsed_ms ~ 60000 AND avenging_wrath_off_cd AND zealotry_off_cd)

  healthstone:
    item_id: 36892
```

---

## Non-encounter content

### Solo combat

```yaml
solo_offensive_rotation:
  - tier 1: SELF.hp_pct < 30 AND mana > 20 -> WORD_OF_GLORY self
  - tier 2: SELF.hp_pct < 15 AND no_forbearance -> LAY_ON_HANDS
  - tier 3: aura.missing(SELF, INQUISITION) AND holy_power >= 1 -> INQUISITION
  - tier 4: holy_power == 3 -> TV
  - tier 5: target.hp_pct <= 20 OR aura.active(AW) OR aura.active(ZEALOTRY) -> HAMMER_OF_WRATH
  - tier 6: aura.active(SELF, ART_OF_WAR_PROC) -> EXORCISM
  - tier 7: spell.off_cooldown(CS) -> CRUSADER_STRIKE
  - tier 8: spell.off_cooldown(JUDGE) -> JUDGEMENT
  - tier 9: spell.off_cooldown(HOLY_WRATH) AND mana > 30 -> HOLY_WRATH
notes: Pre-Templar's Verdict (under L30): auto + CS + Seal of Righteousness. Seal of Insight if mana-tight.
```

### Travel & out-of-combat

```yaml
travel:
  crusader_aura:
    use_when:
      - mounted
      - distance_to_destination >= 80
    cast: CRUSADER_AURA
    notes: +20% mounted speed (L62).

  mount:
    use_when:
      - not in_combat
      - distance_to_destination >= 80

ooc_maintenance:
  - check: aura.missing(SELF, SEAL_OF_TRUTH) AND NOT use_aoe_seal -> SEAL_OF_TRUTH
  - check: aura.missing(SELF, RETRIBUTION_AURA) -> RETRIBUTION_AURA
  - check: aura.missing(SELF, BLESSING_OF_MIGHT_BUFF) -> BLESSING_OF_MIGHT on raid
  - check: SELF.mana_pct < 80 AND no_enemies_within(40) -> drink
  - check: SELF.hp_pct < 95 AND no_enemies_within(40) -> eat
```

### Idle

```yaml
idle:
  primary_action: follow master
  secondary_actions:
    - maintain Seal of Truth + Retribution Aura
    - maintain Blessing of Might
  do_not:
    - do not engage neutral mobs unprompted
```

### Disengage

```yaml
disengage:
  divine_protection:
    use_when:
      - SELF.hp_pct < 60
      - SELF.taking_magic_damage
    cast: DIVINE_PROTECTION

  divine_shield:
    use_when:
      - SELF.hp_pct < 10
      - aura.missing(SELF, FORBEARANCE)
    cast: DIVINE_SHIELD

  hand_of_protection_self:
    use_when:
      - SELF.hp_pct < 30
      - taking_physical_damage
      - aura.missing(SELF, FORBEARANCE)
    cast: HAND_OF_PROTECTION on SELF
```

---

## Leveling rotation

Pure Ret from L10.

### Spec recommendation

```yaml
spec_choice:
  for_leveling_dungeon_dps_bot:
    spec: Retribution
    talent_split: 31 Ret / 7 Prot / 3 Holy
    talent_picks_ret: Two-Handed Specialization 3/3 + Sanctity of Battle 3/3 + Crusade 3/3 + The Art of War 3/3 + Sanctified Wrath 3/3 + Eye for an Eye 0-2 + Acts of Sacrifice 0-3 + Repentance 1/1 + Inquiry of Faith 3/3 + Seals of Command 1/1 + Long Arm of the Law 1/1 + Pursuit of Justice 0-2 + Rule of Law 3/3 + Divine Storm 1/1 + Zealotry 1/1
    talent_picks_prot: Toughness 3/3 + Improved Judgement 2/2 + Pursuit of Justice 2/2
    talent_picks_holy: Communion 2/2 + Divinity 1/3
    rationale: 31/7/3 Icy Veins consensus.
```

### Spell unlock table

```yaml
spell_unlocks:
  CRUSADER_STRIKE:           {level: 1,  spell_id: 35395}
  HAMMER_OF_JUSTICE:         {level: 1,  spell_id: 853}
  RIGHTEOUS_FURY:            {level: 1,  spell_id: 25780}
  SEAL_OF_RIGHTEOUSNESS:     {level: 3,  spell_id: 20154}
  JUDGEMENT:                 {level: 4,  spell_id: 20271}
  DEVOTION_AURA:             {level: 8,  spell_id: 465}
  WORD_OF_GLORY:             {level: 9,  spell_id: 85673}
  REDEMPTION:                {level: 12, spell_id: 7328}
  DIVINE_PROTECTION:         {level: 12, spell_id: 498}
  HAND_OF_RECKONING:         {level: 14, spell_id: 62124}
  LAY_ON_HANDS:              {level: 16, spell_id: 633}
  PURIFY:                    {level: 18, spell_id: 1152}
  DIVINE_SHIELD:             {level: 18, spell_id: 642}
  BLESSING_OF_KINGS:         {level: 20, spell_id: 20217}
  HAND_OF_FREEDOM:           {level: 22, spell_id: 1044}
  HAND_OF_SALVATION:         {level: 22, spell_id: 1038}
  CONSECRATION:              {level: 24, spell_id: 26573}
  SEAL_OF_INSIGHT:           {level: 26, spell_id: 20165}
  HAND_OF_PROTECTION:        {level: 28, spell_id: 1022}
  RETRIBUTION_AURA:          {level: 30, spell_id: 7294}
  AVENGING_WRATH:            {level: 30, spell_id: 31884, requires_spec: ret}
  TEMPLARS_VERDICT:          {level: 30, spell_id: 85256, requires_spec: ret}
  INQUISITION:               {level: 30, spell_id: 84963, requires_spec: ret}
  RESISTANCE_AURA:           {level: 30, spell_id: 19891}
  SEAL_OF_TRUTH:             {level: 30, spell_id: 31801, requires_spec: ret}
  DIVINE_STORM:              {level: 30, spell_id: 53385, requires_spec: ret, requires_talent: true}
  ZEALOTRY:                  {level: 30, spell_id: 85696, requires_spec: ret, requires_talent: true}
  EXORCISM:                  {level: 32, spell_id: 879}
  HAMMER_OF_WRATH:           {level: 36, spell_id: 24275}
  BLESSING_OF_MIGHT:         {level: 40, spell_id: 19740}
  CONCENTRATION_AURA:        {level: 50, spell_id: 19746}
  HOLY_WRATH:                {level: 50, spell_id: 2812}
  REBUKE:                    {level: 54, spell_id: 96231}
  CRUSADER_AURA:             {level: 62, spell_id: 32223}
  GUARDIAN_OF_ANCIENT_KINGS_RET: {level: 85, spell_id: 86698, requires_spec: ret}
  REPENTANCE:                {requires_talent: true, spell_id: 20066}
  # Talent-gated:
  THE_ART_OF_WAR:            {requires_talent: true}
  SANCTIFIED_WRATH:          {requires_talent: true}
  TWO_HANDED_SPECIALIZATION: {requires_talent: true}
  RULE_OF_LAW:               {requires_talent: true}
  CRUSADE:                   {requires_talent: true}
  PARAGON_OF_VIRTUE:         {requires_talent: true}
  IMPROVED_JUDGEMENT:        {requires_talent: true}
  PURSUIT_OF_JUSTICE:        {requires_talent: true}
  TOUGHNESS:                 {requires_talent: true}
  COMMUNION:                 {requires_talent: true}
  DIVINITY:                  {requires_talent: true}
  # Mastery / passives:
  MASTERY_HAND_OF_LIGHT:     {passive_mastery: true, base_pct: 16.8, per_point_pct: 2.1}
  PLATE_SPECIALIZATION:      {passive: true, str_pct: 5}
  HOLY_POWER:                {passive_resource: true, max: 3}
```

### Leveling brackets

```yaml
bracket_1_3:
  available: CRUSADER_STRIKE, HAMMER_OF_JUSTICE
  rotation: CS spam

bracket_4_8:
  available: + SEAL_OF_RIGHTEOUSNESS (3), + JUDGEMENT (4), + DEVOTION_AURA (8)
  rotation: SoR + CS + Judgement + auto-attack

bracket_9_29:
  available: + WORD_OF_GLORY (9), + DIVINE_PROTECTION (12), + LAY_ON_HANDS (16), + DIVINE_SHIELD (18), + BoK (20), + CONSECRATION (24), + SEAL_OF_INSIGHT (26)
  rotation: SoR + CS + Judgement; WoG defensive; Consecration AoE

bracket_30_31:
  available: + RETRIBUTION_AURA, + AW, + TV, + INQUISITION, + SEAL_OF_TRUTH, + DIVINE_STORM (talent), + ZEALOTRY (talent capstone)
  rotation_change: full Ret rotation activates

bracket_32_35:
  available: + EXORCISM (32)
  rotation_addition: Exorcism on AoW proc

bracket_36_39:
  available: + HAMMER_OF_WRATH (36)
  rotation_addition: HoW execute / AW window

bracket_40_84:
  available: + BoM (40), + Concentration Aura (50), + HOLY_WRATH (50), + REBUKE (54), + CRUSADER_AURA (62)
  notes: full single-target rotation; HoW becomes execute filler.

bracket_85:
  available: + GUARDIAN_OF_ANCIENT_KINGS_RET (85)
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
| 1 | Holy Power generation on Communion | UNVERIFIED HP-on-Judgement | Talent.dbc |
| 2 | Templar's Verdict damage formula per HP-stack | TBD | Spell.dbc 85256 |
| 3 | Inquisition duration formula | 4s × HP | Spell.dbc 84963 |
| 4 | Sanctified Wrath AW-CD reduction | 50% | Talent.dbc |
| 5 | Hammer of Wrath base damage scaling | TBD | Spell.dbc 24275 |
| 6 | Censure tick rate + 5-stack damage | TBD | Spell.dbc 31803 |
| 7 | Seal of Truth on-hit damage formula | TBD | Spell.dbc 31801 |
| 8 | Mastery Hand of Light formula | 16.8% + 2.1%/pt | Mastery DBC |
| 9 | Two-Handed Specialization | +20% 2H | Talent.dbc |
| 10 | Plate Specialization | +5% Str | Class passive |
| 11 | Art of War proc rate + 15s window | TBD | Talent.dbc |
| 12 | Zealotry HP-generation effect | UNVERIFIED set vs grant | Spell.dbc 85696 |
| 13 | Guardian of Ancient Kings Ret stacks | TBD | Spell.dbc 86698 |
| 14 | Avenging Wrath +20% scope | all damage? | Spell.dbc 31884 |
| 15 | Divine Storm radius / target cap | 8y / TBD | Spell.dbc 53385 |
| 16 | Exorcism cast time hardcast | 1.5s vs 2.0s | Spell.dbc 879 |
| 17 | Holy Wrath AoE / target cap | TBD | Spell.dbc 2812 |
| 18 | Consecration mana / radius / duration | TBD | Spell.dbc 26573 |
| 19 | T13 2P/4P (Ret) | TBD | Item set DBC |
| 20 | JotP haste % + duration | 9% / 60s | Talent.dbc |
| 21 | Glyph of TV (+15%) | TBD | Glyph.dbc |
| 22 | Glyph of CS (+5% crit) | TBD | Glyph.dbc |
| 23 | Glyph of Exorcism | TBD | Glyph.dbc |
| 24 | Hand of Reckoning taunt-status | TBD | Spell.dbc 62124 |
| 25 | Repentance break-on-damage + PvE duration | TBD | Spell.dbc 20066 |
| 26 | Forbearance shared CD list | LoH/DS/HoP | Spell.dbc |
| 27 | Golemblood Potion duration | 25s | Item DBC |
| 28 | Word of Glory + Eternal Glory glyph proc rate | TBD | Glyph.dbc |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kRetPaladin_Spells[]` |
| Rotation tiers | `static const RotationTier kRetPaladin_Rotation[]` |
| AoE rotation | `static const RotationTier kRetPaladin_RotationAoE[]` |
| Cooldown Profile | `static const CooldownTrigger kRetPaladin_Cooldowns[]` |
| Mana + Holy Power | dual-resource state |
| Inquisition tracking | aura listener + remaining ms |
| Art of War proc | aura listener |
| Censure stacks | 0-5 tracker |
| Forbearance state | shared CD on LoH/DS/HoP |
| Seal mode | Truth / Righteousness / Insight |
| Leveling rotation | `LevelBracketRotation kRetPaladin_Leveling[]` |

**Key new helper APIs:**
- `HolyPower(bot)` — read 0-3 (shared with Holy Paladin).
- `InquisitionRemainingMs(bot)`.
- `ArtOfWarActive(bot)`.
- `CensureStacks(target)` — read 0-5.
- `ForbearanceActive(bot)`.
- `IsHammerOfWrathUsable(target)` — `target.hp <= 20% OR aura.AW OR aura.Zealotry`.
- `IsAoEMode(bot)` — `enemies_within_8y_self >= 4`.

Cross-reference to `docs/specs/holy-paladin.md` for shared paladin mechanics, `docs/roles/melee-dps.md` for melee patterns.
