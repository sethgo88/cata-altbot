# Frost Mage — 4.3.4 Skill Spec

This document is an **AI implementation spec**, not a player-facing strategy guide. The YAML blocks below are designed to lift cleanly into `static const` tables in a future `AltbotCombat.cpp`.

**Numerical data** (spell IDs, mana %, cast times, cooldowns, durations) verified against Wowhead's Cata 4.3.4 archive and Warcraft Wiki. Items the AI will need but that public archives don't pin down with confidence are tagged `UNVERIFIED`.

**Strategy content** reconciled across 11 community guides. See `docs/research/frost-mage-guide-survey.md` for the comparison table and which thresholds are consensus vs author-judgment.

**SPEC-CHOICE RATIONALE.** Frost is **F-tier raid DPS** in 4.3.4 Dragon Soul (Fire dominates). Project picks Frost on utility/leveling/CC grounds: permanent Water Elemental, Frost Nova + Polymorph + Ring of Frost CC kit, Counterspell + Improved Counterspell silence, leveling-friendly rotation from level 28 onward. Documented downside: bottom-tier for mythic raid parses. Future option: a sibling Fire spec doc for max-level raid play.

This is the **first ranged-DPS spec** in the project (5 healer specs preceded).

---

## Identity

```yaml
spec: frost_mage
class: mage
role: RANGED_DPS
resource_model: mana
defining_mechanic: Shatter combo (frozen target = ×3 crit chance)
defining_buffs:
  - Molten_Armor             # +3% spell crit (default)
  - Mage_Armor               # +3% mana per 5s (mana-stress mode)
  - Frost_Armor              # 15% phys reduction + chill on melee (defensive override)
  - Arcane_Brilliance        # raid stat buff
defining_procs:
  - Brain_Freeze             # 15% chance on Frost-with-chill — instant + free FFB
  - Fingers_of_Frost         # 20% chance on Frost-with-chill — 2 charges, 15s; "frozen" status for next Ice Lance/Deep Freeze
mastery: Frostburn           # 5% base + 2.5%/point bonus damage to frozen targets
key_cooldowns:
  - Icy_Veins                # 3 min — +20% spellcasting haste, 20s
  - Cold_Snap                # 8 min — resets all Frost-school CDs
  - Mirror_Image             # 3 min — 3 copies cast Frostbolts, threat reduction, 30s
  - Time_Warp                # 5 min — +30% raid haste, 40s (Bloodlust equiv)
  - Ice_Block                # 4 min — full immunity, 10s
  - Mage_Ward                # 30s — absorbs Fire/Frost/Arcane
  - Counterspell             # 24s — 7s school lockout interrupt
  - Polymorph                # 50s PVE CC
ranged: 40
notes: |
  Frost mage in 4.3.4 plays as a low-DPS but high-utility ranged caster.
  Rotation heartbeat: Frostbolt as filler generates Brain Freeze (15%) +
  Fingers of Frost (20%) procs; consume FoF on Deep Freeze (Shatter ×3 crit
  combo) for highest damage; consume Brain Freeze on Frostfire Bolt
  (preferably with FoF active for a triple-multiplier hit). Permanent
  Water Elemental tank-let with 25s Freeze that grants 2 FoF charges.

  CC kit (Counterspell, Polymorph, Frost Nova, Ring of Frost) is the bot's
  primary group-utility lane. Defaults are conservative: auto-fire
  Counterspell only when interrupt-rotation-position OR no other
  interrupter off-CD; Polymorph auto only on cc-marked targets in 5-man.
```

---

## Target taxonomy

```yaml
target_selectors:
  SELF:
  CURRENT_DPS_TARGET:           # bot's assigned damage target (master's victim by default)
  ENEMY_CASTING_INTERRUPTIBLE:   # nearest enemy <40y casting an interruptible spell
  CC_MARKER_POLY:                # raid-target-icon: moon (mage CC marker)
  CC_MARKER_SKULL:               # raid-target-icon: skull (kill priority)
  AOE_CLUSTER:                   # ≥3 enemies within 15y of CURRENT_DPS_TARGET
  PET_TARGET:                    # Water Elemental's auto-attack target (mirrors mage)
  MELEE_THREAT:                  # ≥1 enemy in 8y of bot
```

---

## Spell Catalog

