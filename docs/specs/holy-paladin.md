# Holy Paladin — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide. The YAML blocks below are designed to lift cleanly into `static const` tables in a future `AltbotCombat.cpp`. Prose explains the reasoning so thresholds can be tuned without re-deriving them.

**Numerical data** (spell IDs, mana %, cast times, cooldowns, durations, item IDs) verified against Wowhead's Cata 4.3.4 archive (`wowhead.com/cata/spell=<id>`) and Warcraft Wiki spell pages. Items the AI will need but that public archives don't pin down with confidence are tagged `UNVERIFIED` — those should be confirmed against the server's `Spell.dbc` before code commits to the value.

**Strategy content** (rotation priority, cooldown triggers, mana/Holy-Power thresholds) reconciled across 9 community guides. See `docs/research/holy-paladin-guide-survey.md` for the comparison table and which thresholds are consensus vs author-judgment.

---

## Identity

```yaml
spec: holy_paladin
class: paladin
role: HEALER
resource_model: mana + holy_power_charges  # dual-resource — Holy Power 0–3
weapon_imbue: none                          # paladins use seals (Seal of Insight) not weapon imbues
defining_buffs:
  - Beacon_of_Light          # tank-tagged 100%-Holy-Light / 50%-other transfer aura
  - Seal_of_Insight          # always-on melee mana regen + healing %
  - Judgements_of_the_Pure   # haste buff refreshed by Judgement, 60s
defining_procs:
  - Daybreak                 # free off-CD Holy Shock
  - Infusion_of_Light        # reduces next Divine Light / Holy Radiance cast time
  - Conviction               # mastery-fed multiplicative healing buff
mastery: Illuminated_Healing  # absorb shield from direct heals
key_cooldowns:
  - Avenging_Wrath           # 3 min — +20% healing burst
  - Guardian_of_Ancient_Kings  # 5 min — Holy variant: healing CD; stack with AW
  - Aura_Mastery             # 2 min — magic-damage-aura phase
  - Hand_of_Sacrifice        # 2 min — tank emergency damage transfer
  - Lay_on_Hands             # 8 min — full-HP panic save
  - Divine_Plea              # 1 min — mana regen with -50% healing penalty
  - Divine_Shield            # 5 min — self-bubble (immunity)
  - Divine_Protection        # 1 min — personal -20% damage
ranged: 40
notes: |
  Holy Paladin in 4.3.4 plays as the pre-eminent single-target tank healer with
  a dual-resource model (mana for direct heals, Holy Power charges generated
  by Holy Shock / Crusader Strike / Tower-of-Radiance Beacon-target heals,
  spent on Word of Glory or Light of Dawn). Beacon of Light on the tank
  transfers 100% of Holy Light healing and 50% of all other heals to the
  tank — turning every other-target heal into partial tank healing.
  Mastery (Illuminated Healing) creates absorb shields from direct heals.

  Rotation heartbeat: Holy Shock on cooldown (6s base, modified by Speed of
  Light / Daybreak), Holy Light filler, spend Holy Power at 3 charges
  via Word of Glory (single-target, no CD for Holy spec) or Light of Dawn
  (cone AoE) when 3+ allies need help.
```

---

## Target taxonomy

The AI references these target selectors throughout. Implementations should resolve these against group state once per tick and reuse the resolved unit pointers across the rotation pass.

```yaml
target_selectors:
  SELF:                       # the bot itself
  TANK:                       # group member with TANK role flag; fallback = master
  BEACON_TARGET:              # current target of Beacon of Light buff (default = TANK)
  PARTY_LOWEST_HP:            # ally member with lowest hp_pct, in 40y, alive
  PARTY_LOWEST_MISSING_HP:    # ally with greatest absolute missing HP (favors tanks late-fight)
  HOLY_SHOCK_TARGET:          # lowest-HP injured ally <20y → tank → self → enemy(damage)
  LIGHT_OF_DAWN_CONE:         # 30y forward cone — Light of Dawn area
  HOLY_RADIANCE_ANCHOR:       # ally with the most injured allies clustered within 10y
  ENEMY_TARGET:               # bot's current attack target
  ENEMY_CASTING_INTERRUPTIBLE: # nearest enemy <10y casting an interruptible spell (Rebuke range)
  IN_MELEE_TARGET:            # enemy within 5y for Crusader Strike
```

---

## Spell Catalog

