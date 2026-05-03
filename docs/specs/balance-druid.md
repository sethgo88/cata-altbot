# Balance Druid — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide.

**Numerical data** verified against Wowhead's Cata 4.3.4 archive and Warcraft Wiki. Items tagged `UNVERIFIED` need DBC verification.

**Strategy content** reconciled across 10 community sources. See `docs/research/balance-druid-guide-survey.md`.

**SPEC-CHOICE RATIONALE.** Balance is the only ranged-DPS Druid spec; Feral is melee/tank, Resto is healer (already documented). **A-tier** in Dragon Soul. This is the **fifth ranged-DPS spec** in the project.

---

## Identity

```yaml
spec: balance_druid
class: druid
role: RANGED_DPS
resource_model: mana + eclipse_bar (-100 Lunar to +100 Solar)
defining_mechanic: Eclipse bar swing — Wrath pushes Solar, Starfire pushes Lunar; Eclipse buff +30% damage to corresponding school
defining_buffs:
  - Moonkin_Form               # +15% magic damage; +10% Spell Crit aura (Moonkin Aura)
  - Mark_of_the_Wild
  - Eclipse_Lunar              # +30% Arcane (Starfire/Moonfire/Starsurge), 15s
  - Eclipse_Solar              # +30% Nature (Wrath/Insect Swarm/Hurricane/Sunfire), 15s
  - Natures_Grace              # +15% haste 15s on Eclipse trigger
  - Lunar_Shower               # +15/+30/+45% Moonfire/Sunfire damage stacked from instant casts
defining_procs:
  - Shooting_Stars             # 2/4% per Moonfire/Sunfire tick crit — instant Starsurge
  - Euphoria                   # +1 Eclipse energy on non-Eclipse Wrath/Starfire + 12% mana on Eclipse trigger
mastery: Total_Eclipse         # +2%/pt to Eclipse damage modifier (16% baseline)
key_cooldowns:
  - Force_of_Nature            # 3min, 3 Treant pet summon, 30s
  - Starfall                   # 90s (60s glyph), 10s aura, 20 stars
  - Innervate                  # 3min, 20% mana over 10s
  - Typhoon                    # 20s, knockback (manual command-only auto)
  - Solar_Beam                 # 1min, 10y AoE silence, lvl 85 talent
  - Rebirth                    # 10min combat-rez
ranged: 40
notes: |
  Balance Druid in 4.3.4 plays as an Eclipse-cycle ranged caster. Bar swings
  -100 (Lunar) to +100 (Solar). Wrath +13 toward Solar; Starfire -20 toward
  Lunar. Reaching extreme triggers 15s Eclipse buff (+30% to corresponding
  school's direct damage). Starsurge on CD (15s) gives +15 Eclipse energy
  in current direction. Shooting Stars proc on Moonfire/Sunfire tick crit
  → instant Starsurge.

  DoT priority: Moonfire under Lunar Eclipse, Sunfire under Solar Eclipse
  (mutually exclusive — Solar Eclipse converts Moonfire→Sunfire). Insect
  Swarm always-on Nature DoT. Starfall + Force of Nature on CD.

  Pre-Moonkin Form leveling (under L20): Wrath + Moonfire only.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  CURRENT_DPS_TARGET:
  ENEMY_CASTING_INTERRUPTIBLE:
  AOE_CLUSTER:                  # ≥4 enemies within 8y of target (Hurricane/Detonate)
  SECONDARY_DOT_TARGET:
```

---

## Spell Catalog