```yaml
spells:
  # ─────── Filler & resource generation ───────

  FROSTBOLT:
    id: 116
    mana_pct: 13
    cast_time_ms: 1500           # haste-affected
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: frost
    target_type: enemy
    notes: filler. Generates Brain Freeze (15%) and FoF (20%) procs. Slows target 40% / 9s.

  ICE_LANCE:
    id: 30455
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: frost
    target_type: enemy
    notes: |
      Instant. Damage ×3 against frozen targets (Shatter combo). +15% damage
      with Fingers of Frost active (any rank).

  FROSTFIRE_BOLT:
    id: 44614
    mana_pct: 14                 # baseline; Brain Freeze proc makes it free
    cast_time_ms: 3000           # haste-affected; instant when Brain Freeze active
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: frostfire
    target_type: enemy
    duration_ms: 9000             # periodic damage component (UNVERIFIED — see DBC queue)
    notes: |
      Frost+Fire dual-school. Brain Freeze proc makes free + instant.
      Crits proc Ignite (Fire talent). With FoF active, gets Shatter ×3.

  # ─────── Burst & high-priority ───────

  DEEP_FREEZE:
    id: 44572
    mana_pct: 9
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: true
    range: 35
    school: frost
    target_type: enemy
    duration_ms: 5000             # 5s stun on stunnable targets
    requires_frozen_status: true
    notes: |
      Tier 1 priority. On stun-immune bosses deals 1065-1334 base + spellpower
      damage AND counts as frozen for Shatter. REQUIRES FoF or natural-freeze
      to activate. Reset by Cold Snap.

  FROSTFIRE_ORB:
    id: 92283                     # talent-converted from FLAME_ORB (82731)
    mana_pct: 6
    cast_time_ms: 0
    cooldown_ms: 60000
    on_gcd: true
    range: 40
    school: frostfire
    target_type: ground_line
    duration_ms: 15000
    notes: |
      Tier 3. Travels in straight line, damages enemies along path.
      Frostfire Orb (talent) converts to Frost school AND can proc FoF.
      Pre-talent (Flame Orb): Fire school, no FoF proc.

  ICY_VEINS:
    id: 12472
    mana_pct: 3
    cast_time_ms: 0
    cooldown_ms: 180000           # 3 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 20000
    notes: +20% spellcasting haste; immune to pushback. Primary DPS CD.

  COLD_SNAP:
    id: 11958
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 480000           # 8 min
    on_gcd: false
    range: 0
    target_type: self
    notes: |
      Resets cooldowns on: Icy Veins, Cone of Cold, Ice Block, Frost Nova,
      Deep Freeze. Aligned to Icy Veins for double-burn opener.

  MIRROR_IMAGE:
    id: 55342
    mana_pct: 1
    cast_time_ms: 0
    cooldown_ms: 180000           # 3 min (150s with Glyph of Mirror Image)
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 30000
    notes: |
      Summons 3 copies that cast Frostbolts. Threat-reducing while up.
      Pre-cast at T-2s before pull.

  TIME_WARP:
    id: 80353
    mana_pct: 4
    cast_time_ms: 0
    cooldown_ms: 300000           # 5 min
    on_gcd: false
    range: 0
    target_type: self_aura_raid
    duration_ms: 40000
    notes: |
      Bloodlust equivalent. Applies Temporal Displacement debuff for 10 min
      (can't benefit from Bloodlust/Heroism/Time Warp). Bot is Time Warp owner
      unless shaman/hunter present.

  # ─────── CC + utility ───────

  COUNTERSPELL:
    id: 2139
    mana_pct: 9
    cast_time_ms: 0
    cooldown_ms: 24000            # 20s with Cold as Ice talent (UNVERIFIED)
    on_gcd: false
    range: 40
    target_type: enemy_casting
    notes: |
      Interrupt + 7s school lockout (post-4.0.6). Improved Counterspell
      talent (2/2 Arcane) adds 4s silence on top.

  POLYMORPH:
    id: 118
    mana_pct: 7
    cast_time_ms: 1700
    cooldown_ms: 0
    on_gcd: true
    range: 35
    school: arcane
    target_type: enemy
    duration_ms: 50000            # PVE; 8s PvP
    notes: |
      Sheep CC. Auto only on cc-marked target in 5-man; OFF in raid (AoE breaks
      instantly). Manual command override for explicit re-CC.

  FROST_NOVA:
    id: 122
    mana_pct: 5
    cast_time_ms: 0
    cooldown_ms: 25000
    on_gcd: true
    range: 0
    target_type: pbaoe
    radius: 8
    duration_ms: 8000             # 8s root
    notes: |
      AoE root. Auto-fired when ≥2 melee enemies within 8y AND bot.hp_pct < 60.
      Reset by Cold Snap.

  CONE_OF_COLD:
    id: 120
    mana_pct: 12
    cast_time_ms: 0
    cooldown_ms: 10000
    on_gcd: true
    range: 0
    target_type: cone_12y_frontal
    duration_ms: 8000             # chill duration
    notes: AoE damage + slow. Reset by Cold Snap.

  RING_OF_FROST:
    id: 113724                    # UNVERIFIED — confirm Cata spell ID
    mana_pct: 14
    cast_time_ms: 2500
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: ground_targeted
    duration_ms: 10000
    notes: |
      AoE freeze ring at target ground; enemies entering are frozen 10s.
      Long cast time + breaks on damage. Manual command only.

  SPELLSTEAL:
    id: 30449
    mana_pct: 21
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 40
    target_type: enemy
    notes: steals 1 magic buff. Manual command only — encounter-specific.

  # ─────── Defensives ───────

  ICE_BLOCK:
    id: 45438
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 240000           # 4 min
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 10000
    notes: full immunity. Cancellable. Castable while stunned/feared.

  MAGE_WARD:
    id: 543
    mana_pct: 1
    cast_time_ms: 0
    cooldown_ms: 30000
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 30000
    notes: absorbs Fire/Frost/Arcane damage. Effectively always-on.

  MANA_SHIELD:
    id: 1463
    mana_pct: 2
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: false
    range: 0
    target_type: self
    duration_ms: 60000
    notes: |
      Drains mana 20% of damage absorbed. NOT primary defense — only when
      Mage Ward on CD AND target taking magic damage AND self.hp_pct < 50.

  EVOCATION:
    id: 12051
    mana_pct: 0
    cast_time_ms: 6000            # 6s channel (haste-affected)
    cooldown_ms: 240000
    on_gcd: true
    range: 0
    target_type: self_channel
    notes: restores ~60% mana over channel. Interruptible by melee.

  CONJURE_MANA_GEM:
    id: 759
    mana_pct: 18
    cast_time_ms: 3000
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    notes: |
      Out-of-combat conjure; in-combat use of Mana Gem charge restores ~6800
      mana with no cooldown shared with potions.

  # ─────── Buffs & maintenance ───────

  ARCANE_BRILLIANCE:
    id: 1459
    mana_pct: 7
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 30
    target_type: party_or_raid
    duration_ms: 3600000          # 1 hour
    notes: +6% spellpower + max-mana raid buff. Pre-pull only.

  MOLTEN_ARMOR:
    id: 30482
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000          # 30 min
    notes: +3% spell crit, +5% reduced critically-hit chance. Default armor.

  MAGE_ARMOR:
    id: 6117
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    notes: |
      +3% mana per 5s + 35% reduced harmful magic duration. Mana-stress mode
      — runtime swap when self.mana_pct < 30 sustained for 30s.

  FROST_ARMOR:
    id: 7302
    mana_pct: 0
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    target_type: self
    duration_ms: 1800000
    notes: |
      Cata redesign — 15% physical damage reduction + chill on melee.
      Reactive defensive when ≥2 melee enemies within 8y AND bot.hp_pct < 60.

  # ─────── Pet ───────

  WATER_ELEMENTAL:
    id: 31687                     # summon spell
    mana_pct: 16
    cast_time_ms: 1500
    cooldown_ms: 0                # Frost spec: permanent — no CD
    on_gcd: true
    range: 0
    target_type: self_pet_summon
    notes: |
      Permanent pet for Frost spec at level 10 (Cata redesign). Auto-attacks
      mage's target with Waterbolts. Has 25s CD Freeze ability.

  PET_FREEZE:
    id: 33395                     # pet ability spell ID
    mana_pct: 0                   # pet has no mana cost to player
    cast_time_ms: 1500
    cooldown_ms: 25000
    on_gcd: false                 # bot-controlled pet ability
    range: 30                     # pet places Freeze at target ground
    target_type: ground_targeted_via_pet
    radius: 8                     # AoE freeze radius
    duration_ms: 8000             # freeze duration
    notes: |
      Grants 2 Fingers of Frost charges to mage on cast. Off-CD usage in
      rotation when no FoF active.

  # ─────── Damage spells: AoE ───────

  BLIZZARD:
    id: 10
    mana_pct: 74                  # heavy mana cost
    cast_time_ms: 8000             # 8s channel
    cooldown_ms: 0
    on_gcd: true
    range: 35
    school: frost
    target_type: ground_targeted
    radius: 10
    notes: sustained-AoE channel. Primary AoE for sustained pulls.

  FLAMESTRIKE:
    id: 2120
    mana_pct: 27
    cast_time_ms: 2000
    cooldown_ms: 0
    on_gcd: true
    range: 40
    school: fire
    target_type: ground_targeted
    radius: 10
    duration_ms: 8000             # ground DoT
    notes: 4+ target sustained AoE; complementary to Blizzard.

  ARCANE_EXPLOSION:
    id: 1449
    mana_pct: 22
    cast_time_ms: 0
    cooldown_ms: 0
    on_gcd: true
    range: 0
    school: arcane
    target_type: pbaoe
    radius: 10
    notes: PBAoE when bot is being meleed by multiple enemies.
```

