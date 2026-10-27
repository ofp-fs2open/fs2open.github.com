> **Revision information**
> OFP Revision: [#77](https://github.com/ofp-fs2open/fs2open.github.com/pull/77)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

This page lists the tables that exist only in OFP, and the existing SCP tables OFP adds fields to. Every standard FreeSpace Open table works in OFP exactly as documented on the [FreeSpace Wiki](https://wiki.hard-light.net/index.php/Tables). Only the differences are listed here.

## Contents

1. [New tables](#new-tables)
2. [Additions to existing tables](#additions-to-existing-tables)

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

\* Notes Modular Tables

| Table | Modular extension | Code | What OFP adds |
|---|---|---|---|
| [props.tbl](props.tbl)\* | `xxx-prp.tbm` | /prop/prop.cpp | Texture replacement for every prop of a class |
| [species.tbl](species.tbl)\* | `xxx-intl.tbm` | /menuui/techmenu.cpp | 3D models for tech room intel entries |
