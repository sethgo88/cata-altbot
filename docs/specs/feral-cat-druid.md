# Feral Cat Druid — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide.

**Strategy content** reconciled across 8+ community sources. See `docs/research/feral-cat-druid-guide-survey.md`. Cross-reference: shared Druid mechanics (Mark of the Wild, Innervate, Rebirth, Tranquility, Faerie Fire, Hibernate, Soothe, Cyclone, Healing Touch, mana model) are documented in `docs/specs/balance-druid.md` and `docs/specs/resto-druid.md`.

**SPEC-CHOICE RATIONALE.** Druid Feral talent tree is shared between Cat (DPS) and Bear (tank) at 4.3.4 — the split into separate Feral / Guardian specs comes in Mists 5.0. This doc covers **Feral Cat (DPS)** only. Bear/Guardian tank is **DEFERRED** to the tank phase per role-ordering rule (`feedback_role_ordering.md`). Balance is ranged caster (already documented); Restoration is healer (already documented). This is the **sixth and final melee-DPS spec** in the project — with this doc complete, all 6 Cata 4.3.4 melee DPS specs (Rogue Combat, Warrior Fury, Paladin Ret, DK Frost, Shaman Enh, Druid Feral-Cat) are covered.

---

## Identity

```yaml
spec: feral_cat_druid
class: druid
role: MELEE_DPS
resource_model: energy (0-100, +10/sec base) + combo_points (0-5 on target) + mana (out-of-cat-form only)
defining_mechanic: Cat Form melee Energy/CP rotation (Rogue-like) + bleed maintenance (Rake DoT + Rip finisher) + Savage Roar self-buff +30% damage 100% uptime + Tiger's Fury 30s CD damage window + Berserk 3min CD energy/cost CD
defining_buffs:
  - Cat_Form                      # mandatory DPS form
  - Savage_Roar                   # +30% melee damage; 12-30s based on CP spent; mandatory uptime
  - Tigers_Fury                   # +15% damage 6s + 60 Energy restore; on-CD usage
  - Berserk_Cat                   # 3min CD; +100% Energy regen, -50% energy cost, no Mangle CD; 15s
  - Mark_of_the_Wild              # raid stat buff
  - Mangle_Bleed_Debuff           # +30% bleed damage on target (60s after first Mangle)
  - Rake_Bleed_DoT                # primary CP-builder bleed
  - Rip_Bleed_DoT                 # 5-CP finisher bleed (16s base, extendable via Glyph of Bloodletting)
  - Predatory_Swiftness           # talent proc: builders → instant Healing Touch / Rebirth / Roots / Cyclone
defining_procs:
  - Predatory_Swiftness           # talent proc on builder ability
  - Primal_Fury                   # crit-on-builder → bonus CP
  - Stampede                      # talent: Ravage usable post-Tiger's-Fury (no stealth req)
mastery: Razor_Claws              # +% bleed damage scaling
key_cooldowns:
  - Tigers_Fury                   # 30s CD; Energy + damage burst
  - Berserk_Cat                   # 3min CD; sustained-damage window
  - Skull_Bash_Cat                # 1min CD; charge-interrupt + gap-closer (Cata-new)
  - Stampeding_Roar               # 3min CD; raid speed buff (Cata-new, L83)
  - Innervate                     # 3min CD; mana restore (party/self utility)
  - Tranquility                   # 8min CD; emergency raid heal
  - Rebirth                       # 30min CD; combat res
  - Barkskin                      # 1min CD; -20% damage taken 12s
  - Survival_Instincts            # 3min CD; -50% damage 12s (Bear/Cat)
  - Frenzied_Regeneration         # bear-only; stance-dance for emergency self-heal (rare)
  - Maim                          # 5-CP stun finisher
melee_range: 5
notes: |
  Feral Cat in 4.3.4 plays as a leather-Agility melee in Cat Form running
  a Rogue-like Energy + Combo Points engine layered onto bleed-DoT
  maintenance. Heartbeat: Cat Form always-on (drop only for Predatory-
  Swiftness Healing Touch / Rebirth / Tranquility utility); Mangle (or
  Shred when behind target) is primary CP-builder; Rake DoT maintained
  with pandemic refresh; Rip 5-CP finisher applied/refreshed on target;
  Savage Roar self-buff 100% uptime via 1-CP refresh emergency or 5-CP
  refresh standard; Tiger's Fury on CD (30s); Berserk on CD (3min)
  ideally inside Tiger's Fury window.

  Burst window: Tiger's Fury + Berserk + Tolvir Potion + trinket on-use
  stacked at first pull; Tiger's Fury drifts on 30s CD; Berserk re-stacks
  every 3 min ideally inside next Tiger's Fury window.

  Pre-Mangle (under L30): Claw + Rake + Rip-when-talented; pre-Berserk
  (talent ~L36): basic Cat rotation; pre-Skull-Bash (L60): no charge-
  interrupt — Bash via stance-dance to Bear is a fallback (rarely used
  in PvE).

  Stance-dance is largely dead in 4.3.4 Cat-DPS — only stance-swap is
  Bear→Bash interrupt-or-stun OR Bear→Frenzied-Regeneration emergency
  self-heal. Skull Bash (Cata-new, L60) replaces both as a Cat-form
  charge interrupt.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  CURRENT_DPS_TARGET:
  ENEMY_BEHIND_TARGET:            # Shred positional check (must be behind target)
  ENEMY_CASTING_INTERRUPTIBLE:    # Skull Bash range: 15-25y (Cata charge-interrupt)
  AOE_CLUSTER:                    # ≥3 enemies within 8y (Swipe AoE)
  CC_MARKER_HIBERNATE:            # animal/dragonkin CC
  CC_MARKER_CYCLONE:              # short CC any
  CC_MARKER_MAIM:                 # 5-CP stun finisher
  ALLY_INNERVATE_TARGET:          # mana-restore party utility
  DEAD_ALLY_REBIRTH:              # combat res
```

---

## Spell Catalog

