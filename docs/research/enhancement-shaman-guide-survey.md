# Enhancement Shaman 4.3.4 — Multi-Guide Strategy Survey

Audit trail for `docs/specs/enhancement-shaman.md`. Survey conducted 2026-05-02. This is the **fifth melee-DPS survey** (after Combat Rogue, Fury Warrior, Retribution Paladin, Frost Death Knight). Enhancement is the **only Shaman melee DPS spec**: Elemental is ranged caster (already documented in `docs/specs/elemental-shaman.md`); Restoration is healer (already documented in `docs/specs/resto-shaman.md`). There is no DPS spec-selection contest — Enh is locked in as the lone candidate and the discussion below skips spec contention.

Cross-reference: Cata 4.3.4 Shamans share the **totem framework**, **shield buffs (Lightning Shield / Water Shield / Earth Shield)**, **Searing Totem mechanic**, **Hex CC**, **Wind Shear interrupt**, **Heroism/Bloodlust raid CD**, and the **mana resource pool** with Elemental and Resto. Those mechanics are reused as encoded in the prior two specs; this survey focuses on the Enh-specific shape: **Maelstrom Weapon proc engine**, **Stormstrike + Lava Lash + Flame Shock** core rotation, **dual-wield with weapon imbues (Flametongue MH + Windfury OH)**, **Mental Quickness spellpower-from-AP conversion**, **Shamanistic Rage**, and **Feral Spirit**.

Enhancement is unique among Cata 4.3.4 melee DPS specs because it is the **only hybrid melee-caster** in the patch — Maelstrom Weapon stacks make instant Lightning Bolt / Chain Lightning / Healing Wave casts a core part of the rotation, not a fallback. Bot logic must track Maelstrom-stack count as an event-driven flag and gate Lightning Bolt firing on stack threshold (typically 5).

---

## 1. Spec selection

**Spec disposition: Enhancement (dual-wield).** Enhancement is the only melee DPS spec available to Shamans in Cataclysm 4.3.4. Elemental is ranged caster (covered separately); Restoration is healer (covered separately). There is no Enh-vs-other-spec contest within Shaman melee DPS — Enh is the disposition by elimination.

Enhancement is **dual-wield only** in Cata 4.0+. The 2H Enhancement build that existed in late TBC / early Wrath was removed; Stormstrike, Lava Lash, and Mental Quickness are all coded around two 1H weapons. Bot-side: Enh equip-state must be `weapon_main_hand_1h && weapon_off_hand_1h`; Stormstrike and Lava Lash will fail to cast on a 2H or shield off-hand setup.

