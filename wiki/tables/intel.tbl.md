> **Revision information**
> OFP Revision: [#144](https://github.com/ofp-fs2open/fs2open.github.com/pull/144)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **intel.tbl** defines the **intelligence** entries found in the tech room.

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) **intel.tbl** is the current name of this table. Its original name, **species.tbl**, is deprecated but still read: the game loads intel.tbl if it exists, and species.tbl otherwise (never both). Species themselves are defined in [species_defs.tbl](species_defs.tbl).

This table is one of the Modular Tables and can be extended with **xxx-intl.tbm**.

Code: [/menuui/techmenu.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/menuui/techmenu.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in. Fields without a badge are from the original game. The OFP badge marks what OFP adds.

## Contents

1. [Format and Info](#format-and-info)
2. [Intelligence Entry](#intelligence-entry)
3. [Sample](#sample)

---

## Format and Info

- Table consists of the intelligence data entries, each one starting with **$Entry:**
- In the original game the table ***does not*** have any kind of begin or end marker. ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) It can begin with `#Intel` and end with `#End`; both are optional.
- The fields of an entry must be in the order listed here. Anything else where the next `$Entry:` should be logs a warning, and the rest of the file is not read.
- ![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) There is no limit on the number of entries (it was 75), and no limit on the length of a description (it was 5120 characters).

## Intelligence Entry

### $Entry:

- Begins the intelligence data entry

### $Name:

- Required. Defines the name of the intelligence entry. Number refers to the translations found from [tstrings.tbl](tstrings.tbl)
- A name used again without `+nocreate` logs a warning, and the entry is added anyway.
- Syntax: XSTR("**String**", **Integer**)

### +nocreate

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Optional, directly after `$Name:`. In a modular table, edits the existing entry of that name instead of adding a new one; only the fields given are changed. If no entry has that name, the entry is skipped (noted in the log).
- In intel.tbl itself it logs a warning and is ignored (a new entry is added).

### $Anim:

- Defines the name of the animation used for the intelligence entry
- Syntax: **String**

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- This field is now optional

### $Tech Model:

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg)

- The model file to show for this intel entry, as a spinning 3D model the way ship and weapon entries do.
- If the model loads, it is shown **instead of** the entry's `$Anim:`. If it fails to load, the tech room falls back to the animation.
- **Syntax:** *String*, model filename (e.g. `mystation.pof`)

#### +Closeup_pos:

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg)

- The camera position used to view the model, the same as `$Closeup_pos:` in [ships.tbl](ships.tbl).
- Only read directly after `$Tech Model:`.
- If left out, it is worked out from the model's size: the camera sits on the z axis at twice the model's largest z extent.
- **Syntax:** *Vector*, `x, y, z`

#### +Closeup_zoom:

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg)

- The camera zoom used to view the model, the same as `$Closeup_zoom:` in [ships.tbl](ships.tbl).
- Only read directly after `$Tech Model:` (and `+Closeup_pos:` if present).
- Must be greater than 0. Otherwise a message is logged and the default is used.
- **Default:** `0.5`
- **Syntax:** *Float*

### $AlwaysInTechRoom:

- Sets the entry to appear from the beginning of campaigns. If set to '0' or false the entry has to be triggered with sexps during the campaign
- Syntax: **Integer** (0 or 1)

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Syntax: **Boolean**
- This field is now optional, defaults to false. When a modular table edits an entry, setting it resets the entry's tech room flags first.

### $Description:

- Sets the description shown when the intelligence entry is viewed in tech room. Number refers to the translations found from [tstrings.tbl](tstrings.tbl).
- Ends with **$end_multi_text**
- Syntax: XSTR("**String**", **integer**)

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- This field is now optional

### $Custom data:

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) Optional. Defines a set of custom data for this intel entry, which can then be accessed through Lua scripts. There can be any amount of +Val: entries. It requires a $end_custom_data entry at the end.

#### +Val:

Specifies a custom data value and its name, in the format

```
+Val: Name Value
```

## Sample

- A Retail FreeSpace 2 entry

```
#Intel

$Entry:
$Name: XSTR("The Great War",3184)
$Anim: Intel_Great_War
$AlwaysInTechRoom: 1
$Description:
XSTR(
"In 2335, the Terran-Vasudan War entered its fourteenth year...", 3185)
$end_multi_text

$Entry:
$Name:        Relay Station
$Anim:        tech_relay
$Tech Model:  relaystation.pof
+Closeup_pos: 0, 0, -350
+Closeup_zoom: 0.6
$AlwaysInTechRoom: 1
$Description:
XSTR("A long-range subspace relay.", -1)
$end_multi_text

#End
```
