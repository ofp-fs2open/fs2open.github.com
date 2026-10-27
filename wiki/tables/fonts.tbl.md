> **Revision information**
> OFP Revision: [#118](https://github.com/ofp-fs2open/fs2open.github.com/pull/118)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **fonts.tbl** defines the fonts the game can use. The HUD gauges, the show-subtitle-text SEXP, the [fiction viewer](https://wiki.hard-light.net/index.php/Fiction_Viewer) and many other places can be set to any font in fonts.tbl. Other text (such as the tech room) uses one of the first three fonts.

The engine has a built-in fonts.tbl, used when the mod has none.

This table is one of the Modular Tables and can be extended with **xxx-fnt.tbm**.

Code: [/graphics/software/font.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/graphics/software/font.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in.

## Contents

1. [General Info](#general-info)
2. [Top and Bottom offsets](#top-and-bottom-offsets)
3. [Sample Entry](#sample-entry)

---

## General Info

- Begins with #Fonts and ends with #End.
- Across all tables, at least 3 fonts must load, or the game stops with an error.
- A font's fields must be in the order listed here; one out of order is not read.
- A font that fails to load is an error, and the table moves on to the next font.

Currently, there are two different types of supported fonts. The old "VFNT" type (which probably means **V**olition **F**o**nt**) and the new TrueType fonts. The old font use the usual *$Font:* option while the new fonts use *$TrueType:*.  TrueType fonts can be TTF or OTF.

### $Font:

- Lists a font file to use.
- This font type is not supported if the engine is in [Unicode text mode](game_settings.tbl#unicode-mode)! Such fonts are skipped, with a warning listing them in the log.
- Syntax: **String**

#### +Name:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Sets the name of this font. Defaults to the specified filename if not provided. (Names of these fonts aren't checked for duplicates.) Be aware that using a number as the name makes referring to the font by name impossible, as numbers are parsed as the font index not the font name.
- Syntax: **String**

#### +Default Special Character Index:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Specifies the index of the first special character (e.g., infinity symbol, "match speed" icon) in the font. Must be within the range of 0-248; a value outside it logs a warning. Does not apply to language entries below that use [+Special Character Index:](#special-character-index).
- Syntax: **Integer**

> **Note:** If this parameter is used with the first font, it will override all uses of +Special Character Index: in [strings.tbl](strings.tbl).

#### +Language:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Specifies the name of a language. Any number of these can follow, each with its own index.
- Currently used for overriding the value of [+Default Special Character Index:](#default-special-character-index).
- A language that isn't built in or defined in [strings.tbl](strings.tbl) logs a warning and is skipped.
- Syntax: **String**

##### +Special Character Index:

- Overrides the value of [+Default Special Character Index:](#default-special-character-index) for this language. Must be 0-248; anything else is an error.
- Syntax: **Integer**

#### +Auto Size:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Tells FSO to try to automatically scale this font to match the screen resolution. Also accepted without the colon, as `+Auto Size`.
- Syntax: **Boolean**

#### +Can Scale:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Tells FSO that this font can scale with the Font Size option available to the player in In-Game Options.
- Syntax: **Boolean**

#### +Top offset:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Specifies the top offset for this font. See [Top and Bottom offsets](#top-and-bottom-offsets) for more information.
- Syntax: **float**

#### +Bottom offset:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Specifies the bottom offset for this font. See [Top and Bottom offsets](#top-and-bottom-offsets) for more information.
- Syntax: **float**

#### +Tab width:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- The width of a tab character.
- Default: 20
- Syntax: **float**, pixels before scaling, 0 or more. A negative value is an error and is ignored.

### $TrueType:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Lists a font file to use.
- Syntax: **String**

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) As of 21.0, this can be OTF in addition to the previously supported TTF.

#### +Name:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Specifies the name of this font, defaults to **<fontFilename>-<fontSize>**. A name already used by another font is an error, and this font is skipped. Be aware that using a number as the name makes referring to the font by name impossible, as numbers are parsed as the font index not the font name.
- Syntax: **String**

#### +Size:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Specifies the size of the font, defaults to 8.
- Syntax: **float**, must be greater than 0. Anything else is an error, and 8 is used.

#### +Auto Size:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Tells FSO to try to automatically scale this font to match the screen resolution.
- Syntax: **Boolean**

#### +Can Scale:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- Tells FSO that this font can scale with the Font Size option available to the player in In-Game Options.
- Syntax: **Boolean**

#### +Top offset:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Specifies the top offset for this font. See [Top and Bottom offsets](#top-and-bottom-offsets) for more information.
- Syntax: **float**

#### +Bottom offset:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Specifies the bottom offset for this font. See [Top and Bottom offsets](#top-and-bottom-offsets) for more information.
- Syntax: **float**

#### +Tab width:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- The width of a tab character.
- Default: 20
- Syntax: **float**, pixels before scaling, 0 or more. A negative value is an error and is ignored.

#### +Letter spacing:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Specifies the letter spacing. Defaults to 0.
- Syntax: **float**, must be greater or equal to 0. A negative value is an error and is ignored.

#### +Special Character Font:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

- Specifies the VFNT file to use for the special characters. FSO uses a few special characters for a few things that are not present in standard TrueType fonts. To fix this you can specify a VFNT file from which these characters are loaded. This defaults to **font01.vf** which is a default font file from FS2 retail.
- This option not needed and not supported if the engine is in [Unicode text mode](game_settings.tbl#unicode-mode)!
- Note! Pointing this to the fifth font entry or later can cause crashes, especially on standalones.
- Syntax: **String**

##### +Special Character Index:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- The index of the first special character in the `+Special Character Font:`. Must be 0-248; anything else logs a warning and 0 is used.
- Without it, the index set for that font's own entry is used; if that font has no entry, a warning is logged and 0 is used.
- Syntax: **Integer**

## Top and Bottom offsets

The top and bottom offsets can be used to influence the line spacing for a specific font. Before rendering **top offset** vertical pixels are skipped. Then the text is rendered and then another **bottom offset** pixels are skipped.

## Sample Entry

```
#Fonts

$Font: font01.vf
$Font: font02.vf
$Font: font03.vf

$TrueType: arial.ttf
 +Name: Arial
 +Size: 12

#End
```

## Using a font in the fiction viewer

In a mission file, the fiction viewer picks a font by name or number:

```
#Fiction Viewer

$File: [text file]
$Font: [font file]
```

## References

1. "[Re: Feature Request: 4th font (especially for fiction viewer)](http://www.hard-light.net/forums/index.php?topic=66942.msg1327152#msg1327152)", *Hard Light Productions Forums*, Hard Light Productions, 24-12-2009. Retrieved on January 28, 2011.
