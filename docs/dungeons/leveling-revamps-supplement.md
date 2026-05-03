# Leveling Revamps Supplement — Vanilla / BC / WotLK Dungeons

**Bot-relevant mechanic flags** for the pre-Cata dungeons hit during 1-85 leveling on a 4.3.4 server. The Dungeon Finder pools these instances into the bracket queue with the 4.3.4-revamped versions of vanilla classics (Cata-Deadmines, Cata-SFK).

This doc is a **supplement** to `docs/dungeons/leveling.md`. The leveling doc covers cross-content era patterns (per-role progression, pull discipline, mana profiles); this doc goes dungeon-by-dungeon and flags **only the encounters that are not simple tank-and-spank for the bot**. Pure-tank-and-spank dungeons get a one-line entry.

Per the project's **doc-count strategy**, no dungeon below 80 gets a full ToT-style 6-file bundle. Those bundles are reserved for Cata 5-mans. This supplement is the entirety of the bot's pre-Cata dungeon coverage.

What gets flagged below:
- **CC**: pulls that demand crowd control (Polymorph, Sap, Hex, Banish, Freezing Trap, Hibernate)
- **Kick**: casters whose spells must be interrupted to avoid wipe pressure
- **Cone**: frontal-cone abilities requiring tank-away-from-group positioning
- **Move**: ground AoE / telegraphed damage requiring movement
- **Kite**: enemies that must be kited (slow-application, distance-maintenance)
- **Vehicle**: vehicle / mounted phases the bot module does NOT yet handle
- **Escort**: NPC-follow phases the bot module does NOT yet handle (`AltbotFollow.cpp` lacks `FollowNPCTarget`)
- **Dispel**: dispel-required mechanics (Magic / Curse / Disease / Poison)
- **MC/Fear**: Mind Control or Fear that breaks bot logic (lose control of bot)
- **Gauntlet**: timed event / wave defense
- **Boss-skip**: optional boss bot should default to skipping

What is **NOT** repeated below: era-level patterns (mana floors, threat tools online by level, drink-between-pulls discipline, AoE-pull thresholds). Those live in `leveling.md` per-role progression tables.

Reference for cross-era mechanic responses: `docs/dungeons/leveling.md` mechanic catalog.

---

## Vanilla (level 1–58 on 4.3.4)

Vanilla content was rebalanced in 4.0.3 (Cataclysm pre-patch). Mob HP and damage scale with the LFD bracket; instances are less punishing than at level 60 originally but can still wipe a careless group. **Cata-Deadmines and Cata-SFK** are revamped at 85 (heroic) and are NOT covered here — they live as level-85 heroic content in the regular Cata 5-man scope.

### Ragefire Chasm (Horde, level 8–13)

Ragefire is the smallest instance in the game. Four bosses, all tank-and-spank. **No mechanic flags.** Bot can spam baseline rotation.

### Wailing Caverns (level 17–24)

Linear instance with several optional bosses. **Move** flagged on Lord Cobrahn (poison cloud at low HP — move out). **Boss-skip** on Verdan, Lord Pythas, Lady Anacondra, Lord Serpentis if group is on the speed-clear quest path; bot defaults to clearing all unless leader instructs otherwise. Otherwise tank-and-spank.

### The Deadmines (Alliance, level 17-26 / **revamped 85 heroic** — see Cata bundle)

The level 17–26 leveling version is the **pre-revamp Deadmines** through 4.0.3. Linear. Tank-and-spank with one minor flag: **Cone** on the cannon-fire trash before Sneed (avoid the cannon arc). On the level 17–26 LFD pool the bot treats this as standard. The 85 heroic version is comprehensive bundle scope (separate doc).

### Shadowfang Keep (level 22-30 / **revamped 85 heroic**)

Pre-revamp version. **Kick** on the Voidwalker / Necrotic Aura casters; **Move** on Razorclaw / Wolf Master spinning AoE; otherwise tank-and-spank. The 85 heroic version is a Cata 5-man — separate doc.

### The Stockades (Alliance, level 23–30)

Small linear prison instance. Tank-and-spank with **Kick** on the various caster prisoners (Searing Bolt / Fireball). **No CC needed.**

### Blackfathom Deeps (level 24–32)

