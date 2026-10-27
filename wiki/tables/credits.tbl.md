> **Revision information**
> OFP Revision: [#113](https://github.com/ofp-fs2open/fs2open.github.com/pull/113)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **credits.tbl** defines the text shown on the credits screen, and the music and artwork that go with it.

This table is one of the Modular Tables and can be extended with **xxx-crd.tbm**.

Code: [/menuui/credits.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/menuui/credits.cpp). See [OFP Tables](OFP-Tables) for every table.

Badges show the version a field or behavior was added in. Fields without a badge are from the original game.

## Contents

1. [Format](#format)
2. [Settings](#settings)
3. [Credits text](#credits-text)
4. [Artwork](#artwork)
5. [Sample](#sample)

---

## Format

- A table is a few optional settings, then the credits text, ending with `#end` or the end of the file.
- The settings must be in the order listed below; one out of order is not read. Each table can set them, and a later table overrides an earlier one.

---

## Settings

### $Music:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- The music played during the credits, by its name in [music.tbl](music.tbl).
- Default: Cinema
- Syntax: ***String***

### $Substitute Music:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- The music played during the credits if an optional music pack is present. If this names a valid music entry, it's used instead of `$Music:`.
- Syntax: ***String***

### $Number of Images:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- How many artwork images the credits cycle through (see [Artwork](#artwork)).
- Default: 46, as in the original game. A value of 0 or less is ignored.
- Syntax: ***Integer***

### $Start Image Index:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- The image the artwork starts with.
- Default: a random image. Values outside 0 to `$Number of Images:` minus 1 are moved to the nearest end of that range.
- Syntax: ***Integer***

### $Text scroll rate:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- How fast the text scrolls.
- Default: 15. The minimum is 0.01.
- Syntax: ***Float***, pixels per second

### $Artworks display time:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- How long each image is shown, not counting the fades.
- Default: 9. The minimum is 0.01.
- Syntax: ***Float***, seconds

### $Artworks fade time:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- How long each image takes to fade in or out.
- Default: 1. The minimum is 0.01.
- Syntax: ***Float***, seconds

### $SCP Credits position:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg)

- Whether the built-in FreeSpace Open credits come before or after the mod's credits.
- Default: Start. Anything other than Start or End logs a warning and is ignored.
- Syntax: ***String***, one of:
  - *Start*
  - *End*

---

## Credits text

- Everything after the settings, up to `#end`, is the credits text, line for line. Blank lines are kept.
- Any line can be written as an `XSTR("text", index)` so it can be translated; headers usually are, for example `XSTR("Lead Programmer:", -1)`.
- Lines too long for the credits area are wrapped. (SCPUI's credits screen does its own wrapping instead.)
- The text of each table is shown in table order: credits.tbl first, then each xxx-crd.tbm.
- If the first line of credits.tbl's text is exactly `Design:`, as in the original game's table, a heading "ORIGINAL VOLITION STAFF:" is added before it. A total conversion with its own credits doesn't get it.

---

## Artwork

The images cycled during the credits are named **2_CrIm** followed by the image number with at least two digits (**CrIm** at 640x480): 2_CrIm00, 2_CrIm01, and so on up to `$Number of Images:` minus 1.

---

## Sample

```
$Start Image Index: 0
$Text scroll rate: 17.0
$Artworks display time: 6.5
$Artworks fade time: 1.5
$SCP Credits position: End

XSTR("Lead Programmer:", -1)

Dave Baranec

XSTR("Programming:", -1)

Dave Andsager
Jeff Farris
Neil Kumar

#end
```
