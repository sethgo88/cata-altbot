# Enhancement Shaman — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide.

**Strategy content** reconciled across 8 community sources. See `docs/research/enhancement-shaman-guide-survey.md`. Cross-reference: shaman totem framework + Lightning Shield + Hex + Wind Shear + Heroism are documented in `docs/specs/elemental-shaman.md` and `docs/specs/resto-shaman.md`.

**SPEC-CHOICE RATIONALE.** Enhancement is the only Shaman melee DPS spec — Elemental is ranged caster (already documented), Resto is healer (already documented). No spec-selection contest. **Enhancement is unique among Cata 4.3.4 melee DPS specs because it is the only hybrid melee-caster** — Maelstrom Weapon stacks make instant Lightning Bolt / Chain Lightning / Healing Wave casts a core part of the rotation, not a fallback. This is the **fifth melee-DPS spec** in the project.

---

## Identity

```yaml
spec: enhancement_shaman
class: shaman
role: MELEE_DPS
resource_model: mana + maelstrom_weapon_stacks (0-5)
defining_mechanic: Maelstrom Weapon proc engine (stacking 0-5 on melee swings) + Stormstrike (8s CD) + Lava Lash (10s CD) + Flame Shock DoT + Mental Quickness AP→SP conversion (Enh's primary spellpower source)
defining_buffs:
  - Lightning_Shield            # always-on; +Nature dmg on melee crit (Static Shock)
  - Flametongue_Weapon          # MH weapon imbue; on-hit Fire damage + spellpower
  - Windfury_Weapon             # OH weapon imbue; chance for 3 extra attacks
  - Maelstrom_Weapon_proc       # 0-5 stack proc; spell cast time -20% per stack (5 stacks = instant)
  - Unleash_Wind                # buff from Unleash Elements; +50% melee atk speed 8s
  - Stormstrike_Buff            # next 2 nature spells +25% crit (UNVERIFIED stack mechanic)
  - Searing_Flames              # 5-stack DoT from Searing Totem hits; refreshes Flame Shock
defining_procs:
  - Maelstrom_Weapon            # melee swings → stack 0-5; primary rotation engine
  - Static_Shock                # Lightning Shield charge consumed on melee crit → bonus dmg + mana
  - Mental_Quickness            # passive 50% AP → SP conversion (Enh's spellpower)
mastery: Enhanced_Elements      # +% damage on Nature/Fire/Frost spells
key_cooldowns:
  - Feral_Spirit                # 2min CD; 2 Spirit Wolves 30s
  - Fire_Elemental_Totem        # 5min CD; 2min Fire Elemental
  - Earth_Elemental_Totem       # 5min CD; 2min defensive
  - Shamanistic_Rage            # 1min CD; -30% damage taken 15s + mana regen
  - Heroism / Bloodlust         # 5min CD; raid +30% haste 40s
  - Stoneclaw_Totem             # 30s CD; talent-buffed; absorbs damage
  - Astral_Recall               # hearth-anywhere
  - Wind_Shear                  # 6s CD off-GCD interrupt 25y
  - Hex                         # 35s CD; CC 60s (breaks on damage)
melee_range: 5
notes: |
  Enh in 4.3.4 plays as a dual-wield Agility-melee with two parallel
  rotation engines: the standard melee CD priority (Stormstrike → Lava
  Lash → Unleash Elements → Flame Shock refresh) and the Maelstrom Weapon
  spell-cast pulse (5-stack consumption on Lightning Bolt — or Chain
  Lightning AoE / Healing Wave self-heal). Heartbeat: Lightning Shield
  always-on (recast on expire/charge depletion); Flametongue MH +
  Windfury OH imbues always-on (1-hour duration; recast on expire);
  Searing Totem auto-pulses Searing Flames to keep Flame Shock refreshed;
  Stormstrike + Lava Lash on CD; Maelstrom-Lightning-Bolt fires at 5
  stacks (consumed inside Stormstrike-buff window when active).

  Burst window: Feral Spirit + Fire Elemental Totem + Tolvir Potion
  stacked at first pull, ideally inside raid-Bloodlust. Re-fire Feral
  Spirit on its 2-min CD; Fire Elemental drifts on 5-min CD.

  Pre-Stormstrike (under L30): Primal Strike fills Stormstrike's slot;
  no Maelstrom Weapon (talented only); pre-Lava-Lash same. Pre-talent
  rotation is plain Flame Shock + Earth Shock + Lightning Bolt.

  Mental Quickness is the spell-power source: Agility scales AP, AP
  scales SP via Mental Quickness 50%, so Maelstrom-Lightning-Bolt damage
  comes from melee gear, not caster gear. Enh shares the Hunter's
  "Agility-melee" stat curve — NOT Strength like Warrior/Ret/DK/Rogue.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  CURRENT_DPS_TARGET:
  ENEMY_CASTING_INTERRUPTIBLE:    # Wind Shear range: 25y
  AOE_CLUSTER:                    # ≥3 enemies within 12y (Lava Lash spread + Magma Totem + Fire Nova)
  PRIMARY_AOE_TARGET:             # Flame Shock seed for Lava Lash spread + Fire Nova
  CC_MARKER_HEX:                  # caster add CC
  TOTEM_GROUND_AT_SELF:           # totem placement (8y radius from cast)
  ALLY_PURGE_TARGET:              # Cleansing Totem / Purge target
```

---

## Spell Catalog