---

## Maintenance pass

```yaml
maintenance:
  - name: Molten Armor (default)
    when:
      - aura.missing(SELF, MOLTEN_ARMOR)
      - armor_mode == MOLTEN
    cast: MOLTEN_ARMOR

  - name: Mage Armor (mana-stress mode)
    when:
      - aura.missing(SELF, MAGE_ARMOR)
      - armor_mode == MAGE
    cast: MAGE_ARMOR

  - name: Frost Armor (reactive defensive)
    when:
      - aura.missing(SELF, FROST_ARMOR)
      - armor_mode == FROST
    cast: FROST_ARMOR

  - name: Arcane Brilliance pre-combat
    when:
      - NOT in_combat
      - any_raid_member_missing_buff(ARCANE_BRILLIANCE)
    cast: ARCANE_BRILLIANCE on missing_member

  - name: Mage Ward in combat
    when:
      - in_combat
      - aura.missing(SELF, MAGE_WARD) OR aura.expires_in_ms(SELF, MAGE_WARD) < 2000
      - spell.off_cooldown(MAGE_WARD)
    cast: MAGE_WARD on SELF

  - name: Water Elemental — keep summoned
    when:
      - has_talent(FROST_SPEC)
      - level >= 10
      - pet.dead OR pet.absent
      - any of:
          - NOT in_combat
          - in_combat AND fight_remaining_estimate > 30000
    cast: WATER_ELEMENTAL

  - name: Mirror Image pre-pull
    when:
      - NOT in_combat
      - boss_pull_imminent_in_2s
      - spell.off_cooldown(MIRROR_IMAGE)
    cast: MIRROR_IMAGE
```

