> **Revision information**
> OFP Revision: [#143](https://github.com/ofp-fs2open/fs2open.github.com/pull/143)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

The **species_defs.tbl** defines the species ships belong to (Terran, Vasudan and Shivan in the original game), and what each one uses: briefing icons, thruster glows, shield hit animations, debris textures and mission editor colors. Any new species added here must also be added to [icons.tbl](icons.tbl) and [sounds.tbl](sounds.tbl) (or borrow from another species, below) if the game is to work. ![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg) It is no longer necessary to add species to [asteroid.tbl](asteroid.tbl).

The engine has a built-in version of this table with the original game's species, used when no species_defs.tbl is found.

This table is one of the Modular Tables and can be extended with **xxx-sdf.tbm**.

Code: [/species_defs/species_defs.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/species_defs/species_defs.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in.

## Contents

1. [Format](#format)
2. [Species Entry](#species-entry)
3. [Sample Entry](#sample-entry)

---

## Format

- The table begins with **#SPECIES DEFS** and ends with **#END**.
- An old `$NumSpecies:` line after `#SPECIES DEFS` is still accepted and ignored; the species are counted automatically.
- Each species starts with `$Species_Name:`. Its fields must be in the order listed here.
- The order of the species matters: other tables (such as [icons.tbl](icons.tbl)) refer to species by position, and a species can only borrow from one defined before it.

## Species Entry

### $Species_Name:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the name of the species.
- A name used twice is not checked: both species are added, and the first one is the one found by name. Use `+nocreate` to edit a species.
- Syntax: **String**
  - Example: *Shivan*

### +nocreate

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Optional, directly after the name. Edits the existing species of that name instead of adding a new one; only the fields given are changed, and the required fields become optional. If no species has that name, the whole entry is ignored.

### $Default IFF:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the species default IFF in FRED.
- Required for a new species: without it, a warning is logged and the first IFF in iff_defs.tbl is used. An IFF that doesn't exist logs a warning and the first IFF is used.
- Syntax: **String**, name of the IFF, either as default IFF types or as define in [iff_defs.tbl](iff_defs.tbl)
  - Example: *Hostile*

### $FRED Color: or $FRED Colour:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the color used in FRED for this species
- If not defined, a warning is logged and the color defaults to the original game's color for Terran, Vasudan, Shivan and Ancient(s), and to black ( 0, 0, 0 ) for any other species
- Syntax: ( **Color** ), three **integers**, 0 to 255, red, green and blue respectively
  - Example: ( 255, 0, 0 )

### +Debris_Texture:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Required for a new species. (An old `$MiscAnims:` line before it is accepted and ignored.)
- Defines the name of the standard texture applied to particle-like impact-effect and 'vaporized' ship debris.
- Syntax: **String**, filename
  - Example: *debris01c*

### +Shield_Hit_ani:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the name of the ani file projected to a species shield mesh when hit.
- Syntax: **String**, filename
  - Example: *shivanshield*

> **Note:** The animations fps value is ignored, instead any animation plays for 0.75 secs with all frames playing at equal intervals. Additionally, only the first 75% of the animations frames are currently used (a bug!)

> **Note:** The retail shieldhit01a.ani has 21 frames and 30 fps, which if it were not ignored would mean a playtime of 0.7secs

### $ThrustAnims:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Required for a new species, along with its `+Normal:` and `+Afterburn:`; the other lines are optional.

- **+Normal:** or **+Pri_Normal:**
  - Defines name of standard thruster animation.
  - This maps the image onto the mesh thrusters.
  - Syntax: **String**, filename or *<none>*
    - Example: *thruster03*
- **+Afterburn:** or **+Pri_Afterburn:**
  - Defines name of afterburn thruster animation.
  - This maps the image onto the mesh thrusters.
  - Syntax: **String**, filename or *<none>*
    - Example: *thruster03a*
- **+Sec_Normal:**
  - Defines name of secondary standard thruster animation.
  - This is rendered as a plane, the image is centered but on the left edge of the image, and located at each of the engines. This causes the image to flicker in size. it also grows and shrinks depending on the thrust.
  - Syntax: **String**, filename
    - Example: *thruster02-03*
- **+Sec_Afterburn:**
  - Defines name of secondary afterburn thruster animation.
  - This is rendered as a plane, the image is centered but on the left edge of the image, and located at each of the engines. This causes the image to flicker in size. it also grows and shrinks depending on the thrust.
  - Syntax: **String**, filename
    - Example: *thruster02-03a*
- **+Ter_Normal:**
  - Defines name of tertiary standard thruster animation.
  - This is rendered as a halo, the image is centered, and located at each of the engines. This will always be facing the player directly. Used for an engine glow that flickers in size.
  - Syntax: **String**, filename
    - Example: *thruster03-03*
- **+Ter_Afterburn:**
  - Defines name of tertiary afterburn thruster animation.
  - This is rendered as a halo, the image is centered, and located at each of the engines. This will always be facing the player directly. Used for an engine glow that flickers.
  - Syntax: **String**, filename
    - Example: *thruster03-03a*

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- **+Dist_Normal:**
  - Defines name of normal thruster distortion envelope animation.
  - This is the alpha envelope of the thruster distortion. It will take the brightness of the texture multiply it by 10 and use that as the alpha value for the distortion effect.
  - Syntax: **String**, filename
    - Example: *thruster02-03*
- **+Dist_Afterburn:**
  - Defines name of afterburner thruster distortion envelope animation.
  - This is the alpha envelope of the thruster distortion. It will take the brightness of the texture multiply it by 10 and use that as the alpha value for the distortion effect.
  - Syntax: **String**, filename
    - Example: *thruster02-03a*

### $ThrustGlows:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Required for a new species, along with its `+Normal:` and `+Afterburn:`.

- **+Normal:**
  - Defines name of standard thruster glow bitmap.
  - Syntax: **String**, filename or *<none>*
    - Example: *thrusterglow03*
- **+Afterburn:**
  - Defines name of standard thruster glow bitmap.
  - Syntax: **String**, filename or *<none>*
    - Example: *thrusterglow03a*

### $AwacsMultiplier:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- Defines the awacs multiplier.  All ships belonging to the species have their awacs range multiplied by this number.
- If not defined, a warning is logged and the original game's value is used: 1.25 for Vasudan, 1.5 for Shivan, 1.0 for any other species.
- Syntax: **Float**, (multiplier)
  - Example: *1.25*

### $Countermeasure Type:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Works just like the $Countermeasure Type: on a ship class basis.  If you specify one for the species, you won't have to specify one for each ship class of that species.

### $Support ship:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- The support ship class this species uses: ships of the species call this class for rearm and repair.
- A ship class that doesn't exist logs a warning.
- Syntax: **String**, ship class name

### $Borrows Briefing Icons from:

![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg)

- Defines this species to borrow all of its briefing icons from another species, instead of being listed in [icons.tbl](icons.tbl) or xxx-ico.tbm
- NOTE: The species listed for this must have already been defined before the species being currently defined; otherwise a warning is logged.
- NOTE: Briefing icons assigned in this way *cannot* be modified by modular tables.
- Syntax: **String**, Species

### $Borrows Flyby Sounds from:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Defines this species to borrow all of its flyby sounds from another species, instead of being listed in [sounds.tbl](sounds.tbl) or xxx-snd.tbm
- NOTE: The species listed for this must have already been defined before the species being currently defined; otherwise a warning is logged.
- NOTE: Flyby sounds assigned in this way *cannot* be modified by modular tables.
- Syntax: **String**, Species

### Warp settings

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- `$Warpin Type:`, `$Warpin Start Sound:`, `$Warpin End Sound:`, `$Warpin Speed:`, `$Warpin Time:`, `$Warpin Decel Exp:`, `$Warpin Radius:`, `$Warpin Animation:`, and the same `$Warpout` fields (`$Warpout Type:`, `$Warpout Start Sound:`, `$Warpout End Sound:`, `$Warpout Engage Time:`, `$Warpout Speed:`, `$Warpout Time:`, `$Warpout Accel Exp:`, `$Warpout Radius:`, `$Warpout Animation:`).
- They work exactly as the fields of the same names in [ships.tbl](ships.tbl#warpin-type), and set the default warp effect for every ship class of this species. A ship class's own warp fields override them.

## Sample Entry

```
#SPECIES DEFS

;------------------------
; Terran
;------------------------
$Species_Name: Terran
$Default IFF: Friendly
$FRED Color: ( 0, 0, 192 )
$MiscAnims:
   +Debris_Texture: debris01a
   +Shield_Hit_ani: shieldhit01a
$ThrustAnims:
   +Normal: thruster01
   +Afterburn: thruster01a
$ThrustGlows:
   +Normal: thrusterglow01
   +Afterburn: thrusterglow01a
$AwacsMultiplier: 1.00

;------------------------
; Vasudan
;------------------------
$Species_Name: Vasudan
$Default IFF: Friendly
$FRED Color: ( 0, 128, 0 )
$MiscAnims:
   +Debris_Texture: debris01b
   +Shield_Hit_ani: shieldhit01a
$ThrustAnims:
   +Normal: thruster02
   +Afterburn: thruster02a
$ThrustGlows:
   +Normal: thrusterglow02
   +Afterburn: thrusterglow02a
$AwacsMultiplier: 1.25

;------------------------
; Shivan
;------------------------
$Species_Name: Shivan
$Default IFF: Hostile
$FRED Color: ( 192, 0, 0 )
$MiscAnims:
   +Debris_Texture: debris01c
   +Shield_Hit_ani: shieldhit01a
$ThrustAnims:
   +Normal: thruster03
   +Afterburn: thruster03a
$ThrustGlows:
   +Normal: thrusterglow03
   +Afterburn: thrusterglow03a
$AwacsMultiplier: 1.50

#END
```