```yaml
# All spell IDs verified at wowhead.com/cata/spell=<id> + warcraft.wiki.gg/wiki/<spell>
# mana_pct = % of base mana (Cata standard)
# cast_time / cooldown / duration in ms
spells:
  HOLY_LIGHT:
    id: 635
    mana_pct: 12
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: holy
    target_type: ally
    notes: cheap mana-efficient default heal; Beacon transfers 100%; feeds Mastery absorb shield

  FLASH_OF_LIGHT:
    id: 19750
    mana_pct: 31
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: holy
    target_type: ally
    notes: fast emergency heal; mana-expensive; Beacon transfers 50%

  DIVINE_LIGHT:
    id: 82326
    mana_pct: 35
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: holy
    target_type: ally
    notes: heavy single-target heal; primary Infusion-of-Light consumer; Beacon transfers 50%

  HOLY_SHOCK:
    id: 20473
    mana_pct: 10                  # UNVERIFIED — Wowhead lists ~10% base; some guides cite 8% — confirm via DBC
    cast_time_ms: 0
    cooldown_ms: 6000
    on_gcd: true
    range: 40
    school: holy
    target_type: ally_or_enemy    # heals allies, damages enemies
    notes: |
      Rotation heartbeat. Generates 1 Holy Power on cast. Crit triggers
      Infusion of Light (next Divine Light/Holy Radiance -1.5s cast). Daybreak
      proc lets a second cast bypass the CD. Cast on lowest-HP ally; falls
      back to enemy when nothing needs healing (Tier 9 support DPS).

  WORD_OF_GLORY:
    id: 85673
    mana_pct: 0                   # spends Holy Power, not mana
    cast_time_ms: 0
    cooldown_ms: 0                # Holy spec only — Walk in the Light passive removes the 20s CD that Prot/Ret have
    on_gcd: true
    range: 40
    target_type: ally
    holy_power_cost: 1            # scales with HP charges spent (1, 2, or 3)
    notes: |
      Single-target Holy Power spend. Holy Pal has NO cooldown (Walk in the Light).
      Hold for 3 charges in non-emergency cases; emergency-spend at any HP if
      tank.hp_pct < 30. Beacon transfers 50%. UNVERIFIED Tower of Radiance
      interaction — spec encodes "Tower of Radiance 3/3 means Flash/Divine
      Light on Beacon target generates Holy Power."

  LIGHT_OF_DAWN:
    id: 85222
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0                      # cone from caster
    target_type: cone_30y
    holy_power_cost: 3            # always spends 3
    cone_angle: 30                # 30y range, ~30deg cone — UNVERIFIED exact cone arc; confirm via DBC
    target_count: 6               # G5 patch note: 5→6 in 4.3 — UNVERIFIED, confirm Spell.dbc EffectChainTarget
    notes: |
      AoE Holy Power spend. Fires when 3+ allies in cone need healing AND
      holy_power == 3. The 6-target cap is patch-4.3 — CRITICAL to verify
      against the engine's spell effect target count.

  HOLY_RADIANCE:
    id: 82327
    mana_pct: 40                  # most expensive heal in the kit
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 40                     # caster range to anchor target
    target_type: ally_anchored
    radius: 10                    # heals allies within 10y of the anchor target — UNVERIFIED, see survey gaps
    notes: |
      Cluster heal anchored on a friendly target. Fires when 3+ injured allies
      are within 10y of an anchor. Mana-expensive — prefer Light of Dawn when
      Holy Power is available. Infusion of Light reduces cast time.

  BEACON_OF_LIGHT:
    id: 53563
    mana_pct: 35
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 60
    target_type: ally
    duration_ms: 300000           # 5 min
    notes: |
      Tank-tagged transfer aura. 100% of Holy Light heals on ANY target
      transfer to the Beacon target; 50% of all other direct heals (Word
      of Glory, Holy Shock, Flash of Light, Divine Light, Light of Dawn,
      Holy Radiance) transfer 50%. Does NOT bounce on heals cast directly
      on the Beacon target — UNVERIFIED, see survey gaps.

  HOLY_POWER:
    # Pseudo-spell for the resource — enumerated here for clarity.
    max_charges: 3                # UNVERIFIED — assumed 3 in 4.3.4; 5 was MoP
    charges_decay: false          # do not decay out of combat
    notes: refer to per-cast holy_power_cost / holy_power_gain on each spell

  CRUSADER_STRIKE:
    id: 35395
    mana_pct: 0                   # melee strike; no mana cost
    cast_time_ms: 0
    cooldown_ms: 4500             # base
    on_gcd: true
    range: 5
    target_type: enemy_melee
    holy_power_gain: 1
    notes: melee Holy Power generator; Tier 9 fallback when out-of-range Holy Shock

  JUDGEMENT:
    id: 20271
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 8000
    on_gcd: true
    range: 30
    target_type: enemy
    notes: refreshes Judgements of the Pure (haste buff, 60s). Cast on cooldown when in range of an enemy.

  SEAL_OF_INSIGHT:
    id: 20165
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000          # 30 min
    notes: maintained at all times. Adds healing-on-melee effect + 4% mana on melee hits.

  AVENGING_WRATH:
    id: 31884
    mana_pct: 8
    cast_time_ms: 0
    cooldown_ms: 180000           # 3 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 30000
    notes: +20% healing/damage for 30s. Fire during heavy-damage phases.

  GUARDIAN_OF_ANCIENT_KINGS_HOLY:
    id: 86150
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 300000           # 5 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 30000
    notes: |
      Holy variant — summons a healing guardian that gives the paladin a
      stacking heal-amplification buff. Stack with Avenging Wrath when both
      are available.

  AURA_MASTERY:
    id: 31821
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 120000           # 2 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 6000
    notes: amplifies active aura effect (e.g., Resistance Aura) + 100% greater for 6s. Encounter-triggered, default off.

  HAND_OF_SACRIFICE:
    id: 6940
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 120000           # 2 min
    on_gcd: false
    range: 40
    target_type: ally
    duration_ms: 12000
    transfer_pct: 30              # 30% of damage taken by target transfers to caster
    notes: |
      Tank emergency CD. Transfers 30% of damage taken by target to caster
      for 12s. CAN self-kill the paladin — gate on self.hp_pct > 70.

  LAY_ON_HANDS:
    id: 633
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 480000           # 8 min (Glyph of Lay on Hands reduces by 2 min)
    on_gcd: false
    range: 40
    target_type: ally
    notes: full HP refill of target. Panic button only.

  DIVINE_PLEA:
    id: 54428
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 60000            # 1 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 9000
    notes: |
      +25% mana over 9s. Healing done is reduced by 50% during the buff —
      auto-cancel via Cancelaura if tank.hp_pct < 50 mid-channel.
      Glyph of Divine Plea: +6% additional mana.

  DIVINE_SHIELD:
    id: 642
    mana_pct: 6
    cast_time_ms: 0
    cooldown_ms: 300000           # 5 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 12000
    notes: full immunity 12s. Drops aggro / prevents most actions. Glyph of Divine Shield reduces some forbearance.

  DIVINE_PROTECTION:
    id: 498
    mana_pct: 3
    cast_time_ms: 0
    cooldown_ms: 60000            # 1 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 10000
    notes: -20% damage 10s (-40% magic with talent improvements). Personal mitigation; reactive.

  HAND_OF_PROTECTION:
    id: 1022
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 300000           # 5 min — UNVERIFIED, double-check against DBC
    on_gcd: false
    range: 30
    target_type: ally
    duration_ms: 10000
    notes: full physical immunity 10s on ally; cannot use on tanks during a melee fight.

  HAND_OF_FREEDOM:
    id: 1044
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 25000
    on_gcd: false
    range: 30
    target_type: ally
    duration_ms: 8000             # base; Speed of Light talent extends
    notes: removes movement-impairing effects + immunity to them for duration. Utility, default-off automatic.

  HAND_OF_SALVATION:
    id: 1038
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 120000
    on_gcd: false
    range: 40
    target_type: ally
    duration_ms: 10000
    notes: -20% threat reduction. Default-off automatic; manual command override only.

  REBUKE:
    id: 96231
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 15000
    on_gcd: false
    range: 5
    target_type: enemy_melee
    notes: melee interrupt + 4s school lockout. Holy Pal's only interrupt; in melee range only.

  CLEANSE:
    id: 4987
    mana_pct: 14
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    notes: removes Magic + Disease + Poison from ally. With Sacred Cleansing talent, also Magic. Fast and cheap; reactive utility.

  REDEMPTION:
    id: 7328
    mana_pct: 60
    cast_time_ms: 10000
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: corpse
    notes: out-of-combat resurrect. Bot uses post-wipe.

  # ─────────────────────────────────────────────────
  # Non-encounter spells (solo combat / utility / exorcism — for Holy Shock damage path)
  # ─────────────────────────────────────────────────

  EXORCISM:
    id: 879
    mana_pct: 6
    cast_time_ms: 1500
    cooldown_ms: 15000
    on_gcd: true
    range: 30
    target_type: enemy
    notes: ranged Holy damage. Useful in solo questing alongside Holy Shock damage path.

  CONSECRATION:
    id: 26573
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 10000
    notes: AoE ground DoT. Holy Pal solo AoE option; Tier 4 in solo rotation.
```

