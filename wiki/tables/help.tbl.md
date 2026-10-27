> **Revision information**
> OFP Revision: [#121](https://github.com/ofp-fs2open/fs2open.github.com/pull/121)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **help.tbl** defines the help overlays shown on interface screens, such as the main hall, when the player presses F1: text with lines and brackets pointing at parts of the screen.

This table is one of the Modular Tables and can be extended with **xxx-hlp.tbm**.

Code: [/gamehelp/contexthelp.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/gamehelp/contexthelp.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in. Fields without a badge are from the original game.

## Contents

1. [Format](#format)
2. [Sections](#sections)
3. [Sample](#sample)

---

## Format

- Table consists of several sections that are each defining their respective screens ingame. Every section starts with the name of the screen preceded with **$** (***Example:* $ship**) and ends with **$end**
- Section names are up to 31 characters. A section with a name used before, in this or an earlier table, replaces that section completely.
  - **Default sections:**
    - $ship
    - $weapon
    - $briefing
    - $main
    - $barracks
    - $control
    - $debrief
    - $multicreate
    - $multistart
    - $multijoin
    - $main2
    - $hotkey
    - $campaign
    - $simulator
    - $tech
    - $command

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) Sections with other names can be defined too. A main hall shows one through `+Help Overlay:` in [mainhall.tbl](mainhall.tbl).

## Sections

- Sections consist of various drawing objects: text, lines or brackets (left or right).
- `+resolutions` and `+font`, if used, come first and in that order. After them, the drawing objects can come in any order and any number.

### +resolutions

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Defines the number of resolutions in this overlay. May be 1 or greater; anything less is an error.
- If this is set to anything other than 2, then all other parameters in this overlay that normally take two sets of values must now have a set of values for each resolution. For example, if an overlay has four resolutions, then any bracket parameter must have four pairs of values instead of two.
- The default value is 2.
  - Example: **+resolutions 4**

### +font

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Defines the font used for the text in this overlay. The number here points to a font entry in fonts.tbl, with 0 pointing to the first one.
- Takes one value for each resolution.
- The default value for each resolution is 0 (normally font01.vf).
  - Example: **+font 0 0 0 0**

### +pline

- Draws lines on the help screen.
- Can be used to draw several connected lines
- Is defined with an array of values
- First value defined the number of points used to draw the line. Any number works: the line has one segment fewer than its points.
  - 2 = straight line (2 points)
  - 4 = 3 segment lines (4 points)
  - 6 = 5 segment lines (6 points)
- Other values are used to define the coordinate locations on screen separately for each resolution in the table. Coordinates are marked as **x1, y1, x2, y2, x3, y3** etc - you must have enough co-ordinates to match the number specified in the 1st argument and you must have an array for each resolution.
  - First array for the lowest resolution, then the next largest, and so on.
  - Example: **+pline 4  170 320 120 320 100 340 60 340  272 512 192 512 160 544 96 544**

### +left_bracket

- Draws bracket opening to left on the help screen
- Is defined with a pair of numerical values for each resolution
- Location is defined with two values per resolution, starting with the lowest resolution.
  - Example: **+left_bracket 257 325 358 517**

### +right_bracket

- Draws bracket opening to right on the help screen
- Is defined with a pair of numerical values for each resolution
- Location is defined with two values per resolution, starting with the lowest resolution.
  - Example: **+right_bracket 257 325 358 517**

### +text

- Draws text on screen
- Is defined with a pair of numerical values for each resolution, followed by a string
- Location is defined with two values per resolution, starting with the lowest resolution.
- The text is one string, used at every resolution. It can be an XSTR for translation, up to 128 characters:
  - XSTR("**text**", -1)
  - Example: **+text 25 280 40 448 XSTR("Choose your ship type.", -1)**

## Sample

```
$ship
+text 25 280 40 448 XSTR("Choose your ship type.", -1)

+text 175 304 276 496 XSTR("Choose a new ship type by dragging", -1)
+text 175 317 276 509 XSTR("its icon over one of the available", -1)
+text 175 330 276 522 XSTR("wing slots.", -1)
+pline 4  170 320 120 320 100 340 60 340  272 512 192 512 160 544 96 544
+left_bracket 169 312 270 504
+pline 6  259 335 280 335 300 355 300 377 280 397 183 397  361 527 448 527 480 559 480 603 448 635 293 635
+right_bracket 257 325 358 517

+text 337 370 565 592 XSTR("Revert to the original", -1)
+text 337 383 565 605 XSTR("ship distribution.", -1)
+pline 2  502 374 570 374  730 596 912 596
+right_bracket 499 365 727 587

+text 400 150 636 265 XSTR("When finished outfitting", -1)
+text 400 163 636 278 XSTR("all available wings,", -1)
+text 400 176 636 291 XSTR("you can go to the", -1)
+text 400 189 636 304 XSTR("weapons loadout screen", -1)
+text 400 202 636 317 XSTR("or enter the mission.", -1)
+pline 6  162 43 330 43 350 63 350 173 370 193 395 193  259 69 528 69 560 101 560 277 592 309 632 309
+left_bracket 394 184 630 299
+pline 4  554 206 588 206 608 226 608 424  790 321 941 321 973 353 973 678
+right_bracket 552 197 788 312
$end
```