```yaml
spells:
  # ─────── Melee CD spenders ───────

  STORMSTRIKE:
    id: 17364
    mana_pct: 8
    cast_time_ms: 0
    cooldown_ms: 8000
    on_gcd: true
    range: 5
    target_type: enemy_melee
    requires_spec: enhancement
    notes: |
      Primary melee CD. Cata change: no longer +nature dmg debuff; now
      gives Stormstrike-buff (next 2 nature spells +25% crit). 2 weapon
      hits + the buff. Spend on rune availability (mana >= 8% gate).

  LAVA_LASH:
    id: 60103
    mana_pct: 4
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: true
    range: 5
    target_type: enemy_melee
    requires_spec: enhancement
    notes: |
      Off-hand-weapon ability. With Flametongue MH imbue: +30% damage.
      Spreads Flame Shock to up to 4 targets within 12y of primary
      (REQUIRES Flame Shock active on primary). Top priority when off-CD.

  PRIMAL_STRIKE:
    id: 73899
    mana_pct: 6
    cast_time_ms: 0
    cooldown_ms: 8000
    on_gcd: true
    range: 5
    target_type: enemy_melee
    notes: |
      Pre-30 placeholder for Stormstrike. Replaced by Stormstrike at
      L30 talent point. Out of rotation post-30.

  # ─────── Shock spells (one shock CD shared) ───────

  FLAME_SHOCK:
    id: 8050
    mana_pct: 17
    cast_time_ms: 0
    cooldown_ms: 6000               # shock CD shared with EARTH_SHOCK / FROST_SHOCK
    on_gcd: true
    range: 25
    target_type: enemy
    duration_ms: 18000              # base; talent-extended
    notes: |
      Primary disease-equivalent DoT. Refresh ≤ 3s remaining if
      shock-CD up. Searing Flames stacks (from Searing Totem) auto-
      refresh duration so manual refresh frequency is reduced. PRIMARY
      SHOCK in Enh rotation.

  EARTH_SHOCK:
    id: 8042
    mana_pct: 18
    cast_time_ms: 0
    cooldown_ms: 6000               # shared shock CD
    on_gcd: true
    range: 25
    target_type: enemy
    notes: |
      Filler shock when Flame Shock healthy (≥ 6s remaining) AND shock
      CD up. Lower priority than Stormstrike/Lava Lash; used as shock-CD
      filler.

  FROST_SHOCK:
    id: 8056
    mana_pct: 18
    cast_time_ms: 0
    cooldown_ms: 6000               # shared shock CD
    on_gcd: true
    range: 25
    target_type: enemy
    notes: Snare; non-rotational. Manual / kiting use only.

  # ─────── Maelstrom-consumption casters ───────

  LIGHTNING_BOLT:
    id: 403
    mana_pct: 10
    cast_time_ms: 2500              # 0 with 5 Maelstrom Weapon stacks
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: enemy
    notes: |
      Cast time -20% per Maelstrom stack: 5 stacks = INSTANT. Fire at
      5 stacks ALWAYS in ST rotation (consumes proc engine). Inside
      Stormstrike-buff window: extra crit chance.

  CHAIN_LIGHTNING:
    id: 421
    mana_pct: 26
    cast_time_ms: 2000              # 0 with 5 Maelstrom Weapon stacks
    cooldown_ms: 3000               # CD remains even at 0 cast time
    on_gcd: true
    range: 30
    target_type: enemy
    notes: |
      AoE Maelstrom-consumer: hits 3 targets. Replace Lightning Bolt
      at 2+ targets. Cast time -20% per Maelstrom stack.

  HEALING_WAVE:
    id: 331
    mana_pct: 9
    cast_time_ms: 3000              # 0 with 5 Maelstrom stacks
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    notes: |
      Defensive Maelstrom-consumer. At HP ≤ 30%, consume Maelstrom
      stacks on Healing Wave instead of Lightning Bolt. Casts a base-
      tier heal (small mana cost, instant at 5 stacks).

  HEALING_SURGE:
    id: 8004
    mana_pct: 24
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ally
    notes: |
      Hard-cast emergency heal; rarely usable in melee combat.
      Maelstrom-Healing-Wave preferred for emergency self-heal.

  # ─────── Cata-new ability ───────

  UNLEASH_ELEMENTS:
    id: 73680
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 15000
    on_gcd: true
    range: 30
    target_type: enemy_or_ally
    requires_level: 81
    notes: |
      Cata-new L81. Releases the active weapon imbue's effect:
      Flametongue → fire damage + +30% next FS damage; Windfury →
      Unleash Wind buff (+50% melee atk speed 8s). Cast on CD —
      primarily for Unleash Wind buff window timing.

  # ─────── Buffs / shields ───────

  LIGHTNING_SHIELD:
    id: 324
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 600000             # 10min OR until charges depleted
    charges_max: 9                  # UNVERIFIED — Static Shock talent extends?
    notes: |
      Always-on Enh shield. Each charge does Nature damage on melee hit
      taken (UNVERIFIED — Cata change may have changed proc-on-hit-given
      vs hit-taken). Recast when charges < 3 OR expired. Static Shock
      talent: chance on melee crit to discharge bonus.

  WATER_SHIELD:
    id: 52127
    mana_pct: 0                       # mana-restoring shield
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 600000
    notes: Healer/Caster shield; NEVER used in Enh DPS rotation.

  FLAMETONGUE_WEAPON:
    id: 8024
    mana_pct: 4
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: false                     # imbue cast off-GCD
    range: 0
    target_type: weapon_self
    duration_ms: 3600000              # 60min
    slot: main_hand
    notes: |
      MH imbue. On-hit Fire damage + +SP buff for self. Always-on; pre-
      pull cast. Recast at < 5min remaining. Lava Lash bonus (+30% dmg
      with FT MH).

  WINDFURY_WEAPON:
    id: 8232
    mana_pct: 4
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: false
    range: 0
    target_type: weapon_self
    duration_ms: 3600000
    slot: off_hand
    notes: |
      OH imbue. Chance on-hit for 3 extra attacks (UNVERIFIED PPM rate).
      Always-on; pre-pull cast.

  # ─────── Totems ───────

  SEARING_TOTEM:
    id: 3599
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 0                    # totem-recall, not CD-based
    on_gcd: true
    range: 0
    target_type: ground_at_self
    radius: 25                        # totem attack range
    duration_ms: 60000                # 1min default
    notes: |
      Fire-totem slot. Pulses Searing Bolt damage every ~1.5s. With
      Searing Flames talent: stacks Searing Flames on target + auto-
      refreshes Flame Shock duration. Mandatory on ST. Recast on totem
      expire.

  MAGMA_TOTEM:
    id: 8190
    mana_pct: 18
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: ground_at_self
    radius: 8                         # PBAoE
    duration_ms: 60000
    notes: |
      AoE Fire-totem (replaces Searing Totem). Pulses fire damage in 8y
      every ~2s. Use AoE 3+ enemies within 8y of totem position.

  FIRE_ELEMENTAL_TOTEM:
    id: 2894
    mana_pct: 23
    cast_time_ms: 0
    cooldown_ms: 300000               # 5min
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 120000               # Fire Elemental persists 2min
    notes: |
      5min DPS CD. Summons Fire Elemental which auto-attacks + casts
      Fire Nova. Stack with Bloodlust + Feral Spirit pull. CONSUMES
      Fire-totem slot — Searing/Magma Totem disabled while Fire
      Elemental Totem active.

  EARTH_ELEMENTAL_TOTEM:
    id: 2062
    mana_pct: 23
    cast_time_ms: 0
    cooldown_ms: 300000
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 60000
    notes: |
      Defensive 5min CD. Earth Elemental tanks add. Used on add waves
      where Bot is at-risk. Consumes Earth-totem slot (replaces Stoneskin
      / Strength-of-Earth raid totem).

  STRENGTH_OF_EARTH_TOTEM:
    id: 8075
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 300000
    notes: Earth-totem raid Str/Agi buff. Pre-pull drop if no Kings/MotW.

  STONESKIN_TOTEM:
    id: 8071
    mana_pct: 6
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 300000
    notes: Earth-totem armor buff. Default if no Str/Agi need.

  HEALING_STREAM_TOTEM:
    id: 5394
    mana_pct: 3
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 300000
    notes: Water-totem passive raid heal (~1% HP/3s).

  MANA_SPRING_TOTEM:
    id: 5675
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 300000
    notes: Water-totem raid mana-regen buff.

  TREMOR_TOTEM:
    id: 8143
    mana_pct: 1
    cast_time_ms: 0
    cooldown_ms: 60000                # Cata: 1min CD + 6s active
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 6000
    notes: Cata change: active CD ability. Cleanses fear/charm/sleep.

  WIND_SHEAR:
    id: 57994
    mana_pct: 1
    cast_time_ms: 0
    cooldown_ms: 6000
    on_gcd: false
    range: 25
    target_type: enemy
    notes: |
      Primary interrupt. Off-GCD. 25y range — Enh's interrupt-distance
      advantage over melee-range-only interrupts. 2s lockout on school
      interrupted.

  FIRE_NOVA:
    id: 1535
    mana_pct: 22
    cast_time_ms: 0
    cooldown_ms: 4000
    on_gcd: true
    range: 0
    target_type: pbaoe_around_targets_with_flame_shock
    radius: 10
    notes: |
      Cata change: detonates Flame Shock on every infected target within
      10y. AoE-only when 3+ targets diseased via Lava Lash spread. Mana-
      hungry; gate behind enemies_within_10y >= 3 AND mana > 50%.

  FERAL_SPIRIT:
    id: 51533
    mana_pct: 12
    cast_time_ms: 0
    cooldown_ms: 120000
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 30000
    summon: SPIRIT_WOLVES
    notes: |
      2min CD. Summons 2 Spirit Wolves 30s. Off-GCD AT FIRE — UNVERIFIED.
      Stack with Fire Elemental + Bloodlust at pull. DPS cooldown.

  HEX:
    id: 51514
    mana_pct: 3
    cast_time_ms: 1500
    cooldown_ms: 35000
    on_gcd: true
    range: 20
    target_type: enemy
    duration_ms: 60000
    notes: CC; turns target into frog. Breaks on damage. Manual / encounter.

  GHOST_WOLF:
    id: 2645
    mana_pct: 5
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    notes: Out-of-combat travel form +30% speed. Travel only.

  ASTRAL_RECALL:
    id: 556
    mana_pct: 6
    cast_time_ms: 10000
    cooldown_ms: 900000
    on_gcd: true
    range: 0
    target_type: self
    notes: Hearth-anywhere 15min CD.

  # ─────── Defensive cooldowns ───────

  SHAMANISTIC_RAGE:
    id: 30823
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 15000
    requires_talent: true
    notes: |
      -30% damage taken 15s + mana regen (15% of melee crit chance per
      hit). Cast at HP ≤ 60% OR mana ≤ 30%.

  STONECLAW_TOTEM:
    id: 5730
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: true
    range: 0
    target_type: ground_at_self
    duration_ms: 15000
    notes: |
      Earth-totem; absorbs damage. Talented (Glyph of Stoneclaw):
      shields self for ~25% HP. Defensive 30s CD.

  PURGE:
    id: 370
    mana_pct: 6
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: enemy
    notes: |
      Removes 1 magic buff from enemy. PvP-favored; PvE use only on
      enrage/buff-removal mechanics (rare).

  CLEANSE_SPIRIT:
    id: 51886
    mana_pct: 13
    cast_time_ms: 0
    cooldown_ms: 8000
    on_gcd: true
    range: 40
    target_type: ally
    notes: Removes curse from friendly. Manual / dispel.
```

