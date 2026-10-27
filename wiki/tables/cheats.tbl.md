> **Revision information**
> OFP Revision: [#110](https://github.com/ofp-fs2open/fs2open.github.com/pull/110)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

The **cheats.tbl** adds custom cheat codes: text the player types during a mission to show a message, run a script, or summon a ship. This table is not in the original game. The original game's own cheats still work alongside these.

This table is one of the Modular Tables and can be extended with **xxx-cht.tbm**.

Code: [/cheats_table/cheats_table.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/cheats_table/cheats_table.cpp). See [OFP Tables](OFP-Tables) for every table.

## Contents

1. [Format](#format)
2. [$Cheat:](#cheat)
3. [Using cheats in scripts](#using-cheats-in-scripts)
4. [Example](#example)

---

## Format

- The table starts with `#CUSTOM CHEATS` (optional) and ends with `#END`.
- Each cheat starts with `$Cheat:`. Its fields must be in the order listed on this page.

---

## $Cheat:

- Starts a cheat.

### +Code:

- The text the player types to use the cheat.
- Up to 17 characters. A longer code logs a warning and is cut to 17.
- A code that's already defined, in this or an earlier table, logs a warning and replaces the earlier cheat.
- A code that matches one of the original game's cheats runs both: the original cheat and this one.
- Syntax: ***String***

### +Message:

- Optional. The message shown on the HUD when the cheat is used. Shown only during a mission.
- Syntax: ***String***

### +RequireCheats:

- Optional. If YES, the cheat only works once cheats have been turned on by typing www.freespace2.com.
- Default: NO
- Syntax: ***Boolean***

### +SpawnShip:

- Optional. The cheat summons a ship near the player, during a mission. Needs both of the fields below.
- The ship warps in at a random spot within 700 meters of the player, is put on the escort list, takes every player order, and has its turrets set to fire beams freely.

#### +Name:

- The name for the summoned ship. A number is added, counting up until the name is unused: *Name* 1, *Name* 2, and so on.
- Syntax: ***String***

#### +Class:

- The ship class to summon, from [ships.tbl](ships.tbl).
- A class that doesn't exist logs a warning, and the cheat works without summoning a ship.
- Syntax: ***String***

---

## Using cheats in scripts

Every custom cheat, whether or not it has a message or ship, runs the `On Cheat` scripting hook with the code in `hv.Cheat`, so a cheat can trigger any scripted effect. See [scripting.tbl](scripting.tbl).

---

## Example

```
#CUSTOM CHEATS

$Cheat:
	+Code: arrrrwalktheplank
	+Message: Walk the plank
	+SpawnShip:
		+Name: Volition Bravos
		+Class: Volition Bravos

$Cheat:
	+Code: lightsout
	+RequireCheats: YES

#END
```