---

## Rotation — Priority List

Single-target priority. Tiers evaluated top-down; first true tier fires.

```yaml
rotation:
  # ───────── Tier 1: Deep Freeze on FoF ─────────
  - tier: 1
    name: Deep Freeze — Shatter combo
    when:
      - fingers_of_frost.charges >= 1
      - spell.off_cooldown(DEEP_FREEZE)
      - target_in_range(35) AND target.alive
    cast: DEEP_FREEZE on CURRENT_DPS_TARGET
    notes: highest-damage Shatter combo. FoF-required (synthesizes frozen status on bosses).

  # ───────── Tier 2: FFB on Brain Freeze + FoF ─────────
  - tier: 2
    name: Frostfire Bolt — Brain Freeze + FoF triple-multiplier
    when:
      - aura.active(SELF, BRAIN_FREEZE)
      - fingers_of_frost.charges >= 1
      - target_in_range(40)
    cast: FROSTFIRE_BOLT on CURRENT_DPS_TARGET
    notes: |
      Brain Freeze + FoF + Shatter ×3 — best free-cast in the rotation.
      Burn FoF on this OVER Ice Lance when Brain Freeze is also up.

  # ───────── Tier 3: Frostfire Orb on CD ─────────
  - tier: 3
    name: Frostfire Orb on CD
    when:
      - spell.off_cooldown(FROSTFIRE_ORB)
      - has_talent(FROSTFIRE_ORB)        # talent-gated; pre-talent skip this tier
      - target_in_range(40)
    cast: FROSTFIRE_ORB at CURRENT_DPS_TARGET
    notes: 1-min CD line-AoE; can proc FoF.

  # ───────── Tier 4: Pet Freeze for FoF generation ─────────
  - tier: 4
    name: Water Elemental Freeze for FoF charges
    when:
      - pet.alive
      - spell.off_cooldown(PET_FREEZE)
      - fingers_of_frost.charges < 2
      - enemies_within_8y_of(CURRENT_DPS_TARGET) >= 1
    cast: PET_FREEZE at CURRENT_DPS_TARGET
    notes: grants 2 FoF charges; short CD (25s) makes this regular rotation.

  # ───────── Tier 5: FFB on Brain Freeze (no FoF) ─────────
  - tier: 5
    name: Frostfire Bolt — Brain Freeze burn before expiry
    when:
      - aura.active(SELF, BRAIN_FREEZE)
      - aura.expires_in_ms(SELF, BRAIN_FREEZE) < 5000
      - target_in_range(40)
    cast: FROSTFIRE_BOLT on CURRENT_DPS_TARGET
    notes: don't waste Brain Freeze; burn even without FoF.

  # ───────── Tier 6: Ice Lance on FoF ─────────
  - tier: 6
    name: Ice Lance — FoF burn
    when:
      - any of:
          - fingers_of_frost.charges == 2
          - fingers_of_frost.charges == 1 AND aura.expires_in_ms(SELF, FINGERS_OF_FROST) < 3000
      - target_in_range(40)
    cast: ICE_LANCE on CURRENT_DPS_TARGET
    notes: burn FoF charges via Ice Lance when no higher-tier action available.

  # ───────── Tier 7: Frostbolt filler ─────────
  - tier: 7
    name: Frostbolt — filler / proc generator
    when:
      - target_in_range(40)
      - SELF.mana_pct > 25
    cast: FROSTBOLT on CURRENT_DPS_TARGET
    notes: default cast; generates Brain Freeze + FoF procs.
```

---

## AoE Mode

Activated by `enemies_within_15y_of(CURRENT_DPS_TARGET) >= 3`. Hysteresis: exit after `<3 sustained for 5s`.

```yaml
rotation_aoe:
  - tier: 1: Frost Nova when ≥2 melee within 8y AND not_already_frozen
  - tier: 2: Cone of Cold when ≥2 enemies in 12y frontal cone AND off_CD
  - tier: 3: Frostfire Orb on cluster (talent + off_CD)
  - tier: 4: Flamestrike when ≥4 enemies sustained AND mana_pct > 50
  - tier: 5: Blizzard channel on cluster centroid AND mana_pct > 40
  - tier: 6: Arcane Explosion when ≥3 enemies in 10y of bot AND being meleed
notes: AoE rotation deprioritizes Brain Freeze / FoF tracking — they still proc, but Tier 1-6 dominate.
```