---

## Maintenance pass

Runs **before** the priority rotation each tick. Always-on substrate.

```yaml
maintenance:
  - name: Seal of Insight
    when:
      - aura.missing(SELF, SEAL_OF_INSIGHT)
    cast: SEAL_OF_INSIGHT

  - name: Beacon of Light on tank
    when:
      - target_resolves(TANK)
      - aura.missing(TANK, BEACON_OF_LIGHT) OR aura.expires_in_ms(TANK, BEACON_OF_LIGHT) < 30000
    cast: BEACON_OF_LIGHT on TANK
    notes: |
      30s refresh buffer is bot-author judgment (no guide gave a number);
      5-min duration is generous so the buffer is conservative. Spec doc
      mentions G5 wiki / G6 / G8 as the "Beacon on tank" consensus sources.

  - name: Judgement (Judgements of the Pure haste buff)
    when:
      - in_combat
      - aura.missing(SELF, JUDGEMENTS_OF_THE_PURE) OR aura.expires_in_ms(SELF, JUDGEMENTS_OF_THE_PURE) < 10000
      - target_resolves(ENEMY_TARGET)
      - distance_to(ENEMY_TARGET) <= 30
      - spell.off_cooldown(JUDGEMENT)
    cast: JUDGEMENT on ENEMY_TARGET
    notes: 60s buff; 10s refresh buffer is bot-author judgment.
```

---

## Rotation — Priority List

Tiers evaluated top-down. First true tier selects the cast. Only one cast per tick.

