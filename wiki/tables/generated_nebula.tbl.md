> **Revision information**
> OFP Revision: [#77](https://github.com/ofp-fs2open/fs2open.github.com/pull/77)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg) This table exists only in OFP.

The **generated_nebula.tbl** defines the patterns and colors of the generated nebula: a soft cloud backdrop drawn on a sphere behind the stars, the modern version of FreeSpace 1's background nebula. A mission picks one pattern and one color (see [Using it in a mission](#using-it-in-a-mission)).

This table is one of the Modular Tables and can be extended with **xxx-gneb.tbm**.

The engine includes a built-in copy of this table with FS1's three patterns (**Nebula01**, **Nebula02**, **Nebula03**) and nine colors. A mod that ships its own `generated_nebula.tbl` replaces the built-in copy entirely; a `*-gneb.tbm` adds to it or overrides entries by name.

## Contents

1. [Format](#format)
2. [#Generated Nebula Colors](#generated-nebula-colors)
   1. [$Name:](#name)
   2. [+RGB:](#rgb)
3. [#Generated Nebula Patterns](#generated-nebula-patterns)
   1. [$Name:](#name-1)
   2. [+Clouds Only:](#clouds-only)
   3. [+Cloud Detail:](#cloud-detail)
   4. [+Cloud:](#cloud)
   5. [+Density:](#density)
   6. [+Frequency:](#frequency)
   7. [+Octaves:](#octaves)
   8. [+Warp:](#warp)
   9. [+Contrast:](#contrast)
   10. [+Intensity:](#intensity)
   11. [+Seed:](#seed)
   12. [+Resolution:](#resolution)
   13. [+Band:](#band)
4. [Loading rules](#loading-rules)
5. [How a pattern is drawn](#how-a-pattern-is-drawn)
6. [Using it in a mission](#using-it-in-a-mission)
7. [Sample](#sample)

---

## Format

- The table has two sections, `#Generated Nebula Colors` and `#Generated Nebula Patterns`, each ending with `#End`.
- Either section may come first, and either may be left out, so a `*-gneb.tbm` can carry just colors or just patterns.
- Entries begin with `$Name:`. Every other field is optional, but the fields an entry does use must appear in the order listed on this page.
- Anything after the last `#End` is reported as a table error.

---

## #Generated Nebula Colors

**OFP, 26.1:**
- Section header for the tint colors. Ends with `#End`.

### $Name:

**OFP, 26.1:**
- The name of the color. Missions refer to it in `+Color:`.
- Names are case-insensitive. Using an existing name overrides that color.
- **Syntax:** *String*

### +RGB:

**OFP, 26.1:**
- The color of the brightest parts of the nebula. Dimmer parts fade evenly toward black.
- Each value is clamped to 0 to 255.
- The built-in colors are FS1's exactly. Each FS1 mission palette held a 20-step ramp from black to one color, and `+RGB` is that ramp's brightest entry.
- **Default:** `255 255 255`
- **Syntax:** *Integer list*, `red green blue`

| Built-in color | +RGB |
|---|---|
| Red | 151 0 0 |
| Blue | 0 55 183 |
| Gold | 107 91 27 |
| Purple | 107 0 95 |
| Maroon | 107 0 39 |
| Green | 0 95 0 |
| Grey blue | 0 67 91 |
| Violet | 63 51 79 |
| Grey Green | 0 87 63 |

---

## #Generated Nebula Patterns

**OFP, 26.1:**
- Section header for the patterns. Ends with `#End`.

### $Name:

**OFP, 26.1:**
- The name of the pattern. Missions refer to it in `+Nebula:`.
- Names are case-insensitive. Using an existing name overrides that pattern, changing only the fields the entry lists.
- **Syntax:** *String*

### +Clouds Only:

**OFP, 26.1:**
- `YES` draws only the placed clouds, with no procedural background.
- **Default:** `NO`
- **Syntax:** *Boolean*

### +Cloud Detail:

**OFP, 26.1:**
- How much the pattern's noise breaks up the placed clouds: `0` leaves them as smooth soft shapes, `1` carves them into ragged wisps. The clouds keep about the same overall brightness either way.
- Clamped to 0 to 1.
- **Default:** `0.35`
- **Syntax:** *Float*

### +Cloud:

**OFP, 26.1:**
- A placed cloud: a soft ellipse on the nebula. Repeat the field once per cloud.
- Listing any `+Cloud:` in an entry replaces that pattern's whole cloud list, so an override restates every cloud it wants.
- **Syntax:** *Float list*, `lon lat width height angle brightness`
  - **lon:** center longitude in degrees, 0 to 360. It wraps, so a cloud can cross the seam.
  - **lat:** center latitude in degrees, -90 to 90.
  - **width:** size across, in degrees of longitude, measured where the cloud is at half its peak brightness.
  - **height:** size along the other axis, on the same scale as width.
  - **angle:** rotation of the ellipse, in degrees.
  - **brightness:** peak brightness, 0 to 1. Where clouds overlap, their brightness adds up, capped at 1.

### +Density:

**OFP, 26.1:**
- Procedural background: roughly how much of the sky has bright patches, 0 to 1.
- **Default:** `0.30`
- **Syntax:** *Float*

### +Frequency:

**OFP, 26.1:**
- Noise frequency around the sphere and from pole to pole. Unequal values stretch the noise into streaks.
- Each value is clamped to 0.1 to 64.
- **Default:** `2 5`
- **Syntax:** *Float list*, `around along`

### +Octaves:

**OFP, 26.1:**
- Layers of noise detail. Fewer gives broader, smoother masses; more gives a busier look.
- Clamped to 1 to 8.
- **Default:** `4`
- **Syntax:** *Integer*

### +Warp:

**OFP, 26.1:**
- How much the noise is swirled, stretching it into wisps.
- Clamped to 0 to 8.
- **Default:** `0.4`
- **Syntax:** *Float*

### +Contrast:

**OFP, 26.1:**
- Procedural background: the falloff curve of the bright patches. Higher values give tighter, harder-edged patches.
- Clamped to 0.05 to 10.
- **Default:** `1.5`
- **Syntax:** *Float*

### +Intensity:

**OFP, 26.1:**
- Overall brightness multiplier for the whole pattern. `0` turns it off.
- **Default:** `1`
- **Syntax:** *Float*

### +Seed:

**OFP, 26.1:**
- Noise seed. Patterns with the same settings and different seeds look different.
- Clamped to -1000000 to 1000000.
- **Default:** `0`
- **Syntax:** *Integer*

### +Resolution:

**OFP, 26.1:**
- How finely the background sphere is built, around and from pole to pole. The brightness is drawn per pixel either way, so this only affects how round the sphere is.
- **Default:** `24 12`
- **Syntax:** *Integer list*, `around along`

### +Band:

**OFP, 26.1:**
- The part of the sphere covered, from 0 (one pole) to 1 (the other).
- **Default:** `0 1`
- **Syntax:** *Float list*, `start end`

---

## Loading rules

1. The game's own `generated_nebula.tbl`, if it has one, otherwise the built-in copy.
2. Then every `*-gneb.tbm`, in the usual modular table order. A `$Name:` that already exists is overridden in place; a new name adds an entry.

---

## How a pattern is drawn

- A pattern has two layers:
  - a **procedural background** made from noise (`+Density` through `+Seed`), unless `+Clouds Only` is set;
  - the **placed clouds** (`+Cloud`), broken up by the same noise according to `+Cloud Detail`.
- The layers are combined with a screen blend, so clouds brighten the background without clipping to flat white.
- The result is tinted by the mission's color, baked into a 1024 x 512 texture, and drawn additively on a sphere behind the stars and in front of the skybox.
- The built-in patterns are clouds only, laid out to match where FS1's originals had their bright regions.

---

## Using it in a mission

The mission's `#Background bitmaps` section picks the pattern and color with the existing FS1 fields, and an OFP field turns it on:

| Field | Meaning |
|---|---|
| `+Nebula:` | Pattern name |
| `+Color:` | Color name |
| `+Pitch:`, `+Bank:`, `+Heading:` | Orientation, in degrees |
| `+Generated Nebula: true` | **OFP, 26.1.** Draws the nebula. Without it the mission shows nothing, even with the fields above. |

`+Generated Nebula:` exists because many missions saved by older FRED versions carry an unchosen `+Nebula: Nebula01` and `+Color: Blue`. Those missions stay as they always were.

In qtFRED, set all of this in the Background Editor's **Generated BG Nebula** group: tick **Generated Nebula**, then choose the pattern, color and orientation. The viewport shows the nebula as you edit. It isn't available while Full Nebula is on.

Pattern or color names that the current tables don't define are kept and written back unchanged when the mission is saved.

---

## Sample

```
#Generated Nebula Colors

$Name: Teal
+RGB: 0 140 130

#End

#Generated Nebula Patterns

;; a fully procedural pattern
$Name: Drift
+Density: 0.4
+Frequency: 3 6
+Octaves: 3
+Warp: 0.6
+Seed: 7

;; placed clouds over a faint background
$Name: Twin Streaks
+Cloud Detail: 0.5
+Cloud: 90 10 70 12 15 0.8
+Cloud: 250 -20 60 10 170 0.6
+Density: 0.2
+Intensity: 0.8

#End
```
