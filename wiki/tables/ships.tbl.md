> **Revision information**
> OFP Revision: [#140](https://github.com/ofp-fs2open/fs2open.github.com/pull/140)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **ships.tbl** defines all the ship classes used in FSO: their models, physics, weapons, subsystems, effects and sounds.

This table is one of the Modular Tables and can be extended with **xxx-shp.tbm**.

Code: [/ship/ship.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/ship/ship.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in. Fields without a badge are from the original game.

## Contents

1. [General Format](#general-format)
2. [Default Player Ship](#default-player-ship)
3. [Engine Wash](#engine-wash)
4. [Ship Template Options](#ship-template-options)
5. [Ships Table Options](#ships-table-options)
6. [$Subsystem:](#subsystem)
7. [Ships.tbl flags](#shipstbl-flags)
8. [Wing Formations](#wing-formations)
9. [Player Ship Precedence](#player-ship-precedence)
10. [Sample](#sample)

---

## General Format

- ships.tbl is formed of several sections, in this order. Each section that begins with `#` needs `#End` before the next one.
  - **#Default Player Ship**: the default player ship, also used for any ship in a mission whose class isn't in the tables.
  - **#Engine Wash Info**: the engine washes used by the ships.
  - ![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg) **#Ship Templates**: templates ship classes can be based on (see [Ship Template Options](#ship-template-options)).
  - **#Ship Classes**: every ship class and its attributes (see [Ships Table Options](#ships-table-options)).
  - ![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) **#Wing Formations**: custom wing formations.
  - **$Player Ship Precedence**: the ships used in ship selection when the ship the mission designer chose isn't available to the player.
- The fields of a ship class must be in the order listed on this page. Most are optional.
- A ship class name used twice in ships.tbl is an error. In a modular table, a name that already exists edits that class (see `+nocreate` and `+remove`).

## Default Player Ship

- Defines the default ship in the game.
- Syntax: *Name*, name of a ship class as defined in [Ships Table Options](#ships-table-options).

## Engine Wash

- Defines the engines washes
- First entry must be named 'default'

### $Name:

- Defines the name of the engine wash.
- Syntax: ***String***

### +nocreate

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- In [Modular Tables](https://wiki.hard-light.net/index.php/Modular_Tables) a name can be followed by the line **+nocreate** to prevent the game from creating an incomplete entry with very few valid fields.  If the entry does not already exist (either in ships.tbl or in earlier parsed *-shp.tbm files), the modular entry is skipped.

### +remove

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- In [Modular Tables](https://wiki.hard-light.net/index.php/Modular_Tables) a name can be followed by the line **+remove** to completely remove a previously parsed entry.

### $Angle:

- Defines the angle from the engine where the engine wash exists
- Syntax: ***Float***, degrees

### $Radius Mult:

- Defines engine wash multiplier
- Syntax: ***Float***

### $Length:

- Defines the length of the engine wash
- Syntax: ***Float***, meters

### $Intensity:

- Defines the strength of the engine wash
- This setting does nothing
- Syntax: ***Float***, blast effect

**Example**

   $Name:           Default
   $Angle:          10.0
   $Radius Mult:    1.2
   $Length:         400
   $Intensity:      1.0

## Ship Template Options

> **Warning:** The ship templates feature, when originally introduced, caused crashes and memory issues. Between 2014/07/24 and 2015/8/20, it was removed from the codebase. In short: do not use in versions prior to 3.7.4.

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Templates are defined like ship classes, with `$Template:` instead of `$Name:`, and take the same fields. A ship class uses one with `+Use Template:` and then only needs the fields that differ.
- A template can't be edited by a modular table (`+nocreate` on a template logs a warning and is ignored).

### $Template:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the name of template
- Syntax: ***String***, name

### +Use Template:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Another template can be used as template for the current template
- Syntax: ***String***, name

### Example

Both GTB Artemis variants defined from a single template:

```
#Default Player Ship
$Name: GTF Ulysses
#End

#Engine Wash Info
...
#End

#Ship Templates
$Template: GTB Artemis Template
$Short name:					Bomber2T-03
$Species:                       Terran
+Type:                          XSTR("Medium Bomber", 2999)
+Maneuverability:				XSTR("Poor", 3000)
+Armor:                         XSTR("Medium", 3001)
+Manufacturer:					XSTR("Han-Ronald Corp.", 3002)
+Description:					XSTR(" ", 3003)
$end_multi_text
+Tech Description:
XSTR("The GTB Artemis is the most agile bomber in the GTVA fleet, which makes it ideal for quick-hitting missions against warships and other slow targets. Of the same technological generation as the Boanerges class, the Artemis is less heavily armed and carries a lighter weapon payload, but it is significantly more maneuverable. While the Boanerges can inflict more damage, the Artemis can undertake (and return from) missions the slower Boanerges would have no hope of surviving. All Artemis-class bombers in the GVTA fleet are currently serving with the 2nd and 3rd Battle Groups.", 3004)
$end_multi_text
+Length:          36 m
+Gun Mounts:      2
+Missile Banks:   3

$POF file:                      Bomber2T-03.pof
$Detail distance:       (0, 100, 300, 1300)
$Show damage:           YES
$Density:                       1
$Damp:                          0.35
$Rotdamp:                       0.55
$Max Velocity:          0.0, 0.0, 65.0
$Rotation time:         5.0, 4.5, 5.0
$Rear Velocity:         0.0
$Forward accel:         4.0
$Forward decel:         2.0
$Slide accel:           0.0
$Slide decel:           0.0
$Expl inner rad:        50.0
$Expl outer rad:        100.0
$Expl damage:           25.0
$Expl blast:            2000.0
$Expl Propagates:       NO                                                      ;; If set to Yes, then when the ship dies, the explosion propagates through it.
$Shockwave Speed:       0.0                                             ;; speed shockwave expands at, 0 means no shockwave
$Allowed PBanks:        ( "Subach HL-7" "Prometheus S" "Prometheus R" "Lamprey" "Circe" "Maxim")
$Allowed Dogfight PBanks:        ( "Subach HL-D" "Prometheus D" "Circe" "MorningStar D" "Lamprey" )
$Default PBanks:        ( "Subach HL-7" )
$Allowed SBanks:        ( "Rockeye" "Hornet" "Tornado" "Trebuchet" "Stiletto II" "Piranha" "Cyclops"  "cyclops#short"  "Hornet#Weak" "EMP Adv." "Infyrno" )
$Allowed Dogfight SBanks:        ( "Rockeye D" "Hornet D" "Tornado" "Piranha" "Infyrno D" )
$Default SBanks:        ( "Hornet" "Cyclops#short" "Cyclops#short" )
$SBank Capacity:        ( 40, 60, 60 )
$Shields:                       700
$Shield Color:		100 100 255
$Power Output:          4.0
$Max Oclk Speed:        65.0
$Max Weapon Eng:        100.0
$Hitpoints:             275
$Flags:                         ( "player_ship" "default_player_ship" "bomber")
$AI Class:                      Captain
$Afterburner:           YES
	+Aburn Max Vel: 0.0, 0.0, 110.0
	+Aburn For accel:       0.7
	+Aburn Fuel:            300.0
	+Aburn Burn Rate:       50.0
	+Aburn Rec Rate:        25.0
$Countermeasures:       35
$Scan time:                     2000
$EngineSnd:       127                   ;; Engine sound of ship
$Closeup_pos:           0.0, 0.0, -45
$Closeup_zoom:          0.5
$Shield_icon:           shieldbt-03
$Ship_icon:                     iconbomber2t-03
$Ship_anim:                     ssbomber2t-03
$Ship_overhead:		loadBomber2t-03
$Score:             10
$Trail:
	+Offset:		6.6 -6.25 -5.3
	+Start Width:	0.25
	+End Width:	0.05
	+Start Alpha:	1.0
	+End Alpha:	0.0
	+Max Life:	1.0
	+Spew Time:	120
	+Bitmap:	Contrail01
$Trail:
	+Offset:		-6.6 -6.25 -5.3
	+Start Width:	0.25
	+End Width:	0.05
	+Start Alpha:	1.0
	+End Alpha:	0.0
	+Max Life:	1.0
	+Spew Time:	120
	+Bitmap:	Contrail01
#End

#Ship Classes

$Name:                          GTB Artemis
+Use Template: GTB Artemis Template
$Subsystem:                     communications, 10,     0.0
$Subsystem:                     navigation,                     10,     0.0
$Subsystem:                     weapons,                                20,     0.0
$Subsystem:                     sensors,                                10,     0.0
$Subsystem:                     engine,                 15,     0.0

$Name:                          GTB Artemis D.H.
+Use Template: GTB Artemis Template
+Type:                          XSTR("Medium Bomber", 3005)
+Maneuverability:				XSTR("Average", 3006)
+Armor:                         XSTR("Medium", 3007)
+Manufacturer:					XSTR("Han-Ronald Corp.", 3008)
+Description:					XSTR(" ", 3009)
$end_multi_text
+Tech Description:
XSTR("The GTB Artemis D.H. is an experimental variant of the standard Artemis class. The most notable change is that the Artemis's factory-issued Han-Ronald engines have been replaced by the same Nankam NA-27f powerplant now found in the GTF Ares fightercraft. The D.H. variant is faster and more maneuverable than the standard Artemis bomber, at no cost to armor or armament. During the OpEval period, Artemis D.H. bombers are being deployed in only select squadrons in different theatres of the war. If the evaluation trials go well, wider deployment of the Artemis D.H. class is expected.", 3010)
$end_multi_text

$POF file:                      Bonus2t-01.pof
$ND: 					254 126 126
$ND: 					233 68 68
$ND: 					192 32 32
$Allowed PBanks:        ( "Subach HL-7" "Prometheus S" "Prometheus R" "Lamprey" "Circe" "Maxim")
$Allowed Dogfight PBanks:        ( "Subach HL-7" "Prometheus S" "Prometheus R" "Lamprey" "Circe" "Maxim")
$Default PBanks:        ( "Subach HL-7" )
$Allowed SBanks:        ( "Rockeye" "Hornet" "Tornado" "Trebuchet" "Stiletto II" "Piranha" "Cyclops"  "Cyclops#short"  "Hornet#Weak" "EMP Adv." "Infyrno" )
$Allowed Dogfight SBanks:        ( "Rockeye" "Hornet" "Tornado" "Trebuchet" "Stiletto II" "Piranha" "Cyclops"  "Cyclops#short"  "Hornet#Weak" "EMP Adv." "Infyrno" )
$Default SBanks:        ( "Hornet" "Cyclops#short" "Cyclops#short" )
$SBank Capacity:        ( 40, 60, 60 )
$Ship_icon:                     iconbonusarty
$Ship_anim:                     ssbonus2t-01
$Ship_overhead:		loadBonusartemis
$Subsystem:                     communications, 10,     0.0
$Subsystem:                     navigation,                     10,     0.0
$Subsystem:                     weapons,                                20,     0.0
$Subsystem:                     sensors,                                10,     0.0
$Subsystem:                     engine,                 15,     0.0

#End
```

## Ships Table Options

### $Name:

- Model name that is used also in game. It is also used as name for the whole ship entry. There are two special characters can be used in the name: **#** and **@**
  - **#**. It's used in the middle of the name and cannot be the first character in the $Name: string. It makes the right part of the name not being shown in the ship class info of the target window, (or beside the target brackets in FS_Open builds). It allows making different versions of a base ship while showing the same class name inside the game. An example from FSPort FreeSpace 1: The **SF Dragon#weakened** ship, used in [Enter the Dragon](https://wiki.hard-light.net/index.php/Enter_the_Dragon), appears to the player just as any other **SF Dragon**.
  - **@**. It's used before the proper ship name. It is fully ignored by the game. Moreover, **@WhateverShip** and **WhateverShip** are exactly the same name and therefore they both appear as **WhateverShip** inside the game. It just allows the user marking ships in the table because of any modding reason. In the retail FS2 ships there are several ships starting by **@**.
- Syntax: ***String***, name

### +nocreate

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- In [Modular Tables](https://wiki.hard-light.net/index.php/Modular_Tables) name can be followed by line **+nocreate** that prevent game from creating an incomplete entry by leaving the entry unread unless it already exists (either in ship.tbl or in earlier parsed *-shp.tbm files) by the time the *-shp.tbm is read.

### +Use Template:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Sets the table entry to use template based on an existing table entry instead of creating totally new one
- Syntax: ***String***, name of the template

### +Use Ship as Template:

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- Creates a copy of the specified ship to use as a base for this ship.
- ***Caution!*** This is highly dependent on the order the tables are loaded. If the ship used as a template is modified using `+nocreate`, the tabled changes will only be applied to the newly created ship if they are loaded *before* `+Use Ship as Template`. Any `+nocreate` changes *after* `+Use Ship as Template` will not have any effect on the newly created ship class. Thus, proper templates should be preferred if possible.
- Cannot be used in conjunction with `+Use Template`.
- Syntax: ***String***, name of the ship class to use as a template

### $Alt Name:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Alternative $Name: for the ship. Used to rename ships without breaking missions. Also accepted as `$Display Name:`.
- Syntax: ***String***, Alternate $Name:

### $Short name:

- Shortened model name, for FRED, Usually more descriptive than the real name.
- Syntax: ***String***'

### $Species:

- Defines species (Terran, Vasudan or Shivan). Determines many things, including the color of the engine glow, the flyby sound effects, the briefing icon and the support ship.

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- More species have been enabled, and all of these have to be defined in [species_defs.tbl](species_defs.tbl).

- Syntax: ***String***, name of the species
  - Example: *Terran*

### Technical Description

List of descriptive terms for techroom and ship selection screens. Number after the quotes refers to [tstrings.tbl](tstrings.tbl)

#### +Type:

- Syntax: XSTR("***String***", integer)
- Example: XSTR("", -1)

#### +Maneuverability:

- Syntax: XSTR("***String***", integer)
- Example: XSTR("", -1)

#### +Armor:

- Syntax: XSTR("***String***", integer)
- Example: XSTR("", -1)

#### +Manufacturer:

- Syntax: XSTR("***String***", integer)
- Example: XSTR("", -1)

#### +Description:

- Syntax: XSTR("***String***", integer)
- Requires **$end_multi_text** after the entry
- Example:
XSTR("", -1)
$end_multi_text

#### +Tech Title:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Defines name of the ship as displayed in the tech room
- Syntax: ***String***

#### +Tech Description:

- Syntax: XSTR("***String***", integer)
- Requires **$end_multi_text** after the entry
- Example:
XSTR("", -1)
$end_multi_text

#### +Length:

- Syntax: XSTR("***String***", integer)
- Example: XSTR("", -1)

#### +Gun Mounts:

- Syntax: XSTR("***String***", integer)
- Example: XSTR("", -1)

#### +Missile Banks:

- Syntax: XSTR("***String***", integer)
- Example: XSTR("", -1)

### $Selection Effect:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Defines which selection effect to use when the -ship_choice_3d commandline flag is enabled or no ani can be found for the selected ship.
- Syntax: String, either "FS1", or "Off". This defaults to the FS2 effect.


### $FS2 effect grid color:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- The color of the grid in the FS2 selection effect.
- Syntax: ***( Integer, Integer, Integer )***, red, green, blue, 0 to 255 (clamped)

### $FS2 effect scanline color:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- The color of the scan line in the FS2 selection effect.
- Syntax: ***( Integer, Integer, Integer )***, red, green, blue, 0 to 255 (clamped)

### $FS2 effect grid density:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- How many lines the FS2 selection effect's grid has. Must be above 0; anything else logs a warning and is ignored.
- Syntax: ***Integer***

### $FS2 effect wireframe color:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- The color of the wireframe in the FS2 selection effect.
- Syntax: ***( Integer, Integer, Integer )***, red, green, blue, 0 to 255 (clamped)

### $HUD Gauge Configs:

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- The named HUD layouts this ship class uses, from `$Name:` entries in [hud_gauges.tbl](hud_gauges.tbl).
- Syntax: ***( "String" "String" )***, HUD layout names

### $Cockpit POF file:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Filename of the cockpit model file
- Syntax: ***String.pof***, model filename

### $Cockpit Animations:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Allows specifying animations that only run on the cockpit model of a ship
- See [$Animations](#animations)
- Syntax: ***( "String" "String" )***, names of the animations for this ship, as defined in the Animation.tbl
  - Example: ( "wingflap" "docksequence" )

### $Driven Cockpit Animations:

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- See [Animation_Drivers#Parsing (+Driver Set:)](https://wiki.hard-light.net/index.php/Animation_Drivers#Parsing_(+Driver_Set:))
- Driven cockpit animations **are not** compatible with $Cockpit Animations! If you need cockpit animations without drivers, create a driver set that has no driver and list all animations in it's +Affected Animations:

### $Cockpit Moveables:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Allows specifying moveables that only run on the cockpit model of a ship
- See [$Moveables](Animation#moveables)
- Syntax: ***( "String" "String" )***, names of the moveable animations for this ship, as defined in the Animation.tbl
  - Example: ( "wingflap" "docksequence" )

### +Cockpit Offset:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the offset of the cockpit model
- Syntax: ***Vector***, three **floats**, x-axis, y-axis, z-axis, respectively


### +Cockpit Sway Multiplier:

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- How much the cockpit model sways with the ship's acceleration. 0 turns the sway off.
- Syntax: ***Float***

### $Cockpit Display:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- Defines a rectangle surface on a cockpit texture that can be updated dynamically. Multiple displays can be defined, but only one needs to be defined for each texture on the cockpit model.
- **+Texture:**
- The filename of the cockpit texture to be replaced.
- Syntax: ***String***, texture filename
- **+Offsets:**
- (Optional) The offsets of the rectangle surface into the texture that will be drawn into.
- Syntax: ***Vector***, two **integers**
- **+Size:**
- The size of the rectangle surface that will be drawn into.
- Syntax: ***Vector***, two **integers**
- **+Background:**
- (Optional) Filename of the bitmap to be used as the background of this display.
- Syntax: ***String***, texture filename
- **+Foreground:**
- (Optional) Filename of the bitmap to be used as the foreground of this display.
- Syntax: ***String***, texture filename
- **+Display Name:**
- A name to be used as a reference handle.
- Syntax: ***String***

### $POF File:

- Filename of the model file (.pof) at data/models folder
- Syntax: ***String.pof***, model filename

### $POF file Techroom:

- Filename of the model file (.pof) at data/models folder; used only in the weapon loadout screen
- Syntax: ***String.pof***, model filename

### $Texture Replace:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **+old:**
- Defines the texture to be replaced
  - Syntax: ***String***, filename
- **+new:**
- Defines the new texture
  - Syntax: ***String***, filename

### $POF Target File:

- Optional hud targeting model
- Syntax: ***String.pof***, model filename

### $POF Target LOD:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the LOD (Level-Of-Detail) of target model used in the HUD targetbox
- Syntax: ***Integer***

### $Detail Distance:

- Defines the distance where the change between different Levels-Of-Details (LODs) occurs
- Take notice that these are base values. Model detail in Detail options, (within game press F2), applies a multiplier to these values. These multipliers default to (from left to right): 1/8, 1/4, 1, 4, and 8, and may be overridden by the [$Detail Distance Multiplier](mission_profiles.tbl#detail-distance-multiplier)
- Syntax: ***(Detail Distances)*** , list of **integers** (same amount as LODs), distances from the model in meters
  - Example: *(0, 80, 300, 900)*

### $Collision LOD:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Defines an alternative LOD (Level-Of-Detail) to use for collision detection. Using this can increase performance without visible drawbacks if the LOD chosen has much simpler geometry but the shape still closely matches the base model.
- This works as follows: collision detection proceeds normally through the various model/submodel radius and bounding box checks based on LOD0, but when the game decides to perform a mesh-based collision check of a given submodel, it will use the submodel's LOD mesh instead. However, all submodels do not need to have a counterpart in the given LOD; if one is not found, the closest alternative will automatically be used instead.
- For example, if *$Collision LOD: 3* is used and the submodel engine01a is checked for a hit, the game will attempt to use the mesh of engine01d instead. However, if there is no engine01d submodel, the game will find the closest alternative: it'll look for engine01c, then engine01b, and finally fall back to engine01a. Note the implications of for example the root LOD submodel (detaild, detail3, etc) including the geometry of subsystem submodels; if detail0 is checked for a collision before engine01a, and detail3 includes the engine geometry (as opposed to having it in engine01d), then the subsystem engine01a cannot be hit.
  - Syntax: ***Integer***

### $ND:

NOTE: this option has done nothing since before 2008

- No Dim field field was used in FreeSpace for ship lights. Use of glowmaps is preferred to the $ND entry. Only noticeable in Glide or Software mode.
- Syntax: ***Color***, red, green, blue respectively, value from 0 to 255
  - Example: *42, 42, 224*

### $Enable Team Colors:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- Enables Team Colors for a ship without setting a default color setting.
- Syntax: ***Boolean***

### $Default Team:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- Specifies the default team name to be used for coloring a ship.
- None will set the default to 'no coloring'.
- Syntax: ***String***, name of the teams as defined in [colors.tbl](colors.tbl)

### $Show Damage:

- No function at all. The engine will read values, but discard them. In 3.6.10 or later, this can be left out without adverse consequences.
- Syntax: ***Boolean***, yes or no, usually yes

### $Damage Lightning Type:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Changes the damage lightning effect shown on highly damaged ships.
- Can be defined as:
  - *None*
  - *Default*

### $Impact:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **+Damage Type:**
  - Defines the damage type of the collision
  - Syntax: ***String***, name of the armor as defined in [armor.tbl](armor.tbl)

### $Impact Spew Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Specifies the [particle effect](Particle-Effects) which will be used on weapon impact,
- Syntax: **String**, the particle effect name
- If this option is specified then *$Impact Spew* is not a valid option!

### $Impact Spew:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Used to define the impact particle spew
- Note: You CANNOT define a different type of particle spew here; the only option possible is listed below
- **+Max particles:**
  - Defines the maximum number of particles spewed
  - Setting to zero causes model to generate no particles at all
  - Syntax: ***Integer***
  - Default Value: 30

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- **+Min particles:**
  - Defines the minimum number of particles spewed
  - Syntax: ***Integer***
  - Default Value: 25
- **+Max Radius:**
  - Defines the maximum radius of particles
  - Syntax: ***Float***
  - Default Value: 0.5f
- **+Min Radius:**
  - Defines the minimum radius of particles
  - Syntax: ***Float***
  - Default Value: 0.2f
- **+Max Lifetime:**
  - Defines the maximum lifetime of particles
  - Syntax: ***Float***
  - Default Value: 0.55f
- **+Min Lifetime:**
  - Defines the minimum lifetime of particles
  - Syntax: ***Float***
  - Default Value: 0.05f
- **+Max Velocity:**
  - Defines the maximum velocity of particles
  - Syntax: ***Float***
  - Default Value: 12.0f
- **+Min Velocity:**
  - Defines the minimum velocity of particles
  - Syntax: ***Float***
  - Default Value: 2.0f
- **+Normal Variance:**
  - Defines the variance of particle direction from the normal
  - Syntax: ***Float***
  - Default Value: 1.0f

### $Damage Spew Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Specifies the [particle effect](Particle-Effects) which will be used for the smoke of a weapon impact,
- Syntax: **String**, the particle effect name
- If this option is specified then *$Damage Spew* is not a valid option!
- Note that the scaling applied by *$Damage Spew* **is not** applied to*$Damage Spew Effect:* automatically. If desired, it has to be manually added using *$Particle Source Curve:* within the particle definition

### $Damage Spew:

See [Impact Spew entry](ships.tbl#impact-spew)
- Controls the 'smoke' released on weapon impact point
- Certain default values (marked with **X**) gain a specific multiplier based on ship radius
If the radius is less than 20 the multiplier is 1
If the radius is larger than 40 the multipler is 1.2
Otherwise it is between 1 and 1.2

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **+Max Particles:**
  - Default Value: 50 * **X**

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- **+Min Particles:**
  - Default Value: 20 * **X**
- **+Max Radius:**
  - Default Value: 1.3
- **+Min Radius:**
  - Default Value: 0.7
- **+Max Lifetime:**
  - Default Value: 1.5 * **X**
- **+Min Lifetime:**
  - Default Value: 0.7 * **X**
- **+Max Velocity:**
  - Default Value: 12.0
- **+Min Velocity:**
  - Default Value: 3.0
- **+Normal Variance:**
  - Default Value: 0.2 * **X**

### $Collision Physics:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Parameters affecting collision physics, including landings.
- "Landing" means the ship will not take damage or shake when it touches down. Instead, the ship will reorient towards a neutral resting orientation. "Reorient" thresholds below mean that the ship will still take damage (counts as a crash) but will move towards the correct landing orientation.

#### +Bounce:

When this ship collides with a large ship, how many meters to instantly move it away. Default is 5.0.

#### +Both Small Bounce:

When this ship collides with a small ship, how many meters to instantly move it away. Default is 5.0.

#### +Friction:

During collisions, how much friction to apply (slowing lateral movement). Default is 0.0.

#### +Rotation Factor:

Affects the rotational energy of collisions. Default is 0.2.

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

#### +Rotation Magnitude Maximum:

What the maximum resulting rotational velocity can be from a ship collision in degrees per second. Setting this to a value such as 360 (degrees/second) properly allows mods to have ships spin out if they have collision while not spinning out at wild speeds that look immersion breaking. If negative, then there is no limit to rotational velocity from collisions. Default is -1.0.

#### +Landing Max Forward Vel:

Maximum velocity for which landing physics are used.

#### +Landing Min Forward Vel:

Minimum velocity for which landing physics are used.

#### +Landing Max Descent Vel:

Maximum velocity at which the ship can "hit the deck" and still be considered a landing.

#### +Landing Max Horizontal Vel:

Maximum sideways velocity the ship can land at.

#### +Landing Max Angle:

Maximum angle of attack the ship can land at (in degrees).

#### +Landing Min Angle:

Minimum angle of attack the ship can land at (in degrees).

#### +Landing Max Rotate Angle:

How many degrees the ship can be rotated relative to the landing surface in order to count as a landing.

#### +Reorient Max Forward Vel:

Maximum velocity for which the ship will be adjusted to the correct landing orientation.

#### +Reorient Min Forward Vel:

Minimum velocity for which the ship will be adjusted to the correct landing orientation.

#### +Reorient Max Descent Vel:

Maximum velocity at which the ship can "hit the deck" and still reorient.

#### +Reorient Max Horizontal Vel:

Maximum sideways velocity for reorienting.

#### +Reorient Max Angle:

Maximum angle of attack for reorienting to kick in (in degrees).

#### +Reorient Min Angle:

Minimum angle of attack for reorienting to kick in (in degrees).

#### +Reorient Max Rotate Angle:

How many degrees the ship can be rotated relative to the landing surface in order to be reoriented on impact.

#### +Reorient Speed Mult:

How quickly the reorientation takes place (when applicable)

#### +Landing Rest Angle:

Angle of the ship's nose relative to the plane of the landing surface when the ship is at rest. Reorient will move the ship towards this angle.

#### +Landing Sound:

Sound to play when landing (if it's not a landing, normal collision sound is used).

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

#### +Collision Sound Light:

Sound when this ship collides normally.

#### +Collision Sound Heavy:

Sound when this ship collides at high speed.

#### +Collision Sound Shielded:

Sound when this ship collides with shields active.

### $Gravity Const:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- If the mission has gravity, this is the multiplier applied to acceleration from gravity on the ship.
  - Note that due to the exponential damping from $Damp, the acceleration from gravity will eventually even out with $Damp and result in a constant velocity in the direction of gravity.
  - Syntax: ***Float***

### $Dying Gravity Const:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Same as above, but while ths ship is dying/in death roll.
  - The aforementioned exponential damp is still in effect during this time, but it is weaker.
  - Syntax: ***Float***
  - Default: The same as $Gravity Const:

### $Debris:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **+Min Lifetime:**
  - Defines the minimum lifetime of the debris (Default value is a random number)
  - Syntax: **''Float,** Seconds
- '''+Max Lifetime:
  - Defines the maximum lifetime of the debris
  - Syntax: **''Float,** Seconds
- '''+Min Speed:
  - Defines the minimum speed of the debris
  - Syntax: **''Float,** Meters/second
- '''+Max Speed:
  - Defines the maximum speed of the debris
  - Syntax: **''Float,** Meters/second
- '''+Min Rotation speed:
  - Defines the minimum rotational speed of the debris (Default is (6 to 10)/Ship-radius)
  - Syntax: **''Float,** Radians/second
- '''+Max Rotation speed:
  - Defines the maximum rotational speed of the debris
  - Syntax: **''Float,** Radians/second
- **+Damage Type:**
  - Defines the damage type of the debris
  - Syntax: ***String***, name of the armor as defined in [armor.tbl](armor.tbl)

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- **+Min Hitpoints:**
  - Defines the minimum hitpoints assigned for the generated debris pieces
  - Syntax: ***Float***
- **+Max Hitpoints:**
  - Defines the maximum hitpoints assigned for the generated debris pieces
  - Syntax: ***Float***

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- **+Hitpoints Radius Multiplier:**
  - Defines the hitpoints multiplier. By default this is -1 and thus disabled, but if set to a value greater than or equal 0, the debris hitpoints will be multiplied by this value and the radius of the debris submodel. Useful for making debris have hitpoints that vary based on size of the debris piece.
  - Syntax: ***Float***

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- **+Damage Multiplier:**
  - Defines the collision damage multiplier for collisions against the generated debris pieces
  - Syntax: ***Float***
- **+Lightning Arc Percent:**
  - Controls what percent of debris pieces will have the damage lightning effect applied to them.
  - Defaults to 50%.
  - Syntax: **''Float,** 0-100

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- **+Debris Gravity Const:**
  - If the mission has gravity, this is the multiplier applied to acceleration from gravity on the ship's debris.
  - Syntax: ***Float***
  - Default: The same as $Dying Gravity Const

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- **+Debris Density:**
  - Same as [$Density:](#density) but only applied to the ship's debris.
  - Syntax: ***Float***
  - Default: 1.0

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- **+Ambient Sound:**
  - The sound played in the vicinity of debris.
- **+Collision Sound Light:**, **+Collision Sound Heavy:**
  - The sounds of the debris hitting something, normally and at high speed.
- **+Explosion Sound:**
  - The sound of the debris exploding.

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

- **+Generic Debris POF file:**
  - The filename of the ship's generic debris, by default debris01.pof.
  - Big or huge ships do not normally have generic debris, but if this is specified for them, they will generate generic debris on explosion.
  - These debris pieces are temporary, noncollideable and are generated in random positions and orientations.
  - Syntax: **String**
- **+Generic Debris Spew Num:**
  - How many generic debris fragments to generate when the ship explodes.
  - If this is a big or huge ship, this is the number of generic debris fragments to generate for every individual propagating explosion.
  - Syntax: **''Integer**
  - Default: *20*

### $Density:

- Multiplies the mass and moment of inertia of the relevant ship.
- Ships with a high mass and moment of inertia will be moved/rotated by collisions and impacts less strongly.
- Syntax: ***Float***, usually 1. Must not be 0.

### $Damp:

- Sets the natural period (1 / omega) of the dampening effects on top of the acceleration model. (See [$Forward Accel:](#forward-accel))
- In essence, affects how quickly you will accel/decel to your target velocity. Higher damp means slower acceleration and deceleration, while lower values (down to 0.0001) means faster acceleration/deceleration.
  - Specifying a value of 0.0 means there is no damping placed on top of the acceleration model, which is the most responsiveness you'll get.

- Syntax: ***Float***

### $Rotdamp:

- Shares the same principles of [$Damp](#damp), but is applied to rotations directly. (there isn't a engine/thrust acceleration model for rotations)
- Syntax: ***Float***

### $Banking Constant:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines a factor for how much roll is added during a yaw. Set as 1 for full roll and 0 is no roll. Default is 0.5.
- Syntax: ***Float***

### $Max Velocity:

- Defines maximum velocities with standard energy settings on x (left/right), y (up/down), and z (forward) -axis (respectively). z -axis velocity defines the maximum forward velocity.
- The x and y velocities are considered slide veocities
- Syntax: ***Vector***, three **floats**, x-axis, y-axis, z-axis, respectively, meters per second
  - Example: *0.0, 0.0, 80.0*

### $Player Minimum Velocity:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Defines minimum velocities on x (left/right), y (up/down), and z (forward) -axis (respectively). This affects the player only, the AI ignores the value.
- The x and y velocities are considered slide velocities, the minimums for them are **not currently implemented**.
- When gliding, if the absolute speed vector of the players ship falls below the minimum then the ships engines will re-engage and cannot be switched off again until the ships absolute vector exceeds the z-minimum.
- Syntax: ***Vector***, three **floats**, x-axis, y-axis, z-axis, respectively, meters per second
  - Example: *0.0, 0.0, 40.0*

### $Rotation Time:

- Defines rotation times around the x, y, and z-axis. Number is seconds it takes to complete rotate a ship (360 degrees) around the given axis. X-axis is "pitch," y-axis is "yaw," and z-axis is "roll."
- Syntax: ***Vector***, three **floats**, seconds per 360 degree turn around x-axis, y-axis, z-axis, respectively
  - Example: *3.0, 2.6, 5.0*
  - Note: Setting to Zero will cause the model to disappear in mission.

### $Rear Velocity:

- Defines rear velocity
- Syntax: ***Float***, meters per second
  - Note: This value must be set for [+Aburn_Max_Reverse_Vel:](#aburn-max-reverse-vel) to work

### $Forward Accel:

- Number of seconds needed from full stop to 63.21% of the maximum forward (z) velocity
  - We use 63.21% instead of 100% because FSO's acceleration/velocity model is based on a simplistic exponential function, rather than the more complex models seen for prop-driven, jet-driven, or rocket-driven craft.
  - The equation is a derivative of v = 1 - e^(-t / T), where T is what we define in the ship.tbl
- Syntax: ***Float***, seconds

### $Forward Decel:

- Number of seconds needed from maximum forward (z) velocity to full stop
- Syntax: ***Float***, seconds

### $Slide Accel:

- Number of seconds needed from full stop to 63.21% maximum slide velocity
- Note: this is applied to both slide (x and y) axes
  - See [$Forward Accel](#forward-accel) for an explanation of the 63.21%
- Syntax: ***Float***, seconds

### $Slide Decel:

- Number of seconds needed from 63.21% maximum slide velocity to full stop
- Note: this is applied to both slide (x and y) axes
  - See [$Forward Accel](#forward-accel) for an explanation of the 63.21%
- Syntax: ***Float***, seconds

### $Glide:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Allows gliding in game, i.e. turning the ship's attitude/facing without changing its current movement direction and speed.
- Syntax: ***Boolean***, yes or no

#### +Dynamic Glide Cap:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- If set, game uses dynamic glide cap. The dynamic glide cap means that the maximum glide speed will follow the same rules as normal flight, including the effects of afterburner and power to engines. If set to YES, the value in +Max Glide Speed is ignored.
- Syntax: ***Boolean***

#### +Max Glide Speed:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the maximum glide speed if glide has been enabled. If 0, this is set to the fastest speed the ship can go.
- If negative, there is no glide cap (allowing practically infinite maximum gliding speed), but only for the player, the AI will act as though it is still 0.
- Syntax: ***Float***
- Defaults to 0.

#### +Glide Accel Mult:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the acceleration multiplier for the gliding mode.
  - If > 0, sets a fixed acceleration rate in glide mode: ships will accelerate at their tabled maximum velocity along the given axis, interpreted in m/s^2, multiplied by *+Glide Accel Mult:*.
  - If set to 0, impossible to accelerate while in glide mode.
  - If negative, enables glide speed ramping: acceleration in glide mode ramps using the same constants as in normal flight. Note, however, that glide acceleration ignores the ship's *$Damp:* value, so glide acceleration will always be faster than in normal flight; and if the damping constant is significantly larger than the acceleration constant along a given axis, this difference will become very pronounced.
- Syntax: ***Float***
- Default: *0*

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Entry exists as a dummy entry which doesn't do anything to prevent compatibility issues

### $Use Newtonian Dampening:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- If set, forces the ship to use (or not use) newtonian dampening. Basically, allows overriding the AI Profiles flag on a per-ship basis.
- Syntax: ***Boolean***

### $Autoaim FOV:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the field of view for autoaim
- Syntax: ***Integer***

#### +Converging Autoaim

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines that the autoaim uses convergence

#### +Minimum Distance:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the minimum distance for the convergence

### +Primary Bank Autoaim FOV:

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- Defines the autoaim for the primary bank.
- Can be listed once for each existing bank on the ship.
- Syntax: ***Float***

### $Aims at Flight Cursor:

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- Enables flight cursor-style flying for the ship, and causes the ship's primaries to fire in the direction of the cursor.
- **Note:** Only works for players! AI may need a system like autoaim to mimic it.
- Syntax: ***Boolean***

### +Secondary Aims at Flight Cursor:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- As above, but for secondaries as well.
- Syntax: ***Boolean***

### +Extent:

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- Required if the above is true.
- How many degrees away from the center the player is allowed to shoot, using the flight cursor.
- Syntax: ***Float***, degrees

### $Convergence:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the weapons use convergence

> **Note:** If none of the following optional strings are added, the weapon will not converge. Don't forget to use them.

#### +Automatic

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines that the ship uses automatic convergence - converging distance calculated from estimated hit position (lead) of the current target.
- **+Minimum Distance:**
  - Defines the minimum distance for the convergence.  Overrides the setting for $Autoaim FOV's minimum distance.

#### +Standard

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines that ship uses fixed convergence.
- **+Distance:**
  - Defines the converging distance from the ship

#### +Offset:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the offset from the ship centerpoint for the convergence
- Syntax: ***Vector***, three **floats**, x-axis, y-axis, z-axis, respectively

### $Warpin Type:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Allow an animation to be defined as a warpin effect.
- Can be defined as:
  - *Default*
  - *Galactica* (called *BTRL* before 3.6.12; the old name is no longer accepted)
  - *Homeworld*
  - *Hyperspace*
  - ![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg) *Knossos*
  - ![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg) *Babylon5*
  - ![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg) *[Fireball Unique ID]* - Uses the default mechanics but replaces the portal effect with the identified fireball.
- Anything else logs a warning and Default is used.
- Syntax: ***String***

### $Warpin Start Sound:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- File for playing at start of warp in effect
- Syntax: ***String***, filename

### $Warpin End Sound:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- File for playing at end of warp in effect
- Syntax: ***String***, filename

### $Warpin Speed:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Sets the speed at which the ship warps in for **default** warpin effect
- Sets the time that the ships warp in (not the animation itself) takes for **BTRL** type warpin effects
- Syntax: ***Float***, meters per second or miiliseconds

### $Warpin Time:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the effect duration
- Syntax: ***Float***, seconds

### $Warpin Decel Exp:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Only for the **hyperspace** warpin effect.
- Defines an exponent for the deceleration curve at the end of the warpin. Must be higher than 0; a value of 1 produces a constant velocity throughout the effect and higher values produce gradual deceleration.
- Syntax: ***Float***
  - Default: 1.0

### $Warpin Radius:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the effect radius
- Syntax: ***Float***, meters

### $Warpin Animation:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the animation used for the Homeworld warpin effect. Has no effect for other warpin types.
- Syntax: ***String***, filename

### $Supercap warpin physics:

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- Specifies whether this ship should use supercap deceleration when arriving, similar to the Colossus. Can also disable supercap deceleration on a ship that normally has it.
- Syntax: ***Boolean***

### $Warpout Type:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Allow an animation to be defined as a warpout effect.
- Can be defined as:
  - *Default*
  - *Galactica* (called *BTRL* before 3.6.12; the old name is no longer accepted)
  - *Homeworld*
  - *Hyperspace*
  - ![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg) *Knossos*
  - ![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg) *Babylon5*
  - ![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg) *[Fireball Unique ID]* - Uses the default mechanics but replaces the portal effect with the identified fireball.
- Anything else logs a warning and Default is used.
- Syntax: ***String***

### $Warpout Start Sound:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- File for playing at start of warp out effect
- Syntax: ***String***, filename

### $Warpout End Sound:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- File for playing at end of warp out effect
- Syntax: ***String***, filename

### $Warpout Engage Time:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- Defines the delay the it takes for the warp drive of the ship to engage, during which the player may still abort the warpout. Affects both the player as well as AI ships. If not defined, [defaults to](game_settings.tbl#minimum-player-warpout-time) 3 seconds for the player and none for the AI.
- Syntax: ***Float***, seconds

### $Warpout Speed:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Sets the speed at which the ship warps out for **default** warpout effect
- Sets the time that the ships warp out (not the animation itself) takes for **BTRL** type warpout effects

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Sets the minimum speed required for the ship to warp out for **hyperspace** type warpout effects

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Syntax: ***Float***, meters per second or milliseconds

### $Warpout Time:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the effect duration
- Syntax: ***Float***, seconds

### $Warpout Accel Exp:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Only for the **hyperspace** warpin effect.
- Defines an exponent for the acceleration curve at the start of the warpout. Must be higher than 0; a value of 1 produces a constant velocity throughout the effect and higher values produce gradual acceleration.
- Syntax: ***Float***
  - Default: 1.0

### $Warpout Radius:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the effect radius
- Syntax: ***Float***, meters

### $Warpout Animation:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the animation used as the warpout effect.
- Syntax: ***String***, filename

### $Supercap warpout physics:

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- Specifies whether this ship should use special physics when departing, as a complement to $Supercap warpin physics.  Currently has no effect.
- Syntax: ***Boolean***

### $Player Warpout Speed:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Sets the speed at which the player warps out from the game
- Syntax: ***Float***

### $Expl Inner Rad:

- Radius at which the full explosion damage is done
- Syntax: ***Float***, meters

### $Expl Outer Rad:

- Maximum radius at which any damage is done
- Syntax: ***Float***, meters

### $Expl Damage:

- Amount of damage done inside the inner radius
- Syntax: ***Float***, damage

### $Expl Blast:

- The intensity of the blast effect (impulse, aka change in momentum) when you're within the outer radius
- Syntax: ***Float***, blast effect

### $Expl Propagates:

- This decides whether the ship has many smaller explosions while in its death-roll. Also determines whether the ship splits in two or not unless explicitly set below.
- Syntax: ***Boolean***, yes or no

### $Expl Splits Ship:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Whether the ship splits in two and slowly explodes outward towards its front and back, or just immediately explodes into debris.
- Syntax: ***Boolean***, yes or no, defaults to the value of '$Expl Propagates:' above

### $Propagating Explosion Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- The [particle effect](Particle-Effects) used for the explosions of a splitting ship, instead of the default one. Use this or `$Propagating Expl Radius Multiplier:`, not both.
- Syntax: **String**, the particle effect name

### $Propagating Expl Radius Multiplier:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- This sets the multiplier for the ship splitting propagating explosions
- Syntax: ***Float***, defaults to 1

### $Expl Visual Rad:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- This determines the visual size of the main fireball for an exploding ship. Does not affect ships with propagating explosions.
- Syntax: ***Float***, meters

### $Base Death-Roll Time:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Defines the base death-roll time
- Values is modified depending on conditions when the ship died
- Syntax: ***Integer***, milliseconds
  - Default: 3000

### $Death Roll Explosion Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- The [particle effect](Particle-Effects) used for the surface explosions during the death roll. Use this or `$Death-Roll Explosion Radius Mult:`, not both.
- Syntax: **String**, the particle effect name

### $Death-Roll Explosion Radius Mult:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Defines the multiplier for the radius of the ship surface explosions during death roll
- Syntax: ***Float***
  - Default: 1

### $Death-Roll Explosion Intensity Mult:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Defines the multiplier for the amount of the ship surface explosions during death roll
- Syntax: ***Float***
  - Default: 1

### $Death FX Explosion Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- The [particle effect](Particle-Effects) used for the ship's death explosions. Use this or `$Death FX Explosion Radius Mult:`, not both.
- Syntax: **String**, the particle effect name

### $Death FX Explosion Radius Mult:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Defines the multiplier applied to the ship death explosions
- Syntax: ***Float***
  - Default: 1

### $Death FX Explosion Count:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Defines the number of explosion effect game uses when a large ship dies
- Syntax: ***Integer***
  - Default: 6

### $Death Roll Rotation Multiplier:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Multiplies the strength of the rotation imparted by death-rolling.
- Syntax: ***Float***
  - Default: 1.0

### $Death Roll X rotation Cap:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

- Defines the cap (aka maximum) on X-axis (pitch) rotation during the ships deathroll. The value is radians per second. The current hard-coded minimum is 0.8. Note the code generally won't use this value for ships with radius > 150 (hard-coded).
- Syntax: ***Float***
  - Default: 4.725

### $Death Roll Y rotation Cap:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

- Defines the cap (aka maximum) on Y-axis (yaw) rotation during the ships deathroll. For other details see $Death Roll X rotation Cap:
- Syntax: ***Float***
  - Default: 4.725

### $Death Roll Z rotation Cap:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

- Defines the cap (aka maximum) on Z-axis (roll) rotation during the ships deathroll. For other details see $Death Roll X rotation Cap:
- Syntax: ***Float***
  - Default: 4.725

### $Ship Splitting Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Specifies the [particle effect](Particle-Effects) which will be used when a ship is splitting apart,
- Syntax: **String**, the particle effect name
- If this option is specified then *$Ship Splitting Particles* is not a valid option!

### $Ship Splitting Particles:

See [Impact Spew entry](ships.tbl#impact-spew)
- Controls the particles released on every explosion event which happens when ship is splitting apart
- The old 'popcorn' effect

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- **+Max Particles:**
  - Default Value: 80
- **+Min Particles:**
  - Default Value: 40
- **+Max Radius:**
  - Default Value: Ship radius * 0.015
- **+Min Radius:**
  - Default Value: Ship radius * 0.005
- **+Max Lifetime:**
  - Default Value: 6.0 * (1 + 0.002 * Ship radius)
- **+Min Lifetime:**
  - Default Value: 0.5 * (1 + 0.002 * Ship radius)
- **+Max Velocity:**
  - Default Value: Speed of the propagating explosion
- **+Min Velocity:**
  - Default Value: 0.0
- **+Normal Variance:**
  - Default Value: 2.0

### $Ship Death Effect:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Specifies the [particle effect](Particle-Effects) which will be used the final explosion of this ship type,
- Syntax: **String**, the particle effect name
- If this option is specified then *$Ship Death Particles* is not a valid option!

### $Ship Death Particles:

See [Impact Spew entry](ships.tbl#impact-spew)
- Controls the particles released on the moment 'non-knossos' type object dies

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- **+Max Particles:**
  - Default Value: 100
- **+Min Particles:**
  - Default Value: 50
- **+Max Radius:**
  - Default Value: 1.5
- **+Min Radius:**
  - Default Value: 0.1
- **+Max Lifetime:**
  - Default Value: 4.0
- **+Min Lifetime:**
  - Default Value: 0.5
- **+Max Velocity:**
  - Default Value: 20.0
- **+Min Velocity:**
  - Default Value: 0.0
- **+Normal Variance:**
  - Default Value: 2.0

### $Alternate Death Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Specifies the [particle effect](Particle-Effects) which will be used when a knossos type object is dying.
- Syntax: **String**, the particle effect name
- If this option is specified then *$Alternate Death Particles* is not a valid option!

### $Alternate Death Particles:

See [Impact Spew entry](ships.tbl#impact-spew)
- Controls the particles released when 'knossos' type object is dying

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- **+Max Particles:**
  - Default Value: 30
- **+Min Particles:**
  - Default Value: 15
- **+Max Radius:**
  - Default Value: 100.0
- **+Min Radius:**
  - Default Value: 30.0
- **+Max Lifetime:**
  - Default Value: 12.0
- **+Min Lifetime:**
  - Default Value: 2.0
- **+Max Velocity:**
  - Default Value: 350.0
- **+Min Velocity:**
  - Default Value: 50.0
- **+Normal Variance:**
  - Default Value: 2.0


### $Disable Main Fireball:

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- If YES, the ship doesn't make the big fireball when it finishes exploding (ships whose explosion splits them are not affected).
- Syntax: ***Boolean***

### $Debris Flame Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Specifies the [particle effect](Particle-Effects) which will be added to hull debris spawned from this ship.
- Syntax: **String**, the particle effect name

### $Shrapnel Flame Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Specifies the [particle effect](Particle-Effects) which will be added to shrapnel (small, non-hull) debris spawned from this ship.
- Syntax: **String**, the particle effect name

### $Debris Death Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Specifies the [particle effect](Particle-Effects) spawned when a piece of hull debris from this ship is destroyed. When set, it replaces the default fireball.
- Syntax: **String**, the particle effect name

### $Shrapnel Death Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Specifies the [particle effect](Particle-Effects) spawned when a piece of shrapnel debris from this ship is destroyed.
- Syntax: **String**, the particle effect name

### $Skip Death Roll Percent Chance:

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- Allows ships to sometimes explode instantly instead of going through a death roll first. Overrides the same flag in [objecttypes.tbl](objecttypes.tbl).
- Values outside 0 to 100 generate a warning and are set to 0.
- Syntax: ***Float***, percent chance of instant explosion, 0 to 100

### $Vaporize Percent Chance:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Allows ships to sometimes explode instantly instead of going through a death roll first. Overrides the same flag in [objecttypes.tbl](objecttypes.tbl).  Note that this option is misleadingly named; "vaporization" usually refers to a ship exploding into tiny debris shards rather than hull chunks.
- Syntax: ***Float***, percent chance of instant explosion, 0 to 100

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) **DEPRECATED**: Use [$Skip Death Roll Percent Chance:](#skip-death-roll-percent-chance) instead, which behaves identically. Only one of the two may be specified.

### $Shockwave Damage Type:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **REQUIRES ADDITIONAL TABLE**. Defines the damage type used for the ship explosions
- Syntax: ***String***, name of the armor as defined in [armor.tbl](armor.tbl)

### $Shockwave Speed:

- Speed shockwave expands at, 0 means no shockwave
- Syntax: ***Float***, meters per second

### $Shockwave Count:

- Defines the number of shockwaves to spawned
- Syntax: ***Integer***

### $Shockwave Model:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the textured model used as the ship's 3D shockwave
- Syntax: ***String.pof***, model filename

### $Shockwave Name:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the name of the animation to used for the 2D shockwave
- Syntax: ***String***, filename

### $Shockwave Sound:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Optional sound to be used for the sound heard when for the ship explosion shockwave (default is sound 109).
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $Explosion Animations:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the explosion animations used for the ship
- Syntax: ***Integer list***

### $Weapon Model Draw Distance:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Distance that external weapons are shown, ie. missiles and such.
- Syntax: ***Float***, meters
  - Default: *200*

### Primary Weapons

#### $Allowed PBanks:

- List of allowed primary weapons in brackets (weapon name marked with quotes).
- Syntax: ***( "String" "String" )***, names of the weapons

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Each of the primary weapon banks **can optionally** be given their own list of allowed weapons, in which case each bank has its own bracketed entry.
- Syntax: ***( "String" "String" ) ( "String" "String" )***

#### $Allowed Dogfight PBanks:

- Primary weapons for multiplayer dogfights.
- Syntax: ***( "String" "String" )***, names of the weapons

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Each of the primary weapon dogfight banks **can optionally** be given their own list of allowed weapons, in which case each bank has its own bracketed entry.
- Syntax: ***( "String" "String" ) ( "String" "String" )***

#### $Default PBanks:

- Defines default primary weapons for each of the primary weapon banks
- Syntax: ***( "String" "String" )***, names of the weapons

#### $PBank Capacity:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines ammunition capacity of primary weapon banks. In brackets, different weapon banks separated by commas.
- NOTE: In order for this to work, at least one of the primary weapons in the list of default primaries must be a ballistic weapon.
- Syntax: ***( Integer, Integer )***

#### $Show Primary Models:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the use of weapon models for primary weapons. In brackets, different weapon banks may have different definitions
- Syntax: ***( Boolean Boolean )***, yes or no
  - Example: *( YES YES )* - Note the spaces on the insides of the brackets.

### Secondary Weapons

#### $Allowed SBanks:

- Same for secondary weapons.
- Syntax: ***( "String" "String" )***, names of the weapons

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Each of the secondary weapon banks can be given their own list of allowed weapons, in which case each bank has its own bracketed entry.
- Syntax: ***( "String" "String" ) ( "String" "String" )***

#### $Allowed Dogfight SBanks:

- Same for secondary weapons.
- Syntax: ***( "String" "String" )***, names of the weapons

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Each of the secondary weapon dogfight banks can be given their own list of allowed weapons, in which case each bank has its own bracketed entry.
- Syntax: ***( "String" "String" ) ( "String" "String" )***

#### $Default SBanks:

- Defines default primary weapons for each of the primary weapon banks
- Syntax: ***( "String" "String" )***, names of the weapons

#### $SBank Capacity:

- Defines ammunition capacity of secondary weapon banks. In brackets, different weapon banks separated by commas.
- Syntax: ***( Integer, Integer )***

#### $Show Secondary Models:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the use of weapon models for secondary weapons. In brackets, different weapon banks may have different definitions
- Syntax: ***( Boolean Boolean )***
  - Example: *(YES YES)*
- Note: This requires the [$External_Model_File entry to be defined in weapons.tbl](weapons.tbl#external-model-file). Weapons without external models will revert to the standard behaviour.
- Behavior is as follows:
  - If your bank carries fewer or as many missiles as the number of gunpoints on your bank, you will have exactly as many external model shown as you have missiles left in your bank (going in the order of gunpoints defined in the POF). External models will disappear when missiles are fired.
  - If your bank carries more missiles than you have gunpoints in your bank, then external models will "recoil" (to simulate a reloading animation) every time you fire, and will disappear based on the % of missile left in the bank. Example: you have 2 gunpoints and 4 missiles, the first external model will disappear after you fire the second missile.
  - If your weapon uses the ["external_model_launcher" flag](weapons.tbl#external-model-launcher), then the model will be displayed on all gunpoints and will not recoil nor disappear when fired.

### $Ship Recoil Modifier:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Multiplies the recoil force of a weapon by this value, use a value greater than 1.0 to increase the force, use a value between 1.0 and 0.0 to decrease.
  - Syntax: ***Float*** (defaults to 1.0)

> **Note:** It is not required to have any weapons with the "apply recoil" flag set for this to be enabled.


### $Ship Shudder Modifier:

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- Multiplies how much the screen shakes when this ship fires a weapon that causes shudder.
- Syntax: ***Float*** (defaults to 1.0)

### $Allowed Firing Patterns for Dynamic Primary Linking:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- If the ship has dynamic primary linking enabled, defines the list of firing patterns which the player may cycle through using the appropriate control.
- This is optional. If not set, the only firing pattern available will be CYCLE FORWARD.
- Takes a list of patterns. The options are:
  - **CYCLE FORWARD**: Fires  **$Shots** shots from the first **$Shots** gunpoints (starting at gunpoint 0), then from the next **$Shots** gunpoints, and so on.
  - **CYCLE REVERSE**: Same behavior as CYCLE FORWARD, but begins at the highest-numbered gunpoint and works downward.
  - **RANDOM EXHAUSTIVE**: Fires **$Shots** shots at a time from randomly-selected gunpoints until each gunpoint has been fired once, and then starts over.
  - **RANDOM NONREPEATING**: Fires **$Shots** randomly-selected gunpoints at a time, but never fires the same gunpoint twice in a row (unless forced to because $Shots is equal to more than half the number of gunpoints).
  - **RANDOM REPEATING**: Fires **$Shots** randomly-selected gunpoints at a time. No restrictions.
- Syntax: **String list**, for example, ("CYCLE FORWARD", "CYCLE REVERSE")

### $Shields:

- Determines the shield strength
- Syntax: ***Float***, total shield strength

#### +Auto Spread:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Defines the thickness of auto spread shields. Note that the ["auto spread shields"](#auto-spread-shields) ship flag is also needed.
- Syntax: ***Float***, shield thickness in meters.

#### +Minimum Weapon Span:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Determines the minimum distance weapons must have traveled before impacting the shield (for example if fired close to or inside the shield). Does not affect whether the weapon bypasses shields, unless +Allow Bypass (see below) is set to *YES*. Only valid with auto spread shields.
- Syntax: ***Float***, defaults to the shield thickness.

#### +Allow Bypass:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Determines whether weapons which impact on the ship's hull (see +Minimum Weapon Span above) will bypass shields. Only valid with auto spread shields.
- Syntax: ***Boolean***
  - Default: *NO*

#### +Spread From LOD:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Causes the auto spread shields calculations to be done based off a LOD level other than LOD0. Using a lower-poly LOD will considerably speed up the shield collision detection, while being visually indistinguishable in most cases.
- Syntax: ***Integer***, defaults to 0

#### +Max Shield Impact Effect Radius:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Caps the size of the visual effect displayed on the shield mesh from impacts.
- Syntax: ***Float***, meters

### $Model Point Shield Controls:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Allows remapping of the 4 shield augmentation keys when [Model Point Shields](https://wiki.hard-light.net/index.php/Model_Point_Shields) are used. You should usually supply as many values as you have shield segments. The first value determines which augmentation key to use for the first shield segment, the second value determines which key to use for the second shield segment, and so on (the order being defined by the order of the shield points in the model file). If less than 4 values are given, "none" is assumed for the missing ones. Note that as there are only 4 augmentation keys, player ships with more than 4 shield segments would generally not be a good idea.
- Syntax: ***( String list )***, possible values "front", "left", "right", "rear" and "none"
  - Example: *( "front" "rear" )* for a ship with only two shield sections, front and rear; the left and right augmentation keys would be disabled.
  - Example: *( "left" "rear" "right" )* for a ship with only three shield sections, left, right and rear; the front augmentation key would be disabled.
  - Example: *( "front" "right" "rear" "left" )* for a ship with four shield sections but perhaps positioned in a non-standard way.

### $Shield Color:

- RGB value for shield color
- Syntax: ***Color***, red, green, blue respectively, value from 0 to 255
  - Example: *100, 100, 255*

### $Shield Impact Explosion Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Specifies the [particle effect](Particle-Effects) which will be triggered when this ship's shields are hit.
- Syntax: **String**, the particle effect name
- If this option is specified then *$Shield Impact Explosion* is not a valid option!

### $Shield Impact Explosion:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Impact explosion animation to trigger when this ship's shields are hit. Works identically for all shield types.
- Note: The size of the explosion is defined by the weapon, see [Weapons.tbl#.24Shield_Impact_Explosion_Radius:](https://wiki.hard-light.net/index.php/Weapons.tbl#.24Shield_Impact_Explosion_Radius:).
- Syntax: ***String***, filename or *none*

### $Max Shield Recharge:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Determines a maximum recharge limit of shields. When used, the combined maximum shield energy is lowered, meaning that shield energy can be moved between quadrants even with shields fully charged.
- Syntax: ***Float***, multiplier ranging from 0.0 to 1.0, defaults to 1.0
  - Example: *$Max Shield Recharge: 0.5* caps the combined shield energy to 50%, meaning that a ship with 4 shield quadrants will by default have 12.5% strength on all quadrants (instead of normal 25%), allowing it to for example augment one quadrant to 50% while dropping other quadrants to 0%.

### $Power Output:

- Actually does *NOTHING* unless set to zero; in which case weapon/shield/afterburner energy regeneration is disabled. Checked via code inspection 2014-12-29

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) If the current AI profile has the **$ETS uses ship class power output:** flag enabled, this will act as a multiplier for ETS energy.  For example, a ship with a power output of 2 will recharge its afterburner twice as fast as a ship with the same stats but a power output of 1.

- Syntax: ***Float***

### $Shield Regeneration Rate:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Repair rates for shields at percent per second
- Syntax: ***Float***, multiplier of total shield hitpoints recharged each second. (**NOT A PERCENTAGE**, 0.02 equals 2% recharged per second)
  - Default: *0.02*
- Affected by ETS Shields value (1/3rd value recharged per sec at default ETS setting of '4')
- Affected by AI Profile Setting: $Player Shield Recharge Scale:

### +Shield Regen Hit Delay:

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- Specifies a time delay after being hit by a weapon before the shield may begin regenerating again.
- Syntax: ***Float***, seconds

### $Support Shield Repair Rate:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Shield repair rate when docked to a support ship
- Syntax: ***Float***

### $Weapon Regeneration Rate:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Regeneration rates for primary guns at percent per second
- Syntax: ***Float***, multiplier of total gun energy recharged each second (**NOT A PERCENTAGE**, 0.04 equals 4% recharged per second)
  - Default: *0.04*
- Affected by ETS Guns value (1/3rd value recharged per sec at default ETS setting of '4')
- Affected by AI Profile Setting: $Player Weapon Recharge Scale:

### $Shield to Weapon Transfer Quantity:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- The fraction of *maximum* shield capacity to use when making direct shield -> weapon energy transfers.
- Syntax: ***Float***
  - Default: *0.2*
- **Note:** default direct transfers have a large and poorly-appreciated impact on ETS balancing. If you care about balancing your ship's weapon and shield recharge rates you are advised to set this to 0.

### $Shield to Weapon Transfer Efficiency:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- The fraction of the transferred shield energy that will actually end up in the weapon capacitor.
- Syntax: ***Float***
  - Default: *1.0*
- Note that the amount deposited will be absolute, not relative. For instance, for a ship with 100 weapon energy and 200 shields and default quantity and efficiency, a shield->weapon transfer will deduct 40 shield energy and add 40 weapon energy. It is entirely possible for efficiency to be more than 1.0.
- Unless transfer efficiency is tuned to account for the different charge rates for weapons and shields, direct transfers will tend to make the ETS unbalanced in the hands of an experienced player, as in retail.

### $Shield to Weapon Transfer Speed:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- The rate at which transferred energy will move into the weapon capacitor,
- Syntax: ***Float***
  - Default:*0.04*
- When energy is transferred it is instantly deducted from shields, but it must 'trickle' into the weapon capacitor at a fixed fraction per second. This acts as a second bonus recharge on top of whatever the ETS produces.

### $Weapon to Shield Transfer Quantity:

### $Weapon to Shield Transfer Efficiency:

### $Weapon to Shield Transfer Speed:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) As above, but with the transfer direction reversed.

### $Max Oclk Speed:

- Can also be **$Max Overclock Speed:**
- Maximum velocity with all energy diverted to engines
- Syntax: ***Float***, meters per second

### $Max Weapon Eng:

- Can also be **$Max Weapon Energy:**
- The maximum amount of energy you can store in your primary weapons bank.
- Syntax: ***Float***

### $Hitpoints:

- How much damage the ship can take
- Syntax: ***Float***, hitpoints
  - Default: *100*

### $Hull Repair Rate:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Repair rates for hull at percent per second
- Syntax: ***Float***, percentage of hitpoints repaired per second

### $Hull Self Repair Maximum:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

- Maximum percentage of total hull strength that the hull will self-repair to.
- Syntax: ***Float***, percentage of hull hitpoints

### $Support Hull Repair Rate:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Repair rate for hull when docked to a support ship
- Syntax: ***Float***

### $Subsystem Repair Rate:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Repair rates for sybsystems percent per second
- Syntax: *Value*, percentage of total subsystem hitpoints repaired per second

### $Subsystem Self Repair Maximum:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

- Maximum percentage of total subsystem strength that subsystems will self-repair to.
- Syntax: ***Float***, percentage of subsystems hitpoints

### $Support Subsystem Repair Rate:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Repair rate for subsystems when docked to a support ship
- Syntax: ***Float***

### $Armor Type:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **REQUIRES ADDITIONAL TABLE**. Defines armor type used for the ship
- Syntax: ***String***, name of the armor as defined in [armor.tbl](armor.tbl)

### $Shield Armor Type:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- **REQUIRES ADDITIONAL TABLE**. Defines the armor type used for shield of the ship
- Syntax: ***String***, name of the armor as defined in [armor.tbl](armor.tbl)

### $Flags:

- See [below](#shipstbl-flags)
- Syntax: ***( "String" "String" )***, names of the flags assigned to the ship
  - Example: ( "fighter" "in tech database" )

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **+noreplace**
  - Can be used to force game to retain existing flags instead of reseting the ship flags before parsing in the new flags.

### $AI Class:

- Default AI class
- Syntax: ***String***, name of the AI class as defined in [ai.tbl](ai.tbl)

### $Afterburner:

- Defines if the ships has afterburner
- Syntax: ***Boolean***, yes or no

#### +Aburn Max Vel:

- Syntax: ***Vector***, three **floats**, x-axis, y-axis, z-axis, respectively, meters per second

#### +Aburn For accel:

- Defined only for the forward acceleration. In same format as [$Forward Accel](#forward-accel)
- Syntax: ***Float***, seconds

#### +Aburn Max Reverse Vel:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defined only for the reverse acceleration, Sets the top speed for Reverse Afterburner.
- Syntax: ***Float***, seconds
  - NOTE: Requires [$Rear Velocity:](#rear-velocity) to be set in order to work.

#### +Aburn Rev accel:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defined only for the reverse acceleration. In same format as [$Forward Accel](#forward-accel)

- Syntax: ***Float***, seconds*'*

#### +Aburn Fuel:

- Amount of afterburner fuel the ship has
- Syntax: ***Float***, fuel

#### +Aburn Burn Rate:

- Rate the fuel is consumed when afterburner is engaged (in units per seconds)
- Syntax: ***Float***, fuel units per second

#### +Aburn Rec Rate:

- Rate the fuel is restored.
- Exact fuel recharged per second is given by the ETS value (as a percentage of total power output) multiplied by twice the tabled recharge rate, and then multiplied by the difficulty setting.
- Syntax: ***Float***, fuel units per second

#### +Aburn Minimum Start Fuel:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Minimum afterburner fuel the ship must have before allowed to afterburn.
- Syntax: ***Float***, fuel

#### +Aburn Minimum Fuel to Burn:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Forces the ship to afterburn until at least this much fuel is used, or it runs out of fuel.
- Syntax: ***Float***, fuel

#### +Aburn Cooldown Time:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- How much time must have elapsed since the last afterburn finished to allow afterburning again.
- Syntax: ***Float***, seconds

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

#### $Trails:

- Allows afterburner trails to be used

##### +Bitmap:

- Texture used to draw the afterburner trail
- Syntax: ***String***, filename

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

##### +Bitmap Stretch:

- Multiplies how far the bitmap is stretched over each trail segment.
- Syntax: ***Float***
  - Default:*1*

##### +Width:

- Width of afterburner trail at the beginning of it (ie. the thruster end)
- Syntax: ***Float***, meters

##### +Alpha:

- Transparency of the trail, value from 1 (non-transparent) to 0 (totally transparent)
- Syntax: ***Float***

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

##### +Alpha End:

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

- Transparency of the trail at its end.
- Syntax: ***Float***

##### +Alpha Decay Exponent:

- Causes the trail to fade away non-linearly. The 0 -> 1 progress through the trail's life is taken to the power of the provided number when being used for trail alpha purposes
- Smaller numbers below 1 cause the trail to fade away very slowly at first, and then suddenly at the end. Larger numbers above 1 cause it to fade away quickly at first, and then slow down for most of its life.
- Syntax: ***Float***, positive
  - Default:*1*

##### +Life:

- How long the trail will last
- Syntax: ***Float***, seconds

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

##### +Spread:

- Causes the trail points to diffuse and drift apart at this speed over time.
- Syntax: ***Float***, meters per second

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

##### +Faded Out Sections:

- Defines the number of the trail sections from the leading edge that are gradually faded away.
- Syntax: ***Integer***

### $Countermeasure Type:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the weapon used as a countermeasure. Weapon does not have to be included in the countermeasure section of the weapons.tbl, but it has to have "countermeasure" flag in that case.
- Syntax: ***String***, name of the weapon as defined in [weapons.tbl](weapons.tbl)

### $Countermeasures:

- Number of countermeasures that can be loaded to the ship
- Syntax: ***Integer***

### $Scan Time:

- Time it takes to scan the ship
- Syntax: ***Integer***, milliseconds

### $Scan range Normal:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

- Distance at which the ship can scan other small ships or large ship subsystems
- Syntax: ***Float***, distance

### $Scan range Capital:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

- Distance at which the ship can scan capital (& similar size) ships
- Syntax: ***Float***, distance

### $Scanning time multiplier:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

- A factor to apply to scan time when this ship is doing the scanning
- Syntax: ***Float***

### $Scanning range multiplier:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

- A factor to apply to scan distance when this ship is doing the scanning
- Syntax: ***Float***

### $Ask Help Shield Percent:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

- Percentage of shield damage at which the ship will send a call for assistance message.
- Syntax: ***Float***, percentage. Valid range 0.0-1.0

### $Ask Help Hull Percent:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

- Percentage of hull damage at which the ship will send a call for assistance message.
- Syntax: ***Float***, percentage. Valid range 0.0-1.0

### $EngineSnd:

- Engine sound used for the ship with number referring to sounds.tbl
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $Minimum Engine Volume:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Specifies how to multiply the volume of the engine sound based on ship speed. When the ship is stationary, the engine volume will be multiplied by the given value, with the value approaching 1.0 as the ship speed approaches maximum.
- Note: Default behavior is to ramp the multiplier from 0.5 (when ship is stationary) to 1.0 (when ship is at half speed or more).
- Syntax: ***Float***, volume multiplier, usually you'll want a value between 0.0 and 1.0

### $GlideStartSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used when entering glide mode, instead of the default throttle down sound.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $GlideEndSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used when exiting glide mode, instead of the default throttle up sound.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $Flyby Sound:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

- Optional sound to be used for the fly-by sound
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $CockpitEngineSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for looping engine sound heard in the cockpit.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $FullThrottleSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when setting throttle to full power.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $ZeroThrottleSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when setting throttle to zero power
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $ThrottleUpSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when increasing throttle power by 1/3
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $ThrottleDownSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when decreasing throttle power by 1/3
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $AfterburnerSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional optional sound to be used for the looping sound heard when the afterburner is active
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $AfterburnerEngageSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional optional sound to be used for the sound heard when the afterburner is engaged
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $AfterburnerFailedSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when the activation of the afterburner failed
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $MissileTrackingSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when a aspect seeking missile of the player tries to acquire a lock
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $MissileLockedSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when a aspect seeking missile of the player has acquired a lock
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $PrimaryCycleSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when the player cycles his primary weapon
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $SecondaryCycleSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when the player cycles his secondary weapon
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $TargetAcquiredSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when the player acquires a new target
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $PrimaryFireFailedSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when the primary weapon fails to fire
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $SecondaryFireFailedSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when the secondary weapon fails to fire
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $HeatSeekerLaunchWarningSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when a heat-seeker is fired at the player
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $AspectSeekerLaunchWarningSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when an aspect-seeker is fired at the player
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $MissileLockWarningSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when an aspect-seeker is locked onto the player
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $HeatSeekerProximityWarningSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when a heat-seeker is in close proximity to the player
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $AspectSeekerProximityWarningSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when an aspect-seeker is in close proximity to the player
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $MissileEvadedSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when a missile has been evaded
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $CargoScanningSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when the cargo of an object is being scanned
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $DeathRollSnd:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Optional sound to be used for the sound heard when the ship enters its death roll
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $ExplosionSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Optional sound to be used for the sound heard when the ship explodes
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $SubsysExplosionSnd:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Optional sound to be used for the sound heard when a subsystem on this ship explodes
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $Closeup_pos:

- How the model will show at techroom and briefing screen animations (assuming a pre-rendered animation is not specified). The position of the camera relative to the model in the tech room and the target box view. If 3D ship selection is used, also affects the drawing of the ship icon in the weapon & ship selection briefing screens.
- Syntax: ***Vector***, three **floats**, x, y and z values respectively

### $Closeup_zoom:

- How the model will show at techroom and briefing screen animations (assuming a pre-rendered animation is not specified). How far the camera's zoomed in, defines camera's FOV. If 3D ship selection is used, also affects the drawing of the ship icon in the weapon & ship selection briefing screens.
- Syntax: ***Float***, radians

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

### $Closeup_pos_targetbox:

- Optional value to set for the position of the camera relative to the model in the HUD target box view. If not set then the value for $Closeup_pos: will be used.
- Syntax: ***Vector***, three **floats**, x, y and z values respectively

### $Closeup_zoom_targetbox:

- Optional value to set how the model will show in the HUD target box view. How far the camera's zoomed in, defines camera's FOV. If not set then the value for $Closeup_zoom: will be used.
- Syntax: ***Float***, radians

### $Icon_closeup_pos:

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- The camera position for the ship's 3D icon in the ship and weapon selection screens. If not set, `$Closeup_pos:` is used.
- Syntax: ***Vector***, three **floats**, x, y and z values respectively

### $Icon_closeup_zoom:

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- The zoom for the ship's 3D icon in the ship and weapon selection screens. If not set, `$Closeup_zoom:` is used. A value of 0 or less is noted in the log and ignored.
- Syntax: ***Float***, radians

### $Topdown Offset:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Specifies how high the camera is above the center point of a ship in topdown camera views, or 2D missions.
- Syntax: ***Float***, meters

### $Chase View Offset:

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- Specifies a custom camera position when the ship is being viewed in the chase camera.
- Syntax: ***Vector***, three **floats**, x, y and z values respectively in meters

### $Chase View Rigidity:

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- Specifies how 'rigidly' the camera adheres to the 'rest' position of the chase view camera when doing maneuvers or changing speed.
- Syntax: ***Float***
  - Defaults to 5.0.

### $Shield_Icon:

- This .ani file must be at data/hud and included to the hud.tbl.

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Reference to the hud.tbl is no longer needed.

  - Syntax: *Filename*

### $Model Icon Direction:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- The direction from which the ship should be drawn from, if $Ship_Icon is not used and the ship icon is generated from the model.
  - By default, the direction is based on whether the game thinks the ship is a small ship, a capship or an installation.
  - Syntax: *String*, one of "top", "bottom", "front", "back", "left" or "right"

### $Ship_Icon:

- The icon used in ship selection.

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Ship model is used by default to generate the image.

  - Syntax: *Filename*

### $Ship_Anim:

- The glowing green grid animation used in ship selection.

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Ship model is used by default to generate the image.

  - Syntax: ***String***, filename

### $Ship_Overhead:

- The overhead view used in the weapons loadout.

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Ship model is used by default to generate the image.

  - Syntax: ***String***, filename

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

### $Wingmen Gauge Dot Override:

- (Optional) Defines an animation to use for the dots in the wingmen status gauge. Overrides 'Dot Filename:' under the '+Wingman Status:' entry in [Hud_gauges.tbl](hud_gauges.tbl#wingman-status)
- Must be an animation with two frames.
- Syntax: ***String***, filename

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

### $Briefing icon:

- Defines if the ship has its own briefing icon information which overrides the standard briefing icon image

#### +Regular:

- Syntax: ***<regular anim filename>***

#### +Fade:

- Syntax: ***<fade anim filename>***

#### +Highlight:

- Syntax: ***<highlight anim filename>***

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

### $Briefing icon with cargo:

- Defines if the ship has its own cargo briefing icon (which will only be an option if a ship icon is also specified)

#### +Regular:

- Syntax: ***<regular anim filename>***

#### +Fade:

- Syntax: ***<fade anim filename>***

#### +Highlight:

- Syntax: ***<highlight anim filename>***

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

### $Briefing wing icon:

- Defines if the ship has its own wing briefing icon (which will only be an option if a ship icon is also specified)

#### +Regular:

- Syntax: ***<regular anim filename>***

#### +Fade:

- Syntax: ***<fade anim filename>***

#### +Highlight:

- Syntax: ***<highlight anim filename>***

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

### $Briefing wing icon with cargo:

- Defines if the ship has its own wing-with-cargo briefing icon (which will only be an option if a ship icon is also specified)

#### +Regular:

- Syntax: ***<regular anim filename>***

#### +Fade:

- Syntax: ***<fade anim filename>***

#### +Highlight:

- Syntax: ***<highlight anim filename>***

### $Score:

- Points awarded for destroying the ship
- Syntax: ***Float***

### Customized Thruster Effects

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the texture of the modeled thruster effect cone
- **$Thruster Normal Flame:**
  - Syntax: ***String***, filename
- Defines the texture of the modeled thruster effect cone with afterburners lit
- **$Thruster Afterburner Flame:**
  - Syntax: ***String***, filename

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **Defines the primary thruster glow - the one visible only from behind**
  - **$Thruster Bitmap 1:**
    - Defines the effect for the standard drives. Overrides [species_defs.tbl](species_defs.tbl) line **+Normal:** under the **$Thrustglows:** entry.
    - Syntax: ***String***, filename
  - **$Thruster Bitmap 1a:**
    - Defines the effect for the afterburner. Overrides [species_defs.tbl](species_defs.tbl) line **+Afterburner:** under the **$Thrustglows:** entry.
    - Syntax: ***String***, filename
  - **$Thruster01 Radius factor:**
    - Defines the radius multiplier
    - Syntax: ***Float***
- **Defines the secondary glow type-effect for thrusters - the feathery trail like haze behind the main glow**
  - **$Thruster Bitmap 2:**
    - Defines the effect for the standard drives. Overrides [species_defs.tbl](species_defs.tbl) line **+Sec_Normal:** under the **$ThrustAnims:** entry.
    - Syntax: ***String***, filename
  - **$Thruster Bitmap 2a:**
    - Defines the effect for the afterburner. Overrides [species_defs.tbl](species_defs.tbl) line **+Sec_Afterburn:** under the **$ThrustAnims:** entry.
    - Syntax: ***String***, filename
  - **$Thruster02 Radius factor:**
    - Defines the radius multiplier
    - Syntax: ***Float***
  - **$Thruster02 Length factor:**
    - Defines the length multiplier for the trail type (secondary) glowpoint thrusters
    - Replaces the older **$Thruster01 Length factor:**
    - Syntax: ***Float***
- **Defines the tertiary glow type-effect for thrusters - the one visible from all directions**
  - NOTE: The rotation of this effect is jittered, which can look quite bad with certain types of effects.
  - **$Thruster Bitmap 3:**
    - Defines the effect for the standard drives. Overrides [species_defs.tbl](species_defs.tbl) line **+Ter_Normal:** under the **$ThrustAnims:** entry.
    - Syntax: ***String***, filename
  - **$Thruster Bitmap 3a:**
    - Defines the effect for the afterburner. Overrides [species_defs.tbl](species_defs.tbl) line **+Ter_Afterburn:** under the **$ThrustAnims:** entry.
    - Syntax: ***String***, filename
  - **$Thruster03 Radius factor:**
    - Defines the radius multiplier
    - Syntax: ***Float***

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- **Defines the distortion effect for thrusters**
  - **$Thruster Bitmap Distortion:**
    - Defines name of normal thruster distortion envelope animation. Overrides [species_defs.tbl](species_defs.tbl) line **+Dist_Normal:** under the **$ThrustAnims:** entry.
    - Syntax: ***String***, filename
  - **$Thruster Bitmap Distortion a:**
    - Defines name of afterburner thruster distortion envelope animation. Overrides [species_defs.tbl](species_defs.tbl) line **+Dist_Afterburn:** under the **$ThrustAnims:** entry.
    - Syntax: ***String***, filename
  - **$Thruster Distortion Radius factor:**
    - Defines the radius multiplier
    - Syntax: ***Float***
  - **$Thruster Distortion Length factor:**
    - Defines the length multiplier for the thruster distortion
    - Syntax: ***Float***
  - **$Thruster Distortion:**
    - Defines whether the distortion effect is rendered for this ship.
    - Syntax: ***Bool***

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- **$Thruster Glow Noise Mult:**
  - A multiplier for how much noise is applied to the thruster glow radius. A value of 0 causes the radius to remain perfectly steady and values higher than 1 cause the radius to fluctuate more than the default.
  - Defaults to 1.0.
  - Syntax: ***Float***

### $Thruster Particles:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Defines the alternative particle thruster.
- **$Thruster Effect:** OR **$Afterburner Effect:**
- Specifies the [particle effect](Particle-Effects) which will be used for thruster and afterburner effects.
- Any number of thruster and afterburner particles can be specified here.
- Afterburner particles require at least 1 existing particle thruster entry.
- These can be used both instead and in combination with the legacy particles (*$Thruster Particle Bitmap:*, *$Afterburner Particle Bitmap:*) described below
- Syntax: **String**, the particle effect name

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the alternative particle thruster
- **$Thruster Particle Bitmap:** OR **$Afterburner Particle Bitmap:**
  - Name of the animation used for particle thrusters, up to **three** can be defined for both **standard** thrusters and **afterburners** (6 in total). Afterburner particles require at least 1 existing particle thruster entry.
  - Syntax: ***String***, filename
- **$Min Radius:**
  - Defines the minimum radius of new particles
  - Syntax: ***Float***, meters
- **$Max Radius:**
  - Defines the maximum radius of new particles
  - Syntax: ***Float***, meters
- **$Min Created:**
  - Defines the minimum amount of particles created
  - Syntax: ***Float***
- **$Max Created:**
  - Defines the maximum amount of particles created
  - Syntax: ***Float***
- **$Variance:**
  - Value that describes the randomness of particle generation, values close to 0 will cause particles to spew very close to the thrusters normal while values close 1 will cause some particles to be spewed even to opposite direction
  - Syntax: ***Float***

### $Stealth:

- Defines: Ship behaves as a stealth-ship: non-targettable, and not on radar.

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg) This field is [deprecated](https://wiki.hard-light.net/index.php/deprecated), and mods using 3.6.8 and above should use the "stealth" flag.

### $Max Decals:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines: Maximum number of weapon decals on a ship
- Syntax: ***Integer***, maximum number of decals on ship
  - Default: 50 for anything below cruiser size, 100 for anything below superdestroyer/installation size, 300 for anything above destroyer size, 10 for everything else.

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Feature disabled. Parsing retained to maintain compatibility

### $Trail:

- These are the origins of the white contrails that appear in the nebula missions. Ship may have multiple trails.

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- **+ClearAll**
  - Clears all existing trail data from the ship

#### +Offset:

- The position of the contrail's head
- Syntax: ***Vector***, three **floats**, x, y and z values respectively, relative to the center point of the model

#### +Start Width:

- The width of the contrail's head
- Syntax: ***Float***, meters

#### +End Width:

- The width of the contrail's tail
- Syntax: ***Float***, meters

#### +Start Alpha:

- The opacity of the contrail's head
- Syntax: ***Float***, value between 0 and 1

#### +End Alpha:

- The opacity of the contrail's tail
- Syntax: ***Float***, value between 0 and 1

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

#### +Alpha Decay Exponent:

- Causes the trail to fade away non-linearly. The 0 -> 1 progress through the trail's life is taken to the power of the provided number when being used for trail alpha purposes
- Smaller numbers below 1 cause the trail to fade away very slowly at first, and then suddenly at the end. Larger numbers above 1 cause it to fade away quickly at first, and then slow down for most of its life.
- Syntax: ***Float***, positive
  - Default:*1*

#### +Max Life:

- How long the contrail remains
- Syntax: ***Float***', seconds

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

##### +Spread:

- Causes the trail points to diffuse and drift apart at this speed over time.
- Syntax: ***Float***, meters per second

#### +Spew Time:

- Time in milliseconds to create new trail points, lower means less 'jagged' trails.
- Syntax: ***Integer***

#### +Bitmap:

- The image to use for the contrail.
- Syntax: ***String***, filename

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

#### +Bitmap Stretch:

- Multiplies how far the bitmap is stretched over each trail segment.
- Syntax: ***Float***
  - Default:*1*

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

#### +Faded Out Sections:

- Defines the number of the trail sections from the leading edge that are gradually faded away.

### $Thruster:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines maneuvering thrusters. Ship may have multiple $Thrusters defined.
- **+Index:**
  - Optional
  - Rather than adding a new entry, an existing entry can be overriden by using its index in the list (starting at 0).
  - Syntax: ***Integer***
- **+Used For:**

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- This field may be omitted, and the activation of the thruster will be calculated automatically based on its position and direction.

  - Defines the maneuver that triggers the maneuvering thruster
  - List of maneuvers: Bank right, Bank left, Pitch up, Pitch down, Roll right, Roll left, Slide right, Slide left, Slide up, Slide down, Forward, Reverse
    - THESE NAMES MAY BE MISLEADING FOR YOU.
    - 'Roll left' and 'Roll right' are actually yaw, activated when the ship rotates around its top-bottom axis.
    - 'Bank left' and 'Bank right' are actually roll, activated when the ship rotates around its front-back axis.

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- 'Yaw left' and 'Yaw right' are now valid strings for this field.  'Roll left' and 'Roll right' are still recognized for compatibility.

  - Syntax: ***("String" "String")***
  - Allows one or more strings, does not have a set limit.
- **+Position:**
  - Defines thruster location
  - Syntax: ( ***Float*** ***Float*** ***Float*** ), position from the ships center point, x, y, z axis respectively
- **+Normal:**
  - Defines the direction where the thruster points to
  - Syntax: ( ***Float*** ***Float*** ***Float*** ), direction of the thrusters normal, x, y, z axis respectively
- **+Texture:**
  - Defines the graphic that will be used for trail, rendered like beams
  - Syntax: ***String***, filename
- **+Radius:**
  - Defines the radius of cylinder
  - Syntax: ***Float***, meters
- **+Length:**
  - Defines the length of thruster trail
  - Syntax: ***Float***, meters
- **+StartSnd:**
  - Defines the sound played when the thruster is started
  - Syntax:  ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)
- **+LoopSnd:**
  - Defines the sound played when the thruster is continuously used
  - Syntax:  ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)
- **+StopSnd:**
  - Defines the sound played when the thruster is shutting down
  - Syntax:  ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $Glowpoint overrides:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Creates a point that casts light onto ships and objects with the source being a glowpoint defined in the model file.
- Syntax: ***( "String:Integer" )***
- String refers to name in [glowpoints.tbl](glowpoints.tbl)
- Essentially, [Name_of_glowpoint_override_in_glowpoints.tbl]:[Bank_in_POF]. If there is no override entry then the game will not generate a light. If you have multiple banks that use the same override, use a comma if they are not in order (2,4,7) and use a dash if they are in order (3-5 yields 3, 4, and 5).
  - Example: **$Glowpoint overrides: ( "teal_glow_10:1" "red_glow:2,3" "white_glow_10:4" "red_glow_10:5" "green_glow_10:6")**

### $Radar Image 2D:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the bitmap used to represent the ship on the radar screen.
- Syntax: ***Filename***

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- **$Radar Color Image 2D:**
  - Defines the color bitmap used to represent the ship on the radar screen.
  - Syntax: ***Filename***

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- **$Radar Image Size:**
  - Defines the size of the bitmap on the radar screen (length of the longer dimension)
  - Syntax: ***Integer***
- **$3D Radar Blip Size Multiplier:**
  - Defines the multiplier used for blip size on the 3D radar.
  - Syntax: ***Float***

### $Ship IFF Colors: or $Ship IFF Colours:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the alternate color for the ship. Affects both the targeting brackets as well as the radar screen.
- **+Seen By:**
  - Defines the IFF actually viewing the object (most often 'Friendly')
  - Syntax: ***String***, name of an IFF class.
- **+When IFF Is:**
  - Defines the IFF which the ship has to have for its color to be altered.
  - Syntax: ***String***, name of an IFF class.
- **+As Color:** (or +As Colour:)
  - Defines the new color of the ship's targeting brackets and radar blip
  - Syntax: ***Integer list***, red, green and blue respectively
- Can be listed any number of times. An unknown IFF logs a warning.

### $Target Priority Groups:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines into which target priority groups the ship belongs to
- Syntax: ***String list***, names of the targeting groups
- **+Override**
  - Overrides the ships existing groupings

### $EMP Resistance Modifier:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the ships EMP resistance modifier. The strength of the emp effect ranges from 0-1, and this value will subtract from it, with 1 being completely immune.
- Syntax: ***Float***

### $Piercing Damage Draw Limit:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the maximum percentage of hitpoints ship has before piercing effects are enabled
- Syntax: ***Float***
  - Example: **10**, sets the limit to 10%

### $Path Metadata:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Allows definition of extra metadata
- Syntax: ***String***, name of the path as defined in the ship POF file.
  - Example: **$Path Metadata: $Bay01**

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

#### +arrival rvec:

- When using this path to arrive via fighterbay, the arriving ship will rotate to align itself with this rvec (so the right side of the ship is pointing in this direction). Only affects paths used to arrive via fighterbay.
- Syntax: ***Vector***, three **floats**, x-axis, y-axis, z-axis, respectively

#### +departure rvec:

- When using this path to depart via fighterbay, the departing ship will rotate to align itself with this rvec (so the right side of the ship is pointing in this direction). Only affects paths used to depart via fighterbay.
- Syntax: ***Vector***, three **floats**, x-axis, y-axis, z-axis, respectively

#### +arrive speed multiplier:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Modifies the speed of ships arriving from a fighterbay using this path. See [Ai_profiles.tbl#.24bay_arrive_speed_multiplier:](https://wiki.hard-light.net/index.php/Ai_profiles.tbl#.24bay_arrive_speed_multiplier:) for details.
  - Syntax: ***Float***

#### +depart speed multiplier:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Modifies the speed of ships departing to a fighterbay using this path. See [Ai_profiles.tbl#.24bay_depart_speed_multiplier:](https://wiki.hard-light.net/index.php/Ai_profiles.tbl#.24bay_depart_speed_multiplier:) for details.
  - Syntax: ***Float***

### $Passive Lightning Arcs:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

- Creates a 'lightning arc' between two points on the ship's hull, like when the ship is EMP'd or damaged, but naturally and fixed to two points on the ship.
- This section can be repeated.
- **All subfields are required**

#### +Submodel 1:

- The name of the name of the submodel the first end is connected to.
- Note that the arc will *not* move along with a submodel while it is animating (but it will be initially created in the correct position). In the case of an animating submodel, it is recommended to use very short durations/frequencies.
  - Syntax: ***String***

#### +Position 1:

- The position on the ship this end is attached to, relative to the origin of the entire ship.
  - Syntax: ***Vector***, three **floats**, x-axis, y-axis, z-axis, respectively

#### +Submodel 2:

#### +Position 2:

- The same as the above for the other end of the arc.

#### +Duration:

- How long the arc persists while active.
  - Syntax: ***Float***, seconds

#### +Frequency:

- How long after creating an arc it should wait before creating another.
  - Syntax: ***Float***, seconds

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

#### +Primary color 1:

- The first primary color of the arc, chosen randomly between color 1 and 2.
  - Syntax: three ***Integers***, 0-255

#### +Primary color 2:

- The second primary color of the arc, chosen randomly between color 1 and 2.
  - Syntax: three ***Integers***, 0-255

#### +Secondary color:

- The secondary color of the arc. This is the inner color, usually close to white.
  - Syntax: three ***Integers***, 0-255

### $Animations:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

- The new way of specifying triggered animations, replacing the subsystem-based definitions
- See [animation.tbl](Animation)
- Syntax: ***( "String" "String" )***, names of the animations for this ship, as defined in the Animation.tbl
  - Example: ( "wingflap" "docksequence" )

### $Driven Animations:

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- See [Animation_Drivers#Parsing (+Driver Set:)](https://wiki.hard-light.net/index.php/Animation_Drivers#Parsing_(+Driver_Set:))
- Driven animations **are not** compatible with $Animations! If you need animations without drivers, create a driver set that has no driver and list all animations in it's +Affected Animations:

### $Animation Moveables:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

- Like $Animations: but for specific in-mission adjustable moveable animations.
- See [animation.tbl](Animation)
- Syntax: ***( "String" "String" )***, names of the moveable animations for this ship, as defined in the Animation.tbl
  - Example: ( "wingflap" "docksequence" )

### $Custom data:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg) Defines a set of custom data that for this ship class, which can then be accessed through Lua scripts. There can be any amount of +Val: entries. It requires a $end_custom_data entry at the end.
Check the scripting documentation of Ship Class to find the CustomData Lua Table.

#### +Val:

Specifies a custom data value and its name, in the format

+Val: Name Value

These values will always be strings of text. If you use them in Lua as other types of value, you might need to convert them in the script.

#### $end_custom_data

Mandatory entry to indicate that the Custom Data list has ended.

### $Custom Strings

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg) Defines a set of custom strings for this ship class, which can then be accessed through Lua scripts. There can be any number of $Name: entries. It requires a $end_custom_strings entry at the end.
Check the scripting documentation of Ship Class to find the CustomStrings Lua Table.

#### $Name:

- The name of the custom string
- Syntax: ***String***

#### +Value:

- An arbitrary value, which can be used to group strings together
- Syntax: ***String***

#### +String:

- The text of the custom string
- Syntax: ***Multi-line String***

#### $end_custom_strings

Mandatory entry to indicate that the Custom Strings list has ended.

### $Default Subsystem Death Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Specifies the [particle effect](Particle-Effects) spawned at a subsystem of this ship when it is destroyed, for subsystems that do not define their own [$Subsystem Death Effect:](#subsystem-death-effect). When set, it replaces the default fireballs.
- Syntax: **String**, the particle effect name

### $Default Subsystem Debris Flame Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Specifies the [particle effect](Particle-Effects) which will be added to hull debris spawned from a subsystem of this ship, for subsystems that do not define their own [$Debris Flame Effect:](#debris-flame-effect).
- Syntax: **String**, the particle effect name

### $Default Subsystem Shrapnel Flame Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Specifies the [particle effect](Particle-Effects) which will be added to shrapnel debris spawned from a subsystem of this ship, for subsystems that do not define their own [$Shrapnel Debris Flame Effect:](#shrapnel-debris-flame-effect).
- Syntax: **String**, the particle effect name

### $Subsystem:

The options in this section are entered into each **$Subsystem:** entry of a ship class. Multiple subsystems may be defined.

- All subsystems a ship can have must be defined in the ship's model file (.pof).
- See [Subobject and subsystem properties](https://wiki.hard-light.net/index.php/Subobject_and_subsystem_properties) for information about what subsystems the game has defined meaning for.
- ![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg) For animating subsystems, use [Animation (-anim.tbm)](Animation) with [$Animations:](#animations) instead of the old [Animation Code](https://wiki.hard-light.net/index.php/Animation_Code) (still needed to set the initial orientation of multipart turrets).

#### $Subsystem: line

- The name of the subsystem as defined in .pof file, % of ship's total hitpoints the subsystem has and rotation time (360 degrees) for multi-part turrets. Note: Assigning zero to the %-hitpoint entry causes the subsystem to be without hitpoints, like fighterbays on some capital ships. If multiple ship classes use the same model, they should list the same subsystems in the same order.
- Syntax: ***String***, ***Float***, ***Float***, name of the subsystem followed by the subsystem hitpoints as % of ship's hitpoints and seconds per 360 degree turn. Subsystem name must be the same as described in model file (.pof).

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Entries can be left empty in which case game uses default values

  - Example: communications, 5, 0.0

#### $Alt Subsystem Name:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the alternate name for the subsystem. This name is shown for example in the targeting window when subsystem is targeted.
- Syntax: ***String***

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Option to use pipe character '|' to force line breaks into the alt subsystem name

#### $Alt Damage Popup Subsystem Name:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the alternate name for the subsystem when it is displayed on the **damaged systems** popup.
- Syntax: ***String***

#### $Armor Type:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **REQUIRES ADDITIONAL TABLE**. Defines armor type used for the subsystem. If subsystem armor type is not defined the ship's armor type is used instead.
- Syntax: ***String***, as defined in [armor.tbl](armor.tbl)

#### $Default PBanks:

- Defines default primary weapon for the turret
- Syntax: ( ***"String"*** )
  - Example: ( *"Subach HL-7"* )

#### $PBank Capacity:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines ammunition capacity of turret's ballistic primary weapon.
- Syntax: ( ***Integer*** )

#### $Default SBanks:

- Defines default secondary weapon for the turret
- Syntax: ( ***"String"*** )

#### $SBank Capacity:

- Defines ammunition capacity of turret's secondary weapon.
- Syntax: ( ***Integer*** )

#### $Engine Wash:

- All engines may use information concerning the engine wash. Engine washes are usually used only with larger vessels.
- Syntax: ***String***, name as defined in Engine Wash section in [ships.tbl](ships.tbl)
  - Example: *Default1000*

#### $AliveSnd:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Specifies a sound to play from the subsystem while it is not destroyed.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

#### $DeadSnd:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Specifies a sound to play from the subsystem while it is destroyed.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

#### $RotationSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Can be used to define a sound that is played when this subsystem rotates.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

#### $Turret Base RotationSnd:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the sound which is played when the turret's base rotates
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

#### $Turret Gun RotationSnd:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the sound which is played when the turret's guns change their elevation
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

#### $Turret BaseSnd Volume:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the multiplier for the sound volume for turret base rotations
- Syntax: ***Float***

#### $Turret GunSnd Volume:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the multiplier for the sound volume for turret gun elevation changes
- Syntax: ***Float***

#### $AWACS:

- Sets this subsystem's effective AWACS range. (AWACS reveals stealthed ships, cuts through nebula fog.)
- Syntax: intensity, ***Float***, and radius, ***Float***, - intensity is an ignored but required field. - radius is the maximum range of the AWACS.

#### $Scan time:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

- Scan time for this subsystem.  If this is not set, the scan time for the ship is used for the subsystem
- Syntax: ***Integer*** in milliseconds

#### $Maximum Barrel Elevation:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the maximum barrel elevation for the turret gun object of a multipart turret.
- Basically 'inverse FOV'.
- Syntax: ***Integer***, from 0 to 90

#### $Turret Base FOV:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the FOV of the turret base of a multipart turret, possibly restricting how far it can traverse left and right.
- Syntax: ***Integer***, from 0 to 359

#### $Turret Barrel FOV:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Defines the overall FOV of the turret, overriding any FOV defined in the POF.
- Syntax: ***Integer***, from 0 to 359

#### $Turret Reset Delay:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the time after losing the target before the turret begins to reset itself
- Syntax: ***Integer***, in milliseconds

#### $Turret Optimum Range:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the optimum range for the turret
- Syntax: ***Float***

#### $Turret Direction Preference:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the turret's preference to keep targeting targets based on how far the turret should turn instead of nearest target
- The higher the value the higher the preference to keep targeting targets close to the previous target's direction
- Syntax: ***Integer***, from 0 to 100, 0 disables the preference setup

#### $Target Priority:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the target priorities, and their order for the turret
- Syntax: ***String list***

#### $Max Turrets per Bomb:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Limits the number of turrets that can be targeting the same bomb.
- When this is set, when this turret tries to target a bomb, it will check to see how many turrets are already targeting it and only choose it as a target if there are fewer than this number.
- This allows control over the default behavior, where every turret will fire at the leading bomb.
- Syntax: ***Integer***

#### $Max Turrets per Target:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Limits the number of turrets that can be targeting the same target.
- **ONLY AFFECTS SMALL TARGETS** such as fighters and bombers. This limit does **NOT** apply vs large ships.
- When this is set, when this turret tries to target a ship, it will check to see how many turrets are already targeting it and only choose it as a target if there are fewer than this number.
- This helps turrets spread their efforts out among multiple targets.
- Syntax: ***Integer***

#### $ROF:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Defines the modifier applied to the turret rate of fire (ROF).

##### +Use firingpoints

- Modifies (multiplies) the rate of fire by the number of firingpoints the turret has.

##### +Multiplier:

- Checked only if *+Use firingpoints* has not been defined
- If set to larger than 0 the value acts as multiplier
- If set to 1.0 the feature is disabled
- If set to equal to or smaller than 0 it acts as if *+Use firingpoints* option would have been enabled
- Syntax: ***Float***

#### $Subsystem Death Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Defines the particle effect spawned at the subsystem when it is destroyed. When set, it replaces the default fireballs.
- If not set, the ship's *$Default Subsystem Death Effect:* is used, if any.
- Syntax: ***String***, name of the particle effect as defined in [Particle Effects](Particle-Effects)

#### $Debris Density:

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- Defines the density of hull debris from this subsystem, used to calculate the debris mass.
- Syntax: ***Float***, default 1.0

#### $Debris Flame Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Defines the flame particle effect attached to hull debris from this subsystem.
- If not set, the ship's *$Default Subsystem Debris Flame Effect:* is used, if any.
- Syntax: ***String***, name of the particle effect as defined in [Particle Effects](Particle-Effects)

#### $Shrapnel Debris Flame Effect:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Defines the flame particle effect attached to shrapnel debris from this subsystem.
- If not set, the ship's *$Default Subsystem Shrapnel Flame Effect:* is used, if any.
- Syntax: ***String***, name of the particle effect as defined in [Particle Effects](Particle-Effects)

#### $Flags:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Can be used to set additional subsystem related properties.  NOTE: specifying flags in a modular ships table will replace the original flags!  Use +noreplace to augment them instead.
- Syntax: ( ***"String" "String"*** )
- List of available flags:

##### "untargetable"

- Subsystem cannot be targeted

##### "carry no damage"

- Damage caused to the subsystem will not carry over to the parent ship

##### "use multiple guns"

- Enables the turret to fire all equipped weapons, picking a not-in-use firing point at each trigger. Not to be confused with 'Salvo mode'.

##### "fire down normals"

- Forces the turret to fire along the turret normals - regardless of the swarm setting.

##### "check hull"

- Forces the firing turret to check against the parent model if valid line of sight exists between the firingpoint and the target.

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

##### "fixed firingpoints"

- Forces the firing turret to retain the firingpoints of their weapons: if the turret has multiple banks, the first bank will only fire from the first firepoint, the second bank only from the second firepoint, and so on. Maximum number of weapons is the same as maximum number of weapons in turret (3 primary, 4 secondary)

##### "salvo mode"

- Forces the firing turret to use only the first weapon defined for it but fires it simultaneously from all the firing points. Not to be confused with 'use multiple guns'

##### "no subsystem targeting"

- Prevents the firing turret from targeting the target's subsystems - instead forces the turret to fire randomly at the target's hull.

##### "fire on target"

- Prevents the turret from firing unless the turret barrels (firingpoints) and targets are aligned.

##### "reset when idle"

- Forces the turret to reset to its initial position if it has no targets.

##### "carry shockwave"

- Forces the turret to carry damage caused by shockwaves to the hull regardless of other flags.

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

##### "allow landing"

- Defines the subsystem as something on which player can attempt to land on

##### "target requires fov"

- Prevents turret from accidentally selecting target out of its field of view (fov) when acquiring targets

##### "fov edge checks"

- The turret will more eagerly acquire targets as long as a sufficient portion of the target is within its fov.

##### "no replace"

- Tells game not to use the replacement submodel (-destroyed) when the subsystem loses all of its hitpoints.

##### "no live debris"

- Prevents the subsystem from releasing live debris pieces when destroyed

##### "ignore if dead"

- When subsystem has been destroyed prevents homing weapons from attempting to hit the now destroyed submodel and instead tells them to home on the main hull of the target. Note, for this to actually work you must also enable the AI Profiles flag of "$fix 'ignore if dead' flag" and/or target the mod to 26.0. If you do not do this then using this flag will mean that any weapon locked onto a destroyed subsystem with this flag will result in the weapon not homing at all.

##### "allow vanishing"

- Allow subsystem to be destroyed from the ship without sound or graphical effects

##### "damage as hull"

- Treats subsystem as if it were hull when applying damage.

##### "starts locked"

- Turret starts locked and won't shoot until you use a turret-free or turret-free-all SEXP on it.

##### "no aggregate"

- FreeSpace keeps track of a ship's overall functionality (such as if a ship is sensorless, disarmed or disabled) via a total hitpoint count of the associated subsystems. For example if a ship has three engines with 100HP each, the 'engines' count will be at 300HP. Shooting at any of the subsystems with the keyword 'engine' in them will decrement this total, and when it is at 0 then the ship becomes disabled.
- This flag tells FS to ignore the associated subsystem and not count it towards those totals. For example, if a ship had three 100HP engines with one set to 'no aggregate' then the 'engines' count will be 200HP. Shooting the 'no aggregate' engine will not decrement the 'engines' count, and so in other words the status of this engine is irrelevant to whether or not the ship is disabled. In the same way when applied to a turret, the turret's status is irrelevant to whether the ship is disarmed or not.

##### "wait for animation"

- When this flag is active, a turret will only fire after any associated subsystem animations (such as a turret hatch opening) have played out.

##### "play fire sound for player"

- If this turret is on a player ship, play its associated sounds.

##### "only target if can fire"

- Do additional checks to make sure that a turret which cannot fire at its target releases that target.

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

##### "no disappear"

- If this subsystem is a physical subsystem, it won't disappear nor switch to the -destroyed model when destroyed.

##### "collide submodel"

- If this subsystem is a physical subsystem, it will only take damage from hits which impact the submodel. By default damage is taken from all hits within the submodel's radius*2. Does not affect area damage such as shockwaves.

##### "allow destroyed rotation"

- Allows a rotating subobject to continue to rotate even when it has been destroyed

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

##### "turret use ammo"

- If the subsystem is a turret, it will use ammo.
  - Note: If the turret does not have ammo, it will not fire. Remember that the maximum ammo a turret can hold is the Capacity of the Bank divided by the Cargo Size of the weapon.

##### "autorepair if disabled"

- Allows the subsystem to repair itself even if disabled (normally, disabled subsystems do not repair themselves)

##### "don't autorepair if disabled"

- Inverse of "autorepair if disabled": excludes this subsystem from autorepair if the ship class ["repair disabled subsystems"](#repair-disabled-subsystems) flag is set

##### "share fire direction"

- If this subsystem is a turret, whenever the turret fires it will make all of the firing points shoot their weapon in the same direction instead of at the same point or set of points on a target.

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

##### "no damage spew"

- If this subsystem is hit then there will be no damage spew particles emitted.

##### "no impact debris"

- If this subsystem is hit then no impact debris will be generated.
- **DEPRECATED**: Use ["disable all generic impact debris"](#disable-all-generic-impact-debris) instead, which behaves identically.

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

##### "disable all generic impact debris"

- Newer name for ["no impact debris"](#no-impact-debris)--turns off the tiny debris shards that are typically spawned when something impacts a subsystem.

##### "disable all generic explosion debris"

- Turns off the generic debris shards that are typically spawned when the subsystem is destroyed. Live debris is not affected.

##### "burst ignores rof mult"

- If this subsystem is a turret, the delays between shots within a burst are not scaled by the turret's rate of fire multiplier (see [$ROF:](#rof)). The delay after a burst completes is still scaled.

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

##### "hide turret from loadout stats"

- This subsystem is not counted as a turret in the auto-generated "+Turrets:" line in the statistics summary window of the ship loadout editor.

![FSO 22.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.2.svg)

##### "turret has distant firepoint"

- Turret barrel is very long and should be taken into account when aiming (rather than calculating the aim from the turret center)

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

##### "override submodel impact"

- If a weapon impacted a submodel, but this subsystem is within range, this subsystem takes priority in the hit sequence rather than the submodel.  This allows case-by-case exceptions to the [$Damage Impacted Subsystem First:](game_settings.tbl#damage-impacted-subsystem-first) mod flag.

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

##### +noreplace

- Placed directly after the flag list. Adds the listed flags to the subsystem's existing flags instead of replacing them. Useful in modular tables.
- Syntax: ***+noreplace***

##### Legacy flag tags

- **DEPRECATED**: Use the equivalent **$Flags:** entries instead.
- These tags may follow **$Flags:** (or stand in its place), in this order:
  - **+non-targetable** - misspelled form of **+untargetable**; generates a warning
  - **+untargetable** - same as "untargetable"
  - **+carry-no-damage** - same as "carry no damage"
  - **+use-multiple-guns** - same as "use multiple guns"
  - **+fire-down-normals** - same as "fire down normals"

#### [Animation Code](https://wiki.hard-light.net/index.php/Animation_Code)

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Option for animating the submodels

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg) **DEPRECATED**: Unless setting initial orientation of multipart turrets, consider using the [animation.tbl](Animation) in combination with [$Animations:](#animations)

#### $On Beam Warmdown:

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg) Specifies an [action system](https://wiki.hard-light.net/index.php/Action_System) program set to be executed when a beam attached to this subsystem is starting its warmdown sequence.

### Case Specific Entries

#### Turret

- All turrets require information concerning the weapon mounted to the turret. The entry can be either **$Default PBanks** for turret primaries (including beams) or **$Default SBanks** for turret secondaries

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- [Animation Code](https://wiki.hard-light.net/index.php/Animation_Code#Setting_Turret_Angle) can be used with multi-part turrets.
- Multiple defined weapons will all fire but the targeting is handled by the first weapon in the list. **Multiple beams are not possible**. All weapons alternate between the firingpoints of the turret so turrets do not need to have more than one weapon defined no matter how many barrels or firingpoints they have.

##### Example

```
$Subsystem: turret01, 1.0, 1.0
$Default PBanks: ( "Terran Turret" )
```

#### Engine

##### Example

```
$Subsystem: engine01, 5.0, 0.0
$Engine Wash: Default1000
```

## Ships.tbl flags

### "no_collide"

- Defines ship as non-collideable, ie. everything will go through it
- Can also be written as "no-collide"

### "player_ship"

- Makes it possible for the player to fly the ship

### "default_player_ship"

- Makes this ship as a default part of the loadout options in FRED

---

### Ship Type

- Flags in this section are used to define the type of the ship
- Use only one **ship type** flag per table entry

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- New ship types can be defined in [objecttypes.tbl](objecttypes.tbl)

#### "repair_rearm" or "support"

- Defines the ship as repair and rearm vessel and sets AI behaviour accordingly

#### "cargo"

- Defines ship as a cargo crate and also sets the AI behaviour
- In FS1/FS2 ships that are marked with this flag cannot accept any player orders

#### "fighter"

- Defines ship as a fighter and also sets the AI behaviour
- In FS1/FS2 only ships flagged as fighters or bombers can accept the full set of player orders

#### "bomber"

- Defines ship as a bomber and also sets the AI behaviour
- In FS1/FS2 only ships flagged as fighters or bombers can accept the full set of player orders

#### "transport"

- Defines ship as a transport and also sets the AI behaviour

#### "freighter"

- Defines ship as a freighter and also sets the AI behaviour

#### "capital"

- Defines ship as a capital ship and also sets the AI behaviour
- By default, the ship's subsystems are scannable (this can be overridden by mission designers)
- The ship will warp out without ramping up to warpout speed
- When using the "Knossos" warpout, this ship will aim for the center of the portal

#### "supercap" or "super cap"

- Defines ship as a super capital ship and also sets the AI behaviour
- The following things are true for all ships with this flag:
  - When hit by debris, damage dealt by the collision is capped to between 10 and 20% of the ship's health maximum (determined randomly for each collision)
  - All damage dealt by weapons without the corresponding supercap flag from weapons.tbl will be reduced by 75%
  - In addition, once the ship is at or below 75% health, damage from non-supercap weapons is completely negated
  - Do note that shockwave damage, regardless of origin, is not subject to these rules unless the "$Weapon shockwave damage respects huge ship flags:" setting in game_settings.tbl is set to true
  - Ship subsystems are scannable by default (can be overridden by mission designers)
  - The ship does not need to ramp up to warpout speed before entering subspace
  - When using the "Knossos" warpout, the ship will aim for a departure point at the center of the Knossos portal

#### "drydock"

- Defines ship as a drydock and also sets the AI behaviour

#### "cruiser"

- Defines ship as a cruiser and also sets the AI behaviour

#### "navbuoy"

- Defines ship as a navbuoy and also sets the AI behaviour
- In FS1/FS2 ships that are marked with this flag cannot accept any player orders

#### "sentrygun" or "sentry gun"

- Defines ship as a sentrygun and also sets the AI behaviour
- In FS1/FS2 ships that are marked with this flag cannot accept any player orders

#### "escapepod" or "escape pod"

- Defines ship as a escapepod and also sets the AI behaviour
- In FS1/FS2 ships that are marked with this flag cannot accept any player orders

#### "corvette"

- Defines ship as a corvette and also sets the AI behaviour

#### "gas miner"

- Defines ship as a gas miner and also sets the AI behaviour

#### "awacs"

- Defines ship as a AWACS and also sets the AI behaviour

#### "knossos" or "knossos device"

- Defines ship as a Knossos and also sets the AI behaviour
- In FS1/FS2 ships that are marked with this flag cannot accept any player orders

#### "stealth"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines ship as a stealth-ship: non-targetable, and not on radar.

#### "no type"

- Does not enable any type-specific behavior

---

### "ship copy"

- Defines the ship as a copy of another ship. For ships which have this flag and are a hash-variant of another ship (like **SF Dragon** and **SF Dragon#2 (Weakened)**), the ship copy will be listed on the scoreboard as the original ship. This only affects the listing, not the score.

### "in tech database"

- Includes ships description to the tech database from the beginning of the campaign

### "in tech database multi"

- Includes the ship to multiplayer tech database

### "don't collide invisible" or "dont collide invisible"

- Defines invisible textures as non-collidable (For example, a hangar entrance)

### "big damage"

- Limits the damage made to the ship by non-huge weapons.

### "no_fred"

- Ship will not appear in any of the FRED dropdown lists for ship classes.

### "ballistic primaries"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Enables the ship to use ballistic primaries
- No longer needed; game handles it automatically

### "flash"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Makes a flash when a ship dies

### "surface shields"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Shots that impact the hull are applied to shields, regardless of if the ship has a modelled shield mesh.

### "show ship"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Enable rendering the ship even when the ship is a player ship
- Will result in the player's ship model being rendered, when in cockpit mode

### "generate icon"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Creates a HUD shield icon from the model itself

### "no weapon damage scaling"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Weapons that impact this ship will not by scaled according to 'big damage' or supercap rules

### "gun convergence"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Enables ship to use the firing points' normals in defining the direction where the weapon fire

### "no thruster geometry noise"

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Prevents noise from being added to the thruster effects geometry

### "intrinsic no shields"

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines that ship has no shields by default, to allow mixing and matching shielded and unshielded craft in loadout

### "dynamic primary linking"

![FSO 3.7](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.svg)

- Allows this ship use the primary firepoint cycling code.  With a key bind added in 3.6.13, you can toggle between various factorizations of primary firepoint groupings on a per-primary bank basis.  So a 4-firepoint bank could cycle through all four firepoints firing together (1x4), each firepoint firing individually (4x1, all 4 firing over the timespan of the bank cooldown, so no change in damage over time, or DPS, occurs from retail behavior), or firing in pairs (2x2).

### "no primary linking"

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Bypasses linked primaries while cycling.  Does not block SEXPs/scripts that might be able to lock them, only the act of cycling for players and determing whether to link or not for AI.

### "no pain flash"

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Prevents pain flash from being shown when ship is hit

### "no ets"

![FSO 3.7](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.svg) Sets ship class to have no ETS

### "no lighting"

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg) Ship will not receive lighting calculations

### "auto spread shields"

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) Causes the ship's shield mesh (if any) to be ignored and a shield of a given thickness to be automatically projected around the ship's hull. Weapons will impact the ship's shields at a distance equal to the shield thickness defined in [#.2BAuto_Spread:](#auto-spread), but not before the weapon itself has travelled at least a distance equal to that. Note that impact effects normally projected onto the shield mesh will not work.

### "model point shields"

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) Instead of the standard 4 shield quadrants, the ship uses an arbitrary number of shield sections which must have been defined in the model file, see [Model Point Shields](https://wiki.hard-light.net/index.php/Model_Point_Shields). If used on a player ship, you must also define [$Model_Point_Shield_Controls:](#model-point-shield-controls) to allow the shield augmentation controls to be used.

### "repair disabled subsystems"

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) Destroyed subsystems will auto-repair, provided [$Subsystem Repair Rate:](#subsystem-repair-rate) is greater than 0.

### "don't bank when turning"

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) All maneuvers by this ship class will have the same effect as the equivalent flag in `ship-maneuver`: the ship will not bank when changing its heading.
- Can also be written as "dont bank when turning"

### "don't clamp max velocity"

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) All maneuvers by this ship class will have the same effect as the equivalent flag in `ship-maneuver`: the ship's velocity will not be limited by its tabled max velocity, provided the max velocity is greater than 0.  Use with caution!
- Can also be written as "dont clamp max velocity"

### "instantaneous acceleration"

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) All maneuvers by this ship class will have the same effect as the equivalent flag in `ship-maneuver`: whenever the ship decides to change speed, it will instantaneously reach its desired speed.

### "large ship deathroll"

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Causes the ship to deathroll like a fighter, even if it is big or huge (which is disabled by default). Note that their deathroll intensity is scaled down by being larger size.

### "no impact debris"

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg) Turns off the tiny debris shards that are typically spawned when something impacts a ship.
- **DEPRECATED**: Use ["disable all generic impact debris"](#disable-all-generic-impact-debris) instead, which behaves identically.

### "disable all generic impact debris"

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) Newer name for ["no impact debris"](#no-impact-debris)--turns off the tiny debris shards that are typically spawned when something impacts a ship.

### "disable all generic explosion debris"

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) Turns off the tiny debris shards that are typically spawned when a ship explodes.

## Wing Formations

### $Name:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Provides a unique name for the formation, followed by the list of positions occupied by each member of the wing relative to their leader. The leader is always at 0,0,0 and is omitted, so the first position is the second in the wing, and second is the third in the wing, etc. Each formation must contain all 5 positions.
- NOTE: FSO automatically scales these these positions by the ship's size, so the table values will be very small. You can see the retail formation's values below for reference.
- Syntax: (a list of 5 ***Vectors***), each has three **floats**, x-axis, y-axis, z-axis, respectively

### Example

```
#Wing Formations

$Name: Default
((-0.39, 0.20, -0.59)
 ( 0.39, 0.20, -0.59)
 (-0.78, 0.39, -1.18) 
 ( 0.00, 0.39, -1.18)
 ( 0.78, 0.39, -1.18)) 

#End
```

## Player Ship Precedence

- Syntax: ( ***"String" "String"***)

### Example

```
$Player Ship Precedence: (
     "GTF Ulysses"
     "GTF Hercules"
     "GTB Medusa"
     "GTB Ursa"   )
```

## Sample

```
#Default Player Ship

$Name:                       GTF Ulysses

#End

#Engine Wash Info

$Name:                       Default
$Angle:                      10.0
$Radius Mult:                1.2
$Length:                     400
$Intensity:                  1.0

#End

#Ship Classes
$Name:                       GTF Ulysses
$Short name:                 TFight
$Species:                    Terran
   +Type:                       XSTR("Space Superiority", -1)
   +Maneuverability:            XSTR("Excellent", -1)
   +Armor:                      XSTR("Light", -1
   +Manufacturer:               XSTR("Triton / Mekhu", -1)
   +Description:                XSTR( " ", -1)
   $end_multi_text
   +Tech Description:           XSTR("Descriptive text", -1)
   $end_multi_text
   +Length:                     16 m
   +Gun Mounts:                 4
   +Missile Banks:              1
$POF File:                   fighter01.pof
$Detail Distance:            (0, 80, 300, 900)
$ND:                         42 42 244
$Show Damage:                YES
$Density:                    1
$Damp:                       0.1
$Rotdamp:                    0.35
$Max Velocity:               0.0, 0.0, 70.0
$Rotation Time:              3.0, 2.6, 5.0
$Rear Velocity:              0.0
$Forward Accel:              2.0
$Forward Decel:              1.5
$Slide Accel:                0.0
$Slide Decel:                0.0
$Expl Inner Rad:             25.0
$Expl Outer Rad:             55.0
$Expl Damage:                15.0
$Expl Blast:                 1000.0
$Expl Propagates:            NO
$Shockwave Speed:            0.0
$Allowed PBanks:             ( "Subach HL-7" "Prometheus R")
$Allowed Dogfight PBanks:    ( "Subach HL-D" "Prometheus D" )
$Default PBanks:             ( "Subach HL-7" "Prometheus R" )
$Allowed SBanks:             ( "Rockeye" "Tempest" )
$Allowed Dogfight SBanks:    ( "Tempest D"  "Hornet D" )
$Default SBanks:             ( "Harpoon" )
$SBank Capacity:             ( 40 )
$Shields:                    380
$Shield Color:               100 100 255
$Power Output:               2.0
$Max Oclk Speed:             94.0
$Max Weapon Eng:             80.0
$Hitpoints:                  180
$Flags:                      (  "player_ship"
                                "default_player_ship"
                                "fighter"
                                "in tech database")
$AI Class:                   Captain
$Afterburner:                YES
   +Aburn Max Vel:              0.0, 0.0, 150.0
   +Aburn For accel:            0.7
   +Aburn Fuel:                 300.0
   +Aburn Burn Rate:            50.0
   +Aburn Rec Rate:             25.0
$Countermeasures:            25
$Scan Time:                  2000
$EngineSnd:                  128
$Closeup_Pos:                0.0, 0.0, -22
$Closeup_Zoom:               0.5
$Shield_Icon:                shield-f01
$Ship_Icon:                  iconfighter01
$Ship_Anim:                  ssfighter01
$Ship_Overhead:              loadfighter01
$Score:                      8
$Trail:
   +Offset:                     -7.75 -2.8 0.75
   +Start Width:                0.25
   +End Width:                  0.05
   +Start Alpha:                1.0
   +End Alpha:                  0.0
   +Max Life:                   1.0
   +Spew Time:                  60
   +Bitmap:                     Contrail01
$Trail:
   +Offset:                     7.75 -2.8 0.75
   +Start Width:                0.25
   +End Width:                  0.05
   +Start Alpha:                1.0
   +End Alpha:                  0.0
   +Max Life:                   1.0
   +Spew Time:                  60
   +Bitmap:                     Contrail01
$Subsystem:    communication,   10, 0
$Subsystem:    navigation,      10, 0
$Subsystem:    weapons,         20, 0
$Subsystem:    sensors,         10, 0
$Subsystem:    engines,         35, 0

#End

$Player Ship Precedence: (
      "GTF Ulysses")
```