```yaml
spells:
  # ─────── Eclipse-cycle filler ───────

  WRATH:
    id: 5176
    mana_pct: 9
    cast_time_ms: 2000              # haste-affected
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: nature
    target_type: enemy
    eclipse_energy: +13             # +14 with Euphoria 2/2 outside Eclipse
    notes: Solar-favored; cast during Solar Eclipse for +30% damage.

  STARFIRE:
    id: 2912
    mana_pct: 11
    cast_time_ms: 2700              # haste-affected
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: arcane
    target_type: enemy
    eclipse_energy: -20             # -21 with Euphoria 2/2 outside Eclipse
    notes: Lunar-favored; cast during Lunar Eclipse for +30% damage.

  STARSURGE:
    id: 78674
    mana_pct: 11
    cast_time_ms: 2000              # instant on Shooting Stars proc
    cooldown_ms: 15000
    on_gcd: true
    range: 40
    school: arcane_or_nature        # picks higher-damage school based on Eclipse
    target_type: enemy
    eclipse_energy: +15             # in current bar direction
    notes: |
      Cast on CD; Shooting Stars proc makes instant + ignores CD. Awards
      +15 Eclipse energy in current movement direction; defaults Lunar at zero.

  # ─────── DoTs ───────

  MOONFIRE:
    id: 8921
    mana_pct: 18
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: arcane
    target_type: enemy
    duration_ms: 18000              # base; Glyph of Starfire extends +9s under Lunar
    tick_ms: 3000
    notes: Apply during Lunar Eclipse. Mutually exclusive with Sunfire.

  SUNFIRE:
    id: 93402
    mana_pct: 18
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: nature
    target_type: enemy
    duration_ms: 18000
    tick_ms: 3000
    requires_talent: true
    notes: Solar Eclipse converts Moonfire→Sunfire. Mutually exclusive with Moonfire.

  INSECT_SWARM:
    id: 5570
    mana_pct: 8
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: nature
    target_type: enemy
    duration_ms: 14000
    tick_ms: 2000
    notes: |
      Maintained on primary target. NO -hit debuff in 4.3.4 (legacy Wrath
      mechanic removed). Pure DoT.

  # ─────── AoE ───────

  HURRICANE:
    id: 16914
    mana_pct: 81                    # heavy — 8% per second × 10s
    cast_time_ms: 10000             # 10s channel
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: ground_targeted
    radius: 10
    notes: Solar-favored AoE channel. Cast during Solar Eclipse on dense AoE.

  STARFALL:
    id: 48505
    mana_pct: 35
    cast_time_ms: 0
    cooldown_ms: 90000              # 60s with Glyph
    on_gcd: false
    range: 0
    target_type: self_aura
    duration_ms: 10000
    requires_talent: true
    notes: 20 stars over 10s, random nearby enemies. Free DPS; cast on CD.

  WILD_MUSHROOM:
    id: 88747
    mana_pct: 11
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ground_targeted
    max_charges: 3
    notes: Place mushroom; 3 charges. Manual command for AoE setup.

  WILD_MUSHROOM_DETONATE:
    id: 88751
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: detonate_all_mushrooms
    notes: AoE damage at all placed mushrooms.

  # ─────── Cooldowns / utility ───────

  FORCE_OF_NATURE:
    id: 33831
    mana_pct: 12
    cast_time_ms: 0
    cooldown_ms: 180000             # 3 min
    on_gcd: true
    range: 30
    target_type: ground_targeted
    duration_ms: 30000
    requires_talent: true
    notes: 3 Treant pets, 30s. Cast on CD. NOT snapshot (assumed).

  INNERVATE:
    id: 29166
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 180000             # 3 min
    on_gcd: false
    range: 30
    target_type: self_or_ally
    duration_ms: 10000
    notes: 20% mana over 10s. Glyph: +50% to caster on ally-cast.

  TYPHOON:
    id: 61384
    mana_pct: 12
    cast_time_ms: 0
    cooldown_ms: 20000
    on_gcd: true
    range: 0
    target_type: cone_30y_frontal
    requires_talent: true
    notes: AoE knockback + 6s daze. Glyph removes knockback. Manual command-only.

  SOLAR_BEAM:
    id: 78675
    mana_pct: 39
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: true
    range: 40
    target_type: ground_targeted
    duration_ms: 10000
    radius: 10
    requires_talent: true
    requires_level: 85
    notes: AoE silence at target ground. Auto-fire on caster targets + manual command.

  FAERIE_FIRE:
    id: 770
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 6000
    on_gcd: true
    range: 30
    target_type: enemy
    duration_ms: 300000              # 5 min
    notes: -12% armor + reveals stealth. Apply if missing.

  MOONKIN_FORM:
    id: 24858
    mana_pct: 13                    # shapeshift cost
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    requires_talent: true
    notes: |
      +15% magic damage; +10% Spell Crit aura (Moonkin Aura) — raid buff.
      Always-on. Bot enters once at session start.

  REBIRTH:
    id: 20484
    mana_pct: 60
    cast_time_ms: 2000
    cooldown_ms: 600000             # 10 min
    on_gcd: true
    range: 30
    target_type: corpse
    notes: combat-rez.

  REVIVE:
    id: 50769
    mana_pct: 60
    cast_time_ms: 10000
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: corpse
    notes: out-of-combat resurrect.

  MARK_OF_THE_WILD:
    id: 1126
    mana_pct: 39
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: party_or_raid
    duration_ms: 3600000
    notes: stat raid buff.

  REMOVE_CORRUPTION:
    id: 2782
    mana_pct: 13
    cast_time_ms: 0
    cooldown_ms: 8000
    on_gcd: true
    range: 40
    target_type: ally
    notes: removes 1 poison + 1 curse.

  HIBERNATE:
    id: 2637
    mana_pct: 8
    cast_time_ms: 1500
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: enemy_beast_dragonkin
    duration_ms: 40000
    notes: CC. Manual command only.

  CYCLONE:
    id: 33786
    mana_pct: 14
    cast_time_ms: 1700
    cooldown_ms: 0
    on_gcd: true
    range: 20
    target_type: enemy
    duration_ms: 6000
    notes: CC. Manual command only.

  ENTANGLING_ROOTS:
    id: 339
    mana_pct: 10
    cast_time_ms: 1700
    cooldown_ms: 0
    on_gcd: true
    range: 35
    target_type: enemy
    duration_ms: 30000
    notes: root. Manual command only.

  SOOTHE:
    id: 2908
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: true
    range: 40
    target_type: enemy
    notes: dispels enrage. Manual + auto when target.has_enrage.

  THORNS:
    id: 467
    mana_pct: 8
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: true
    range: 40
    target_type: ally
    duration_ms: 8000
    notes: rarely valuable in PvE; situational.
```

