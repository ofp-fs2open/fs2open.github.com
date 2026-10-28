> **Revision information**
> OFP Revision: [#131](https://github.com/ofp-fs2open/fs2open.github.com/pull/131)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **music.tbl** defines the music played during the game: the soundtracks used in missions, and the menu music used everywhere else.

This table is one of the Modular Tables and can be extended with **xxx-mus.tbm**, which uses the exact same format.

Code: [/gamesnd/eventmusic.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/gamesnd/eventmusic.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in. Fields without a badge are from the original game.

## Contents

1. [Format](#format)
2. [Soundtrack](#soundtrack)
3. [Menu Music](#menu-music)
4. [Format Sample](#format-sample)
5. [Related Links](#related-links)

---

## Format

`music.tbl` consists of two kinds of sections defined as follows:
1. The soundtracks, which are music files intended to be played *during a mission*.
1. The menu music, which are music files intended to be played for other purposes.

Both kinds of section can appear any number of times, in any order. Parsing stops at the first line that isn't `#Soundtrack Start` or `#Menu Music Start`, so anything after that is ignored.

Comments (which are not parsed by the game engine) can be added at the start of a line, or after any line containing formatting, by beginning with a semicolon (`;`).

`-mus.tbm` modular tables use the exact same formatting as `music.tbl`.

## Soundtrack

**Each** individual soundtrack **must** start with `#SoundTrack Start`, end with `#SoundTrack End`, and have the following lines in between:
1. `$Soundtrack Name:`
1. One of the following:
   * 12 lines, all starting with `$Name:`
   * ![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg) 12-14 lines, all starting with `$Name:`
   * up to 14 lines, starting with the `$String:` names listed [below](#string)

### $Soundtrack Name:

`$Soundtrack Name:` is immediately followed by the name of the soundtrack. This primarily affects how the soundtrack is referred to in [FRED](https://wiki.hard-light.net/index.php/FRED) as well as its variants. The name of the soundtrack can be prefixed with a number to keep listings organized, for example:
```
$Soundtrack Name: 1: Genesis
```

A name that already exists (in this or an earlier table) edits that soundtrack: the tracks it lists replace the matching ones, and the rest are kept.

#### Flags

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg) The following flags can be added in extra lines after `$Soundtrack Name:`, in this order.

##### +nocreate

Only for editing: the soundtrack is changed only if one with this name already exists. If it doesn't, the whole soundtrack is skipped instead of creating an incomplete one.

##### +Cycle: FS1

This cycles the Normal and Battle tracks the FS1 way, instead of the FS2 way. Any value other than FS1 does nothing, and an edit can't turn it back off.

##### +Allied Arrival Overlay: NO

This allows the Allied Arrival track to play on its own, as it did in FS1.

- Default: YES. This is applied every time the soundtrack is read, so a modular table that edits the soundtrack must repeat `+Allied Arrival Overlay: NO` or the overlay is turned back on.

### $Name:

For this section, refer to the code for `FS2_Amb_A01.wav`, as follows:
```
$Name:	FS2_Amb_A01.wav				25.0	105840	; * Ambience
```

The code above can be explained using the following table:
| $Name: (*fixed*) | Filename | Number of Measures (*m*) | Samples per Measure (*s*) | Comments (optional) |
|---|---|---|---|---|
| `$Name:` | `FS2_Amb_A01.wav` | `25.0` | `105840` | `; * Ambience` |

The three values can be separated by spaces, tabs or commas. A line with a missing or extra value logs a warning.

The subsections describe each of the above columns in more detail.

#### $Name:

The individual music segments in a soundtrack is defined using **at least 12 lines**, all beginning with `$Name:`. The *FreeSpace* engine parses and plays the audio file referenced in each line in the following order:

| Order | Situation | Details |
|---|---|---|
| 1 | Ambience | Normal track. Plays when nothing is happening or units are out of combat range. Loops on itself until another situation is called, but if Ambience 2 is defined in FSO builds, will usually segue to it. |
| 2 | Allied Arrival during Ambience | Plays when a friendly unit arrives while Ambience is playing. By default, plays on top of other audio tracks without seguing; in FSO builds, if `+Allied Arrival Overlay: NO` is added below `$Soundtrack Name:`, the currently playing track will segue into this instead. |
| 3 | Enemy Arrival during Ambience | Plays when a hostile unit arrives while Ambience is playing. |
| 4 | Battle 1 | Battle track 1. Plays when units are in combat range. Usually segues into Battle 2. |
| 5 | Battle 2 | Battle track 2. Plays when units are in combat range. Usually segues into Battle 3. |
| 6 | Battle 3 | Battle track 3. Plays when units are in combat range. Usually segues into Battle 1. |
| 7 | Allied Arrival during Battle | Plays when a friendly unit arrives while any Battle track is playing. By default, plays on top of other audio tracks without seguing; in FSO builds, if `+Allied Arrival Overlay: NO` is added below `$Soundtrack Name:`, the currently playing track will segue into this instead. |
| 8 | Enemy Arrival during Battle | Plays when a hostile unit arrives while any Battle track is playing. |
| 9 | Victory 1 | Plays when a goal is completed. Segues into Victory 2, Ambience, or a Battle track depending on the remaining goals and hostile units remaining, and whether Victory 2 has already been called. |
| 10 | Victory 2 | Victory 1 segues into this track when all goals are completed *and* there are absolutely no hostile units present. Is only called once; if additional goals are validated in-mission and/or new hostile units arrive after this track has already been played, Victory 1 will segue into Ambience or a Battle track. |
| 11 | Goal Failed | Plays when the goals of a mission are failed. Does not appear to be called in-game. |
| 12 | Player Dies | Plays when the player is killed and goes to the death screen. |

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- The following lines can be added.

| Order | Situation | Details |
|---|---|---|
| 13 | Ambience 2 | Normal track. Plays when nothing is happening or units are out of combat range. If Ambience 3 is defined, usually segues into it, otherwise segues into Ambience. |
| 14 | Ambience 3 | Normal track. Plays when nothing is happening or units are out of combat range. Usually segues into Ambience. |

The order and situation of the lines **cannot be changed** and are **required** in order for the engine to parse the audio files in the correct situations. More than 14 lines log a warning and the extras are ignored. If there is no audio file available for any of the situations (such as Battle 3, for instance), a placeholder line denoting an empty track should still be included, such as the bolded code below:
```
...
$Name:	FS2_Btl_A02.wav				27.8	81415	; * Battle 2
$Name:	none.wav				0.0	0	; * Battle 3
$Name:	FS2_AArv_A01.wav			1.8	105840	; * Allied Arrival during Battle
...
```

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg) A track named **none** (or none.wav), or with 0 measures or 0 samples per measure, is treated as empty: it is never played and no file is needed.

#### Filename

The filename of the audio file must also include its file extension. FS1 and FS2 only support the playback of Waveform Audio (`.wav`) files.

![FSO 3.6.x](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.x.svg) FSO supports the playback of Waveform Audio and OGG Vorbis (`.ogg`) files.

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg) The file is looked for with the exact extension given, since the measure values depend on the audio format. If it isn't found but the same name exists with the other extension, a warning is logged and that file is used instead. If no file is found at all, a warning is logged and **the whole soundtrack is not used**.

Negative measure or sample values are not allowed: they log a warning and the soundtrack is not used.

#### Number of Measures and Samples per Measure

The *FreeSpace* engine's dynamic music capabilities are defined using the number of measures (*m*) and samples per measure (*s*). Using *m*, the engine parses the audio track by dividing it into individual parts, called *measures*; if an event happens that would change the music being played, the game will play the current audio track until the end of the current measure, whereupon it segues into the correct audio track via a built-in crossfade. The length of the measures is defined using *s*, which is the number of *samples* in a measure.

*m* is user-defined and can be set to a decimal point for greater fine-tuning.

*s* is calculated using the following formula:
```
s = 2 x sample rate (hertz) x total audio length (seconds; up to 3 decimal places) / m
```

Depending on the *m* and *s* values declared, the game may only play a portion of the actual audio file or, if the values result in it attempting to play the audio file beyond its actual length, playing nothing for a period of time after it finishes playing the actual audio file.

In the case of `FS2_Amb_A01.wav`, it is defined in `music.tbl` as an audio track that has 25 *m* and 105,840 *s*, so even though the file has an *actual* audio length of 62.556 seconds, the engine only ever parses the first 60 seconds of the audio file as its *total* audio length (hence omitting the last 2.556 seconds), split into 25 measures. This is because a reversal of the above calculation for *s* results in:
```
105,840 (s) x 25 (m) / 22,050 (hertz) / 2 = 60.000 seconds
```

**The length of the measures in the audio track *cannot* be individually adjusted by any means** - *s* applies to **all** the measures in the audio file with **no exceptions**.

##### Alternate Calculation Method

If defining *m* and *s* is too time-consuming, the following formulae can be used to still make use of the *FreeSpace* engine's dynamic music capabilities, and would result in a maximum of 2 seconds' silence (since each measure would be 2 seconds long):
```
m = audio file length (seconds; up to 3 decimal places) / 2
s = 4 x sample rate (hertz)
```

As an example, if we use the above alternate formulae with the *actual* audio length of `FS2_Amb_A01.wav`, we get the following:
```
m = 62.556 seconds / 2 = 31.278
s = 4 x 22,050 (hertz) = 88,200
```

*m* would be rounded to the nearest decimal point. Hence, we would input the following as the alternate length for `FS2_Amb_A01.wav` in `music.tbl`:
```
$Name:	FS2_Amb_A01.wav				31.3	88200	; * Ambience
```

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

### $String:

FSO includes an alternate way to parse individual tracks.

Instead of using `$Name:`, `$String:` can be used, with the `*String*` syntax being the situation reference, such as:
```
$Normal 1:	FS2_Amb_A01.ogg				25.0	105840	; * Ambience
```

Note that this system follows a different situation order from parsing `$Name:`, and it also cannot be changed under any circumstances. Any line can be left out, but the ones given must be in this order; a line out of order isn't read. The names are not case sensitive. The situation order for parsing `$String:`, as well as the corresponding `$String:` to use for each situation, is as follows:

| Order | Situation | `$String:` to Use | Details |
|---|---|---|---|
| 1 | Ambience | `$Normal 1:` | Normal track - plays when nothing is happening or units are out of combat range; usually segues into Ambience 2 |
| 2 | Ambience 2 | `$Normal 2:` | Normal track - plays when nothing is happening or units are out of combat range; usually segues into Ambience 3 |
| 3 | Ambience 3 | `$Normal 3:` | Normal track - plays when nothing is happening or units are out of combat range; usually segues into Ambience |
| 4 | Allied Arrival during Ambience | `$Ally arrival 1:` | Plays when a friendly unit arrives while any Ambience track is playing |
| 5 | Allied Arrival during Battle | `$Ally arrival 2:` | Plays when a friendly unit arrives while any Battle track is playing |
| 6 | Enemy Arrival during Ambience | `$Enemy arrival 1:` | Plays when a hostile unit arrives while any Ambience track is playing |
| 7 | Enemy Arrival during Battle | `$Enemy arrival 2:` | Plays when a hostile unit arrives while any Battle track is playing |
| 8 | Battle 1 | `$Battle 1:` | Battle track 1 - plays when units are in combat range; usually segues into Battle 2 |
| 9 | Battle 2 | `$Battle 2:` | Battle track 2 - plays when units are in combat range; usually segues into Battle 3 |
| 10 | Battle 3 | `$Battle 3:` | Battle track 3 - plays when units are in combat range; usually segues into Battle 1 |
| 11 | Goal Failed | `$Failure 1:` | Plays when the primary goals of a mission are failed |
| 12 | Victory 1 | `$Victory 1:` | Plays when a goal is completed |
| 13 | Victory 2 | `$Victory 2:` | Plays when all goals are completed and there are absolutely no hostile units present; is only called once |
| 14 | Player Dies | `$Dead 1:` | Plays when the player is killed and goes to the death screen |

## Menu Music

Menu music tracks are defined in the lines between `#Menu Music Start` and `#Menu Music End`. ![FSO 22.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.2.svg) There is no limit on the number of tracks (older versions allowed 50).

Each menu music track has the following 2 lines in order:
1. `$Name:`
1. `$Filename:`

For the subsections, refer to the code used to call `FS2_Brief_01.wav` in-game, which is:
```
$Name:		Brief1
$Filename:	FS2_Brief_01.wav			; briefing music 1
```

### $Name:

The first line always starts with `$Name:`, followed by the name of the audio track, which is mainly referred to in FRED and its variants. A name that already exists edits that track.
```
$Name:		Brief1
```

![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg)

- The following flag can be added after `$Name:` in modular tables:

### +nocreate

Only for editing: the track is changed only if one with this name already exists. If it doesn't, the track is skipped.

### $Filename:

The last line starts with `$Filename:`, followed by the file name of the audio track. It is optional when editing. **none** (or none.wav) clears the track. A file that doesn't exist (in any supported audio format) makes the track unusable, without a warning.
```
$Filename:	FS2_Brief_01.wav			; briefing music 1
```

## Format Sample

The following is a heavily truncated version of `music.tbl` located within `Root_FS2.vp`.
```
;
; music.tbl
;
; Specifies which .wav files to use for the different sections of event-driven sound-tracks
;
; format:  $Name:  filename.wav  num_measures (samples_per_measure)
;
#SoundTrack Start	         				; SoundTrack 0
$Soundtrack Name: 1: Genesis
$Name:	FS2_Amb_A01.wav				25.0	105840	; * Ambience
$Name:	FS2_AArv_A01.wav			1.8	105840	; * Allied Arrival during Normal
$Name:	FS2_Arv_B01.wav				3.8	81415	; * Enemy Arrival during Normal
$Name:	FS2_Btl_A01.wav				25.8	66150	; * Battle 1
$Name:	FS2_Btl_A02.wav				27.8	81415	; * Battle 2
$Name:	FS2_Btl_A03.wav				39.8	81415	; * Battle 3
$Name:	FS2_AArv_A01.wav			1.8	105840	; * Allied Arrival during Battle
$Name:	FS2_Arv_B02.wav				3.8	81415	; * Enemy Arrival during Battle
$Name:	FS2_Vict_A01.wav			2.8	52920	; * Victory 1 ( goal completed )
$Name:	FS2_Vict_A02.wav			30.8	52920	; * Victory 2	( all enemies dead, goals completed )
$Name:	FS2_Dth_A01.wav				23.8	132300	; * Goal Failed
$Name:	FS2_Dth_A01.wav				23.8	132300	; * Player dies
#SoundTrack End							; End Soundtrack 0
;
#SoundTrack Start	     					; SoundTrack 1
$Soundtrack Name: 2: Exodus
$Name:	FS2_Amb_B01.wav				17.0	132300	; * Ambience
$Name:	FS2_AArv_A01.wav			1.8	105840	; * Allied Arrival during Normal
$Name:	FS2_Arv_B01.wav				4.8	81415	; * Enemy Arrival during Normal
$Name:	FS2_Btl_B01.wav				39.8	132300	; * Battle 1
$Name:	FS2_Btl_B02.wav				23.8	75600	; * Battle 2
$Name:	FS2_Btl_B03.wav				31.8	75600	; * Battle 3
$Name:	FS2_AArv_A01.wav			1.8	105840	; * Allied Arrival during Battle
$Name:	FS2_Arv_B04.wav				4.8	75600	; * Enemy Arrival during Battle
$Name:	FS2_Vict_A01.wav			2.8	52920	; * Victory 1 ( goal completed )
$Name:	FS2_Vict_B02.wav			40.8	88200	; * Victory 2	( all enemies dead, goals completed )
$Name:	FS2_Dth_A01.wav				23.8	132300	; * Goal Failed
$Name:	FS2_Dth_A01.wav				23.8	132300	; * Player dies
#SoundTrack End							; End Soundtrack 1
;
#Menu Music Start
$Name:		Brief1
$Filename:	FS2_Brief_01.wav			; briefing music 1
```

```
$Name:		Brief2
$Filename:	FS2_Brief_02.wav			; briefing music 2
```

```
$Name:		Brief3
$Filename:	FS2_Brief_03.wav			; briefing music 3
#Menu Music End
```

## Related Links

- [Music tables 301 | Hard Light Productions Forums](https://www.hard-light.net/forums/index.php?topic=89836.msg1787087)
- [Archived post about music.tbl from the FreeSpace Developer List](https://web.archive.org/web/20010314181125/http://ml.warpcore.org/fdl/199911/msg00204.html)