---

## Maintenance pass

```yaml
maintenance:
  - id: LIGHTNING_SHIELD
    target: SELF
    aura: LIGHTNING_SHIELD_BUFF
    refresh_when:
      - aura.missing(SELF, LIGHTNING_SHIELD_BUFF)
      - OR aura.charges(SELF, LIGHTNING_SHIELD_BUFF) < 3
    cast: LIGHTNING_SHIELD on SELF
    notes: Pre-pull buff; recast on charge depletion.

  - id: FLAMETONGUE_WEAPON
    target: SELF
    aura: FLAMETONGUE_WEAPON_IMBUE
    refresh_when:
      - aura.missing(SELF, FLAMETONGUE_WEAPON_IMBUE)
      - OR aura.expires_in_ms(SELF, FLAMETONGUE_WEAPON_IMBUE) < 300000
    cast: FLAMETONGUE_WEAPON on SELF
    notes: MH imbue; 1hr duration; recast at < 5min remaining.

  - id: WINDFURY_WEAPON
    target: SELF
    aura: WINDFURY_WEAPON_IMBUE
    refresh_when:
      - aura.missing(SELF, WINDFURY_WEAPON_IMBUE)
      - OR aura.expires_in_ms(SELF, WINDFURY_WEAPON_IMBUE) < 300000
    cast: WINDFURY_WEAPON on SELF
    notes: OH imbue; 1hr duration; recast at < 5min remaining.

  - id: FLAME_SHOCK
    target: CURRENT_DPS_TARGET
    aura: FLAME_SHOCK_DOT
    refresh_when:
      - aura.missing(CURRENT_DPS_TARGET, FLAME_SHOCK_DOT)
      - OR aura.expires_in_ms(CURRENT_DPS_TARGET, FLAME_SHOCK_DOT) < 3000
      - shock_cd_ready
    cast: FLAME_SHOCK on CURRENT_DPS_TARGET
    notes: Searing Flames stacks (from Searing Totem) auto-refresh; manual refresh rare.

  - id: SEARING_TOTEM
    target: GROUND_AT_SELF
    refresh_when:
      - totem.missing(FIRE_TOTEM_SLOT)
      - OR totem.expires_in_ms(SEARING_TOTEM) < 5000
      - NOT aura.active(SELF, FIRE_ELEMENTAL_TOTEM_ACTIVE)
      - NOT aoe_mode
    cast: SEARING_TOTEM at SELF
    notes: ST-mode default fire totem; replaced by Magma Totem in AoE.

  - id: MAGMA_TOTEM_AOE
    target: GROUND_AT_SELF
    refresh_when:
      - aoe_mode
      - totem.missing(FIRE_TOTEM_SLOT) OR totem.is(SEARING_TOTEM)
      - NOT aura.active(SELF, FIRE_ELEMENTAL_TOTEM_ACTIVE)
    cast: MAGMA_TOTEM at SELF
    notes: AoE-mode fire totem.
```

