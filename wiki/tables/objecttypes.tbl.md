> **Revision information**
> OFP Revision: [#133](https://github.com/ofp-fs2open/fs2open.github.com/pull/133)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **objecttypes.tbl** sets the attributes of ship types and defines the target priority groups turrets use. A ship 'type' is shared by many ship classes defined in [ships.tbl](ships.tbl). For example, the GTF Perseus and GTF Ulysses belong to the 'fighter' ship type, while the GTD Orion belongs to the 'capital' ship type.

The engine has a built-in version of this table (shown in the example below).

This table is one of the Modular Tables and can be extended with **xxx-obt.tbm**.

Code: [/ship/ship.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/ship/ship.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in. Fields without a badge are from the original game.

## Contents

1. [General Format](#general-format)
2. [#Target Priorities](#target-priorities)
3. [#Weapon Targeting Priorities](#weapon-targeting-priorities)
4. [#Ship Types](#ship-types)
5. [Examples](#examples)

---

## General Format

- The engine has a built-in objecttypes.tbl with the retail ship types (shown in the [example](#default-freespace-2-table) below). A mod's objecttypes.tbl *replaces* it entirely, so to add to or change the retail types, use a modular table (xxx-obt.tbm) instead.
- The table has three optional sections, in this order: `#Target Priorities`, `#Weapon Targeting Priorities` and `#Ship Types`. Each ends with `#End`.
- The fields of each entry must be in the order listed here. Every field is optional unless noted.
- Note that a number of aspects of the retail types are still hardcoded, although they may be moved to the objecttypes.tbl if a mod requests it.

## #Target Priorities

All set target priorities function on additive basis. For example setting 'object type' "ship" already includes all the ships in the game.

Turrets use these groups, through `$Target Priority:` on ship subsystems in [ships.tbl](ships.tbl) and through [#Weapon Targeting Priorities](#weapon-targeting-priorities).

### $Name:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Name of target priority group.
- A name that already exists (in this or an earlier table) **replaces** that group completely: every field starts empty again, including the ship types that `$Target Priority Groups:` added to it.
- Syntax: **String**

### +Object Type:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- The object types which belong to this target priority group.
- Syntax: **String**
  - Example: *ship*
  - Options: *"ship"*, *"asteroid"*, *"weapon"*, ![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) *"debris"*
- An unknown type is ignored without a warning.

### +Weapon Class:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Name of weapon classes which belong to this target priority group.
- An unknown weapon logs a warning.
- Syntax: **String list**, names of the weapons

  - Example: ( "*Cyclops*" "*Tsunami*")

### +Object Flags:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- The object flags which make their owners belong to this target priority group.
- Syntax: **String list**
- An unknown flag logs a warning.
- Options:
  - *"no shields"*
  - *"targetable as bomb"*
  - ![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) *"renders"*
  - ![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) *"collides"*
  - ![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) *"physics"*
  - ![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) *"invulnerable"*
  - ![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) *"player ship"*
  - ![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) *"special warpin"*
  - ![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) *"immobile"*
  - ![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg) *"don't-change-position"*
  - ![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg) *"don't-change-orientation"*

### +Ship Class Flags:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- The ship class flags which make their owners belong to this target priority group.
- Syntax: **String list**
- An unknown flag logs a warning.
- Valid Options (![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) for every option after *"has awacs"*):
  - *"afterburners"*
  - *"big damage"*
  - *"has awacs"*
  - *"no collide"*
  - *"player ship"*
  - *"default player ship"*
  - *"path fixup"*
  - *"support"*
  - *"cargo"*
  - *"fighter"*
  - *"bomber"*
  - *"cruiser"*
  - *"freighter"*
  - *"capital"*
  - *"transport"*
  - *"navbuoy"*
  - *"sentrygun"*
  - *"escapepod"*
  - *"no type"*
  - *"ship copy"*
  - *"in tech database"*
  - *"in tech database multi"*
  - *"stealth"*
  - *"supercap"*
  - *"drydock"*
  - *"dont collide invisible"*
  - *"corvette"*
  - *"gas miner"*
  - *"awacs"*
  - *"knossos"*
  - *"no fred"*
  - *"default in tech database"*
  - *"default in tech database multi"*
  - *"flash"*
  - *"show ship"*
  - *"surface shields"*
  - *"generate icon"*
  - *"no weapon damage scaling"*
  - *"gun convergence"*
  - *"no thruster geometry noise"*
  - *"intrinsic no shields"*
  - *"no primary linking"*
  - *"no pain flash"*
  - *"allow landings"*
  - *"no ets"*
  - *"no lighting"*
  - *"dyn primary linking"*
  - *"auto spread shields"*
  - *"draw weapon models"*
  - *"model point shields"*
  - *"repair disabled subsystems"*

### +Weapon Class Flags:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- The weapon class flags which make their owners belong to this target priority group.
- Syntax: **String list**
- An unknown flag logs a warning. *"cycle"* is no longer accepted (it logs a note and is ignored).
- Valid Options (![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) for every option after *"capital+"* unless marked):
  - *"bomb"*
  - *"huge damage"*
  - *"supercap damage"*
  - *"bomber+"*
  - *"electronics"*
  - *"puncture"*
  - *"emp"*
  - *"heat seeking"*
  - *"aspect seeking"*
  - *"engine seeking"*
  - *"pierce shields"*
  - *"local ssm"*
  - ![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg) *"capital+"*
  - *"heat-seeking"*
  - *"aspect-seeking"*
  - *"javelin"*
  - *"spawn"*
  - *"remote detonate"*
  - *"countermeasure"*
  - *"turns"*
  - *"swarm"*
  - *"trail"*
  - *"big ship"*
  - *"child"*
  - *"no dumbfire"*
  - ![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) *"no doublefire"*
  - *"thruster"*
  - *"in tech database"*
  - *"player allowed"*
  - ![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) *"default player weapon"*
  - *"corkscrew"*
  - *"particle spew"*
  - *"esuck"*
  - *"flak"*
  - *"tag"*
  - *"shudder"*
  - *"lockarm"*
  - *"ballistic"*
  - *"default in tech database"*
  - *"tagged only"*
  - *"small only"*
  - *"same turret cooldown"*
  - *"apply no light"*
  - *"transparent"*
  - *"training"*
  - *"smart spawn"*
  - *"inherit parent target"*
  - *"no emp kill"*
  - *"variable lead homing"*
  - *"untargeted heat seeker"*
  - *"no radius doubling"*
  - *"no subsystem homing"*
  - *"no lifeleft penalty"*
  - *"custom seeker str"*
  - *"can be targeted"*
  - *"show on radar"*
  - *"show friendly on radar"*
  - *"chain external model fps"*
  - *"external model launcher"*
  - *"takes blast damage"*
  - *"takes shockwave damage"*
  - *"hide from radar"*
  - *"render flak"*
  - *"ciws"*
  - *"anti-subsystem beam"*
  - *"no primary linking"*
  - *"same emp time for capships"*
  - *"no primary linked penalty"*
  - *"no homing speed ramp"*
  - *"pulls aspect seekers"*
  - *"turret interceptable"*
  - *"fighter interceptable"*
  - *"aoe electronics"*
  - *"apply recoil"*
  - *"don't spawn if shot"*
  - *"die on lost lock"*
  - ![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) *"no impact spew"*
  - ![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) *"require exact los"*
  - ![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg) *"multilock target dead subsys"*
  - ![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) *"ignores countermeasures"*
  - ![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) *"freespace 1 missile behavior"*

## #End

## #Weapon Targeting Priorities

### $Name:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Name of the weapon for which the target priorities are set
- An unknown weapon logs a warning and its entry is skipped.
- Syntax: ***String***, name of the weapon

### +Target Priority:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Names (in order) of the target priority groups the weapon prefers, when fired from a turret. This replaces the weapon's existing list.
- At most 32; extra names are dropped.
- **Known issue:** an unknown group logs a warning, but the groups after it in the list then land in the wrong slots, so the weapon's priorities come out wrong. Make sure every name exists.
- Syntax: ***String list***

## #End

## #Ship Types

### $Name:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Name of ship type. To assign a ship class to a given type, simply place the name entered here in the ship class's "$Flags:" field.
- A name used twice in objecttypes.tbl logs a warning and the second entry is skipped. In a modular table, a name that already exists edits that type.
- The old names sentrygun, escapepod, repair_rearm, supercap and knossos log a warning naming the current one (sentry gun, escape pod, support, super cap, knossos device).
- A type named *stealth* logs a warning and is removed after all tables are read, since it conflicts with the stealth ship class flag.
- If no type named *fighter/bomber* exists, that name means "fighter or bomber" wherever a ship type is asked for.
- Syntax: **String**

### +nocreate

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Optional. Goes directly after `$Name:`.
- In a modular table, edits the type only if it already exists, instead of creating a new one. Using it in objecttypes.tbl itself logs a warning.

### $Target Priority Groups:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the target priority groups ships of this objecttype belong to
- An unknown group logs a warning.
- Syntax: ***String list***
- **+Override**
  - Optional, after the list. Empties the *first* listed group of every ship type it already had before adding this one. It does not remove this type from groups that aren't listed.

### $Counts for Alone:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Whether ship counts for determining whether or not Freespace 2 will play the "No reinforcements are available" message.
- Syntax: **Boolean**, Yes or No

### $Praise Destruction:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Whether wingmates will praise destruction of the ship.
- Syntax: **Boolean**, Yes or No

### $On Hotkey List:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Whether ship will show up in hotkey screen.
- Syntax: **Boolean**, Yes or No

### $Target as Threat:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Whether ship will be targeted with the 'H' key.
- Syntax: **Boolean**, Yes or No

### $Show Attack Direction:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Whether ships of this type attacking the player are shown by the HUD's hostile direction indicator (the triangle around the reticle pointing at the closest attacker).
- Syntax: **Boolean**, Yes or No

### $Scannable:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Indicates whether the ship type can be targeted by the "target unscanned object" function.  In retail, these are the Cargo and Freighter types.  This is independent of whether the ship type can actually be scanned, which by default are the Cargo and *Transport* types.
- Syntax: **Boolean**, Yes or No

![FSO 26.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.0.svg) **$Targetable as unscanned:** can be used as a synonym

### $Scannable by default:

![FSO 26.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.0.svg)

- Indicates whether the ship type can be scanned if its cargo is unknown and it has cargo other than Nothing.  In retail, these are the Cargo and Transport types.  This not applicable if the **[$Unify scanning behavior:](https://wiki.hard-light.net/index.php/Game_settings.tbl#.24Unify_scanning_behavior:)** flag is set.
- Syntax: **Boolean**, Yes or No

### $Warp Pushes:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Whether ship will push other (pushable) ships out of the way while warping.
- Syntax: **Boolean**, Yes or No

### $Warp Pushable:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Whether ship can be pushed out of the way by ships with Warp Pushes enabled
- Syntax: **Boolean**, Yes or No

### $Turrets prioritize ship target:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines that ships turrets prioritize ships current target above normal turret targeting priorities
- Syntax: **Boolean**, Yes or No

### $Max Debris Speed:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the maximum speed for debris from this ship type
- Syntax: **float**

### $FF Multiplier:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Friendly-fire damage to ship will be multiplied by this amount, but only for purposes of scoring and traitor check, not actually damaging ships in-mission
- Syntax: **Float**

### $EMP Multiplier:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Not currently implemented. Originally used for "electronics" weapons. Deactivated in commit 941faf9ab9ed05a1b0efebf7de32c6e3c724baf7 as it broke retail compatibility.
- Syntax: **Float**

### $Warp Sound Range Multiplier:

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- Modifies the audible range used for warp fireball 3d sound effects.  In retail, "big ships" have a multiplier of 6.0 while all other types (including larger types) have 1.0.
- Syntax: **Float**

### $Beams Easily Hit:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Whether a type 4 anti-fighter beam should be converted to a type 0 direct-fire beam when firing at this type of target.
- Syntax: **Boolean**, Yes or No

### $Protected on cripple:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- If ship should be protected when its subsystems are being targeted
- Syntax: **Boolean**, Yes or No

### $No Huge Beam Impact Effects:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines if the huge beams will generate impact effects on ships of this type
- Syntax: **Boolean**

### $Don't display class in briefing:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Option to prevent ship class data from being shown during the briefings
- Syntax: **Boolean**

### $Fog:

This likely didn't do anything since FSO 20.0, roughly

![FSO 26.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.0.svg)

- These options are deprecated and should not be used anymore, they likely never did anything anyway, and certainly don't have an effect going forward

#### +Start Dist:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Distance from camera that fogging effect will start
- Syntax: **Float**

#### +Compl Dist:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Distance from camera that this object will be completely obscured by the fogging effect
- Syntax: **Float**

### $AI:

#### +Valid Goals:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Valid goals that can be given to this object.
- Syntax: **( "String" "String" )**
- **Goals:**
  - attack ship
  - dock
  - waypoints
  - waypoints once
  - depart
  - attack subsys
  - form on wing
  - undock
  - attack wing
  - guard ship
  - disable ship
  - disarm ship
  - attack any
  - ignore ship
  - ignore ship (new)
  - guard wing
  - evade ship
  - stay near ship
  - keep safe dist
  - rearm ship
  - stay still
  - play dead
  - attack weapon
  - fly to ship
  - ![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) attack ship class
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) play dead (persistent)
  - ![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) disable ship (tactical)
  - ![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) disarm ship (tactical)
  - ![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) attack ship type
- Not case sensitive. An unknown goal is ignored without a warning.
- A modular table that edits the type adds these goals to the ones it already has. The same goes for +Player Orders:, +Active Docks: and +Passive Docks:.

#### +Accept Player Orders:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Whether or not ship type will accept orders from player
- Syntax: *Yes/No*

#### +Player Orders:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Orders that ship type will accept from player
- Syntax: **( "String" "String" )**
- The names must match exactly, in lowercase. A name that doesn't match is ignored without a warning.
- **Orders:**
  - attack ship
  - disable ship
  - disarm ship
  - disable subsys
  - guard ship
  - ignore ship
  - form on wing
  - cover me
  - attack any
  - dock
  - rearm me
  - abort rearm
  - depart
  - stay near me
  - stay near ship
  - keep safe dist
  - ![FSO 22.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.2.svg) **Any [LuaAI order registered as a player order](https://wiki.hard-light.net/index.php/Dynamic_SEXPs#.24Player_Order:)**

#### +Auto Attacks:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Whether or not ship type will auto attack nearby ships
- Syntax: **Boolean**, Yes or No

#### +Attempt broadside:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Whether or not the ship type will use capital ship attack and movement behavior, IE: approach and orbit its target. This overrides general fighter attack behavior and anything derived from it will not work.
- Syntax: **Boolean**, Yes or No

#### +Actively Pursues:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Which ship types the current ship type will actively pursue (rather than simply return to guard position or whatnot)
- Syntax: **( "String" "String" )**, shiptypes

#### +Guards Attack this:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Whether ships assigned to guard another ship will attempt to engage ships of the type
- Syntax: **Boolean**, Yes or No

#### +Turrets Attack This:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Whether or not turrets will target the ships of the type
- Syntax: **Boolean**, Yes or No

#### +Can Form Wing:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Whether or not ship type can be formed into a wing
- Syntax: **Boolean**, Yes or No

#### +Active Docks:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Preferred docking bay type for ship when it is initiating the dock with another ship
- Syntax: **( "String" "String" )**, names of the dock bays
- **Dock bays:**
  - cargo
  - rearm
  - generic

#### +Passive Docks:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Preferred docking bay type for ship when it is receiving a dock from another ship
- Syntax: **( "String" "String" )**
- **Dock bays:**
  - cargo
  - rearm
  - generic

#### +Ignored on cripple by:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Defines the ship types which ignore this ship if its has been crippled
- Syntax: **String list**, ( "string" "string" )

#### +Targeted by 'Huge' weapons and Ignored by 'small only' weapons:

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

- Determines whether this ship type is a valid target for weapons with the 'huge' flag and to be ignored by weapons with the 'small only' flag.
- Syntax: **Boolean**
- Default: **Whether the ship is a default big or huge class: Cruiser, Freighter, Transport, Corvette, Gas miner, Awacs, Capital, Supercap, Drydock, or Knossos Device**

### $Explosion Animations:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the explosion animations used by ships of this type.
- Syntax: **List** of fireball names or indexes from [fireball.tbl](fireball.tbl); an index that doesn't exist logs a warning and is dropped

### $Skip Death Roll Percent Chance:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Allows ships to sometimes explode instantly instead of going through a death roll first. This can be overridden on a per-ship basis in [ships.tbl](ships.tbl).  Note that this option is misleadingly named; "vaporization" usually refers to a ship exploding into tiny debris shards rather than hull chunks.
- Syntax: ***Float***, percent chance of instant explosion, 0 to 100. A value outside that range logs a warning and 0 is used.
- ![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) This is the current name. The older name **$Vaporize Percent Chance:** (![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)) still works but is deprecated.

## #End

## Examples

### Default FreeSpace 2 Table

- This is the default table built into the engine, used when no objecttypes.tbl is found.

```
#Ship types

$Name:					Navbuoy
$Max Debris Speed:		200
$FF Multiplier:			1.0
$EMP Multiplier:		10.0
$Fog:
	+Start dist:			10.0
	+Compl dist:			500.0
$AI:
	+Actively Pursues:		( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Turrets attack this:	YES
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
$Skip Death Roll Percent Chance: 0.0

$Name:					Sentry Gun
$Counts for Alone:		YES
$On Hotkey List:		YES
$Target as Threat:		YES
$Show Attack Direction:	YES
$Max Debris Speed:		200
$FF Multiplier:			0.10
$EMP Multiplier:		10.0
$Protected on cripple:	YES
$Fog:
	+Start dist:			10.0
	+Compl dist:			500.0
$AI:
	+Accept Player Orders:	NO
	+Auto attacks:			YES
	+Actively Pursues:		( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Guards attack this:	YES
	+Turrets attack this:	YES
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
$Skip Death Roll Percent Chance: 0.0

$Name:					Escape Pod
$Praise Destruction:	YES
$On Hotkey List:		YES
$Warp Pushable:			YES
$Turrets prioritize ship target: YES
$Max Debris Speed:		200
$FF Multiplier:			1.0
$EMP Multiplier:		10.0
$Protected on cripple:	YES
$Fog:
	+Start dist:			10.0
	+Compl dist:			600.0
$AI:
	+Valid goals:			( "fly to ship" "attack ship" "attack wing" "dock" "waypoints" "waypoints once" "depart" "undock" "stay still" "play dead" "play dead (persistent)" "stay near ship" )
	+Actively Pursues:		( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Turrets attack this:	YES
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
$Skip Death Roll Percent Chance: 0.0

$Name:						Cargo
$Targetable as unscanned:	YES
$Scannable by default:		YES
$Max Debris Speed:		200
$FF Multiplier:			0.10
$EMP Multiplier:		10.0
$Beams Easily Hit:		YES
$Protected on cripple:	YES
$Fog:
	+Start dist:			10.0
	+Compl dist:			500.0
$AI:
	+Passive docks:			( "cargo" )
$Skip Death Roll Percent Chance: 0.0

$Name:					Support
$Counts for Alone:		YES
$Praise Destruction:	YES
$On Hotkey List:		YES
$Target as Threat:		YES
$Show Attack Direction:	YES
$Warp Pushable:			YES
$Turrets prioritize ship target: YES
$Max Debris Speed:		200
$FF Multiplier:			1.0
$EMP Multiplier:		3.5
$Protected on cripple:	YES
$Fog:
	+Start dist:			10.0
	+Compl dist:			500.0
$AI:
	+Valid goals:			( "fly to ship" "dock" "undock" "waypoints" "waypoints once" "stay near ship" "keep safe dist" "stay still" "play dead" "play dead (persistent)" "rearm ship" )
	+Accept Player Orders:	YES
	+Player orders:			( "rearm me" "abort rearm" "depart" )
	+Auto attacks:			YES
	+Actively Pursues:		( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Guards attack this:	YES
	+Turrets attack this:	YES
	+Active docks:			( "support" )
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
$Skip Death Roll Percent Chance: 0.0

$Name:					Fighter
$Counts for Alone:		YES
$Praise Destruction:	YES
$On Hotkey List:		YES
$Target as Threat:		YES
$Show Attack Direction:	YES
$Warp Pushable:			YES
$Turrets prioritize ship target: YES
$Max Debris Speed:		200
$FF Multiplier:			1.0
$EMP Multiplier:		4.0
$Protected on cripple:	YES
$Fog:
	+Start dist:			10.0
	+Compl dist:			500.0
$AI:
	+Valid goals:			( "fly to ship" "attack ship" "waypoints" "waypoints once" "depart" "attack subsys" "attack wing" "guard ship" "disable ship" "disable ship (tactical)" "disarm ship" "disarm ship (tactical)" "attack any" "attack ship class" "attack ship type" "ignore ship" "ignore ship (new)" "guard wing" "evade ship" "stay still" "play dead" "play dead (persistent)" "stay near ship" "keep safe dist" "form on wing")
	+Accept Player Orders:	YES
	+Player Orders:			( "attack ship" "disable ship" "disable ship (tactical)" "disarm ship" "disarm ship (tactical)" "guard ship" "ignore ship" "ignore ship (new)" "form on wing" "cover me" "attack any" "attack ship class" "attack ship type" "depart" "disable subsys" )
	+Auto attacks:			YES
	+Actively Pursues:		( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Guards attack this:	YES
	+Turrets attack this:	YES
	+Can Form Wing:			YES
	+Passive docks:			( "support" )
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
$Skip Death Roll Percent Chance: 0.0

$Name:					Bomber
$Counts for Alone:		YES
$Praise Destruction:	YES
$On Hotkey List:		YES
$Target as Threat:		YES
$Show Attack Direction:	YES
$Warp Pushable:			YES
$Turrets prioritize ship target: YES
$Max Debris Speed:		200
$FF Multiplier:			1.0
$EMP Multiplier:		4.0
$Protected on cripple:	YES
$Fog:
	+Start dist:			10.0
	+Compl dist:			500.0
$AI:
	+Valid goals:			( "fly to ship" "attack ship" "waypoints" "waypoints once" "depart" "attack subsys" "attack wing" "guard ship" "disable ship" "disable ship (tactical)" "disarm ship" "disarm ship (tactical)" "attack any" "attack ship class" "attack ship type" "ignore ship" "ignore ship (new)" "guard wing" "evade ship" "stay still" "play dead" "play dead (persistent)" "stay near ship" "keep safe dist" "form on wing" )
	+Accept Player Orders:	YES
	+Player Orders:			( "attack ship" "disable ship" "disable ship (tactical)" "disarm ship" "disarm ship (tactical)" "guard ship" "ignore ship" "ignore ship (new)" "form on wing" "cover me" "attack any" "attack ship class" "attack ship type" "depart" "disable subsys" )
	+Auto attacks:			YES
	+Actively Pursues:		( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Guards attack this:	YES
	+Turrets attack this:	YES
	+Can Form Wing:			YES
	+Passive docks:			( "support" )
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
$Skip Death Roll Percent Chance: 0.0

$Name:					Transport
$Counts for Alone:		YES
$Praise Destruction:	YES
$On Hotkey List:		YES
$Target as Threat:		YES
$Show Attack Direction:	YES
$Targetable as unscanned:	YES
$Scannable by default:	YES
$Max Debris Speed:		150
$FF Multiplier:			1.0
$EMP Multiplier:		2.0
$Warp Sound Range Multiplier:	6.0
$Beams Easily Hit:		YES
$Protected on cripple:	YES
$Fog:
	+Start dist:			10.0
	+Compl dist:			500.0
$AI:
	+Valid goals:			( "fly to ship" "attack ship" "attack wing" "dock" "waypoints" "waypoints once" "depart" "undock" "stay still" "play dead" "play dead (persistent)" "stay near ship" )
	+Accept Player Orders:	YES
	+Player Orders:			( "attack ship" "dock" "depart" )
	+Auto attacks:			YES
	+Attempt Broadside:		YES
	+Actively Pursues:		( "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Guards attack this:	YES
	+Turrets attack this:	YES
	+Can Form Wing:			YES
	+Passive docks:			( "support" )
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Targeted by 'Huge' weapons and Ignored by 'small only' weapons: YES
$Skip Death Roll Percent Chance: 0.0

$Name:					Freighter
$Counts for Alone:		YES
$Praise Destruction:	YES
$On Hotkey List:		YES
$Target as Threat:		YES
$Show Attack Direction:	YES
$Max Debris Speed:		150
$FF Multiplier:			1.0
$EMP Multiplier:		1.75
$Warp Sound Range Multiplier:	6.0
$Beams Easily Hit:		YES
$Protected on cripple:	YES
$Fog:
	+Start dist:			10.0
	+Compl dist:			600.0
$AI:
	+Valid goals:			( "fly to ship" "attack ship" "attack wing" "dock" "waypoints" "waypoints once" "depart" "undock" "stay still" "play dead" "play dead (persistent)" "stay near ship" )
	+Accept Player Orders:	YES
	+Player Orders:			( "attack ship" "dock" "depart" )
	+Auto attacks:			YES
	+Attempt Broadside:		YES
	+Actively Pursues:		( "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Guards attack this:	YES
	+Turrets attack this:	YES
	+Can Form Wing:			YES
	+Active docks:			( "cargo" )
	+Passive docks:			( "support" )
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Targeted by 'Huge' weapons and Ignored by 'small only' weapons: YES
$Skip Death Roll Percent Chance: 0.0

$Name:					AWACS
$Counts for Alone:		YES
$Praise Destruction:	YES
$On Hotkey List:		YES
$Target as Threat:		YES
$Show Attack Direction:	YES
$Max Debris Speed:		150
$FF Multiplier:			1.0
$EMP Multiplier:		0.8
$Warp Sound Range Multiplier:	6.0
$Beams Easily Hit:		YES
$Protected on cripple:	YES
$Fog:
	+Start dist:			10.0
	+Compl dist:			600.0
$AI:
	+Valid goals:			( "fly to ship" "attack ship" "attack wing" "dock" "waypoints" "waypoints once" "depart" "undock" "stay still" "play dead" "play dead (persistent)" "stay near ship" )
	+Accept Player Orders:	YES
	+Player Orders:			( "attack ship" "depart" )
	+Auto attacks:			YES
	+Attempt Broadside:		YES
	+Actively Pursues:		( "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Guards attack this:	YES
	+Turrets attack this:	YES
	+Can Form Wing:			YES
	+Passive docks:			( "support" )
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Targeted by 'Huge' weapons and Ignored by 'small only' weapons: YES
$Skip Death Roll Percent Chance: 0.0

$Name:					Gas Miner
$Counts for Alone:		YES
$Praise Destruction:	YES
$On Hotkey List:		YES
$Target as Threat:		YES
$Show Attack Direction:	YES
$Max Debris Speed:		150
$FF Multiplier:			1.0
$EMP Multiplier:		1.0
$Warp Sound Range Multiplier:	6.0
$Beams Easily Hit:		YES
$Protected on cripple:	YES
$Fog:
	+Start dist:			10.0
	+Compl dist:			600.0
$AI:
	+Valid goals:			( "fly to ship" "attack ship" "attack wing" "dock" "waypoints" "waypoints once" "depart" "undock" "stay still" "play dead" "play dead (persistent)" "stay near ship" )
	+Accept Player Orders:	YES
	+Player Orders:			( "attack ship" "depart" )
	+Auto attacks:			YES
	+Attempt Broadside:		YES
	+Actively Pursues:		( "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Guards attack this:	YES
	+Turrets attack this:	YES
	+Can Form Wing:			YES
	+Passive docks:			( "support" )
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Targeted by 'Huge' weapons and Ignored by 'small only' weapons: YES
$Skip Death Roll Percent Chance: 0.0

$Name:					Cruiser
$Counts for Alone:		YES
$Praise Destruction:	YES
$On Hotkey List:		YES
$Target as Threat:		YES
$Show Attack Direction:	YES
$Max Debris Speed:		150
$FF Multiplier:			1.0
$EMP Multiplier:		0.9
$Warp Sound Range Multiplier:	6.0
$Beams Easily Hit:		YES
$Protected on cripple:	YES
$Fog:
	+Start dist:			10.0
	+Compl dist:			600.0
$AI:
	+Valid goals:			( "fly to ship" "attack ship" "attack wing" "dock" "waypoints" "waypoints once" "depart" "undock" "stay still" "play dead" "play dead (persistent)" "stay near ship" )
	+Accept Player Orders:	YES
	+Player Orders:			( "attack ship" "depart" )
	+Auto attacks:			YES
	+Attempt Broadside:		YES
	+Actively Pursues:		( "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Guards attack this:	YES
	+Turrets attack this:	YES
	+Can Form Wing:			YES
	+Passive docks:			( "support" )
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Targeted by 'Huge' weapons and Ignored by 'small only' weapons: YES
$Skip Death Roll Percent Chance: 0.0

$Name:					Corvette
$Counts for Alone:		YES
$Praise Destruction:	YES
$On Hotkey List:		YES
$Target as Threat:		YES
$Show Attack Direction:	YES
$Max Debris Speed:		150
$FF Multiplier:			1.0
$EMP Multiplier:		0.3333
$Warp Sound Range Multiplier:	6.0
$Beams Easily Hit:		YES
$Protected on cripple:	YES
$Fog:
	+Start dist:			10.0
	+Compl dist:			600.0
$AI:
	+Valid goals:			( "fly to ship" "attack ship" "attack wing" "dock" "waypoints" "waypoints once" "depart" "undock" "stay still" "play dead" "play dead (persistent)" "stay near ship" )
	+Accept Player Orders:	YES
	+Player Orders:			( "attack ship" "depart" )
	+Auto attacks:			YES
	+Attempt Broadside:		YES
	+Actively Pursues:		( "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Guards attack this:	YES
	+Turrets attack this:	YES
	+Can Form Wing:			YES
	+Passive docks:			( "support" )
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Targeted by 'Huge' weapons and Ignored by 'small only' weapons: YES
$Skip Death Roll Percent Chance: 0.0

$Name:					Capital
$Counts for Alone:		YES
$Praise Destruction:	YES
$On Hotkey List:		YES
$Target as Threat:		YES
$Show Attack Direction:	YES
$Warp Pushes:			YES
$Max Debris Speed:		100
$FF Multiplier:			1.0
$EMP Multiplier:		0.2
$Beams Easily Hit:		YES
$Protected on cripple:	NO
$Fog:
	+Start dist:			10.0
	+Compl dist:			750.0
$AI:
	+Valid goals:			( "fly to ship" "attack ship" "attack wing" "waypoints" "waypoints once" "depart" "stay still" "play dead" "play dead (persistent)" "stay near ship" )
	+Accept Player Orders:	YES
	+Player Orders:			( "depart" )
	+Auto attacks:			YES
	+Attempt Broadside:		YES
	+Actively Pursues:		( "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Guards attack this:	YES
	+Turrets attack this:	YES
	+Can Form Wing:			YES
	+Passive docks:			( "support" )
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Targeted by 'Huge' weapons and Ignored by 'small only' weapons: YES
$Skip Death Roll Percent Chance: 0.0

$Name:					Super Cap
$Counts for Alone:		YES
$Praise Destruction:	YES
$On Hotkey List:		YES
$Target as Threat:		YES
$Show Attack Direction:	YES
$Warp Pushes:			YES
$Max Debris Speed:		100
$FF Multiplier:			1.0
$EMP Multiplier:		0.075
$Beams Easily Hit:		YES
$Protected on cripple:	NO
$Fog:
	+Start dist:			10.0
	+Compl dist:			1000.0
$AI:
	+Valid goals:			( "fly to ship" "attack ship" "attack wing" "waypoints" "waypoints once" "depart" "stay still" "play dead" "play dead (persistent)" "stay near ship" )
	+Auto attacks:			YES
	+Attempt Broadside:		YES
	+Actively Pursues:		( "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Guards attack this:	YES
	+Turrets attack this:	YES
	+Can Form Wing:			YES
	+Passive docks:			( "support" )
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Targeted by 'Huge' weapons and Ignored by 'small only' weapons: YES
$Skip Death Roll Percent Chance: 0.0

$Name:					Drydock
$Counts for Alone:		YES
$Praise Destruction:	YES
$On Hotkey List:		YES
$Target as Threat:		YES
$Show Attack Direction:	YES
$Max Debris Speed:		100
$FF Multiplier:			1.0
$EMP Multiplier:		0.5
$Beams Easily Hit:		YES
$Protected on cripple:	NO
$Fog:
	+Start dist:			10.0
	+Compl dist:			750.0
$AI:
	+Accept Player Orders:	YES
	+Auto attacks:			YES
	+Attempt Broadside:		YES
	+Actively Pursues:		( "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Guards attack this:	YES
	+Turrets attack this:	YES
	+Passive docks:			( "support" )
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Targeted by 'Huge' weapons and Ignored by 'small only' weapons: YES
$Skip Death Roll Percent Chance: 0.0

$Name:					Knossos Device
$Counts for Alone:		YES
$Praise Destruction:	YES
$On Hotkey List:		YES
$Target as Threat:		YES
$Show Attack Direction:	YES
$Max Debris Speed:		100
$FF Multiplier:			1.0
$EMP Multiplier:		0.10
$Protected on cripple:	NO
$Fog:
	+Start dist:			10.0
	+Compl dist:			1000.0
$AI:
	+Auto attacks:			YES
	+Attempt Broadside:		YES
	+Actively Pursues:		( "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Guards attack this:	YES
	+Turrets attack this:	YES
	+Passive docks:			( "support" )
	+Ignored on cripple by:	( "navbuoy" "sentry gun" "escape pod" "cargo" "support" "fighter" "bomber" "transport" "freighter" "awacs" "gas miner" "cruiser" "corvette" "capital" "super cap" "drydock" "knossos device" )
	+Targeted by 'Huge' weapons and Ignored by 'small only' weapons: YES
$Skip Death Roll Percent Chance: 0.0

#End
```