```yaml
spells:
  # ─────── Form ───────

  CAT_FORM:
    id: 768
    mana_pct: 5                       # cost only out of form
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    notes: |
      Mandatory DPS form. All Cat abilities require Cat Form active.
      Cancel cost-free; recast to re-enter. Drop only for Predatory-
      Swiftness Healing Touch / Rebirth / Tranquility / Innervate.

  BEAR_FORM:
    id: 5487
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    notes: |
      NOT used in Cat-DPS rotation. Stance-dance to Bear only for
      Frenzied Regeneration emergency self-heal (rare; usually Barkskin
      + Survival Instincts cover).

  # ─────── CP builders ───────

  MANGLE_CAT:
    id: 33876
    energy_cost: 35                   # base; Berserk -50%
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    cp_gain: 1
    notes: |
      Primary omnidirectional CP builder. Applies Mangle bleed-debuff
      (+30% bleed damage on target, 60s; refresh on each Mangle cast).
      Front-of-target use; Shred replaces when behind target.

  SHRED:
    id: 5221
    energy_cost: 40                   # base; Berserk -50%
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee_behind
    cp_gain: 1
    requires_position: behind
    notes: |
      Higher-damage CP builder. REQUIRES behind-target position. With
      Glyph of Bloodletting: each Shred extends Rip duration +2s up to
      6s cap. Mandatory glyph for Cat ST.

  RAKE:
    id: 1822
    energy_cost: 35                   # base
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    cp_gain: 1
    duration_ms: 9000                 # base 9s; talented + Endless Carnage to 15s+
    notes: |
      Bleed DoT CP builder. Refresh ≤ 3s remaining (pandemic). Cast inside
      Tiger's Fury / Berserk window for damage snapshot via window-buff
      (Cata bleeds inherit current stats on tick, not snapshot — but
      Tiger's Fury / Berserk +damage % buffs the cast itself + ticks
      while active).

  RAVAGE:
    id: 6785
    energy_cost: 60                   # base; Berserk -50%
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    cp_gain: 1
    requires_stealth: true            # OR aura.active(SELF, STAMPEDE_PROC)
    notes: |
      Stealth opener (or Stampede-proc post-Tiger's-Fury per talent).
      Massive damage. Bot opener: Pounce → Ravage OR Stampede-Ravage
      after Tiger's Fury cast.

  POUNCE:
    id: 9005
    energy_cost: 50
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    cp_gain: 1
    requires_stealth: true
    notes: Stealth opener stun (3s) + CP. Used in PvE for opening burst alignment.

  # ─────── CP finishers ───────

  RIP:
    id: 1079
    energy_cost: 30
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    cp_required: 5                    # max-power finisher; rarely cast at <5 CP
    duration_ms: 16000                # base; extends per CP (UNVERIFIED scaling)
    notes: |
      Primary 5-CP finisher. Bleed DoT. Refresh ONLY at 5 CP AND
      (current Rip < pandemic-window 5s OR (Tiger's Fury / Berserk
      active AND no better window coming)). Glyph of Bloodletting:
      Shred extends Rip up to 6s cap.

  FEROCIOUS_BITE:
    id: 22568
    energy_cost: 25                   # base; uses up to 35 extra Energy for damage
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 5
    target_type: enemy_melee
    cp_required: 1                    # min; consumes all CP
    notes: |
      Alternate finisher. Below 25% target HP: Glyph of Ferocious Bite
      extends Rip +6s (UNVERIFIED). Sub-25% execute window. NOT used
      above 25% — Rip is always preferred ST finisher.

  SAVAGE_ROAR:
    id: 52610
    energy_cost: 25
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    cp_required: 1                    # min; consumes all CP
    duration_ms: 12000                # 1 CP base; +6s per additional CP (5 CP = 42s)
    notes: |
      Self-buff +30% melee damage. Mandatory 100% uptime. Refresh at
      1 CP if Roar < 5s remaining + no time for 5-CP build; otherwise
      refresh at 5 CP for max duration. Endless Carnage talent extends.

  MAIM:
    id: 22570
    energy_cost: 35
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: true
    range: 5
    target_type: enemy_melee
    cp_required: 5                    # 1 min; scales by CP
    notes: |
      CP-spending stun finisher. CC use (caster adds, mob-control).
      Replaces Rip for utility-CC; not standard rotation.

  # ─────── Cooldowns ───────

  TIGERS_FURY:
    id: 5217
    energy_cost: 0
    energy_gain: 60
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: false                     # off-GCD
    range: 0
    target_type: self
    duration_ms: 6000
    notes: |
      30s CD. Off-GCD. +60 Energy AND +15% damage 6s. On CD always —
      cast when Energy < 40 to avoid overcap. Stack inside Berserk
      window for stacked damage burst.

  BERSERK_CAT:
    id: 50334
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 180000
    on_gcd: false                     # off-GCD; UNVERIFIED
    range: 0
    target_type: self
    duration_ms: 15000
    requires_talent: true
    notes: |
      3min CD. +100% Energy regen, -50% Energy cost, no Mangle CD,
      15s. Stack inside Tiger's Fury window for compressed burst.
      King of the Jungle talent: + damage during Berserk.

  STAMPEDING_ROAR:
    id: 77764
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 180000
    on_gcd: true
    range: 0
    target_type: party_buff
    duration_ms: 8000
    requires_level: 83
    notes: |
      Cata-new L83. Raid +60% movement speed 8s in 10y radius. Movement-
      heavy phases (Spine of Deathwing roll, Yor'sahj black slime).

  SKULL_BASH_CAT:
    id: 80965
    energy_cost: 25
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: true
    range: 25                         # 13-25y — charge range
    target_type: enemy_casting
    duration_ms: 4000                 # 4s school lockout
    requires_level: 60
    notes: |
      Cata-new L60. Charge-interrupt: closes gap to target + interrupts
      cast + 4s school lockout. Combined gap-closer + interrupt — Cat's
      only mid-range tool.

  BARKSKIN:
    id: 22812
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: false                     # off-GCD
    range: 0
    target_type: self
    duration_ms: 12000
    notes: -20% damage taken 12s. Cat-form usable. Defensive; reactive use.

  SURVIVAL_INSTINCTS:
    id: 61336
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 180000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 12000
    notes: -50% damage taken 12s. Cat/Bear usable. Major defensive CD.

  # ─────── Utility / shared Druid ───────

  FAERIE_FIRE_FERAL:
    id: 16857
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 6000
    on_gcd: true
    range: 35
    target_type: enemy
    duration_ms: 300000
    notes: |
      Armor debuff -12% (UNVERIFIED Cata value); range pull; dispels
      stealth. Furor talent: Energy gain on cast. Maintenance
      buff/debuff; recast on expire OR for stealth-dispel.

  MARK_OF_THE_WILD:
    id: 1126
    mana_pct: 19                      # mana cost from caster form
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: party_buff
    duration_ms: 3600000
    notes: Pre-pull party buff. Cast in caster form.

  REBIRTH:
    id: 20484
    mana_pct: 35
    cast_time_ms: 2000
    cooldown_ms: 1800000              # 30min
    on_gcd: true
    range: 30
    target_type: dead_ally
    notes: |
      30min combat res. Cast in caster form OR via Predatory-Swiftness
      proc instant cat form. Prioritize healers / important DPS.

  HEALING_TOUCH:
    id: 5185
    mana_pct: 25
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    notes: |
      Predatory-Swiftness proc instant cast. Self-heal at HP < 30%
      OR ally-save use. Drop Cat Form OR use Predatory-Swiftness instant.

  INNERVATE:
    id: 29166
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 180000
    on_gcd: true
    range: 40
    target_type: ally
    duration_ms: 10000
    notes: |
      3min CD party mana restore. Bot use: cast on healer at low mana.
      Self-cast disabled in Cat Form — must drop to caster form first.

  TRANQUILITY:
    id: 740
    mana_pct: 32
    cast_time_ms: 0                   # channeled 8s
    cooldown_ms: 480000               # 8min
    on_gcd: true
    range: 0
    target_type: party_aoe
    duration_ms: 8000
    notes: |
      Channeled raid heal 8min CD. Drop Cat Form to cast (not
      Predatory-Swiftness compatible). Emergency-only.

  SOOTHE:
    id: 2908
    mana_pct: 8
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: true
    range: 40
    target_type: enemy_buffed
    notes: |
      Enrage dispel. Cast in any form (Cata change). Encounter-driven
      (e.g., enraged trash, Magmaw enrage).

  HIBERNATE:
    id: 2637
    mana_pct: 8
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: enemy_animal_or_dragonkin
    duration_ms: 40000
    notes: CC against beasts/dragons. Manual / encounter use.

  CYCLONE:
    id: 33786
    mana_pct: 17
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 20
    target_type: enemy
    duration_ms: 6000
    notes: 6s CC any target. Cast in caster form OR via Predatory-Swiftness.

  COWER:
    id: 1742
    energy_cost: 30
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: true
    range: 0
    target_type: self
    notes: -threat 5s. Cat-form threat reduction. Manual use.

  PROWL:
    id: 5215
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 6000
    on_gcd: false
    range: 0
    target_type: self
    notes: |
      Cat Form stealth. Out-of-combat only. Pre-pull stealth → Pounce
      / Ravage opener.

  TRAVEL_FORM:
    id: 783
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    notes: Out-of-combat travel speed (+40%); skip if mounted.

  REMOVE_CORRUPTION:
    id: 2782
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 8000
    on_gcd: true
    range: 40
    target_type: ally
    notes: Curse / poison dispel. Cat must drop form (Cata change UNVERIFIED).

  FRENZIED_REGENERATION:
    id: 22842
    energy_cost: 0
    cast_time_ms: 0
    cooldown_ms: 180000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 20000
    notes: |
      BEAR-FORM ONLY. Stance-dance emergency self-heal: rage→HP
      conversion. Cat → Bear → Frenzied → Cat. Rare; mainly Survival
      Instincts + Barkskin cover.
```

