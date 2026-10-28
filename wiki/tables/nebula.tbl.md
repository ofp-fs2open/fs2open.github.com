> **Revision information**
> OFP Revision: [#132](https://github.com/ofp-fs2open/fs2open.github.com/pull/132)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **nebula.tbl** is used to define the various nebula bitmaps and poofs used by full nebula missions. (The FS1-style background nebula is defined in [generated_nebula.tbl](generated_nebula.tbl) instead.)

This table is one of the Modular Tables and can be extended with **xxx-neb.tbm**.

Code: [/nebula/neb.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/nebula/neb.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in. Fields without a badge are from the original game.

## Contents

1. [Format](#format)
2. [+Nebula:](#nebula)
3. [+Poof:](#poof)
4. [$Name:](#name)
5. [Sample](#sample)

---

## Format

- Table begins with list of all bitmaps available for the nebulae
- This section ends with **#end**
- Table continues with a list of nebula poofs that can be added to nebulae
- This section also ends with **#end**

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) The bitmap section can be skipped in a modular table: start the file directly with `+Poof:` or `$Name:` (no `#end` for the skipped section).

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg) The poof section can be skipped in a modular table: end the file after the bitmap section's `#end`.

- Retail-style `+Poof:` lines and named `$Name:` poofs can be mixed in the poof section.
- Poofs are chosen by name in the mission editor (a retail-style poof's name is its file name).

## +Nebula:

- Defines the graphics used for the nebula bitmaps: the background images a full nebula mission can use.
- A bitmap or animation that can't be found logs a warning and is skipped.
- The `+Nebula:` itself is optional; a bare file name on its own line is also read.
- Syntax: **String**, file name

## +Poof:

- Defines a retail-style nebula poof: the bitmap used, with the default settings of a named poof. The poof's name is the file name.
- A bitmap or animation that can't be found logs a warning and the poof is skipped.
- Syntax: **String**, file name

## $Name:

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- Defines a named poof with more configuration options than retail poofs. Its fields must be in the order listed here.
- A name used twice in nebula.tbl is an error. In a modular table, a name that already exists edits that poof.
- Syntax: **String**

### +nocreate

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Optional. Goes directly after `$Name:`.
- In a modular table, edits the poof only if it already exists, instead of creating a new one. Using it in nebula.tbl itself logs a warning.

### $Bitmap:

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- The filename for the poof bitmap. Required for a new named poof, optional when editing one.
- A bitmap or animation that can't be found logs a warning (the poof is kept).
- Syntax: ***String***, filename

### $Scale:

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- The radius of a spawned named poof.
- Syntax: **(Float)** or **(Float Float)**, meters, will pick a random value between the two specified values, or the value if only one is specified

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- Syntax is now a [Parsed random range](https://wiki.hard-light.net/index.php/Parsed_random_range).

- Default value: (175)
- Values are limited to 0.01 to 100000.

### $Density:

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- On average, how much space in every dimension between two named poofs.
Note: This is scaled depending on how many poofs are active, as to not change total perceived density regardless of how many poofs are active
- Syntax: **Float**, meters
- Default value: 110
- A value of 0 or less logs a warning and 150 is used.

### $Alignment:

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- This represents a directional vector which will cause the poofs to align themselves in the specified direction. With VERTICAL, all the poofs will point the top of their bitmap in the global up direction.
- If omitted, there will be no alignment and orientations will be random.
- Syntax: **String**, "VERTICAL"; anything else logs a warning and is ignored

### $Rotation:

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- How much the spawned named poof will rotate over time.
- Syntax: **(Float)** or **(Float Float)**, degrees per second, will pick a random value between the two specified values, or the value if only one is specified

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- Syntax is now a [Parsed random range](https://wiki.hard-light.net/index.php/Parsed_random_range).

- Default value: (-3.7, 3.7)
- Values are limited to -1000 to 1000.

### $View Distance:

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- The maximum distance at which this named poof can be visible
- Syntax: **Float**, meters
- Default value: 250
- A negative value logs a warning and 360 is used.
- If the view distance and density would put more than 100,000 poofs on the field at once, a warning is logged: the count grows with the cube of the view distance divided by `$Density:`.

### $Alpha:

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg)

- The opacity of the spawned named poof.
- Syntax: **(Float)** or **(Float Float)**, will pick a random value between the two specified values, or the value if only one is specified

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- Syntax is now a [Parsed random range](https://wiki.hard-light.net/index.php/Parsed_random_range).

- Default value: (0.8)
- Values are limited to 0 to 1.

## Sample

- Retail FreeSpace 2 table
```
+Nebula:			nbackblue1
+Nebula:			nbackblue2
+Nebula:			nbackcyan
+Nebula:			nbackgreen
+Nebula:			nbackpurp1
+Nebula:			nbackpurp2
+Nebula:			nbackred
+Nebula:			nbackblack

#end

+Poof:				PoofGreen01
+Poof:				PoofGreen02
+Poof:				PoofRed01
+Poof:				PoofRed02
+Poof:				PoofPurp01
+Poof:				PoofPurp02

#end
```