---

## Cooldown Profile

```yaml
cooldowns:
  - spell: ICY_VEINS
    when:
      - in_combat
      - spell.off_cooldown(ICY_VEINS)
      - any of:
          - boss_engagement OR fight_just_started
          - cooldown_window_aligned_with_bloodlust
    rationale: 3-min DPS CD. Fire on engagement; subsequent recasts on CD.

  - spell: COLD_SNAP
    when:
      - in_combat
      - aura.active(SELF, ICY_VEINS) AND icy_veins_remaining_ms < 5000
      - spell.off_cooldown(COLD_SNAP)
    rationale: |
      Aligned to Icy Veins for double-burn. Cold Snap resets Icy Veins
      (and all Frost CDs) — fire as Icy Veins is about to expire to
      recast Icy Veins immediately.

  - spell: MIRROR_IMAGE
    when:
      - any of:
          - boss_pull_imminent_in_2s          # pre-pull cast
          - in_combat AND spell.off_cooldown(MIRROR_IMAGE)
    rationale: 3-min CD; sustained-throughput contribution + threat dump.

  - spell: TIME_WARP
    when:
      - in_combat
      - level >= 85
      - time_warp_owner == true              # runtime flag, default true
      - spell.off_cooldown(TIME_WARP)
      - aura.missing(SELF, TEMPORAL_DISPLACEMENT)
      - boss_engagement OR raid_burn_phase
    rationale: |
      5-min CD raid haste. Bot only fires if it's the designated Bloodlust
      owner (no shaman/hunter in group).

  - spell: ICE_BLOCK
    when:
      - SELF.hp_pct < 20
      - no_other_save_ready_in_1500ms
      - spell.off_cooldown(ICE_BLOCK)
    rationale: 4-min CD panic button.

  - spell: EVOCATION
    when:
      - SELF.mana_pct < 40
      - no_active_emergency
      - NOT being_meleed                     # melee interrupts channel
      - spell.off_cooldown(EVOCATION)
    rationale: 6s channel mana CD; cancellable by melee.
```

---

## Proc Reactions

```yaml
procs:
  BRAIN_FREEZE:
    talent_id: 44546             # UNVERIFIED — Talent.dbc lookup
    triggered_by: Frost-school spell with chilling effect (mostly Frostbolt)
    proc_chance_pct: 15          # at 3/3
    duration_ms: 15000
    effect: next Frostfire Bolt is free + instant
    ai_behavior: |
      Tier 2 fires on Brain Freeze + FoF; Tier 5 burns Brain Freeze before
      expiry (< 5s remaining) without FoF.

  FINGERS_OF_FROST:
    talent_id: 44544             # UNVERIFIED
    triggered_by: Frost-school spell with chilling effect
    proc_chance_pct: 20          # at 3/3
    duration_ms: 15000
    charges: 2
    effect: next Ice Lance OR Deep Freeze acts as if target frozen; +15% Ice Lance damage
    ai_behavior: |
      Tier 1 (Deep Freeze) is highest priority. Tier 6 (Ice Lance) consumes
      remaining charges. Always burn before expiry.

  IGNITE:
    talent_id: 11119             # Fire tier 1, 8 Fire points get this
    triggered_by: Fire-school crit (FFB crit qualifies)
    effect: 40% of crit damage applied as DoT over 4s
    ai_behavior: |
      Passive — no rotation logic. Critical: validates the 8-Fire-points
      allocation. **UNVERIFIED — confirm FFB crit procs Ignite via DBC.**

  SHATTER:
    talent_id: 11170             # Frost tier 1 — passive multiplier
    effect: ×3 crit chance against frozen targets (capped at 100%)
    ai_behavior: passive — drives target-state preference for FoF/freeze.

  FROSTBURN_MASTERY:
    mastery_passive: true
    effect: 5% base + 2.5% per mastery point bonus damage to frozen targets
    ai_behavior: passive — biases rotation toward maintaining frozen status via FoF.
```

---

## Mana Management