---

## Rotation — Priority List (Single-Target)

```yaml
rotation:
  # ───────── Tier 1: Stormstrike on CD ─────────
  - tier: 1
    name: Stormstrike on CD
    when:
      - spell.off_cooldown(STORMSTRIKE)
      - target_in_range(5)
      - SELF.mana_pct > 10
    cast: STORMSTRIKE on CURRENT_DPS_TARGET
    notes: Top priority — Stormstrike-buff drives Maelstrom-LB crit window.

  # ───────── Tier 2: Lava Lash on CD ─────────
  - tier: 2
    name: Lava Lash on CD
    when:
      - spell.off_cooldown(LAVA_LASH)
      - target_in_range(5)
    cast: LAVA_LASH on CURRENT_DPS_TARGET
    notes: |
      With FT MH imbue: +30% damage. AoE: spreads Flame Shock to nearby targets.

  # ───────── Tier 3: Maelstrom-Lightning-Bolt at 5 stacks ─────────
  - tier: 3
    name: Maelstrom-LB — 5 stacks instant cast
    when:
      - aura.stacks(SELF, MAELSTROM_WEAPON_PROC) >= 5
      - target_in_range(30)
    cast: LIGHTNING_BOLT on CURRENT_DPS_TARGET
    notes: |
      Always consume at 5 stacks. Inside Stormstrike-buff window:
      bonus crit chance. Skip if HP < 30% — use Maelstrom-Healing-Wave
      defensive instead.

  # ───────── Tier 4: Unleash Elements on CD ─────────
  - tier: 4
    name: Unleash Elements on CD (L81+)
    when:
      - level >= 81
      - spell.off_cooldown(UNLEASH_ELEMENTS)
      - target_in_range(30)
    cast: UNLEASH_ELEMENTS on CURRENT_DPS_TARGET
    notes: Triggers Unleash Wind buff (+50% melee atk speed 8s).

  # ───────── Tier 5: Flame Shock refresh ─────────
  - tier: 5
    name: Flame Shock — refresh / apply
    when:
      - any_of:
          - aura.missing(CURRENT_DPS_TARGET, FLAME_SHOCK_DOT)
          - aura.expires_in_ms(CURRENT_DPS_TARGET, FLAME_SHOCK_DOT) < 3000
      - shock_cd_ready
      - SELF.mana_pct > 20
      - target_in_range(25)
    cast: FLAME_SHOCK on CURRENT_DPS_TARGET

  # ───────── Tier 6: Earth Shock filler ─────────
  - tier: 6
    name: Earth Shock — shock-CD filler
    when:
      - shock_cd_ready
      - aura.expires_in_ms(CURRENT_DPS_TARGET, FLAME_SHOCK_DOT) >= 6000
      - SELF.mana_pct > 25
      - target_in_range(25)
    cast: EARTH_SHOCK on CURRENT_DPS_TARGET
    notes: Spent shock-CD on damage when FS healthy.
```

---

## AoE Mode

Activated by `enemies_within_12y_of_self >= 3`. Hysteresis: exit `<2 sustained for 5s`.

