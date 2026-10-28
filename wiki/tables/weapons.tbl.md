> **Revision information**
> OFP Revision: [#152](https://github.com/ofp-fs2open/fs2open.github.com/pull/152)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **weapons.tbl** defines all the weapons: primaries, secondaries, beams and countermeasures.

This table is one of the Modular Tables and can be extended with **xxx-wep.tbm**.

Code: [/weapon/weapons.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/weapon/weapons.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in. Fields without a badge are from the original game.

---

## Introduction

The weapons.tbl is responsible for defining the behavior of the numerous weapon effects used in FSO, including the amount of damage dealt, the firing rate, and graphical effects.

This wiki page goes over all of the entries used exclusively in the weapons.tbl file. Hyperlinks to individual weapon creation/editing tutorials are provided when appropriate.

## Special Case Weapons

### Beam Cannons

**REQUIRES:**
- **"beam"** flag
- **[$BeamInfo:](#beaminfo)**

See also: [Tutorial - Beam Weapons](http://www.hard-light.net/wiki/index.php/Tutorial_-_Beam_Weapons)

### Flak Guns

**REQUIRES:**
- **"flak"** flag
- **"particle spew"** flag
- Must have inner and outer radius defined to have any effect.
- A **.pof** model must be defined ( usually **Hornet.pof** )
  - Note: the model is not actually used... Instead, it uses [particle spew](#pspew). Trails may also be added.

## General Format

- The weapons table consists of several sections
- The fields of a weapon must be in the order listed on this page. Most are optional.
- A weapon name used twice in weapons.tbl is an error. In a modular table, a name that already exists edits that weapon (see `+nocreate` and `+remove`).
- The sections must be in this order. All sections beginning with # need #**End** before the next section.
  - #**Primary Weapons**
    - Defines all **standard primary weapons**, as well **flak weapons**.  Traditionally, **beam weapons** are also defined in this section.
  - #**Secondary Weapons**
    - Defines all **secondary weapons**.
  - #**Beam Weapons**
    - Defines **beam weapons**.  Traditionally they appear in the primary weapon section, but this is technically their proper place.
  - #**[Countermeasures](#countermeasures)**
    - Defines the used countermeasure.
  - **$Player Weapon Precedence**
    - Player Weapon Precedence is used in weapon selection when a default specified by the mission designer is not available to the player. The next weapon on the list is used in its place (assuming that weapon is allowed for the player).

NOTE: Beam weapons defined in the primary section will receive the WP_LASER subtype, and beam weapons defined in the beam section will receive the WP_BEAM subtype.  They will behave identically in most respects, with the only in-game difference being a somewhat larger and asymmetrical impact explosion effect for WP_BEAM.  In scripts, beams will return true for **isLaser()** if they have the WP_LASER subtype, and false if they have the WP_BEAM subtype.  (They will always return true for **isBeam()**.)

## Core Table

### $Name:

- This is the name of the weapon. If it's going to be used by the player, keep it under 12 characters so it doesn't spill over the HUD box. Two special characters can be used in the name: **#** and **@**. Their usage details are the very same than inside the [$Name field from Ships.tbl](ships.tbl#name-2).
- Syntax: ***String***

#### +nocreate

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- In [Modular Tables](https://wiki.hard-light.net/index.php/Modular_Tables) a name can be followed by the line **+nocreate** to prevent the game from creating an incomplete entry with very few valid fields.  If the entry does not already exist (either in weapons.tbl or in earlier parsed *-wep.tbm files), the modular entry is skipped.

### +remove

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- In [Modular Tables](https://wiki.hard-light.net/index.php/Modular_Tables) a name can be followed by the line **+remove** to completely remove a previously parsed entry.

### $Alt Name:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- An alternative name that can be referenced instead of $Name:
- Also accepted as `$Display Name:` (![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)).
- Syntax: ***String***

### $Subtype:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg) Specifies the weapon 's subtype. Overrides the default subtype as specified by the #-section it is in (#Primary, #Secondary, #Beam Weapon, etc.).

> **Note:** Since the weapon subtype is already specified by the #-section it is in, this option is not particularly useful when the weapon is inside a #Primary or #Secondary section. However, it may be used to specify the weapon slot it uses on fighters/bombers for weapons that are normally capship only (such as beam cannons).

- Syntax: ***String***
- Valid Entries:
Primary
Secondary

### +Title:

- This is the title of the weapon in the Weapons loadout screen. Only needed for player allowed weapons. Title may be anything.  Numerical value is a reference to translations in [tstrings.tbl](tstrings.tbl)
- Syntax: XSTR(***"String"***, -1)

### +Description:

- This is the description you see in the weapons loadout screen. Again, these aren't based on any hard numbers, just approximations. Only needed for player allowed weapons. Numerical value is a reference to translations in [tstrings.tbl](tstrings.tbl)
- Must be followed by **$end_multi_text**

**Example**

```
XSTR(
"''Standard Issue
''Level 3 Hull Damage
''Level 2 Shield Damage''", -1)
$end_multi_text
```

### +Tech Title:

- Title of the weapon in the Tech room. Only needed for player allowed weapons.
- Syntax: XSTR(***"String"***, -1)

### +Tech Anim:

- Animation used for the weapon in the Tech room. Only needed for player allowed weapons.
- Syntax: ***String***, animation filename
  - Example: *Tech_Subach_HL-7*

### +Tech Description:

- Descriptive text you see in the tech room. Only needed for player allowed weapons. Numerical value is a reference to translations in [tstrings.tbl](tstrings.tbl)
- Must be followed by **$end_multi_text**

**Example**

```
XSTR(
"''The Subach-Innes HL-7...''", 3245)
$end_multi_text
```

### $Turret Name:

![FSO 22.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.2.svg)

- Renames a turret to this if this is the first weapon on a turret.
- Syntax: XSTR(***"String"***, -1)

### $Tech Model:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines model used in wing loadout screen for the weapon. 3D weapon models in the tech room have not yet been implemented.
- Syntax: ***String.pof***, model filename

#### +Closeup_Pos:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- How the model will show on the Loadout Screen. The position of the camera relative to the model center. Only valid if a tech model is defined
- Syntax: ***Vector***, three **floats**, x, y and z values respectively

#### +Closeup_Zoom:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- How the model will show on the Loadout Screen. How far the camera's zoomed in, defines camera's FOV.
- Syntax: ***Float***, radians


#### +Icon_closeup_pos:

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- The camera position for the weapon's 3D icon in the weapon selection screen. If not set, `+Closeup_pos:` is used.
- Syntax: ***Vector***, three **floats**, x, y and z values respectively

#### +Icon_closeup_zoom:

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- The zoom for the weapon's 3D icon in the weapon selection screen. If not set, `+Closeup_zoom:` is used. A value of 0 or less is noted in the log and ignored.
- Syntax: ***Float***, radians

### $Selection Effect:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Defines which selection effect to use when the -weapon_choice_3d commandline flag is enabled or no ani can be found for the selected weapon.
- Syntax: ***String***, either "FS1", or "Off". This defaults to the FS2 effect.


### $FS2 effect grid color:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- The color of the grid in the FS2 selection effect.
- Syntax: ***( Integer, Integer, Integer )***, red, green, blue, 0 to 255

### $FS2 effect scanline color:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- The color of the scan line in the FS2 selection effect.
- Syntax: ***( Integer, Integer, Integer )***, red, green, blue, 0 to 255

### $FS2 effect grid density:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- How many lines the FS2 selection effect's grid has. Must be above 0.
- Syntax: ***Integer***

### $FS2 effect wireframe color:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- The color of the wireframe in the FS2 selection effect.
- Syntax: ***( Integer, Integer, Integer )***, red, green, blue, 0 to 255

### $HUD Image:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the picture used to represent the weapon in HUD loadout list
- Syntax: ***String***, filename

### $Model File:

- Filename of the model file (.pof) at data/models folder. Additional fields beginning from ***@Laser Bitmap:*** and ending to ***@Laser Tail Radius:*** are not needed if a model has been defined for the weapon.
- Standard primaries and beam cannons do not use specific models so for those the name is 'none'.
- Flak weapons and secondaries use models, though the exact model is not important with the flak weapons.
- Syntax: ***String.pof*** or *none*, model filename
  - Example: *none*

### $POF target LOD:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the LOD (Level-Of-Detail) of the model used in HUD targeting view
- Syntax: ***Integer***

### $Detail Distance:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Defines the distance where the change between different Levels-Of-Details (LODs) occurs
- Take notice that these are base values. Model detail in Detail options, (within game press F2), applies a multiplier to these values. These multipliers are (from left to right): 1/8, 1/4, 1, 4, 8
- Optional.  If not defined the game uses the formula ((radius * 20.0 + 20.0) * LOD)
- Syntax: ***(Detail Distances)*** , list of **integers** (same amount as LODs), distances from the model in meters
  - Example: *(0, 80, 300, 900)*

### $External Model File:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the models shown on the gunpoint, used with Bobb's external-visible-weapon code.
- Syntax: ***String.pof***, model filename

### $Submodel Rotation Speed:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the rotation speed of the subobject, with the $gun_rotation property defined, in the external weapon model.
- Syntax: ***Float***
- Note: weapon fire will be delayed by the time it takes for the weapon to spin up to this speed (using $Submodel Rotation Acceleration or its default value), regardless of whether an external model is used or not.

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Note: It's no longer required that the ship actually have external models enabled (whether or not this weapon has one defined) for the spin up delay to occur.

### $Submodel Rotation Acceleration:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the rotation acceleration of the external weapon model.
- Syntax: ***Float***
- Default value: 10

### @Laser

In the event a 3D model is not defined for the weapon, FSO creates an object similar to a glowpoint. Essentially, it is a 2D plane that always faces the player's camera with an additive texture applied to it, and may or may not have an omnilight follow it.

As suggested by its name, this option group is normally used for lasers and similar primaries, but it may also be used for other weapon types as well.

Unless said otherwise, the graphic files used within this group may be of any compatible type (.DDS, .PCX, .TGA, etc.), however a .DDS is recommended.

#### @Laser Bitmap:

Defines the graphic file to be used as the weapon effect.

Blending type: additive

Syntax: ***String***, filename

**Example:**
```
@Laser Bitmap: my_laser_bitmap
```

#### @Laser Head-on Bitmap:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg) Defines another laser graphic file to be used and crossfaded into as the camera angles towards the front or bank of this weapon. See also @Laser Head-on Transition Angle and @Laser Head-on Transition Rate.

Syntax: ***String***, filename

#### @Laser Glow:

Defines the graphic file to be used for the glowmap of the weapon effect.

Blending type: additive

- This file is 'multiplied' by a value between Color and Color2.
- The physical dimensions of the glowmap in-game is 2x the size of the bitmap.

Syntax: ***String***, filename

**Example:**
```
@Laser Glow: my_laser_glowmap
```

#### @Laser Glow Head-on Bitmap:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg) Defines another graphic file to be used for the laser glow and crossfaded into as the camera angles towards the front or bank of this weapon. See also @Laser Head-on Transition Angle and @Laser Head-on Transition Rate.

Syntax: ***String***, filename

#### @Laser Head-on Transition Angle:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg) Overrides the angle in degrees from the front (or back) that the head-on transition crossfade is at exactly a 50/50 blend.

Syntax: ***Float***, degrees
Default: Calculated from the ratio of the laser's length to its average radius

#### @Laser Head-on Transition Rate:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg) Defines how quickly the head-on transition fades from one bitmap to the other. Greater than 20 is an instant switch with no blending.

Syntax: ***Float***
Default: 2

#### @Laser Color:

The RGB color of the glow effect.

- The color is applied both to the glowmap and the omnilight that follows the effect.

Syntax: ***Color***, red, green, blue respectively, 8-bit value from 0 to 255

**Example:**
```
@Laser Color: 255, 0, 0    ; Red glow effect at the start of the effect's lifetime
```

#### @Laser Color2:

The RGB color of the glow effect at the end of the effect's lifetime (at its maximum range).

- The color is applied both to the glowmap and the omnilight that follows the effect.
- The actual color of the glow effect is somewhere in between the values defined by Color and Color2. Essentially, the actual color "fades" from Color to Color2 over its lifetime.
  - If Color is not defined, then a default value of 255, 255, 255 is assumed for its initial color.

Syntax: ***Color***, red, green, blue respectively, 8-bit value from 0 to 255

**Example:**
```
@Laser Color2:  0, 0, 255    ; Blue glow effect at the end of the effect's lifetime.
```

**Example:**
```
@Laser Color: 255, 0,   0    ; Start with a Red glow effect, and fade into...
@Laser Color2:  0, 0, 255    ; a Blue glow effect over the lifetime of the effect.
```

#### @Laser Length:

Defines the length of the weapon effect.
- This option stretches the 2D plane "away" from its origin.
Syntax: ***Float***, meters

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

### @Laser Length Multiplier over Lifetime Curve:

- Specifies the curve to use for changing the laser graphic's length over time.
- The life of the weapon (0-1) is X, the length multiplier is Y.
- Syntax: *String*, the name of the curve from [curves.tbl](curves.tbl)

#### @Laser Head Radius:

Defines the radius of the leading edge, or "head," of the weapon effect.
- The edge of the 2D plane farthest away from its origin is the leading edge.
- The specified radius value is multiplied by 0.5 twice before being applied to the laser, so the laser head's radius will be one-quarter of what's tabled. Its *diameter* will be half of the tabled radius value.
Syntax: ***Float***, meters

#### @Laser Tail Radius:

Defines the radius of the trailing edge, or "tail" of the weapon effect.
- The edge of the 2D plane that is closest to its origin is the trailing edge.
- The specified radius value is multiplied by 0.5 twice before being applied to the laser, so the laser tail's radius will be one-quarter of what's tabled. Its *diameter* will be half of the tabled radius value.
Syntax: ***Float***, meters

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

### @Laser Radius Multiplier over Lifetime Curve:

- Specifies the curve to use for changing the laser graphic's radius values (both head and tail) over time.
- The life of the weapon (0-1) is X, the radius multiplier is Y.
- Syntax: *String*, the name of the curve from [curves.tbl](curves.tbl)

#### @Laser Glow Length Scale:

Scaling factor for glow bitmap along the length axis, relative to the length of the main laser bitmap. Default is 2.0.

Syntax: ***Float***

#### @Laser Glow Head Scale:

Scaling factor for the radius of the glow bitmap at its head, relative to the radius of the main laser bitmap's head. Default is 2.3.
- Note that laser length is calculated in an odd way: the head and tail are treated as *spheres*, with the length being the distance between the centers of those spheres, so the actual length of a laser (or laser glow) is length + head radius + tail radius.
- This can result in unintuitive behavior if glow length scaling is set differently from head or tail scaling.
- However, as long as length, head, and tail glow scale are all set to the same value, the glow bitmap will be scaled up while maintaining the same aspect ratio as the laser.
Syntax: ***Float***

#### @Laser Glow Tail Scale:

Scaling factor for the radius of the glow bitmap at its tail, relative to the radius of the main laser bitmap's tail. Default is 2.3.
- See note in head scale entry regarding unintuitive behavior.
Syntax: ***Float***

#### @Laser Position Offset:

Moves the laser relative to its collision point by multiples of the laser length, along any of the three axes. For example, a value of -1.0 along the Z axis will move the laser backward so that the visible laser no longer extends forward from the collision point, but backward, with the collision point at the head rather than the tail. A value of 1.0 along the X axis will move the laser one laser-length to the right.
- Important note: the X and Y axes are relative to the orientation of the weapon, which normally seems to match the orientation of the firing ship, but which is not guaranteed to remain stable. If the weapon rotates, the orientation of the offset will rotate with it.
Syntax: ***Vector***

#### @Laser Min Pixel Size:

If this is set to a non-zero value, the laser will always be at least the specified number of pixels wide, no matter how far away it is. Defaults to the $Minimum Pixel Size Lasers: value in Game settings.tbl, or to 0 if that is not set.

Syntax: ***Integer***

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

### @Laser Opacity Multiplier over Lifetime Curve:

- Specifies the curve to use for changing the laser graphic's opacity over time.
- The life of the weapon (0-1) is X, the opacity is Y, with 0 being invisible and 1 being typical opacity.
- Syntax: *String*, the name of the curve from [curves.tbl](curves.tbl)

### $Light color:

![FSO 22.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.2.svg)

- Sets the color of light cast by the weapon.
- Syntax: ***Color***, red, green, blue respectively, from 0 to 255
- Default value: If not set, the color is white for weapons with models, or based on the laser colors otherwise

### $Light radius:

![FSO 22.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.2.svg)

- Sets the maximum distance light cast by the weapon will reach. Increasing this will tend to make it appear brighter near it's center
- Syntax: ***float***, any positive value
- Default value: The defaults for different weapon types are set in [lighting_profiles.tbl](lighting_profiles.tbl)

### $Light intensity:

![FSO 22.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.2.svg)

- Acts as a multiplier to the brightness of the light cast by the weapon. This changes the brightness without affecting the radius.
- Syntax: ***float***, any positive value
- Default value: 1

### $Collision Radius Override:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Weapon - weapon collisions are treated as two spheres, where a laser's sphere's radius is equal to its Head Radius, and a missile's is the radius of its associated model (possibly doubled). Using this will override either of those values.
- If used on a beam it will override its collision radius as well, instead of using the widest section.
- NOTE: If the weapon is interceptable, such as a bomb or has been given hitpoints, this value will *still* be doubled, unless the "no radius doubling" flag is used.
- Syntax: ***Float***, meters

### $Mass:

- Scales the kinetic effect this weapon has on its target. With higher mass imparting a stronger effect.
- The eventual kinetic effect is also multiplied by the speed. ie *$Mass: 3* and *$Velocity: 200* has much weaker kinetic effect than *$Mass: 3* and *$Velocity: 1000*. This is also true for the intesnity of the "shudder" effect, if flagged.
- Syntax: ***Float***, blast effect

### $Velocity:

- How fast the weapon travels. Is also used to define the weapon range, ***$Velocity x $Lifetime = range***.
- Beams travel instantly so this field has effect only on weapon range and AI beam targeting with certain beam types. $Velocity of beam also applies to its kinetic effect along with $Mass.
- Syntax: ***Float***, meters per second

### $Fire Wait:

- Time in seconds between firings
- Note that Fire wait will be reset to 1 if it is set over 5 for non turreted primary weapons (doesn't apply to 3.6.14+)
- Syntax: ***Float***, seconds

#### +Max Delay:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Defines the maximum amount of time it takes to refire a weapon in a random fire delay range.
  - This must be used with +Min Delay: to have any effect.
- Syntax: ***Float***, seconds

#### +Min Delay:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Defines the minimum amount of time it takes to refire a weapon in a random fire delay range.
  - This must be used with +Max Delay: to have any effect.
- Syntax: ***Float***, seconds

### $Damage:

- Base damage this weapon does. The actual damage is this number multiplied by the factors below.
- Base damage this beam does is continuous damage, damage is approximately 5.88 times the damage value per second. The actual damage is this number multiplied by one of the factors below.
- The exact factor to multiply by is 100/17, or one 'pulse' of damage every 170 seconds. The generally used wisdom of 5.5 times may have been generally accurate before beam damage was made continuous and frame-rate independent, but is now out of date information.
- Syntax: ***Float***, damage
- Note: adding an inner and/or outer radius to a weapon doubles its effective damage to the target (i.e. 1x impact, 1x area effect)

### $Damage Time:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- This sets a time from the end of the weapons lifetime that damage will start to attenuate at. ***Example: Weapon lifetime is 2.0 seconds and the desired effect is to attenuate after 0.5 seconds have passed, set this to 1.5 seconds.***
- Syntax: ***Float***, seconds. Defaults to 0.0.
  - Note: If neither +Min Damage: nor +Max Damage: is set, the damage will always attenuate to zero at max range.
    - If both +Min Damage: and +Max Damage: are set, both are defaulted to 0.0 and damage will be attenuated to zero.

#### +Attenuation Damage:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- This sets the damage a weapon will attenuate to after reaching $Damage Time:.
- Syntax: ***Float***, damage.

#### +Min Damage:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- This sets the lowest amount of damage possible assuming damage attenuation is taking place. ***This has been deprecated, please use +Attenuation Damage: instead.***
- Syntax: ***Float***, damage. Defaults to 0.0.

#### +Max Damage:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- This sets the highest amount of damage possible assuming damage attenuation is taking place. ***This has been deprecated, please use +Attenuation Damage: instead.***
- Syntax: ***Float***, damage. Defaults to 0.0.

### $Angle of Incidence Damage Multiplier:

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

- Causes the weapon's damage to be multiplied by these factors depending on the angle of incidence.
- It is *not* linear, it is based on the sine of the angle to the surface. If the factors are 1x for minimum, and 2x for maximum, at an angle of 45 degrees the multiplier is 1.707x, for instance.

#### +Min:

- The damage multiplier at a perfectly glancing hit, parallel to the surface (although not technically possible).
- Syntax: ***Float***

#### +Max:

- The damage multiplier when hit straight on, perpendicular to the surface.
- Syntax: ***Float***

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

### $Damage Multiplier over Lifetime Curve:

- Specifies the curve to use for changing the weapon's damage over time.
- The life of the weapon (0-1) is X, the damage multiplier is Y.
- Syntax: *String*, the name of the curve from [curves.tbl](curves.tbl)

### $Damage Type:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg) Defines the damage type that this weapon uses. All armor effects are applied after shield and have no function against shockwave or subsystem damage.

**REQUIRES:** [armor.tbl](armor.tbl)

- Syntax: ***String***, name of the damage type ( as defined by [$Damage Type](armor.tbl#damage-type) )

### $Arm time:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the time after the launch that is required before the weapon comes hot
- If the time has not been reached when the weapon detonates the Dinky explosion is used
- Syntax: ***Float***, seconds

### $Arm distance:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the distance from the firing ships that is required before the weapon comes hot
- If the distance has not been reached when the weapon detonates the Dinky explosion is used
- Syntax: ***Float***, meters

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- This parameter is also applied to flak weapons. Previous behaviour was to arm weapons after 10 meters of travel.

### $Arm radius:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the distance from the target where the weapon will arm itself
- Syntax: ***Float***, meters

### $Detonation Range:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the distance from the point in space that the weapon was launched where the weapon will detonate
- Syntax: ***Float***, meters

### $Detonation Radius:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the distance between the target center point point and the weapon which detonates the weapon.
- Syntax: ***Float***, meters


### $Proximity Radius:

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- Makes the weapon detonate when a suitable object comes within this distance. Any weapon (mine, flak, missile and so on) can use it; the filters below decide what counts as suitable.
- Must be above 0; anything else logs a warning and 1 is used.
- Syntax: ***Float***, meters

#### +Proximity IFF:

- Optional. Only objects of these IFFs set it off. An unknown IFF logs a warning.
- Syntax: ***( "String" "String" )***, IFF names

#### +Proximity Species:

- Optional. Only ships of these species set it off. An unknown species logs a warning.
- Syntax: ***( "String" "String" )***, species names

#### +Proximity Type:

- Optional. Only ships of these ship types (from [objecttypes.tbl](objecttypes.tbl)) set it off.
- Syntax: ***( "String" "String" )***, ship type names

#### +Proximity Class:

- Optional. Only ships of these classes set it off.
- Syntax: ***( "String" "String" )***, ship class names

#### +Proximity Relation:

- Optional. Only objects with this relation to the firing ship set it off: any of **hostile**, **friendly**, **neutral**. Default: any relation. Anything else logs a warning.
- Syntax: ***( "String" "String" )***

#### +Detonate Chance:

- Optional. The chance the weapon actually detonates when something comes in range. Values outside 0 to 1 log a warning and are clamped.
- Syntax: ***Float***, 0 to 1

#### +Stealth Proximity Multiplier:

- Optional. Multiplies the proximity radius for stealth ships. Negative values log a warning and 0 is used.
- Syntax: ***Float***

A modular table that sets any of these lists replaces the whole list.

### $MineInfo:

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- Makes the weapon a mine (the same as the `"mine"` flag). A mine without `$Proximity Radius:` uses 50.
- No value; the fields below are optional.

#### +Sensors Range:

- How far away the mine shows up on sensors. Default: 60 times the proximity radius. Negative values log a warning and 0 is used. It is raised (with a warning) if it is smaller than +Targetable Range:.
- Syntax: ***Float***, meters

#### +Targetable Range:

- How far away the mine can be targeted. Default: 30 times the proximity radius. Negative values log a warning and 0 is used.
- Syntax: ***Float***, meters

#### +Chase Duration:

- How long the mine chases a target once triggered. 0 means it detonates at once. Negative values log a warning and 0 is used.
- Syntax: ***Float***, seconds

#### +Detonates on Chase Timeout:

- Whether the mine detonates when the chase time runs out.
- Syntax: ***Boolean***

#### +Chase Cooldown:

- How long the mine waits before it can be triggered again. Negative values log a warning and 0 is used.
- Syntax: ***Float***, seconds

### $Flak Detonation Accuracy:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- Normally, flak weapons detonate at a range of 65 to 99 meters from their intended target, depending on the firing ships' weapon subsystem health. This parameter can be used to adjust this behaviour, as it defines the base range used to determine this detonation range. Standard value is 65.
- Syntax: ***Float***, range in meters

### $Flak Targeting Accuracy:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- Turret-fired flak weapons are normally firing in a cone around the target's predicted position. This parameter can be used to adjust the size of this cones' base radius. Standard is 60 meters.
- Syntax: ***Float***, radius in meters

### $Untargeted Flak Range Penalty:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- If a flak weapon is fired without a target selected, this parameter defines the range penalty applied to that weapon. Standard is 20 meters, meaning an untargeted flak weapon will detonate after traveling its max range - 20 meters.
- Syntax: ***Float***, penalty in meters.

### $Shockwave damage:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Override for the damage caused by shockwaves (default is [#$Damage:](https://wiki.hard-light.net/index.php/#$Damage:))
- Syntax: ***Float***, damage
- Note: adding a shockwave to a weapon doubles its effective damage to the target (i.e. 1x impact, 1x shockwave)

### $Shockwave Damage Type:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **REQUIRES ADDITIONAL TABLE**. Defines the damage type used for the weapon explosions
- Syntax: ***String***, name as defined in [armor.tbl](armor.tbl)

### $Blast Force:

- The intensity of the kinetic blast effect when ship is within the outer radius of the explosion. Comes in effect only if inner and outer radius are defined.
- Syntax: ***Float***, blast effect

### $Inner Radius:

- Radius at which the full explosion damage is done. Marks the line where damage attenuation begins. Damage is reduced by the square of the distance between the inner radius and outer radius.  i.e. half way between inner and outer results in 1/4 damage
- Note: adding an inner and/or outer radius to a weapon doubles its effective damage to the target (i.e. 1x impact, 1x area effect)
- Syntax: ***Float***, meters

### $Outer Radius:

- Maximum radius at which any damage is done.  See $Inner Radius for the attenuation formula.
- Also defines the size of the shockwave (if shockwave speed is > 0)
- Note: adding an inner and/or outer radius to a weapon doubles its effective damage to the target (i.e. 1x impact, 1x area effect)
- Syntax: ***Float***, meters

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

### $Shockwave Radius Multiplier over Lifetime Curve:

- Specifies the curve to use for changing the shockwave's radius over time. This will *ignore* $Shockwave Speed for the purposes of determining the size of the shockwave over time, however $Outer radius / $Shockwave Speed will still be the total time that the shockwave lasts. Even if the multiplier goes beyond 1, the shockwave still cannot hit or damage targets beyond its $Outer radius.
- The life of the shockwave (0-1) is X, the outer radius multiplier is Y. This will entirely determine how the shockwave grows and shrinks over time.
- Syntax: *String*, the name of the curve from [curves.tbl](curves.tbl)

### $Shockwave Speed:

- Speed shockwave expands at. Zero means no shockwave and that any area-effect damage is applied immediately
- Syntax: ***Float***, meters per second

### $Shockwave rotation:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines how the 3D shockwave is rotated. Random, if left undefined.
- Syntax: ***Angle***, three values, pitch, bank, heading respectively, in degrees

### $Shockwave model:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the model file (.pof) used as a shockwave for the 3D shockwaves.
- Syntax: ***String.pof***, model filename

### $Shockwave name:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the name of the animation used for the 2D shockwave
- Syntax: ***String***, filename

### $Shockwave Sound:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Optional sound to be used for the sound heard when for the shockwave (default is sound 109).
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $Dinky shockwave:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Dinky shockwave are used if the weapon is shot down before arming or if the conditions marked at **Arm time** and **Arm distance** are not reached before impact
- It uses exactly same entries as the standard shockwaves
- ***+Shockwave damage:***
- ***+Shockwave damage type:***
- ***+Blast Force:***
- ***+Inner Radius:***
- ***+Outer Radius:***
- ***+Shockwave Speed:***
- ***+Shockwave Rotation:***
- ***+Shockwave Model:***
- ***+Shockwave Name:***

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- ***+Shockwave sound:***

### $Armor Factor:

- Multiplier for the damage done to armor
- Syntax: ***Float***, damage multiplier

### $Shield Factor:

- Multiplier for the damage done to shields
- Syntax: ***Float***, damage multiplier

### $Subsystem Factor:

- Multiplier for the damage done to subsystems
- Syntax: ***Float***, damage multiplier

### $Lifetime Min:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the minimum lifetime of the weapon
- Syntax: ***Float***, seconds

### $Lifetime Max:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the maximum lifetime of the weapon
- Syntax: ***Float***, seconds

### $Lifetime:

- Time before the standard primaries fizzle out and secondaries self destruct. Multiply this by the speed to get the range. If values are too low (below framerate) then the collision detection and graphic effects may suffer.
- This field has no effect for the beams apart from weapon range and AI beam targeting with certain beam types
- Syntax: ***Float***, seconds

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

### $Launch Curve:

- A set of [modular curve](https://wiki.hard-light.net/index.php/modular_curve)s that controls the firing behavior of the weapon. These curves will have no effect on beams.
- Available inputs:
  - **Num Firepoints**: the number of barrels in the bank firing the weapon
  - **Distance to Target**
  - **Target Radius**
- Available outputs:
  - **Fire Wait Mult**
  - **Shots Mult**
  - **Burst Shots Mult**
  - **Burst Delay Mult**
  - **Velocity Mult**
  - **Life Mult**
  - **Failure Rate Mult**
  - **FoF Mult**

### $Lifetime Curve:

- A set of [modular curve](https://wiki.hard-light.net/index.php/modular_curve)s, dealing with changes to the weapon during its existence. These curves will have no effect on beams.
- Available inputs:
  - **Lifetime**: starts at 0.0 when the weapon is created, ends at 1.0 when it runs out of life
  - **Age**: how long the weapon has existed, in seconds
  - **Base Velocity**: the weapon's velocity. This accounts for launch curves, but does NOT account for other lifetime curves, so as to avoid a feedback loop.
  - **Base Damage**: the weapon's damage. This accounts for launch curves, but does NOT account for hit curves, so as to avoid a feedback loop.
  - **Max Hitpoints**
  - **Current Hitpoints**
  - **Hitpoints Fraction**: the weapon's current hitpoints divided by its maximum hitpoints
  - **Parent Radius**: radius of the ship ultimately responsible for firing the weapon
  - **Viewing Angle**: direction from which the player is looking at the weapon. -1.0 means directly head-on, 0.0 means directly from the side, and 1.0 means directly from the rear.
  - **Apparent Size**: the weapon's radius as a fraction of the screen height
- Available outputs:
  - **Laser Length Mult**
  - **Laser Radius Mult**
  - **Laser Head Radius Mult**
  - **Laser Tail Radius Mult**
  - **Laser Offset X Mult**
  - **Laser Offset Y Mult**
  - **Laser Offset Z Mult**
  - **Laser Headon Transition Angle Mult**
  - **Laser Headon Transition Rate Mult**
  - **Laser Animation State**: sets the animation state of the laser, with 0.0 being the first frame of the animation, and 1.0 being the last. If this is used, all typical animation behavior (i.e. playing at a certain framerate over time) will be ignored.
  - **Laser Animation State Add**: adds a certain number of frames to the laser's animation state. Does not disable typical animation behavior on its own.
  - **Laser Alpha Mult**
  - **Laser Bitmap R Mult**
  - **Laser Bitmap G Mult**
  - **Laser Bitmap B Mult**
  - **Laser Glow R Mult**
  - **Laser Glow G Mult**
  - **Laser Glow B Mult**
  - **Light Intensity Mult**
  - **Light Radius Mult**
  - **Light R Mult**
  - **Light G Mult**
  - **Light B Mult**
  - **Detonation Radius Mult**
  - **Turn Rate Mult**
  - **Child Spawn Rate Mult**
  - **Child Spawn Count Mult**

### $Hit Curve:

- A set of [modular curve](https://wiki.hard-light.net/index.php/modular_curve)s, controlling how the weapon affects things it hits. These curves will have no effect on beams.
- Available inputs:
  - **All of the Lifetime Curve inputs, as well as...**
  - **Target Hitpoints**
  - **Target Radius**
  - **Dot**: the angle at which the weapon has struck the target surface. 1.0 means a direct hit, perpendicular to the target surface. 0.0 would mean a hit exactly parallel to the surface.
- Available outputs:
  - **Damage Mult**
  - **Hull Damage Mult**
  - **Shield Damage Mult**
  - **Subsystem Damage Mult**
  - **Mass Mult**

### $Beam Curve:

- A set of [modular curve](https://wiki.hard-light.net/index.php/modular_curve)s dealing with beam attributes. These curves only affect beams.
- Available inputs:
  - **Beam Lifetime**: the beam's lifetime, from 0.0 to 1.0, not including the warmup or warmdown phases
  - **Beam Age**: in seconds
  - **Beam Total Life**: how long the beam is capable of lasting, not including warmup or warmdown
  - **Warmup Lifetime**: lifetime of the warmup phase, starting at 0.0 and ending at 1.0 when the beam fires
  - **Warmdown Lifetime**
  - **Warmdown Age**: time elapsed since warmdown started, in seconds
  - **Parent Radius**
- Available outputs:
  - **Beam Width Mult**: does NOT disable tabled flickering or shrinking if set
  - **Beam Alpha Mult**
  - **Beam Anim State**: behaves like laser anim state
  - **Muzzle Glow Radius Mult**: if this is set, the typical hardcoded muzzle glow size behaviors are disabled
  - **Muzzle Glow Alpha Mult**: if this is set, the typical hardcoded muzzle glow fading behaviors are disabled
  - **Muzzle Glow Anim State**: behaves like laser anim state
  - **Move End Position by Target Velocity**: if the beam is slashing at a target, the beam's end position will be offset by the target's velocity (at time of beam creation) times this output value

### $Beam Hit Curve:

- A set of [modular curve](https://wiki.hard-light.net/index.php/modular_curve)s dealing with beam attributes. These curves only affect beams.
- Available inputs:
  - **All of the Beam Curve inputs, as well as...**
  - **Target Hitpoints**
  - **Target Radius**
- Available outputs:
  - **Damage Mult**
  - **Hull Damage Mult**
  - **Shield Damage Mult**
  - **Subsystem Damage Mult**
  - **Mass Mult**

### $Energy Consumed:

- The amount of energy each bolt drains firing ship. Has no effect on turreted weapons.
- Syntax: ***Float***

### $Cargo Size:

- Cargo size, is not used with non-ballistic primaries.
- Amount of missiles available = Bank capacity / Cargo Size.
- Syntax: ***Float***

### $Autoaim FOV:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) **This option is exclusive to primaries.**
- Defines if the weapon will have auto-aim and what the FOV of the auto-aim is. Values above 180 log a warning and 180 is used.
- Syntax: ***Float***, degrees

#### +Converging Autoaim

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- Optional. The autoaim also converges the guns on the target.

#### +Minimum Distance:

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- Optional. The minimum convergence distance for the autoaim.
- Syntax: ***Float***, meters

### $Convergence:

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- Gives this weapon its own gun convergence, instead of the ship's.

#### +Automatic

- Optional. Converges on the target automatically. Can be followed by `+Minimum Distance:` (***Float***, meters).

#### +Standard

- Optional. Converges at a fixed distance, given by the required `+Distance:` (***Float***, meters).

### $Homing:

**This option is exclusive to secondaries.**

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg) **This option may now be applied to primaries**. They may only use heat seeking. For optimal results the "no homing speed ramp" flag may be used.

Defines if the bolts/missiles in question is homing

If set to NO, then following fields are not required in the table.

- Syntax: ***Boolean***, yes or no

#### **+Type:**

Defines the homing method
- Syntax: ***String***, *ASPECT* or *HEAT*

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg) An additional option, *JAVELIN*, has been added.  This behaves like an aspect missile but automatically only locks onto the engines of a target, and only if the engines are in sight.

#### **+Turn Time:**

Determines how quickly the missile can turn. Ostensibly the number of seconds it takes the missile to do a 360 degree circle at full turn speed, but for complicated reasons this is only true if [AI respect tabled turn time and rotdamp](game_settings.tbl#ai-respect-tabled-turn-time-and-rotdamp) is enabled.
  - Syntax: ***Float***, seconds

#### **+Turning Acceleration Time:**

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Determines how quickly the missile reaches its maximum turn speed.
- Note: Requires [$AI use framerate independent turning](game_settings.tbl#ai-use-framerate-independent-turning) in game_settings.tbl
  - Syntax: ***Float***, seconds

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

### +Turn Rate Multiplier over Lifetime Curve:

- Specifies the curve to use for changing the weapon's turn rate over time.
- The life of the weapon (0-1) is X, the turn rate multiplier is Y.
- Syntax: *String*, the name of the curve from [curves.tbl](curves.tbl)

#### **+View Cone:**

**Heat-seeking missiles only**

This is the width of the weapon's viewing cone, given in degrees. Objects outside this cone are effectively invisible to the missile. The value / 2 gives the angle from the missile's centerline to the edge of the viewing cone.
- Syntax: ***Float***, degrees

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg) Has been enabled also to aspect seekers as an option. They will continue to home on targets outside their view cone, but will take a continuous lifetime penalty every second their target is outside this cone.

#### **+Min Lock Time:**

**For aspect/javelin missiles only**

The minimum number of seconds to gain a target lock.
- Syntax: ***Float***, seconds

#### **+Lock Pixels/Sec:**

**For aspect/javelin missiles only**
The number of pixels the aspect triangle moves per second while attempting lock-on.

- Syntax: ***Integer***, pixels per second

#### **+Catch-up Pixels/Sec:**

**For aspect/javelin missiles only**

The number of pixels moved per second while attempting to catch up to the target.
- Syntax: ***Integer***, pixels per second

#### **+Catch-up Penalty:**

**For aspect/javelin missiles only**

Extra pixels to move after catching up. (Needs verification )
  - Syntax: ***Integer***, pixels

#### **+Seeker Strength:**

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg) Defines the seeking ability of the homing system. Default of heat seekers is 3.0 and for aspect seekers 2.0. Spoofing chance is defined as **[cm type effectiveness](#countermeasure)**/**seeker strength** (missile must be within the countermeasures **[+Effective Radius:](#countermeasure)**
- Syntax: ***Float***, seeker strength

#### **+Lock FOV:**

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) Defines the width of the cone within which the missile is capable of achieving a lock, in degrees. Only applies to aspect seekers.
- Syntax: ***Float***, default (approximately): 63.57666

#### **+Target Lead Scaler:**

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg) Defines the type of pursuit course the missile tries to obtain.
- Values < 0 give lag pursuit, values ~ 0 give pure pursuit, values > 0 give lead pursuit.
- Syntax: ***Float***, default for heat seekers: 0, default for aspect or javelin seekers: 1.

#### **+Target Lock Restriction:**

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Defines if the missile can lock onto ships besides the current target
- Syntax: ***String***, name of the behavior, default is *current target only*;.
- *Current target only* As normal, can only acquire locks on the targeted ship
- *Any target* Missile will lock on any valid target. Missile locking is still disabled in certain cases, such as when a friendly is the player's target
- *Current target, any subsystem* Targeting is restricted to the current ship, but will pick turrets and subsystems on that ship.

#### **+Independent Seekers:**

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Defines the ability for a missile to target multiple objects at once.
- Syntax: ***Boolean***, yes or no, default is no.

#### **+Target Lock Objecttypes:**

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg) Defines what type of object can be locked on with this weapon
- Syntax: ***String***, name of the object type, default is *ships*
- *ships* Will target only ships
- *bombs* Will target bombs and targetable missiles

#### **+Trigger Hold:**

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Defines the ability to hold the trigger to lock and release to fire.
- Syntax: ***Boolean***, yes or no, default is no.

#### **+Reset On Launch:**

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Defines the ability for locks to reset upon missiles being fired.
- Syntax: ***Boolean***, yes or no, default is no.

#### **+Max Seekers Per Target:**

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Defines the maximum number of seekers that can target a single object.
- Syntax: ***Integer***, number of seekers.

#### **+Max Active Seekers:**

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Defines the maximum number of total seekers for the weapon to have.
- Syntax: ***Integer***, number of seekers.

#### **+Ship Types:**

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Defines the ship types that this missile can target. In order for a ship to be targetable it must be able to match one of the listed types, if some of the below lists are also specified, it must match into those as well.
- Syntax: **("String" "String")**, ship types.

#### **+Ship Classes:**

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Defines the ship classes that this missile can target. In order for a ship to be targetable it must be able to match one of the listed classes, if some of the above/below lists are also specified, it must match into those as well.
- Syntax: **("String" "String")**, ship classes.

#### **+Species:**

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Defines the species whose ships this missile can target. In order for a ship to be targetable it must be able to match one of the listed Species, if some of the above/below lists are also specified, it must match into those as well.
- Syntax: **("String" "String")**, species.

#### **+IFFs:**

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Defines the IFF's whose ships this missile can target. In order for a ship to be targetable it must be able to match one of the listed IFFs, if some of the above lists are also specified, it must match into those as well.
- Syntax: **("String" "String")**, IFFs.

### $Homing Auto-Target Method:

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- If this weapon needs to acquire a target on its own, such as a heatseeker losing its target or having the "untargeted heatseeker" flag, this determines how it should do so. Options are:
- **CLOSEST**, the weapon will acquire the nearest target that it is able.
- **RANDOM**, the weapon will acquire a random target from those available.

### $Swarm:

- The number of missiles to fire per volley if it's a swarm missile
- Can be issued to Standard primaries also. Causes single part turrets to malfunction but causes multi part turrets to fire only on their barrel line. Can be used for 'burst fire weapons'. Does not work with player weapons.
- Is incompatible with "Corkscrew" option.
- Swarm will negatively affect the missile's ability to hit its target and will not lead its target even with  [Target Lead Scaler](#target-lead-scaler) defined unless [$Swarmers Lead Targets](game_settings.tbl#swarmers-lead-targets) is enabled.
- Syntax: ***Integer***, number of shots in a swarm

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **+SwarmWait:**
  - Defines the time between weapons fired in a swarm
  - Syntax: ***Float***, seconds
    - Default: 0.150)

### $Acceleration Time:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Defines how long it takes for a secondary weapon to attain full speed. Takes precedence over $Free Flight Time (see below) in terms of the weapon's velocity, and affects both locked-on and dumbfired secondaries.
- Syntax: ***Float***, seconds
  - Default: 0.0

### $Velocity Inherit:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Defines how much of the parent ship's velocity to inherit, if [Ai_profiles.tbl#.24use_additive_weapon_velocity:](https://wiki.hard-light.net/index.php/Ai_profiles.tbl#.24use_additive_weapon_velocity:) is used.
- Syntax: ***Float***, percents
  - Default: 100.0

### $Free Flight Time:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the free flight time of homing weapons. During this time weapon flies at 1/4 of its velocity + firing ships velocity in a straight line.
- Syntax: ***Float***, seconds
  - Default: 0.25

**Note:** due to dumbfires never changing their speed, using this on a dumbfire missile will have it stuck at freeflight speed even after the end of the freeflight time.

### $Free Flight Speed:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Defines the speed that the weapon will be traveling at during the free flight segment (see above).
- Syntax: ***Float***, Multiplier for the actual speed, minimum 0.01.
  - Default: 0.25

> **Note:** This feature was never implemented correctly and was removed in r8867. FSO parses but doesn't use the value, and prints a warning in the log. This feature has subsequently been reimplemented, see $Free Flight Speed Factor: below

### $Free Flight Speed Factor:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Defines the speed that the weapon will be traveling at during the free flight segment (see above).
- Syntax: ***Float***, Multiplier for the actual speed, minimum 0.01.
  - Default: 0.25

### $Gravity Const:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- If the mission has gravity, this is the multiplier applied to acceleration from gravity on the weapon.
- AI will lead for ballistic trajectories, and this may affect their range as they will not fire unless they can successfully make a ballistic path to their target.
- **Homing weapons are unaffected by gravity no matter what**.
- Syntax: ***Float***
  - Default: 0

### $PreLaunchSnd:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Defines the one-shot sound effect to play at the beginning of a firing stream. Currently only has an effect on primary weapons.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)
  - Default: No Sound

### +PreLaunchSnd Min Interval:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Defines  Minimum interval between when the player stops firing and the next time the pre-launch sound can play, as a limiter in case the player is pumping the trigger.
- Syntax: ***Integer***, Minimum delay in milliseconds between the end of the previous firing sequence and the next time the pre-launch sound effect will be played.
  - Default: 0 (play pre-launch sound at the beginning of every firing sequence, if present)

### $LaunchSnd:

- Corresponding sound effect when the weapon is launched.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $CockpitLaunchSnd:

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- Corresponding sound effect when the weapon is launched from the player's ship, allowing use of 2d stereo sounds.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $ImpactSnd:

- Sound the weapon makes when it hits a hull.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $Disarmed ImpactSnd:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Sound the weapon makes when it hits a hull before being armed.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)


### $Shield ImpactSnd:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- The sound played when the weapon hits a shield, instead of the impact sound.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $FlyBySnd:

- The sound this weapon makes if it narrowly misses you. -1 is no sound.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl), usually -1

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

### $AmbientSnd:

- The sound this weapon passively emits in 3d space. The weapon version of [EngineSnd](ships.tbl#enginesnd) for ships.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

### $StartFiringSnd:

- The sound that plays when the weapon initially starts firing.
- Does not work for turrets.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

### $LoopFiringSnd:

- The sound that plays while the weapon is firing.
- Does not work for turrets.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)
  - Note: This must be a 3d sound.

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)


### $LinkedLoopFiringSnd:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- The looping firing sound used instead of `$LoopFiringSnd:` when the weapon is fired linked.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $EndFiringSnd:

- The sound that plays when the weapon stops firing.
- Does not work for turrets.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $TrackingSnd:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- The looping sound played when the player is tracking a target with this weapon.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $LockedSnd:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- The sound played when the player has locked onto their target with this weapon.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $InFlightSnd:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- The looping sound played while the player has this weapon currently flying.
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)
- **+Inflight sound type:**
  - Specifies when the in flight sound is played.
  - The value can be one of the following values which are matched case-insensitively:
    - ***'TARGETED***': The sound is played when the missile tracks a target
    - ***'UNTARGETED***': The sound is played when the missile is currently not tracking a target
    - ***'ALWAYS***': The sound is played always
  - Anything else logs a warning.

### $Model:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines used model file. Included for compatibility with countermeasures as they have been merged with weapons in the code.
- Syntax: ***String.pof***, model filename

### $Rearm Rate:

- Defines the frequency that weapons are loaded to the ship
- Syntax: ***Float***, How many times per second the weapons are loaded.
  - Example: 2.0 will reload every half second.  0.1 will load every ten seconds.

### $Rearm Ammo Increment:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Defines how many missiles or how much ballistic ammo is loaded per load sound.
- Syntax: ***Integer***, how many bullets or missiles are loaded. Default 4 for missiles, 100 for ballistic primaries.


### $Disallow Support Rearm:

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- If YES, support ships don't rearm this weapon.
- Syntax: ***Boolean***

### +Weapon Range:

- If set, the AI will use this value or the result of weapon lifetime * velocity as the weapon's effective range, whichever is lower. This also affects the [Lead Indicator](hud_gauges.tbl#lead-indicator)
- Syntax: ***Float***, meters

### +Weapon Min Range:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the minimum range to the target where the weapons under AI will still fire.
- Implemented before FS_Open 3.6.5
- Syntax: ***Float***, meters

### +Weapon Optimum Range:

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- AI fightercraft will attempt to close the distance to their target while they are outside of this range and using this weapon as their currently equipped primary.
- Syntax: ***Float***, meters

### $Pierce Objects:

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

- Causes the weapon to penetrate the objects impacted and not be destroyed.
- Syntax: ***Boolean***

#### +Spawn Children On Pierce:

- Will spawn the weapons children on each hit, not just when it eventually gets destroyed through lifetime
- Syntax: ***Boolean***

### $Flags:

- See [below](#weaponstbl-flags)
- Syntax: ***( "String" "String" )***, names of the flags assigned to the weapon
  - Example: ( "Player allowed" "in tech database" )

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Can be followed by **+override** option in order to reset the weapon flags before parsing in the new ones
- Using this option will erase all **homing**-related flags as well as **swarm**, **variable lead**, **trail**, **TAG**, and **transparent** settings in addition to the listed flags. All these should be set again - if they are required - when this option is used.

  - Syntax: ***+override***

### $Trail:

- This is used if you want your weapon to have a trail. Trail is often used only with secondaries though it works with standard primaries as well.
- **+Start Width:**
  - How wide the head of the trail is.
  - Syntax: ***Float***, meters
- **+End Width:**
  - How wide the tail of the trail is.
  - Syntax: ***Float***, meters
- **+Start Alpha:**
  - The opacity of the trail head.
  - Syntax: ***Float***, value between 0 and 1
- **+End Alpha:**
  - The opacity of the trail tail.
  - Syntax: ***Float***, value between 0 and 1

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

- **+Alpha Decay Exponent:**
  - Causes the trail to fade away non-linearly. The 0 -> 1 progress through the trail's life is taken to the power of the provided number when being used for trail alpha purposes
  - Larger numbers above 1 cause the trail to fade away very slowly at first, and then suddenly at the end. Smaller numbers below 1 cause it to fade away quickly at first, and then slow down for most of its life.
  - Syntax: ***Float***, positive

- **+Max Life:**
  - How long the trail lasts.
  - Syntax: ***Float***, seconds

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- **+Spread:**
  - Causes the trail points to diffuse and drift apart at this speed over time.
  - Syntax: ***Float***, meters per second

- **+Bitmap:**
  - The graphic file to use for the trail.
  - Syntax: ***String***, filename

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

- **+Bitmap Stretch:**
  - Multiplies how far the bitmap is stretched over each trail segment.
  - Syntax: ***Float***

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- **+Faded Out Sections:**
  - Defines the number of the trail sections (counting from the leading edge) which are gradually faded out.
  - Syntax: ***Integer***, number of trail sections

### $Icon:

- Loadout icon used in the weapons selection screen.
- Syntax: ***String***, filename

### $Anim:

- Green grid animation used on the weapons screen.
- Syntax: ***String***, filename

### $Impact Effect:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Specifies the [particle effect](Particle-Effects) which will be used for impacts of this weapon.
- Syntax: **String**, the particle effect name
- If this option is specified then *$Impact Explosion* and *$Impact Explosion Radius* are not valid options!
- If used for beams, this will create an impact every 170ms or every frame with a 1/4th chance.

### $Impact Explosion:

- Impact explosion used. Must be defined in weapon_expl.tbl,

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

- A weapon_expl.tbl entry is no longer required.

- Syntax: ***String***, filename or *none*

### $Impact Explosion Radius:

- Radius of the defined impact explosion.
- Syntax: ***Float***, meters, default 1.0

### $Shield Impact Explosion Radius:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Radius of the impact explosion if it hits the shield, see [Ships.tbl#.24Shield_Impact_Explosion:](https://wiki.hard-light.net/index.php/Ships.tbl#.24Shield_Impact_Explosion:).
- Syntax: ***Float***, meters, defaults to value of $Impact Explosion Radius

### $Shield Impact Effect Radius:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Radius of the visual effect displayed on the shield mesh when this weapon hits the shield, see also [Ships.tbl#.2BMax Shield Impact Effect Radius:](https://wiki.hard-light.net/index.php/Ships.tbl#.2BMax_Shield_Impact_Effect_Radius:).
- Syntax: ***Float***, meters.
- Default: Half of the target's length width or depth, whichever is smallest.

### $Dinky Impact Effect:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Specifies the [particle effect](Particle-Effects) which will be used for Dinky (not armed) explosions of this weapon.
- Syntax: **String**, the particle effect name
- If this option is specified then *$Dinky Impact Explosion* and *$Dinky Impact Explosion Radius* are not valid options!

### $Dinky Impact Explosion:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Same as Impact Explosion but for Dinky (not armed) explosions
- Syntax: ***String***, filename or *none*

### $Dinky Impact Explosion Radius:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Same as Impact Explosion Radius but for Dinky explosions
- Syntax: ***Float***, meters

### $Piercing Impact Effect:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Specifies the [particle effect](Particle-Effects) which will be used for piercing impacts.
- Syntax: **String**, the particle effect name
- If this option is specified then piercing impact options below are not valid!

### $Piercing Impact Explosion:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the effect used for events when weapon pierces the target
- Syntax: ***String***, filename or *none*

### $Piercing Impact Radius:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the base radius of the effect used for piercing weapon impacts
- Syntax: ***Float***

### $Piercing Impact Velocity:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the base velocity for the particles piercing the target
- Syntax: ***Float***

### $Piercing Impact Splash Velocity:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the base velocity for the particles splashing from the target (secondary piercing effects
- Negative value causes the second batch or particles to splash to opposite direction
- Syntax: ***Float***

### $Piercing Impact Variance:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the variance added to the particles
- Values near 0 cause very little variance and values close to 1 cause extreme dispersal.
- Syntax: ***Float***

### $Piercing Impact Life:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the lifetime of the effect for single frame effects
- For multiple frame effects the lifetime is the lifetime of the animation used
- Syntax: ***Float***

### $Piercing Impact Particles:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the number of particles released per impact
- Syntax: ***Integer***

### $Piercing Impact Draw Modifier:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the multiplier applied to the ships piercing effect limit percentage
- Syntax: ***Float***

- Option removed in 3.6.12. Use [armor.tbl](armor.tbl) to finetune piercing effects.

### $Conditional Impact:

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- Defines an impact effect that will play for this weapon only under certain conditions.
- May be defined any number of times.
- Currently only works for targets that are ships or weapons. Impacts against weapons are not considered to have a hit angle.
- All conditional impacts whose requirements are met will play.
- Non-conditional impact effects, the ones defined above, will only play if no conditional impacts' requirements are met.
- **+Armor Type:**, or ![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) one of **+Asteroid**, **+Debris**, **+Empty Space** (no value) to make the impact play on that kind of target instead
  - Name of armor type the target must have in order for this impact to play. An unknown armor type logs a warning and the impact is not added. Shields can have their own armor types. (Subsystems can as well, but at present this system will not detect that.)
  - Syntax: ***'String***', name of armor type in [armor.tbl](armor.tbl)
- **+Min Health Threshold:**
  - Fraction of target health below which impact will not play. Health is calculated separately for individual shield quadrants and hull.
  - Optional; defaults to 0.0.
  - Syntax: ***'Float***', fraction of health from 0.0 to 1.0
- **+Max Health Threshold:**
  - Fraction of target health above which impact will not play. Health is calculated separately for individual shield quadrants and hull.
  - Optional; defaults to 1.0.
  - Syntax: ***'Float***', fraction of health from 0.0 to 1.0
- ![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) **+Min Damage/Hitpoints Ratio:**, **+Max Damage/Hitpoints Ratio:**
  - Optional. The impact only plays if the damage dealt, divided by the target's hitpoints, is within this range.
  - Syntax: random range
- **+Min Angle Threshold:**
  - Hit angle -- that is, the angle measuring how 'straight on' you're hitting the target surface -- below which impact will not play.
  - Optional; defaults to 0.0.
  - Syntax: ***'Float***', degrees
- **+Max Angle Threshold:**
  - Hit angle above which impact will not play.
  - Optional; defaults to 180.0.
  - Syntax: ***'Float***', degrees
- **+Dinky:**
  - Whether to play if and only if the weapon is not armed. If false, the impact will not play if the weapon is not armed.
  - Optional; defaults to false.
  - Syntax: ***'Boolean***'
- ![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) **+Disable If Player Parent:**
  - Optional. The impact doesn't play when the player fired the weapon.
  - Syntax: ***'Boolean***'
- ![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) **+Disable On Subsystem Passthrough:**
  - Optional. The impact doesn't play when the weapon passes through a subsystem.
  - Syntax: ***'Boolean***'
- **+Effect Name:**
  - Required. Name of [particle effect](Particle-Effects) that will play when conditions are met.
  - Syntax: ***'String***', name of effect
- ![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) **+Laser Pokethrough Effect Name:**
  - Optional. A second particle effect played where a laser pokes out the far side of the target.
  - **+Laser Pokethrough Threshold:**: optional, ***'Float***', the threshold for it
  - **+Disable Main On Pokethrough:**: optional, ***'Boolean***', skips the main effect when the pokethrough effect plays
- The Health Threshold, Damage/Hitpoints and Angle values are random ranges.

### $Inflight Effect:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Sets the inflight [particle effect](Particle-Effects) which is used when the laser is active. This is only used by laser weapons.
- Syntax: ***String***, the particle effect name

### $Freeflight Effect:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Sets the [particle effect](Particle-Effects) which is used when the missile is in the free flight phase. See [$Free Flight Time](#free-flight-time) for more information.
- Syntax: ***String***, the particle effect name
- This option is only valid for missile weapons

### $Ignition Effect:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Sets the ignition [particle effect](Particle-Effects) which is used when the missile is transitioning from free flight to homed or unhomed flight.
- Syntax: ***String***, the particle effect name
- This option is only valid for missile weapons

### $Homed Flight Effect:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Sets the ignition [particle effect](Particle-Effects) which is used when the missile is in homed flight mode.
- Syntax: ***String***, the particle effect name
- This option is only valid for missile weapons

### $Unhomed Flight Effect:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Sets the ignition [particle effect](Particle-Effects) which is used when the missile is in unhomed flight mode.
- Syntax: ***String***, the particle effect name
- This option is only valid for missile weapons

### $Muzzleflash:

- Normally only with flak cannons.
- Defines the muzzle flash the weapon uses
- Syntax: ***String***, muzzleflash name, as defined in [mflash.tbl](mflash.tbl)

### $Muzzle Effect:

- Defines the [particle effect](Particle-Effects) used when the weapon fires. For muzzle effects, +Parent Velocity Factor uses the velocity of the firing ship, not the weapon.
- Syntax: ***String***, the particle effect name

### $EMP Intensity:

- How deeply an EMP effect scrambles your electronics
  - Needs the [EMP](#emp) flag set to have any effect
- Basically a percentage, a value of 250 results in a 25% chance of something getting scrambled each frame.
- Maximum value of 500.0
  - Syntax: ***Float*** (defaults to 300.0)

### $EMP Time:

- Duration of the EMP effect
  - Needs the [EMP](#emp) flag set to have any effect
  - Syntax: ***Float***, seconds (defaults to 10.0)

### $Recoil Modifier:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Multiplies the recoil force of a weapon by this value, use a value greater than 1.0 to increase the force, use a value between 1.0 and 0.0 to decrease.
  - Syntax: ***Float*** (defaults to 1.0)

> **Note:** The "apply recoil" weapon flag must be set for this to be valid. If the flag is not set, FSO will throw a warning and automatically set it at runtime.


### $Shudder Modifier:

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- Multiplies the screen shake this weapon causes. Needs the `"shudder"` flag; without it a warning is logged and the flag is set.
- Syntax: ***Float***

### $Leech Weapon:

  - The amount of weapon energy to drain with each hit (a flat value, not a percentage)
  - Needs the [Esuck](#esuck) flag set to have any effect
  - Syntax: ***Float*** (defaults to 10.0)

### $Leech Afterburner:

  - The amount of afterburner fuel to drain with each hit (a flat value, not a percentage)
  - Needs the [Esuck](#esuck) flag set to have any effect
  - Syntax: ***Float*** (defaults to 10.0)

### $Vampiric Healing Factor:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Multiplier for how much the shooter is healed after dealing hull damage (1.0 means healing done is equal to damage dealt)
  - Needs the [Vampiric](#vampiric) flag set to have any effect
- Must be a positive number higher than 0.0
  - Syntax: ***Float*** (defaults to 1.0)

### $Corkscrew:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Allow corkscrew parameters to be defined
- **+Num Fired:**
  - Defines the number of weapons fired in sequence. Same as the standard swarm entry but for corkscrew missiles
  - Syntax: ***Integer***, number of missiles
- **+Radius:**
  - Defines the radius used in the corkscrew motion
  - Syntax: ***Float***, meters
- **+Fire Delay:**
  - Defines the delay between missile firings
  - Syntax: ***Integer***, milliseconds
- **+Counter rotate:**
  - Defines if every other missile rotates to a different direction (clockwise / anticlockwise)
  - Syntax: ***Boolean***, *0* or *1*, no or yes
- **+Twist:**
  - Defines the speed of rotation movement on the radius of corkscrew motion.
  - Syntax: ***Float***, radians per second

### $Electronics:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the weapons effect against electronics. Requires ["electronics"](#electronics) flag. **+New Style:** and **+Old Style:** options are mutually exclusive.
- **+New Style:**
  - Defines that the electronics use the new style which enables more options.
- **+Old Style:**
  - Defines that the electronics use the old style.  The old style was the way the D-Missile worked in Descent: FreeSpace.

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- **+Area Of Effect**
  - If specified, allows the electronics effect to apply to every ship hit by this weapon's blast or shockwave (the "Electronics" flag isn't necessary with this option).

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **+Intensity:**
  - May only be used with **+New Style:** electronics. ***Has been deprecated***.
  - Syntax: ***Float***
- **+Lifetime:**
  - Syntax: ***Integer***, milliseconds
- **+Engine Multiplier:**
  - May only be used with **+New Style:** electronics. Defines how much longer the disruption effect lasts for engine subsystems.
  - Syntax: ***Float***
- **+Weapon Multiplier:**
  - May only be used with **+New Style:** electronics. Defines how much longer the disruption effect lasts for non-beam turrets.
  - Syntax: ***Float***
- **+Beam Turret Multiplier:**
  - May only be used with **+New Style:** electronics. Defines how much longer the disruption effect lasts for beam turrets.
  - Syntax: ***Float***
- **+Sensors Multiplier:**
  - May only be used with **+New Style:** electronics. Defines how much longer the disruption effect lasts for sensor subsystems.
  - Syntax: ***Float***
- **+Randomness Time:**
  - Defines the randomness of duration target suffers from the electronics hit. A random number between 0 and this time will be added or subtracted from the disruption time.
  - Syntax: ***Integer***, milliseconds

- Without any electronics parameters, a weapon with the "electronics" flag will default to the *Old Style* mode with a lifetime of 6000 milliseconds and a randomness of 4000 milliseconds.  Thus any ship that gets hit can get disrupted for anywhere between 2 to 10 seconds.

### $Spawn Angle:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the cone (with an angle) where the spawned weapons appear. *Value* gives the angle from the missile's centerline.  The maximum possible angle between any two child weapons would be *Value*2*.
- Like all spawn options, Spawn Angle should be defined as many times as the carrier weapon has child weapons defined for it (up to 5), before moving to the next option.
- Syntax: ***Float***, degrees, default is 180

### $Spawn Minimum Angle:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Defines an inner angle for the cone within which the spawned weapons appear. *Value* gives the angle from the missile's centerline.
- Like all spawn options, Spawn Minimum Angle should be defined as many times as the carrier weapon has child weapons defined for it (up to 5), before moving to the next option.
- Syntax: ***Float***, degrees, default is 0

### $Spawn Interval:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Causes the associated spawn event to happen continuously at this time interval. Enabling this will prevent the spawn event from triggering on detonation.
- NOTE: Having very large numbers of total children (low interval and/or high count) can cause issues for multiplayer, and you will be warned in multiplayer if the total count is greater then 300.
- Like all spawn options, Spawn Interval (and its associated Delay) should be defined as many times as the carrier weapon has child weapons defined for it (up to 5), before moving to the next option.
- Syntax: ***Float***, seconds, minimum is 0.1, **You will not be warned for a negative spawn interval. ** The value will be ignored and the spawn will be treated as a regular detonate spawn if negative. This is on purpose so that continuous/detonate spawns can be set regardless of the order in which they are defined in the flags.

#### +Delay:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Continuous spawn will not begin until this has elapsed.
- Syntax: ***Float***, seconds, default is 0

### $Spawn Chance:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- The chance (0 to 1) that the matching child weapon is spawned. Like all spawn options, list it once per child weapon, in order. A value outside 0 to 1 logs a warning and is ignored.
- Syntax: ***Float***

### $Spawn Effect:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Specifies the [particle effect](Particle-Effects) which will be used for continuous spawns of this weapon. Will not be used for detonation.
- Like all spawn options, Spawn Effect should be defined as many times as the carrier weapon has child weapons defined for it (up to 5), before moving to the next option.
- Syntax: **String**, the particle effect name


### $Spawn Aimed:

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- Whether the matching child weapons are aimed at the parent's target instead of spread. List it once per child weapon, in order.
- Syntax: ***Boolean***

### $Spawn Aim Lead:

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- How much aimed child weapons lead the target. Only works with `$Spawn Aimed: YES` (otherwise a warning is logged and it is ignored). Negative values log a warning and 0 is used. List it once per child weapon, in order.
- Syntax: ***Float***

### $Lifetime Variation Factor When Child:

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- Varies the lifetime of this weapon when it is spawned as a child weapon.
- Syntax: ***Float***

### $Local SSM:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the local SSM (Subspace Missile). In effect missile moves via subspace to a position close to the target and starts homing.
- Weapon must be ***aspect seeking missile*** and must have ***[local ssm](#local-ssm)*** flag.
- **+Warpout Delay:**
  - Defines the time between the missile launch and the subspace jump.
  - Syntax: ***Integer***, milliseconds
- **+Warpin Delay:**
  - Defines the time missile stays in subspace before exiting.
  - Syntax: ***Integer***, milliseconds
- **+Stage 5 Velocity:**
  - Defines the velocity of the missile after the subspace jump.
  - Syntax: ***Float***, meters per second
- **+Warpin Radius:**
  - Defines how far from the SSM will appear from the outer radius of the target.
  - Syntax: ***Float***, meters
- **+Lock Range:**
  - Defines the maximum range where SSM can achieve target lock
  - Syntax: ***Float***, meters
- ![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg) **+Warp Effect:**
  - The fireball used for the warp effect: a fireball index or its unique ID from [fireball.tbl](fireball.tbl). An unknown one logs a warning and the default warp is used.
  - Syntax: ***Integer*** or ***String***

### $Countermeasure:

- Defines the countermeasure effectiveness, radius, etc

> **Note:** - Spoofing chance is defined as **cm type effectiveness**/**[seeker strength](#seeker-strength)** (missile must be within the countermeasures **+Effective Radius:**).
> - All countermeasures in the mission perform tests to spoof missiles for two frames after **any countermeasure** is created in the mission. No, this doesn't seem to make much sense.
> 
> ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) The global frame counter can be bypassed by using **[pulse interval](#pulse-interval)**.
> 
> - **Provisional**; Countermeasures are only given one chance to spoof any individual missile. This makes tuning seeker strength/effectiveness meaningful.
>   - Previously some record of which countermeasures had been seen by each missile was kept, but it broke when multiple countermeasures were involved. A good chance of breaking with two countermeasures, and a guaranteed chance of breaking with three or more. This reduced the impact of the seeker strength/effectiveness ratio, but was partly cancelled out by the 2 frames thing.

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

#### **+Heat Effectiveness:**

- Defines the countermeasures effectiveness against heat seeking missiles
- Syntax: ***Float***, cm effectiveness
  - Default: 1.0

#### **+Aspect Effectiveness:**

- Defines the countermeasures effectiveness against aspect seeking missiles
- Syntax: ***Float***, cm effectiveness
- Default: 1.0

#### **+Effective Radius:**

- Defines the countermeasures effective radius
- Syntax: ***Float***, meters
  - Default: 300.0

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

#### +Missile Detonation Radius:

- Defines the radius within which a missile homing on this countermeasure will trigger a detonation (see **+Single Missile Kill:**)
- Syntax: ***Float***, meters
  - Default: 40.0

> **Note:** only affects missiles homing on countermeasures, i.e. heat or aspect with "pulls aspect seekers" flag

#### +Launch Vector:

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- The direction the countermeasure is launched in, relative to the ship.
- Syntax: ***Vector***, three **floats**

#### +Launch Speed:

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- The countermeasure's launch speed. Negative values log a warning and the default is used.
- Syntax: ***Float***, meters per second
  - Default: 25

#### +Launch Variance:

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- Random variation of the launch. Negative values log a warning and the default is used.
- Syntax: ***Float***
  - Default: 2

#### +Single Missile Kill:

- Defines if countermeasures will detonate only the missile homing on this countermeasure, or every missile within **+Missile Detonation Radius:**
- Syntax: ***Boolean***
  - Default: false

> **Note:** only affects missiles homing on countermeasures, i.e. heat or aspect with "pulls aspect seekers" flag

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

#### +Pulse Interval:

- Defines how much time passes between each pulse of this countermeasure.
- Syntax: ***Float***, milliseconds
  - Default: 0

> **Note:** any value less than or equal to 0 means the countermeasure doesn't pulse, and uses the global 2-frame counter after any countermeasure is fired.

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

#### +Use Fire Wait:

- Makes the countermeasure fire wait use what is specified in the **$Fire Wait:** section of the weapon.
- Default countermeasure fire wait is 333 milliseconds, regardless of that value was set in **$Fire Wait:**.

#### +Failure Launch Delay Multiplier for AI:

- After a failed countermeasure launch the game waits a certain time interval before attempting another launch.
- Previously, the amount of time was calculated as ('fire wait' + 2000 * 'countermeasure firing chance').
- If set to a non negative number, this value will be multiplied to the fire wait value to determine the amount of time before another countermeasure can be attempted.
- Syntax: ***Float***
  - Default: -1

> **Note:** any value less than 0 means the default formula will be used.

#### +Successful Launch Delay Multiplier for AI:

- After a successful countermeasure launch the game waits a certain time interval before attempting another launch.
- This value is multiplied to the fire wait value to determine the amount of time before another countermeasure can be attempted.
- Syntax: ***Float***
  - Default: 2

> **Note:** any value less than 0 means the default value will be used.

### $BeamInfo:

- **+Type:**
- Defines the type of beam in question. This can be a number or, as of 21.4, a named type. Options are:
  - *0* or *Direct fire*, a standard non-slashing type beam that fires directly at a random point on the target's hull
  - *1* or *Slashing*, a slashing type beam, that slashes between opposing corners of the ship
  - *2* or *Targeting*, a targeting beam that is used to trigger SSMs.
  - *3* or *Antifighter*, a beam type that fires pulses in anti-fighter role and acts like type 0 beam in other roles.
  - *4* or *Normal Fire*, a beam that is restricted to what is directly in front of it, along turrets firepoints normal.

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- *5* or *Omni*, a highly flexible kind of beam with many options for its exact behavior. See $Type 5 Beam Options below.

- **+Life:**
  - How long it lasts once the beam is actually firing.
  - Syntax: ***Float***, seconds
- **+Warmup:**
  - Warmup time
  - Syntax: ***Integer***, milliseconds
- **+Warmdown:**
  - Warmdown time
  - Syntax: ***Integer***, milliseconds
- **+Radius:**
  - Muzzle glow radius
  - Syntax: ***Float***, meters

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- '''+Muzzle Particle Effect:
  - Specifies the [particle effect](Particle-Effects) which will be used for the muzzle warmup particles of this beam.
  - Syntax: **String**, the particle effect name
  - If this option is specified then *+PCount*, *+PRadius*, *+PAngle* and *+PAni* are not valid options!

- **+PCount:**
  - Particles spewed every interval. ??
  - Syntax: ***Integer***
- **+PRadius:**
  - Particle radius
  - Syntax: ***Float***, meters
- **+PAngle:**
  - Defines the random "cone" where the particles are generated
  - Syntax: ***Float***, angle of the cone
- **+PAni:**
  - Animation used to represent the particles
  - Syntax: ***String***, filename
- **+Miss Factor:**
  - The higher the numbers, the more likely that this beam will miss. One value for every difficulty level
  - Syntax: ***Miss Factor***, five **floats**, very easy, easy, moderate, hard, insane, respectively.
    - Example: 0.5 0.975 1.35 1.875 2.5

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- **IFF specific miss factors**
  - Identical to the preceding entry but IFF specific. Replace the <IFF> with the name of the IFF from [iff_defs.tbl](iff_defs.tbl)
  - Syntax: ***+<IFF> Miss Factor:***, five **floats**, very easy, easy, moderate, hard, insane, respectively.
    - Example: 0.5 0.975 1.35 1.875 2.5

- **+BeamSound:**
  - Corresponding looping sound effect when the beam is firing.
  - Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)
- **+WarmupSound:**
  - Corresponding sound effect when the beam is beam warmed up.
  - Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)
- **+WarmdownSound:**
  - Corresponding sound effect when the beam is warming down.
  - Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)
- **+Muzzleglow:**
  - Defines the bitmap used to create the muzzle glow
  - Syntax: ***String***, filename

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- **+Directional Glow:**
  - Defines the length to stretch the muzzleglow along the beam.
  - Syntax: ***Float***, length in meters, the muzzleglow is rendered along (in the same plane) as the beam instead of perpendicular to the camera.

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- **+Opacity over Lifetime Curve:**
  - Specifies the curve to use for changing the beam's opacity over time.
  - The life of the beam while firing (0-1) is X, the opacity is Y, where 0 is invisible and 1 is typical opacity.
  - Syntax: *String*, the name of the curve from [curves.tbl](curves.tbl)

- **+Shots:**
  - Defines into how may invidual shots the type 3 (AAA) beams are divided. Note total firing is the same as the +Life:
  - ALSO AFFECTS FIGHTER BEAMS. If greater than 0, fires from up to that many fire points at a time before cycling. 1 mimics the behavior of weapons with the "cycle" flag, 0 fires from all available fire points like normal weapons.
  - Syntax: ***Integer***, Default: 1
- **+ShrinkFactor:**
  - How much of the beam's life is remaining before it starts to shrink in width. (0.1 means 10% life left)
  - Syntax: ***Float***, value from 0 to 1
- **+ShrinkPct:**
  - The beam's width factor is reduced by this amount every second. For example, a value of 0.25 means the beam is at 75% width after 1 second, 50% after 2, 25% after 3, and by 4 seconds it reaches the minimum size of 10%.
  - Syntax: ***Float***, a positive value

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- **+GrowFactor:**
  - How much of the beam's life is remaining before it starts to grow in width. (1.0 means as soon as the beam fires)
  - Syntax: ***Float***, value from 0 to 1
- **+GrowPct:**
  - The beam's width factor is increased by this amount every second. For example, a value of 0.25 means a beam starting at 0% is at 25% width after 1 second, 50% after 2, 75% after 3, and by 4 seconds it reaches the maximum size of 100%.
  - Syntax: ***Float***, a positive value
- **+Initial Width Factor:**
  - What factor of the beam's maximum width it starts with.
  - Syntax: ***Float***, value from 0 to 1

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **+Range:**
  - Defines the maximum range where the beam still has any effect.
  - Syntax: ***Float***, meters
    - Default: 30000
- **+Attenuation:**
  - Defines the point where the damage caused by the beam starts to attenuate. At maximum range damage is zero.
  - Syntax: ***Float***, from 0 - firing point to 1 - no effect
    - Default: 1

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- **+BeamWidth:**
  - Defines the width of the beam for collision test purposes instead of using widest beam section.
  - Syntax: **''Float**'

![FSO 22.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.2.svg)

- **+Beam Light Flickers:**
- Controls if the dynamic flickering of the beam width is applied to the light cast by the beam as well
- Syntax: ***boolean***
- Default value: True
- **+Beam Width Multiplies Light Radius:**
- Controls if the light radius of a beam is multiplied by the beam width.
- Syntax: ***boolean***
- Default value: True

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- **+Beam Flash Particle Effect:**
  - Specifies the beam flash [particle effect](Particle-Effects) (will be applied every frame where the beam is colliding).
  - Syntax: ***String***, particle effect name
  - If this is specified then **+Beam Flash Effect** and **+Beam Flash Radius** are not valid options!

- **+Beam Flash Effect:**
  - Defines the additional impact flash effect for the beam.
  - Syntax: ***String***, filename
- **+Beam Flash Radius:**
  - Defines the radius for the additional impact flash effect.
  - Syntax: ***Float***
- ![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) **+Beam Piercing Particle Effect:**
  - The [particle effect](Particle-Effects) for the beam's piercing effect. If set, the other +Beam Piercing options below are not valid.
  - Syntax: ***String***, particle effect name
- **+Beam Piercing Effect:**
  - Defines the beam piercing (bursting flames) effect for the beam (will be applied every frame with a beam that is strong / large enough to pierce through its target).
  - Syntax: ***String***, filename
- **+Beam Piercing Radius:**
  - Defines the average radius used for the beam piercing effects.
  - Syntax: ***Float***
- **+Beam Piercing Effect Velocity:**
  - Defines the base velocity for the piercing effects
  - Syntax: ***Float***
- **+Beam Piercing Splash Effect Velocity:**
  - Defines the base velocity for the secondary piercing effects
  - Negative velocity causes the particles to move to opposite direction
  - Syntax: ***Float***
- **+Beam Piercing Effect Variance:**
  - Defines the variances of the particles from the main path
  - Values close to 0 cause minimum variance, higher values cause higher variances
  - Syntax: ***Float***

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- **$Type 5 Beam Options:**
  - Species the unique attributes for type 5 beams. **None of these properties will be used if the weapon is not a type 5 beam.**
  - Fundamentally, type 5 beams are slashing type beams, where the start and end positions refer to physical positions in space (and not angles from the point of view of the shooter) and have many parameters for choosing their positions, including having them be on top of each other, and thus forgoing standard slashing behavior, and also for having rotations, both during the beam firing and in between firings.
- **+Start Position:**
  - Determines how the starting position should be chosen. Options are:
    - **RANDOM ON SHIP**, Like type 0 direct-fire beams, chooses a random spot on the target's geometry.
    - **RANDOM OFF SHIP**, Like type 1 slash beams, chooses a random spot just outside of the target's geometry.
    - **CENTER**, Chooses the center of the target.
- **+Start Position Offset:**
  - Displaces the chosen start position in the specified direction and by the specified amount.
    - Whether the x, y, and z directions refer to the shooter's point-of-view, or the target's point-of-view is determined by **Orient Offsets to Target** below.
    - Whether a value of 1 for any coordinate means the target's radius, or a fixed span from the point-of-view of the shooter is determined by **Scale Offsets to Target** below.
    - Syntax: ***Vector***, x, y, and z coordinate. Example: "1, -0.2, -0.9"
- **+Start Position Randomness:**
  - Further displaces the chosen start position but to a random degree in the specified axes from the number provided to its negative (i.e. a value of 1 will randomly choose between 1 and -1).
    - Whether the x, y, and z directions refer to the shooter's point-of-view, or the target's point-of-view is determined by **Orient Offsets to Target** below.
    - Whether a value of 1 for any coordinate means the target's radius, or a fixed span from the point-of-view of the shooter is determined by **Scale Offsets to Target** below.
    - Syntax: ***Vector***, x, y, and z coordinate. Example: "1, 0.2, 1"

- The following three fields can skipped, and the beam will not traditionally sweep like a slash beam (although it may still rotate).
- **+End Position:**
  - Determines how the starting position should be chosen. Options are:
    - **RANDOM ON SHIP**, Like type 0 direct-fire beams, chooses a random spot on the target's geometry.
    - **RANDOM OFF SHIP**, Like type 1 slash beams, chooses a random spot just outside of the target's geometry.
    - **CENTER**, Chooses the center of the target.
    - **SAME RANDOM**, If the start position was randomly determined, this will cause the end position to inherit that position (it will not inherit offset or randomness, only the randomness from RANDOM ON/OFF SHIP).
- **+End Position Offset:**
  - Displaces the chosen end position in the specified direction and by the specified amount.
    - Whether the x, y, and z directions refer to the shooter's point-of-view, or the target's point-of-view is determined by **Orient Offsets to Target** below.
    - Whether a value of 1 for any coordinate means the target's radius, or a fixed span from the point-of-view of the shooter is determined by **Scale Offsets to Target** below.
    - Syntax: ***Vector***, x, y, and z coordinate. Example: "1, -0.2, -0.9"
- **+End Position Randomness:**
  - Further displaces the chosen end position but to a random degree in the specified axes from the number provided to its negative (i.e. a value of 1 will randomly choose between 1 and -1).
    - Whether the x, y, and z directions refer to the shooter's point-of-view, or the target's point-of-view is determined by **Orient Offsets to Target** below.
    - Whether a value of 1 for any coordinate means the target's radius, or a fixed span from the point-of-view of the shooter is determined by **Scale Offsets to Target** below.
    - Syntax: ***Vector***, x, y, and z coordinate. Example: "1, 0.2, 1"

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- **+Slash position over beam lifetime curve:**
  - Curve defining the beam's slash position over its lifetime.
    - X is the beam's lifetime, with 0 being the beginning and 1 the end. Y is slash position, with 0 being the start position and 1 the end position. Values outside of the 0-1 range will work.
    - This is optional.
    - Syntax: *String*, the name of the curve from [curves.tbl](curves.tbl)

- **+Orient Offsets to Target:**
  - This determines the orientation of the offsets and randomness.
  - If this is set to *TRUE* then the x, y, and z, directions used refer to the target's x, y, and z coordinates. X being left/right, Y being up/down, Z being front/back. A start offset of 0, 0, 1, and an end offset of 0, 0, -1 would always cause the beam to fire at the target's bow and sweep to its aft, no matter which way the shooter is oriented.
  - If this is set to *FALSE* then the x, y, and z, directions used refer to the shooter's x, y, and z coordinates. Using the z coordinate with this being false will have little effect. A start offset of 1, 0, 0, and an end offset of -1, 0, 0 would always cause the beam to fire towards the target, but to the shooter's right, and sweep in the direction of the shooter's left, regardless of how the target is oriented.
  - Syntax: ***Boolean***, yes or no, Default: No
- **+Scale Offsets to Target:**
  - This determines what a magnitude of 1 on the offsets and randomness mean.
  - If this is set to *TRUE* then a value of 1 for a coordinate means a position in the chosen direction but at a distance equal to the target's radius. Values greater than 1, will drift further and further from the target entirely, while smaller than 1 will bring it closer to its originally chosen start position.
  - If this is set to *FALSE* then it is as though it were firing at a target with a 300m radius at a 300m distance. A value of 1 corresponds to 45 degrees, which it will start at regardless of the distance of the target. Values greater than 1 will increase beyond 45 degrees but will never exceed 90 degrees
  - Syntax: ***Boolean***, yes or no, Default: No

- **+Continuous Rotation:**
  - This defines the degrees per second that the beam will rotate while it is firing.
  - Syntax: ***Float***, degrees per second
- **+Continuous Rotation Axis:**
  - This defines the axis about which the beam will continuously rotate. Options are:
    - **START POSITION BEFORE OFFSET**, the beam will rotate about the start position before the offset or randomness were applied. A beam which does not slash (end position paramters omitted), but is offset and rotates about this axis will trace out circles, for instance.
    - **END POSITION BEFORE OFFSET**, as above but for the end position.
    - **START POSITION AFTER OFFSET**, the beam will rotate about the final start position after offset and randomness have been applied. If this beam slashes (end position paramters specified) it will trace out a spiral.
    - **END POSITION BEFORE OFFSET**, as above but for the end position.
    - **CENTER**, the beam will rotate about the center of the target.

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- **+Rotation over beam lifetime curve:**
  - Curve defining the beam's rotational position over its lifetime.
    - X is the beam's lifetime, with 0 being the beginning and 1 the end. Y is rotational position, with 0 being no rotation and 1 being a full 360-degree turn. Values outside of the 0-1 range will work.
    - This is optional.
    - If a rotation curve is defined, "+Continuous Rotation" will do nothing.
    - Syntax: *String*, the name of the curve from [curves.tbl](curves.tbl)

- **+Burst Rotation Pattern:**
  - Defines a list of rotations, for the final start and end positions to be performed right before firing for each specific shot in a burst (will not rotate the beam while firing). **You will not be warned for failing to give this weapon burst capability**, because on spawned or subspace strike beams, providing a pattern has the additional effect of spawning multiple copies of this weapon all at once, each one corresponding to another entry in the pattern.
  - For instance, a standard slash with a rotation pattern of  (0, 180) will cause the beam weapon to slash back and forth, or in a scissor pattern if the burst is very short and the beams fire at similar times.
  - (0,120,240) will have 3 slashes in a triangular pattern.
  - If a RANDOM ON/OFF SHIP option has been chosen for the start or end positions, the "burst shares random target" flag may be used so all shots in the burst share the same random position.
  - If any values are ***negative*** they will instead use a random amount of rotation for that entry.
  - Syntax: ***List of Floats***, degrees. Example: (0, 90, 180, 270)
- **+Burst Rotation Axis:**
  - The axis about which to rotate for the burst rotation pattern. Options are:
    - **START POSITION BEFORE OFFSET**, the beam will be rotated about the start position before the offset or randomness are applied.
    - **END POSITION BEFORE OFFSET**, as above but for the end position.
    - **START POSITION AFTER OFFSET**, the beam will be rotated about the final start position after offset and randomness have been applied.
    - **END POSITION BEFORE OFFSET**, as above but for the end position.
    - **CENTER**, the beam will be rotated about the center of the target.
- **+Per Burst Rotation:**
  - Will rotate the final start and end positions, possibly in addition to the burst rotation pattern, for each burst of the beam, or every shot if not a burst weapon.
  - If this value is ***negative*** this will instead be treated as a random amount of rotation.
  - Syntax: ***Float***, degrees.
- **+Per Burst Rotation Axis:**
  - The axis about which to rotate for the per burst rotation. Options are:
    - **START POSITION BEFORE OFFSET**, the beam will be rotated about the start position before the offset or randomness are applied.
    - **END POSITION BEFORE OFFSET**, as above but for the end position.
    - **START POSITION AFTER OFFSET**, the beam will be rotated about the final start position after offset and randomness have been applied.
    - **END POSITION AFTER OFFSET**, as above but for the end position.
    - **CENTER**, the beam will be rotated about the center of the target.

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- **+Firing Length Effect:**
- Specifies the [particle effect](Particle-Effects) which will be created along the entire active length of the beam while it is firing and active.
- Note that the `+Particle Count Per Spawn:` (or `+Number:` for legacy particles) for the effect used will be *per kilometer of length*.
- [Parent Velocity Factor](https://wiki.hard-light.net/index.php/Particle_Effects#+Parent_Velocity_Factor:) also works, inheriting the [Velocity](#velocity) of this weapon.
- Syntax: **String**, the particle effect name

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- **+Paused Length Effect:**
- Specifies the [particle effect](Particle-Effects) which will be created along the length of the beam while it is firing, but in between bursts of a type 4 anti-fighter beam.
- Note that the +Number for the effect used will be *per kilometer of length*.
- [Parent Velocity Factor](https://wiki.hard-light.net/index.php/Particle_Effects#+Parent_Velocity_Factor:) also works, inheriting the [Velocity](#velocity) of this weapon.
- Syntax: **String**, the particle effect name

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

- **+Beam Flags:**
- Extra flags specific to beams
  - **burst shares random target**, all shots from this beam weapon in the same burst will target the same random point (or slash direction)
  - **track own texture tiling**, the 'U' position of the beam texture, whose speed is set by **+Translation:** below, is tracked independently, rather than being shared among all beams like normal.

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- **direct fire lead target**, if the beam is direct fire (type 0), makes the beam aim for the target's predicted position when the beam initially starts warming up, allowing the target to 'dodge' the beam.

- **$Section:**
  - Beams are made up of up to five sections layered atop one another to create the effect. Each section has its own parameters.


  ![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

  - **+Index:**
    - Field used to define with [Modular Tables](https://wiki.hard-light.net/index.php/Modular_Tables) the beam section which is edited.
    - Only valid when editing beam sections that already exist in other tables. Attempting to index new sections will generate warnings, adding new sections is the default behavior for un-indexed sections.
    - Syntax: ***Integer***, index number legal values are from 0 to 5
  - **+remove**
    - Removes the set beam section.
  - **+nocreate**
    - Does not create new section for the beam

  - **+Width:**
    - Width of the section
    - Syntax: ***Float***, meters
  - **+Texture:**
    - Texture for the section
    - Syntax: ***String***, filename
  - **+RGBA Inner:**
    - Defines the color and transparency of the center of the section


  ![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

    - Not used in current builds. Settings entered here will have no effect.

    - Syntax: ***Color***, red, green, blue, alpha, respectively, value from 0 to 255
    - Example: 255 255 255 255
  - **+RGBA Outer:**
    - Defines the color and transparency of the edges of the section


  ![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

    - Not used in current builds. Settings entered here will have no effect.

    - Syntax: ***Color***, red, green, blue, alpha, respectively, value from 0 to 255
    - Example: 150 150 150 10
  - **+Flicker:**
    - Defines how much the section flickers
    - Syntax: ***Float***, value from 0 to 1
  - **+Zadd:**
    - Defines the 'thickness' of the section. The higher the Zadd, the closer the section appears to be. (Zadd can be safely ignored; it actually does nothing.)
    - Syntax: ***Float***


  ![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

  - **+Tile Factor:**
    - Defines the beam texture tiling factor. Second number defines the tiling type. Type 0 tiles the graphic in the beam as many times as defined by the 1st number. Type 1 uses the 1st value as the lenght of the tile.
    - Syntax: ***Float***, ***Integer***
  - **+Translation:**
    - Defines the speed of translation of the beam, a positive value moves from the impact point to the firing point, while a negative value moves away from the firing point towards the impact point. Measured in tiles per second, where the tile count is defined in the "+Tile Factor:" field above.
    - Syntax: ***Float***

### $Pspew:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines a particle spew effect for the given weapon

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- **+Index:**
  - indicates which spewer to add/edit/delete. may be 0-3. used for xmt compatibility.

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- May be any positive number.

  - Syntax: ***Integer***
- **+Remove**
  - removes spewer at +Index: (requires +index:)

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- **+Effect:**
  - Specifies the [particle effect](Particle-Effects) which will be used for this Pspew.
  - Syntax: **String**, the particle effect name
  - If this option is specified then *+Type*, *+Count*, *+Time*, *+Vel*, *+Radius*, *+Life*, *+Scale*, *+Z Scale*, *+Rotation Rate*, *+Offset*, *+Initial Velocity* and *+Bitmap* are not valid options!

- **+Type:**
  - type of particle spew
  - Syntax: ***String*, Default, Helix, Sparkler, Ring, or Plume **

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **+Count:**
  - Defines how many particles are spawn when they are ready to be created (at each +Time: interval)
  - Syntax: ***Integer***
- **+Time:**
  - Defines the time in milliseconds between next particle(s) spewed
  - Syntax: ***Integer***, milliseconds
- **+Vel:**
  - Defines the created particles velocity as percentage of the weapons velocity
  - Syntax: ***Float***
- **+Radius:**
  - Defines the size of the created particles
  - Syntax: ***Float***, meters
- **+Life:**
  - Defines how long the particles last.
  - Syntax: ***Float***, seconds
- **+Scale:**
  - Defines the particle spread. Small value will make particles group together
  - typically controls how big the effect is or how fast it moves away from the weapon path on a perpendicular (or sometimes arbitrary) axis.
  - helical spews and rings use this value to control how fast the particle helix moves away from the central axis.
  - sparklers use this to control how fast particles move away from the spawn point, in any direction
  - plumes use this to set the diameter of the disk across which particles are spawned
  - Syntax: ***Float***

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- **+Z Scale:**
  - for sparklers, allows you to elongate the effect to surround other effects, like lasers. essentially a length multiplier
  - for plume, it multiplies against +scale to set the plume to spread or converge, zero would be full convergence, 1 would be straight flight, and bigger numbers would cause particles to diverge from the disk.
  - Syntax: ***Float***
- **+Rotation Rate:**
  - used only by helical spews, controls how many rotations per second the helix makes
  - Syntax: ***Float*, may be negative, useful for setting up 2 spews that spin in opposite directions**
- **+Offset:**
  - position of effect with regards to the weapon center
  - Syntax: ***Vector***
- **+Initial Velocity:**
  - to indicate both direction and speed of particle effects regards to the weapon velocity and +scale:
  - Syntax: ***Vector***

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- '''+Bitmap:
  - Defines the animation used to create the particle spew effect
  - Syntax: ***String***, filename

### $Tag:

- Allowed for secondary weapons

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Allowed for primary weapons and secondary weapons

- Sets this weapon as a TAG missile, level 1, 2, or 3. The second number is the duration in seconds.
- TAG Level 1 will illuminate hidden targets such as stealth fighters or ships normally obscured by nebula conditions.
- TAG Level 2 illuminates targets much like TAG Level 1, but turrets will prefer to shoot at them over other ships.
- TAG Level 3 has no effect in retail.

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- TAG Level 3 allows the weapon to trigger [SSMs](weapons.tbl#ssm)

- Syntax: ***Integer*** ***Float***, TAG level and seconds respectively
- **Note**: TAG will not trigger unless the weapon in question has a non-zero damage value.

### $SSM:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the weapons SSM index number.
- Requires [TAG level 3](#tag) or a targeting beam.
- Syntax: ***Integer***

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Now also accepts SSM strike names.
- Syntax: ***String***

### $FOF:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the field of fire for the weapon. Causes weapon to fire into a firing cone instead of direct firing line thus causing light inaccuracy
- Syntax: ***Float***, degrees

#### +FOF Spread Rate:

![FSO 3.7](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.svg)

- Defines the spread rate of the field of fire. This is a percentage of the +Max FOF: defined below, and is applied each time the weapon is fired. For exaple, a value of 0.1 causes the field of fire to increase by 10% of the +Max FOF: per trigger pull. This is unaffected by the number of fire points.
- Syntax: ***'Float***', rate

#### +FOF Reset Rate:

![FSO 3.7](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.svg)

- Rate at which the spread decreases if the weapon isn't firing. This is a percentage of the +Max FOF: defined below, and is applied once per second. For example, a value of 0.1 causes the field of fire to decrease by 10% of the +Max FOF: per second.
- (If you want to get technical, this is applied every frame, but scaled by the percentage of a second that frame represents. For almost all practical purposes, saying it's applied once per second is equivalent, and makes the calculation easier to think about.)
- Syntax: ***'Float***', rate
- This is required if **+FOF Spread Rate:** is present

#### +Max FOF:

![FSO 3.7](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.svg)

- The maximum FOF. After the FOF reaches this value it will not increase any further.
- Syntax: ***'Float***', degrees. Must be greater than or equal to zero.
- This is required if **+FOF Spread Rate:** is present

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg) As of 24.2, this can be specified without also specifying `+FOF Spread Rate:`.

### $Firing Pattern:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Determines the order in which the weapon is fired from a bank's gunpoints. Does nothing for secondary weapons.
- This is optional. If a firing pattern is not set, the weapon will fire from all gunpoints simultaneously, as is standard retail behavior.
- If a firing pattern is set, the weapon's **$Fire Wait** will be divided by the number of gunpoints in the bank, so as to keep shots-per-second constant.
- Has five options:
  - **CYCLE FORWARD**: Fires  **$Shots** shots from the first **$Shots** gunpoints (starting at gunpoint 0), then from the next **$Shots** gunpoints, and so on.
  - **CYCLE REVERSE**: Same behavior as CYCLE FORWARD, but begins at the highest-numbered gunpoint and works downward.
  - **RANDOM EXHAUSTIVE**: Fires **$Shots** shots at a time from randomly-selected gunpoints until each gunpoint has been fired once, and then starts over.
  - **RANDOM NONREPEATING**: Fires **$Shots** randomly-selected gunpoints at a time, but never fires the same gunpoint twice in a row (unless forced to because $Shots is equal to more than half the number of gunpoints).
  - **RANDOM REPEATING**: Fires **$Shots** randomly-selected gunpoints at a time. No restrictions.
- Syntax: ***'String***'

### $Shots:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the number of shots created at each firepoint each time the weapon is fired. Produces shotgun-like behavior if used with **$FOF**.
- If the weapon has a non-standard firing pattern, instead determines the number of firepoints fired at a time.
- Works only for primary weapons
- Syntax: ***Integer***, number of shots

### $Cycle Multishot:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- If the weapon has a non-standard firing pattern, defines the number of shots created at each firepoint. Otherwise, does nothing.
- Syntax: ***'Integer***', number of shots

### $decal:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines impact decals if they are in use
- **+texture:**
  - Defines the texture used for impact decals (can use ***-burn*** and ***-glow*** textures)
  - Syntax: ***String***, filename
- **+backface texture:**
  - Defines ???
  - Syntax: ***String***, filename
- **+radius:**
  - Defines the size of the impact decals on target
  - Syntax: ***Float***, meters
- **+burn time:**
  - Defines the time the ***-burn*** texture appears on the target
  - Syntax: ***Integer***, milliseconds

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Feature removed - table parsing retained to maintain compatibility

### $Impact decal:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

- Defines an impact decal (think bullet impact/scorch mark) using the new system introduced in FSO 19.0.0.
  - Syntax:  ***String***, this must match to one of the entries in a [decal tbm](https://wiki.hard-light.net/index.php/Decals).
- **+Radius:**
  - Syntax: ***[Parsed random range](https://wiki.hard-light.net/index.php/Parsed_random_range)***, Radius of the impact decal; default is 1.0
- **+Lifetime:**
  - Syntax: Either ***Eternal*** or ***[Parsed random range](https://wiki.hard-light.net/index.php/Parsed_random_range)***, lifetime in seconds

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- **+Use Random Rotation:**
  - Syntax: ***Boolean***, Whether or not a random rotation is added to the decal; default is NO

### $Transparent:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines weapon effect as transparent. Enables optional fields for 'alpha animations'
- **+Alpha:**
  - Defines the strength of the alpha (transparency). And also the maximum value for the alpha cycle option
  - Syntax: ***Float***, from 0 to 1
- **+Alpha Min:**
  - Defines the minimum alpha value for alpha cycle option
  - Syntax: ***Float***, from 0 to 1
- **+Alpha Cycle:**
  - Defines the change in alpha per frame, between **Alpha** and **Alpha Min** values
  - Syntax: ***Float***

### $Weapon Hitpoints:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the amount of hitpoints a weapon has.
- Weapons with **"bomb"** flag have 50 hitpoints by default
- Giving weapon hitpoints makes it possible for the weapon to be shot down or be destroyed by EMP.
  - Syntax: ***Integer***

### $Armor Type:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- REQUIRES ADDITIONAL TABLE. Defines the armor type for the weapon. This works exactly like ship armor types.
  - Syntax: ***String***, name of the armor as defined in [armor.tbl](armor.tbl)

### $Burst Shots:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the number of shots in a burst
- Shots in a burst use shorter ( **$Burst Delay:** ) delay than usually
- If this weapon is cycling each individual firing event counts as one burst shot, so bursts end sooner in time for large banks than small.
  - Syntax: ***Integer***

### $Burst Delay:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the time in milliseconds between the shots in a burst
  - Syntax: ***Integer***

### $Burst Flags:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the flags used to define the burst firing behaviour
  - Syntax: ( ***String*** ***String*** ), list of flags
  - Available burst flags:
    - **"fast firing"**, acts as if **same turret cooldown** flag would be used between the shots in the burst.
    - **"random length"**, makes AI to fire burst of random length (from 1 to burst shots).

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) **"resets"**, as long as a full fire wait has passed since last fired, the burst is reset and can be fired in full again.

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg) **"num firepoints for burst shots"**, for turrets, makes the burst length equal to the number of firepoints on the turret.

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) **"burst only loop sounds"**, StartFiringSnd, LoopFiringSnd, and EndFiringSnd, if used, additionally also trigger off of when bursts begin and end.

### $Thruster Flame Effect:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the filename of the effect used on the modeled thruster of the missile
  - Syntax: ***String***, filename

### $Thruster Glow Effect:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the filename of the effect used on the glow type effect of the missile
  - Syntax: ***String***, filename

### $Thruster Glow Radius Factor:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the radius multiplier for the thruster's glow type effect
  - Syntax: ***Float***

### $Failure Rate:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Defines the random chance that the weapon will fail to fire, or spawn the substitute below if specified.
  - Syntax: ***Float***, 0 - 1

#### +Failure Substitute:

- Defines the weapon to substitute when the weapon 'fails'.
  - Syntax: ***String***, Weapon

### $On Create:

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

- Defines an [action system](https://wiki.hard-light.net/index.php/Action_System) program set to be executed when a weapon of this type is created. Only applicable to laser and missile style weapons.

### $Animations:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

- The new way of specifying triggered animations for weapons. Weapon needs to be a modelled weapon for animations.
- See [animation.tbl](Animation)
- Syntax: ***( "String" "String" )***, names of the animations for this ship, as defined in the Animation.tbl
  - Example: ( "wingflap" "docksequence" )

### $Driven Animations:

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- See [Animation_Drivers#Parsing (+Driver Set:)](https://wiki.hard-light.net/index.php/Animation_Drivers#Parsing_(+Driver_Set:))
- Driven animations **are not** compatible with $Animations! If you need animations without drivers, create a driver set that has no driver and list all animations in it's +Affected Animations:

### $Substitute:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg) Defines a pattern of ammo to substitute for primary or secondary weapons. **$Substitute:** can be repeated any number of times (so you could make something like a rainbow laser with several ammo substitutions, for example). If neither *+period* or *+index* is specified, it is assumed to be equivalent to *+index: **0***.

- Syntax: ***Weapon Name***
- **Swarm missiles act in a strange way when used with $Substitute, use at your own risk.**

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) Turrets can use $Substitute as well.

#### +period:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg) FSO will substitute **Weapon Name** every *period* shots.

- Syntax: ***Positive Integer***
- **You may not define both +period and +index for the same $Substitute entry**

##### +offset:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg) A suboption of +period, offsets the substitute pattern according to the launched ammo. For example, an offset of 3 and a period of 4 will substitute on the 3rd ammo launched, and then every 4th ammo after that.

- Syntax: ***Positive Integer***

#### +index:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg) FSO will substitute the **Weapon Name** at firepoint *index* in the shot pattern. Similar to carefully using both +period and +offset, but is not affected by ships with different firepoint numbers. i.e. Firepoint *index* will always have the substitution, while +period and +offset will cycle at different rates dependant on the number of firepoints.'''

- Syntax: ***Positive Integer***

- **You may not define both +period and +index for the same $Substitute entry**

- **Indexes start at 0 for the first weapon**

- **If the first index you specify is higher than 0, it will also be used for the indexes before it.**

### $Custom data:

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg) Defines a set of custom data that for this weapon class, which can then be accessed through Lua scripts. There can be any amount of +Val: entries. It requires a $end_custom_data entry at the end.
Check the scripting documentation of Weapon Class to find the CustomData Lua Table.

#### +Val:

Specifies a custom data value and its name, in the format

+Val: Name Value

These values will always be strings of text. If you use them in Lua as other types of value, you might need to convert them in the script.

#### $end_custom_data

Mandatory entry to indicate that the Custom Data list has ended.


### $Score:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Optional. The score for destroying this weapon (for weapons that can be shot down).
- Syntax: ***Integer***

### $Custom Strings

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- Defines a set of custom strings for this weapon class, which can then be accessed through Lua scripts. There can be any amount of entries. It requires a $end_custom_strings entry at the end. Note: no colon after `$Custom Strings`.

#### $Name:

- Specifies a custom string name
- Syntax: **String**

#### +Value:

- Required. A "value" or additional data point paired with the string. A common use case is an XSTR ID.
- Syntax: **String**

#### +String:

- Required. The string itself, a multitext string ending with $end_multi_text.
- Syntax: **String**

## Weapons.tbl Flags

### "Electronics"

- Will seize up the weapons and engines systems of any ship for a short period of time.

### "Spawn *Name*,*Value*"

- Will spawn the specified number of defined missiles upon detonation.
- The flag will parse without a value, using a default of 10.

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Multiple spawn types can be defined for a single 'carrier' weapon up to maximum of 5 different weapons

### "Remote Detonate"

- Can be triggered remotely via a second press on the trigger

### "Big Ship"

- These weapons cannot be mounted on small ships like fighters, bombers, sentry guns etc.

### "Huge"

- Allows this weapon to do the final 10% damage to a ship with the "Big Damage" flag. Also the AI will not try to hit fighters with it, but only if the weapon is in a turret or is a secondary. If the weapon is a primary in a fighter then there are no targeting restrictions. See [capital+](#capital) instead.

### "Supercap"

- Allows this weapon to do the final 75% damage to a ship with the "supercap" flag.

### "Bomber+"

- The AI will prefer to use this weapon against bombers

### "child"

- Cannot be set as a weapon on any ship, but is instead a child of a weapon. Usually defined with "Spawn *Name*,*Value*"

### "Bomb"

- Makes this weapon target-able with the B key and interceptable by AIs
  - Has been long thought to make the AI prefer this weapon against large ships, but in fact has no effect on AI weapon selection.

### "No Dumbfire"

- Missile will not fire without a lock

### "in tech database"

- Has the weapon in the tech database at the start of the campaign

### "Player allowed"

- Allows this weapon as a player weapon.

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- **EXTREMELY IMPORTANT NOTE:**
  - Whenever you plan to use a weapon as a player one in your campaign or missions, **USE THIS FLAG**.
  - Not using it can cause problems within fs_open and it's a BAD MOD DESIGN.
  - In the future, fs_open will maybe be able to avoid these problems but, nevertheless, it's still a BAD DESIGN.
  - As another possible example: be careful to update "retail" tables if you plan to use Shivan weapons in player fighters.

### "Particle Spew"

- Makes the weapon projectile spew out particles

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Can be defined with [Particle Spew parameters](#pspew)

### "EMP"

- This weapon will have an EMP effect on any ship within the blast radius
- Can be defined with [EMP Intensity](#emp-intensity) and [EMP Time](#emp-time)

### "Esuck"

- Drains the target's energy banks
- Can be defined with [Leech Weapon](#leech-weapon) and [Leech Afterburner](#leech-afterburner)

### "Vampiric"

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Heals shooter upon dealing hull damage to target
- Can be defined with [Vampiric Healing Factor](#vampiric-healing-factor)

### "Flak"

- Makes this weapon a [flak gun](#flak-guns)

### "Corkscrew"

- Makes this weapon spiral AND swarm 4.

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Can be defined with [Corkscrew parameters](#corkscrew)

### "Shudder"

- Makes your ship shudder while you're firing the weapon

### "lockarm"

- Requires a target lock in order to arm its warhead. In other words weapon causes less damage if it hits the target without target lock.

### "beam"

- Makes this weapon a [beam cannon](#beam-cannons)

### "stream"

- Does nothing

### "puncture"

- Reduces hull damage by 75%
The AI will prefer these weapons for anti-subsystem use

### "countermeasure"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the weapon as a countermeasure

### "Ballistic"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the weapon as a ballistic weapon that needs ammunition.
- Make sure you define the size and reload rate for ballistic weapons

### "pierce shields"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Causes weapons to penetrate shield without causing damage to it.

### "no pierce shields"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Prevents weapons (beams) from piercing shields causing shields to be damaged by weapons first.

### "local ssm"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

### "tagged only"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Weapon can only be fired at tagged targets

### "beam no whack"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Disables beam whacking (push). Practically same as setting beam mass to zero.

### "cycle"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Causes weapon to alternate between firing points

### "small only"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Causes AI to use the weapon only on small targets (fighters, bombers etc.) Only applies to the target selection of turrets, has no effect on the behavior of fighters picking primary and secondary bank weapons.

### "same turret cooldown"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Forces the turrets to fire on the designated rate of fire on all difficulty levels and all AI levels

### "apply no light"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Prevents light from being applied to the weapon model.

### "training"

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Sets weapon to be of training weapon type. ?

### "smart spawn"

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Forces turret fired spawn weapon of this type to be fired like normal weapons.

### "inherit parent target"

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Sets spawned 'child' weapons to use parent weapon's target.

### "no emp kill"

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the weapon (with hitpoints) to be immune to automatic EMP destruction.

### "untargeted heat seeker"

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Defines the weapon as a heatseeker type missile which acquires its target by itself.

### "no radius doubling"

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Prevents weapon radius from being doubled when doing collision test. Makes bombs harder to intercept.

### "no subsystem homing"

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Prevents weapon from acquiring a subsystem as its target. Missile homing points are spread over the target ship.

### "no lifeleft penalty"

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Prevents weapon lifetime from being penalized if the angle to the target is too big.

### "can be targeted"

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Allows weapon to be targeted by player. Does not influence AI

### "show on radar"

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Allows weapon to be visible on the radar

### "show friendly on radar"

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Allows friendly, otherwise visible to radar weapon, to be visible on player radar

### "hide from radar"

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Prevents weapon from being visible on the radar

### "capital+"

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Tells AI fighters/bombers to use this weapon only on capital ships. Has no effect on turret AI.

### "chain external model fps"

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Tells game to keep switching external model firingpoints when firing

### "external model launcher"

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Tells game that the external model is a launcher so that it would be kept static without loading motion

### "takes blast damage"

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Weapon takes damage from weapon blasts

### "takes shockwave damage"

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- Weapon takes damage from shockwaves

### "ciws"

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- Weapon can deal shockwave/blast damage to bombs, regardless of whether the target weapons have the "take blast/shockwave damage" flags.

### "render flak"

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- Normally, flak shells are not rendered. This flag switches rendering on for those weapons.

### "anti-subsystem beam"

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- Beam weapons with this flag can target subsystems on enemy ships, just like normal weapons.

### "no primary linking"

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- Primary weapons with this flag can not be linked with other weapons.

### "same emp time for capships"

![FSO 3.7](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.svg)

- capships affected by this weapon will use the tabled EMP time rather than the fixed retail value (7.5 secs)

### "no primary linked penalty"

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Primary weapons with this flag do not incur the retail linked fire penalty

### "no homing speed ramp"

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Locked homing weapons will not perform the standard 1 second long acceleration when fired. Note that use of $Acceleration Time is **not** affected by this flag.

### "pulls aspect seekers"

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- If an aspect seeker is grabbed by this countermeasure the missile will follow the countermeasure instead of going into dumbfire mode.
**Only valid for countermeasures!**

### "turret interceptable"

### "fighter interceptable"

### "interceptable"

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Allows turrets/fighters/both (respectively) to shoot at this weapon without requiring it to be a bomb. If this is the first time this weapon has been defined, it also gives it a default of 25 hitpoints so that it can be shot down.
**If you use a TBM to give this flag to an existing weapon, you'll need to specify hitpoints manually (unless you want it to be unkillable but still draw defensive fire).**

### "aoe electronics"

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Apply electronics effect across the weapon's entire area of effect instead of just on the impacted ship

### "apply recoil"

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- A ship that fires a weapon with this flag will experience a recoil effect, slowing the ship down and, with lighter strikecraft, inducing some rotation if the firing points are offset from the center of mass.

### "don't spawn if shot"

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Prevents parent weapons from spawning children in the even of the parent weapon being shot down.

### "die on lost lock"

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- Aspect seeking missiles (only, not heat seekers) will detonate upon losing their lock. Currently they only lose lock if decoyed by a countermeasure, AND "pulls aspect seekers" or similar is not set. The detonation will use their "dinky" explosion (i.e. same as if ran out of lifetime).

### "no doublefire"

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Prevents dual-firing of secondary weapons. If the ship tries to dual-fire a weapon with this flag they are set back to single fire.

### "no impact spew"

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Turns off the usual spew effects that occur on the impacted ship when a weapon hits it.

### "require exact los"

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- Prevent AI form firing this weapon from a turret or as a secondary weapon if there is no clear line of sight from the weapons firing point to its targeted ship (center). Note that this may keep the weapon trained on a target out of sight while another target may be available to shoot.

### "can damage shooter"

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

- This weapon can collide with and directly damage the original shooter of this weapon (even if it was spawned), which is not normally possible.

### "heals"

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- This weapon uses its $Damage value to heal its target(s) instead of damaging them.
- This works very similarly to damage, the armor/shield/subsystem factors apply to healing as well as do armor classes. However, there are some caveats.
- Healing will *always* carry. Unlike damage, which first has to go through the shield, has damage subtracted, then damages subsystems, has damage subtracted, then finally damages the hull if any damage remains, healing will apply in full to hull, shield, and subsystems (factors or armor classes notwithstanding). A weapon with 100 'healing' will attempt to apply that healing to the hull, shield and nearby subsystems to the impact. Even if the weapon only heals hull, it will still do so even if it strikes the shield.
- This flag has ***no effect*** on AI. Fighters armed with a healing weapon will happily heal hostile targets they were ordered to attack.

### "secondary no ammo"

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

- If this is a secondary weapon, it does not need to consume any ammo, and will not display ammo in the weapon HUD gauge, etc.

### "no collide"

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

- This weapon will not collide with any other object.

### "multilock target dead subsys"

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

- If this is a multilock weapon, dead subsystems will be treated as just as valid targets as alive ones.

### "no evasion"

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

- AI will not attempt to evade this weapon when fired at them

### "no_fred"

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Weapon will not appear in any of the FRED dropdown lists for weapon classes.

### "detonate on expiration"

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- Primary weapon will detonate as if hitting a target when it reaches the end of its lifetime - just like a secondary weapon does by default.

### "freespace 1 missile behavior"

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Enables a set of behaviors observed from missiles in the Freespace 1 release.
  - Detonate when within 50 meters and not pointing at their target.
  - Move slower than normal when not pointing at their target.
  - Aspect seekers follow a -1x lag pursuit scalar for the last second before hitting their target.
- Note that the last point will make aspect seekers much less likely to hit moving targets.
- [Can be applied globally using an AI profile](mission_profiles.tbl#enable-freespace-1-style-missile-behavior)
- Syntax: ***Boolean***


### "default player weapon"

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- The weapon is in the player's weapon pool by default.

### "don't merge lead indicators"

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Keeps this weapon's lead indicator separate on the HUD instead of merging it with the other weapons'.

### "ignores countermeasures"

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- Homing weapons with this flag aren't distracted by countermeasures.

### "dogfight variant"

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Marks the weapon as a multiplayer dogfight variant of another weapon, so it is skipped where the game lists the normal player weapons.

### "mine"

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- Makes the weapon a mine; see [$MineInfo:](#mineinfo).

## Countermeasures

- Usually not altered. Only one can exist

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- More than one can exist

### $Name:

- Name of the countermeasure
- Syntax: ***String***, name

### $Velocity:

- Defines the speed of the countermeasure, relative to the ship.
- Syntax: ***Float***, meter per second

### $Fire Wait:

- Definies the minumum time between the countermeasure launches
- Syntax: ***Float***, seconds

### Lifetime

- Countermeasure's life time is defined between with minimum and maximum lifetime
- **$Lifetime Min:**
  - Syntax: ***Float***, seconds
- **$Lifetime Max:**
  - Syntax: ***Float***, seconds

### $LaunchSnd:

- Defines the sound for the countermeasure launch
- Syntax: ***Integer*** or ***String***, name or identification number of the sound as defined in [sounds.tbl](sounds.tbl)

### $Model:

- Defines the used model file (.pof)
- Syntax: ***String.pof***, model filename

## Player Weapon Precedence

- First the primary weapons are defined, from most powerful to least powerful, and then the same for secondaries
- Syntax: ( ***"String" "String"***), names of the weapons
- Example:

```
$Player Weapon Precedence: (
     "UD-8 Kayser"
     "Prometheus R"
     "Subach HL-7"
     "Harpoon"
     "Tempest"
     "Rockeye"   )
```

## Sample

```
#Primary Weapons

$Name:                            @Subach HL-7
   +Title:                           XSTR("GTW Subach HL-7", 3243)
   +Description:
XSTR( "Standard Issue
Level 3 Hull Damage
Level 2 Shield Damage", 3244)
$end_multi_text
   +Tech Title:                      XSTR("Subach HL-7", 146)
   +Tech Anim:                       Tech_Subach_HL-7
   +Tech Description:
XSTR(
"Descriptive text...", -1)
$end_multi_text
$Model File:                      none
   @Laser Bitmap:                    newglo9
   @Laser Glow:                      2_laserglow03
   @Laser Color:                     250, 0, 0
   @Laser Color2:                    0, 0, 250
   @Laser Length:                    10.0
   @Laser Head Radius:               0.90
   @Laser Tail Radius:               0.90
$Mass:                            0.2
$Velocity:                        450.0
$Fire Wait:                       0.2
$Damage:                          15
$Armor Factor:                    0.9
$Shield Factor:                   0.7
$Subsystem Factor:                0.3
$Lifetime:                        2.0
$Energy Consumed:                 0.20
$Cargo Size:                      0.0
$Homing:                          NO
$LaunchSnd:                       76
$ImpactSnd:                       85
$Flags:                           (  "in tech database"
                                     "player allowed"
                                     "stream")
$Icon:                            iconSD4
$Anim:                            SD4
$Impact Explosion:                none

#End

#Secondary Weapons

$Name:                            EMP Adv.
   +Title:                           XSTR("GTM-14 EMP Adv.", 3396)
   +Description:
XSTR( "Advanced Missile
Electromagnetic Pulse Weapon
General Purpose Suppression", 3397)
$end_multi_text
   +Tech Title:                      XSTR("GTM-14 EM Pulse Adv.", 3398)
   +Tech Anim:                       Tech_EMP
   +Tech Description:
XSTR(
"Descriptive text...", -1)
$end_multi_text
$Model File:                      EMPulse2.pof
$Mass:                            15.0
$Velocity:                        275.0
$Fire Wait:                       2.0
$Damage:                          45
$Blast Force:                     20.0
$Inner Radius:                    80.0
$Outer Radius:                    300.0
$Shockwave Speed:                 120.0
$Armor Factor:                    1.0
$Shield Factor:                   0.8
$Subsystem Factor:                0.5
$Lifetime:                        5.0
$Energy Consumed:                 0.0
$Cargo Size:                      4.0
$Homing:                          YES
   +Type:                            ASPECT
   +Turn Time:                       1.0
   +Min Lock Time:                   2.0
   +Lock Pixels/Sec:                 70
   +Catch-up Pixels/Sec:             100
   +Catch-up Penalty:                30
$LaunchSnd:                       118
$ImpactSnd:                       88
$FlyBySnd:                        -1
$Rearm Rate:                      2.0
$Flags:                           (  "player allowed"
                                     "EMP")
$Trail:
   +Start Width:                     0.25
   +End Width:                       0.75
   +Start Alpha:                     1.0
   +End Alpha:                       0.0
   +Max Life:                        1.5
   +Bitmap:                          MissileTrail04
$Icon:                            iconEMPulse
$Anim:                            ecm
$Impact Explosion:                ExpMissileHit1
$Impact Explosion Radius:         10.0
$EMP Intensity:                   375.0
$EMP Time:                        20.0

#End

$Player Weapon Precedence: (
     "Subach HL-7"
     "EMP Adv."   )
```