```yaml
mana:
  thresholds:
    drink_floor_pct: 30
    drink_resume_pct: 95
    evocation_floor_pct: 40
    mana_gem_floor_pct: 60
    armor_swap_to_mage_floor_pct: 30
    armor_swap_to_mage_sustained_ms: 30000
    conserve_mode_pct: 50
    crisis_mode_pct: 25

  conserve_behavior:
    when: SELF.mana_pct < 50
    rules:
      - skip Tier 7 Frostbolt filler unless target.hp_pct < 20    # finish-the-mob exception
      - skip Tier 4 Pet Freeze if FoF charges == 1 (don't waste mana on a partial fill)
      - skip Blizzard channel in AoE mode unless ≥4 sustained targets

  crisis_behavior:
    when: SELF.mana_pct < 25
    rules:
      - swap to MAGE armor mode
      - skip Tiers 3-7 except free Brain Freeze FFB (Tier 2/5)
      - fire MANA_GEM if charges available
      - fire EVOCATION if not in melee

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
      - OR (in_combat AND fight_elapsed_ms ~ 60000 AND icy_veins_off_cd AND cold_snap_off_cd)   # 1-min mark re-pot

  mana_potion:
    item: Mythical_Mana_Potion
    item_id: 57192
    use_when:
      - in_combat
      - SELF.mana_pct < 25
      - cooldown_shared_potion.off_cooldown
    notes: shares CD with Volcanic Potion; bot prefers Volcanic for DPS unless mana crisis.

  healthstone:
    item: Healthstone
    item_id: 36892
```

---

## Non-encounter content

### Solo combat

Frost is one of the strongest leveling specs. Permanent pet + Frost Nova kiting + Ice Lance burst.

```yaml
solo_offensive_rotation:
  - tier: 1: SELF.hp_pct < 25 -> ICE_BLOCK if off_cd else MAGE_WARD + retreat
  - tier: 2: ≥2 melee within 8y -> FROST_NOVA
  - tier: 3: fingers_of_frost.charges >= 1 AND deep_freeze.off_cd -> DEEP_FREEZE
  - tier: 4: brain_freeze.active AND fingers_of_frost.charges >= 1 -> FROSTFIRE_BOLT
  - tier: 5: pet.alive AND pet_freeze.off_cd AND enemies_in_8y >= 1 -> PET_FREEZE
  - tier: 6: brain_freeze.active AND remaining < 5s -> FROSTFIRE_BOLT
  - tier: 7: fingers_of_frost.charges >= 1 -> ICE_LANCE
  - tier: 8: SELF.mana_pct > 25 -> FROSTBOLT
  - tier: 9: SELF.mana_pct <= 25 -> wand auto-attack
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
  - check: aura.missing(SELF, MOLTEN_ARMOR) AND armor_mode == MOLTEN -> cast MOLTEN_ARMOR
  - check: aura.missing(SELF, ARCANE_BRILLIANCE) -> cast ARCANE_BRILLIANCE on raid
  - check: pet.dead AND not_in_combat -> WATER_ELEMENTAL
  - check: SELF.mana_pct < 80 AND no_enemies_within(40) -> drink
  - check: SELF.hp_pct < 95 AND no_enemies_within(40) -> eat
```

### Idle

```yaml
idle:
  primary_action: follow master
  secondary_actions:
    - maintain MOLTEN_ARMOR
    - maintain WATER_ELEMENTAL pet
    - maintain ARCANE_BRILLIANCE on raid
  do_not:
    - do not cast offensive spells while idle
    - do not Polymorph neutral mobs
```

### World boss / elite scaling

```yaml
world_boss_or_elite:
  rotation_override:
    if_in_party:
      use: encounter rotation
      thresholds:
        ice_block_threshold: 30                # raised — elites hit harder
        crisis_mode_pct: 30
    if_solo:
      use: solo_offensive_rotation
      thresholds:
        frost_nova_health_threshold: 80        # frost nova more aggressively
        pet_re_summon_aggression: high          # always have pet up
        do_not_engage_unless_can_solo: true
```

### Disengage / emergency utility

```yaml
disengage:
  invisibility:
    use_when:
      - SELF.threat_pct > 90% on boss target
      - NOT main_threat_intended
    cast: INVISIBILITY on SELF

  ice_block_drop_aggro:
    use_when:
      - SELF.hp_pct < 40
      - taking_aoe_damage
    cast: ICE_BLOCK on SELF
    notes: drops aggro briefly during 10s immunity
```

---

## Leveling rotation

Pure Frost from level 10 (spec choice). Standard leveling-friendly path.

### Spec recommendation

```yaml
spec_choice:
  for_leveling_dungeon_dps_bot:
    spec: Frost
    talent_split: 2 Arcane / 8 Fire / 31 Frost
    talent_picks_arcane: Improved Counterspell 2/2
    talent_picks_fire: Ignite 3/3 + Master of Elements 2/2 + Critical Mass 3/3
    talent_picks_frost_capstone: Deep Freeze (31-pt)
    rationale: |
      Per survey: 2/8/31 is universal consensus. 2 Arcane in Improved
      Counterspell reinforces the spec's iconic-interrupt rationale.
      8 Fire = Ignite + Master of Elements + Critical Mass (Frostfire Bolt
      crit procs Ignite — UNVERIFIED). 31 Frost reaches Deep Freeze capstone.
```

### Spell unlock table

