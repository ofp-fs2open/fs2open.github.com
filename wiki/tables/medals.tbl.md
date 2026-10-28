> **Revision information**
> OFP Revision: [#128](https://github.com/ofp-fs2open/fs2open.github.com/pull/128)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **medals.tbl** is used for defining the various medals the player can achieve while playing the game, and the medal case screen they are shown in.

This table is one of the Modular Tables and can be extended with **xxx-mdl.tbm**.

Code: [/stats/medals.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/stats/medals.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in. Fields without a badge are from the original game.

## Contents

1. [Format](#format)
2. [#Medals](#medals)
3. [Sample](#sample)

---

## Format

- Table begins with **#Medals**
- Optional positions of headers
- Individual medal entries
- Table ends with **#End**
- The header settings and each medal's fields must be in the order listed here. Every field is optional unless noted.
- Pilot files, missions and campaigns refer to medals by name. The order still matters for the defaults: the first 18 medals in medals.tbl get the retail positions and images, and the default mask index is the medal's position.
- After all tables are read, a medal named **Rank** must exist (it shows the player's rank); without it, a warning is logged.

## #Medals

### +Background Bitmap:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Optional. Specifies the image that should be used as the background of the medal case.  (A 2_ will be prepended to this image for the GR_1024 resolution.)
- A modular table without it keeps the current one.
- Syntax: **String**
- Default: **MedalsDisplayEmpty**

### +Mask Bitmap:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Optional. Specifies the image that should be used as the mask of the medal case.  (A 2_ will be prepended to this image for the GR_1024 resolution.)
- A modular table without it keeps the current one.
- Syntax: **String**
- Default: **Medals-M**

### +Exit Button Hotspot Index:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

- Optional. Specifies the hotspot index (color index of the mask bitmap file) that should be used for the Exit button.  The default is 18; overriding it with this field is useful if you need index 18 for an 18th medal entry.
- Syntax: **Integer**
- Default: **18**

### +Callsign Position 640:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Optional. Specifies where the player's callsign should appear on the screen, for the GR_640 resolution.  An X value of -1 causes the callsign to be centered on the X-axis.
- Syntax: **Integer X, Integer Y**
- Default: **-1, 54**

### +Callsign Position 1024:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Optional. Specifies where the player's callsign should appear on the screen, for the GR_1024 resolution.  An X value of -1 causes the callsign to be centered on the X-axis.
- Syntax: **Integer X, Integer Y**
- Default: **-1, 89**

### +Label Position 640:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Optional. Specifies where the medal label should appear on the screen, for the GR_640 resolution.  Note that it is necessary to specify the width of this area in addition to X and Y, because the label is centered within it.
- Syntax: **Integer X, Integer Y, Integer W**
- Default: **241, 458, 300**

### +Label Position 1024:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Optional. Specifies where the medal label should appear on the screen, for the GR_1024 resolution.  Note that it is necessary to specify the width of this area in addition to X and Y, because the label is centered within it.
- Syntax: **Integer X, Integer Y, Integer W**
- Default: **386, 734, 480**

### $Name:

- Defines the name of the medal
- A name used twice in medals.tbl is an error. In a modular table, a name that already exists edits that medal.
- Syntax: **String**

### +nocreate

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Optional. Goes directly after `$Name:`.
- In a modular table, edits the medal only if it already exists, instead of creating a new one. Using it in medals.tbl itself logs a warning.

### $Alt Name:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

- Alternate display name: shown instead of `$Name:` wherever the medal's name is displayed, so the medal can be renamed without changing the name missions and pilot files use.
- Syntax: **String**

### $Bitmap:

- Defines the bitmap used for the medal in the medal case.
- Required in practice: a medal with no bitmap after all tables are read is an error.
- Syntax: **String**

### +Position 640:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Optional. Specifies where the medal bitmap should appear on the screen, for the GR_640 resolution.
- Syntax: **Integer X, Integer Y**
- The first 18 medals in medals.tbl default to the retail medal positions. Any other medal (including every medal added by a modular table) must set it, or a warning is logged and it is drawn at 0, 0.

### +Position 1024:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Optional. Specifies where the medal bitmap should appear on the screen, for the GR_1024 resolution.
- Syntax: **Integer X, Integer Y**
- The first 18 medals in medals.tbl default to the retail medal positions. Any other medal (including every medal added by a modular table) must set it, or a warning is logged and it is drawn at 0, 0.

### +Mask Index:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- The index of the color in the mask that will tell FSO when the mouse is hovering over this medal
- Defaults to the position of the medal in the list of medals, counting from 0
- Syntax: **Integer**

### +Debriefing Bitmap:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Optional. Defines the bitmap used for the medal in the debriefing screen.
- Syntax: **String**
- The first 18 medals in medals.tbl default to the retail debriefing images. Any other medal without one logs a warning.
- NOTE: For medals which have more than one debriefing image, such as Rank or Wings, the file name can contain ##. This will be replaced with a number corresponding to the version, so DebriefWings## will show the plain Wings medal for the first version and the Wings medal with laurels for subsequent versions. This should be applicable to any medal, not just Rank or Wings.

### $Num mods:

- Sets the number of versions of the medal: how many times it can be awarded, each time shown with its next image.
- Syntax: **Integer**

### +Version starts at 1:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Optional. Defines whether the debriefing medal image starts numbering at 01 versus 00.
- Syntax: **Boolean**
- Default: YES for the medal named Rank, NO for every other medal
- NOTE: This setting only exists because Volition started the numbering for Rank at DebriefRank01 and for Wings at DebriefWings00. So you can start the numbering for your custom medal version at whatever number suits your fancy.

### +Available From Start:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Optional. If true, the medal will always be in the medal case.
- Syntax: **Boolean**
- Defaults to false.

### For medals based on kill counts

Medals with `+Num Kills:` are badges: they are awarded automatically when the player reaches the kill count, with a voice and text in the debriefing. The fields below only exist after `+Num Kills:`.

#### +Num Kills:

- Defines the number of kills needed for the particular medal
- Syntax: **Integer**

#### $Wavefile 1:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- An older name for `$Wavefile Base:`.
- Syntax: **String**

#### $Wavefile 2:

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- An older name for `$Wavefile Base:`. `$Wavefile 1:`, `$Wavefile 2:` and `$Wavefile Base:` all set the same file, and the last one given wins.
- Syntax: **String**

#### $Wavefile Base:

- Defines the voice file played in the debriefing when the badge is awarded.
- The persona number of the mission's debriefing persona is put in front of it: with badge_a.wav and persona 2, the game plays 2_badge_a.wav. If that file doesn't exist, or there is no persona, it plays 9_badge_a.wav.
- Syntax: **String**

#### $Promotion Text:

- Defines the text that is displayed when the medal is obtained. Value refers to translations found from [tstrings.tbl](tstrings.tbl). Is followed with **$end_multi_text**.
- A badge needs one text without a `+Persona:` (the default); without it, a warning is logged and the text is empty.
- Syntax: XSTR("**String**", **Integer**)

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) Multiple Promotion Text fields may exist, each (except for the default) followed by its own Persona. The mission's Debriefing Persona Index controls which one is used.

##### +Persona:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- Optional. Defines the persona associated with this text.  If specified, this particular text will be chosen when the Debriefing Persona Index matches.
- A negative persona logs a warning and the text is ignored.
- Syntax: **Integer**

## Sample

- Retail FreeSpace 2 table
```
#Medals

$Name:		Epsilon Pegasi Liberation
$Bitmap:		Medal00.pcx
$Num mods:	4

$Name:		Imperial Order of Vasuda
$Bitmap:		Medal01.pcx
$Num mods:	4

$Name:		Distinguished Flying Cross
$Bitmap:		Medal02.pcx
$Num mods:	4

$Name:		SOC Service Medallion
$Bitmap:		Medal03.pcx
$Num mods:	4

$Name:		Intelligence Cross
$Bitmap:		Medal04.pcx
$Num mods:	4

$Name:		Order of Galatea
$Bitmap:		Medal05.pcx
$Num mods:	4

$Name:		Meritorious Unit Commendation
$Bitmap:		Medal06.pcx
$Num mods:	4

$Name:		Medal of Valor
$Bitmap:		Medal07.pcx
$Num mods:	4

$Name:		GTVA Legion of Honor
$Bitmap:		Medal08.pcx
$Num mods:	4

$Name:		Allied Defense Citation
$Bitmap:		Medal09.pcx
$Num mods:	4

$Name:		Nebula Campaign Victory Star
$Bitmap:		Medal10.pcx
$Num mods:	4

$Name:		NTF Campaign Victory Star
$Bitmap:		Medal11.pcx
$Num mods:	4

$Name:		Rank
$Bitmap:		Medal12.pcx
$Num mods:	10

$Name:		Wings
$Bitmap:		Medal13.pcx
$Num mods:	2

$Name:		Ace
$Bitmap:		Medal14.pcx
$Num mods:	1
+Num Kills:	60
$Wavefile Base: badge_a.wav
$Promotion Text:

XSTR("For your excellent record of confirmed kills, you have achieved the status of Ace.", 2722)

$end_multi_text

$Name:		Double Ace
$Bitmap:		Medal15.pcx
$Num mods:	1
+Num Kills:	150
$Wavefile Base: badge_b.wav
$Promotion Text:

XSTR("For your outstanding record of confirmed kills, you have achieved the status of Double Ace.", 2723)

$end_multi_text

$Name:		Triple Ace
$Bitmap:		Medal16.pcx
$Num mods:	1
+Num Kills:	350
$Wavefile Base: badge_c.wav
$Promotion Text:

XSTR("For your extraordinary record of confirmed kills, you have achieved the status of Triple Ace.", 2724)

$end_multi_text

$Name:		SOC Unit Crest
$Bitmap:		Medal17.pcx
$Num mods:	1

#End
```
