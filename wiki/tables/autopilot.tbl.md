> **Revision information**
> OFP Revision: [#109](https://github.com/ofp-fs2open/fs2open.github.com/pull/109)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

The **autopilot.tbl** sets how the autopilot behaves: how close ships must be to link, what interrupts it, and the messages it shows. This table is not in the original game.

The engine has a built-in autopilot.tbl, used when the mod has none. It sets `$Link Distance:` to 1000 and gives every message below its default text.

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg) This table is one of the Modular Tables and can be extended with **xxx-aplt.tbm**. Each modular table changes only the settings and messages it includes.

Code: [/autopilot/autopilot.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/autopilot/autopilot.cpp). See [OFP Tables](OFP-Tables) for every table.

Badges show the version a field or behavior was added in.

## Contents

1. [Format](#format)
2. [Settings](#settings)
3. [Messages](#messages)
4. [Example](#example)

---

## Format

- The table starts with `#Autopilot` and ends with `#END`.
- Everything in between is optional, but must be in the order listed on this page. A setting out of order is not read.

---

## Settings

### $Link Distance:

- How close another ship must be to the player for its autopilot to link, so it flies along with the player. See NAV_NEEDS_LINK in the mission editor.
- Default: 1000
- Syntax: ***Integer***, meters

### $Interrupt autopilot if enemy within distance:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- The autopilot won't engage with an enemy this close to the player, and stops if an enemy comes this close to the autopilot flight leader.
- Default: 5000
- Syntax: ***Integer***, meters

### $Interrupt autopilot if asteroid within distance:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- The autopilot won't engage with an asteroid this close to the player, and stops if an asteroid comes this close to the autopilot flight leader.
- Default: 1000
- Syntax: ***Integer***, meters

### $Lock Weapons During Autopilot:

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

- Disables the weapons of every ship taking part in the cinematic autopilot sequence.
- Default: NO
- Syntax: ***Boolean***

### +No_Cutscene_Bars

- Turns the cutscene bars off while the autopilot runs.

### +No_Autopilot_Interrupt

- The autopilot ignores interruptions.

---

## Messages

Each message sets the text shown, and a sound, for one autopilot event. A message whose text is empty or `none` shows nothing. Like the settings, they must be in this order:

| Message | Shown when |
|---|---|
| `$No Nav Selected:` | The autopilot is turned on with no navpoint selected. |
| `$Gliding:` | The autopilot is turned on while gliding. |
| `$Too Close:` | The selected navpoint is too close. |
| `$Hostiles:` | An enemy is within `$Interrupt autopilot if enemy within distance:` of the player. |
| `$Linked:` | Another ship links to the player's autopilot by coming within `$Link Distance:`. |
| `$Hazard:` | An asteroid is within `$Interrupt autopilot if asteroid within distance:` of the player. |
| `$Support Present:` ![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg) | A friendly support ship is in range and the engine can't dismiss it. |
| `$Support Working:` ![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg) | A friendly support ship in range has a repair or rearm order still to finish. |

Each message needs both of these:

### +Msg:

- The text shown on screen, for five seconds.
- Syntax: ***String***

### +Snd File:

- The sound to play with the message, or `none`.
- Syntax: ***String***, file name with extension
- **Known issue:** these sounds currently never play. The engine only tries to open the sound when the name *is* `none`, the reverse of what it means to check, so a real file name is never opened.

---

## Example

```
#Autopilot

$Link Distance: 1000
$Interrupt autopilot if enemy within distance: 4000
$Lock Weapons During Autopilot: YES

$No Nav Selected:
	+Msg: Cannot engage autopilot, no navpoint selected.
	+Snd File: none
$Hostiles:
	+Msg: Cannot engage autopilot: hostile craft near.
	+Snd File: none

#END
```