```yaml
spell_unlocks:
  FROSTBOLT:           {level: 7,  spell_id: 116}
  FROST_NOVA:          {level: 8,  spell_id: 122}
  COUNTERSPELL:        {level: 9,  spell_id: 2139}
  WATER_ELEMENTAL:     {level: 10, spell_id: 31687}    # PERMANENT pet for Frost
  EVOCATION:           {level: 12, spell_id: 12051}
  POLYMORPH:           {level: 14, spell_id: 118}
  CONE_OF_COLD:        {level: 18, spell_id: 120}
  SLOW_FALL:           {level: 22, spell_id: 130}
  ICE_LANCE:           {level: 28, spell_id: 30455}
  CONJURE_MANA_GEM:    {level: 28, spell_id: 759}
  MANA_SHIELD:         {level: 30, spell_id: 1463}
  ICE_BLOCK:           {level: 30, spell_id: 45438}
  MOLTEN_ARMOR:        {level: 34, spell_id: 30482}
  MAGE_WARD:           {level: 36, spell_id: 543}
  MIRROR_IMAGE:        {level: 50, spell_id: 55342}
  BLIZZARD:            {level: 52, spell_id: 10}
  FROST_ARMOR:         {level: 54, spell_id: 7302}
  FROSTFIRE_BOLT:      {level: 56, spell_id: 44614}
  ARCANE_BRILLIANCE:   {level: 58, spell_id: 1459}
  DEEP_FREEZE:         {level: 60, spell_id: 44572}    # baseline at 60 OR via 31-Frost capstone
  MAGE_ARMOR:          {level: 68, spell_id: 6117}
  SPELLSTEAL:          {level: 70, spell_id: 30449}
  FLAME_ORB:           {level: 81, spell_id: 82731}    # converted to Frostfire Orb via talent
  RING_OF_FROST:       {level: 83, spell_id: 113724}
  TIME_WARP:           {level: 85, spell_id: 80353}
  # Talent-gated:
  SHATTER:                {requires_talent: true, talent_tier: 1, tree: frost}
  PIERCING_ICE:           {requires_talent: true, talent_tier: 1, tree: frost}
  FINGERS_OF_FROST:       {requires_talent: true, talent_tier: 3, tree: frost}
  ICY_VEINS:              {requires_talent: true, talent_tier: 3, tree: frost}
  COLD_SNAP:              {requires_talent: true, talent_tier: 4, tree: frost}
  BRAIN_FREEZE:           {requires_talent: true, talent_tier: 4, tree: frost}
  FROSTFIRE_ORB:          {requires_talent: true, talent_tier: 6, tree: frost}
  IMPROVED_COUNTERSPELL:  {requires_talent: true, talent_tier: 2, tree: arcane}
  IGNITE:                 {requires_talent: true, talent_tier: 1, tree: fire}
  CRITICAL_MASS:          {requires_talent: true, talent_tier: 3, tree: fire}
```

### Leveling brackets