```yaml
rotation_aoe:
  - tier: 1
    name: Flame Shock on primary AoE target
    when:
      - aura.missing(PRIMARY_AOE_TARGET, FLAME_SHOCK_DOT)
      - shock_cd_ready
    cast: FLAME_SHOCK on PRIMARY_AOE_TARGET
    notes: Required seed for Lava Lash spread + Fire Nova.

  - tier: 2
    name: Lava Lash — Flame Shock spread
    when:
      - aura.active(PRIMARY_AOE_TARGET, FLAME_SHOCK_DOT)
      - spell.off_cooldown(LAVA_LASH)
    cast: LAVA_LASH on PRIMARY_AOE_TARGET

  - tier: 3
    name: Magma Totem — replace Searing Totem
    when:
      - NOT totem.is(MAGMA_TOTEM)
      - NOT aura.active(SELF, FIRE_ELEMENTAL_TOTEM_ACTIVE)
    cast: MAGMA_TOTEM at SELF

  - tier: 4
    name: Fire Nova — detonate FS on infected
    when:
      - enemies_with_flame_shock_within_10y >= 3
      - spell.off_cooldown(FIRE_NOVA)
      - SELF.mana_pct > 50
    cast: FIRE_NOVA

  - tier: 5
    name: Stormstrike on CD
    when:
      - spell.off_cooldown(STORMSTRIKE)
    cast: STORMSTRIKE on CURRENT_DPS_TARGET

  - tier: 6
    name: Maelstrom-Chain-Lightning at 5 stacks
    when:
      - aura.stacks(SELF, MAELSTROM_WEAPON_PROC) >= 5
      - spell.off_cooldown(CHAIN_LIGHTNING)
    cast: CHAIN_LIGHTNING on CURRENT_DPS_TARGET

  - tier: 7
    name: Unleash Elements on CD
    when:
      - level >= 81
      - spell.off_cooldown(UNLEASH_ELEMENTS)
    cast: UNLEASH_ELEMENTS on CURRENT_DPS_TARGET

  - tier: 8
    name: Earth Shock filler
    when:
      - shock_cd_ready
      - aura.expires_in_ms(PRIMARY_AOE_TARGET, FLAME_SHOCK_DOT) >= 6000
    cast: EARTH_SHOCK on PRIMARY_AOE_TARGET
```

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: FERAL_SPIRIT
    when:
      - in_combat
      - spell.off_cooldown(FERAL_SPIRIT)
      - any_of:
          - boss_engagement
          - cooldown_window_aligned (Bloodlust + Fire Elemental Totem)
    rationale: 2min CD; align first w/ Fire Elemental + Bloodlust.

  - spell: FIRE_ELEMENTAL_TOTEM
    when:
      - in_combat
      - spell.off_cooldown(FIRE_ELEMENTAL_TOTEM)
      - any_of:
          - boss_engagement
          - cooldown_window_aligned (Bloodlust + Feral Spirit)
    rationale: 5min CD; align first w/ Feral Spirit + Bloodlust.

  - spell: EARTH_ELEMENTAL_TOTEM
    when:
      - SELF.taking_high_damage
      - SELF.hp_pct < 50
      - spell.off_cooldown(EARTH_ELEMENTAL_TOTEM)
    rationale: 5min CD defensive (replaces Fire Elemental's totem slot).

  - spell: SHAMANISTIC_RAGE
    when:
      - any_of:
          - SELF.hp_pct < 60
          - SELF.mana_pct < 30
      - spell.off_cooldown(SHAMANISTIC_RAGE)
    rationale: 1min CD; defensive + mana sustain.

  - spell: STONECLAW_TOTEM
    when:
      - SELF.taking_burst_damage
      - SELF.hp_pct < 50
      - spell.off_cooldown(STONECLAW_TOTEM)
    rationale: 30s defensive shield.

  - spell: WIND_SHEAR
    when:
      - target.casting AND target.cast_remaining_ms > 300
      - target.cast_interruptible
      - target_in_range(25)
      - spell.off_cooldown(WIND_SHEAR)
    rationale: 6s CD off-GCD interrupt; 25y range advantage.

  - spell: HEX
    when:
      - manual_command OR cc_marker_tagged
      - target.cc_able
      - target_in_range(20)
      - spell.off_cooldown(HEX)
    rationale: 35s CD; 60s CC; breaks on damage.

  - spell: HEALING_SURGE
    when:
      - SELF.hp_pct < 30
      - aura.stacks(SELF, MAELSTROM_WEAPON_PROC) < 5
      - no_other_save_ready
    rationale: Hard-cast emergency only.
```

---

## Proc Reactions

```yaml
procs:
  MAELSTROM_WEAPON_PROC:
    talent_id: 51530
    triggered_by: melee auto-attack OR Stormstrike OR Lava Lash hit
    proc_chance_per_hit: ~10%         # UNVERIFIED — talent-rank scaled
    effect: stacks 0-5; each stack -20% spell cast time on next nature spell
    duration_ms: 30000
    max_stacks: 5
    ai_behavior: |
      Tier-3 priority — consume at 5 stacks on Lightning Bolt
      (single-target) OR Chain Lightning (AoE) OR Healing Wave
      (HP < 30%). Inside Stormstrike-buff window: prefer LB for crit.

  STATIC_SHOCK:
    talent_id: 51527
    triggered_by: melee crit
    proc_chance: 15-45% (talent rank)
    effect: discharge 1 Lightning Shield charge → bonus Nature damage + 8% mana
    duration_ms: instant
    ai_behavior: Passive — informs Lightning Shield maintenance (charge depletion).

  ENHANCED_ELEMENTS_MASTERY:
    mastery_passive: true
    triggered_by: any Nature/Fire/Frost spell damage
    effect: +% damage scaling with Mastery rating
    base_pct: 20.0                    # UNVERIFIED Enh-mastery base
    per_point_pct: 2.5                # UNVERIFIED per-point
    ai_behavior: Passive — informs reforge.

  MENTAL_QUICKNESS:
    talent_id: 30814
    passive: true
    effect: |
      AP→SP conversion: 50% of Attack Power becomes Spell Power for nature/fire/
      frost spells. Enh's primary spellpower source.
    ai_behavior: Passive — drives Agility-melee gear curve (NOT Strength).

  STORMSTRIKE_BUFF:
    spell_id: 17364                   # buff applied by Stormstrike
    triggered_by: Stormstrike cast
    effect: next 2 nature spells cast +25% crit chance
    duration_ms: 12000                # UNVERIFIED
    max_stacks: 2
    ai_behavior: |
      Inside this window, prefer Lightning Bolt (Maelstrom-consumer) over
      shock spells for crit chance. Don't waste on shock-CD spells (Flame
      Shock / Earth Shock are nature but Stormstrike-buff is per-cast,
      not per-shock-CD).

  UNLEASH_WIND:
    spell_id: 73681                   # buff from Unleash Elements w/ Windfury
    triggered_by: Unleash Elements cast (Windfury imbue active)
    effect: +50% melee atk speed 8s
    duration_ms: 8000
    ai_behavior: Passive — drives Maelstrom proc rate inside window.
```

---

## Resource Management

```yaml
mana:
  pool_pct_max: 100
  regen: passive_5sr + Static_Shock_proc + Shamanistic_Rage + Mana_Spring_Totem
  conserve_threshold_pct: 30
  crisis_threshold_pct: 15
  decay: none

maelstrom_weapon:
  pool_max: 5
  generators:
    - melee_auto_attack: ~10% per hit (talent-scaled)
    - Stormstrike_hit: triggers MW chance
    - Lava_Lash_hit: triggers MW chance
  consumers:
    - Lightning_Bolt: full consume (5 stacks)
    - Chain_Lightning: full consume (5 stacks)
    - Healing_Wave: full consume (5 stacks)
    - Healing_Surge: full consume (5 stacks; rare)
  decay: none in combat; full reset on combat exit (UNVERIFIED — may persist short window)

shock_cd:
  pool: shared 6s CD across FLAME_SHOCK / EARTH_SHOCK / FROST_SHOCK
  ai_behavior: shock-CD up = check FS refresh first, ES filler second