---

## Maintenance pass

```yaml
maintenance:
  - name: Moonkin Form (always)
    when:
      - has_talent(MOONKIN_FORM)
      - aura.missing(SELF, MOONKIN_FORM)
    cast: MOONKIN_FORM

  - name: Mark of the Wild on raid
    when:
      - NOT in_combat
      - any_raid_member_missing_buff(MARK_OF_THE_WILD)
    cast: MARK_OF_THE_WILD on missing_member

  - name: Faerie Fire on target
    when:
      - in_combat
      - target_resolves(CURRENT_DPS_TARGET)
      - aura.missing(CURRENT_DPS_TARGET, FAERIE_FIRE) OR aura.missing(CURRENT_DPS_TARGET, ARMOR_DEBUFF_8PCT)
      - spell.off_cooldown(FAERIE_FIRE)
    cast: FAERIE_FIRE on CURRENT_DPS_TARGET
    notes: skip if Sunder Armor / Expose Armor already applied.
```

---

## Rotation — Priority List (Single-Target)

```yaml
rotation:
  # ───────── Tier 1: Shooting Stars proc ─────────
  - tier: 1
    name: Starsurge — Shooting Stars instant proc
    when:
      - aura.active(SELF, SHOOTING_STARS_PROC)
      - target_in_range(40)
    cast: STARSURGE on CURRENT_DPS_TARGET
    notes: Free instant cast; bypasses 15s CD.

  # ───────── Tier 2: Starsurge on CD ─────────
  - tier: 2
    name: Starsurge on CD
    when:
      - spell.off_cooldown(STARSURGE)
      - target_in_range(40)
    cast: STARSURGE on CURRENT_DPS_TARGET
    notes: +15 Eclipse energy in current direction.

  # ───────── Tier 3: Insect Swarm maintenance ─────────
  - tier: 3
    name: Insect Swarm — apply/refresh
    when:
      - target_in_range(40)
      - any of:
          - aura.missing(CURRENT_DPS_TARGET, INSECT_SWARM)
          - aura.expires_in_ms(CURRENT_DPS_TARGET, INSECT_SWARM) < 2000
    cast: INSECT_SWARM on CURRENT_DPS_TARGET
    notes: Refresh during Solar Eclipse for snapshot.

  # ───────── Tier 4: Moonfire/Sunfire maintenance (Eclipse-conditional) ─────────
  - tier: 4
    name: Moonfire — apply/refresh during Lunar Eclipse
    when:
      - aura.active(SELF, ECLIPSE_LUNAR)
      - target_in_range(40)
      - any of:
          - aura.missing(CURRENT_DPS_TARGET, MOONFIRE) AND aura.missing(CURRENT_DPS_TARGET, SUNFIRE)
          - aura.expires_in_ms(CURRENT_DPS_TARGET, MOONFIRE) < 2000
    cast: MOONFIRE on CURRENT_DPS_TARGET

  - tier: 4b
    name: Sunfire — apply/refresh during Solar Eclipse
    when:
      - aura.active(SELF, ECLIPSE_SOLAR)
      - has_talent(SUNFIRE)
      - target_in_range(40)
      - any of:
          - aura.missing(CURRENT_DPS_TARGET, SUNFIRE) AND aura.missing(CURRENT_DPS_TARGET, MOONFIRE)
          - aura.expires_in_ms(CURRENT_DPS_TARGET, SUNFIRE) < 2000
    cast: SUNFIRE on CURRENT_DPS_TARGET

  # ───────── Tier 5: Movement filler — Moonfire/Sunfire (Lunar Shower) ─────────
  - tier: 5
    name: Moonfire/Sunfire — movement filler with Lunar Shower stacks
    when:
      - in_movement_mode
      - has_talent(LUNAR_SHOWER)
      - lunar_shower_stacks < 3
      - target_in_range(40)
    cast: |
      MOONFIRE if Lunar Eclipse (or no Eclipse) else SUNFIRE

  # ───────── Tier 6: Eclipse filler — Wrath (Solar) ─────────
  - tier: 6
    name: Wrath — Solar filler / Solar push
    when:
      - any of:
          - aura.active(SELF, ECLIPSE_SOLAR)
          - eclipse_bar_direction == TOWARD_SOLAR
      - target_in_range(40)
      - SELF.mana_pct > 25
    cast: WRATH on CURRENT_DPS_TARGET

  # ───────── Tier 7: Eclipse filler — Starfire (Lunar) ─────────
  - tier: 7
    name: Starfire — Lunar filler / Lunar push
    when:
      - any of:
          - aura.active(SELF, ECLIPSE_LUNAR)
          - eclipse_bar_direction == TOWARD_LUNAR
      - target_in_range(40)
      - SELF.mana_pct > 25
    cast: STARFIRE on CURRENT_DPS_TARGET
```