```yaml
bracket_1_6:
  available: ARCANE_MISSILES, FROST_ARMOR (lvl 1 baseline)
  rotation:
    - tier 1: SELF.hp_pct < 25 -> retreat
    - tier 2: enemy in range -> ARCANE_MISSILES + wand
    - tier 3: idle -> follow
  notes: pre-Frostbolt; bot is essentially wand+missile only

bracket_7_9:
  available: + FROSTBOLT (7), + FROST_NOVA (8)
  rotation:
    - tier 1: SELF.hp_pct < 25 -> retreat
    - tier 2: enemy in melee -> FROST_NOVA + retreat
    - tier 3: enemy in range -> FROSTBOLT
  notes: Frostbolt becomes primary cast at 7

bracket_10_13:
  available: + WATER_ELEMENTAL (Frost spec selection at 10), + COUNTERSPELL (9)
  maintenance: keep Water Elemental summoned
  rotation: same as 7_9 plus Counterspell on enemy casts

bracket_14_27:
  available: + POLYMORPH (14), + CONE_OF_COLD (18)
  rotation_addition:
    - Polymorph on humanoid/beast adds in dungeon (cc-marker only)
    - Cone of Cold for AoE / kite

bracket_28_29:
  available: + ICE_LANCE (28), + CONJURE_MANA_GEM (28)
  rotation_change:
    - Tier 6 Ice Lance enters when FoF talent allocated (~level 30)

bracket_30_33:
  available: + MANA_SHIELD (30), + ICE_BLOCK (30)
  rotation_addition:
    - Ice Block panic button at self.hp < 20

bracket_34_55:
  available: + MOLTEN_ARMOR (34), + MAGE_WARD (36), + MIRROR_IMAGE (50), + BLIZZARD (52), + FROST_ARMOR (54)
  rotation_addition:
    - Mirror Image cooldown profile
    - Blizzard for sustained AoE
  maintenance_change: Molten Armor becomes default (replaces low-level Frost Armor for solo)

bracket_56_67:
  available: + FROSTFIRE_BOLT (56), + ARCANE_BRILLIANCE (58), + DEEP_FREEZE (60)
  rotation_change:
    - Tier 1 Deep Freeze enters
    - Tier 2/5 Frostfire Bolt enters (Brain Freeze talent allocation expected ~30s)
  maintenance_addition:
    - Arcane Brilliance on raid pre-pull

bracket_68_80:
  available: + MAGE_ARMOR (68), + SPELLSTEAL (70)
  rotation_change:
    - Mage Armor swap when mana-stressed
    - Spellsteal manual command only

bracket_81_84:
  available: + FLAME_ORB (81 — converted to Frostfire Orb via talent), + RING_OF_FROST (83)
  rotation_addition:
    - Tier 3 Frostfire Orb enters

bracket_85:
  available: + TIME_WARP, + all talents fully allocated
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
| 1 | Brain Freeze proc rate at 3/3 | 15% | Tier 2/5 timing | Talent.dbc |
| 2 | Fingers of Frost proc rate at 3/3 | 20% | Tier 1/6 expectation | Talent.dbc |
| 3 | Shatter multiplier value | ×3 (capped at 100%) | Rotation throughput model | Talent.dbc |
| 4 | Frostfire Bolt periodic damage | 9s DoT (assumed) | Spell catalog | DBC EffectAura |
| 5 | Mage Armor mana regen value | 3% per 5s | Spell catalog | Spell.dbc |
| 6 | Molten Armor crit value | 3% spell crit | Spell catalog | Spell.dbc |
| 7 | Water Elemental Freeze CD | 25s | Tier 4 timing | Spell.dbc 33395 |
| 8 | Water Elemental Freeze radius | 8y | Tier 4 predicate | Spell.dbc |
| 9 | Deep Freeze damage formula | 1065-1334 base + spellpower | Throughput model | Spell.dbc 44572 |
| 10 | Cold Snap exact reset list | Icy Veins, CoC, Ice Block, Frost Nova, Deep Freeze | Cooldown profile | Spell.dbc cooldown-category |
| 11 | Counterspell silence at 2/2 IC | 4s | Counterspell catalog | Talent.dbc |
| 12 | Counterspell CD with Cold as Ice talent | 20s (vs 24s baseline) | Counterspell catalog | Talent.dbc |
| 13 | Polymorph PVE duration | 50s | Spell catalog | Spell.dbc 118 |
| 14 | Time Warp Temporal Displacement duration | 10 min | Cooldown rule | Spell.dbc |
| 15 | Frostfire Orb FoF proc capability | YES (assumed) | Tier 3 expectation | Spell.dbc + talent effect |
| 16 | Frostbolt slow value | 40% / 9s | Catalog | Spell.dbc |
| 17 | Glyph of Frostbolt +5% crit on shattered | yes (assumed; crit cap matters) | Glyph effect | Glyph.dbc |
| 18 | Glyph of Deep Freeze +20% additivity | additive vs Frostburn | Throughput math | Glyph.dbc |
| 19 | **Ignite procs from Frostfire Bolt crits** | **YES (assumed; CRITICAL for 8-Fire allocation)** | Talent build justification | Talent.dbc + spell-school proc-flag |
| 20 | Ring of Frost Cata spell ID | 113724 (assumed) | Spell catalog | DBC lookup |

---

## Open hooks for `AltbotCombat.cpp`

| Doc tier | Code structure |
|---|---|
| Spell Catalog | `static const SpellEntry kFrostMage_Spells[]` |
| Rotation tiers | `static const RotationTier kFrostMage_Rotation[]` |
| AoE rotation | `static const RotationTier kFrostMage_RotationAoE[]` (separate table) |
| Cooldown Profile | `static const CooldownTrigger kFrostMage_Cooldowns[]` |
| Mana Management | `ManaState` enum + threshold helpers |
| Leveling rotation | `LevelBracketRotation kFrostMage_Leveling[]` |
| Brain Freeze proc tracking | aura listener for Brain Freeze buff (active/expiry) |
| Fingers of Frost charge tracking | aura listener for FoF charges (0-2) + remaining duration |
| Water Elemental pet state | `PetState` on `AltbotAI` (alive/dead, Freeze CD, position) |
| Armor mode | `ArmorMode` enum (Molten / Mage / Frost) on `AltbotAI` |
| Time Warp owner flag | `bool time_warp_owner` runtime flag (default true) |
| AoE mode hysteresis | `AoEMode` state with 5s entry/exit hysteresis |

**Key new helper APIs:**
- `BrainFreezeActive(bot)` — read proc state.
- `BrainFreezeRemainingMs(bot)` — for Tier 5 burn-before-expiry.
- `FingersOfFrostCharges(bot)` — read 0-2.
- `FingersOfFrostRemainingMs(bot)` — for Tier 6 burn timing.
- `EnemiesWithin15yOfTarget(target)` — for AoE-mode trigger.
- `IsBeingMeleed(bot, count)` — for Frost Armor swap + Frost Nova trigger.
- `PetFreezeOffCd(bot)` — pet ability state.

Cross-reference to `docs/roles/ranged-dps.md` for range management / target priority / interrupt rotation patterns. This is the first ranged-DPS spec; future ranged DPS specs (Affliction Warlock, Marksmanship Hunter, Shadow Priest, Balance Druid, Elemental Shaman) will layer on the same role doc.