action_eligibility:
  stormstrike: spell.off_cooldown(STORMSTRIKE) AND mana_pct > 10
  lava_lash: spell.off_cooldown(LAVA_LASH) AND mana_pct > 5
  flame_shock: shock_cd_ready AND mana_pct > 20
  earth_shock: shock_cd_ready AND mana_pct > 25
  maelstrom_lightning_bolt: aura.stacks(MW) >= 5 AND mana_pct > 12
  maelstrom_chain_lightning: aura.stacks(MW) >= 5 AND mana_pct > 28
  unleash_elements: spell.off_cooldown(UE) AND mana_pct > 10 AND level >= 81
  fire_nova: enemies_with_flame_shock_within_10y >= 3 AND mana_pct > 50
```

---

## Consumables

```yaml
consumables:
  - item: TOLVIR_POTION                 # +Agility 25s
    when:
      - boss_engagement (pre-pot)
      - cooldown_window_aligned (Feral Spirit + Fire Elemental + Bloodlust)
    notes: Pre-pot 1s before pull; combat-pot 1min mark when CDs realign.

  - item: FLASK_OF_THE_WINDS              # +Agility flask
    when:
      - in_combat AND aura.missing(SELF, FLASK_BUFF)
    notes: Always-on flask.

  - item: SCROLL_OF_AGILITY_VIII
    when:
      - aura.missing(SELF, [BLESSING_OF_KINGS, MARK_OF_THE_WILD])
    notes: Stat scroll fallback.

  - item: AGILE_SHADOWSPIRIT_DIAMOND      # +Agility meta-gem
    when: passive
    notes: Helm meta-gem.

  - item: SHARPENING_STONE_PYRIUM         # +500 weapon damage (BLOCK — Enh imbues both weapons)
    when: NEVER
    notes: |
      Enh uses Flametongue MH + Windfury OH; sharpening stones / weapon
      enchants override imbues. NEVER apply.

  - item: FOOD_BUFF_SKEWERED_EEL          # +90 Agility
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
      Lightning Shield + imbues + Searing Totem pre-pull. Stormstrike on CD,
      Lava Lash on CD, Flame Shock refresh, Maelstrom-LB at 5 stacks. No DPS
      CDs (Feral Spirit / Fire Elemental held for elite/group).
  - condition: solo_elite_target
    rotation: standard rotation + Feral Spirit + Earth Elemental Totem on engagement.
  - condition: low_hp_self
    cast: |
      Maelstrom-Healing-Wave (HP < 30%) →
      Shamanistic Rage (HP < 60% AND mana < 30%) →
      Healing Surge hard-cast (HP < 20% no other save).
```

### Travel & out-of-combat

```yaml
travel:
  - condition: master_riding AND not_combat
    cast: standard mount (handled by AltbotMount); fallback Ghost Wolf
  - condition: out_of_combat AND aura.missing(SELF, LIGHTNING_SHIELD_BUFF)
    cast: LIGHTNING_SHIELD on SELF
  - condition: out_of_combat AND aura.missing(SELF, FLAMETONGUE_WEAPON_IMBUE)
    cast: FLAMETONGUE_WEAPON on SELF
  - condition: out_of_combat AND aura.missing(SELF, WINDFURY_WEAPON_IMBUE)
    cast: WINDFURY_WEAPON on SELF
```

### Idle

```yaml
idle:
  - condition: out_of_combat AND no_active_target
    behavior: |
      Maintain shield + imbues. No proactive cast otherwise.
  - condition: out_of_combat AND HP < 50%
    cast: HEALING_SURGE on SELF (hard-cast, safe out of combat); fallback Bandage.
```

### Disengage

```yaml
disengage:
  - condition: master.fleeing AND in_combat
    behavior: |
      Drop combat → Ghost Wolf if drop succeeds → catch up via standard mount /
      Ghost Wolf.
  - condition: bot.HP < 25% AND no_save_ready
    cast: |
      EARTH_ELEMENTAL_TOTEM (taunt + tank) OR STONECLAW_TOTEM (shield) OR
      Maelstrom-Healing-Wave at 5 stacks.
```

---

## Leveling rotation

Pure Enh from L10.

### Spec recommendation

```yaml
spec_choice:
  for_leveling_dungeon_dps_bot:
    spec: Enhancement
    talent_split: 31 Enhancement / 7 Elemental / 3 Restoration
    talent_picks_enh: |
      Mental Quickness 3/3, Improved Stormstrike 2/2, Searing Flames 3/3,
      Maelstrom Weapon 3/3, Shamanistic Rage 1/1, Static Shock 3/3,
      Toughness 3/3, Improved Lava Lash 2/2, Feral Spirit 1/1, Improved
      Shields 3/3, Frozen Power 0/2 (or 2/2 for Frost Shock kiting),
      Stormstrike 1/1, Earthen Power 2/2, Acuity 3/3, Unleashed Rage 3/3
    talent_picks_ele: |
      Convection 3/3, Concussion 2/2, Elemental Devastation 3/3 (crit chance)
      OR
      Acuity 3/3, Reverberation 2/2 (alt path)
    talent_picks_resto: |
      Tidal Focus 3/3 (mana cost reduction) OR Improved Shields supplement
    rationale: Icy Veins 31/7/3 consensus 6/8 sources.

  alternatives:
    31_3_7_resto_split:
      talent_split: 31 Enh / 3 Elemental / 7 Restoration
      key_changes: 7 pts Resto for Tidal Focus + Improved Shields stacking
      rationale: Higher mana sustain; ceiling slightly lower than 31/7/3.