---

## Maintenance pass

```yaml
maintenance:
  - id: CAT_FORM
    target: SELF
    aura: CAT_FORM_BUFF
    refresh_when:
      - aura.missing(SELF, CAT_FORM_BUFF)
      - NOT predatory_swiftness_consume_pending
    cast: CAT_FORM
    notes: Mandatory form; recast on accidental drop.

  - id: SAVAGE_ROAR
    target: SELF
    aura: SAVAGE_ROAR_BUFF
    refresh_when:
      - aura.missing(SELF, SAVAGE_ROAR_BUFF)
      - OR aura.expires_in_ms(SELF, SAVAGE_ROAR_BUFF) < 5000
      - combo_points >= 1
    cast: SAVAGE_ROAR on SELF
    notes: |
      Mandatory uptime. Refresh at 1 CP if Roar < 5s remaining (emergency);
      otherwise wait for 5 CP for max duration (42s).

  - id: RAKE
    target: CURRENT_DPS_TARGET
    aura: RAKE_BLEED
    refresh_when:
      - aura.missing(CURRENT_DPS_TARGET, RAKE_BLEED)
      - OR aura.expires_in_ms(CURRENT_DPS_TARGET, RAKE_BLEED) < 3000
      - energy_pct >= 35
    cast: RAKE on CURRENT_DPS_TARGET
    notes: Pandemic refresh; CP gain on cast.

  - id: RIP
    target: CURRENT_DPS_TARGET
    aura: RIP_BLEED
    refresh_when:
      - aura.missing(CURRENT_DPS_TARGET, RIP_BLEED)
      - OR (aura.expires_in_ms(CURRENT_DPS_TARGET, RIP_BLEED) < 5000 AND combo_points == 5)
      - OR (aura.active(SELF, [TIGERS_FURY_BUFF, BERSERK_BUFF]) AND aura.expires_in_ms(CURRENT_DPS_TARGET, RIP_BLEED) < 9000 AND combo_points == 5)
      - energy_pct >= 30
    cast: RIP on CURRENT_DPS_TARGET
    notes: 5-CP only. Pandemic refresh OR window-favored refresh.

  - id: MANGLE_BLEED_DEBUFF
    target: CURRENT_DPS_TARGET
    aura: MANGLE_BLEED_DEBUFF_60S
    refresh_when:
      - aura.missing(CURRENT_DPS_TARGET, MANGLE_BLEED_DEBUFF_60S)
      - OR aura.expires_in_ms(CURRENT_DPS_TARGET, MANGLE_BLEED_DEBUFF_60S) < 10000
    cast: MANGLE_CAT on CURRENT_DPS_TARGET
    notes: |
      Mangle Cat applies +30% bleed dmg debuff 60s. Mangle on CD already
      for CP gen, so this maintenance is usually satisfied automatically.
      Edge case: behind-target Shred-only — manually weave Mangle if
      debuff falls off.

  - id: FAERIE_FIRE_FERAL
    target: CURRENT_DPS_TARGET
    aura: FAERIE_FIRE_DEBUFF
    refresh_when:
      - aura.missing(CURRENT_DPS_TARGET, FAERIE_FIRE_DEBUFF)
    cast: FAERIE_FIRE_FERAL on CURRENT_DPS_TARGET
    notes: -12% armor (UNVERIFIED Cata value); maintain raid debuff.
```

---

## Rotation — Priority List (Single-Target)