---

## AoE Mode

Activated by `enemies_within_8y_of(CURRENT_DPS_TARGET) >= 4`. Hysteresis: exit `<3 sustained for 5s`.

```yaml
rotation_aoe:
  - tier: 1: Shooting Stars proc -> Starsurge
  - tier: 2: Starfall off_CD -> Starfall
  - tier: 3: Force of Nature off_CD -> Force of Nature
  - tier: 4: aura.active(SELF, ECLIPSE_SOLAR) AND mana_pct > 50 -> Hurricane channel
  - tier: 5: enemies_in_8y >= 5 AND wild_mushroom_charges == 3 -> Wild Mushroom: Detonate (manual setup required)
  - tier: 6: multi-DoT 3-4 secondary targets if living >= 10s
  - tier: 7: continue Eclipse rotation (Wrath/Starfire)
notes: Wild Mushroom requires pre-placement; bot does NOT auto-place mushrooms (manual command).
```

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: FORCE_OF_NATURE
    when:
      - in_combat
      - target_in_range(30)
      - spell.off_cooldown(FORCE_OF_NATURE)
    rationale: 3-min CD; cast on CD. NOT snapshot — fire whenever ready.

  - spell: STARFALL
    when:
      - in_combat
      - spell.off_cooldown(STARFALL)
      - any of:
          - target.is_boss
          - aoe_mode_active
    rationale: 90s CD (60s glyph); free DPS; cast on CD.

  - spell: INNERVATE
    when:
      - in_combat
      - SELF.mana_pct < 40
      - spell.off_cooldown(INNERVATE)
    rationale: self-cast at 40% mana. Manual command for ally-cast.

  - spell: SOLAR_BEAM
    when:
      - target.casting AND target.cast_remaining > 1000
      - spell.off_cooldown(SOLAR_BEAM)
      - level >= 85
    rationale: AoE silence on caster targets.

  - spell: SOOTHE
    when:
      - target.has_enrage_dispellable
      - spell.off_cooldown(SOOTHE)
    rationale: enrage dispel.

  - spell: REBIRTH
    when:
      - in_combat
      - any_dead_party_member
      - spell.off_cooldown(REBIRTH)
    target_priority: tank > healer > caster_dps > melee_dps
    rationale: combat-rez.