Underwater-themed instance with terrestrial walking paths. **Move** on Aku'mai's poison cloud at low HP. Trash includes **Kick** targets (Aku'mai Servants — Frost Bolt). Also features **Twilight Shore Strider** trash with a knockback — tanks position with back-to-wall on those pulls. Otherwise tank-and-spank.

### Gnomeregan (level 24–34)

Sprawling layout with several optional sub-areas. **Kick** on Mechanical Squirrel / Tinkerers; **Move** on Crowd Pummeler 9-60's Gnomeregan Pummeler ground slam; **Boss-skip** on Mekgineer Thermaplugg's lieutenants (optional rooms). The trash includes self-detonating Walking Bombs — bot ranged DPS must kite or kill before detonate (quick burst). **No dispels required.**

### Razorfen Kraul (level 24–32)

**CC** flagged: Razorfen Snufflenose Geomancers and Defenders cast spells that hit hard if not interrupted; Polymorph valid on humanoid quilboar. **Move** on Charlga Razorflank's Mortal Strike and Demoralizing Shout (party-wide debuff). Otherwise standard.

### Razorfen Downs (level 30–38)

**Fear** flagged on the Plaguemaw the Rotting and Ragglesnout encounters; bot Resto Shaman should drop Tremor Totem pre-pull on those pulls. **Kick** on Tuten'kash's Web Wrap. **Move** on the conveyor-style trash that leaves blue ground patches. Otherwise standard.

### Scarlet Monastery (level 27-42, four wings)

Four sub-instances; LFD treats as separate dungeons.

- **Graveyard** (27-32): Tank-and-spank. **Fear** on Bloodmage Thalnos (Bone Storm + summons skeletons; spread melee).
- **Library** (29-37): **CC** required on patrol caster mobs (Houndmaster, Doan's adds). **Kick** on Houndmaster Loksey (multiple Whirlwind cleaves) and Arcanist Doan's Polymorph (he sheeps a player). **Move** on Doan's Detonation channel — run out of his AoE field before it explodes. Bot must run-away on Detonation cast detection.
- **Armory** (32-38): **CC** on the Templar packs. **Kick** on Herod's Charge (he charges random target — heavy pull mechanic; melee don't run away or you get charged). **Move** on Herod's Whirlwind — kite-out for melee. After Herod dies, summons skeletons; AoE-burn.
- **Cathedral** (35-42): **CC** mandatory on patrols. **Kick** on Mograine and Whitemane casters. **Boss-skip** on Mograine's "fake death" — Whitemane's a-must-die-second mechanic: kill Mograine, then Whitemane resurrects him and the group must kill her before he comes back at full HP. **Bot must NOT prioritize Mograine until Whitemane is at low HP.** This is a target-priority swap mechanic that needs explicit encounter handling.

### Uldaman (level 32–42)

**CC** on Galgann Firehammer (Frost Nova breaks DPS rotation). **Move** on Archaedas pillar phase — adds spawn from pillars and bot must kill them off boss while staying clear of Archaedas's pulse damage. **Boss-skip** on Baelog's chest mob (loot side branch only). **Kite** on Ancient Stone Keeper on heroic-scaled levels. Otherwise standard.

### Maraudon (level 41–48)

Large instance with three sub-zones (Purple, Orange, Princess wing). **Kick** on Noxxion (Toxic Volley), Tinkerer Gizlock (Goblin Land Mine — drops on floor, **move out**). **Move** on Lord Vyletongue's poison cloud. **Boss-skip** on optional Veng (off-path bonus boss). The Princess Theradras encounter has **Move** (Repulse knockback) and **Adds** (summons elementals on ledges; AoE-burn).

### Sunken Temple / The Temple of Atal'Hakkar (level 50–60)

**Notable mechanic-rich instance.** Multi-tier layout. **CC** mandatory on the Atal'ai Defender pulls. **Move** on Hakkar's The Soulflayer (Drain Life + curse-style bleed). **Curse dispel** on Atal'ai Deathwalker's Spirit (curse of weakness, removes damage). **Boss-skip** on the optional dragonkin in the side rooms (only forced if group wants pet drop).

The big-scope mechanic is the **statue-activation event**: 6 statues around a circular chamber must be activated in a specific order to spawn the optional Avatar of Hakkar boss. Bots can't follow the activation puzzle natively — if the leader activates the statues, the bot will fight whatever spawns. Default behavior: skip statue puzzle unless leader walks the bot through it.

### Zul'Farrak (level 44–54)

**Gauntlet** mechanic: the pyramid stairs gauntlet — Hydromancer Velratha summons waves of trolls climbing the stairs while the group fights at the top. Bot AoE-burn the waves; healer pre-HoTs tank for sustained damage. **Move** on Antu'sul (Healing Ward + Lightning Storm). **CC** on the chained-prisoner trash (each pull is mixed humanoid + beast). **Boss-skip** on the optional Sandfury Executioner and Sergeant Bly mini-boss (off-path).

### Blackrock Depths (level 47–60)

**The longest leveling instance in the game.** ~30+ named bosses, sprawling layout, multiple optional wings. **CC** mandatory on most pulls (Anvilrage Officers / Ringleaders / Wardens, Doomforge mages). **Kick** required on Doomforge Mage Fireballs and Ribbly's Crew Magus casts.

Notable mechanics:
- **MC/Fear**: Phalanx encounter has fear (Death Sentence) — Tremor Totem on Resto Shaman. Lord Roccor's Earthquake — move out.
- **Gauntlet**: the **Ring of Law** is an arena event with random boss spawns from a pool of 6+ — bot can't predict who spawns; must react. Some require kiting (Ironhand Guardian), some require AoE (Ribbly Screwspigot adds). Treat as "boss appears, fight it" with no pre-positioning.
- **Boss-skip**: most named bosses are optional. The dungeon can be cleared three different ways (Emperor Dagran Thaurissan run, Princess Moira run, Bar event run). Bot defaults to **follow the leader's path**; do not initiate boss pulls.
- **Magic dispel** on Princess Moira's Mind Control on heroic-scaled groups (rare in LFD scaling).
- **Bar event**: Plugger Spazzring, the Phalanx, and adds spawn when the keg is bumped or Plugger is provoked. Bot AoE-burn the wave; healer regen between waves.

### Lower Blackrock Spire (level 55–62)

**Multi-pull discipline required.** The spire is dragonkin + orc-themed; pulls are large and aggressive.

- **CC** on most caster pulls (Spirestone Battle Mages — Polymorph; Smolderthorn Shadow Priests — Fear, Sap, Polymorph)
- **Kick** mandatory on Smolderthorn Shadow Priests (Mind Blast / Fear) and Battle Mages (Frostbolt / Fireball)
- **Fear**: Smolderthorn Mystics fear; counter with Tremor / Fear Ward
- **Cone**: Halycon's Charge + Cleave; Wyrmthalak's Cleave
- **Boss-skip**: Mother Smolderweb is optional; **Highlord Omokk** boss event requires triggering at his throne (avoidable but on the kill list); Halycon's puppy-trigger event (kill Halycon → Gizrul the Slavener spawns), bot continues if leader directs
- **Gauntlet**: at some scaling tiers, Burning Felguards spawn near the Beast event. Tank-and-spank but high cleave damage.

### Upper Blackrock Spire (level 55–62, **historically 60+ raid; treated as 5-man via LFD scaling**)

UBRS is technically a 10-man raid in vanilla, scaled to 5-man via the LFD. **In current 4.3.4 LFD it's NOT in the standard leveling pool** — included here for completeness only. Skip.

### Stratholme — Live Side (level 53–60)

**Hard.** Was the canonical pre-raiding 5-man.

- **45-minute Baron timer**: kill Baron Rivendare within 45 minutes from instance start to spawn the rare epic mount-drop. Bot doesn't track time pressure; ignored.
- **CC** mandatory on most pulls (Stratholme Citizens — Polymorph; Crimson Mages — Polymorph)
- **Kick** required on Crimson Mages, Crimson Defenders, Magistrate Barthilas, Cannon Master Willey
- **Magic dispel**: Postmaster Malown (rare elite) summons mailbox-style; **rarely seen in LFD**. Skip on default path.
- **Move**: Magistrate Barthilas flame strike; Cannon Master Willey grenades (drop ground patches)
- **Gauntlet**: the **Ziggurat phase** spawns waves of skeletons + scourge-themed mobs around the three ziggurats; bot AoE-burn each wave.
- **Boss-skip**: most are optional (Maleki, Magistrate, Cannon Master, Ramstein, Skull). Default path is Service Gate → Crimson Charge → Slaughterhouse → Baron. Bot follows leader.

### Stratholme — UD Side (level 53–60)

The Naxxramas-themed wing. **Boss-skip** Postmaster Malown. The Baron Rivendare event (Living vs Undead) is the same. UD-side is generally skipped in LFD on 4.3.4 because Live-side has the same Baron at the end.

- **Adds** waves throughout — **AoE-burn**.
- **Move** on Baron's Death Coil (random target, fear + heal himself if healing).
- **Magic dispel** on Baron's Mark of Death on tank (rarely seen in current scaling).

### Scholomance (level 53–60)

**Magic-school heavy.**
- **CC** mandatory on most casters (Plagued Hatchlings, Diseased Ghouls — Polymorph valid since they're humanoid-flavor)
- **Kick** required on every caster pack — Plagued Hatchlings, Necrology Schools, Plague Walkers
- **Disease dispel** flagged: many trash mobs apply diseases. **Sustained dispel pressure** for a Resto Shaman / Holy Pally / Resto Druid (Disease dispel: Cure Disease for Pally/Priest, Abolish Poison for Druid — note Druids do NOT cure disease in 4.3.4). **Bot healer dispel logic must filter by class capability.**
- **Magic dispel** on Lord Alexei Barov (curse) and Doctor Theolen Krastinov (curse).
- **Curse dispel** flagged: Decrepit Touch (Vectus, Theolen). Mage / Druid / Shaman curse dispel.
- **MC**: Jandice Barov has illusions + Mind Blast — bot ignore the illusions (untargettable).
- **Gauntlet**: the **Gandling teleport event** sends random group members to one of 6 sealed rooms with adds — bot fights the adds in their room, leader rejoins on door open. Bot doesn't navigate room re-entry; will fight wherever it ends up.
- **Boss-skip**: most named bosses are optional. Default LFD path is Rattlegore → Ras Frostwhisper → Krastinov → Vectus → Gandling. Bot follows leader.

### Dire Maul — East / North / West (level 56–60)

Three sub-instances; LFD treats as separate dungeons.

- **East** (Tribute run): **Move** on Pusillin's lure-the-imp event (he runs through trash; group follows). **Boss-skip** on the Pusillin chase since it's loot-only. Final boss Alzzin the Wildshaper: **Move** on Druidic Cyclone, **Adds** (small wisps); AoE-burn.
- **North** (Tribute / King run): **Tribute mechanic** — keep certain bosses alive in side rooms to receive the tribute chest from King Gordok. Bot doesn't track tribute logic; will fight whatever leader pulls. **CC** on the Gordok Mistress (Polymorph). King Gordok himself: **Move** on Mortal Strike + Berserker Charge.
- **West** (Prince Tortheldrin run): **Magic dispel** on the imp swarm (Detonate Magic). **Move** on Tendris Warpwood's roots. **CC** on Hydrospawn (humanoid? — Sap valid). Final boss Prince Tortheldrin: **Move** on Whirlwind, otherwise tank-and-spank.

### Vanilla bosses with Mind Control

- Scholomance: Jandice Barov (illusions, not true MC)
- BRD: Princess Moira (true MC at low HP — bot must NOT auto-attack feared/charmed party members)

**Bot logic note**: when a party member has the Mind Control buff, the bot's "is friendly" check should still treat them as friendly. **Do NOT attack a charmed party member.** Wait for charm to break. This is a generic rule, not Vanilla-specific.

---

## Burning Crusade (level 58–68 on 4.3.4)

**The hardest bracket of leveling.** Mana discipline is absolute; CC + kicks are mandatory on most pulls. Pull discipline is tighter than any other era.

### Hellfire Ramparts (level 58–62)

Linear short instance. **CC** mandatory on the Hellfire Watchers (Polymorph valid since they're humanoid). **Kick** on every caster pack — Bonechewer Striders, Hellfire Imps, Hellfire Felguards. **Move** on Vazruden the Herald's Fire Nova bombs. The final boss has a **vehicle phase** — Vazruden flies on a drake while Nazan the Drake is targetable from the ground. **Vehicle phase here is "stand on ground and shoot up"; not the OCULUS-style player-piloted vehicle.** Bot handles fine.

### The Blood Furnace (level 59–63)

**CC** on most pulls (Blood Furnace Cultists). **Kick** on Cultists and Magisters. **Magic dispel** on the Magisters (Frost Nova breaks DPS rotation).

- **Cone**: The Maker's Domination + frontal cleave
- **Move**: Broggok's Poison Cloud — bot must move to the cardinal-direction safe spot during channel
- **Gauntlet**: Broggok pre-fight is an **add wave gauntlet** — 4 waves of orc cultists from 4 cells; AoE-burn.
- **Adds**: Keli'dan the Breaker's pre-fight is also an add wave
- **Boss-skip**: optional sub-bosses if path skipped; default LFD clears all four

### Slave Pens (level 60–64)

**CC** flagged: most underbog mobs (Naga, Sporebats). **Kick** on Naga casters.

- **Cone**: Mennu the Betrayer's Earth Shock chain (bounces) — spread casters
- **Move**: Rokmar the Crackler's Water Spit + Grievous Wound (heavy DoT — pre-shield)
- **Magic dispel** on Quagmirran's Acid Geyser
- **Boss-skip**: Quagmirran is the final boss; no skipping

### The Underbog (level 61–65)

**CC** on Sporebats and Underbat Spore Striders. **Move** on most boss mechanics:
- Hungarfen's mushroom adds — **AoE-burn**
- Ghaz'an's Acid Breath — **frontal cone**, side-position melee
- Swamplord Musel'ek's Bow Shot phase — **kite** during phase change to give DPS room
- The Black Stalker's Static Charge — **spread** to avoid chain damage

### Mana-Tombs (level 62–65)

**CC** mandatory: ethereal mobs, can be Polymorphed (humanoid for sheep purposes). **Kick** on caster ethereals (Mind-Spell on healers).

- **Boss-skip**: Yor (avoidable side branch on heroic-scaled, sometimes appears in LFD)
- **MC**: Pandemonius's Dark Shell — **bot don't dispel/dispel-attempt** Dark Shell; it's a damage-reflect mechanic
- **Move**: Tavarok's Earthquake (telegraphed AoE)
- **Add**: Nexus-Prince Shaffar summons illusions; **AoE-burn**, ignore illusions for primary kill priority

### Auchenai Crypts (level 62–65)

**CC** mandatory on most pulls (Avatars of the Martyred — humanoid). **Kick** required.

- **Cone**: Shirrak the Dead Watcher channels casts at random players — interrupt-able? UNVERIFIED — bot kicks if cast detected
- **Adds**: Exarch Maladaar's Avatar of Earthen Ring (Cone, Sword, Stomp). Each Avatar has unique abilities; AoE-burn the smallest first
- **MC**: Maladaar can stun-MC a player; bot waits, doesn't auto-attack

### Sethekk Halls (level 62–66)

**CC** flagged on the avian humanoid trash (Polymorph — they ARE humanoid for sheep purposes since 4.0.3 patch).

- **Kick** on every caster pack
- **MC**: Anzu (optional summoned boss for druid epic flight quest) — **Bot-skip** unless leader summons
- **Cone**: Talon King Ikiss's Cone of Knowledge mind sear — silence, **move-out** — bot must run during channel
- **Move**: Darkweaver Syth's Shadowshift (he splits into 4 elemental copies); bot AoE-burn the elementals before re-engaging Syth

### Shadow Labyrinth (level 64–67)

**MC HEAVY.** This is the hard BC dungeon.

- **CC** mandatory on most pulls
- **Kick** required on every caster pack — Cabal Acolytes (Word of Shadow)
- **MC**: Mind-Controlling Cabal Spellbinders cast Mind Control on random players. **Bot must NOT attack a charmed teammate.** Wait for break or dispel. Resto Shaman Purge (Magic), Holy Priest Dispel Magic, Holy Pally Cleanse — all valid for breaking MC. **Bot healer dispel priority should be: emergency Mind Control immediately.**
- **Cone**: Murmur's Sonic Boom — **knockback + damage**, bot must run-in-then-out (channel cycle ~20s); **DPS players must be at max range or stack at the wall behind boss**
- **Move**: Murmur's Sonic Shock (shadow ground patches), Murmur's Resonance (dispellable curse — Druid Remove Curse, Mage Remove Curse, Shaman Cleanse Spirit)
- **Adds**: Hellmaw's Banish-able demons; some specs (Lock) can banish but bot doesn't use Banish coordination — let the demons be tanked instead
- **Boss-skip**: Hellmaw is mandatory; Blackheart the Inciter (MC boss) is also mandatory; Grandmaster Vorpil is the third (summon shadow vortex — **bot ranged players move OUT of Vorpil's pull-to-center channel**); Murmur is the final.

**Shadow Labs is the canonical BC mana-test for healers** — high CC dependency, multiple dispels, and steady damage on Murmur.

### The Steamvault (level 65–68)

**CC** on the Naga casters. **Kick** mandatory.

- **Cone**: Hydromancer Thespia's Lightning Cloud (avoid)
- **Move**: Mekgineer Steamrigger's mechanic adds (small repair-bots) — **AoE-burn before they reach Steamrigger**, otherwise they heal him
- **Boss-skip**: Warlord Kalithresh's pre-pull involves clearing 4 vials in cardinal directions — bot follow leader on vial smashing; main fight has **Move** (Spell Reflect — bot ranged reduce hard-cast frequency) and **Add** (channels with naga adds; AoE-burn)

### The Shattered Halls (level 67–70)

**Hardest BC dungeon.**

- **CC** mandatory on all caster pulls
- **Kick** mandatory on every caster — Shattered Hand Houndmasters, Shattered Hand Heathens
- **Cone**: Grand Warlock Nethekurse's Death Coil (single-target DoT — let him cast it on tank)
- **Gauntlet**: the **Executioner gauntlet** — 4 prisoners must be saved within a 3-minute timer; wave defense pulls (executioners + elite mobs); AoE-burn each wave. Bot doesn't track the timer; healer just sustains.
- **Adds**: Warbringer O'mrogg's Add waves; Warchief Kargath Bladefist's Phase 2 add waves; AoE-burn
- **Move**: Kargath's Blade Dance phase — **kite/spread**, he charges a random player and chains hits
- **Boss-skip**: none — all three are mandatory

### The Mechanar (level 67–70)

**CC** flagged on most caster pulls (Mechano-Lords; mostly humanoid).

- **Kick** required on every caster
- **Cone**: Gatewatcher Iron-Hand's Hammer Punch (frontal); Gatewatcher Gyro-Kill's Stream of Machine Fluid (cone)
- **Move**: Mechano-Lord Capacitus's Polarity Shift — players gain positive/negative polarity buff; **same-polarity stacks together, opposite-polarity stacks apart.** **Bot CANNOT handle polarity grouping natively** — flag for bot-implementation.
- **Adds**: Sepethrea's Inferno Elemental phase — **AoE-burn**, **kite** (the elemental hits hard)
- **Move**: Pathaleon's Cone of Frost; Mind Vision telegraph
- **Boss-skip**: Sepethrea is optional; LFD typically clears all

### Botanica (level 67–70)

**CC** on most pulls. **Kick** on caster mobs.

- **Cone**: Commander Sarannis's Cleave; **Move** on her Bloodletting (random-target chain damage)
- **Move**: Warp Splinter's Adds (AoE-burn the saplings before they reach the boss); High Botanist Freywinn's Tranquility (channeled heal — **MUST INTERRUPT**); Thorngrin the Tender's Hellfire (AoE damage cycle — group spread)
- **Cone**: Laj's Summon Lasher (small sapling adds; AoE-burn)
- **Boss-skip**: most named mid-bosses are mandatory

### Arcatraz (level 68–70)

**The hardest BC instance after Shadow Labs.**

- **CC** mandatory
- **Kick** mandatory on every caster
- **MC**: Skyriss's Mind Control + Domination (during second phase; he splits into clones at 50% HP); **bot must NOT attack charmed teammates**; bot resto-shaman / priest / pally CLEANSES MC immediately on detection
- **Cone**: Wrath-Scryer Soccothrates (Felfire Line — frontal AoE)
- **Move**: Dalliah the Doomsayer's Whirlwind phase (kite); Soccothrates Felfire Shock (random ground patches)
- **Adds**: Harbinger Skyriss's split into 3 clones at 50% — **EACH clone has full HP**; bot must AoE-burn the clones one at a time, or single-target one and ignore the rest (clones de-spawn when boss dies)
- **Mellichar event**: pre-boss event with 5 cells; each cell pops a different boss-flavor add. **Gauntlet**, AoE-burn waves. Bot follow leader on cell-trigger order.

### Old Hillsbrad Foothills (level 64–68 — Caverns of Time)

**Vehicle / NPC-Escort mechanics — bot module gap.**

- **Escort**: the entire instance is escort-flavored. Erozion's intro → **Thrall NPC follows the group through 3 boss encounters**; if Thrall dies the instance fails.
- **Bot follows tank as usual; works imperfectly when the bot must protect Thrall instead of tank.** Flagged in `docs/dungeons/leveling.md` Open Issues.
- **Adds** waves throughout — guard-style **AoE-burn**.
- **Move** on each boss (Captain Skarloc's Whirlwind; Drake Killcrazed's Acid Pool; Epoch Hunter's Wing Buffet)
- **Boss-skip**: none mandatory; the instance is linear

### The Black Morass (level 64–68 — Caverns of Time)

**Wave-defense + NPC-Escort — bot module gap.**

- **Escort**: Medivh NPC channels at center; the group defends him while waves of demons spawn from 18 portals. **If Medivh dies, instance fails.**
- **Adds**: 18 portal-waves of mixed demons (imps, voidwalkers, ele felhunters, large demonkin)
- **Bot vehicle/escort follow logic missing**; bot fights the demons but doesn't pick the right priority when Medivh's health drops. Tank may not pick up adds in time. **Flagged for bot-implementation.**
- **Boss**: Aeonus (final dragonkin boss) has **Move** (Sand Breath cone), **Adds** (mini-drakes), and is just a long fight
- **Magic dispel** flagged on demon casts — Holy Pally / Resto Shaman / Disc Priest

### BC bosses with non-standard MC

- Shadow Labs Cabal Spellbinders — Mind Control
- Arcatraz Skyriss — Domination + clones
- Bot logic note: **dispel MC immediately**; **never attack a charmed party member**.

---

## Wrath of the Lich King (level 68–80 on 4.3.4)

WotLK is the **easy bracket**. Linear layouts, abundant AoE threat tools (Thunder Clap, Swipe, DnD), Replenishment baseline mana regen, mp5 mature. Pull discipline relaxes; CC is mostly optional.

### Utgarde Keep (level 68–72)

Linear. **Kick** on Skarvald the Constructor's Charge; **Move** on Ingvar the Plunderer's Shrieking Roar (4-second fear; pop fear-counter); Ingvar phase 2 (he reanimates as ghost) — same fight with shadow flavor. **Cone**: Ingvar Smash. **No CC required.** Tank-and-spank with mechanics.

### The Nexus (level 68–72)

**CC** optional on most pulls. **Kick** on Spell-flinger Aldris (mage trash).

- **Move**: Grand Magus Telestra's Frost Nova — bot ranged dispels-to-self via PvP trinket / Iceblock if available, else stand still; her **clone phase** at 50% HP (3 elemental clones spawn) — **kill in priority order: Frost first, Fire second, Arcane last** because Frost roots melee
- **Adds**: Anomalus's Crazed Mana Wraiths — **AoE-burn**
- **Move**: Ormorok the Tree-Shaper's Crystal Spikes (ground patches); Trample (charge)
- **Adds**: Keristrasza's Crystallize (root + AoE damage) — break with damage; **Move** out of Crystal patch
- **Boss-skip**: none mandatory

### Azjol-Nerub (level 70–73)

Three-phase descending instance. **Move** on Krik'thir's Mind Flay channel and add-spawn waves.

- **Adds**: Krik'thir the Gatewatcher's Watchers + Skittering Swarmers — **AoE-burn**
- **Vehicle/Phase**: Hadronox is fought during **descent into the lower chamber** — multiple add waves, **kite** the boss; bot AoE-burn waves while keeping Hadronox alive (kill triggers boss progress). **Phase change**: when Hadronox is dragged to the top platform, she's killable.
- **Move**: Anub'arak's Pound (ground spike); Submerge phase (he burrows, summons skeletal adds — AoE-burn while waiting for resurface)
- **Boss-skip**: none mandatory

### Ahn'kahet: The Old Kingdom (level 71–74)

**CC** optional except on faceless trash (Insanity Sphere — debuff). **Kick** on caster Faceless Watchers.

- **Cone**: Elder Nadox's Plague (frontal cone + DoT)
- **Adds**: Prince Taldaram's Vampiric Bites (he becomes invuln on a target until killed via spheres) — **bot follows the spheres through the boss path**; **bot CANNOT handle the spheres mechanic natively** (each player picks up a sphere and uses it on boss to break invuln). Flag for bot-implementation.
- **Move**: Jedoga Shadowseeker's Sacrifice phase — **bot players don't volunteer for sacrifice**; tank pulls boss to center; AoE-burn adds
- **Adds**: Herald Volazj's Insanity phase — **bot players become hostile** to each other temporarily; **bot must NOT attack charmed teammates**; this is a 30-second illusion phase. Wait for end. **MAJOR BOT-IMPL FLAG.**

### Drak'Tharon Keep (level 72–75)

**CC** flagged on caster trash (Drakkari Trolls — humanoid for Polymorph). **Kick** required.

- **Cone**: Trollgore's Frenzy + Cleave; **Move** on his Corpse Explode (corpses on the floor explode; pre-clear corpses)
- **Adds**: Novos the Summoner's Add waves + Crystal Channeler clones — **AoE-burn**, dispel his Frostbolt Volley with **Magic dispel** if available
- **Move**: King Dred's Mangling Slash (frontal cleave) and Raptor charges; **Cone** on the small-raptor adds
- **Adds**: The Prophet Tharon'ja Phase 2 — **bot players become a SKELETON** (you control a re-themed character); **bot CANNOT handle the form-change phase**. Wait for phase end. **MAJOR BOT-IMPL FLAG.**
- **Boss-skip**: none

### Violet Hold (level 73–76)

**Wave-defense instance.** Like Black Morass.

- **Adds**: 18 portal-waves of mixed mobs from a pool. **AoE-burn each wave.**
- **Move** on each portal mob's specific mechanic (each is a mini-boss with one or two abilities)
- **Bosses**: 2 random main bosses from a pool of 6 (Erekem, Moragg, Ichoron, Xevozz, Zuramat, Lavanthor); **bot can't predict who spawns**. Bot follows leader on prioritization.
- **Magic dispel** flagged on Xevozz's Arcane Bolt (Magic) and Zuramat's Shroud of Darkness (Magic on tank)
- **Final boss**: Cyanigosa (dragonkin) — **Move** on Tail Sweep, **Cone** on Frost Breath. Tank-and-spank with mechanics.

### Gundrak (level 73–76)

**Drakkari troll-themed.** Tank-and-spank on most.

- **Move**: Slad'ran's Snake Wrap (random-target paralysis); **Adds** (snake adds — AoE-burn)
- **Move**: Drakkari Colossus's Phase 2 (Mojo elemental pops out at 50% HP — kill the elemental, the colossus reanimates; kill the elemental again, the colossus dies) — **bot priority swap on phase trigger**
- **Move**: Moorabi's Transformation (he transforms players into mammoths — **MUST INTERRUPT** every cast); also **Earthquake** (move out)
- **Adds**: Eck the Ferocious (rare optional boss; **bot-skip** unless leader pulls); **Boss-skip**: Eck is the only optional encounter
- **Final boss**: Gal'darah — **Cone** (Impale; he charges a random player), **Move** out of his Stampede phase

### Halls of Stone (level 75–78)

**Escort-flavored — bot module gap.**

- **Escort**: **Brann Bronzebeard NPC follows the group**; if Brann dies, the **Tribunal of Ages event** fails. **Bot follow-Brann logic missing.**
- **Boss 1 — Maiden of Grief**: **Move** on Storm of Grief (ground patches)
- **Boss 2 — Krystallus**: **Move** on Boulder Toss; **Shatter** mechanic — when frozen, take 2x damage; bot move-out of his Boulder ground patches
- **Tribunal of Ages event**: Brann channels at a console; 4 add waves spawn from cardinal directions; **AoE-burn** each wave. **Adds become tougher each wave.** **Brann finishes channel → next boss.**
- **Boss 3 — Sjonnir the Ironshaper**: **Move** on Lightning Ring (kite), **Adds** (Iron Sludge, Frost / Acid pool patches)

### Halls of Lightning (level 75–78)

**Loken is the canonical WotLK leveling difficulty spike.**

- **CC** optional except on Storm Magus trash (humanoid; Polymorph valid). **Kick** required on Storm Magus and Iron Maidens.
- **Boss 1 — General Bjarngrim**: **Stance phases** — Defensive (high damage to him, high mitigation), Battle (cleave + slam + reflect), Berserker (Whirlwind + heavy damage). **Bot must NOT attack during Defensive Reflect window** (raid-style swap mechanic — but in 5-man, just take the damage; bot can keep DPS).
- **Boss 2 — Volkhan**: **Adds** (Brittle Iron Constructs spawn → he Shatters them with Stomp → fragments do AoE damage). **Bot must kill the constructs before Stomp** (telegraphed cast). Pre-Cata-style "kill the right targets in time" mechanic.
- **Boss 3 — Ionar**: **Move** on Disperse phase (he splits into 8 lightning bolts that home on random players — kite/run away, do NOT stop). Reform after 10s — re-engage. **MOVEMENT-INTENSIVE** — bot cast-while-moving classes excel here.
- **Boss 4 — Loken**: **Lightning Nova** every ~10 seconds — bot **runs OUT to max range** → after Nova, runs back IN (~3 seconds to safe). **Pulsing AoE that scales with distance.** **THIS IS THE CANONICAL HEALER MANA CHECK** of WotLK leveling. Bot healer pre-shields tank; ranged DPS plays max-range; melee plays max-melee-range out + back. **Critical sequencing: attack-during-Nova kills the bot.**

### The Oculus (level 76–79)

**Vehicle phase — bot module gap.**

- **Pre-vehicle**: 3 boss platforms with regular non-vehicle fights (Drakos, Varos, Mage-Lord Urom)
  - **Drakos the Interrogator**: **Move** on Magic Pulse + Knockback. Tank-and-spank otherwise.
  - **Varos Cloudstrider**: **Move** on Amplify Magic; **Adds** (centrifuge constructs)
  - **Mage-Lord Urom**: **Adds** (clones) phase + Time Bomb mechanic — **bot move out** of Time Bomb radius
- **Eregos final boss**: **MANDATORY VEHICLE PHASE.** All 5 players mount drakes (3 colors with different roles: Bronze = main DPS, Ruby = AoE healer, Emerald = tank/buff). **Bot CANNOT handle vehicle phase.** **Bot blacklists Oculus from LFD queue** (per leveling.md Open Issues).

### Utgarde Pinnacle (level 77–80)

**Skadi gauntlet is the difficulty spike.**

- **CC** optional. **Kick** on caster trash.
- **Boss 1 — Svala Sorrowgrave**: **Adds** (the ritual sacrifice — she lifts random players in air; bot freed by killing the channeler add); **Move** out of her Flame Strike
- **Boss 2 — Gortok Palehoof**: **Adds rotation** — 4 mini-bosses spawn one at a time; **kill in order spawned**; then Gortok activates. Each mini-boss has its own mechanic (Wolf — fear, Worg — Frenzy, Yeti — knockback, Rhino — charge). **Bot priority swap each mini-boss.**
- **Boss 3 — Skadi the Ruthless**: **Gauntlet on harpoons.** Skadi flies around the chamber on his Drake; players collect Drake Harpoons from a pile and shoot Skadi to ground him. **Bot CANNOT handle harpoon-pickup mechanic natively** — defaults to fighting whatever's targetable; will fail the gauntlet. **MAJOR BOT-IMPL FLAG.** Once grounded, Skadi is a tank-and-spank with **Whirlwind**, **Cone** (Crush), and **Move** (Poisoned Spear).
- **Boss 4 — King Ymiron**: **Tank-swap** flagged at high stack of Bane (debuff on tank). Phase changes via the four Vrykul Ancestor totems — each grants different abilities (4 phases). Bot follows leader on phase progression.

### The Culling of Stratholme (level 76–80 — Caverns of Time)

**Escort + chase + zone-progression — bot module gap.**

- **Phase 1 (intro)**: Group escorts Arthas through the city; multiple add waves; the **plague-bearing crates** on streets must be cleared. Bot AoE-burns waves.
- **Phase 2 (Meathook)**: Boss in a market square; **Move** on Constricting Chains
- **Phase 3 (Salramm the Fleshcrafter)**: **Magic dispel** on Curse of Twisted Flesh (Curse). Spawns Ghouls — **AoE-burn**
- **Phase 4 (timed Chrono-Lord run)**: 5-minute timer for optional boss spawns. Bot doesn't track time pressure; ignored.
- **Phase 5 (Chrono-Lord Epoch)**: **Move** on Wounding Strike; tank-and-spank
- **Phase 6 (Mal'Ganis)**: Final dreadlord. **Move** on Carrion Swarm (cone); **Sleep** (he sleeps the tank periodically — bot melee continues; bot ranged kicks); **Vampiric Touch** Magic dispel.
- **Bot follow-Arthas logic missing** — he leads the group between phases; bot follows tank by default, which is approximately right since the tank follows Arthas.

### WotLK bosses with charm / form-change

- Ahn'kahet Herald Volazj — Insanity (party becomes hostile illusions)
- Drak'Tharon Tharon'ja — players forced to become skeleton (form change)
- Bot logic note: **wait for end of phase**; do NOT attack ex-teammates during these effects.

---

## Bot-implementation gaps (consolidated from above)

This list is the union of every "bot-impl flag" raised above. Repeats `docs/dungeons/leveling.md` Open Issues table where overlap exists.

| Mechanic | Dungeon(s) | Module work needed |
|---|---|---|
| Vehicle phase | Oculus (final boss) | Detect vehicle entry, pause AltbotAI until exit |
| Vehicle phase (optional) | Halls of Lightning Ionar (Disperse), Utgarde Pinnacle Skadi (drake harpoons) | Same — vehicle / mounted handling |
| Escort NPC follow | Halls of Stone (Brann), Old Hillsbrad (Thrall), Black Morass (Medivh), Culling of Stratholme (Arthas) | Extend `AltbotFollow.cpp` with `FollowNPCTarget(guid)` mode |
| Polarity / position-by-buff | Mechanar Capacitus (Polarity Shift) | Bot needs to read polarity buff and re-position relative to other players' polarity |
| Player form-change phase | Drak'Tharon Tharon'ja (skeleton), Ahn'kahet Volazj (illusions) | Bot must detect form-change buff and pause aggressive logic until form ends |
| Sphere pickup mechanic | Ahn'kahet Prince Taldaram (Vampiric Bite spheres) | Item-pickup-and-use during boss fight |
| Harpoon mechanic | Utgarde Pinnacle Skadi | Same — item-pickup-and-use during boss fight |
| Statue activation puzzle | Sunken Temple (Atal'ai statues), BC Mechanar (cardinal vials) | Sequence-detection + player-positioning; default behavior is "follow leader" |
| Mind Control on party | Shadow Labs Cabal Spellbinders, Arcatraz Skyriss, Scholo Jandice, BRD Princess Moira | "Do not attack charmed teammate" rule + healer prioritize-MC-dispel |
| Wave-defense NPC-protection | Black Morass, Violet Hold, Halls of Stone Tribunal | Tank picks up adds; healer prioritize the channeling NPC's HP |
| Boss random from pool | Violet Hold, BRD Ring of Law | Bot can't pre-position; reactive only |
| Stance/phase-by-stance | Halls of Lightning Bjarngrim | Detect boss buff (Defensive / Battle / Berserker stance) and adjust DPS rotation (don't reflect-damage during Defensive) |
| Baron 45-min timer | Stratholme Live | Not bot-handled; ignored |
| Tribute keep-alive | Dire Maul North | Not bot-handled; ignored |

---

## Cross-era mechanic resolution by class

The healer.md / tank.md / ranged-dps.md / melee-dps.md role docs already cover the cross-class generalizations. This is a quick lookup table for the **specific dispel / interrupt / counter abilities** referenced in dungeon-specific anti-pattern flags above.

| Mechanic | Class counter |
|---|---|
| Mind Control on party | Holy Priest Dispel Magic (4987) → Mass Dispel (32375), Holy Pally Cleanse (4987), Resto Shaman Purge (370) [Magic enemy debuff], Disc Priest Dispel Magic (528) |
| Fear (group AoE) | Resto Shaman Tremor Totem (8143), Holy Priest Fear Ward (6346), Disc Priest Fear Ward (6346), Warrior Berserker Rage (18499), Undead racial (Will of the Forsaken) |
| Magic debuff dispel | Holy Pally / Disc Priest / Resto Shaman / Holy Priest standard Cleanse / Dispel Magic |
| Curse dispel | Resto Druid Remove Curse (2782), Mage Remove Curse (475), Resto Shaman Cleanse Spirit (51886) |
| Disease dispel | Holy Priest / Disc Priest Cure Disease (528 → 528), Holy Pally Cleanse (4987) — note: Cleanse covers Magic + Disease + Poison since 4.0.1; **Resto Druid does NOT cure disease in 4.3.4** |
| Poison dispel | Holy Pally Cleanse, Resto Shaman Cleanse Spirit (51886) — **Druid's Abolish Poison was removed in 4.0.1; Druid does NOT cure poison in 4.3.4** |
| Interrupt (Kick) | Rogue Kick (1766), Warrior Pummel (6552), DK Mind Freeze (47528), Pally Rebuke (96231), Shaman Wind Shear (57994), Mage Counterspell (2139), Druid Skull Bash (78675), Hunter (Counter Shot — pet ability), Lock pet (Spell Lock) |

---

## What's NOT in this supplement

- **Heroic-difficulty deltas** — vanilla / BC / WotLK heroic versions exist for some dungeons (BC heroics, WotLK heroics) but are NOT in the LFD leveling pool on 4.3.4. Players queue heroic separately, and at 80+ those queues prefer Cata heroic 5-mans. Out of scope here.
- **Per-boss spell IDs** — vanilla / BC / WotLK content doesn't get full DBC verification. The bot module's leveling-bracket logic doesn't reference these constants compile-time; mechanics are detected by spell-cast events generically (spellcast-with-cast-bar = potential interrupt target).
- **Trash mob enumeration** — only mechanic-flagged trash mentioned. Standard tank-and-spank trash is omitted.
- **Cata 5-mans** — covered in their own bundles under `docs/dungeons/{slug}/`.
- **Cata raid content** — out of scope here; lives at `docs/raids/` (when written).

For cross-class era-level patterns (mana floors per bracket, threat tools online, per-role progression by level), see `docs/dungeons/leveling.md`.