```yaml
rotation:
  # ───────── Tier 1: Tiger's Fury overcap-prevention ─────────
  - tier: 1
    name: Tiger's Fury — energy < 40
    when:
      - energy_pct < 40
      - spell.off_cooldown(TIGERS_FURY)
    cast: TIGERS_FURY on SELF
    notes: |
      Off-GCD; restores 60 Energy + 15% damage 6s. Cast when Energy
      low to maximize 60E gain. Ideally pair with Berserk window.

  # ───────── Tier 2: Savage Roar emergency refresh (1 CP, < 5s) ─────────
  - tier: 2
    name: Savage Roar — emergency 1 CP refresh
    when:
      - aura.expires_in_ms(SELF, SAVAGE_ROAR_BUFF) < 5000
      - combo_points >= 1
      - energy_pct >= 25
    cast: SAVAGE_ROAR on SELF
    notes: Roar < 5s; preserve uptime even at low CP.

  # ───────── Tier 3: Rip refresh (5 CP, pandemic window or burst window) ─────────
  - tier: 3
    name: Rip — refresh at 5 CP
    when:
      - combo_points == 5
      - any_of:
          - aura.missing(CURRENT_DPS_TARGET, RIP_BLEED)
          - aura.expires_in_ms(CURRENT_DPS_TARGET, RIP_BLEED) < 5000
          - aura.active(SELF, TIGERS_FURY_BUFF) AND aura.expires_in_ms(CURRENT_DPS_TARGET, RIP_BLEED) < 9000
          - aura.active(SELF, BERSERK_BUFF) AND aura.expires_in_ms(CURRENT_DPS_TARGET, RIP_BLEED) < 9000
      - energy_pct >= 30
      - target_in_range(5)
      - target.hp_pct > 25 OR Glyph_of_Ferocious_Bite_NOT_active
    cast: RIP on CURRENT_DPS_TARGET

  # ───────── Tier 4: Savage Roar standard refresh (5 CP, < 12s) ─────────
  - tier: 4
    name: Savage Roar — 5 CP refresh (standard)
    when:
      - combo_points == 5
      - aura.expires_in_ms(SELF, SAVAGE_ROAR_BUFF) < 12000
      - aura.active(CURRENT_DPS_TARGET, RIP_BLEED) AND aura.expires_in_ms(CURRENT_DPS_TARGET, RIP_BLEED) > 5000
      - energy_pct >= 25
    cast: SAVAGE_ROAR on SELF
    notes: 5 CP refresh for 42s duration; Rip already healthy.

  # ───────── Tier 5: Rake refresh / apply ─────────
  - tier: 5
    name: Rake — refresh / apply (pandemic)
    when:
      - any_of:
          - aura.missing(CURRENT_DPS_TARGET, RAKE_BLEED)
          - aura.expires_in_ms(CURRENT_DPS_TARGET, RAKE_BLEED) < 3000
      - energy_pct >= 35
      - target_in_range(5)
    cast: RAKE on CURRENT_DPS_TARGET

  # ───────── Tier 6: Ferocious Bite — execute window ─────────
  - tier: 6
    name: Ferocious Bite — sub-25% execute
    when:
      - target.hp_pct <= 25
      - combo_points == 5
      - aura.active(CURRENT_DPS_TARGET, RIP_BLEED) AND aura.expires_in_ms(CURRENT_DPS_TARGET, RIP_BLEED) >= 5000
      - aura.expires_in_ms(SELF, SAVAGE_ROAR_BUFF) >= 8000
      - energy_pct >= 25
      - target_in_range(5)
    cast: FEROCIOUS_BITE on CURRENT_DPS_TARGET
    notes: Glyph of Ferocious Bite extends Rip when sub-25%; check glyph state.

  # ───────── Tier 7: Shred — primary CP builder (behind target) ─────────
  - tier: 7
    name: Shred — behind-target builder
    when:
      - position.behind_target
      - combo_points < 5
      - energy_pct >= 40
      - target_in_range(5)
    cast: SHRED on CURRENT_DPS_TARGET
    notes: Mandatory glyph: Bloodletting (Shred extends Rip).

  # ───────── Tier 8: Mangle — front-of-target builder ─────────
  - tier: 8
    name: Mangle — non-behind-target builder
    when:
      - NOT position.behind_target
      - combo_points < 5
      - energy_pct >= 35
      - target_in_range(5)
    cast: MANGLE_CAT on CURRENT_DPS_TARGET
    notes: Mangle CD removed in Berserk window.

  # ───────── Tier 9: Energy-pool wait ─────────
  - tier: 9
    name: Pool energy — wait for next cast slot
    when:
      - combo_points < 5
      - energy_pct < 40
      - NOT spell.off_cooldown(TIGERS_FURY)
      - NOT spell.off_cooldown(BERSERK_CAT)
    cast: WAIT
    notes: Auto-attack only; no GCD-burning while pooling.
```

---

## AoE Mode

Activated by `enemies_within_8y_of_self >= 3`. Hysteresis: exit `<2 sustained for 5s`.