```

---

## Proc Reactions

```yaml
procs:
  SHOOTING_STARS:
    talent_id: 93399                # UNVERIFIED
    triggered_by: Moonfire/Sunfire tick crit
    proc_chance_pct: 4              # at 2/2
    effect: next Starsurge instant + ignores CD
    duration_ms: 12000
    ai_behavior: Tier 1 priority — consume immediately.

  EUPHORIA:
    talent_passive: true
    triggered_by_outside_eclipse: Wrath/Starfire cast (+1 Eclipse energy)
    triggered_by_eclipse_trigger: 12% mana restored
    ai_behavior: passive — implicit benefit.

  NATURES_GRACE:
    talent_passive: true
    triggered_by: Eclipse trigger
    effect: +15% spell haste 15s
    ai_behavior: passive.

  LUNAR_SHOWER:
    talent_passive: true
    triggered_by: Moonfire/Sunfire cast
    max_stacks: 3
    effect_per_stack_pct: 15        # +15%/+30%/+45%
    duration_ms: 3000
    ai_behavior: |
      Tier 5 movement filler stacks via Moonfire/Sunfire while moving;
      bot tracks stacks and refreshes before expiry.

  ECLIPSE_TRIGGER:
    triggered_by: bar reaching ±100
    effect: applies Lunar or Solar Eclipse buff (+30%) for 15s
    ai_behavior: passive — bar tracking handles.

  MASTERY_TOTAL_ECLIPSE:
    mastery_passive: true
    base_pct: 16
    per_point_pct: 2
    effect: scales Eclipse damage modifier
    ai_behavior: passive — informs reforge.
```

---

## Mana Management

```yaml
mana:
  thresholds:
    drink_floor_pct: 30
    drink_resume_pct: 95
    innervate_floor_pct: 40
    conserve_mode_pct: 50
    crisis_mode_pct: 25

  conserve_behavior:
    when: SELF.mana_pct < 50
    rules:
      - skip Hurricane channel even in AoE-mode
      - keep DoT maintenance + Starsurge on CD
      - prefer Wrath over Starfire (cheaper)

  crisis_behavior:
    when: SELF.mana_pct < 25
    rules:
      - fire Innervate immediately
      - skip Force of Nature recast
      - drop AoE-mode (single-target rotation only)

  out_of_combat:
    drink_when:
      - not in_combat
      - SELF.mana_pct < drink_floor_pct
      - no enemies within 40y for 5000ms
    eat_when: out_of_combat AND SELF.hp_pct < 80

