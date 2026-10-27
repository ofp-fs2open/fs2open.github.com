> **Revision information**
> OFP Revision: [#77](https://github.com/ofp-fs2open/fs2open.github.com/pull/77)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

This page lists the tables that exist only in OFP, and the existing SCP tables OFP adds fields to. Every standard FreeSpace Open table works in OFP exactly as documented on the [FreeSpace Wiki](https://wiki.hard-light.net/index.php/Tables). Only the differences are listed here.

## Contents

1. [New tables](#new-tables)
2. [Additions to existing tables](#additions-to-existing-tables)
3. [All tables](#all-tables)
4. [Version badges](#version-badges)

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

---

## All tables

Full documentation of every table, moving here from the [FreeSpace Wiki](https://wiki.hard-light.net/index.php/Tables) one table at a time. Tables not listed yet are still documented there.

| Table | Modular extension | Code |
|---|---|---|
| [ai.tbl](ai.tbl) | `xxx-aic.tbm` | /ai/aicode.cpp |

---

## Version badges

Table pages mark when a field or behavior was added with a version badge. Fields without a badge are from the original game.

**FreeSpace Open:**

![FSO 3.6.x](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.x.svg) ![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg) ![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg) ![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg) ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) ![FSO 3.7](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.svg) ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) ![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) ![FSO 20.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-20.4.svg) ![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) ![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg) ![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg) ![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg) ![FSO 22.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.2.svg) ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) ![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg) ![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) ![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg) ![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) ![FSO 26.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.0.svg) ![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

**OFP:**

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg)