```yaml
rotation_aoe:
  - tier: 1
    name: Savage Roar maintenance (1 CP if < 5s)
    when:
      - aura.expires_in_ms(SELF, SAVAGE_ROAR_BUFF) < 5000
      - combo_points >= 1
    cast: SAVAGE_ROAR on SELF

  - tier: 2
    name: Tiger's Fury energy-overcap-prevention
    when:
      - energy_pct < 40
      - spell.off_cooldown(TIGERS_FURY)
    cast: TIGERS_FURY

  - tier: 3
    name: Mangle on primary AoE target (apply Mangle bleed-debuff)
    when:
      - aura.missing(PRIMARY_AOE_TARGET, MANGLE_BLEED_DEBUFF_60S)
      - energy_pct >= 35
    cast: MANGLE_CAT on PRIMARY_AOE_TARGET

  - tier: 4
    name: Swipe (Cat) — AoE builder
    when:
      - enemies_within_8y_of_self >= 3
      - energy_pct >= 50
    cast: SWIPE_CAT
    notes: |
      Cat-form Swipe (id 62078). Hits all enemies within 8y. Base CP gen
      = 1 per cast. Mana-equivalent cost: 50 Energy.

  - tier: 5
    name: Thrash (Bear-form alternative — UNVERIFIED Cat-form availability)
    when: false                       # placeholder; Thrash is Bear-only in 4.3.4
    cast: THRASH
    notes: NOT used in Cat AoE. Reference for tank phase.

  - tier: 6
    name: Rake on primary
    when:
      - aura.missing(PRIMARY_AOE_TARGET, RAKE_BLEED)
      - energy_pct >= 35
    cast: RAKE on PRIMARY_AOE_TARGET

  - tier: 7
    name: Rip — 5 CP finisher on primary
    when:
      - combo_points == 5
      - aura.expires_in_ms(PRIMARY_AOE_TARGET, RIP_BLEED) < 5000
      - energy_pct >= 30
    cast: RIP on PRIMARY_AOE_TARGET

notes: |
  Cat AoE is weaker than dedicated AoE specs (Frost DK, Combat Rogue Blade
  Flurry). Swipe (Cat) is only AoE-specific tool. Below 5+ targets, ST
  rotation on primary often outperforms Swipe spam.
```

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: TIGERS_FURY
    when:
      - in_combat
      - energy_pct < 40
      - spell.off_cooldown(TIGERS_FURY)
      - NOT aura.active(SELF, BERSERK_BUFF) OR cooldown_window_aligned (Berserk)
    rationale: 30s CD; on-CD usage; align with Berserk for stacked window.

  - spell: BERSERK_CAT
    when:
      - in_combat
      - spell.off_cooldown(BERSERK_CAT)
      - any_of:
          - boss_engagement
          - cooldown_window_aligned (Tiger's Fury just-cast OR off-CD)
    rationale: 3min CD; align first w/ Tiger's Fury for compressed burst.

  - spell: BARKSKIN
    when:
      - SELF.taking_burst_damage
      - SELF.hp_pct < 70
      - spell.off_cooldown(BARKSKIN)
    rationale: 1min CD; -20% damage 12s. Reactive defensive.

  - spell: SURVIVAL_INSTINCTS
    when:
      - SELF.hp_pct < 40
      - no_other_save_ready
      - spell.off_cooldown(SURVIVAL_INSTINCTS)
    rationale: 3min CD; -50% damage 12s. Major defensive.

  - spell: SKULL_BASH_CAT
    when:
      - target.casting AND target.cast_remaining_ms > 300
      - target.cast_interruptible
      - target_in_range(25)
      - spell.off_cooldown(SKULL_BASH_CAT)
      - level >= 60
    rationale: 1min CD charge-interrupt; gap-closer + lockout.

  - spell: STAMPEDING_ROAR
    when:
      - manual_command OR raid_movement_phase
      - spell.off_cooldown(STAMPEDING_ROAR)
      - level >= 83
    rationale: 3min raid speed buff; encounter-driven.

  - spell: INNERVATE
    when:
      - ally_healer.mana_pct < 30
      - spell.off_cooldown(INNERVATE)
    rationale: 3min party mana CD. Self-cast requires drop-Cat-Form.

  - spell: REBIRTH
    when:
      - ally_dead AND in_combat AND ally.role IN [healer, key_dps]
      - spell.off_cooldown(REBIRTH)
    rationale: 30min combat res. Predatory-Swiftness instant or drop-form.

  - spell: TRANQUILITY
    when:
      - raid.avg_hp_pct < 40 AND in_combat
      - spell.off_cooldown(TRANQUILITY)
    rationale: 8min emergency raid heal. Drop Cat Form.

  - spell: COWER
    when:
      - SELF.threat_pct > 90 AND tank_alive
      - spell.off_cooldown(COWER)
    rationale: Threat reduction; rare in PvE w/ Salvation.

  - spell: MAIM
    when:
      - manual_command OR cc_marker_tagged
      - combo_points == 5
      - target_in_range(5)
      - spell.off_cooldown(MAIM)
    rationale: 5-CP stun; CC use.
```

---

## Proc Reactions

```yaml
procs:
  PREDATORY_SWIFTNESS:
    talent_id: 16972
    triggered_by: builder ability cast (Mangle / Shred / Rake / Ravage / Pounce)
    proc_chance: ~20% per builder (UNVERIFIED talent rank scaled)
    effect: next Healing Touch / Rebirth / Entangling Roots / Cyclone instant
    duration_ms: 8000
    ai_behavior: |
      Consume on:
      1. Healing Touch SELF if HP < 50% (priority)
      2. Healing Touch ally if ally.hp < 30%
      3. Rebirth if ally dead in combat
      4. Cyclone CC if marker tagged
      5. Hold otherwise — let proc expire (no DPS gain from forced consume)

  PRIMAL_FURY:
    talent_id: 16959
    triggered_by: melee crit on builder (Mangle / Shred / Rake / Ravage)
    proc_chance: 100% on crit (talent-rank scaled)
    effect: bonus +1 CP on crit
    ai_behavior: Passive — informs CP-overflow protection in rotation tier ordering.

  STAMPEDE:
    talent_id: 81021
    triggered_by: Tiger's Fury cast
    effect: next Ravage usable without stealth requirement
    duration_ms: 10000
    ai_behavior: |
      Consume Ravage immediately post-Tiger's-Fury cast for opener-tier
      damage. Bot opener: TF → Stampede-Ravage → Mangle/Shred priority.

  RAZOR_CLAWS_MASTERY:
    mastery_passive: true
    triggered_by: Bleed damage tick (Rake / Rip)
    effect: +% bleed damage scaling with Mastery rating
    base_pct: 25.0                    # UNVERIFIED Feral mastery base
    per_point_pct: 3.13               # UNVERIFIED per-point
    ai_behavior: Passive — informs reforge.

  KING_OF_THE_JUNGLE:
    talent_id: 48492
    triggered_by: Berserk-Cat cast OR Tiger's Fury cast (talent rank dependent)
    effect: +% damage during Berserk window
    duration_ms: same as Berserk (15s)
    ai_behavior: Passive — informs CD stacking decisions.
```

---

## Resource Management

```yaml
energy:
  pool_max: 100
  regen_per_sec: 10                   # base; +haste% scaling
  decay: none in combat; gradual decay out-of-combat
  generators:
    - Tigers_Fury: +60 Energy
    - Berserk_Cat: +100% regen rate
    - Furor_talent: + Energy on Faerie Fire (Feral) cast (UNVERIFIED amount)
  spenders:
    - Mangle_Cat: 35 Energy
    - Shred: 40 Energy
    - Rake: 35 Energy
    - Ravage: 60 Energy
    - Pounce: 50 Energy
    - Cower: 30 Energy
    - Swipe_Cat: 50 Energy
    - Maim: 35 Energy
    - Rip: 30 Energy
    - Ferocious_Bite: 25 Energy (+up to 35 extra for damage)
    - Savage_Roar: 25 Energy
  overcap_threshold: 80                # cast Tiger's Fury below this for max 60E gain

combo_points:
  pool_max: 5
  per_target: true                    # CP attached to target, not caster (Cata 4.0+)
  generators:
    - all_builders: 1 CP per cast
    - Primal_Fury_proc: +1 CP on builder crit
  consumers:
    - Rip: full consume (5 CP req)
    - Ferocious_Bite: min 1 CP, scales by CP
    - Savage_Roar: min 1 CP, duration scales by CP
    - Maim: min 1 CP, scales by CP
  decay: cleared on target swap (not transferable)

mana:
  pool_pct_max: 100
  regen: passive 5sr (out of cat form)
  in_cat_form_use: NEVER (forms convert melee; mana untouched in DPS rotation)
  decay: 0
  notes: |
    Mana only matters for Tranquility / Innervate / Rebirth / Healing Touch
    drop-form casts. Predatory-Swiftness instant Healing Touch still consumes
    mana but is fast enough to ignore in non-emergency.

action_eligibility:
  mangle_cat: energy_pct >= 35 AND target_in_range(5)
  shred: energy_pct >= 40 AND position.behind_target AND target_in_range(5)
  rake: energy_pct >= 35 AND target_in_range(5)
  rip: combo_points == 5 AND energy_pct >= 30 AND target_in_range(5)
  ferocious_bite: combo_points >= 1 AND energy_pct >= 25 AND target_in_range(5)
  savage_roar: combo_points >= 1 AND energy_pct >= 25
  tigers_fury: spell.off_cooldown(TIGERS_FURY)
  berserk_cat: spell.off_cooldown(BERSERK_CAT)
  swipe_cat: enemies_within_8y_of_self >= 3 AND energy_pct >= 50
```

---

## Consumables

```yaml
consumables:
  - item: TOLVIR_POTION                 # +Agility 25s
    when:
      - boss_engagement (pre-pot)
      - cooldown_window_aligned (Berserk + Tiger's Fury + trinket)
    notes: Pre-pot 1s before pull; combat-pot 1min mark.

  - item: FLASK_OF_THE_WINDS              # +Agility flask
    when:
      - in_combat AND aura.missing(SELF, FLASK_BUFF)
    notes: Always-on.

  - item: AGILE_SHADOWSPIRIT_DIAMOND      # +Agility meta-gem
    when: passive
    notes: Helm meta-gem.

  - item: WEIGHTED_CONSORTIUM_CRYSTAL     # +Agility weapon enchant
    when: passive
    notes: Replaces sharpening stones; weapon enchant.

  - item: FOOD_BUFF_SKEWERED_EEL          # +90 Agility
    when:
      - in_combat AND aura.missing(SELF, FOOD_BUFF)
    notes: Always-on.
```

---

## Non-encounter content

### Solo combat

```yaml
solo:
  - condition: open_world AND level < 85
    rotation: |
      Pre-buff Mark of the Wild + Cat Form. Engagement: stealth → Pounce
      / Ravage opener if available; otherwise Mangle. Maintain Rake +
      Rip + Savage Roar. Mangle (or Shred behind) builder; 5-CP Rip
      finisher. No DPS CDs (Tiger's Fury yes; Berserk held for elite).
  - condition: solo_elite_target
    rotation: standard rotation + Berserk on engagement; Survival Instincts on HP < 50%; Predatory-Swiftness Healing Touch self-heal.
  - condition: low_hp_self
    cast: |
      Predatory-Swiftness Healing Touch (HP < 50%) →
      Survival Instincts (HP < 40%) →
      Barkskin (HP < 70%) →
      drop Cat Form + hard-cast Healing Touch (last resort).
```

### Travel & out-of-combat

```yaml
travel:
  - condition: master_riding AND not_combat
    cast: standard mount (handled by AltbotMount); fallback Travel Form
  - condition: out_of_combat AND aura.missing(SELF, MARK_OF_THE_WILD)
    cast: drop_form → MARK_OF_THE_WILD on SELF → CAT_FORM on SELF
  - condition: stealth_pre_pull
    cast: PROWL on SELF
```

### Idle

```yaml
idle:
  - condition: out_of_combat AND no_active_target
    behavior: |
      Maintain Cat Form + MotW. Prowl if pre-pull stealth desired.
  - condition: out_of_combat AND HP < 50%
    cast: drop_form → HEALING_TOUCH on SELF → CAT_FORM on SELF
    notes: Predatory-Swiftness proc may persist from prior combat.
```

### Disengage

```yaml
disengage:
  - condition: master.fleeing AND in_combat
    behavior: |
      Drop combat → Travel Form / mount. Cat Form → Travel Form direct
      via Druid's Travel Form availability mid-combat (Cata change UNVERIFIED).
  - condition: bot.HP < 25% AND no_save_ready
    cast: |
      SURVIVAL_INSTINCTS → BARKSKIN → Predatory-Swiftness Healing Touch.
```

---

## Leveling rotation

Pure Feral Cat from L20 (Cat Form unlock).

### Spec recommendation

```yaml
spec_choice:
  for_leveling_dungeon_dps_bot:
    spec: Feral_Cat
    talent_split: 31 Feral / 7 Restoration / 3 Balance
    talent_picks_feral: |
      Predatory Strikes 3/3 (Cat AP scaling), Feral Aggression 3/3
      (Mangle/Maim dmg), King of the Jungle 3/3 (Berserk dmg),
      Stampede 2/2 (Ravage post-TF), Berserk 1/1, Primal Fury 2/2
      (crit gen CP), Endless Carnage 2/2 (Roar/Rake duration),
      Bloodletting glyph (NOT a talent — key glyph), Furor 3/3
      (Energy gen on Faerie Fire), Predatory Swiftness 3/3 (instant
      HT proc), Rend and Tear 3/3 (Maul/Ferocious Bite vs bleeding
      target), Pulverize 0/2 (BEAR-tank skip), Brambles 0/2 (BEAR-tank skip)
    talent_picks_resto: |
      Furor 3/3 (Energy on Faerie Fire) — wait, Furor is FERAL.
      Actual Resto picks: Heart of the Wild 3/3 (Stamina/Int/AP scaling),
      Naturalist 2/2 (Healing Touch cast time), Master Shapeshifter 2/2
      (form-specific bonus). UNVERIFIED Cata Resto talent placement.
    talent_picks_balance: |
      Furor 3/3 (Energy on Faerie Fire — wait this is Feral) OR
      Genesis 3/3 (DoT damage). UNVERIFIED Cata Balance pickup.
    rationale: Icy Veins 31/7/3 Feral consensus 5/8 sources.

  alternatives:
    31_5_5_split:
      talent_split: 31 Feral / 5 Resto / 5 Balance
      key_changes: 5 pts Resto for Naturalist + 5 pts Balance for spell-haste
      situational: Encounter-tunable for Healing Touch heavy fights.
```

### Spell unlock table

```yaml
spell_unlocks:
  # ─────── L1-10 baseline ───────
  WRATH:                     {level: 1,  spell_id: 5176}        # caster-form
  MOONFIRE:                  {level: 4,  spell_id: 8921}
  HEALING_TOUCH:             {level: 7,  spell_id: 5185}
  REJUVENATION:              {level: 9,  spell_id: 774}
  MARK_OF_THE_WILD:          {level: 14, spell_id: 1126}
  TRAVEL_FORM:               {level: 16, spell_id: 783}
  REBIRTH:                   {level: 24, spell_id: 20484}
  REMOVE_CORRUPTION:         {level: 24, spell_id: 2782}

  # ─────── L20+ Cat-form unlocks ───────
  CAT_FORM:                  {level: 20, spell_id: 768}
  POUNCE:                    {level: 20, spell_id: 9005}
  RAKE:                      {level: 24, spell_id: 1822}
  PROWL:                     {level: 22, spell_id: 5215}
  RIP:                       {level: 28, spell_id: 1079}
  CLAW:                      {level: 22, spell_id: 1082}        # pre-Mangle CP builder
  COWER:                     {level: 26, spell_id: 1742}
  TIGERS_FURY:               {level: 24, spell_id: 5217}        # UNVERIFIED Cata level (was 18)
  FEROCIOUS_BITE:            {level: 32, spell_id: 22568}
  SHRED:                     {level: 22, spell_id: 5221}
  RAVAGE:                    {level: 32, spell_id: 6785}
  SAVAGE_ROAR:               {level: 50, spell_id: 52610}      # UNVERIFIED level
  MAIM:                      {level: 62, spell_id: 22570}
  SWIPE_CAT:                 {level: 62, spell_id: 62078}      # UNVERIFIED Cata level
  FAERIE_FIRE_FERAL:         {level: 36, spell_id: 16857}

  # ─────── Spec abilities ───────
  MANGLE_CAT:                {level: 36, spell_id: 33876, requires_spec: feral}
  BERSERK_CAT:               {level: 50, spell_id: 50334, requires_talent: true}

  # ─────── Cata-new ───────
  SKULL_BASH_CAT:            {level: 60, spell_id: 80965}
  STAMPEDING_ROAR:           {level: 83, spell_id: 77764}

  # ─────── Cooldowns / utility ───────
  INNERVATE:                 {level: 38, spell_id: 29166}
  TRANQUILITY:               {level: 70, spell_id: 740}
  BARKSKIN:                  {level: 44, spell_id: 22812}
  SURVIVAL_INSTINCTS:        {level: 56, spell_id: 61336}
  HIBERNATE:                 {level: 18, spell_id: 2637}
  CYCLONE:                   {level: 50, spell_id: 33786}      # UNVERIFIED Cata level
  SOOTHE:                    {level: 16, spell_id: 2908}

  # ─────── Talent-gated passives ───────
  PREDATORY_STRIKES:         {requires_talent: true}            # Cat AP
  FERAL_AGGRESSION:          {requires_talent: true}            # Mangle/Maim
  KING_OF_THE_JUNGLE:        {requires_talent: true}            # Berserk dmg
  STAMPEDE:                  {requires_talent: true}            # Ravage post-TF
  PRIMAL_FURY:               {requires_talent: true}            # crit→CP
  ENDLESS_CARNAGE:           {requires_talent: true}            # Roar/Rake duration
  PREDATORY_SWIFTNESS:       {requires_talent: true}            # instant HT proc
  REND_AND_TEAR:             {requires_talent: true}            # +dmg vs bleeding
  FUROR:                     {requires_talent: true}            # Energy on FF cast
  NURTURING_INSTINCT:        {requires_talent: true}            # Agi→SP for HT

  # ─────── Mastery / passives ───────
  MASTERY_RAZOR_CLAWS:       {passive_mastery: true, base_pct: 25.0, per_point_pct: 3.13}
  LEATHER_SPECIALIZATION:    {passive: true, agi_pct: 5}        # +5% Agi w/ all-leather
```

### Leveling brackets

```yaml
bracket_1_19:
  available: WRATH, MOONFIRE, HEALING_TOUCH, REJUVENATION, MARK_OF_THE_WILD (14), TRAVEL_FORM (16), HIBERNATE (18)
  rotation: caster-form Moonfire + Wrath. NOT in Cat Form yet.

bracket_20_23:
  available: + CAT_FORM (20), + POUNCE (20), + CLAW (22), + SHRED (22), + PROWL (22)
  rotation: Cat Form + Claw / Shred + Pounce opener; no Mangle yet.

bracket_24_31:
  available: + RAKE (24), + TIGERS_FURY (24 UNVERIFIED), + COWER (26), + RIP (28)
  rotation: Cat Form + Rake DoT + Claw/Shred builder + Rip 5-CP finisher + Tiger's Fury on CD.

bracket_32_35:
  available: + FEROCIOUS_BITE (32), + RAVAGE (32)
  rotation: + Ferocious Bite execute window; Ravage stealth opener.

bracket_36_49:
  available: + MANGLE_CAT (36 spec), + FAERIE_FIRE_FERAL (36), + INNERVATE (38), + BARKSKIN (44)
  rotation_change: Mangle replaces Claw as primary front-of-target builder. Mangle bleed-debuff applied. FF feral debuff maintained.

bracket_50_55:
  available: + BERSERK_CAT (50 talent), + SAVAGE_ROAR (50 UNVERIFIED), + CYCLONE (50)
  rotation: + Savage Roar self-buff; Berserk burst CD on engagement.

bracket_56_69:
  available: + SURVIVAL_INSTINCTS (56), + SKULL_BASH_CAT (60), + MAIM (62), + SWIPE_CAT (62), + TRANQUILITY (70)
  rotation: + Skull Bash gap-close-interrupt; AoE Swipe Cat at 3+ enemies.

bracket_70_82:
  available: + Tranquility (raid heal); continuing Cata rune talents fill
  rotation: ST priority list active per §"Rotation".

bracket_83_84:
  available: + STAMPEDING_ROAR (83)
  rotation: + Raid speed buff on movement-heavy fights.

bracket_85:
  available: full Cata spell list
  rotation: full encounter rotation per §"Rotation".
```

### Mana floors by era

Cat-form combat does NOT use mana. Mana floors only relevant to drop-form Healing Touch / Tranquility / Rebirth.

```yaml
mana_floors_by_level:
  1_60:    { conserve: 0,  crisis: 0  }
  60_70:   { conserve: 55, crisis: 30 }
  70_80:   { conserve: 45, crisis: 25 }
  80_85:   { conserve: 50, crisis: 25 }
notes: Mana floors apply to caster-form spells only. Cat-form rotation uses Energy + CP.
```

### Energy / CP starvation thresholds

```yaml
resource_starvation:
  energy_starvation:
    detect: energy_pct < 25 AND combo_points < 5
    response: WAIT (auto-attack only); cast Tiger's Fury if off-CD
  combo_overflow:
    detect: combo_points == 5 AND no_finisher_window
    response: spend on Savage Roar refresh OR hold for Rip refresh window
  energy_overcap:
    detect: energy_pct >= 90
    response: cast immediate builder (Mangle/Shred) to dump
```

---

## UNVERIFIED items

| # | Item | Spec value | Verification |
|---|---|---|---|
| 1 | Predatory Swiftness proc rate | ~20% per builder (talent rank) | Talent.dbc 16972 |
| 2 | Predatory Swiftness duration | 8s | Spell.dbc 16974 |
| 3 | Primal Fury crit→CP rate | 100% on crit | Talent.dbc 16959 |
| 4 | Stampede duration post-TF | 10s | Spell.dbc 81021 |
| 5 | Razor Claws Mastery base + per-point | 25.0% + 3.13%/pt | Mastery DBC |
| 6 | Mangle bleed-debuff +30% bleed dmg | 30% | Spell.dbc 33876 |
| 7 | Mangle bleed-debuff duration | 60s | Spell.dbc 33876 |
| 8 | Faerie Fire (Feral) armor debuff % | 12% | Spell.dbc 16857 |
| 9 | Furor Energy gain on FF cast | UNVERIFIED amount | Talent.dbc |
| 10 | Glyph of Bloodletting Shred→Rip extension | +2s/Shred up to 6s cap | Glyph.dbc |
| 11 | Glyph of Tiger's Fury effect | TBD | Glyph.dbc |
| 12 | Glyph of Berserk effect (duration?) | +5s duration | Glyph.dbc |
| 13 | Glyph of Rip effect | TBD | Glyph.dbc |
| 14 | Glyph of Mangle effect | +%dmg | Glyph.dbc |
| 15 | Glyph of Shred effect | TBD | Glyph.dbc |
| 16 | Glyph of Ferocious Bite Rip-extension at sub-25% | +6s | Glyph.dbc |
| 17 | Berserk-Cat off-GCD | UNVERIFIED | Spell.dbc 50334 |
| 18 | Tiger's Fury off-GCD | true | Spell.dbc 5217 |
| 19 | Skull Bash min/max range | 13-25y | Spell.dbc 80965 |
| 20 | Skull Bash school lockout | 4s | Spell.dbc 80965 |
| 21 | Stampeding Roar speed % | 60% | Spell.dbc 77764 |
| 22 | Stampeding Roar radius | 10y | Spell.dbc 77764 |
| 23 | Rip CP→duration scaling | 16s base; +CP scaling | Spell.dbc 1079 |
| 24 | Savage Roar 1CP→5CP duration | 12s base; +6s/CP (5CP=42s) | Spell.dbc 52610 |
| 25 | Rake duration base | 9s base; +talent → 15s+ | Spell.dbc 1822 |
| 26 | Endless Carnage talent SR/Rake duration extension | UNVERIFIED | Talent.dbc |
| 27 | Predatory Strikes AP scaling | UNVERIFIED | Talent.dbc |
| 28 | Feral Aggression Mangle/Maim dmg | UNVERIFIED | Talent.dbc |
| 29 | King of the Jungle Berserk dmg % | UNVERIFIED | Talent.dbc 48492 |
| 30 | Tiger's Fury level (18 vs 24) | 24 (UNVERIFIED) | Spell trainer table |
| 31 | Savage Roar level (50 baseline?) | 50 (UNVERIFIED) | Spell trainer table |
| 32 | T13 2P set bonus (Cat) | TBD | Item set DBC |
| 33 | T13 4P set bonus (Cat) | TBD | Item set DBC |
| 34 | Cyclone level (Cata) | 50 (UNVERIFIED) | Spell trainer table |
| 35 | Travel Form mid-combat allowed (Cata change) | UNVERIFIED | Spell.dbc 783 |
| 36 | Remove Corruption Cat-form usable | UNVERIFIED | Spell.dbc 2782 |
| 37 | Cat Form Swipe (62078) Cata level | UNVERIFIED | Spell trainer table |
| 38 | Mangle CD removal during Berserk | true | Spell.dbc 50334 |
| 39 | Bleed snapshot rule (Cata 4.0 change) | bleeds NOT snapshot; tick inherits current | Spell mechanics |
| 40 | Pandemic window for Rip refresh | ~5s | Mechanic |
| 41 | Pandemic window for Rake refresh | ~3s | Mechanic |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kFeralCat_Spells[]` |
| Maintenance pass | `static const MaintenanceTrigger kFeralCat_Maintenance[]` |
| Rotation tiers | `static const RotationTier kFeralCat_Rotation[]` |
| AoE rotation | `static const RotationTier kFeralCat_RotationAoE[]` |
| Cooldown Profile | `static const CooldownTrigger kFeralCat_Cooldowns[]` |
| Energy + CP | dual-resource state (energy 0-100, combo_points 0-5 per-target) |
| Combo Points per-target | tracked on target GUID, not bot |
| Predatory Swiftness proc | aura listener + remaining ms |
| Stampede proc | aura listener |
| Bleed state | per-target (RAKE_BLEED, RIP_BLEED) with remaining_ms |
| Mangle bleed-debuff | per-target 60s tracker |
| Savage Roar | self-aura listener |
| Form state | Cat Form / Bear Form / caster form tracker |
| Stealth state | Prowl active flag |
| Position-behind-target | check for Shred eligibility |
| Leveling rotation | `LevelBracketRotation kFeralCat_Leveling[]` |

**Key new helper APIs:**
- `Energy(bot)` — read 0-100 (shared infra w/ Rogue Combat).
- `EnergyRegenRate(bot)` — base + Berserk + haste scaling.
- `ComboPoints(bot, target)` — read 0-5 attached to target (NOT bot).
- `PredatorySwiftnessActive(bot)` — proc aura tracker.
- `StampedeActive(bot)` — proc aura tracker.
- `IsRipUsable(bot, target)` — `combo_points(target) == 5 AND energy_pct >= 30`.
- `IsBehindTarget(bot, target)` — position check for Shred.
- `RipRemainingMs(target)` / `RakeRemainingMs(target)` — bleed timers.
- `SavageRoarRemainingMs(bot)` — self-buff timer.
- `MangleBleedDebuffRemainingMs(target)` — 60s debuff tracker.
- `IsAoEMode(bot)` — `enemies_within_8y_self >= 3`.
- `InCatForm(bot)` — form tracker.
- `IsProwling(bot)` — stealth flag.

**Feral-Cat-specific patterns NEW for codebase:**
1. **Combo Points per-target** — Cata 4.0 changed CP to attach to TARGET, not caster. Bot rotation must read `ComboPoints(bot, current_dps_target)` — switching targets resets CP. (Combat Rogue shares this; helper API can be shared.)
2. **Position-aware-rotation** — Shred (40E builder) requires behind-target; Mangle (35E builder) is omnidirectional. Bot rotation tier ordering swaps based on position. NEW for codebase — no other current spec has positional-only abilities.
3. **Energy pooling rule** — Tier-9 WAIT directive when Energy < 40 AND no CD ready. Bot must NOT burn GCDs on sub-optimal Mangle when pooling for Tiger's Fury / Berserk Energy refill. (Combat Rogue shares this concept.)
4. **Bleed pandemic windows** — Rake (3s) + Rip (5s) refresh rules. Bleeds inherit current stats on tick (Cata 4.0 change), BUT Tiger's Fury / Berserk windows still give all-damage-window value via window-buff. Refresh-during-window decision logic.
5. **Rogue-style resource model adapted for Druid** — shares Energy + CP infra with Combat Rogue. Helper APIs `Energy(bot)`, `ComboPoints(bot, target)` should be designed once and reused.
6. **Form-locked rotation** — Cat Form mandatory for DPS abilities. Bot must auto-recover form on accidental drop AND drop form for Predatory-Swiftness Healing Touch / Tranquility / Innervate utility.

Cross-reference to `docs/specs/balance-druid.md` for shared Druid mechanics (Mark of the Wild, Innervate, Faerie Fire caster, Hibernate, Soothe, Cyclone, Healing Touch, mana model), `docs/specs/resto-druid.md` for shared healing utility (Tranquility, Rebirth, Healing Touch, Predatory-Swiftness Resto-spell coverage), `docs/specs/combat-rogue.md` for shared Energy + CP resource model (per-target CP), and `docs/roles/melee-dps.md` for melee positioning patterns.
