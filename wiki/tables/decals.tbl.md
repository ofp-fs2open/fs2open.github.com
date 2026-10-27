> **Revision information**
> OFP Revision: [#116](https://github.com/ofp-fs2open/fs2open.github.com/pull/116)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

Decal tables define decals: images drawn onto a model's surface, such as weapon impact marks, that stay for a while or for good.

There is no decals.tbl itself: decals are only read from modular tables named **xxx-dcl.tbm**.

Decals need deferred lighting. On a system without it, the decal system turns itself off and the log says so. Players can also turn decals off in the graphics options.

Code: [/decals/decals.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/decals/decals.cpp). See [OFP Tables](OFP-Tables) for every table.

Badges show the version a field or behavior was added in.

## Contents

1. [Format](#format)
2. [$Decal:](#decal)
3. [Using decals](#using-decals)
4. [Sample](#sample)

---

## Format

- Each file has one section, starting with `#Decals` and ending with `#End`.
- Each decal starts with `$Decal:`. Its fields must be in the order listed here.

---

## $Decal:

- Starts a decal definition. Weapons refer to it by this name.
- Names are case-sensitive and aren't checked for duplicates: if two decals share a name, the first one is used. A later table can't change an existing decal.
- Syntax: ***String***

### +Diffuse:

- The diffuse (color) map. Can be an animation.
- Syntax: ***String***, file name without an extension. A name that isn't a valid file name is an error and is ignored; one that fails to load logs a warning.

#### +Loop:

- Whether the animation loops.
- Default: YES
- Syntax: ***Boolean***

### +Glow:

- The glow map. Can be an animation.
- Syntax: ***String***, file name without an extension, as for `+Diffuse:`.

#### +Loop:

- Whether the animation loops.
- Default: YES
- Syntax: ***Boolean***

### +Normal:

- The normal map. Can be an animation.
- Syntax: ***String***, file name without an extension, as for `+Diffuse:`.

#### +Loop:

- Whether the animation loops.
- Default: YES
- Syntax: ***Boolean***

---

## Using decals

A weapon in [weapons.tbl](weapons.tbl) leaves a decal where it hits with `$Impact Decal:`:

### $Impact Decal:

- The name of a decal defined here. An unknown name is an error.

#### +Radius:

- The size of the decal. Required for a new weapon.
- Syntax: ***Float***, meters, or a random range.

#### +Lifetime:

- How long the decal stays. Required for a new weapon.
- Syntax: `Eternal` to keep it for good, or ***Float***, seconds, or a random range.

#### +Use Random Rotation:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Rotates each decal randomly, so repeated impacts don't all look the same.
- Syntax: ***Boolean***

---

## Sample

```
#Decals

$Decal: Scorch
	+Diffuse: scorch_d
	+Glow: scorch_g
		+Loop: NO
	+Normal: scorch_n

#End
```

In weapons.tbl:

```
$Impact Decal: Scorch
	+Radius: 4
	+Lifetime: 30
	+Use Random Rotation: YES
```