```

### Spell unlock table

```yaml
spell_unlocks:
  # ─────── L1-10 baseline ───────
  LIGHTNING_BOLT:            {level: 3,  spell_id: 403}
  EARTH_SHOCK:               {level: 4,  spell_id: 8042}
  HEALING_WAVE:              {level: 7,  spell_id: 331}        # later renamed Healing Surge in Cata
  LIGHTNING_SHIELD:          {level: 8,  spell_id: 324}
  FLAME_SHOCK:               {level: 10, spell_id: 8050}
  PRIMAL_STRIKE:             {level: 3,  spell_id: 73899}      # Enh placeholder

  # ─────── L10-30 ───────
  SEARING_TOTEM:             {level: 12, spell_id: 3599}
  STONESKIN_TOTEM:           {level: 4,  spell_id: 8071}
  STRENGTH_OF_EARTH_TOTEM:   {level: 16, spell_id: 8075}
  WATER_SHIELD:              {level: 20, spell_id: 52127}
  HEALING_STREAM_TOTEM:      {level: 8,  spell_id: 5394}
  MANA_SPRING_TOTEM:         {level: 30, spell_id: 5675}
  MAGMA_TOTEM:               {level: 26, spell_id: 8190}
  GHOST_WOLF:                {level: 16, spell_id: 2645}
  WIND_SHEAR:                {level: 16, spell_id: 57994}      # Cata-restored
  HEALING_SURGE:             {level: 7,  spell_id: 8004}       # base Healing Wave renamed
  FLAMETONGUE_WEAPON:        {level: 10, spell_id: 8024}
  WINDFURY_WEAPON:           {level: 30, spell_id: 8232, requires_spec: enh}

  # ─────── L30+ Enh-spec abilities ───────
  STORMSTRIKE:               {level: 30, spell_id: 17364, requires_spec: enh}
  LAVA_LASH:                 {level: 30, spell_id: 60103, requires_spec: enh}    # UNVERIFIED level
  FERAL_SPIRIT:              {level: 30, spell_id: 51533, requires_talent: true}
  MAELSTROM_WEAPON:          {requires_talent: true}                              # passive proc
  SHAMANISTIC_RAGE:          {level: 50, spell_id: 30823, requires_talent: true}

  # ─────── Cata-new (L81+) ───────
  UNLEASH_ELEMENTS:          {level: 81, spell_id: 73680}
  SPIRITWALKERS_GRACE:       {level: 85, spell_id: 79206}     # Resto/Ele primarily

  # ─────── Cooldowns / utility ───────
  FIRE_ELEMENTAL_TOTEM:      {level: 66, spell_id: 2894}      # UNVERIFIED Cata level shift
  EARTH_ELEMENTAL_TOTEM:     {level: 60, spell_id: 2062}
  TREMOR_TOTEM:              {level: 22, spell_id: 8143}
  CLEANSE_SPIRIT:            {level: 22, spell_id: 51886}
  PURGE:                     {level: 12, spell_id: 370}
  HEX:                       {level: 38, spell_id: 51514}
  ASTRAL_RECALL:             {level: 28, spell_id: 556}
  BLOODLUST:                 {level: 70, spell_id: 2825}      # Horde
  HEROISM:                   {level: 70, spell_id: 32182}     # Alliance
  FIRE_NOVA:                 {level: 28, spell_id: 1535}

  # ─────── Talent-gated passives ───────
  MENTAL_QUICKNESS:          {requires_talent: true}           # AP→SP 50%
  STATIC_SHOCK:              {requires_talent: true}           # LS proc on melee crit
  SEARING_FLAMES:            {requires_talent: true}           # Searing Totem stacking DoT
  IMPROVED_STORMSTRIKE:      {requires_talent: true}           # SS mana cost / dmg
  IMPROVED_LAVA_LASH:        {requires_talent: true}           # LL bonus
  TOUGHNESS:                 {requires_talent: true}           # Stamina passive
  FROZEN_POWER:              {requires_talent: true}           # Frost Shock dmg/snare
  ACUITY:                    {requires_talent: true}           # +crit
  UNLEASHED_RAGE:            {requires_talent: true}           # raid melee haste

  # ─────── Mastery / passives ───────
  MASTERY_ENHANCED_ELEMENTS: {passive_mastery: true, base_pct: 20.0, per_point_pct: 2.5}  # UNVERIFIED
  ANCESTRAL_SWIFTNESS:       {passive: true}                   # +5% melee/ranged speed
  PLATE_SPECIALIZATION:      {passive: false}                  # mail-armor, no plate spec
  MAIL_SPECIALIZATION:       {passive: true, agi_pct: 5}       # +5% Agi w/ all-mail
```

### Leveling brackets

```yaml
bracket_1_3:
  available: PRIMAL_STRIKE, LIGHTNING_BOLT (3)
  rotation: Primal Strike + LB hard-cast + auto

bracket_4_9:
  available: + EARTH_SHOCK (4), + HEALING_WAVE (7), + LIGHTNING_SHIELD (8), + HEALING_STREAM_TOTEM (8)
  rotation: PS + LS + ES + LB + auto

bracket_10_15:
  available: + FLAME_SHOCK (10), + SEARING_TOTEM (12), + FLAMETONGUE_WEAPON (10), + GHOST_WOLF (16), + WIND_SHEAR (16)
  rotation: |
    Pre-buff LS + FT MH; Searing Totem on engagement; PS on CD; FS DoT;
    LB hard-cast filler; ES shock-CD filler. Wind Shear interrupts.

bracket_20_29:
  available: + WATER_SHIELD (20), + STR/AGI talents
  rotation: same; transition to first Enh talent points (Maelstrom Weapon partial)

bracket_30_49:
  available: + STORMSTRIKE (30 spec ability), + LAVA_LASH (30 UNVERIFIED), + WINDFURY_WEAPON (30), + MAGMA_TOTEM (26)
  rotation_change: |
    Full Enh rotation activates. Stormstrike replaces Primal Strike. Lava Lash
    on CD. WF OH imbue replaces unmodified OH. MW stacks track to 5 →
    instant Lightning Bolt. Searing Flames stacks (talent) auto-refresh FS.

bracket_50_65:
  available: + SHAMANISTIC_RAGE (50), + FERAL_SPIRIT (talent ~30+), continued talent fills
  rotation: + Shamanistic Rage on HP/mana low; Feral Spirit on engagement.

bracket_66_80:
  available: + FIRE_ELEMENTAL_TOTEM (~66), + EARTH_ELEMENTAL_TOTEM (60)
  rotation: + Fire Elemental on 5min CD; Earth Elemental defensive add-tank.

bracket_81_84:
  available: + UNLEASH_ELEMENTS (81)
  rotation: + Unleash Elements on 15s CD for Unleash Wind buff.