eclipse_bar:
  range: [-100, 100]
  starting_value: 0
  starting_direction_at_engage: TOWARD_SOLAR
  generation_table:
    WRATH: +13 (+14 with Euphoria 2/2 outside Eclipse)
    STARFIRE: -20 (-21 with Euphoria 2/2 outside Eclipse)
    STARSURGE: +15 in current direction
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
  - tier 1: SELF.hp_pct < 35 -> retreat OR PW:Shield equivalent (no shield as druid)
  - tier 2: aura.missing(target, MOONFIRE) -> MOONFIRE
  - tier 3: aura.missing(target, INSECT_SWARM) -> INSECT_SWARM
  - tier 4: off_cd(STARSURGE) -> STARSURGE
  - tier 5: aura.active(SELF, ECLIPSE_SOLAR) -> WRATH
  - tier 6: aura.active(SELF, ECLIPSE_LUNAR) -> STARFIRE
  - tier 7: bar moving to Solar -> WRATH
  - tier 8: bar moving to Lunar -> STARFIRE
notes: Pre-Moonkin (under L20-ish), bot uses Wrath + Moonfire only.
```

### Travel & out-of-combat

```yaml
travel:
  travel_form:
    use_when:
      - not in_combat
      - distance_to_destination > 30 AND distance_to_destination < 80
    cast: TRAVEL_FORM
    notes: druid travel form (+40% run speed); breaks on combat. Lvl 16.

  mount:
    use_when:
      - not in_combat
      - distance_to_destination >= 80

ooc_maintenance:
  - check: aura.missing(SELF, MOONKIN_FORM) AND has_talent -> MOONKIN_FORM
  - check: aura.missing(SELF, MARK_OF_THE_WILD) -> MARK_OF_THE_WILD
  - check: SELF.mana_pct < 80 AND no_enemies_within(40) -> drink
  - check: SELF.hp_pct < 95 AND no_enemies_within(40) -> eat
```

### Idle

```yaml
idle:
  primary_action: follow master
  secondary_actions:
    - maintain Moonkin Form
    - maintain Mark of the Wild
  do_not:
    - do not cast offensive spells while idle
```

### Disengage

```yaml
disengage:
  entangling_roots:
    use_when:
      - melee_count_within_5y >= 1
      - SELF.hp_pct < 50
    cast: ENTANGLING_ROOTS on adjacent_enemy
```

---

## Leveling rotation

Pure Balance from L10. Pre-Moonkin Form (under L20 talent reach), Wrath + Moonfire only.

### Spec recommendation

```yaml
spec_choice:
  for_leveling_dungeon_dps_bot:
    spec: Balance
    talent_split: 31 Balance / 2 Feral / 8 Restoration
    talent_picks_balance: Moonfury 3/3 + Genesis 3/3 + Moonglow 3/3 + Nature's Grace 3/3 + Starlight Wrath 3/3 + Earth and Moon 1/1 + Lunar Shower 3/3 + Euphoria 2/2 + Shooting Stars 2/2 + Force of Nature 1/1 + Sunfire 1/1 + Solar Beam 1/1 + Typhoon 1/1 + Starfall 1/1 + Owlkin Frenzy 3/3 + Balance of Power 2/2
    talent_picks_feral: Furor 2/3 (+Int)
    talent_picks_resto: Heart of the Wild 3/3 + Blessing of the Grove 2/2 + Natural Shapeshifter 3/3 (Feral path alternate)
    rationale: 31/2/8 Heart-of-the-Wild path per Icy Veins consensus.
