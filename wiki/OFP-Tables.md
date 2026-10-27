> **Revision information**
> OFP Revision: [#77](https://github.com/ofp-fs2open/fs2open.github.com/pull/77)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

This page lists the tables that exist only in OFP, and the fields OFP adds to existing SCP tables. Every standard FreeSpace Open table works in OFP exactly as documented on the [FreeSpace Wiki](https://wiki.hard-light.net/index.php/Tables). Only the differences are listed here.

## Contents

1. [New tables](#new-tables)
2. [Additions to existing tables](#additions-to-existing-tables)
   1. [species.tbl: Intel entry tech models](#speciestbl-intel-entry-tech-models)
   2. [props.tbl: Texture replacement](#propstbl-texture-replacement)

---

## New tables

\* Notes Modular Tables

| Table | Modular extension | Code | Purpose |
|---|---|---|---|
| [coordinate_points.tbl](coordinate_points.tbl)\* | `xxx-cps.tbm` | /coordinate_points/coordinate_shapes.cpp | Custom 2D shapes for coordinate points |
| [editor.tbl](editor.tbl)\* | `xxx-edt.tbm` | /mission/missionparse.cpp | Editor settings, and custom data fields for missions, campaigns and ships (editor only) |
| [generated_nebula.tbl](generated_nebula.tbl)\* | `xxx-gneb.tbm` | /starfield/nebula.cpp | Patterns and colors for the generated (FS1-style) background nebula |

---

## Additions to existing tables

### species.tbl: Intel entry tech models

Code: /menuui/techmenu.cpp · Modular extension: `xxx-intl.tbm`

These fields go in a `#Intel` `$Entry:`, straight after `$Anim:` if the entry has one. They let a tech room intel entry show a spinning 3D model, the way ship and weapon entries do.

#### $Tech Model:

**OFP, 26.1:**
- The model file to show for this intel entry.
- If the model loads, it is shown **instead of** the entry's `$Anim:`. If it fails to load, the tech room falls back to the animation.
- **Syntax:** *String*, model filename (e.g. `mystation.pof`)

#### +Closeup_pos:

**OFP, 26.1:**
- The camera position used to view the model, the same as `+Closeup_pos:` in ships.tbl.
- Only read directly after `$Tech Model:`.
- If left out, it is worked out from the model's size: the camera sits on the z axis at twice the model's largest z extent.
- **Syntax:** *Vector*, `x, y, z`

#### +Closeup_zoom:

**OFP, 26.1:**
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

### props.tbl: Texture replacement

Code: /prop/prop.cpp · Modular extension: `xxx-prp.tbm`

#### $Texture Replace:

**OFP, 26.1:**
- Replaces textures on the prop's model for every prop of this class. It works the same as `$Texture Replace:` in ships.tbl.
- Goes at the end of the prop entry, after `$Custom Strings` if the entry has that block.
- Followed by any number of `+old:` / `+new:` pairs. File extensions are ignored.
- Use `invisible` as the `+new:` texture to hide that part of the model.
- **Syntax:** `$Texture Replace:` followed by pairs of *String* values

##### +old:

- The name of the texture on the model to replace.
- **Syntax:** *String*

##### +new:

- The texture to use instead, or `invisible`.
- **Syntax:** *String*

**Sample**

```
$Name: Cargo Pod
$POF file: cargopod.pof
$Texture Replace:
+old: pod_hull
+new: pod_hull_red
+old: pod_decal
+new: invisible
```