bracket_85:
  available: + SPIRITWALKERS_GRACE (85; rare-use for Enh)
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
| 1 | Maelstrom Weapon proc-aura ID | 53817 (UNVERIFIED) | Spell.dbc / aura ID lookup |
| 2 | Maelstrom Weapon proc rate per swing | ~10% (talent-rank scaled) | Talent.dbc 51530 |
| 3 | MW stack duration | 30s | Spell.dbc 53817 |
| 4 | Stormstrike-buff stack mechanic (per-cast vs per-shock) | UNVERIFIED | Spell.dbc 17364 |
| 5 | Stormstrike-buff duration | 12s | Spell.dbc 17364 |
| 6 | Lava Lash + FT MH bonus % | 30% | Spell.dbc 60103 |
| 7 | Lava Lash spread radius | 12y | Spell.dbc 60103 |
| 8 | Lava Lash spread target cap | 4 | Spell.dbc 60103 |
| 9 | Searing Flames refresh-FS-duration mechanic | yes (UNVERIFIED whether cap-extending) | Talent.dbc |
| 10 | Searing Totem pulse interval | ~1.5s | Spell.dbc 3599 |
| 11 | Searing Totem range | 25y | Spell.dbc 3599 |
| 12 | Magma Totem pulse radius | 8y | Spell.dbc 8190 |
| 13 | Mental Quickness AP→SP % | 50% | Talent.dbc 30814 |
| 14 | Static Shock proc rate | 15-45% (talent rank) | Talent.dbc 51527 |
| 15 | Lightning Shield max charges | 9 | Spell.dbc 324 |
| 16 | Enhanced Elements Mastery base + per-point | 20.0% + 2.5%/pt | Mastery DBC |
| 17 | Windfury OH PPM rate | UNVERIFIED | Spell.dbc 8232 |
| 18 | Flametongue MH on-hit dmg formula | TBD | Spell.dbc 8024 |
| 19 | Feral Spirit Spirit Wolves dmg + duration | 30s wolves | Spell.dbc 51533 |
| 20 | Fire Elemental Totem Fire Elemental dmg | TBD | Spell.dbc 2894 |
| 21 | Lava Lash level (30 vs 26 vs other) | 30 (UNVERIFIED) | Spell trainer table |
| 22 | Fire Elemental Totem level (66 vs other) | 66 (UNVERIFIED — Cata may have shifted) | Spell trainer table |
| 23 | Unleash Wind buff +50% melee atk speed | 50% | Spell.dbc 73681 |
| 24 | Unleash Wind buff duration | 8s | Spell.dbc 73681 |
| 25 | Glyph of Stormstrike effect | TBD | Glyph.dbc |
| 26 | Glyph of Lava Lash effect | TBD | Glyph.dbc |
| 27 | Glyph of Feral Spirit effect (heal pct?) | TBD | Glyph.dbc |
| 28 | Glyph of Stoneclaw Totem effect | shield self ~25% HP | Glyph.dbc |
| 29 | T13 2P set bonus (Enh) | TBD — Stormstrike-buff cleave OR Lava Lash CD reset | Item set DBC |
| 30 | T13 4P set bonus (Enh) | TBD | Item set DBC |
| 31 | Shamanistic Rage talent vs trainer | talent | Talent.dbc 30823 |
| 32 | MW stack persistence on combat exit | UNVERIFIED — may persist short window | Test in-game |
| 33 | Tremor Totem Cata mechanic (active vs passive) | active 1min CD | Spell.dbc 8143 |
| 34 | Wind Shear school-lockout duration | 2s | Spell.dbc 57994 |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kEnhShaman_Spells[]` |
| Maintenance pass | `static const MaintenanceTrigger kEnhShaman_Maintenance[]` |
| Rotation tiers | `static const RotationTier kEnhShaman_Rotation[]` |
| AoE rotation | `static const RotationTier kEnhShaman_RotationAoE[]` |
| Cooldown Profile | `static const CooldownTrigger kEnhShaman_Cooldowns[]` |
| Mana + Maelstrom Weapon | dual-resource state (mana_pct, mw_stacks 0-5) |
| Maelstrom Weapon proc | aura listener + stack count |
| Static Shock proc | Lightning Shield charge tracker |
| Stormstrike-buff | aura listener + remaining ms |
| Searing Flames stacks | per-target tracker (5-stack max) |
| Shock CD | shared 6s timer across FS/ES/FrS |
| Totem state | per-slot (Fire/Earth/Water/Air) active totem tracker |
| Weapon imbue state | MH / OH imbue aura listeners |
| Leveling rotation | `LevelBracketRotation kEnhShaman_Leveling[]` |

**Key new helper APIs:**
- `MaelstromStacks(bot)` — read 0-5 (shared infra w/ resource trackers).
- `LightningShieldCharges(bot)` — read 0-9 (UNVERIFIED max).
- `StormstrikeBuffActive(bot)` — aura listener.
- `FlameShockStacksOnTarget(target)` — Searing Flames tracker.
- `IsAoEMode(bot)` — `enemies_within_12y_self >= 3` (Lava Lash spread radius).
- `IsTotemActive(slot, totemId)` — totem-slot tracker.
- `IsFlameShockActiveAndExpiring(target, threshold_ms)` — refresh-window check.
- `WeaponImbueActive(bot, slot)` — MH / OH imbue tracker (extends existing aura infra).

**Enh-specific patterns NEW for codebase:**
1. **Dual rotation engines** — melee CD priority + Maelstrom-stack-driven spell cast pulse run in parallel. Rotation tier ordering balances both.
2. **AP→SP conversion (Mental Quickness)** — Enh's Lightning Bolt damage from AP, not SP. Affects gear scoring (already an Agility-melee class profile).
3. **Weapon imbues** — Cata classes (DK, Rogue, Shaman) all use weapon imbues; Enh is the most aggressive (FT MH + WF OH always-on). Bot needs imbue-state tracking + recast on expire.
4. **Totem cooldown stacking** — Searing Totem (1min) + Magma Totem (replaces) + Fire Elemental (replaces both) all share the Fire-totem slot. Slot tracking required.
5. **Shock-CD shared** — like Resto Shaman, all shock spells share one 6s CD. Existing infra applies.

Cross-reference to `docs/specs/elemental-shaman.md` for shared mechanics (Lightning Shield, Hex, Wind Shear, Heroism, totems), `docs/specs/resto-shaman.md` for shared utility (Mana Spring, Tremor, Cleanse), and `docs/roles/melee-dps.md` for melee positioning patterns.