```

### Spell unlock table

```yaml
spell_unlocks:
  WRATH:                   {level: 1,  spell_id: 5176}
  MARK_OF_THE_WILD:        {level: 1,  spell_id: 1126}
  MOONFIRE:                {level: 4,  spell_id: 8921}
  REJUVENATION:            {level: 4,  spell_id: 774}
  HEALING_TOUCH:           {level: 6,  spell_id: 5185}
  ENTANGLING_ROOTS:        {level: 8,  spell_id: 339}
  STARFIRE:                {level: 10, spell_id: 2912}
  REGROWTH:                {level: 12, spell_id: 8936}
  TRAVEL_FORM:             {level: 16, spell_id: 783}
  REBIRTH:                 {level: 18, spell_id: 20484}
  HIBERNATE:               {level: 20, spell_id: 2637}
  REMOVE_CORRUPTION:       {level: 20, spell_id: 2782}
  THORNS:                  {level: 22, spell_id: 467}
  CYCLONE:                 {level: 24, spell_id: 33786}
  INNERVATE:               {level: 28, spell_id: 29166}
  INSECT_SWARM:            {level: 30, spell_id: 5570}
  FAERIE_FIRE:             {level: 14, spell_id: 770}
  HURRICANE:               {level: 42, spell_id: 16914}
  TRANQUILITY:             {level: 60, spell_id: 740}
  STARSURGE:               {level: 78, spell_id: 78674}
  WILD_MUSHROOM:           {level: 85, spell_id: 88747}
  WILD_MUSHROOM_DETONATE:  {level: 85, spell_id: 88751}
  # Talent-gated:
  MOONKIN_FORM:            {requires_talent: true, spell_id: 24858}
  SUNFIRE:                 {requires_talent: true, spell_id: 93402}
  EUPHORIA:                {requires_talent: true}
  NATURES_GRACE:           {requires_talent: true}
  SHOOTING_STARS:          {requires_talent: true}
  LUNAR_SHOWER:            {requires_talent: true}
  FORCE_OF_NATURE:         {requires_talent: true, spell_id: 33831}
  STARFALL:                {requires_talent: true, spell_id: 48505}
  TYPHOON:                 {requires_talent: true, spell_id: 61384}
  SOLAR_BEAM:              {requires_talent: true, spell_id: 78675, level: 85}
  BALANCE_OF_POWER:        {requires_talent: true}
  EARTH_AND_MOON:          {requires_talent: true}
  STARLIGHT_WRATH:         {requires_talent: true}
  GENESIS:                 {requires_talent: true}
  MOONFURY:                {requires_talent: true}
  # Cross-tree:
  FUROR:                   {requires_talent: true, tree: feral}
  HEART_OF_THE_WILD:       {requires_talent: true, tree: feral}
  # Mastery / passives:
  MASTERY_TOTAL_ECLIPSE:   {passive_mastery: true, base_pct: 16, per_point_pct: 2}
  ECLIPSE_BAR:             {passive_resource: true, range: [-100, 100], default: 0}
```

### Leveling brackets

```yaml
bracket_1_3:
  available: WRATH, MARK_OF_THE_WILD
  rotation: WRATH spam

bracket_4_9:
  available: + MOONFIRE (4), + HEALING_TOUCH (6), + ENTANGLING_ROOTS (8)
  rotation: MOONFIRE apply -> WRATH filler

bracket_10_15:
  available: + STARFIRE (10)
  rotation:
    - tier 1: aura.missing(target, MOONFIRE) -> MOONFIRE
    - tier 2: 2-spell alternate WRATH/STARFIRE based on stationary

bracket_16_19:
  available: + TRAVEL_FORM (16), + FAERIE_FIRE (14)
  notes: travel form for OOC movement.

bracket_20_29:
  available: + MOONKIN_FORM (talent ~L20), + REBIRTH (18), + HIBERNATE (20), + INNERVATE (28)
  rotation_change: Moonkin Form unlocked; +15% magic damage. Eclipse bar-aware rotation begins.

bracket_30_41:
  available: + INSECT_SWARM (30), + Force of Nature (talent), + Starfall (talent), + Typhoon (talent)
  rotation_addition: Insect Swarm DoT maintenance.

