> **Revision information**
> OFP Revision: [#3](https://github.com/ofp-fs2open/fs2open.github.com/pull/3)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **species.tbl** defines the tech room's intel entries (despite the name, species themselves are defined in species_defs.tbl). Everything in it works in OFP as documented on the FreeSpace Wiki's [species.tbl](https://wiki.hard-light.net/index.php/Species.tbl) page; this page covers only what OFP adds.

This table is one of the Modular Tables and can be extended with **xxx-intl.tbm**.

The OFP badge marks what OFP adds.

Code: [/menuui/techmenu.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/menuui/techmenu.cpp). See [OFP Tables](OFP-Tables) for every table.

## Contents

1. [Intel entry tech models](#intel-entry-tech-models)
   1. [$Tech Model:](#tech-model)
   2. [+Closeup_pos:](#closeup_pos)
   3. [+Closeup_zoom:](#closeup_zoom)

---

## Intel entry tech models

These fields go in a `#Intel` `$Entry:`, straight after `$Anim:` if the entry has one. They let a tech room intel entry show a spinning 3D model, the way ship and weapon entries do.

### $Tech Model:

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg)

- The model file to show for this intel entry.
- If the model loads, it is shown **instead of** the entry's `$Anim:`. If it fails to load, the tech room falls back to the animation.
- **Syntax:** *String*, model filename (e.g. `mystation.pof`)

### +Closeup_pos:

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg)

- The camera position used to view the model, the same as `+Closeup_pos:` in ships.tbl.
- Only read directly after `$Tech Model:`.
- If left out, it is worked out from the model's size: the camera sits on the z axis at twice the model's largest z extent.
- **Syntax:** *Vector*, `x, y, z`

### +Closeup_zoom:

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg)

- The camera zoom used to view the model, the same as `+Closeup_zoom:` in ships.tbl.
- Only read directly after `$Tech Model:` (and `+Closeup_pos:` if present).
- Must be greater than 0. Otherwise a warning is logged and the default is used.
- **Default:** `0.5`
- **Syntax:** *Float*

**Sample**

```
#Intel

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
```
