> **Revision information**
> OFP Revision: [#147](https://github.com/ofp-fs2open/fs2open.github.com/pull/147)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **strings.tbl** holds the translations of the game's built-in interface text, by index, for each language. (Translations of mission and table text go in [tstrings.tbl](tstrings.tbl).)

This table is one of the Modular Tables and can be extended with **xxx-lcl.tbm**.

Code: [/localization/localize.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/localization/localize.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in. Fields without a badge are from the original game.

## Contents

1. [General Info](#general-info)
2. [Format](#format)
3. [Sample](#sample)

---

## General Info

- Table consists of several sections named for the language of the translation.
- Other files use the index values for finding translations located in the **strings.tbl** (the number in an `XSTR("text", number)`).
- Only the section for the language the game is running in is read.
- The engine knows four languages without being told: English, German (extension gr), French (fr) and Polish (pl).

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) An optional `#Supported Languages` section adds new languages.

## Format

- Table consists of one or more sections, each beginning with a number sign followed by the name of the language (e.g., **#English**, **#German**, **#French**).
- Each section ends at the next line that starts with **#** (the original game's table uses **#end**).
- Each section consists of many indexed text entries.

### #Supported Languages

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) Optional, and must come first in the file. Adds languages, or changes the extension and special character index of one that exists (matched by name, case sensitive). Ends with `#End`.

#### $Language:

- The name of the language. Its strings go in a section named `#` plus this name.
- Syntax: **String**

#### +XSTR:

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- Optional. The strings.tbl index of the language's own name, so the name can be translated.
- Syntax: **Integer**

#### +Extension:

- Required. The suffix added to file names for this language's versions of files, for example `gr` for German.
- Syntax: **String**

#### +Special Character Index:

- The first character index of the font's special characters for this language. Required, except in Unicode text mode (see [game_settings.tbl](game_settings.tbl)), where it is read and ignored.
- Syntax: **Integer**, 0 to 255

More than 100 languages in total (the four built-in ones included) log a warning, since they would conflict with special-case language numbers.

This is an example of the optional section to specify new languages
```
#Supported Languages

$Language:	German
+Extension: gr
+Special Character Index: 164

$Language:	French
+Extension: fr
+Special Character Index: 164

$Language:	Spanish
+Extension: sp
+Special Character Index: 164

#End
```

### Individual Lines

- Each line starts with the number or index that it will be linked to from menu areas.
- Each line also contains the full text to use for that index throughout FSO.
- A string can use `\n` and `\r` for line breaks.
- An index can't be negative (that is an error). An index used twice in one table logs a warning; a modular table that uses an existing index replaces that string.
- Each string may be followed by one or two X-offset values. If two values exist, then the first one is used in 640x480, and the second one is used in 1024x768. If only one value exists, then it's used in both resolutions.
- Syntax: **Integer**, **String** or **Integer**, **String** **Integer** or **Integer**, **String** **Integer** **Integer**

## Sample

- Part of retail FreeSpace 2 table
```
#English
;-------------------------------------------------
; File: KeyControl.cpp
;-------------------------------------------------

0, "Player ship changed to %s"
1, "Changed player target to %s"
2, "Soundtrack changed to: %s"
3, "Event music is not playing"
4, "Framerate delay increased to %i milliseconds per frame."
5, "Framerate delay decreased to %i milliseconds per frame."
6, "Countermeasure firing: %s"
7, "ENABLED"
8, "DISABLED"
9, "You whacked yourself down to %7.3f percent hull.\n"
10, "You are %s"
...
...
1560, "PXO Banners"
1561, "Misc"
1562, "Force local port"
1563, "German"								; language choices in Misc tab
1564, "English"

; nope, there is more.  added 10/26
1565, "NEW USER TIP\n\n%s"					; heading on tip popup
1566, "Flak turret"
1567, "Beam turret"
1568, "Laser turret"
1569, "Missile lnchr"

#end
```

[The thread announcing new languages support](http://www.hard-light.net/forums/index.php?topic=85041.0)
