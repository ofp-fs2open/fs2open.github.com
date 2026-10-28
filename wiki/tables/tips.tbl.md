> **Revision information**
> OFP Revision: [#148](https://github.com/ofp-fs2open/fs2open.github.com/pull/148)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **tips.tbl** defines the tips that pop up in the main hall (shown to new pilots, until the player turns them off).

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) This table is one of the Modular Tables and can be extended with **xxx-tip.tbm**. Tips from modular tables are added after the ones before them.

Code: [/menuui/playermenu.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/menuui/playermenu.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in. Fields without a badge are from the original game.

## Contents

1. [General Info](#general-info)
2. [Format](#format)
3. [Sample](#sample)

---

## General Info

- These are the tips that will be displayed in a pop up menu in the mainhall.
- Key names can be written in [] brackets, as in the original tips.
- Tips can use the same tokens as training messages: a control name between `$` signs is replaced by the key bound to it.
- ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) There is no limit on the number of tips (it was 40).

## Format

- Table file consists of tips, each on a single line starting with **+Tip:**
- Table ends with **#End**

### $Start Tips at Index:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Optional, and must come before the first tip. Sets which tip is shown first when tips are displayed to the player, counting from 0 across all tip tables. To always display the first tip first, set this to 0. By default, this value is -1, which means the game picks a random tip to show to the player. In either case, when the player clicks 'Next Tip' the game moves onto the next tip in the list sequentially, or back to the beginning of the list if the last tip in the list was shown.
- An index past the last tip logs a warning and the default (random) is used.
- The last table that sets it wins.
- Syntax: ***Integer***
- Default Value: ***-1***

### +Tip:

- Entry consists of a string and a value that refers to the translation found from [tstrings.tbl](tstrings.tbl)
- Syntax: XSTR("*string*", *integer*)

## Sample

- Part of retail FreeSpace 2 table
```
+Tip: XSTR("When your shields...", 3204)
+Tip: XSTR("Press [R] to target...", 3205)
+Tip: XSTR("Press [Alt-H] to enable...", 3206)
+Tip: XSTR("Attacking the Weapons subsystem...", 3207)
```