bracket_42_77:
  available: + HURRICANE (42)
  rotation_addition: AoE-mode entry on 4+ enemies.

bracket_78_84:
  available: + STARSURGE (78), + Sunfire (talent ~L78)
  rotation_change: Starsurge becomes Tier 1-2 priority; Sunfire enters Solar Eclipse rotation.

bracket_85:
  available: + WILD_MUSHROOM (85), + DETONATE (85), + SOLAR_BEAM (85 talent)
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
| 1 | Wrath Eclipse energy | +13 | Spell.dbc 5176 |
| 2 | Starfire Eclipse energy | -20 | Spell.dbc 2912 |
| 3 | Starsurge Eclipse energy | +15 (current direction) | Spell.dbc 78674 |
| 4 | Euphoria Eclipse-trigger mana % | 12% | Talent.dbc |
| 5 | Shooting Stars proc rate at 2/2 | 4% per Moonfire/Sunfire tick crit | Talent.dbc |
| 6 | Lunar Shower max stacks | 3 | Talent.dbc |
| 7 | Lunar Shower per-stack damage | 15%/30%/45% | Talent.dbc |
| 8 | Mastery Total Eclipse formula | 16% + 2%/pt | Mastery DBC |
| 9 | Sunfire spell ID | 93402 | Spell.dbc |
| 10 | Starsurge at-zero direction default | Lunar | Spell.dbc + edge case |
| 11 | Force of Nature snapshot | NOT snapshot (assumed) | Pet engine logic |
| 12 | Haste breakpoints (IS 8th tick at 12.5%, MF/SF 5th at 25%) | yes | Spell.dbc + haste model |
| 13 | Glyph of Starfire Moonfire-extension cap | +9s max | Glyph.dbc |
| 14 | Wild Mushroom: Detonate damage formula | TBD | Spell.dbc |
| 15 | Mark of the Wild Cata stat values | TBD | Spell.dbc |
| 16 | Nature's Grace haste % + duration | 15% / 15s | Talent.dbc |
| 17 | Balance of Power Spirit→Hit % at 2/2 | 100% | Talent.dbc |
| 18 | Solar Beam silence radius | 10y | Spell.dbc 78675 |
| 19 | Insect Swarm hit debuff | REMOVED in 4.3.4 | Spell.dbc 5570 |
| 20 | Volcanic Potion +1200 Int | yes | Item DBC |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kBalanceDruid_Spells[]` |
| Rotation tiers | `static const RotationTier kBalanceDruid_Rotation[]` |
| AoE rotation | `static const RotationTier kBalanceDruid_RotationAoE[]` |
| Cooldown Profile | `static const CooldownTrigger kBalanceDruid_Cooldowns[]` |
| Eclipse bar | `int8 eclipse_bar` (-100..+100) + `EclipseDirection` enum |
| Eclipse buff state | `EclipseBuff` enum (None/Lunar/Solar) + remaining ms |
| Lunar Shower stacks | `uint8 lunar_shower_stacks` (0-3) |
| Shooting Stars proc | aura listener |
| Moonkin Form state | `bool moonkin_active` |
| Mana / leveling | `ManaState` enum + threshold helpers |

**Key new helper APIs:**
- `EclipseBar(bot)` — read -100..+100.
- `EclipseDirection(bot)` — read TOWARD_LUNAR / TOWARD_SOLAR / NEUTRAL.
- `EclipseBuff(bot)` — read None / Lunar / Solar.
- `EclipseBuffRemainingMs(bot)`.
- `LunarShowerStacks(bot)` — read 0-3.
- `ShootingStarsActive(bot)` — proc state.
- `IsBalanceDoTActive(target)` — `aura.active(target, MOONFIRE) OR aura.active(target, SUNFIRE)`.
- `GetCorrectDoTForCurrentEclipse(bot)` — returns MOONFIRE or SUNFIRE based on Eclipse state.

Cross-reference to `docs/roles/ranged-dps.md` and prior ranged-DPS specs.