```yaml
rotation:
  # ───────── Tier 1: tank/self panic CDs ─────────
  - tier: 1
    name: Lay on Hands — panic button
    when:
      - target_resolves(SELF) AND SELF.hp_pct < 15
        OR target_resolves(TANK) AND TANK.hp_pct < 15
      - spell.off_cooldown(LAY_ON_HANDS)
      - no_other_save_available_in_next_1500ms
    cast: LAY_ON_HANDS on (SELF if SELF.hp_pct<15 else TANK)
    notes: 8min CD — the panic button. Author-judgment HP threshold (no guide gave a number).

  # ───────── Tier 2: Holy Shock on cooldown (rotation heartbeat) ─────────
  - tier: 2
    name: Holy Shock — on CD on lowest-HP ally
    when:
      - spell.off_cooldown(HOLY_SHOCK) OR aura.active(SELF, DAYBREAK)
      - target_resolves(HOLY_SHOCK_TARGET)
    cast: HOLY_SHOCK on HOLY_SHOCK_TARGET
    notes: |
      Generates 1 Holy Power. Daybreak proc bypasses CD — cast immediately
      when proc active. Target priority: lowest-HP ally <20y → tank → self →
      enemy (Tier 9 fallback when no allies need healing).

  # ───────── Tier 3: emergency tank protection ─────────
  - tier: 3
    name: Hand of Sacrifice — tank emergency damage transfer
    when:
      - target_resolves(TANK) AND TANK.hp_pct < 50
      - SELF.hp_pct > 70
      - spell.off_cooldown(HAND_OF_SACRIFICE)
      - tank_taking_predictable_burst       # bot heuristic — prior 3s incoming damage > 30% tank max HP
    cast: HAND_OF_SACRIFICE on TANK
    notes: |
      30% damage transfer for 12s. Self.hp>70 gate prevents self-kill.
      "Tank taking predictable burst" is a heuristic — until the bot has
      encounter-script awareness, use rolling damage window as a proxy.

  # ───────── Tier 4: Word of Glory at 3 Holy Power ─────────
  - tier: 4
    name: Word of Glory — single-target HP spend
    when:
      - holy_power == 3
      - target_resolves(PARTY_LOWEST_HP)
      - PARTY_LOWEST_HP.hp_pct < 70
      - NOT (party.injured_count(radius=30, hp_pct_lt=80) >= 3 AND holy_power == 3)
        # the negation defers to Tier 5 (Light of Dawn) when an AoE spend dominates
    cast: WORD_OF_GLORY on PARTY_LOWEST_HP
    notes: |
      Holy Pal has NO cooldown on WoG (Walk in the Light). Hold for 3 charges
      in non-emergency cases. 70% HP gate is bot-author judgment (no guide gave
      a number; matches "spend when there's a target that benefits").

  # ───────── Tier 4b: WoG emergency-spend at <3 charges ─────────
  - tier: 4b
    name: Word of Glory — emergency spend
    when:
      - target_resolves(TANK) AND TANK.hp_pct < 30
      - holy_power >= 1
      - spell.on_cooldown(HOLY_SHOCK)        # if HS off CD, prefer that for instant heal + HP gen
    cast: WORD_OF_GLORY on TANK
    notes: emergency override of the "save for 3" rule. Tank-only target.

  # ───────── Tier 5: Light of Dawn AoE HP spend ─────────
  - tier: 5
    name: Light of Dawn — cone AoE Holy Power spend
    when:
      - holy_power == 3
      - party.injured_count(radius=30, hp_pct_lt=80) >= 3
      - average_cluster_hp_pct(LIGHT_OF_DAWN_CONE) < 80
    cast: LIGHT_OF_DAWN at LIGHT_OF_DAWN_CONE
    notes: |
      30y forward cone. Always spends 3 charges. Cluster trigger of 3 is
      bot-author judgment (G3/G7 split between 3+ and 4+; spec uses 3 to
      cover 5-mans). Survey item Light-of-Dawn-target-count needs DBC verify.

  # ───────── Tier 6: Holy Radiance for stacked melee/cluster ─────────
  - tier: 6
    name: Holy Radiance on cluster anchor
    when:
      - target_resolves(HOLY_RADIANCE_ANCHOR)
      - party.injured_count_within(HOLY_RADIANCE_ANCHOR, radius=10, hp_pct_lt=85) >= 3
      - SELF.mana_pct > 50                   # mana-prohibitive otherwise
      - holy_power < 3 OR Light_of_Dawn_unavailable
        # prefer Light of Dawn when 3 HP available
    cast: HOLY_RADIANCE on HOLY_RADIANCE_ANCHOR
    notes: |
      40% base mana — most expensive spell in the kit. Only fires when LoD
      is not the better choice (no 3 HP, or LoD on cluster threshold not met).

  # ───────── Tier 7: Divine Light heavy single-target ─────────
  - tier: 7
    name: Divine Light on heavy-damage target
    when:
      - target_resolves(PARTY_LOWEST_HP)
      - any of:
          - PARTY_LOWEST_HP.hp_pct < 65
          - aura.active(SELF, INFUSION_OF_LIGHT)    # consume IoL on Divine Light
      - SELF.mana_pct > 50
    cast: DIVINE_LIGHT on PARTY_LOWEST_HP
    notes: |
      Divine Light is the IoL consumer of choice (G1/G4/G6/G7 consensus).
      DO NOT consume IoL on Flash of Light (mana-inefficient).

  # ───────── Tier 8: Flash of Light for emergency single-target ─────────
  - tier: 8
    name: Flash of Light — emergency burst
    when:
      - target_resolves(PARTY_LOWEST_HP)
      - PARTY_LOWEST_HP.hp_pct < 35
      - spell.on_cooldown(HOLY_SHOCK)
      - holy_power < 1                       # if any HP, prefer WoG
      - SELF.mana_pct > 25
    cast: FLASH_OF_LIGHT on PARTY_LOWEST_HP
    notes: emergency-only. Mana-expensive. Consensus across 5/5 strategy guides.

  # ───────── Tier 9: Holy Light filler ─────────
  - tier: 9
    name: Holy Light — efficient default cast
    when:
      - target_resolves(PARTY_LOWEST_HP)
      - PARTY_LOWEST_HP.hp_pct < 90
      - SELF.mana_pct > 25
    cast: HOLY_LIGHT on PARTY_LOWEST_HP
    notes: |
      Mana-efficient default. Beacon transfers 100% — biases target selection
      toward injured non-tank allies (the tank gets the Beacon back-transfer).
      Feeds Mastery absorb shields.

  # ───────── Tier 10: support DPS ─────────
  - tier: 10
    name: Support DPS — Holy Shock damage cast / Crusader Strike for HP gen
    when:
      - target_resolves(ENEMY_TARGET)
      - SELF.mana_pct >= 30                  # don't burn mana on offense when conserve mode
      - party.lowest_hp_pct >= 95            # nobody is injured
      - aura.active(TANK, BEACON_OF_LIGHT)   # tank's Beacon is up
      - not currently_casting
      - no_predicted_damage_in_next_2s

    cast_priority:
      - condition: in_melee_range(ENEMY_TARGET) AND spell.off_cooldown(CRUSADER_STRIKE) AND holy_power < 3
        cast: CRUSADER_STRIKE on ENEMY_TARGET   # Tier 10a — melee HP gen
      - condition: spell.off_cooldown(HOLY_SHOCK)
        cast: HOLY_SHOCK on ENEMY_TARGET        # Tier 10b — ranged HP gen via damage cast

    cancel_conditions:
      - cancel_if: any of tiers 1-3 becomes true
      - cancel_if: time-critical dispel becomes pending
      - cancel_if: SELF.mana_pct drops below 30 mid-cast

    notes: |
      Mirrors Resto Shaman Tier 9 support-DPS pattern. Cross-link to
      `docs/roles/healer.md` Support DPS section.
```

