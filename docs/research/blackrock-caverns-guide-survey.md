# Blackrock Caverns — Cataclysm 5-man Survey

**Date:** 2026-05-02
**Patch:** 4.3.4
**Audience:** bot-control engineer (cata-altbot)

---

## Overview

| Field | Value |
|---|---|
| Zone | Blackrock Caverns (northern Blackrock Mountain, Searing Gorge) |
| Continent | Eastern Kingdoms |
| Level range (normal) | 80–82 (entry-level Cata 5-man) |
| Level range (heroic) | 85 |
| Group size | 5 (1 tank / 1 healer / 3 DPS) |
| Boss count | 4 required + 1 optional (Beauty) |
| Layout | Mostly linear with one optional branch (Beauty's chamber) |
| Cata-launch class | Yes — released alongside Throne of the Tides as the two pre-Heroic-gating leveling 5-mans |
| Group buffs/debuffs | None unique to instance (no instance-wide aura) |

The instance is short (~25-30 min on heroic for a geared group, ~45 min on first launch-tier clears). Trash density is moderate; the path is "one elevator down → first room (Rom'ogg) → ramp tunnel (Beauty branch + Corla room) → forge (Karsh) → throne room (Obsidius)." Beauty's chamber forks off the main spine and can be skipped without consequence.

---

## Sources

This survey reconciles the following community sources. Wowhead Cata is treated as authoritative for spell IDs and numeric values; strategy is cross-checked across 4 guides.

1. **Wowhead Cata — encounter & NPC pages** (`/cata/npc=...`, `/cata/spell=...`) — primary for spell IDs, cast times, cooldowns, schools, debuff durations
2. **Icy Veins — Blackrock Caverns Strategy Guide** (`icy-veins.com/cataclysm-classic/blackrock-caverns-dungeon-strategy-guide`)
3. **Warcraft Tavern — Blackrock Caverns Guide** (`warcrafttavern.com/cataclysm-classic/guides/blackrock-caverns/`)
4. **Wowhead Cata Guide — Blackrock Caverns Walkthrough** (community guide attached to the zone page)
5. **Wowtbc.gg / Cata-Tavern strat notes** (cross-reference for heroic delta + add-spawn timings)

Where guides disagree, the disagreement is captured in the **Disagreements** section and the consensus position is listed in **Strong consensus**.

---

## Strong consensus (table)

| Mechanic | Consensus |
|---|---|
| Rom'ogg Skullcracker | **Spread is wrong** — group **stacks** to share the channel damage; some sources still describe it as shareable rather than spread, and the "Chains of Woe → stack → Skullcracker" chain is the canonical sequence |
| Rom'ogg Quake | Interruptible (Bash/Kick) on heroic; on normal it is survivable but interrupt is preferred |
| Rom'ogg Bonecrusher adds (Angered Earth) | Tank picks up; AoE-burn before they reach group; 4 spawn at 50% HP |
| Corla beams | Three drakonids channel, three players (typically the 3 DPS) stand in beams to block them; the player accumulates Evolution stacks and **must step out before 5 stacks** (heroic: 5 stacks transforms / kills the player) |
| Corla beam blocker rotation | **Required on heroic**, optional on normal — second player swaps in around stack 3-4 |
| Karsh kiting | Tank drags Karsh **through** the central pillar/lava plume to apply Quicksilver Armor (damage reduction debuff) repeatedly — every ~10s |
| Karsh DPS pattern | DPS stays out of lava; **only the tank** stands in / passes through the lava plume |
| Karsh Cinderbreath cone | Frontal cone — melee from the side/rear; ranged out of the cone arc |
| Beauty — optional | Skippable; clearable for loot (notable trinket on heroic) |
| Beauty pups | 3 named pups (Lucky, Buster, Spot) precede Beauty in the same chamber but are pulled before her |
| Obsidius Crepuscular Veil | **Magic** debuff applied to a non-tank; clones spawn that need to be tanked or kited — **not** dispelled in standard strat |
| Obsidius shadow puddles | Three Shadow of Obsidius adds spawn around the room; they swap target with Obsidius and the tank manages the swap |
| Obsidius tank-swap | Required on heroic; threat-swap mechanic where Obsidius and clones rotate aggro |
| Heroic-only dispel pressure | Beam stack management on Corla, faster Karsh kite, harder Obsidius clone swaps. There is **no** instance-wide curse/poison dispel requirement |
| Curse/Disease/Poison dispel needs | None of the bosses require curse, disease, or poison dispels — only Magic on Obsidius (and even that is mostly tanked-through, not dispelled) |

---

## Boss 1 — Rom'ogg Bonecrusher

NPC ID: 39665. First boss, ground-floor chamber after the elevator. Encounter is single-phase with periodic add waves.

### Phase structure

Single phase, on a repeating cycle:

1. Tank-and-spank with periodic **Quake** stomps (interruptible)
2. At ~50% HP, **Call Bonecrushers** — 4 Angered Earth adds spawn from corners
3. Repeating **Chains of Woe → Skullcracker** combo throughout the fight (every ~30s)

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Quake | 75272 (UNVERIFIED — see DBC list) | ~2s cast, ~10-15s cd | Physical/Nature | **Interruptible.** Stomp-style AoE; ticks on hardcast. Bash/Kick/Pummel/Counterspell to cancel. |
| Skullcracker | 75543 (UNVERIFIED) | 6s channel | Physical | Channeled AoE; **damage is split among players in range**, so group **stacks under boss**. Heavy hit if soloed. |
| Chains of Woe | 75539 (UNVERIFIED) | ~1.5s cast | Physical | Pulls all players to a fixed point near the boss; immediately precedes Skullcracker. Cannot be interrupted reliably (short cast). |
| Wounding Strike | (tank) | melee | Physical | Tank-only DoT; healer pressure |
| Call Bonecrushers / Angered Earth adds | 75319 (summon) | scripted | — | 4 small earth elementals spawn at ~50% HP on normal; on heroic, additional waves |

### Interrupts required

- **Quake** — primary kick target. Bot interrupt priority: HIGH.
- Chains of Woe technically has a cast bar, but the cast is short and the mechanic is intended to fire (it sets up Skullcracker). Do not interrupt.

### Dispels required

None. No curse/disease/poison/magic dispel calls in this fight.

### Avoidable damage

- The Skullcracker channel is **damage-shared**, not avoidable. The "avoid" is meta — don't get caught alone (out of stack range) when it ticks.
- Stomp (Quake) is unavoidable AoE if not interrupted.

### Tank-swap requirements

None. Single tank.

### Heroic delta

- Additional add waves (some guides report 2 waves, others a single larger wave at 50% — see **Disagreements #1**)
- Quake hits harder; interrupting becomes mandatory rather than optional
- Skullcracker damage is high enough that missing the stack will likely kill the straggler

### Add-handling

- Angered Earth elementals: **tank picks up** via taunt or AoE threat; DPS AoE-burns. They are low-HP. No CC needed.
- They have a small AoE damage aura on death — don't melee them while at 1 HP if it can be helped (low priority concern).

### Healer-pressure spikes

- Skullcracker (predictable: ~6s before the channel resolves, healer pre-HoTs / pre-shields)
- Add wave at 50% (predictable)

### Key positioning

- **Stack on boss** — entire group stacks at melee range so Skullcracker shares damage and Chains of Woe doesn't move anyone unexpectedly
- Tank facing irrelevant (no frontal cone)
- Range can stay at melee for damage-sharing; **this is an exception to the normal "ranged spread" pattern**

### Bot-relevant role notes

- **Tank**: hold threat through Chains of Woe (boss does not lose threat, but adds will go for closest non-tank). Position centrally so all players will end up stacked after Chains of Woe.
- **Healer**: pre-stack damage is the predictable spike. AoE heal cooldown lined up to Skullcracker tick is ideal.
- **Melee**: in melee already; nothing special. Interrupt Quake on rotation.
- **Ranged**: **stack with melee** for Skullcracker — this is unusual; ranged bots cannot use their default "stay at max range" positioning. Override needed for this fight.

---

## Boss 2 — Corla, Herald of Twilight

NPC ID: 39679. Second boss, encountered before Karsh in a circular chamber. Three Twilight Drakonids stand on platforms, each channeling a beam at Corla.

### Phase structure

Two-phase:

1. **Beam phase** — Corla is invulnerable / heavily damage-reduced while drakonids channel beams; players interrupt the beams by standing in them, accruing **Evolution** stacks. When all 3 drakonids are killed (or beams stop), Corla becomes attackable.
2. **Corla burn** — straightforward tank-and-spank after beams are dealt with.

Some guides describe this as a single-phase encounter where Corla is killable from the start with the drakonids handled in parallel. The mechanical reality is the same: beams must be intercepted, drakonids killed, Corla burned. See **Disagreements #2**.

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Evolution (channel beam) | 75610 (UNVERIFIED) | continuous channel | Shadow | Drakonid → Corla beam. Player standing in beam intercepts, taking damage and gaining a stacking debuff. |
| Twilight Evolution (debuff on player) | 75571 (UNVERIFIED) | stacking debuff | Shadow | At ~5 stacks the player **transforms into a Twilight Drakonid** (becomes hostile, effectively dead to the group). Must step out of beam before 5 stacks. Stacks fall off after ~10s of being out of the beam. |
| Dark Command (Corla) | 75823 (UNVERIFIED) | melee | Shadow | Corla auto-attack DoT-flavored melee on tank |
| Drakonid melee | — | — | Physical | Standard caster-melee |

### Interrupts required

The beams themselves are not "interrupts" in the kick-cast sense — they are **intercepted** by body-blocking. There are no kick-required casts on this fight. (Corla has no hardcasts that demand interrupting on heroic.)

### Dispels required

None. The Evolution debuff is **not dispellable** in any standard strat — it is managed by stepping out of the beam.

### Avoidable damage

- Standing in a beam past the "swap out" stack count → death/transform.
- Standing in **two** beams simultaneously (overlap zone) → stacks rise twice as fast. Avoid.

### Tank-swap requirements

None.

### Heroic delta

- Stack threshold for transformation is unchanged (5), but **incoming beam damage is higher**, putting healer pressure on whoever is intercepting
- Beam-rotation is mandatory on heroic — one player cannot solo a beam to 5 stacks safely. **Two players per beam, swapping at ~3 stacks**, is the canonical heroic strat.
- Drakonid HP pool higher; if drakonids aren't killed before Corla's other mechanics, healer drains OOM

### Add-handling

- Three Drakonids on raised platforms. They are stationary while channeling.
- **Kill priority: drakonids before Corla** — beams stop when they die, removing the swap pressure.
- DPS focuses one drakonid at a time; whichever DPS is intercepting that drakonid's beam can DPS the drakonid in melee (some guides recommend this; see **Disagreements #3**).

### Healer-pressure spikes

- Beam intercept damage is steady, not spiky
- Heroic: spike is whoever holds the beam longest before swap. Pre-shield the swap-in.

### Key positioning

- Three platforms arranged around Corla; beams travel from platform → Corla
- Player intercepting a beam stands **between drakonid and Corla**, in the beam path
- **Tank holds Corla still** in the center so beam paths remain predictable
- Ranged DPS and healer can stand at any safe spot; melee on Corla after drakonids die

### Bot-relevant role notes

- **Tank**: pin Corla in the center, do not chase; tank one drakonid in melee if you take the leftover beam.
- **Healer**: rotate cooldowns onto whoever is currently in beam — this is a predictable damage source.
- **Melee**: typically takes one beam (closest platform) and intercepts there, melee'ing the drakonid in beam range. This is the bot-worst case — melee bot must stand still **in a hostile-channeled beam** for ~6-10 seconds.
- **Ranged**: same — intercept assigned beam. Cast-while-moving classes (Shaman with Lava Surge, Hunter, Frost mage) handle this best. Casters may need to stop-cast briefly when stepping in/out of beam.
- **Beam-assignment** is the central bot decision: needs an explicit "I am responsible for beam X" state.

---

## Boss 3 — Karsh Steelbender

NPC ID: 39698. Third boss, in the central forge after Corla. The fight room is a circular platform with a **central magma pillar / lava plume**.

### Phase structure

Single phase. The fight is structured around the tank repeatedly dragging Karsh through the central magma plume to apply a stacking damage-reduction debuff (**Quicksilver Armor**).

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Quicksilver Armor (boss buff) | 75842 (UNVERIFIED) | passive | — | Karsh has a stacking damage-**reduction** buff that needs to be removed. Walking through the lava plume strips/replaces stacks (mechanic is "lava cleanses Quicksilver"). The exact wording differs across guides — see **Disagreements #4**. |
| Cinderbreath | 75763 (UNVERIFIED) | ~2s cast, ~12s cd | Fire | Frontal **cone** AoE breath. Avoidable by standing behind Karsh. **Not interruptible** in most reports. |
| Lava bath (Searing Lava) | 75852 (UNVERIFIED) | passive | Fire | The central plume puts a fire DoT on the tank when standing in it (~3-5s exposure required to apply Quicksilver debuff to boss). |
| Melee | — | — | Physical | Standard tank-melee |

### Interrupts required

None — Cinderbreath is generally reported as **non-interruptible**, and there are no other hardcasts. (One guide source claims Cinderbreath is interruptible on normal; treat as **UNVERIFIED**.)

### Dispels required

None.

### Avoidable damage

- **Cinderbreath cone** — frontal; ranged/healer stay out of arc; melee position behind/side
- **Lava plume** — only the tank stands in it, and only briefly. DPS/healer never stand in it.
- **Lava patches on the floor** (some guides describe ground patches dropped during the fight; others don't mention this — see **Disagreements #5**)

### Tank-swap requirements

None. Single tank kites Karsh through the plume repeatedly.

### Heroic delta

- Quicksilver stacks build faster on Karsh; tank must drag through the plume more frequently (~every 8-10s rather than 12-15s)
- Cinderbreath hits harder — ranged punished for sloppy positioning
- Healer must heal the tank through repeated lava exposure; this is the canonical **OOM check** of normal-mode Cata levelling for new healers

### Add-handling

None during the fight. (Trash leading up to Karsh includes Twilight Elementalists with frontal channels; addressed in **Trash** section.)

### Healer-pressure spikes

- **Tank in lava** is the steady drain; healer plans mana for ~6-8 lava-passes
- Cinderbreath splash if range positioning is off

### Key positioning

- **Tank**: drag boss through the **center plume** repeatedly. Do not park Karsh inside the plume — pass through, then pull him back out so DPS/healer don't take collateral.
- **Melee DPS**: behind boss, **outside the plume**. Don't follow the tank through.
- **Ranged DPS**: at range, **out of the cone arc**, **off the plume**.
- **Healer**: at range, free-positioning; cone-aware.

### Bot-relevant role notes

- **Tank**: the unusual behavior is "deliberately stand in environmental hazard." Bot tank logic must distinguish "lava plume = damage I should take to apply Quicksilver" from generic "lava = avoid." This is a fight-specific override, not a general rule.
- **Healer**: tank takes predictable fire damage spikes — pre-HoT before the plume pass.
- **Melee**: avoid following tank into the plume. Default "stay behind boss" behavior is correct, but **do not chase the tank through the plume** during the kite.
- **Ranged**: maximize range and stay clear of cone. Standard ranged behavior is correct.

---

## Boss 4 — Beauty (optional pre-boss)

NPC ID: 39700. Optional encounter on a side branch off the main spine. Large bear-dog with three named pup adds standing in the same chamber.

### Phase structure

Linear: clear the 3 pups first (each individually pulled), then Beauty.

### Abilities (Beauty)

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Berserker Charge | 76030 (UNVERIFIED) | instant | Physical | Charges a random player; **stuns** on impact. Treat like a charge mechanic (Fear-style movement disruption). |
| Terrifying Roar | 76028 (UNVERIFIED) | ~2s cast, ~30s cd | Shadow | **Fear** AoE on group. **Tremor Totem / Fear Ward / Berserker Rage / Will of the Forsaken** counter. Some guides list this as interruptible — see **Disagreements #6**. |
| Flamebreak | 76032 (UNVERIFIED) | instant | Fire | Fire AoE around boss; stand at range. |
| Magma Spit | (pup) | ranged cast | Fire | One of the named pups ranged casts |
| Little Big Flame Breath | (pup) | frontal cone | Fire | Another pup; cone-cleave |
| Magma Trail | (pup) | passive | Fire | One pup leaves a fire trail; kite |

### Pups (clear before Beauty)

- **Lucky, Buster, Spot** — three named pups; each has a single named ability. Pull individually, each is a soft tank-and-spank.
- Some guides recommend pulling all three at once on heroic to save time; see **Disagreements #6**.

### Interrupts required

- Terrifying Roar — **possibly** interruptible (UNVERIFIED on normal; guides disagree).

### Dispels required

- **Fear** is not a "dispel" in the classic sense — it is broken by Tremor Totem, Fear Ward, Will of the Forsaken, Berserker Rage, or PvP trinket. Standard dispel system does not remove fear.

### Avoidable damage

- Flamebreak (move out)
- Pup magma trail (don't stand in fire)

### Tank-swap requirements

None.

### Heroic delta

- Pups hit harder; Berserker Charge stun is longer; Terrifying Roar fear is harder to break without an off-class fear-counter
- **Loot**: heroic Beauty drops a notable trinket (Tia's Grace per some sources; others list different items — UNVERIFIED, not bot-critical)

### Healer-pressure spikes

- Charge stun + immediate melee hit on the charge target
- Fear (4-6s of unhealable group movement)

### Key positioning

- Tank pulls Beauty to the side of the chamber so charges don't path through the group
- Melee in melee; ranged spread (so charge doesn't multi-hit)

### Bot-relevant role notes

- **Tank**: tank-and-spank.
- **Healer**: be ready for sudden movement (group fears + charge); pre-emptive HoTs help.
- **Melee**: stay behind boss; brace for charge stuns hitting random players.
- **Ranged**: spread for charge; cast-while-moving when feared (lol — fear breaks casts).

This encounter is **optional and skippable**. For an entry-level / leveling LFD bot, default behavior should be **skip Beauty** unless explicitly told to clear (group leader calls for it).

---

## Boss 5 — Ascendant Lord Obsidius

NPC ID: 39705. Final boss. Throne-room style chamber at the end of the instance. Three **Shadow of Obsidius** adds patrol around him pre-pull.

### Phase structure

Single phase, but with a recurring tank-swap event:

1. Pull Obsidius; tank holds threat
2. Periodically Obsidius **applies a debuff (Twilight Corruption / Crepuscular Veil)** to a non-tank, and the **Shadow of Obsidius** clones become aggressive
3. The clones **swap places / aggro** with Obsidius (or fixate the debuffed player) — tank picks up the new "real" target
4. Repeat until kill

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Twilight Corruption (debuff on tank) | 76188 (UNVERIFIED) | applied on melee | Shadow | Stacking on tank; **tank-swap is partly driven by stack count**. Some guides claim this stacks to 3 before swap; others claim swap is on the clone-pull mechanic regardless of stacks. See **Disagreements #7**. |
| Crepuscular Veil (debuff on player) | 75476 (UNVERIFIED) | scripted | Magic | Applied to a random non-tank; visually shrouds them. Most guides say **do not dispel** — the debuff is part of the swap mechanic. |
| Shadow Prison / Shadow puddles | 75763 (UNVERIFIED) | passive | Shadow | The 3 Shadow of Obsidius adds leave a slow-moving shadow zone. Standing in shadow can be defensive (some guides), or hostile (others). See **Disagreements #8**. |
| Stomp / melee | — | — | Physical | Standard boss melee |

### Interrupts required

None confirmed. No hard-cast interrupt-required spells.

### Dispels required

- Crepuscular Veil is **Magic** school per most listings, but the standard strat is **do not dispel** — it's meant to time with the clone-swap. Bot dispel logic should **explicitly exclude this debuff from auto-dispel** during the encounter.

### Avoidable damage

- Shadow puddles (UNVERIFIED whether they damage — see **Disagreements #8**)
- Standard melee positioning (don't stand in front for cleave)

### Tank-swap requirements

**Yes.** The exact trigger is debated:

- **Strat A**: tank-swap when Twilight Corruption hits N stacks (UNVERIFIED stack count, often listed as 3)
- **Strat B**: tank-swap when Obsidius "becomes" a clone (the clones rotate which is the "active" boss, and the off-tank picks up the new active one)
- **Strat C**: it's a soft swap — main tank handles all of it on normal; off-tank is unnecessary

A 5-man only has one tank in standard composition, so the "swap" is **the tank picking up the new clone-as-boss**, not an OT/MT rotation. Heroic punishes failed pickup (clones cleave the group).

### Heroic delta

- Faster clone-pull cycle
- More damage from clones
- Crepuscular Veil applies more frequently; group must position so the affected player isn't in cleave range
- Some guides list a heroic-only **dispel timer** — a window in which Crepuscular Veil **must** be dispelled, contradicting normal-mode "don't dispel" advice. This is **UNVERIFIED** and a candidate for **Disagreements #9**.

### Add-handling

- 3 Shadow of Obsidius adds — pre-existing in the room, **do not need to be killed** in the standard strat
- They function more like environmental hazards / mechanic-triggers than killable adds

### Healer-pressure spikes

- Tank Twilight Corruption stacks → tank takes ramping melee damage
- Crepuscular Veil player → shadow damage / vulnerability

### Key positioning

- Tank in the center of the room, clones positioned at the back
- Ranged/healer at max range, spread to avoid clone cleave on swap
- Avoid the corners where Shadow puddles tend to congregate

### Bot-relevant role notes

- **Tank**: this fight requires **target-swap awareness**. When Obsidius "becomes" a clone, the tank's previous target reference is now the wrong target. Bot tank must re-acquire the active boss/clone.
- **Healer**: **do not auto-dispel Magic** during this encounter — the Crepuscular Veil debuff is intentional. Add an instance-level dispel-blacklist for this spell ID.
- **Melee**: if you become the Crepuscular Veil target, **move out of the group** to avoid cleave.
- **Ranged**: max range, spread; recover quickly when target changes.

---

## Trash mechanics

The critical trash (skippable noted; mandatory mobs are anything blocking the spine).

### Trash before Rom'ogg

- **Twilight Drakonids** — the same mob type as Corla's beam casters, here as standard mobs. Pull individually or in pairs.
- **Twilight Elementalists** — caster trash. **Frontal channels** (Lava Spit-style); turn them away from the group. **Interruptible**.
- **Twilight Torturers** — melee with a fear/stun on cast. **Interruptible**.

### Trash between Rom'ogg and Beauty/Corla

- **Crazed Mage** packs — caster trash with **Fireball** hardcasts. Interrupt to stagger. Sheep/CC valid.
- **Conflagrations** (eyes-of-flame ground patches) — environmental, not a mob; visible on floor; stand-out-of.

### Trash before Karsh

- **Twilight Elementalist** packs in the forge approach
- **Quicksilver Tank** large mobs — **frontal cleave**, single-target tank-and-spank; **side-position melee**
- **Lava Bursts** environmental — small fire patches near the forge

### Trash before Obsidius

- Patrolling **Twilight Sadists** — fear caster (interrupt!)
- **Twilight Drakonids** in last hallway

### CC priorities

- **Polymorph / Hex / Fear / Sap** valid on Twilight humanoids (Elementalists, Torturers, Mages, Sadists)
- Bonecrusher / Drakonid types are not standard CC targets (some are immune; some are too HP-low to bother)

### Skip-able trash

- **Beauty's room is the main optional branch** — the chamber and its contents (Beauty + 3 pups + ambient mobs) can be entirely bypassed. Many speed-clear groups skip Beauty.
- A few mob clusters between Rom'ogg and Corla can be **wall-hugged / LoS-skipped**, but this is dependent on group movement speed; most mixed-skill groups clear them.

### Pull markers for CC

- Twilight Elementalists in 3-pull groups: assign skull (kill order) + cross/X (CC) + square (CC).
- Crazed Mage packs: skull on lead caster, CC the second.

---

## Disagreements

Numbered list of points where the 4-5 reconciled guides do not agree. Bot logic should pick a default and note the deviation.

### Disagreement #1 — Rom'ogg add-wave count on heroic

- **Source A (Icy Veins):** "1 wave at 50%, 4 adds"
- **Source B (Warcraft Tavern):** "Multiple waves throughout, ~every 30s after 75% HP"
- **Source C (Wowhead community guide):** "Single wave at 50%, AoE-burn"

**Default:** assume single wave at 50%, but program AoE-burn behavior to trigger on add-spawn rather than HP-threshold so additional waves are handled the same way. Resolves whether the disagreement matters or not.

### Disagreement #2 — Corla single-phase or two-phase

- **Source A:** Corla is invulnerable while drakonids channel; killable only after all 3 die
- **Source B:** Corla is killable from start, takes reduced damage while channeled

**Default:** treat as "drakonid kill priority absolute" — irrespective of which model is correct, the right behavior is "kill drakonids first." Skip Corla DPS until at least one drakonid is dead.

### Disagreement #3 — Should beam-blocker also melee the drakonid?

- **Source A:** Yes — beam intercept positions the player in melee of the drakonid; DPS the drakonid while in beam
- **Source B:** No — focus only on intercepting; the rest of the group kills drakonids in turn

**Default (heroic):** **No.** The beam-intercepter is taking damage; let the rest of the group kill the drakonids. On normal, both work; preferring the safer "focus only on intercepting" simplifies bot logic.

### Disagreement #4 — Karsh Quicksilver Armor mechanic wording

- **Source A:** Quicksilver Armor is a **boss buff** that grants damage **reduction**; lava plume **dispels/strips** stacks
- **Source B:** Quicksilver Armor is a **stacking debuff/buff swap**; lava plume **applies** a removable debuff that lets DPS happen
- **Source C (Wowhead spell page):** description is ambiguous — passive on the NPC

**Default:** the **mechanical outcome** is identical — tank passes Karsh through the lava periodically; DPS happens efficiently afterward. Bot tank only needs the rule "drag boss through plume every ~10-12s on normal, ~8-10s on heroic." The exact buff/debuff direction doesn't matter for bot logic.

### Disagreement #5 — Lava patches on Karsh fight

- **Source A:** Karsh drops periodic lava patches on the floor (avoid)
- **Source B:** The plume is the only fire hazard; no additional patches

**Default:** treat all visible fire on the floor as "don't stand in" except the central plume that the **tank** stands in.

### Disagreement #6 — Beauty fight: pull pups individually or together; Terrifying Roar interruptible?

- **Source A:** pull pups one at a time on normal; together on heroic for speed
- **Source B:** always pull individually on heroic too — pups have unique mechanics that compound badly
- **Source A:** Terrifying Roar interruptible
- **Source B:** Terrifying Roar **not** interruptible

**Default:** pull pups individually (safer); attempt interrupt on Roar but do not assume success.

### Disagreement #7 — Obsidius Twilight Corruption stack count for swap

- **Source A:** swap at 3 stacks
- **Source B:** swap at 5 stacks
- **Source C:** swap is on the clone-rotation event, stacks are flavor

**Default:** use the **clone-rotation event** as the swap trigger (most reliable to detect). Stack counts are an UNVERIFIED secondary signal.

### Disagreement #8 — Obsidius shadow puddles: helpful or harmful?

- **Source A:** Shadow zones reduce damage taken (defensive — stand in)
- **Source B:** Shadow zones are passive damage/visual effect (avoid)

**Default:** **avoid** the puddles unless explicitly directed otherwise. Treating them as harmful is the safer assumption and matches the more recent guides.

### Disagreement #9 — Heroic Obsidius dispel-Veil window

- **Source A:** there is a "heroic dispel timer" where Crepuscular Veil **must** be dispelled within X seconds
- **Source B:** never dispel Crepuscular Veil; the swap mechanic handles it
- **Source C:** dispel is optional; situational

**Default:** **never auto-dispel Crepuscular Veil**. If a heroic dispel timer exists, the cost of missing it is lower than the cost of breaking the swap mechanic, and bot healers shouldn't be making frame-perfect dispel timing decisions anyway.

---

## DBC verification (UNVERIFIED items)

Spell IDs and stack values listed below are from community-guide cross-reference, not direct DBC lookup. Add to `docs/research/dbc-verification-checklist.md` for resolution when the TC fork session opens.

| # | Spell / mechanic | Listed value | Why it matters for bot logic |
|---|---|---|---|
| 1 | Rom'ogg Quake | 75272 | Interrupt priority list — bot needs the real spell ID |
| 2 | Rom'ogg Skullcracker | 75543 | Damage-share AoE detection — bot stack-positioning trigger |
| 3 | Rom'ogg Chains of Woe | 75539 | Pre-Skullcracker stack-up signal |
| 4 | Corla Evolution beam | 75610 | Bot beam-intercept role assignment |
| 5 | Corla Twilight Evolution debuff | 75571 | Stack-count check for swap-out timing |
| 6 | Corla beam stack threshold | 5 stacks | Bot swap-out at 3-4; verify ceiling |
| 7 | Karsh Quicksilver Armor | 75842 | Tank kite-through trigger |
| 8 | Karsh Cinderbreath | 75763 | Cone avoidance + verify interruptibility |
| 9 | Karsh Searing Lava | 75852 | Tank "stand in this fire" exception flag |
| 10 | Karsh kite cadence | ~10s normal / ~8s heroic | Tank kite-frequency timer |
| 11 | Beauty Berserker Charge | 76030 | Charge-target prediction |
| 12 | Beauty Terrifying Roar | 76028 | Fear-counter trigger; verify interruptibility |
| 13 | Beauty Flamebreak | 76032 | AoE-avoid trigger |
| 14 | Obsidius Twilight Corruption | 76188 | Tank-swap stack signal |
| 15 | Obsidius Crepuscular Veil | 75476 | **Dispel-blacklist entry** — high importance |
| 16 | Obsidius shadow puddle effect ID | 75763 (likely collision with #8) | Confirm puddle is hostile vs defensive |
| 17 | Obsidius Twilight Corruption swap stack count | 3 vs 5 | If used as primary swap signal, must be exact |
| 18 | Heroic Obsidius dispel-window timer | UNVERIFIED existence | Confirm if mechanic exists at all |

Spell ID 75763 appears twice in community sources (Karsh Cinderbreath and Obsidius shadow puddle) — at least one is mis-attributed. Resolve at DBC-check time.

---

## Notes on prompt deviations

- **Skipped spec selection** as instructed — no spec recommendation section.
- **Heroic delta is per-boss** rather than a separate top-level section (matches Throne of the Tides format).
- **"Tank-swap requirements"** is included per-boss even when the answer is "none," for consistency with the reference survey format. In a 5-man this is mostly a redundant field, but it's load-bearing for Obsidius.
- **CC pull-markers** were rolled into the Trash section rather than a separate section — the format is otherwise identical to Throne of the Tides.
- **Beauty is included as Boss 4** in the per-boss section (matching the prompt) even though she is optional — the bot must be able to fight her if instructed, so the fact that she's optional is captured in her per-boss "Beauty is skippable" notes plus the trash-section "Beauty's room is the main optional branch" call-out.
- **Many spell IDs are UNVERIFIED** — Blackrock Caverns is an old launch-tier dungeon and community guides often list spell IDs imprecisely. The **DBC verification** section is unusually long for a 5-man because of this. Recommend verifying at least items #1, #4, #5, #6, #7, #15, #17 before any bot encounter logic references these constants compile-time.
- **No instance-wide buff/debuff** unlike some Cata 5-mans (e.g., Halls of Origination's heat aura). Nothing instance-level to model.
- Wowhead Cata is the spell-ID source of record; where it disagreed with Icy Veins / Warcraft Tavern, Wowhead won. Where Wowhead was silent, the disagreement was logged.