**Raid viability (Dragon Soul tier):** Enhancement clears all of Dragon Soul comfortably; consensus tier rating across guides is **B-tier** (mid-pack on simulated DPS). Strong fights for Enh include cleave / multi-target encounters (Warmaster Blackhorn adds, Spine of Deathwing tendons, Yor'sahj slimes) where Lava Lash's flame-shock-spread mechanic shines. Weak fights include high-mobility / single-target burn (Ultraxion, Madness of Deathwing tentacle phases) where Maelstrom-cast windows compress.

**Maelstrom Weapon uniqueness:** Among melee DPS, only Enhancement gates instant-cast spell damage on a stacking proc. This is the spec's most distinctive bot-control challenge.

---

## 2. Sources

| # | Guide | URL | Confidence |
|---|---|---|---|
| G1 | Icy Veins Enhancement Shaman Rotation/CDs | https://www.icy-veins.com/cataclysm-classic/enhancement-shaman-pve-rotation-cooldowns-abilities | High |
| G2 | Icy Veins Enhancement Shaman Talents/Glyphs | https://www.icy-veins.com/cataclysm-classic/enhancement-shaman-pve-spec-builds-talents-glyphs | High |
| G3 | Icy Veins Enhancement Shaman Stat Priority | https://www.icy-veins.com/cataclysm-classic/enhancement-shaman-pve-stat-priority | High |
| G4 | Wowhead Cata Enhancement Shaman | https://www.wowhead.com/cata/guide/classes/shaman/enhancement/dps-overview-pve | Medium-High |
| G5 | wowtbc.gg Enhancement Shaman | https://wowtbc.gg/cata/class-guides/enhancement-shaman/ | Medium |
| G6 | Warcraft Tavern Enhancement Shaman | https://www.warcrafttavern.com/cataclysm/guides/pve-enhancement-shaman/ | Medium-High |
| G7 | Method Cata Enhancement Shaman | https://www.method.gg/guides/cataclysm/enhancement-shaman | Medium |
| G8 | Noxxic Enhancement Shaman 4.3 | https://www.noxxic.com/wow/pve/shaman/enhancement/ | Medium |
| G9 | (Cross-ref) `docs/specs/elemental-shaman.md` (totem / shield / Heroism shared) | local | Authoritative on shared mechanics |
| G10 | (Cross-ref) `docs/specs/resto-shaman.md` (totem / Healing Stream shared) | local | Authoritative on shared mechanics |
| G15 | Warcraft Wiki per-spell | https://warcraft.wiki.gg/wiki/<spell> | High |
| G16 | Wowhead Cata per-spell | https://www.wowhead.com/cata/spell=<id> | High |

---

## 3. Strong consensus

| Item | Consensus | Spec disposition |
|---|---|---|
| **Dual-wield only** — Enh requires two 1H weapons; Stormstrike / Lava Lash gated on DW. | 8/8 | Hard requirement. |
| **Agility = primary stat** (NOT Strength) — Enh shaman scales melee AP from Agility, not Strength. | 8/8 | Encoded. |
| **Maelstrom Weapon (talent, 5/5 ranks)** — melee swings have chance to add a stack (max 5); each stack reduces nature spell cast time by 20% AND mana cost by 20% (5 stacks = instant + free). | 8/8 | Primary rotation engine. |
| **Stormstrike** — 8s CD (Cata baseline) instant strike, both weapons hit; applies Stormstrike buff (next 2 nature spells from caster get +25% crit). | 8/8 | Top single-target priority. |
| **Lava Lash** — 10s CD off-hand-only ability; fire damage; **spreads Flame Shock to up to 4 nearby targets within 12y** if Flame Shock present on primary target. | 8/8 | Top priority, AoE setup. |
| **Flame Shock** — primary DoT; 6s CD shock (shared with Earth/Frost Shock); ~18s base duration (24s with Searing Flames refresh chains). | 8/8 | Maintained DoT. |
| **Earth Shock** — filler shock when Stormstrike on CD AND Flame Shock active; same 6s shock-CD bucket as Flame Shock. | 8/8 | Secondary shock. |
| **Searing Totem** — fire totem; ranged auto-attack-style fire damage; Searing Flames talent stacks DoT on target. | 8/8 | On CD / on totem refresh. |
| **Magma Totem** — fire totem (replaces Searing Totem in AoE); pulse fire damage in 8y. | 8/8 | AoE only. |
| **Feral Spirit (Spirit Wolves)** — 2min CD; summons 2 wolves for 30s; meaningful raw damage + Spirit Hunt heal-passive. | 8/8 | Major DPS CD. |
| **Shamanistic Rage** — 1min CD; 15s; -30% damage taken AND mana-from-melee-attacks (Cata: replaces mana cost reduction with mana regen). | 8/8 | Defensive + mana CD. |
| **Wind Shear** — 6s CD interrupt, off-GCD, 25y range, 2s lockout. | 8/8 | Auto-interrupt. |
| **Hex** — 35s CD CC; turns target into frog 60s; PvE = caster-add CC. | 8/8 | CC. |
| **Heroism / Bloodlust** — 5min CD raid haste 30%; off-GCD. | 8/8 | Raid CD. |
| **Lightning Shield** — 1hr buff; lightning-charge melee retaliation; **mandatory in Enh** (Static Shock proc + Mental Quickness benefit). | 8/8 | Always-on. |
| **Flametongue Weapon (MH imbue)** — main-hand imbue; +spellpower (mandatory because Mental Quickness → spellpower from AP). | 7/8 | MH imbue default. |
| **Windfury Weapon (OH imbue)** — off-hand imbue; melee-swing chance to grant 3 extra attacks at +4000 AP. | 7/8 | OH imbue default. |
| **Mental Quickness (passive talent)** — 50% of melee AP becomes spellpower; +2% spell hit; +30% Stormstrike/Lava Lash/Earth Shock damage. | 8/8 | Passive talent — mandatory. |
| **Static Shock (passive talent)** — Lightning Shield charges have chance on melee attack to deal extra nature damage; restores small mana per proc. | 8/8 | Passive talent. |
| **Searing Flames (talent)** — Searing Totem applies Searing Flames DoT stack on target; up to 5 stacks; refreshes Flame Shock duration. | 8/8 | Passive — extends Flame Shock pandemic. |
| **Unleashed Rage (raid buff talent)** — +10% melee/ranged AP raid-wide. | 8/8 | Passive raid buff. |
| **Improved Lava Lash (talent)** — +damage on Lava Lash + flame-shock-spread mechanic enabled here. | 8/8 | Passive. |
| **Toughness (talent)** — +10% Stamina, -30% movement-impair duration. | 7/8 | Default 3/3. |
| **Frozen Power (talent)** — Frostbrand Weapon imbue chance + +20% damage to chilled targets. | 5/8 | Skipped in default raid build. |
| **Talent split — 31 Enh / 7 Elemental / 3 Resto** (most common). | 6/8 | Default. |
| **Hit cap (spell)** — 17% spell hit needed for Maelstrom-Weapon-instant Lightning Bolt to never miss; **8% melee/spell hit is the soft cap** because Mental Quickness grants +2% spell hit and Draenei racial / talents close the rest. | 6/8 | Soft cap 8% melee, hard cap 17% spell *if pursued*. |
| **Spell hit reality** — most guides target **8% melee hit** as the hit cap; spell hit is only relevant for instant Lightning Bolt and is left intentionally below 17% because reaching it costs too much itemization. | 7/8 | Encoded — 8% hit. |
| **Expertise cap** — 26 (~781 rating) to dodge cap. | 8/8 | Hard. |
| **Mastery: Enhanced Elements** — Enhancement Shaman mastery; increases nature/fire/frost damage dealt by X%. | 8/8 | Reforge target. |
| **Stat priority** — Agility → Hit (8%) → Expertise (26) → Mastery → Haste → Crit (consensus, with caveats — see disagreements). | 6/8 | See §4.3. |
| **Glyphs Prime** — Stormstrike + Feral Spirit + Lava Lash. | 7/8 | Default. |
| **Glyphs Major** — Shamanistic Rage + Lightning Shield + Fire Nova OR Healing Stream Totem (encounter-tunable). | 6/8 | Default. |
| **Glyphs Minor** — Renewed Life + Astral Recall + Water Shield (cosmetic / utility). | 7/8 | Cosmetic. |
| **Pre-pot Tolvir Potion** — +1200 Agility 25s; T-2s + 1-min mark. | 6/8 | Encoded. |
| **Opener** — Lightning Shield (pre-buff) → totems pre-pull → T-2s Tolvir Potion → Searing Totem (or Magma) → Stormstrike → Flame Shock → Lava Lash → Maelstrom-Weapon Lightning Bolt @ 5 stacks → Earth Shock → repeat priority. | 5/8 | Encoded. |
| **Unleash Elements (Cata-new L81)** — releases active weapon imbue effects + grants buff (Unleash Flame: +20% next Flame Shock damage when MH = Flametongue; Unleash Wind: +40% melee haste 8s when OH = Windfury). | 7/8 | On CD. |
| **Spiritwalker's Grace (L85)** — 2min CD; allows casting while moving 15s; primarily Resto/Elemental but available. | 6/8 | Movement utility. |
| **Fire Elemental Totem** — 5min CD; 2min duration; major DPS CD (large fire pet). | 8/8 | Major CD. |
| **Earth Elemental Totem** — 5min CD; 2min duration; tank-pet, defensive utility. | 7/8 | Defensive utility. |
| **Tremor Totem (Cata rework)** — Cata 4.0+: Tremor Totem became an **active CD-based PvP-tool**, no longer the passive fear-break of older expansions. | 5/8 | Reactive. |
| **Healing Stream / Mana Tide** — raid utility totems (shared with Resto/Elemental). | 8/8 | Utility. |

---

## 4. Disagreements

### 4.1. Talent split — 31/7/3 vs 31/3/7 vs 31/5/5

**Spec disposition: 31 Enh / 7 Elemental / 3 Resto.**

- **Enh 31:** all core (Maelstrom Weapon 3/3, Improved Stormstrike 2/2, Improved Shields 3/3, Elemental Devastation, Flurry, Searing Flames 3/3, Static Shock 3/3, Improved Lava Lash 2/2, Mental Quickness, Shamanistic Rage, Frozen Power 0/2 [skip — frost-school not used], Unleashed Rage 3/3, Feral Spirit, Improved Fire Nova 0/2 OR 2/2 depending on Fire Nova use, Stormstrike capstone access).
- **Elemental 7:** **Acuity 3/3** (+3% crit), **Convection 2/2** OR **Concussion 2/2** (+damage to nature/fire spells — affects Lightning Bolt, Flame Shock, Earth Shock), **Elemental Warding 2/3** OR **Reverberation 2/2** (cooldown reduction on shocks). G1/G6 prefer Reverberation 2/2 for the shock-CD reduction; G4 prefers Convection.
- **Resto 3:** **Improved Shields 3/3** (mana-return on Lightning Shield / Water Shield orbs; Enh tree's Improved Shields is different — that's Resto-tree depth) OR **Earthen Power 2/2** + 1 floating point.

G7 (Method) suggests 31/5/5 with Resto-side Earthen Power for movement-impair-cleanse on Earthbind; consensus rejects this as raid-PvE filler value. **Default: 31/7/3.**

### 4.2. Frozen Power talent — take or skip?

**Spec disposition: skip Frozen Power 0/2 in default raid build.** Frozen Power requires Frostbrand Weapon imbue to gain value (chilled target proc). Default Enh imbues are Flametongue MH + Windfury OH; Frostbrand replaces Windfury OH and reduces single-target DPS by ~3%. Take only on PvP / kiting builds. **Default: Frostbrand off; Frozen Power 0/2.**

### 4.3. Stat priority — Mastery vs Haste vs Crit ordering

**Spec disposition: Agility → 8% Hit → 26 Expertise → Mastery → Haste → Crit.**

- G1 (Icy Veins): Mastery > Haste > Crit.
- G3 (Icy Veins stats page): Mastery > Haste > Crit, but notes Haste closes the gap at high gear levels because Maelstrom-Weapon stacks come faster.
- G4 (Wowhead): Mastery > Haste > Crit.
- G6 (Warcraft Tavern): Haste > Mastery > Crit (minority view; Haste-first because Maelstrom-Weapon proc rate scales with melee swing rate which scales with haste).
- G8 (Noxxic): Mastery > Crit > Haste (rare ordering — places Crit above Haste).

**Disposition: Mastery > Haste > Crit (4/5 majority).** Bot-side stat-weight reforge target: Mastery primary, Haste secondary. Encounter-specific: high-cleave fights (Lava Lash Flame-Shock-spread relevance) may favor Mastery harder; low-cleave single-target fights favor Haste for Maelstrom-stack rate.

### 4.4. Spell Hit cap — pursue 17% or stop at 8%?

**Spec disposition: target 8% melee hit; do NOT reforge for 17% spell hit.** Reaching 17% spell hit costs ~9% of itemization that would otherwise be Mastery/Haste/Agility. The damage gain from Maelstrom-Weapon-Lightning-Bolt never missing is smaller than the loss of Mastery on every other ability. **Encoded: 8% melee hit cap; spell hit floats at ~10–12% from passive sources (Mental Quickness +2%, talents, Draenei racial if applicable).**

Bot-side: Lightning Bolt missing on a 5-stack consumption is a notable feel-bad but is mathematically acceptable.

### 4.5. Maelstrom Weapon stack consumption — 5 stacks only or partial?

**Spec disposition: consume at 5 stacks default; consume at 4+ stacks during movement OR at fight-end.**

- G1 / G4 / G6: always 5 stacks for Lightning Bolt (full speed bonus, no mana cost).
- G7 (Method): allow 4-stack consumption if the bot is forced to move and a 5th stack would be wasted by combat ending.
- G5 / G8: 5 stacks always.

**Disposition: 5 stacks default; 4-stack consumption permitted on heavy-movement fights (encounter-tunable bot flag).**

### 4.6. Maelstrom-Lightning-Bolt vs Chain Lightning at 2+ targets

**Spec disposition: Lightning Bolt single-target; Chain Lightning at 3+ targets.**

Lightning Bolt and Chain Lightning both consume Maelstrom Weapon stacks. Chain Lightning hits up to 3 targets in Cata (down from 5 in earlier expansions; **UNVERIFIED** — Cata 4.3.4 jump count may be 3 baseline; Wowhead spell page lists 3). For 1–2 targets, Lightning Bolt out-DPS's Chain Lightning per cast; for 3+, Chain Lightning's bounce damage wins.

### 4.7. Lava Lash priority — always on CD or only when Flame Shock active?

**Spec disposition: always on CD.** Lava Lash deals significant direct fire damage independent of the Flame-Shock-spread mechanic. The flame-shock-spread is a bonus on AoE, not a gate on use.

- G1 / G4 / G6: Lava Lash on CD always.
- G7: Lava Lash on CD; if 4+ targets are present and Flame Shock is NOT on primary, cast Flame Shock first then Lava Lash.

Bot-side priority: Lava Lash before Earth Shock; after Stormstrike (since Stormstrike has the Stormstrike-buff side effect feeding the next Maelstrom-Lightning-Bolt's crit).

### 4.8. Stormstrike priority — before or after Lava Lash?

**Spec disposition: Stormstrike > Lava Lash > Flame Shock > Earth Shock.**

- G1: Stormstrike first (per-cast damage + Stormstrike-buff for next nature crit).
- G4 / G6 / G7: Stormstrike first.
- G5: Lava Lash first (minority — based on shorter CD on Lava Lash if both up; questionable).

**Disposition: Stormstrike first when both off CD.** Stormstrike has the Stormstrike-buff (next 2 nature spells +25% crit) which buffs incoming Lightning Bolt / Earth Shock damage — feeding the buff first compounds value.

### 4.9. Flame Shock pandemic — refresh window

**Spec disposition: refresh Flame Shock when remaining duration ≤ 3s OR when shock-CD is up AND no other shock priority exists.**

Searing Flames passive auto-extends Flame Shock duration by ticking; the refresh window for manual recasts is when ≤ 3s remains. Bot logic: track Flame Shock duration on primary target; if ≤ 3s, recast in the shock slot.

### 4.10. Earth Shock — filler or skip?

**Spec disposition: filler only — cast Earth Shock when shock-CD is up AND Flame Shock has > 6s remaining AND no other priority.**

Earth Shock is the lowest-priority shock. Use it to consume the shock-CD bucket when Flame Shock is healthy. **Bot priority: Flame Shock (if expiring) > Earth Shock (filler).**

### 4.11. Fire Nova — usage frequency

**Spec disposition: AoE 3+ targets only.** Fire Nova explodes Flame Shock on every infected target in 10y; deals fire damage per explosion. On 3+ targets, Fire Nova outpaces filler shocks. On 1–2 targets, Fire Nova is a net DPS loss vs. the standard Stormstrike/Lava Lash/shock priority because it doesn't apply or refresh Flame Shock — only consumes it for damage.

Bot-side: gate Fire Nova on `count_flame_shocked_enemies_in_10y >= 3`.

### 4.12. Glyph of Fire Nova vs Glyph of Healing Stream Totem

**Spec disposition: Major slot — Fire Nova on AoE-heavy fights; Healing Stream Totem on single-target survival fights.**

- Glyph of Fire Nova: -3s CD on Fire Nova (relevant only on AoE).
- Glyph of Healing Stream Totem: +Healing Stream throughput on the totem, +1% raid resist (utility).

**Default: Glyph of Healing Stream Totem (single-target raid fights are more common in Dragon Soul); swap to Glyph of Fire Nova on Madness adds / Spine of Deathwing tendons.**

### 4.13. Searing Totem vs Magma Totem

**Spec disposition: Searing Totem default (single-target); Magma Totem on AoE 4+.**

Searing Totem is a single-target ranged-fire totem; Magma Totem pulses 8y AoE fire. On 1–3 targets, Searing Totem wins because of Searing Flames stack-up (extends Flame Shock duration on primary). On 4+ targets, Magma Totem's pulse damage wins.

Bot-side: AI logic should default Searing Totem; switch on enemy-count threshold.

### 4.14. Feral Spirit (Spirit Wolves) timing

**Spec disposition: on CD (2min); first use stacked with Bloodlust/Heroism + Tolvir Potion at the pull when possible.**

Feral Spirit is the major DPS CD; Spirit Wolves benefit from raid haste buffs (Bloodlust, Time Warp, Heroism). Glyph of Feral Spirit grants the wolves a heal passive (Spirit Hunt — heals caster for 150% of damage dealt by wolves), so glyphed wolves are also a defensive throughput.

### 4.15. Shamanistic Rage usage

**Spec disposition: defensive primary; mana CD secondary.**

- Cata change: Shamanistic Rage no longer reduces mana cost; instead grants mana-from-melee-attacks for 15s + -30% damage taken.
- Use defensively on incoming damage windows; if mana is < 30%, use to refill.

Bot-side: Shamanistic Rage on `self_HP < 60% AND incoming_damage_window` OR `mana < 30%`. Avoid wasting on full HP + full mana.

### 4.16. Fire Elemental Totem vs Earth Elemental Totem

**Spec disposition: Fire Elemental Totem = DPS CD; Earth Elemental Totem = defensive utility.**

Fire Elemental Totem: 5min CD, 2min duration, summons large fire elemental that auto-attacks for ~3500 fire damage per swing + AoE Fire Nova. Major DPS contribution on long fights. Use first pull + on CD.

Earth Elemental Totem: 5min CD, 2min duration, summons earth elemental tank-pet. Defensive only; raid skip.

### 4.17. Unleash Elements (Cata-new L81) — when to cast?

**Spec disposition: on CD (15s baseline) for Unleash Wind buff (+40% melee haste 8s).**

Unleash Elements releases the active weapon imbues:
- Flametongue MH → **Unleash Flame**: +20% damage on next Flame Shock (use just before recasting Flame Shock).
- Windfury OH → **Unleash Wind**: +40% melee haste 8s (use to compress Stormstrike + Lava Lash + auto-attack windows for Maelstrom stack-up).

Both trigger off the same cast (15s CD). Disposition: prioritize timing for Unleash Wind (+40% melee haste = faster Maelstrom stacks).

**Bot-side: cast Unleash Elements on CD; the buff effect depends on weapon imbues, so MH/OH imbue state must be tracked.**

### 4.18. Lightning Shield — buff maintenance

**Spec disposition: always-on; recast when stacks drop below 3 OR buff falls.**

Lightning Shield in Cata is a 1-hour buff with charges that deplete on melee-hit retaliation. The Improved Shields talent grants extra orb count (Resto-tree, Enh-takes-via-cross-tree if 31/3/7 split — most builds skip this). Default: cast Lightning Shield pre-pull; recast when expired or low charges.

### 4.19. Weapon imbues — Flametongue MH / Windfury OH or alternate?

**Spec disposition: Flametongue MH + Windfury OH (default).**

- Flametongue MH grants spellpower (passive scaling on Mental Quickness's spellpower-from-AP — additive).
- Windfury OH grants chance on melee swing for 3 extra hits at +4000 AP (significant DPS).

**Alternate: Windfury MH + Flametongue OH** has been proposed (G5 / G8 minority) on the basis that Windfury MH can proc more often due to MH swing speed; consensus rejects this on simulation data — Windfury OH is the consensus choice because the 3-attack burst includes off-hand swings with the Mental Quickness damage scaling, and the OH-proc-rate calculation favors it.

**Frostbrand OH:** PvP / kiting only; -3% DPS in raid.

### 4.20. Glyphs split

**Spec disposition (Prime):**
- **Glyph of Stormstrike** — +Stormstrike damage (mandatory).
- **Glyph of Feral Spirit** — wolves heal caster for 150% of damage dealt (defensive throughput + DPS-neutral).
- **Glyph of Lava Lash** — +Lava Lash damage (mandatory).

**Spec disposition (Major):**
- **Glyph of Shamanistic Rage** — removes magic effects from caster on cast (cleanse-self utility).
- **Glyph of Lightning Shield** — extra charges / longer duration (utility).
- **Glyph of Healing Stream Totem** — +Healing Stream throughput (default raid; swap to Glyph of Fire Nova on AoE fights).

**Spec disposition (Minor):**
- **Glyph of Renewed Life** — Reincarnation no-reagent.
- **Glyph of Astral Recall** — -CD on Astral Recall.
- **Glyph of Water Shield** — extra charges on Water Shield (rarely used in Enh — Lightning Shield is the default).

### 4.21. Stormstrike-buff stacking — who consumes it?

**Spec disposition: Maelstrom-Lightning-Bolt consumes the Stormstrike-buff first; Earth Shock second.**

Stormstrike buff: next 2 nature spells from caster get +25% crit. Lightning Bolt (consumed via Maelstrom 5-stack), Chain Lightning (AoE), Earth Shock (filler), and Flame Shock all benefit. Highest-damage nature spell during the 8s Stormstrike-buff window is Lightning Bolt (Maelstrom 5-stack) — the bot should prioritize a Maelstrom-Lightning-Bolt cast inside the Stormstrike-buff window if 5 stacks are available.

### 4.22. Opener sequence

**Spec disposition (ST):**
1. **T-30s** — Lightning Shield (pre-buff).
2. **T-15s** — Drop totems (Searing Totem, Healing Stream / Mana Spring, Strength of Earth / Stoneskin — choose based on raid comp).
3. **T-2s** — Tolvir Potion.
4. **T-1.5s** — Stormstrike (cast as it lands at T-0; first GCD).
5. **T-0** — Flame Shock (DoT applied).
6. **T+1.5s** — Lava Lash.
7. **T+3s** — Searing Totem (if not pre-dropped) OR Magma Totem if AoE.
8. **T+4.5s** — Earth Shock (filler) OR Maelstrom-Lightning-Bolt if 5 stacks.
9. **T+~6s** — Feral Spirit + Fire Elemental Totem.
10. **T+~7.5s** — Continue priority.

**Bloodlust/Heroism alignment:** if raid casts Bloodlust pre-pull, Feral Spirit + Fire Elemental Totem stack inside the Bloodlust window for compressed DPS burst.

### 4.23. ST priority list

**Spec disposition (priority list, top → bottom):**
1. **Maintain Lightning Shield** — recast if expired / low charges.
2. **Stormstrike** — on CD (8s).
3. **Lava Lash** — on CD (10s).
4. **Flame Shock** — refresh if ≤ 3s remaining (consumes shock CD).
5. **Unleash Elements** — on CD (15s) for Unleash Wind haste.
6. **Maelstrom-Lightning-Bolt** — at 5 stacks; consume inside Stormstrike-buff window if active.
7. **Earth Shock** — filler shock when shock-CD up AND Flame Shock healthy.
8. **Searing Totem** — recast on totem-timer expire (1min default).
9. **Feral Spirit** — on CD (2min).
10. **Fire Elemental Totem** — on CD (5min).
11. **Shamanistic Rage** — defensive (HP ≤ 60% OR mana ≤ 30%).
12. **Auto-attack** — between GCDs.

### 4.24. AoE rotation (3+ enemies)

**Spec disposition (priority list):**
1. **Flame Shock** on primary target (gates Lava Lash spread + Fire Nova).
2. **Lava Lash** — spreads Flame Shock to up to 4 targets within 12y of primary.
3. **Magma Totem** — replace Searing Totem on AoE (pulse damage).
4. **Fire Nova** — explodes Flame Shock on all infected targets within 10y.
5. **Stormstrike** — on CD (still single-target damage on primary).
6. **Maelstrom-Chain-Lightning** — at 5 stacks; hits 3 targets.
7. **Earth Shock** — filler.

### 4.25. Cleave 2 enemies

**Spec disposition: ST priority on primary; let Lava Lash + Magma Totem provide passive cleave.** Don't switch to AoE-mode rotation until 3+ targets. **Lava Lash flame-shock-spread requires Flame Shock to already be on primary** — bot must apply Flame Shock first before relying on spread.

### 4.26. Movement / kiting / disengage

**Spec disposition: instant-cast tools cover most movement.**

Enh has no melee gap-closer. Movement options:
1. **Maelstrom-Lightning-Bolt at 5 stacks** — instant nature damage during movement.
2. **Earth Shock / Flame Shock** — instant; refresh DoT during movement.
3. **Spiritwalker's Grace (L85)** — 2min CD; allows casting hard-cast spells while moving 15s; skips Maelstrom requirement during movement-heavy phases.
4. **Ghost Wolf** — out-of-combat travel form; +30% movement speed; consumes ~1.5s to cast (interrupted by combat).

Bot-side: prefer instant casts during movement; pre-cast Stormstrike before required moves.

### 4.27. Self-heal / survival

**Spec disposition: Shamanistic Rage primary; Healing Surge / Maelstrom-Healing-Wave secondary.**

- Shamanistic Rage: 1min CD, -30% damage taken 15s + mana regen.
- Healing Surge (hard-cast 1.5s): ~30% HP heal; rarely usable in melee (cast time interrupts).
- **Maelstrom-Healing-Wave at 5 stacks** — instant Healing Wave heal; usable in any melee window (~25–35% HP heal).
- Stoneclaw Totem (talented) + Healing Stream Totem — passive raid healing.

Bot-side: at HP ≤ 30%, consume Maelstrom stacks on Healing Wave instead of Lightning Bolt; if Maelstrom < 5, fire Shamanistic Rage if available.

### 4.28. Hex — when to use?

**Spec disposition: caster-add CC on 35s CD.** Hex turns target into a frog 60s; breaks on damage. Use on caster-add pulls (Hagara crystals' adds, Yor'sahj slime adds) or trash.

### 4.29. Wind Shear interrupt logic

**Spec disposition: 6s CD off-GCD, 25y range, 2s lockout.** Auto-cast on enemy cast ≤ 300ms before completion. **Range advantage:** 25y range (most melee interrupts are melee-range only); Wind Shear lets Enh interrupt mid-room casters from melee distance on the boss.

### 4.30. Tremor / Cleansing / Healing Stream / Mana Tide totems

**Spec disposition: shared with Elemental/Resto — encounter-driven utility.**

- **Tremor Totem (Cata)** — 1min CD; pulses every 3s; removes fear/charm/sleep effects on raid. Active CD-based, not passive.
- **Cleansing Totem** — pulses dispel of poison/disease.
- **Healing Stream Totem** — passive raid heal (~1% HP/3s); +1% raid resist with glyph.
- **Mana Tide Totem (Resto-tree)** — 3min CD raid-mana CD (Resto only; Enh does NOT get Mana Tide).

Bot-side: Tremor Totem on fear-heavy fights (Hagara phase 1 — wait, no fear; not relevant in Dragon Soul); Cleansing Totem on poison/disease debuff fights.

### 4.31. Pre-pot + combat-pot

**Spec disposition: Tolvir Potion T-2s pre-pull AND second Tolvir at 1-min mark when Feral Spirit + Fire Elemental Totem stacks align.** Tolvir grants +1200 Agility 25s. Volcanic Potion (intellect) is NOT used — Enh scales Agility.

### 4.32. Bloodlust / Heroism / Time Warp alignment

**Spec disposition: pre-pull cast by raid (default).** Enh benefits from haste during Bloodlust because Maelstrom-Weapon proc rate scales with melee swing rate (faster swings = faster stacks). Stack Feral Spirit + Fire Elemental Totem inside Bloodlust window.

### 4.33. T13 set bonuses

**Spec disposition: 4-piece T13 = priority set goal.**
- **2P:** Stormstrike chance to grant Stormstrike-buff to extra targets (cleave) OR Stormstrike chance to reset Lava Lash CD — **UNVERIFIED**; both have been reported.
- **4P:** Lava Lash damage increase OR Maelstrom Weapon stack-rate increase — **UNVERIFIED**.

Goal: 4P T13; downgrade to 2P if better off-set tier-bonus stats are blocking gear.

### 4.34. Trinket pairing

**Spec disposition: any on-use Agility trinket aligned with Feral Spirit + Fire Elemental Totem.** Vessel of Acceleration (passive crit), Kiroptyric Sigil (Agility on-use), etc. Bot-side: trinket-firing logic should align with Feral Spirit on-use.

### 4.35. Searing Flames / Flame Shock interaction

**Spec disposition: Searing Totem must be active for Searing Flames to stack on target; Searing Flames stacks REFRESH Flame Shock duration but do not extend it past the cap.**

Mechanical detail: Searing Flames talent gives Searing Totem hits a chance to apply Searing Flames stack on target; up to 5 stacks; each stack pulses fire DoT damage AND refreshes Flame Shock to full duration. This is why Searing Totem is mandatory on single-target — it auto-extends Flame Shock without burning shock-CDs.

**Bot-side: maintain Searing Totem on totem-timer expiration; track Searing Flames stacks as bonus for Flame Shock duration calculation.**

---

## 5. Items NOT addressed (author judgment)

| Spec item | Value | Why |
|---|---|---|
| Lightning Shield | always-on; pre-pull cast; recast on expire | Always-on. |
| Flametongue MH imbue | always-on; pre-pull cast; recast on expire (1hr buff) | Always-on. |
| Windfury OH imbue | always-on; pre-pull cast | Always-on. |
| Searing Totem | totem-timer expire (~60s) | Default. |
| Magma Totem | AoE 4+ enemies; replace Searing Totem | Conditional. |
| Stormstrike | on CD (8s) | Top priority. |
| Lava Lash | on CD (10s) | Top priority. |
| Flame Shock | refresh ≤ 3s remaining OR pull-opener | DoT maintenance. |
| Earth Shock | filler shock when Flame Shock healthy AND shock-CD up | Filler. |
| Maelstrom-Lightning-Bolt | 5 stacks default; 4-stack on movement | Reactive. |
| Maelstrom-Chain-Lightning | 5 stacks AoE 3+ targets | Reactive AoE. |
| Maelstrom-Healing-Wave | 5 stacks AND HP ≤ 30% | Defensive. |
| Unleash Elements | on CD (15s); align before Flame Shock recast for Unleash Flame | CD. |
| Feral Spirit | on CD (2min); first use stacked w/ Bloodlust + Tolvir + Fire Elem | CD. |
| Fire Elemental Totem | on CD (5min); first use stacked w/ Bloodlust | CD. |
| Earth Elemental Totem | defensive only; rare raid use | Defensive. |
| Shamanistic Rage | self HP ≤ 60% OR mana ≤ 30% OR known incoming-damage window | Defensive. |
| Wind Shear | enemy cast ≤ 300ms before completion + 25y range | Reactive interrupt. |
| Hex | caster-add CC | Utility. |
| Spiritwalker's Grace | high-movement phase | CD. |
| Ghost Wolf | out-of-combat travel | Utility. |
| Heroism / Bloodlust | raid CD; bot consumes whatever timing raid provides | Raid CD. |
| Tremor Totem | fear-heavy fight (active CD pulse) | Utility. |
| Cleansing Totem | poison/disease debuff fight | Utility. |
| Healing Stream Totem | passive raid utility | Utility. |
| Stoneskin Totem / Strength of Earth Totem | raid buff (encounter-tunable) | Buff. |
| Pre-pot Tolvir | T-2s + 1-min mark | Bot-side. |
| Stat reforge | Crit/Haste → Mastery; Hit/Exp keep | Standard. |
| Glyph of Stormstrike | Prime — +Stormstrike damage | Default. |
| Glyph of Feral Spirit | Prime — wolves heal-passive | Default. |
| Glyph of Lava Lash | Prime — +Lava Lash damage | Default. |
| Glyph of Shamanistic Rage | Major — self magic-effect cleanse on cast | Default. |
| Glyph of Lightning Shield | Major — extra charges | Default. |
| Glyph of Healing Stream Totem | Major — single-target raid default | Default; swap on AoE. |
| Glyph of Fire Nova | Major — AoE-fight swap | Encounter-tunable. |
| Glyph of Renewed Life | Minor — cosmetic Reincarnation | Cosmetic. |
| Glyph of Astral Recall | Minor — utility CD reduce | Cosmetic. |
| Glyph of Water Shield | Minor — rarely used | Cosmetic. |

---

## 6. Author-judgment items — leveling

```yaml
spell_unlocks:
  # Class baselines (Shaman starts L1; relevant abilities for Enh leveling)
  LIGHTNING_BOLT:            {level: 3, spell_id: 403}
  EARTH_SHOCK:               {level: 4, spell_id: 8042}
  HEALING_WAVE:              {level: 6, spell_id: 331}
  LIGHTNING_SHIELD:          {level: 8, spell_id: 324}
  GHOST_WOLF:                {level: 16, spell_id: 2645}
  FLAME_SHOCK:               {level: 10, spell_id: 8050}
  STONESKIN_TOTEM:           {level: 4, spell_id: 8071}
  EARTHBIND_TOTEM:           {level: 6, spell_id: 2484}
  STRENGTH_OF_EARTH_TOTEM:   {level: 16, spell_id: 8075}
  SEARING_TOTEM:             {level: 12, spell_id: 3599}
  HEALING_STREAM_TOTEM:      {level: 14, spell_id: 5394}
  MANA_SPRING_TOTEM:         {level: 26, spell_id: 5675}
  CHAIN_LIGHTNING:           {level: 28, spell_id: 421}
  WIND_SHEAR:                {level: 36, spell_id: 57994}
  WINDFURY_TOTEM:            {level: 36, spell_id: 8512}
  GROUNDING_TOTEM:           {level: 30, spell_id: 8177}
  CHAIN_HEAL:                {level: 40, spell_id: 1064}
  TREMOR_TOTEM:              {level: 18, spell_id: 8143}
  CLEANSING_TOTEM:           {level: 42, spell_id: 8170}
  PURGE:                     {level: 22, spell_id: 370}
  HEROISM:                   {level: 70, spell_id: 32182}
  BLOODLUST:                 {level: 70, spell_id: 2825}
  HEALING_SURGE:             {level: 7, spell_id: 8004}
  PRIMAL_STRIKE:             {level: 3, spell_id: 73899}  # Replaced by Stormstrike at L30 in Enh
  HEX:                       {level: 40, spell_id: 51514}
  REINCARNATION:             {level: 30, spell_id: 20608}
  ASTRAL_RECALL:             {level: 30, spell_id: 556}
  WATER_WALKING:             {level: 28, spell_id: 546}
  WATER_BREATHING:           {level: 22, spell_id: 131}
  # Cataclysm-new (L80–L85)
  UNLEASH_ELEMENTS:          {level: 81, spell_id: 73680}
  HEALING_RAIN:              {level: 83, spell_id: 73920}  # Resto primarily; available
  SPIRITWALKERS_GRACE:       {level: 85, spell_id: 79206}
  ELEMENTAL_RESISTANCE_TOTEM:{level: 28, spell_id: 8184}
  STONECLAW_TOTEM:           {level: 30, spell_id: 5730}
  # Enhancement-spec talented abilities (level reqs are talent-tier, not character level)
  STORMSTRIKE:               {requires_talent: true, requires_spec: enhancement, spell_id: 17364}
  LAVA_LASH:                 {requires_talent: true, requires_spec: enhancement, spell_id: 60103}
  FERAL_SPIRIT:              {requires_talent: true, requires_spec: enhancement, spell_id: 51533}
  SHAMANISTIC_RAGE:          {requires_talent: true, requires_spec: enhancement, spell_id: 30823}
  MAELSTROM_WEAPON:          {requires_talent: true, requires_spec: enhancement, passive_proc: true}
  STATIC_SHOCK:              {requires_talent: true, requires_spec: enhancement, passive_proc: true}
  MENTAL_QUICKNESS:          {requires_talent: true, requires_spec: enhancement, passive: true}
  SEARING_FLAMES:            {requires_talent: true, requires_spec: enhancement, passive: true}
  IMPROVED_LAVA_LASH:        {requires_talent: true, requires_spec: enhancement}
  IMPROVED_STORMSTRIKE:      {requires_talent: true, requires_spec: enhancement}
  UNLEASHED_RAGE:            {requires_talent: true, requires_spec: enhancement, raid_buff: true}
  TOUGHNESS:                 {requires_talent: true, requires_spec: enhancement}
  FROZEN_POWER:              {requires_talent: true, requires_spec: enhancement}  # Skipped default
  # Fire/Earth Elemental Totems (baseline at L66/58)
  FIRE_ELEMENTAL_TOTEM:      {level: 66, spell_id: 2894}
  EARTH_ELEMENTAL_TOTEM:     {level: 58, spell_id: 2062}
  # Weapon imbues
  FLAMETONGUE_WEAPON:        {level: 12, spell_id: 8024}
  WINDFURY_WEAPON:           {level: 30, spell_id: 8232}
  FROSTBRAND_WEAPON:         {level: 20, spell_id: 8033}
  EARTHLIVING_WEAPON:        {level: 64, spell_id: 51730}  # Resto imbue
  ROCKBITER_WEAPON:          {level: 1, spell_id: 8017}    # Tank imbue (low-level)
  # Mastery / passives
  MASTERY_ENHANCED_ELEMENTS: {passive_mastery: true, base_pct: 16.0, per_point_pct: 2.5, requires_spec: enhancement}
  DUAL_WIELD:                {requires_talent: true, requires_spec: enhancement, passive: true}
  # Cross-tree talents commonly taken (Elemental tree)
  ACUITY:                    {requires_talent: true, tree: elemental}
  CONVECTION:                {requires_talent: true, tree: elemental}
  CONCUSSION:                {requires_talent: true, tree: elemental}
  REVERBERATION:             {requires_talent: true, tree: elemental}
  # Cross-tree talents (Resto tree)
  IMPROVED_SHIELDS:          {requires_talent: true, tree: restoration}
  EARTHEN_POWER:             {requires_talent: true, tree: restoration}
```

**Shaman is a base class — starts at L1.**

**Pre-Stormstrike (under L30):** primary builder is **Primal Strike** (L3 Cata-new instant strike that replaces auto-attack-only leveling for Enh-track Shamans). Primal Strike persists in spellbook but is replaced in priority by Stormstrike at L30. Rotation L1–L29: Lightning Bolt + Primal Strike + Earth Shock + Flame Shock as available.

**Pre-Lava Lash (under L30):** no off-hand spec strike; rotation is Stormstrike + Earth Shock / Flame Shock + Lightning Bolt-on-Maelstrom (after Maelstrom Weapon talented).

**Pre-Maelstrom Weapon (under spec-talent invest):** no instant-cast nature spells; Lightning Bolt is hard-cast 2.5s. Rotation is melee-only plus hard-cast Lightning Bolts when at range / shock-CD up.

**Pre-Mental Quickness (under spec-talent invest):** no AP-to-spellpower conversion; weapon imbues provide raw damage rather than spellpower scaling. Stat priority shifts toward Strength + Agility leveling gear (low-level Shaman gear is mixed-stat).

**Pre-Feral Spirit (under spec-talent invest):** no Spirit Wolves DPS CD; rely on Fire Elemental Totem (L66) once available.

**Pre-Shamanistic Rage (under spec-talent invest):** no defensive CD; rely on Stoneskin Totem (L4) + Healing Stream Totem (L14) for passive mitigation.

**Pre-Lightning Shield (under L8):** no shield buff; cast Earth Shield (Resto-only, L20+) only if Resto-leveled.

**Pre-Wind Shear (under L36):** no interrupt — Earth Shock has a 2s silence component pre-Cata, but in Cata 4.0+ Earth Shock no longer silences (silence was removed; Wind Shear is the dedicated interrupt). Pre-L36, no silence/interrupt option for Enh.

**Pre-Heroism / Bloodlust (under L70):** no raid haste CD.

**Pre-Hex (under L40):** no PvE-CC option; rely on totem-aggro management.

**Pre-Unleash Elements (under L81):** no imbue-release ability; weapon imbues are passive-only.

**Pre-Spiritwalker's Grace (under L85):** no cast-while-moving; movement = instant-cast only (Maelstrom-Lightning-Bolt, shocks).

**Pre-Mastery (under L80):** no Enhanced Elements scaling — nature/fire/frost damage at flat rate.

**Cataclysm Primal Strike note:** Primal Strike is a Cata-new ability that replaces the L1 auto-attack-only Shaman leveling experience. All three specs use Primal Strike pre-L30; at L30, Stormstrike (Enh) replaces it as the primary instant-strike. Bot logic for low-level Shaman bots should cast Primal Strike as the "always-instant melee" until Stormstrike unlocks at L30 via Enh talent investment.

---

## 7. Suggested next research passes

1. Maelstrom Weapon proc rate per-talent-rank (10% / 20% / 30% per swing at 3/3?).
2. Maelstrom Weapon — proc on auto-attack only? OR melee specials too (Stormstrike, Lava Lash, Primal Strike)?
3. Maelstrom Weapon — consume on which spell families (nature only? all instant-castable?).
4. Stormstrike CD — 8s baseline; reduced by Improved Stormstrike?
5. Stormstrike-buff — 8s duration, +25% nature crit on next 2 casts? (verify duration / charge count).
6. Lava Lash flame-shock-spread — 4 targets, 12y radius? Damage to spread targets equal to original or fraction?
7. Lava Lash damage formula (off-hand weapon damage × multiplier; +30% fire damage with Mental Quickness?).
8. Flame Shock duration — 18s base; +6s with Searing Flames refresh?
9. Searing Flames stack-up rate (Searing Totem hit chance to apply per hit?).
10. Searing Totem — 1min duration; ranged auto-attack target priority (closest? lowest HP?).
11. Magma Totem — 1min duration; 8y radius; tick rate 2s?
12. Feral Spirit — 30s duration, 2 wolves, Spirit Hunt heal-passive 150% damage?
13. Shamanistic Rage Cata change — -30% damage taken AND +mana from melee (no longer mana-cost reduction).
14. Mental Quickness — 50% AP → spellpower conversion; +2% spell hit; +30% Stormstrike/Lava Lash/Earth Shock damage?
15. Static Shock proc rate (chance per Lightning Shield charge per melee swing).
16. Searing Flames stacks — visible stack-counter OR hidden DoT overlay?
17. Unleash Elements — 15s CD; Unleash Flame +20% Flame Shock damage (next cast?); Unleash Wind +40% melee haste 8s?
18. Spiritwalker's Grace — 2min CD; 15s; cast-while-moving; affects Maelstrom-Lightning-Bolt OR all spells?
19. Fire Elemental Totem — 5min CD; 2min duration; AP scaling; AoE Fire Nova range?
20. Earth Elemental Totem — 5min CD; 2min duration; tank-pet threat behavior.
21. Glyph of Stormstrike — +Stormstrike damage (X%?).
22. Glyph of Feral Spirit — wolves heal caster (150% damage dealt?).
23. Glyph of Lava Lash — +Lava Lash damage (X%?).
24. Glyph of Shamanistic Rage — magic-effect-cleanse on cast (any debuff school OR specific schools?).
25. Glyph of Lightning Shield — extra charges + duration (specific values?).
26. Glyph of Healing Stream Totem — +healing throughput + +1% raid resist (specific values?).
27. Glyph of Fire Nova — -3s CD on Fire Nova (verify).
28. Maelstrom-Chain-Lightning bounce count — 3 in Cata 4.3.4? (Wrath had 5; Cata reduced; verify).
29. Mastery Enhanced Elements — base 16% + 2.5% per point? formula verification.
30. Wind Shear — 6s CD off-GCD, 25y range, 2s lockout (verify off-GCD).
31. Hex — 35s CD, 60s duration, breaks on damage.
32. Tremor Totem Cata rework — active 1min CD pulsing every 3s (verify behavior).
33. Cleansing Totem — 1min CD pulsing every 2s; removes 1 poison/disease per pulse?
34. Healing Stream Totem — 1% HP/3s pulse; affected by glyph for +healing.
35. Mana Tide Totem — Resto-tree only; Enh does NOT have access (verify exclusion).
36. Tolvir Potion — +1200 Agility 25s; 1min mark for second pot.
37. Lightning Shield — 1hr buff; 9 charges baseline; charge consumed per melee retaliation hit.
38. Flametongue Weapon — 1hr buff; spellpower contribution formula; stacks with weapon proc damage?
39. Windfury Weapon — 1hr buff; proc rate ~20% per swing? +4000 AP for 3 extra hits?
40. Earthliving / Frostbrand — 1hr imbues; Frostbrand frost damage proc rate.
41. Improved Stormstrike — +20% damage AND +20% mana on Stormstrike cast?
42. Improved Lava Lash — +damage to Lava Lash AND +damage to Flame Shock spread?
43. Unleashed Rage — +10% melee/ranged AP raid buff (overlaps Hunting Party / Frost DK Improved Icy Talons).
44. Searing Flames — stack-cap 5; refresh-Flame-Shock-on-hit mechanic (Searing Totem hit OR stack-application?).
45. Static Shock — proc chance % per Lightning Shield charge? mana-restore amount per proc?
46. T13 2P/4P (Enhancement) — exact bonuses.
47. Improved Shields (Resto-tree) — +orb count / +mana-return per orb consumed.
48. Acuity (Elemental-tree) — +3% crit (all schools).
49. Convection (Elemental-tree) — +mana-cost reduction on nature damage spells.
50. Concussion (Elemental-tree) — +damage to Lightning Bolt / Chain Lightning / Lava Burst / Earth Shock / Flame Shock.
51. Reverberation (Elemental-tree) — -CD on shocks (1s? 2s?).
52. Earthen Power (Resto-tree) — Earthbind Totem cleanses movement-impair every pulse.
53. Frozen Power (Enh-tree) — Frostbrand chill targets take +damage (skipped default).

---

## 8. DBC verification

(Spell IDs in §6.)

- Lightning Bolt — baseline. Spell.dbc 403.
- Earth Shock — baseline. Spell.dbc 8042.
- Lightning Shield — baseline. Spell.dbc 324.
- Flame Shock — baseline. Spell.dbc 8050.
- Searing Totem — baseline. Spell.dbc 3599.
- Magma Totem — baseline. Spell.dbc 8190.
- Fire Nova — baseline (replaced Fire Nova Totem in Cata; now an instant-cast on existing totem). Spell.dbc 1535.
- Healing Wave — baseline. Spell.dbc 331.
- Healing Surge — baseline. Spell.dbc 8004.
- Chain Lightning — baseline. Spell.dbc 421.
- Wind Shear — baseline interrupt. Spell.dbc 57994.
- Hex — baseline CC. Spell.dbc 51514.
- Heroism — Horde raid CD. Spell.dbc 32182.
- Bloodlust — Alliance raid CD. Spell.dbc 2825.
- Reincarnation — baseline. Spell.dbc 20608.
- Astral Recall — baseline utility. Spell.dbc 556.
- Ghost Wolf — baseline movement. Spell.dbc 2645.
- Stoneskin Totem — baseline. Spell.dbc 8071.
- Earthbind Totem — baseline snare. Spell.dbc 2484.
- Strength of Earth Totem — baseline raid buff. Spell.dbc 8075.
- Healing Stream Totem — baseline. Spell.dbc 5394.
- Mana Spring Totem — baseline. Spell.dbc 5675.
- Windfury Totem — baseline raid buff. Spell.dbc 8512.
- Tremor Totem — baseline (Cata-rework). Spell.dbc 8143.
- Cleansing Totem — baseline. Spell.dbc 8170.
- Grounding Totem — baseline magic-grounding. Spell.dbc 8177.
- Stoneclaw Totem — baseline absorb. Spell.dbc 5730.
- Fire Elemental Totem — baseline. Spell.dbc 2894.
- Earth Elemental Totem — baseline. Spell.dbc 2062.
- Flametongue Weapon imbue. Spell.dbc 8024.
- Windfury Weapon imbue. Spell.dbc 8232.
- Frostbrand Weapon imbue. Spell.dbc 8033.
- Rockbiter Weapon imbue. Spell.dbc 8017.
- Earthliving Weapon imbue. Spell.dbc 51730.
- Primal Strike (Cata-new L3). Spell.dbc 73899.
- **Stormstrike — Enh talent, 8s CD, 2-weapon hit + Stormstrike-buff. Spell.dbc 17364.**
- **Lava Lash — Enh talent, 10s CD, off-hand fire strike, flame-shock-spread. Spell.dbc 60103.**
- **Feral Spirit — Enh talent, 2min CD, 2 wolves 30s. Spell.dbc 51533.**
- **Shamanistic Rage — Enh talent, 1min CD, -30% damage taken + mana regen 15s. Spell.dbc 30823.**
- **Maelstrom Weapon — Enh talent, passive proc on melee swings. Talent.dbc.**
- **Static Shock — Enh talent, passive Lightning Shield proc on melee. Talent.dbc.**
- **Mental Quickness — Enh talent, AP→spellpower passive, +2% spell hit. Talent.dbc.**
- **Searing Flames — Enh talent, Searing Totem stack DoT + Flame Shock refresh. Talent.dbc.**
- Improved Lava Lash — Enh talent. Talent.dbc.
- Improved Stormstrike — Enh talent. Talent.dbc.
- Unleashed Rage — Enh talent (raid AP buff). Talent.dbc.
- Toughness — Enh talent. Talent.dbc.
- Frozen Power — Enh talent (skipped). Talent.dbc.
- Acuity — Elemental talent. Talent.dbc.
- Convection / Concussion / Reverberation — Elemental talents. Talent.dbc.
- Improved Shields — Resto talent. Talent.dbc.
- Earthen Power — Resto talent. Talent.dbc.
- Mastery Enhanced Elements — Enhancement. Mastery DBC.
- Dual Wield — Enh passive. Talent.dbc.
- Unleash Elements — Cata-new L81. Spell.dbc 73680.
- Spiritwalker's Grace — Cata-new L85. Spell.dbc 79206.
- Healing Rain — Cata-new L83 (Resto-primary; available). Spell.dbc 73920.
- Glyphs (Stormstrike, Feral Spirit, Lava Lash, Shamanistic Rage, Lightning Shield, Healing Stream Totem, Fire Nova, Renewed Life, Astral Recall, Water Shield). Glyph.dbc.
- T13 Enhancement 2P/4P. Item set DBC.
- Tolvir Potion. Item DBC.
- Flame Shock debuff. Spell.dbc.
- Searing Flames debuff. Spell.dbc.
- Stormstrike-buff. Spell.dbc.
- Maelstrom Weapon stack buff. Spell.dbc.
- Unleash Flame buff. Spell.dbc.
- Unleash Wind buff. Spell.dbc.

---

## 9. Notes on prompt deviations

1. **Spec selection: Enhancement (sole Shaman melee DPS).** No spec-vs-spec contest within Shaman melee.
2. **Dual-wield only in Cata 4.0+.** 2H Enhancement was removed; Stormstrike / Lava Lash / Mental Quickness all coded around dual 1H. Bot equip-state hard requirement.
3. **Agility primary stat (NOT Strength).** Enh shamans scale melee AP from Agility — distinct from Warrior / DK / Paladin / Rogue (Combat) which all scale Strength or Agility differently. Bot stat-comparison logic must distinguish.
4. **Maelstrom Weapon as proc engine.** Enh is the **only Cata melee DPS spec with a stacking spellcast-trigger proc**. Bot must track stack count (0–5) as event-driven flag and gate Lightning Bolt / Chain Lightning / Healing Wave on threshold (typically 5).
5. **Mental Quickness AP→spellpower.** Enh's primary spellpower source. 50% of melee AP becomes spellpower; affects Lightning Bolt / Flame Shock / Earth Shock damage. Bot must NOT reforge for spellpower itemization — it scales from AP.
6. **Stormstrike-buff layering.** Stormstrike applies a buff (next 2 nature spells +25% crit). Bot should align Maelstrom-Lightning-Bolt cast inside Stormstrike-buff window when 5 stacks are available — compounding crit on highest-damage nature cast.
7. **Lava Lash flame-shock-spread.** Cleave AoE setup tool. Lava Lash spreads Flame Shock to up to 4 targets within 12y if Flame Shock is on primary. Bot AoE logic should cast Flame Shock first, then Lava Lash to spread.
8. **Searing Flames refresh chains.** Searing Totem hits on target stack Searing Flames; each stack refreshes Flame Shock duration. This is why Searing Totem is mandatory — auto-extends Flame Shock without burning shock-CDs. Bot must maintain Searing Totem on totem-timer expire.
9. **Shock CD bucket.** Flame Shock, Earth Shock, Frost Shock share a 6s CD bucket (with Reverberation talent reducing further). Bot must NOT model these as independent CDs.
10. **Talent split 31/7/3 default.** Variants 31/3/7 exist for Improved Shields cross-tree depth; default raid build is 31/7/3.
11. **Stat priority Mastery > Haste > Crit (consensus).** Minority view (G6) places Haste first; default to Mastery.
12. **Spell hit cap 8% (not 17%).** Most guides target 8% melee hit; spell hit floats below 17% intentionally because the itemization cost of full spell hit cap exceeds the value of Maelstrom-Lightning-Bolt never missing.
13. **Lightning Shield mandatory.** Always-on 1hr buff; Static Shock proc + Mental Quickness damage scaling both gate on Lightning Shield active. Bot must pre-pull cast and recast on expire.
14. **Flametongue MH + Windfury OH default imbues.** Alternate imbue setups exist (Windfury MH / Frostbrand) but consensus rejects on simulation grounds.
15. **Unleash Elements (Cata-new L81)** — releases active weapon imbue effects. Unleash Flame (MH=Flametongue): +20% next Flame Shock damage. Unleash Wind (OH=Windfury): +40% melee haste 8s. Bot casts on CD; effect depends on imbue state.
16. **Feral Spirit major DPS CD (2min).** Spirit Wolves 30s duration; benefit from raid haste; align with Bloodlust.
17. **Fire Elemental Totem major DPS CD (5min).** 2min duration fire pet; align with Bloodlust + Feral Spirit on first use.
18. **Shamanistic Rage Cata change.** No longer mana-cost reduction; now -30% damage taken + mana regen from melee. Defensive primary.
19. **Wind Shear 6s CD 25y range.** Wider range than most melee interrupts; bot can interrupt mid-room casters from boss melee distance.
20. **Maelstrom-Healing-Wave self-heal.** At 5 stacks, instant Healing Wave (~25–35% HP heal). Bot at HP ≤ 30% should consume Maelstrom on Healing Wave instead of Lightning Bolt.
21. **Spiritwalker's Grace (L85).** Cast-while-moving 15s; 2min CD. Movement-phase utility.
22. **Glyph of Healing Stream Totem default Major; swap to Glyph of Fire Nova on AoE.** Encounter-tunable.
23. **Tolvir Potion (Agility) — pre-pot + 1-min mark.** NOT Volcanic Potion (intellect — irrelevant for Enh).
24. **Bot-side: Maelstrom Weapon stack tracking** is event-driven, not timer-based. Each melee swing has a chance to add a stack; stacks decay only on consumption (not timer).
25. **Bot-side: Stormstrike-buff window detection** for Maelstrom-Lightning-Bolt timing — buff lasts 8s OR 2 nature spells consumed, whichever first.
26. **Bot-side: Flame Shock pandemic with Searing Flames refresh.** Auto-refresh from Searing Totem hits means manual Flame Shock recasts are rare on single-target with totem up. Bot should track combined duration (base + Searing Flames refresh).
27. **Bot-side: Lava Lash AoE-spread gate.** Lava Lash always cast on CD; the spread effect only triggers if Flame Shock is on primary AND there are nearby enemies. No extra gating needed — Lava Lash damage alone justifies the cast.
28. **Bot-side: shock-CD bucket** — Flame Shock / Earth Shock / Frost Shock share one CD. Bot priority within shock slot: Flame Shock (refresh ≤ 3s) > Earth Shock (filler).
29. **Bot-side: totem-timer maintenance.** Searing Totem expires after ~60s; bot must recast on totem-timer expire OR on totem death (totems can be killed by AoE in some fights).
30. **Bot-side: Feral Spirit + Fire Elemental Totem stacking.** Both are major DPS CDs; both align with Bloodlust on the pull. Subsequent casts drift independently.
31. **Bot-side: weapon-imbue maintenance.** Flametongue MH + Windfury OH are 1hr buffs; bot should pre-pull cast and recast on expire (rare in raid; common on boss-wipe + repull cycles).
32. **Bot-side: Hex CC** — caster-add gating; bot should use Hex on caster-type adds during fights with multiple add waves.
33. **Bot-side: Wind Shear 25y range** — interrupt logic should prioritize ranged casters (out of melee range of other classes) since Enh has the longer interrupt range.
34. **Bot-side: dual-wield equip check** — Enh gates on `weapon_main_hand_1h && weapon_off_hand_1h && weapon_off_hand != shield`. Stormstrike / Lava Lash will fail if either equipment slot is wrong.
35. **Bot-side: Unleash Elements imbue-dependency** — Unleash Flame requires Flametongue MH; Unleash Wind requires Windfury OH. Bot must verify imbue state before casting and prioritize the haste effect (Unleash Wind) when both are available.
36. **Bot-side: Magma Totem vs Searing Totem switch** — based on enemy count. AoE 4+ targets → Magma; otherwise Searing.
37. **Bot-side: Fire Nova trigger** — gate on count_flame_shocked_enemies_in_10y >= 3. Below 3, Fire Nova is a DPS loss.
38. **Cross-spec shared mechanics** — Lightning Shield, Searing Totem, Hex, Wind Shear, Heroism/Bloodlust, Tremor/Cleansing/Healing Stream Totems, mana resource model are documented in `docs/specs/elemental-shaman.md` and `docs/specs/resto-shaman.md`. Reuse encoded mechanics from those specs; do NOT re-encode.

End of survey.