**Determinism property:** given a fixed snapshot of `(self.hp, self.mana, holy_power, party HP, active auras, active CDs, in_combat)`, exactly one tier resolves to a cast (or zero, in which case the bot idles).

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: AVENGING_WRATH
    when:
      - in_combat
      - any of:
          - party.injured_count(radius=40, hp_pct_lt=80) >= 3
          - target_resolves(TANK) AND TANK.hp_pct < 50
    rationale: |
      +20% healing burst CD. Author-judgment trigger ("3 injured OR tank<50%")
      since guides describe "heavy-damage windows" without numeric gates.

  - spell: GUARDIAN_OF_ANCIENT_KINGS_HOLY
    when:
      - spell.off_cooldown(GUARDIAN_OF_ANCIENT_KINGS_HOLY)
      - any of:
          - aura.active(SELF, AVENGING_WRATH)        # stack with AW
          - party.injured_count(radius=40, hp_pct_lt=70) >= 4
          - TANK.hp_pct < 40
    rationale: stack with Avenging Wrath when both available (4-of-4 guides agree).

  - spell: DIVINE_PLEA
    when:
      - in_combat
      - SELF.mana_pct < 80
      - not (TANK.hp_pct < 50 OR party.injured_count(radius=40, hp_pct_lt=70) >= 4)
        # auto-cancel mid-channel if emergency develops
    auto_cancel_when:
      - aura.active(SELF, DIVINE_PLEA)
      - any of:
          - TANK.hp_pct < 50
          - party.injured_count(radius=40, hp_pct_lt=70) >= 4
    rationale: |
      Mana regen with -50% healing penalty. Bot-aggressive 80% mana floor
      (vs guides' "low-damage window" hint) is justified because the bot
      can react to a rising emergency within ~150ms of detection.

  - spell: AURA_MASTERY
    when:
      - encounter.flag.incoming_aoe_magic_damage_phase   # NOT auto-fired today
    rationale: encounter-triggered only. Bot has no encounter-script awareness today; default-off.

  - spell: DIVINE_SHIELD
    when:
      - SELF.hp_pct < 20
      - not bot_must_stay_in_melee_for_threat_window    # don't bubble while tanking adds
    rationale: |
      Self-bubble. -50% healing penalty during, drops aggro. Author-judgment
      HP threshold; guides describe DSh as "panic" without a number.

  - spell: DIVINE_PROTECTION
    when:
      - SELF.hp_pct < 50
    rationale: -20% damage 10s. Author-judgment HP threshold; reactive (no incoming-damage prediction).

  - spell: HAND_OF_PROTECTION
    when:
      - manual_command_override                   # default-off automatic
    rationale: utility CD; raid context determines value.

  - spell: HAND_OF_FREEDOM
    when:
      - manual_command_override                   # default-off automatic
    rationale: utility; bot has no auto-trigger.

  - spell: HAND_OF_SALVATION
    when:
      - manual_command_override
    rationale: threat-reduction utility.
```

---

## Proc Reactions

```yaml
procs:
  DAYBREAK:
    talent_id: 88819            # UNVERIFIED — confirm Daybreak talent ID via DBC
    triggered_by: any heal cast (chance per cast based on rank)
    effect: next Holy Shock can be used while on cooldown (skips the CD trigger)
    ai_behavior: |
      Tier 2 already encodes "if Daybreak active, fire Holy Shock". No extra
      logic needed beyond aura-state tracking. UNVERIFIED proc rate per rank.

  INFUSION_OF_LIGHT:
    talent_id: 53672            # UNVERIFIED
    triggered_by: Holy Shock crit
    effect: -1.5s cast time on next Divine Light or Holy Radiance
    ai_behavior: |
      Tier 7 (Divine Light) gates on aura.active(SELF, INFUSION_OF_LIGHT)
      — consume IoL on Divine Light, NOT on Flash of Light (4-of-4 guides agree).

  CONVICTION:
    talent_id: 31866            # UNVERIFIED
    triggered_by: damage taken / damage dealt / heals (per Cata mechanic)
    effect: stacking healing-amplification buff (3% per stack, up to 3 stacks)
    ai_behavior: |
      Passive — no active reaction needed. Spec assumes high uptime in
      sustained encounters (G6 claim: 95%+). UNVERIFIED at low gear / crit.

  ETERNAL_GLORY:
    talent_id: 76669            # UNVERIFIED
    triggered_by: Word of Glory cast
    effect: 30% chance per HP charge spent to refund those charges
    ai_behavior: |
      Track WoG outcome — if charges refund, the next-tick rotation re-evaluates
      with the refunded charges available. No special prediction logic; the
      rotation is naturally reactive.
```

---

## Mana / Holy Power Management

```yaml
mana:
  thresholds:
    drink_floor_pct: 30
    drink_resume_pct: 95
    divine_plea_floor_pct: 80          # fire DP when mana drops below 80%
    conserve_mode_pct: 50              # below this, downshift the rotation
    crisis_mode_pct: 25                # below this, only T1-T4 + filler HL

  conserve_behavior:
    when: SELF.mana_pct < 50
    rules:
      - skip Tier 6 (HOLY_RADIANCE) entirely — too mana-expensive
      - skip Tier 7 (DIVINE_LIGHT) unless Infusion of Light proc active
      - prefer Tier 9 (HOLY_LIGHT) as the default cast
      - Flash of Light still permitted in Tier 8 (emergency-only)

  crisis_behavior:
    when: SELF.mana_pct < 25
    rules:
      - skip Tiers 5-8 unless tank.hp_pct < 30
      - Tier 9 forced to HOLY_LIGHT
      - fire MYTHICAL_MANA_POTION if off cooldown
      - fire DIVINE_PLEA regardless of healing-window rule (mana > healing throughput)

  out_of_combat:
    drink_when:
      - not in_combat
      - SELF.mana_pct < drink_floor_pct
      - no enemies within 40y for 5000ms
    eat_when: out_of_combat AND SELF.hp_pct < 80

holy_power:
  max_charges: 3                # UNVERIFIED — confirm 3 vs 5 in 4.3.4 (5 was MoP)

  spend_rules:
    - condition: holy_power == 3 AND party.injured_count(radius=30, hp_pct_lt=80) >= 3
      action: Light of Dawn (Tier 5)
    - condition: holy_power == 3 AND target_resolves(PARTY_LOWEST_HP) AND PARTY_LOWEST_HP.hp_pct < 70
      action: Word of Glory (Tier 4)
    - condition: holy_power < 3
      action: hold (continue generating via Holy Shock / Crusader Strike / Tower-of-Radiance)
    - condition: tank.hp_pct < 30 AND holy_power >= 1
      action: emergency-spend Word of Glory on tank (Tier 4b)

  generation:
    - Holy Shock: +1 per cast (ally OR enemy target)
    - Crusader Strike: +1 per cast (5y melee range)
    - Tower of Radiance (3/3 talent): Flash/Divine Light on Beacon target +1
```

---

## Consumables

```yaml
consumables:
  flask:
    item: Flask_of_the_Draconic_Mind
    item_id: 58086
    apply: pre_combat AND not aura.active(SELF, FLASK_OF_THE_DRACONIC_MIND)
    rationale: +300 intellect — best-in-slot for Holy throughput

  food:
    primary:
      item: Seafood_Magnifique_Feast
      item_id: 62290
      buff_stat: +90 to chosen primary stat (intellect for Holy)
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
      - cooldown_shared_potion.off_cooldown

  healthstone:
    item: Healthstone
    item_id: 36892
    use_when:
      - SELF.hp_pct < 30
      - not currently_casting OR currently_casting.spell_id != FLASH_OF_LIGHT
      - silenced OR cant_cast_for_ms > 1500
```

---

## Non-encounter content

### Solo combat — offensive rotation

When the bot has an enemy target and no friendly heal target requiring attention.

```yaml
solo_offensive_rotation:
  - tier: 1
    name: Self-preservation (break offensive cast)
    when:
      - SELF.hp_pct < 35
    cast: WORD_OF_GLORY on SELF if holy_power >= 1 else FLASH_OF_LIGHT on SELF

  - tier: 2
    name: Holy Shock — ranged HP gen + damage
    when:
      - spell.off_cooldown(HOLY_SHOCK)
      - in_combat
    cast: HOLY_SHOCK on ENEMY_TARGET (damage variant)

  - tier: 3
    name: Crusader Strike — melee HP gen
    when:
      - in_melee_range(ENEMY_TARGET)
      - spell.off_cooldown(CRUSADER_STRIKE)
      - holy_power < 3
    cast: CRUSADER_STRIKE on ENEMY_TARGET

  - tier: 4
    name: Consecration on dense pulls
    when:
      - enemies_in_radius(SELF, 8) >= 3
      - SELF.mana_pct > 50
      - spell.off_cooldown(CONSECRATION)
    cast: CONSECRATION

  - tier: 5
    name: Word of Glory self-heal at 3 charges
    when:
      - holy_power == 3
      - SELF.hp_pct < 70
    cast: WORD_OF_GLORY on SELF

  - tier: 6
    name: Judgement filler
    when:
      - distance_to(ENEMY_TARGET) <= 30
      - spell.off_cooldown(JUDGEMENT)
    cast: JUDGEMENT on ENEMY_TARGET

  - tier: 7
    name: Exorcism filler
    when:
      - distance_to(ENEMY_TARGET) <= 30
      - spell.off_cooldown(EXORCISM)
      - SELF.mana_pct > 30
    cast: EXORCISM on ENEMY_TARGET

  - tier: 8
    name: Out-of-mana fallback
    when:
      - SELF.mana_pct <= 20
    cast: AUTO_ATTACK
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
  - check: aura.missing(SELF, SEAL_OF_INSIGHT)
    action: cast SEAL_OF_INSIGHT
  - check: SELF.mana_pct < 80 AND not enemies_within(40)
    action: drink
  - check: SELF.hp_pct < 95 AND not enemies_within(40)
    action: eat
```

### Idle

```yaml
idle:
  primary_action: follow master (handled by AltbotFollow.cpp)
  secondary_actions:
    - maintain SEAL_OF_INSIGHT
    - drink/eat to full if below maintenance thresholds
  do_not:
    - do not cast offensive spells while idle (no aggro pulls)
    - do not engage neutral mobs unprompted
```

### World boss / elite scaling

```yaml
world_boss_or_elite:
  detection_signals:
    - target.classification == elite OR target.classification == world_boss
    - target.hp_max > 5 * SELF.hp_max

  rotation_override:
    if_in_party:
      use: encounter rotation
      thresholds:
        tier_1_panic_hp: 20            # raised — elites hit harder
        word_of_glory_emergency_tank_hp: 35
        crisis_mode_pct: 30
    if_solo:
      use: solo_offensive_rotation
      thresholds:
        word_of_glory_self_threshold: 80
        flash_of_light_self_threshold: 50
        do_not_engage_unless_can_solo: true

  cooldown_use:
    in_party: full encounter cooldown profile
    solo: Avenging Wrath + Guardian of Ancient Kings stacked when below 50% HP
```

### Disengage / emergency utility

```yaml
disengage:
  divine_shield:
    use_when:
      - SELF.hp_pct < 20
      - not in_combat OR safe_to_drop_aggro
    cast: DIVINE_SHIELD on SELF

  hand_of_protection_self:
    use_when:
      - SELF.hp_pct < 30
      - taking_physical_damage_predominantly
      - spell.on_cooldown(DIVINE_SHIELD)
    cast: HAND_OF_PROTECTION on SELF
```

---

## Leveling rotation

Per-bracket dungeon-healing rotations from level 1 → 85, bracketed at **spell-unlock breakpoints**. Spec choice is **pure Holy from level 10 onward** — bot's mandate is dungeon healing, so dual-spec for solo isn't relevant.

Spell-unlock levels verified against Wowhead Cata Classic + Warcraft Wiki (see `docs/research/holy-paladin-guide-survey.md` for the table). Strategy reconciled across the 9 surveyed sources.

### Spec recommendation

```yaml
spec_choice:
  for_dungeon_healer_bot:
    spec: Holy
    secondary_tree_split: 51 Holy + 5 Prot (Divinity 3/3 + Eternal Glory 2/2)
    rationale: |
      Holy is the only viable healing tree. Prot 5 split (Divinity + Eternal
      Glory) wins 4-of-5 guides over Ret 5 (Crusade + Pursuit of Justice).
      Bot doesn't need movement talents (perfect movement-AI) so PoJ is wasted.
```

### Spell unlock table

```yaml
spell_unlocks:
  HOLY_LIGHT:           {level: 1,  spell_id: 635}
  CRUSADER_STRIKE:      {level: 1,  spell_id: 35395}
  JUDGEMENT:            {level: 4,  spell_id: 20271}
  WORD_OF_GLORY:        {level: 7,  spell_id: 85673}
  SEAL_OF_INSIGHT:      {level: 10, spell_id: 20165}
  HOLY_SHOCK:           {level: 10, spell_id: 20473}        # Holy spec
  FLASH_OF_LIGHT:       {level: 20, spell_id: 19750}
  LAY_ON_HANDS:         {level: 24, spell_id: 633}
  AVENGING_WRATH:       {level: 30, spell_id: 31884}        # Holy spec at 30
  AURA_MASTERY:         {level: 36, spell_id: 31821}        # Holy spec
  BEACON_OF_LIGHT:      {level: 39, spell_id: 53563}        # Holy spec
  HAND_OF_SACRIFICE:    {level: 40, spell_id: 6940}
  DIVINE_LIGHT:         {level: 64, spell_id: 82326}        # Holy spec
  DIVINE_PLEA:          {level: 68, spell_id: 54428}
  HOLY_RADIANCE:        {level: 83, spell_id: 82327}        # Holy spec
  LIGHT_OF_DAWN:        {level: 85, spell_id: 85222}        # Holy spec, talent-gated
  GUARDIAN_OF_ANCIENT_KINGS: {level: 85, spell_id: 86150}   # Holy spec
```

### Bracket 1 — Levels 1–6 (pre-Word-of-Glory)

```yaml
bracket_1_6:
  available: HOLY_LIGHT, CRUSADER_STRIKE, JUDGEMENT (lvl 4)
  rotation:
    - tier: 1: SELF.hp_pct < 25 -> stop combat, retreat or HOLY_LIGHT self
    - tier: 2: PARTY_LOWEST_HP.hp_pct < 60 -> HOLY_LIGHT on PARTY_LOWEST_HP
    - tier: 3: idle -> follow tank
  notes: |
    Pre-Holy-Power. HOLY_LIGHT is the only heal. Holy Power generated by
    Crusader Strike / Judgement just buffers (cannot spend until level 7).
```

### Bracket 2 — Levels 7–9 (Word of Glory unlocks)

```yaml
bracket_7_9:
  available: HOLY_LIGHT, WORD_OF_GLORY (level 7)
  rotation:
    - tier: 1: SELF.hp_pct < 25 -> WORD_OF_GLORY self if holy_power>=1, else HOLY_LIGHT
    - tier: 2: PARTY_LOWEST_HP.hp_pct < 50 AND holy_power == 3 -> WORD_OF_GLORY
    - tier: 3: PARTY_LOWEST_HP.hp_pct < 70 -> HOLY_LIGHT on PARTY_LOWEST_HP
    - tier: 4: idle -> follow tank
  notes: |
    First Holy Power spend unlocks. Generate via Crusader Strike / Judgement
    when in melee range; spend at 3 charges via WoG.
```

### Bracket 3 — Levels 10–19 (Holy Shock + Seal of Insight)

```yaml
bracket_10_19:
  available: + HOLY_SHOCK, SEAL_OF_INSIGHT (Holy spec at 10)
  rotation:
    - tier: 1: SELF.hp_pct < 25 -> WoG self
    - tier: 2: HOLY_SHOCK off CD -> HOLY_SHOCK on lowest-HP ally
    - tier: 3: holy_power == 3 AND PARTY_LOWEST_HP.hp_pct < 70 -> WORD_OF_GLORY
    - tier: 4: PARTY_LOWEST_HP.hp_pct < 80 -> HOLY_LIGHT on PARTY_LOWEST_HP
    - tier: 5: idle -> follow tank
  maintenance:
    - SEAL_OF_INSIGHT always on
  notes: |
    Holy Shock becomes the rotation heartbeat at 10. Cast on cooldown for
    HP generation + the heal/damage. Mana floors: 0 (no conserve mode pre-60).
```

### Bracket 4 — Levels 20–23 (Flash of Light)

```yaml
bracket_20_23:
  available: + FLASH_OF_LIGHT
  rotation:
    - tier: 1: SELF.hp_pct < 25 -> WoG self
    - tier: 2: HOLY_SHOCK off CD -> HOLY_SHOCK
    - tier: 3: holy_power == 3 AND PARTY_LOWEST_HP.hp_pct < 70 -> WoG
    - tier: 4: PARTY_LOWEST_HP.hp_pct < 35 -> FLASH_OF_LIGHT (emergency-only)
    - tier: 5: PARTY_LOWEST_HP.hp_pct < 80 -> HOLY_LIGHT
    - tier: 6: idle -> follow tank
  notes: Flash of Light slots in as emergency-only (5/5 guide consensus). Mana-expensive.
```

### Bracket 5 — Levels 24–29 (Lay on Hands)

```yaml
bracket_24_29:
  available: + LAY_ON_HANDS
  rotation_addition:
    - tier: 0 (panic): tank.hp_pct < 15 OR self.hp_pct < 15 AND LoH off CD -> LAY_ON_HANDS
  notes: 8-min CD panic button enters the rotation.
```

### Bracket 6 — Levels 30–35 (Avenging Wrath)

```yaml
bracket_30_35:
  available: + AVENGING_WRATH (Holy spec)
  cooldown_addition:
    - AVENGING_WRATH when party.injured_count >= 2 AND in_combat
  notes: 3min CD healing burst. Author-judgment trigger (2 injured for low-level smaller groups).
```

### Bracket 7 — Levels 36–38 (Aura Mastery)

```yaml
bracket_36_38:
  available: + AURA_MASTERY (Holy spec)
  notes: encounter-triggered only — bot defaults Aura Mastery off. Player can override via command.
```

### Bracket 8 — Levels 39–63 (Beacon of Light)

```yaml
bracket_39_63:
  available: + BEACON_OF_LIGHT (Holy spec at 39), + HAND_OF_SACRIFICE (40)
  maintenance_addition:
    - BEACON_OF_LIGHT on TANK, refresh < 30s remaining
  cooldown_addition:
    - HAND_OF_SACRIFICE when tank.hp_pct < 50 AND self.hp_pct > 70
  notes: |
    Beacon mechanic activates. 100% Holy Light transfer to Beacon target,
    50% for all other heals. Mana floors at 60: 55 conserve / 30 crisis (BC era).
```

### Bracket 9 — Levels 64–67 (Divine Light)

```yaml
bracket_64_67:
  available: + DIVINE_LIGHT (Holy spec)
  rotation_change:
    - new tier (between WoG and HOLY_LIGHT): PARTY_LOWEST_HP.hp_pct < 65 OR aura.active(SELF, INFUSION_OF_LIGHT) -> DIVINE_LIGHT
  notes: heavy-damage single-target heal. IoL consumer of choice.
```

### Bracket 10 — Levels 68–82 (Divine Plea + Mana Crisis Era)

```yaml
bracket_68_82:
  available: + DIVINE_PLEA
  cooldown_addition:
    - DIVINE_PLEA when self.mana_pct < 80 AND no_emergency
  notes: |
    Mana floors at 70-80: 45 conserve / 25 crisis (WotLK regen overhaul).
    Floors at 80-85: 50 conserve / 25 crisis (Cata regen tightening).
```

### Bracket 11 — Level 83 (Holy Radiance)

```yaml
bracket_83:
  available: + HOLY_RADIANCE (Holy spec)
  rotation_addition:
    - new tier (between Light of Dawn-ish and Divine Light): cluster heal trigger
  notes: |
    Cluster heal anchored on a friendly target. Mana-expensive — gates on
    self.mana_pct > 50.
```

### Bracket 12 — Level 85 (Light of Dawn + Guardian of Ancient Kings)

```yaml
bracket_85:
  available: + LIGHT_OF_DAWN (talent at 85), + GUARDIAN_OF_ANCIENT_KINGS_HOLY
  rotation_change:
    - Tier 5 Light of Dawn enters the rotation (full encounter rotation now active)
  cooldown_addition:
    - GUARDIAN_OF_ANCIENT_KINGS_HOLY when stacking with Avenging Wrath OR raid emergency
  notes: |
    Full encounter rotation (Tiers 1-10) is now active. From here, the
    encounter rotation in §"Rotation — Priority List" is canonical.
```

### Mana floors by era

```yaml
mana_floors_by_level:
  1_60:    { conserve: 0,  crisis: 0  }    # mana isn't tight pre-BC
  60_70:   { conserve: 55, crisis: 30 }    # BC era — highest stress
  70_80:   { conserve: 45, crisis: 25 }    # WotLK regen overhaul
  80_85:   { conserve: 50, crisis: 25 }    # Cata regen tightening
```

### Glyph priorities (per bracket)

```yaml
glyphs_by_level:
  pre_25:  skip                              # glyph slots not unlocked
  25_50:   [Glyph_of_Holy_Shock]             # prime
  50_75:   [+Glyph_of_Seal_of_Insight, +Glyph_of_Divine_Plea (major)]
  75_85:   [+Glyph_of_Divine_Favor (prime alternate), +Glyph_of_Divinity (major), +Glyph_of_Lay_on_Hands (major)]
```

---

## UNVERIFIED items

These are facts the spec relies on that need confirmation against `Spell.dbc` or in-game GM lookup before code commits to the value. Cross-reference to `docs/research/dbc-verification-checklist.md` once a Holy Paladin checklist is added.

| # | Item | Spec value | Where used | Verification path |
|---|---|---|---|---|
| 1 | Holy Power max charges | 3 | `holy_power.max_charges`, all rotation tiers | `Spell.dbc` PowerType for Holy Power resource — confirm cap is 3 not 5 |
| 2 | Holy Shock mana cost | 10% base mana | `HOLY_SHOCK.mana_pct` | `Spell.dbc` row 20473 PowerCost / PowerCostPercentage — some guides cite 8% |
| 3 | Light of Dawn target count | 6 | `LIGHT_OF_DAWN.target_count` | `Spell.dbc` row 85222 EffectChainTarget — was 5 → 6 in 4.3 |
| 4 | Light of Dawn cone arc | 30° (assumed) | `LIGHT_OF_DAWN.cone_angle` | `Spell.dbc` cone arc — not in survey |
| 5 | Holy Radiance radius | 10y | `HOLY_RADIANCE.radius` | `Spell.dbc` row 82327 EffectRadiusIndex — guides give 10y to 20y |
| 6 | Beacon-bounce on direct-Beacon-target heal | does NOT proc | rotation Tier 9 (HOLY_LIGHT target bias) | `Spell.dbc` SpellLinkedSpell + procflag scrutiny |
| 7 | Walk in the Light WoG-no-CD | Holy spec only | `WORD_OF_GLORY.cooldown_ms` | `Spell.dbc` row 85673 + Holy spec aura interaction |
| 8 | Daybreak talent ID | 88819 | `procs.DAYBREAK.talent_id` | `Talent.dbc` lookup |
| 9 | Infusion of Light talent ID | 53672 | `procs.INFUSION_OF_LIGHT.talent_id` | `Talent.dbc` lookup |
| 10 | Conviction talent ID | 31866 | `procs.CONVICTION.talent_id` | `Talent.dbc` lookup |
| 11 | Eternal Glory talent ID | 76669 | `procs.ETERNAL_GLORY.talent_id` | `Talent.dbc` lookup |
| 12 | Hand of Protection cooldown | 5 min | `HAND_OF_PROTECTION.cooldown_ms` | `Spell.dbc` row 1022 |
| 13 | Holy Shock at level 10 | spec ability | `spell_unlocks.HOLY_SHOCK.level` | DBC level requirement; 4-of-9 guides confirm 10 |
| 14 | Conviction uptime at low gear | "high" | `procs.CONVICTION.ai_behavior` | bot telemetry at low-crit brackets |

---

## Open hooks for `AltbotCombat.cpp`

The eventual code structure mirrors the doc layout:

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kHolyPaladin_Spells[]` |
| Rotation tiers | `static const RotationTier kHolyPaladin_Rotation[]` |
| Cooldown Profile | `static const CooldownTrigger kHolyPaladin_Cooldowns[]` |
| Mana / Holy Power Management | `ManaState` enum + `HolyPowerState` enum + threshold helpers |
| Leveling rotation | `LevelBracketRotation kHolyPaladin_Leveling[]` keyed by character level |
| Beacon target tracking | `BeaconState` (target guid + remaining duration) on `AltbotAI` |
| Daybreak / Infusion of Light proc tracking | `ProcState` tracked via aura listeners on the bot's session |

**Key new helper APIs the bot's combat layer needs:**

- `HolyPower(bot)` — read current charges (0–3).
- `BeaconTarget(bot)` — read current Beacon target guid.
- `AverageClusterHpPct(anchor, radius)` — for Light of Dawn / Holy Radiance trigger.
- `IsInMeleeRange(bot, target)` — for Crusader Strike Tier 10a fallback.
- `PredictedDamageInNextMs(target, ms)` — for "no_predicted_damage_in_next_2s" support-DPS gate. Until encounter-script awareness exists, this is a 3s rolling-window heuristic (incoming damage on TANK over the past 3s extrapolated forward).

Cross-reference to `docs/roles/healer.md` for triage hierarchy / dispel taxonomy / mana pacing patterns.
