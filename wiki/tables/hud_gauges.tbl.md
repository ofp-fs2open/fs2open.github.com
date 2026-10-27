> **Revision information**
> OFP Revision: [#122](https://github.com/ofp-fs2open/fs2open.github.com/pull/122)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **hud_gauges.tbl** changes the HUD. Not all gauges can be modified, but custom gauges can be added that mimic the built-in ones (such as the wingman status display).

HUD elements are called gauges, and each gauge has options to customize its appearance and, in some cases, its behavior.

This table is one of the Modular Tables and can be extended with **xxx-hdg.tbm**.

Code: [/hud/hudparse.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/hud/hudparse.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in.

## Contents

1. [#HUD Config Settings](#hud-config-settings)
2. [HUD Configurations](#hud-configurations)
3. [#Gauge Config](#gauge-config)
4. [Gauge Configuration Options](#gauge-configuration-options)

Every setting with "Color" in its name is also accepted spelled "Colour".

---

## #HUD Config Settings

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

Optional, and must come first in the table if used. These set up the HUD options screen: its preview, its color presets and its default preset.

### $Show Default HUD:

- Whether the default HUD layout is listed in the HUD options screen. It's always listed if there are no named layouts (see [$Name:](#name)).
- Default: YES
- Syntax: **Boolean**

### $Head Animation:

- The head animation shown in the HUD options preview.
- Syntax: **String**, file name

### $Shield Gauge Ship:

- The ship class whose shield shape the preview's player shield gauge shows. An unknown class logs a warning.
- Syntax: **String**, ship class name

### $Shield Gauge Target Ship:

- The ship class whose shield shape the preview's target shield gauge shows. An unknown class logs a warning.
- Syntax: **String**, ship class name

### $Primary Weapons:

- The primary weapons shown in the preview's weapons gauge.
- Syntax: **( "Weapon 1" "Weapon 2" ... )**

### $Secondary Weapons:

- The secondary weapons shown in the preview's weapons gauge.
- Syntax: **( "Weapon 1" "Weapon 2" ... )**

### $Example Wing Names:

- The wing names shown in the preview's wingman status gauge, up to five.
- Default: Alpha, Beta, Gamma, Delta, Epsilon
- Syntax: **( "Wing 1" "Wing 2" ... )**

### $Color Preset 1:, $Color Preset 2:, $Color Preset 3:

- The three HUD color presets the player can pick in the HUD options. Each is optional and needs all of the fields below.

#### +Name:

- The preset's name.
- Syntax: **String**

#### +XSTR ID:

- The translation index for the name.
- Syntax: **Integer**

#### +Color:

- The color.
- Syntax: **Integer** red, **Integer** green, **Integer** blue, each 0 to 255; values outside are clamped

#### +Default

- Optional. Makes this preset the default.

### $Default Preset File:

- The HUD configuration preset file used by default.
- Default: hud_3.hcf
- Syntax: **String**, file name

---

## HUD Configurations

The following entries are used on the HUD system, but have not been grouped with any particular gauge. They can be preceded by an optional `#HUD Global Settings` line.

### $Load Retail Configuration:

Optional. If yes, any gauge missing from the HUD gauge definitions below will be loaded using the retail defaults.

If no, any missing (default) gauge will prompt an error.

- Syntax: **Boolean**

### $Color:

Optional. Sets the default HUD color for all gauges.

- Syntax: (**Integer** Red, **Integer** Green, **Integer** Blue)
- Valid Values: all values from 0 to 255

### $Font:

Optional. Sets the font type to use for the HUD. This can either be a string or a number. A string references a font with a specific name while a number references a font with a specific index in the fonts table. The number option is deprecated and is only there for backwards-compatibility. Use the name if possible.

- Syntax: **String** or **Integer** font index number
- Valid Values: Any name or font index as defined in [fonts.tbl](fonts.tbl)

### $Chase View Only:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) Optional. Sets gauges to be displayed only in Chase View.

- Syntax: **Boolean**
- Default Value: **False**

### $Max Directives:

Optional. Sets the default maximum number of directives displayed simultaneously.

- Syntax: **Integer** maximum number of directives
- Valid Values: Any positive number
- Default Value: **5**

### $Max Escort Ships:

Optional. Sets the default maximum number of escort ships to be shown in the escort list.

- Syntax: **Integer**
- Valid Values: Any positive number

> **Warning:** Entries more than 16 will cause the list to overflow into the weapons list for resolutions lower than 1024x768.

### $Length Unit Multiplier:

Optional. Sets the multiplier to applied to the reported distances. For example, if the multiplier is 0.5, then the distance reported to the player will (or at least should) be 0.5 x actual distance.

- Syntax: **Float**
- Valid Values: Any positive number

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) **$Distance Unit Multiplier:** is accepted as a synonym

### $Speed Unit Multiplier:

Optional. Analogous to [$Length Unit Multiplier](#length-unit-multiplier), this sets the multiplier to be applied to all speeds reported to the player. For example, if the multiplier is 0.5, the the speed reported to the player will (or at least should) be 0.5 x actual speed.

- Syntax: **Float**
- Valid Values: Any positive number
- Default Values:
**The value used in $Length Unit Multiplier**, or
**1**, if $Length Unit Multiplier is undefined

### $Wireframe Targetbox:

Optional. Defines the default draw method for the target viewer (a.k.a. targetbox).

- Syntax: **Integer** Draw method ID
- Valid Values:
**0** - Standard with full, bright lighting
**1** - Wireframe
**2** - Standard with proper lighting (i.e. lit the same as the actual target)

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) **3** - Untextured with full bright lighting (i.e. drawn with a single flat color)

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

### $Wireframe Color Override:

Optional. Allows using a static color for the target monitor wireframe instead of one based on the target's IFF.
- Syntax: ( Color ), three integers, values from 0 to 255; red, green and blue respectively
- Example: ( 0, 255, 0 )

### $Targetbox Shader Effect:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) Optional. If set, defines a unique ID for a shader effect to be used when rendering a ship for the targeting box. The ID will be passed to the main shader as in [#Ship_Effects](post_processing.tbl#ship_effects), but a table entry for the effect is not needed.

- Syntax: **Integer**
- Valid Values: Any positive number

### $Lock Wireframe Mode:

Optional. If set, prevents targetbox draw method from being changed during the gaming session.
- Syntax: **Boolean**

### $Scale Gauges:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) Optional. If set, all gauges will use this value as the default for scaling individual gauge sizes (i.e. doesn't affect gauges that use Position:, and doesn't affect the values of Offset:.  In addition, if "$Load Retail Configuration:" is true then all unspecified retail gauges will use this value.

If not set, defaults to yes.

- Syntax: **Boolean**

### $Force Scaling Above:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) Optional. If gauge scaling is disabled, then gauges will be scaled as though the base resolution were this resolution or the real base resolution, whichever is greater.

Use this to disable scaling at lower resolutions while preventing gauges from getting too small at higher resolutions.

To disable this feature, use **(-1, -1)**.

- Syntax: (**Integer** width, **Integer** height)

### $Reticle Style:

Optional. Defines which aiming reticle to use.
- Syntax: **String**
- Valid Values:
**FS1** - uses ***reticle1_fs1.ani***
**FS2** - uses ***reticle1.ani***

## #Gauge Config

The following entries define default gauge appearance and behavior. Each gauge has the option of overriding these options.

### $Ship:

Deprecated and superseded by $Ships. Using it logs a note. If defined, the following config data will only be used if the player ship is of the supplied ship class. Can be defined multiple times within a #Gauge config group to allow unique HUD configurations per ship class.

If not defined, all following config data will be applied to all ships.

- Syntax: **String**, ship class name

### $Ships:

Optional. Can be used to define a list of ships the hud definition that follows will be applied to. Can be defined multiple times within a #Gauge config group to allow unique HUD configurations per ship class.

If not defined, all following config data will be applied to all ships.

- - Syntax: **( "Class name 1" "Class name 2" )**, ship class names formatted like a flag list.

#### $Load Retail Configuration:

Optional. Overrides the [$Load Retail Configuration](#load-retail-configuration-1) global option for this ship class.

- Syntax: **Boolean**

#### $Color:

Optional. Overrides the [$Color](#color) global option for this ship class.

- Syntax: See [$Color](#color)

#### $Font:

Optional. Overrides the [$Font](#font) global option for this ship class.

- Syntax: See [$Font](#font)

#### $Chase View Only:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) Optional. Overrides the [$Chase View Only](#chase-view-only-1) global option for this ship class.

- Syntax: See [$Chase View Only](#chase-view-only-1)

### $Name:

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

Optional, instead of `$Ship:` or `$Ships:`. Gives this HUD layout a name. Ships use it by listing the name in `$HUD Gauge Configs:` in [ships.tbl](ships.tbl), and it's offered as a layout in the HUD options screen.

- Syntax: **String**

#### $Show in HUD Config:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) Optional. Whether this layout is listed in the HUD options screen.

- Default: YES
- Syntax: **Boolean**

#### $Shield Gauge Ship:, $Shield Gauge Target Ship:, $Primary Weapons:, $Secondary Weapons:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) Optional. The same as in [#HUD Config Settings](#hud-config-settings), for this layout's preview.

#### $Load Retail Configuration:, $Color:, $Font:, $Chase View Only:

Optional. The same as for `$Ships:` above.

### $Base:

Required. Defines the base resolution for this HUD configuration, upon which other gauge options (such as [Position:](#position) / [Origin:](#origin) / [Offset:](#offset)) will use as its coordinate system.

- Syntax: (**Integer** width, **Integer** height)
- Valid Values: All supported resolutions (?)

### $Scale Gauges:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) Optional. If set, will configure gauges Scaling behavior for gauges in this #Gauge Config section. i.e. overriding the default, or the value specified at the top of the file.  Individual gauges can override this setting.

- Syntax: **Boolean**

### $Force Scaling Above:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) Optional. If gauge scaling is disabled, then gauges will be scaled as though the base resolution were this resolution or the real base resolution, whichever is greater.

Use this to disable scaling at lower resolutions while preventing gauges from getting too small at higher resolutions.

To disable this feature, use **(-1, -1)**.

- Syntax: (**Integer** width, **Integer** height)

### $Required Aspect:

Optional. If set, this configuration will only be loaded for the specified aspect ratios.

Used as a filter when there are multiple $Base: resolutions in the table

- Syntax: **String**
- Valid Values:
"Full Screen" (4:3, aspect ratio < 1.5)
"Wide Screen" (16:9, 16:10, or similar, aspect ratio >= 1.5)

### $Min:

Optional. Minimum resolution for this config. Any resolution lower than specified will not use this configuration.

Used as a filter when there are multiple $Base: resolutions in the table

- Syntax: (**Integer** width, **Integer** height)
- Valid Values: All supported resolutions (?)

### $Max:

Optional. Maximum resolution for this config.  Any resolution higher than specified will not use this configuration.

Used as a filter when there are multiple $Base: resolutions in the table

- Syntax: (**Integer** width, **Integer** height)
- Valid Values: All supported resolutions (?)

### $Gauges:

Following entries define gauge-specific behavior. For the most part, each gauge has its own overrides that, if not defined, would otherwise use the global values.

Since many gauges have the same configuration options, each gauge entry has a list of valid sub-entries. Unless stated otherwise, these options must be defined in the same order as they are shown, starting from the top and ending at the bottom.

#### +Custom:

Adds a custom gauge type.

- Valid Sub-Entries:
  - [Position:](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Name:](#name)
  - [Text:](#text)
  - [X Offset:](#x-offset) (Optional)
  - [Y Offset:](#y-offset) (Optional)
  - [Gauge Type:](#gauge-type)
  - [Slew:](#slew) (Optional)
  - [Active by default:](#active-by-default) (Optional)
  - [Filename:](#filename)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

#### +Scripted Gauge:

Adds a scripted gauge type.

- Valid Sub-Entries:
  - [Position:](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Name:](#name)

#### +Messages:

Adds a gauge that display mission messages.

- Valid Sub-Entries:
  - [Position](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Max Lines:](#max-lines) (Optional)
  - [Max Width:](#max-width) (Optional)
  - [Line Height](#line-height) (Optional)
  - [Total Lifetime:](#total-lifetime) (Optional)
  - [Scroll Time](#scroll-time) (Optional)
  - [Step Size:](#step-size) (Optional)
  - [Hidden By Comms Menu:](#hidden-by-comms-menu) (Optional)

#### +Training Messages:

Adds a text box, like the ones seen in the FS1 and FS2 training missions.

- Valid Sub-Entries:
  - [Position](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")

#### +Support:

Adds a text box that shows the Support ship's status and ETA

- Valid Sub-Entries:
  - [Position](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename)
  - [Header Offsets:](#header-offsets) (Optional)
  - [Text Y-offset:](#text-y-offset) (Optional)
  - [Dock Status X-offset:](#dock-status-x-offset) (Optional)
  - [Dock Time X-offset:](#dock-time-x-offset) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Enable Rearm Timer:` (Optional boolean. Enables the countdown timer for rearm and repair. Overrides the commandline setting.)

#### +Damage:

Adds a gauge that shows how much damage a target (or the player?) has.

- Valid Sub-Entries:
  - [Position](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Top Background Filename:](#top-background-filename) (Optional)
  - [Entry Background Filename:](#entry-background-filename) (Optional)
  - [Bottom Background Filename:](#bottom-background-filename) (Optional)
  - [Header Offsets:](#header-offsets) (Optional)
  - [Hull Integrity Offsets](#hull-integrity-offsets) (Optional)
  - [Hull Integrity Value X-offset](#hull-integrity-value-x-offset) (Optional)
  - [Top Background Height:](#top-background-height) (Optional)
  - [Subsystem Entry Height:](#subsystem-entry-height) (Optional)
  - [Subsystem List Start Offsets:](#subsystem-list-start-offsets) (Optional)
  - [Subsystem Entry Value X-offset:](#subsystem-entry-value-x-offset) (Optional)
  - [Bottom Background Offset:](#bottom-background-offset) (Optional)
  - ![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) [Always Display:](#always-display) (Optional)

#### +Wingman Status:

Adds a gauge that monitors the status of your wing and squadron.

- Valid Sub-entries:
  - [Position](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Left Background Filename:](#left-background-filename) (Optional) (needs an animation with one frame)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [First Background Filename:](#first-background-filename) (Optional) (needs an animation with one frame)
  - [Entry Background Filename:](#entry-background-filename) (Optional) (needs an animation with one frame)
  - [Right Background Filename:](#right-background-filename) (Optional) (needs an animation with one frame)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Last Background Filename:](#last-background-filename) (Optional) (needs an animation with one frame)
  - [Dot Filename:](#dot-filename) (Optional) (needs an animation with two frames, 1st is for alive wingmen, 2nd is for dead or missing wingmen)
  - [Header Offsets:](#header-offsets) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Fixed Header Position:](#fixed-header-position) (Optional)
  - [Left Background Width:](#left-background-width) (Optional)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) `First Background Size:` (Optional)
  - [Entry Width:](#entry-width) (Optional)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) `Entry Size:` (Optional)
  - `Right Background Offset:` (Optional)
  - `Last Background Offset:` (Optional)
  - [Single Wing Offsets:](#single-wing-offsets) (Optional)
  - [Multiple Wing Start Offsets:](#multiple-wing-start-offsets) (Optional)
  - [Wing Name Offsets:](#wing-name-offsets) (Optional)
  - [Dot Offsets:](#dot-offsets) (Optional)
  - [Expansion Mode:](#expansion-mode) (Optional)
  - ![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg) [Use Full Wing Names:](#use-full-wing-names) (Optional)
  - [Use Expanded Colors:](#use-expanded-colors) (Optional)
  - ![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) [Wingname Align Mode:](#wingname-align-mode) (Optional)

#### +Auto Speed:

Adds the Auto-Speed mode gauge.

- Valid Sub-entries:
  - [Position](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (needs animation with 4 frames, 3rd is without auto-match-speed enabled, 4th is with... and frames 0 & 1 are not used, done this way because retail fs2 uses the same animation file for the auto speed & auto targeting gauges)
  - [Auto Offsets:](#auto-offsets) (Optional)
  - [Speed Offsets:](#speed-offsets) (Optional)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) `On Text Color:` (Optional)
  - `Off Text Color:` (Optional)

#### +Auto Target:

Adds the Auto-target mode gauge.

- Valid Sub-entries:
  - [Position](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (needs animation with 2 frames, 1st is without auto-targeting enabled, 2nd is with)
  - [Auto Offsets:](#auto-offsets) (Optional)
  - [Target Offsets:](#target-offsets) (Optional)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) `On Text Color:` (Optional)
  - `Off Text Color:` (Optional)

#### +Countermeasures:

Adds a gauge that monitors the amount of remaining countermeasures the player's ship currently has.

- Valid Sub-entries:
  - [Position](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (needs an animation with a single frame)
  - [Text Offsets:](#text-offsets) (Optional)
  - [Value Offsets:](#value-offsets) (Optional)

#### +Talking Head:

Adds a video window to display pilot message videos (a.k.a talking heads).

- Valid Sub-entries:
  - [Position](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename)
  - [Header Offsets:](#header-offsets) (Optional)
  - [Animation Offsets:](#animation-offsets) (Optional)
  - [Animation Background Size:](#animation-background-size) (Deprecated)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Animation Size:](#animation-size) (Optional)

#### +Directives:

Adds a gauge that displays all currently active directives (includes current , completed, and failed)

- Valid Sub-entries:
  - [Position](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Top Background Filename:](#top-background-filename) (Optional) (needs animation with one frame)
  - [Entry Background Filename:](#entry-background-filename) (Optional) (needs animation with one frame)
  - [Bottom Background Filename:](#bottom-background-filename) (Optional) (needs animation with one frame)
  - [Header Offsets:](#header-offsets) (Optional)
  - [Top Background Height:](#top-background-height) (Optional)
  - [List Start Offsets:](#list-start-offsets) (Optional)
  - [Entry Height:](#entry-height) (Optional)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Bottom Background Offset:](#bottom-background-offset) (Optional)
  - `Max Line Width:` (Optional)
  - ![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) [Key Line X Offset:](#key-line-x-offset) (Optional)

#### +Weapons:

Adds a weapon status and ammunition gauge.

- Valid Sub-entries:
  - [Position](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Primary List Top Background Filename:](#primary-list-top-background-filename) (Optional) (needs animation with a single frame)
  - [Alt Ballistic Filename:](#alt-ballistic-filename)  (Optional) (needs animation with a single frame)
  - [Primary List Middle Background Filename:](#primary-list-middle-background-filename) (Optional) (needs animation with a single frame)
  - [Alt Ballistic Filename:](#alt-ballistic-filename)  (Optional) (needs animation with a single frame)
  - [Primary List Bottom Background Filename:](#primary-list-bottom-background-filename) (Optional) (needs animation with a single frame)
  - [Alt Ballistic Filename:](#alt-ballistic-filename)  (Optional) (needs animation with a single frame)
  - [Secondary List Top Background Filename:](#secondary-list-top-background-filename) (Optional) (needs animation with a single frame)
  - [Alt Ballistic Filename:](#alt-ballistic-filename)  (Optional) (needs animation with a single frame)
  - [Secondary List Entry Background Filename:](#secondary-list-entry-background-filename) (Optional) (needs animation with a single frame)
  - [Alt Ballistic Filename:](#alt-ballistic-filename)  (Optional) (needs animation with a single frame)
  - [Secondary List Bottom Background Filename:](#secondary-list-bottom-background-filename) (Optional) (needs animation with a single frame)
  - [Alt Ballistic Filename:](#alt-ballistic-filename)  (Optional) (needs animation with a single frame)
  - [Header Offsets:](#header-offsets) (Optional)
  - [Alt Ballistic Offsets:](#alt-ballistic-offsets) (Optional)
  - [Top Primary BackgroundX-offset:](#top-primary-background-x-offset) (Optional)
  - [Alt Ballistic X-offset:](#alt-ballistic-x-offset) (Optional)
  - [Text X-offset:](#text-x-offset) (Optional)
  - [Alt Ballistic X-offset:](#alt-ballistic-x-offset) (Optional)
  - [Top Primary Frame Height:](#top-primary-frame-height) (Optional)
  - [Top Secondary Frame Height:](#top-secondary-frame-height) (Optional)
  - [Primary List Start Y-offset:](#primary-list-start-y-offset) (Optional)
  - [Secondary List Start Y-offset:](#secondary-list-start-y-offset) (Optional)
  - [Primary Weapon Ammo X-offset:](#primary-weapon-ammo-x-offset) (Optional)
  - [Primary Weapon Link X-offset:](#primary-weapon-link-x-offset) (Optional)
  - [Primary Weapon Name X-offset:](#primary-weapon-name-x-offset) (Optional)
  - [Secondary Weapon Ammo X-offset:](#secondary-weapon-ammo-x-offset) (Optional)
  - [Secondary Weapon Linked X-offset:](#secondary-weapon-linked-x-offset) (Optional)
  - [Secondary Weapon Name X-offset:](#secondary-weapon-name-x-offset) (Optional)
  - [Secondary Weapon Reload X-offset:](#secondary-weapon-reload-x-offset) (Optional)
  - [Primary Weapon Entry Height:](#primary-weapon-entry-height) (Optional)
  - [Secondary Weapon Entry Height:](#secondary-weapon-entry-height) (Optional)

#### +Objective notify:

Adds a Objective notifier gauge. This gauge alerts the player when receiving a new objective, or when an objective has been completed or failed. Additionally, this gauge alerts the player when entering subspace, and when a Red Alert mission orders are being received (prior to a forced subspace jump).

- Valid Sub-entries:
  - [Position](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename)
  - [Objective Text Y-offset:](#objective-text-y-offset) (Optional)
  - [Objective Value Y-offset:](#objective-value-y-offset) (Optional)
  - [Subspace Text Y-offset:](#subspace-text-y-offset) (Optional)
  - [Subspace Value Y-offset:](#subspace-value-y-offset) (Optional)
  - [Red Alert Text Y-offset:](#red-alert-text-y-offset) (Optional)
  - [Red Alert Value Y-offset:](#red-alert-value-y-offset) (Optional)

#### +Squad Message

Adds the squad message gauge. In FS1 and FS2, this gauge is used to send commands to your squad. (?)

- Valid sub-entries:
[Position](#position)  (Optional)

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
[Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
[Canvas Size:](#canvas-size) (Optional)
[Display Offset:](#display-offset) (Optional)
[Display Size:](#display-size)

[Color:](#color) (Optional)
[Font:](#font) (Optional)

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")

[Slew:](#slew) (Optional)
[Top Background Filename:](#top-background-filename) (Optional)
[Entry Background Filename:](#entry-background-filename) (Optional)
[Bottom Background Filename:](#bottom-background-filename) (Optional)
[Header Offsets:](#header-offsets) (Optional)
[List Start Offsets:](#list-start-offsets) (Optional)
[Top Background Height:](#top-background-height) (Optional)
[Entry Height:](#entry-height) (Optional)

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Bottom Background Offset](#bottom-background-offset) (Optional)

[Command X-offset:](#command-x-offset) (Optional)
[Page Up Offsets:](#page-up-offsets) (Optional)
[Page Down Offsets:](#page-down-offsets) (Optional)

#### +Lag:

Adds a Lag gauge.

- Valid Sub-entries:
  - [Position](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename)

#### +Mini Target Shields:

Adds a mini Target shields gauge. In FS1 and FS2, the Mini Target Shield gauge is the one right below the center reticle, and shows the hull damage in %

- Valid Sub-entries:
  - [Position](#position)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (animation needs 8 frames; 0->3 are the top, right, bottom & left shield quads; 4->7 are the same except brighter for when the quad is flashing from being hit)
  - [3 Digit Hull Offsets:](#3-digit-hull-offsets) (Optional)
  - [2 Digit Hull Offsets:](#2-digit-hull-offsets) (Optional)
  - [1 Digit Hull Offsets:](#1-digit-hull-offsets) (Optional)

#### +Player Shields:

Adds a gauge that shows the shield and hull status icon of the player's craft or ship.

- Valid Sub-entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)

#### +Target Shields:

Adds a gauge that shows the shield and hull status icon of the targeted craft or ship.

- Valid Sub-entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)

#### +Escort View:

Adds an escort monitor/view gauge. This gauge monitors the status of all ships assigned to the player to escort.

- Valid Sub-entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Top Background Filename:](#top-background-filename) (Optional)
  - [Entry Background Filename:](#entry-background-filename) (Optional)
  - [Bottom Background Filename:](#bottom-background-filename) (Optional)
  - [Entry Height](#entry-height) (Optional)
  - [Entry Stagger Width:](#entry-stagger-width) (Optional)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Bottom Background Offset:](#bottom-background-offset) (Optional)
  - [Header Text](#header-text) (Optional)
  - [Header Offsets:](#header-offsets) (Optional)
  - [List Start Offsets:](#list-start-offsets) (Optional)
  - [Hull X-offset](#hull-x-offset) (Optional)
  - [Name X-offset](#name-x-offset) (Optional)
  - [Status X-offset](#status-x-offset) (Optional)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Ship Name Max Width:](#ship-name-max-width) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Right-Align Ship Names:](#right-align-ship-names) (Optional)

#### +Mission Time:

Adds a gauge that displays the current mission time.

- Valid Sub-entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (Optional)
  - [Text Offsets:](#text-offsets) (Optional)
  - [Value Offsets:](#value-offsets) (Optional)

#### +ETS Retail:

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg) Draws the three other ETS gauges as a single gauge.  Will automatically rearrange itself (like retail) if shields or other systems are missing.

- Valid Sub-entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - ![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg) [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (Optional)
  - [Foreground Clip Height:](#foreground-clip-height) (Optional)
  - [Letter Offsets:](#letter-offsets) (Optional)
  - [Top Offsets:](#top-offsets) (Optional)
  - [Bottom Offsets:](#bottom-offsets) (Optional)
  - [Gauge Offset:](#gauge-offset) (Optional)

#### +ETS Weapons:

Adds a gauge that displays the energy transfer rate for the energy weapons.

- Valid Sub-entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (Optional)
  - [Foreground Clip Height:](#foreground-clip-height) (Optional)
  - [Letter Offsets:](#letter-offsets) (Optional)
  - [Top Offsets:](#top-offsets) (Optional)
  - [Bottom Offsets:](#bottom-offsets) (Optional)

#### +ETS Engines:

Adds a gauge that displays the energy transfer rate for the afterburner/engines.

- Valid Sub-entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (Optional)
  - [Foreground Clip Height:](#foreground-clip-height) (Optional)
  - [Letter Offsets:](#letter-offsets) (Optional)
  - [Top Offsets:](#top-offsets) (Optional)
  - [Bottom Offsets:](#bottom-offsets) (Optional)

#### +ETS Shields:

Adds a gauge that displays the energy transfer rate for the shields.

- Valid Sub-entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (Optional)
  - [Foreground Clip Height:](#foreground-clip-height) (Optional)
  - [Letter Offsets:](#letter-offsets) (Optional)
  - [Top Offsets:](#top-offsets) (Optional)
  - [Bottom Offsets:](#bottom-offsets) (Optional)

#### +Target Monitor:

Adds a target monitor gauge that shows the targeted ship's relative orientation and hull strength, among other things.

- Valid Sub-entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Monitor Filename:](#monitor-filename) (Optional) (needs animation with 2 frames, 1st is the background for the target, 2nd is the border; rendered last)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) `Monitor Alpha Mask Filename:` (Optional) (needs animation with a single frame)
  - [Integrity Bar Filename:](#integrity-bar-filename) (Optional) (needs animation with 2 frames, 1st is a zero/dark filled bar, 2nd is a max/bright filled bar)
  - [Viewport Offsets:](#viewport-offsets) (Optional)
  - [Viewport Size:](#viewport-size) (Optional)
  - [Integrity Bar Offsets:](#integrity-bar-offsets) (Optional)
  - [Integrity Bar Foreground Clip Height:](#integrity-bar-foreground-clip-height) (Optional)
  - [Status Offsets:](#status-offsets) (Optional)
  - [Name Offsets:](#name-offsets) (Optional)
  - [Class Offsets:](#class-offsets) (Optional)
  - [Distance Offsets:](#distance-offsets) (Optional)
  - [Speed Offsets:](#speed-offsets) (Optional)
  - [Hull Offsets:](#hull-offsets) (Optional)
  - [Cargo Contents Offsets:](#cargo-contents-offsets) (Optional)
  - [Cargo Scan Start Offsets:](#cargo-scan-start-offsets) (Optional)
  - [Cargo Scan Size:](#cargo-scan-size) (Optional)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) `Subsystem Name Offsets:` (Optional)
  - `Subsystem Integrity Offsets:` (Optional)
  - `Disabled Status Offsets:` (Optional)
  - `Desaturate:` (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Wireframe:` (Optional. Valid values match $Wireframe Targetbox: and will override the global setting)
  - `Wireframe Color:` (Optional. Color. Takes three ints 0-255, R,G,B; will override the global setting)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) [Cargo Scan Type:](#cargo-scan-type) (Optional)

#### +Extra Target Data:

Adds a gauge that displays "extra" target data, such as its current orders and docking status.

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (Optional) (needs an animation with 1 frame)
  - [Bracket Offsets:](#bracket-offsets) (Optional)
  - [Dock Offsets:](#dock-offsets) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Dock Max Width:](#dock-max-width) (Optional)
  - [Order Offsets:](#order-offsets) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Order Max Width:](#order-max-width) (Optional)
  - [Time Offsets:](#time-offsets) (Optional)

#### +Radar:

Adds a standard radar gauge

- Valid Sub-entries:

  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Ngon Sides:](#ngon-sides) (Optional)
  - [Ngon Offset:](#ngon-sides) (Optional)
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Filename:](#filename) (Optional) (needs animation with 2 frames, only the 2nd frame is used)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Radar Center Offsets:](#radar-center-offsets) (Optional)
  - [Radar Size:](#radar-size) (Optional)
  - [Infinity Distance Offsets](#infinity-distance-offsets) (Optional)
  - [Long Distance Offsets:](#long-distance-offsets) (Optional)
  - [Short Distance Offsets:](#short-distance-offsets) (Optional)

#### +Radar Orb:

Adds a 3D orb radar gauge

- Valid Sub-entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Filename:](#filename) (Optional) (needs animation with 2 frames, only the 2nd frame is used)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Radar Center Offsets:](#radar-center-offsets) (Optional)
  - [Radar Size:](#radar-size) (Optional)
  - [Infinity Distance Offsets](#infinity-distance-offsets) (Optional)
  - [Long Distance Offsets:](#long-distance-offsets) (Optional)
  - [Short Distance Offsets:](#short-distance-offsets) (Optional)

#### +Radar BSG:

Adds a BSG style radar (a.k.a DRADIS).

[Position](#position) (Optional)

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
[Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))

[Font:](#font) (Optional)

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")

[Size:](#size) (Optional)
[XY Disc Filename:](#xy-disc-filename) (Optional)
[XZ YZ Disc Filename:](#xz-yz-disc-filename) (Optional)
[Sweep Disc Filename:](#sweep-disc-filename) (Optional)
[Default Contact Filename:](#default-contact-filename) (Optional)
[Unknown Contact Filename:](#unknown-contact-filename) (Optional)
[Cockpit Target:](#cockpit-target) (Optional)
[Canvas Size:](#canvas-size)
[Display Offset:](#display-offset)
[Display Size:](#display-size)
[Loop Volume:](#loop-volume) (Optional)
[Minimum Beep Delay](#minimum-beep-delay) (Optional)

#### +Afterburner Energy:

Adds a gauge that shows the remaining afterburner energy. Note that it only supports a vertical fill.

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (Optional) (needs animation with 2 frames, the 1st is the bar lit for zero energy, the 2nd is the bar lit for max energy)
  - [Foreground Clip Height:](#foreground-clip-height) (Optional)

#### +Weapon Energy:

Adds a gauge that shows the remaining weapon energy. Note that it only supports a vertical fill.

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (Optional) (needs an animation with 4 frames, the 3rd is the bar lit for zero energy, the 2nd is the bar lit for max energy, frames 1 & 2 are not used)
  - [Foreground Clip Height:](#foreground-clip-height) (Optional)
  - [Text Offsets:](#text-offsets) (Optional)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) `Text Alignment:` (Optional)
  - `Always Show Text:` (Optional)
  - `Text Follows:` (Optional)
  - `Show Ballistic Ammo:` (Optional)
  - `Armed Guns List Offsets:` (Optional)
  - `Armed Guns List Alignment:` (Optional)
  - `Armed Guns List Entry Height:` (Optional)

#### +Text Warnings:

Adds a gauge that warns the player (current warnings are EMP, Launch, Evaded, Collision, Blast & Engine Wash).

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)

#### +Center Reticle:

Adds a "custom" center reticle.

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Filename:](#filename) (Optional)
  - [Firepoint display:](#firepoint-display) (Optional)
  - [Firepoint size:](#firepoint-size) (Optional)
  - [Firepoint X coordinate multiplier:](#firepoint-x-coordinate-multiplier) (Optional)
  - [Firepoint Y coordinate multiplier:](#firepoint-y-coordinate-multiplier) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Autoaim Frame:](#autoaim-frame) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Flight Cursor Frame:](#flight-cursor-frame) (Optional)

#### +Throttle:

Adds a gauge that shows the current throttle position.

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Filename:](#filename) (Optional)
  - [Foreground Clip Bottom Y-offset:](#foreground-clip-bottom-y-offset) (Optional)
  - [Foreground Clip Width:](#foreground-clip-width) (Optional)
  - [Foreground Clip Height:](#foreground-clip-height) (Optional)
  - [Afterburner Clip Height:](#afterburner-clip-height) (Optional)
  - [Show Background:](#show-background) (Optional)
  - [Max Speed Label Offsets:](#max-speed-label-offsets) (Optional)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) `Show Max Speed Label:` (Optional)
  - [Min Speed Label Offsets:](#min-speed-label-offsets) (Optional)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) `Show Min Speed Label:` (Optional)
  - [Orbit Center Offsets:](#orbit-center-offsets) (Optional)
  - [Orbit Radius:](#orbit-radius) (Optional)
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) `Current Speed Offsets:` (Optional)
  - `Target Speed Offsets:` (Optional)
  - `Show Percentage:` (Optional)
  - `Glide Status Offsets:` (Optional)
  - `Match Speed Status Offsets:` (Optional)

#### +Threat Indicator:

Adds a gauge that indicates when a lock-on has been achieved by an attacker, and if the player is being shot at.

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Arc Filename:](#arc-filename) (Optional)
  - [Dumbfire Filename:](#dumbfire-filename) (Optional)
  - [Lock Filename:](#lock-filename) (Optional)
  - [Dumbfire Offsets:](#dumbfire-offsets) (Optional)
  - [Lock Offsets:](#lock-offsets) (Optional)

#### +Multiplayer Messages:

Adds a gauge that displays the multiplayer messages.

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")

#### +Voice Status:

Adds a gauge that displays the voice status.

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")

#### +Ping:

Adds a gauge that the ping time to the multiplayer server.

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")

#### +Supernova:

Adds a gauge that warns the player of an impending supernova shock wave.

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")

#### +Lock Indicator:

Adds a Lock Indicator gauge.

- Valid Sub-Entries:
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Lock Filename:](#lock-filename) (Optional)
  - [Locked Filename:](#locked-filename) (Optional)
  - [Lock Center Offsets:](#lock-center-offsets) (Optional)
  - [Locked Center Offsets:](#locked-center-offsets) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) [Loop Locked Animation:](#loop-locked-animation) (Optional)
  - [Blink Locked Animation:](#blink-locked-animation) (Optional)

#### +Offscreen Indicator:

Adds an Offscreen Indicator.

- Valid Sub-Entries:
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")

#### +Target Brackets:

Adds a Target Bracket gauge. This is drawn around current target(s) and talkers.

- Valid Sub-Entries:
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Dot Filename:](#dot-filename) (Optional) (needs an animation with two frames, 1st is for alive wingmen, 2nd is for dead or missing wingmen)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Enable Target Info:` (Optional boolean. Adds the target name and class to the brackets. Overrides the commandline setting.)

#### +Hostile Triangle:

Adds a gauge that points to the nearest attacker, if it is off-screen and is not currently targeted.

- Valid Sub-entries:
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Radius](#radius) (Optional)
  - [Triangle Base:](#triangle-base) (Optional)
  - [Triangle Height:](#triangle-height) (Optional)

#### +Target Triangle:

Adds a gauge that points to the targeted craft, if it is currently off-screen.

- Valid Sub-entries:
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Radius](#radius) (Optional)
  - [Triangle Base:](#triangle-base) (Optional)
  - [Triangle Height:](#triangle-height) (Optional)

#### +Missile Triangles:

Adds a gauge that is used during attempting to lock onto a target when using an aspect seeking missile.

- Valid Sub-entries:
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Radius](#radius) (Optional)
  - [Triangle Base:](#triangle-base) (Optional)
  - [Triangle Height:](#triangle-height) (Optional)

#### +Lead Indicator:

Adds a gauge that leads (is in front) of the target.  Aim your crosshairs at this indicator to increase probability of hitting the target.

- Valid Sub-entries:
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Filename:](#filename) (Optional)
Frame #0: Secondaries lead indicator (Dumbfire)
Frame #1: Secondaries lead indicator (Homing)
Frame #2: Primaries lead indicator
  - [Center Offsets:](#center-offsets) (Optional)

#### +Orientation Tee:

Adds a gauge that indicates the targeted craft's orientation in relation to the player's craft.

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Radius](#radius) (Optional)

#### +Lead Sight:

Adds a gauge that lags (is behind) the target.  Indicates where your shots will end up relative to the target's position.  To score a hit, this gauge should be over the target.  Notably used by [Diaspora](https://wiki.hard-light.net/index.php/Diaspora).

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (Optional)
Frame #0 is used for secondaries
* If only one frame is found in the file, then it is used for primaries instead
Frame #1 is used for primaries

#### +Kills:

Adds a gauge that keeps track of how many kills the player has scored during a mission.

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (Optional)
  - [Text Offsets:](#text-offsets) (Optional)
  - [Value Offsets:](#value-offsets) (Optional)

#### +Weapon Linking:

Adds a gauge that indicates the selected weapons and their firing rates.

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Arc Filename:](#arc-filename) (Optional)
  - [Single Primary Filename:](#single-primary-filename) (Optional)
  - [Double Primary Filename:](#double-primary-filename) (Optional)
  - [Single Secondary Filename:](#single-secondary-filename) (Optional)
  - [Double Secondary Filename:](#double-secondary-filename) (Optional)
  - [Triple Secondary Filename:](#triple-secondary-filename) (Optional)
  - [Single Primary Offsets:](#single-primary-offsets) (Optional)
  - [Double Primary Offsets:](#double-primary-offsets) (Optional)
  - [Single Secondary Offsets:](#single-secondary-offsets) (Optional)
  - [Double Secondary Offsets:](#double-secondary-offsets) (Optional)
  - [Triple Secondary Offsets:](#triple-secondary-offsets) (Optional)

#### +Fixed Messages:

- Valid Sub-Entries:
  - [Position](#position) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Scale Gauge:](#scale-gauge) (Optional)
  - ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) [Force Scaling Above:](#force-scaling-above) (Optional)
  - ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) [Origin:](#origin) (Optional, incompatible with [Position:](#position), requires [Offset:](#offset))
  - [Offset:](#offset) (Optional, incompatible with [Position:](#position), requires [Origin:](#origin))
  - ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - ![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) [Center Text On Gauge X-Position:](#center-text-on-gauge-x-position) (Optional)

#### +Flight Path Marker:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) Adds a gauge that indicates the direction where your ship is traveling. Notably used by [Diaspora](https://wiki.hard-light.net/index.php/Diaspora).

- Valid Sub-Entries:
  - [Color:](#color) (Optional)
  - [Filename:](#filename) (Optional)
  - [Center Offsets:](#center-offsets) (Optional)

#### +Warhead Count:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) Displays the name of the current secondary weapon. Tallies the ammo left. Notably used by [Diaspora](https://wiki.hard-light.net/index.php/Diaspora).

- Valid Sub-Entries:
  - [Position:](#position) (Optional)
  - [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Filename:](#filename) (Optional) (needs an animation with a single frame)
  - [Name Offsets:](#name-offsets) (Optional)
  - `Count Offsets:` (Optional)
  - `Icon Width:` (Optional)
  - `Icon Height:` (Optional)
  - `Max Icons:` (Optional)
  - `Max Columns:` (Optional)
  - `Name Alignment:` (Optional)

#### +Hardpoints:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) Draws a silhouette of the player's ship and mounted weapons. Notably used by [Diaspora](https://wiki.hard-light.net/index.php/Diaspora).

- Valid Sub-Entries:
  - [Position:](#position) (Optional)
  - [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Size:](#size) (Optional)
  - `Line Width:` (Optional)
  - `View Direction:` (Optional)
  - `Show Primary Weapons:` (Optional)
  - `Show Secondary Weapons:` (Optional)

#### +Primary Weapons:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) Like the Weapons gauge but simpler. Only displays primary weapons. Notably used by [Diaspora](https://wiki.hard-light.net/index.php/Diaspora).

- Valid Sub-Entries:
  - [Position:](#position) (Optional)
  - [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Header Offsets:](#header-offsets) (Optional)
  - [Header Text:](#header-text) (Optional)
  - [First Background Filename:](#first-background-filename) (Optional) (needs animation with a single frame)
  - `First Background Height:` (Optional)
  - `First Background X-offset:` (Optional)
  - [Entry Background Filename:](#entry-background-filename) (Optional) (needs animation with a single frame)
  - `Entry Background Height:` (Optional)
  - `Entry Background X-offset:` (Optional)
  - [Last Background Filename:](#last-background-filename) (Optional) (needs animation with a single frame)
  - `Last Background X-offset:` (Optional)
  - `Last Background Y-offset:` (Optional)
  - [Entry Height:](#entry-height) (Optional)
  - `List Start Y-offset:` (Optional)
  - `Ammo X-offset:` (Optional)
  - `Link X-offset:` (Optional)
  - [Name X-offset:](#name-x-offset) (Optional)

#### +Secondary Weapons:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) Like the Weapons gauge but simpler. Only displays secondary weapons. Notably used by [Diaspora](https://wiki.hard-light.net/index.php/Diaspora).

- Valid Sub-Entries:
  - [Position:](#position) (Optional)
  - [Cockpit Target:](#cockpit-target) (Optional)
  - [Canvas Size:](#canvas-size) (Optional)
  - [Display Offset:](#display-offset) (Optional)
  - [Display Size:](#display-size)
  - [Color:](#color) (Optional)
  - [Font:](#font) (Optional)
  - ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) [Chase View Only:](#chase-view-only) (Optional)
  - ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `Cockpit View Choice:` (Optional. Valid values are "cockpitactive", "cockpitinactive", "both")
  - [Slew:](#slew) (Optional)
  - [Header Offsets:](#header-offsets) (Optional)
  - [Header Text:](#header-text) (Optional)
  - [First Background Filename:](#first-background-filename) (Optional)
  - `First Background Height:` (Optional)
  - `First Background X-offset:` (Optional)
  - [Entry Background Filename:](#entry-background-filename) (Optional)
  - `Entry Background Height:` (Optional)
  - `Entry Background X-offset:` (Optional)
  - [Last Background Filename:](#last-background-filename) (Optional)
  - `Last Background X-offset:` (Optional)
  - `Last Background Y-offset:` (Optional)
  - [Entry Height:](#entry-height) (Optional)
  - `List Start Y-offset:` (Optional)
  - `Ammo X-offset:` (Optional)
  - `Link X-offset:` (Optional)
  - [Name X-offset:](#name-x-offset) (Optional)
  - `Unlink X-offset:` (Optional)
  - `Reload X-offset:` (Optional)

### $End Gauges

The Gauges section must end with $End Gauges. No Exceptions.

## #End

The very last entry in the Hud_gauges.tbl must be #End. No Exceptions.

---

---

## Gauge Configuration Options

Since a vast majority of gauges share the same entries, the configuration options for each gauge have been grouped here for formatting consistency.

#### Position:

Defines the screen coordinates for the upper left corner of this gauge.

- Syntax: **Integer** horizontal pixel, **Integer** vertical pixel position

Used by: [+Custom:](#custom), [+Training Messages:](#training-messages), [+Support](#support) (and many others)

#### Scale Gauge:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) Define if a gauge size will scale or not.

If false then gauge bitmaps will always show at their original size which may result in gauges being very small at high resolutions.

If true then gauge bitmaps will scale in proportion to the current resolution vs the base HUD resolution in use. This may result in gauges becoming blurry at non-base resolutions.

Defaults to yes/true.

Incompatible with [Position:](#position)

- Syntax: **Boolean** scale_this_gauge

Used by: most gauges

#### Force Scaling Above:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) Optional. If scaling is disabled for this gauge, then it will be scaled as though the base resolution were this resolution or the real base resolution, whichever is greater.

Use this to disable scaling at lower resolutions while preventing gauges from getting too small at higher resolutions.

To disable this feature, use **(-1, -1)**.

Incompatible with [Position:](#position)

- Syntax: (**Integer** width, **Integer** height)

Used by: most gauges

#### Origin:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) Define gauge position as a ratio of the screen size (base or current, it doesn't matter).

Requires [Offset:](#offset)

Incompatible with [Position:](#position)

- Syntax: **Float** horizontal ratio 0.0 - 1.0, **Float** vertical ratio 0.0 - 1.0

Used by: most gauges

#### Offset:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) Define gauge offset from [Origin:](#origin) in base resolution pixels

Requires [Origin:](#origin)

Incompatible with [Position:](#position)

- Syntax: **Integer** horizontal pixel offset from Origin:, **Integer** vertical pixel offset from Origin:

Used by: most gauges

#### Cockpit Target:

The cockpit display that this gauge will be rendered to. Refers to the +Display Name: defined in [$Cockpit Display:](ships.tbl#cockpit-display).

- Syntax: **String** String defined in +Display Name: in [$Cockpit Display:](ships.tbl#cockpit-display)

##### Canvas Size:

The off-screen canvas this gauge will be rendered to before being interpolated into the actual cockpit display.  The smaller this number is, the larger the gauge will appear on the cockpit display.

- Syntax: **Integer** width, **Integer** height

##### Display Offset:

The position of the canvas on the cockpit display texture

- Syntax: **Integer** width, **Integer** height

##### Display Size:

The interpolated size on the cockpit display texture that the canvas will be rendered to.

- Syntax: **Integer** width, **Integer** height

#### Color:

Optional. Overrides both previous color settings ([$Color](#color) HUD option and [$Color](https://wiki.hard-light.net/index.php/Hud_gauges#$Color:_2) ship class option) for this particular gauge.

- Syntax: See [$Color:](#color)

Used by: [+Custom:](#custom), [+Support](#support)

#### Font:

Optional. Overrides both previous font settings ([$Font](#font) HUD option and [$Font](https://wiki.hard-light.net/index.php/Hud_gauges#$Font:_2) ship class option) for this particular gauge.

- Syntax: See [$Font:](#font)

Used by: [+Custom:](#custom), [+Training Messages:](#training-messages), [+Support](#support)

#### Name:

Defines the name of this custom gauge.

- Syntax: **String**
- Max Length: 32 characters

Used by: [+Custom:](#custom)

#### Text:

Default text to be displayed. Can be altered using the hud-set-message, hud-set-text and hud-set-directive sexps.

- Syntax: **String**
- Max Length: 32 characters

Used by: [+Custom:](#custom)

#### X Offset:

Optional. Defines the horizontal pixel offset (from the gauge's origin as defined by [Position:](#position)) that the Text will be displayed.

- Syntax: **Integer**
- Units: Pixels

Used by: [+Custom:](#custom)

#### Y Offset:

Optional. Defines the vertical pixel offset (from the gauge's origin defined by [Position:](#position)) that the Text will be displayed.

- Syntax: **Integer**
- Units: Pixels

Used by: [+Custom:](#custom)

#### Gauge Type:

Defines the type of gauge this is.

- Syntax: **String**
- Valid Values: Any one of the possible [gauge types](https://wiki.hard-light.net/index.php/gauge_types).

Used by: [+Custom:](#custom)

#### Slew:

Optional. Determines whether or not this gauge is affected by afterburner shaking or similar effects. If set to "true", the gauge will <strong>not</strong> follow the player's head movements when using TrackIR/padlock. i.e. should be used for gauges like the gunsight/leadsight.

- Syntax: **Boolean**

Used by: [+Custom:](#custom), [+Support](#support)

#### Max Lines:

Optional. Specifies the maximum number of message lines that are displayed.

- Syntax: **Integer**
- Default Value: 3

Used by: [+Messages:](#messages)

#### Max Width:

Optional. Determines how long each message line may be.

- Syntax: **Integer**
- Units: Pixels

Used by: [+Messages:](#messages)

#### Line Height:

Optional. Determines the distance between line origins. (i.g. for a 12pt Font, there are needs to be at least 12 pixels between the bottom of one line and the next, otherwise the text will overlap each)

- Syntax: **Integer**
- Units: Pixels
- Default Value: 9

Used by: [+Messages:](#messages)

#### Total Lifetime:

Optional. Determines the maximum time the message gauge will be displayed from the time a new message is received.

- Syntax: **Integer**
- Units: milliseconds
- Default Value: 14000 (14 seconds)

Used by: [+Messages:](#messages)

#### Scroll Time:

Optional. Determines the time for the messages to scroll up one step.( See [Step Size:](#step-size) )

- Syntax: **Integer**
- Units: milliseconds
- Default Value: 30

Used by: [+Messages:](#messages)

#### Step Size:

Optional. Determines how many pixels to shift message lines during scrolling. Messages will be shifted up by this amount until they are on the next line (as defined by [Line Height:](#line-height))

- Syntax: **Integer**
- Units: Pixels
- Default Value: 3

Used by: [+Messages:](#messages)

#### Active by default:

Optional. Determines whether or not this gauge is always active, or only available after being activated via sexp.

- Syntax: **Boolean**

Used by: [+Custom:](#custom)

#### Filename:

Designates the [.ANI](https://wiki.hard-light.net/index.php/Multimedia_Files#ANI) or [.EFF](https://wiki.hard-light.net/index.php/Multimedia_Files#EFF) used for this gauge's background graphic.

> **Note:** All HUD .ANI's must have the same color palette as the retail .ANI's in order to render properly. (.EFF's do not have this restriction)

- Syntax: **Filename**
  - No filename extension is needed.

Used by: [+Custom:](#custom), [+Support](#support)

#### Top Background Filename:

Optional. Designates which file to use for the top background. (?)

- Syntax: **Filename**

Used by: [+Damage](#damage)

#### Bottom Background Filename:

Optional. Designates which file to use for the Bottom background (?)

- Syntax: **Filename**

Used by: [+Damage](#damage)

#### Left Background Filename:

Optional. Designates which file to use for the Left side of gauge.

- Syntax: **Filename**

Used by: [+Wingman Status:](#wingman-status)

#### First Background Filename:

Optional. Alias for [Left Background Filename:](#left-background-filename)

#### Right Background Filename:

Optional. Designates which file to use for the Right side of the gauge.

- Syntax: **Filename**

Used by: [+Wingman Status:](#wingman-status)

#### Last Background Filename:

Optional. Alias for [Right Background Filename:](#right-background-filename)

#### Entry Background Filename:

Optional. Designates which file to use for the (Wing) Entry section of the gauge (aka the middle sections).

- Syntax: **Filename**

Used by: [+Damage](#damage), [+Wingman Status:](#wingman-status)

#### Primary List Top Background Filename:

Optional. Designates which file to use for the Primary List Top Background

- Syntax: **Filename**
- Valid Sub-entries:
  - [Alt Ballistic Filename](#alt-ballistic-filename)

Used by: [+Weapons:](#weapons)

#### Primary List Middle Background Filename:

Optional. Designates which file to use for the Primary List Middle Background

- Syntax: **Filename**
- Valid Sub-entries:
  - [Alt Ballistic Filename](#alt-ballistic-filename)

Used by: [+Weapons:](#weapons)

#### Primary List Bottom Background Filename:

Optional. Designates which file to use for the Primary List Bottom Background

- Syntax: **Filename**
- Valid Sub-entries:
  - [Alt Ballistic Filename](#alt-ballistic-filename)

Used by: [+Weapons:](#weapons)

#### Secondary List Top Background Filename:

Optional. Designates which file to use for the Secondary List Top Background

- Syntax: **Filename**
- Valid Sub-entries:
  - [Alt Ballistic Filename](#alt-ballistic-filename)

Used by: [+Weapons:](#weapons)

#### Secondary List Entry Background Filename:

Optional. Designates which file to use for the Secondary List Entry Background

- Syntax: **Filename**
- Valid Sub-entries:
  - [Alt Ballistic Filename](#alt-ballistic-filename)

Used by: [+Weapons:](#weapons)

#### Secondary List Bottom Background Filename:

Optional. Designates which file to use for the Secondary List Bottom Background

- Syntax: **Filename**
- Valid Sub-entries:
  - [Alt Ballistic Filename](#alt-ballistic-filename)

Used by: [+Weapons:](#weapons)

#### Alt Ballistic Filename:

Optional. Designates an alternate file to be used for ballistic weapons.

- Syntax: **Filename**

Used by: [Primary List Top Background Filename](#primary-list-top-background-filename), [Primary List Middle Background Filename](#primary-list-middle-background-filename), [Primary List Bottom Background Filename](#primary-list-bottom-background-filename), [Secondary List Top Background Filename](#secondary-list-top-background-filename), [Secondary List Entry Background Filename](#secondary-list-entry-background-filename), [Secondary List Bottom Background Filename](#secondary-list-bottom-background-filename)

#### Dot Filename:

Optional. Designates which file to use for the wingman graphic (a.k.a. the Dot).  Needs an animation with two frames, 1st is for alive wingmen, 2nd is for dead or missing wingmen

- Syntax: **Filename**

Used by: [+Wingman Status:](#wingman-status)

#### Header Offsets:

Optional. Designates the offset from the gauge's upper left corner for headers of various gauges

- Syntax: **Integer** horizontal offset, **Integer** vertical offset
- Units: Pixels
- Valid Sub-entries:
Weapons:
  - [Alt Ballistic Offsets:](#alt-ballistic-offsets)

Used by: [+Support:](#support), [+Damage:](#damage),  [+Weapons:](#weapons)

#### Fixed Header Position:

Optional. If set, the header text will not change position when the gauge changes size.

- Syntax: **Boolean**

Used by: [+Wingman Status:](#wingman-status)

#### Text Y-offset:

Optional. Designates the offset from the gauge's upper left corner for the Support Ship Text. (?)

- Syntax: **Integer** vertical offset
- Units: Pixels

Used by: [+Support:](#support)

#### Text Offsets:

Optional. Defines the screen position offsets for the text label.

- Syntax: **Integer** horizontal position, **Integer** vertical position
- Units: Pixels

Used by: :[+Countermeasures:](#countermeasures)

#### Value Offsets:

Optional. Defines the screen position offsets for the value label.

- Syntax: **Integer** horizontal position, **Integer** vertical position
- Units: Pixels

Used by: :[+Countermeasures:](#countermeasures)

#### Dock Status X-offset:

Optional. Designates the offset from the gauge's upper left corner for the Support Ship's dock status. (?)

- Syntax: **Integer** vertical offset
- Units: Pixels

Used by: [+Support:](#support)

#### Dock Time X-offset:

Optional. Designates the offset from the gauge's upper left corner for the Support Ship's dock time. (?)

- Syntax: **Integer** vertical offset
- Units: Pixels

Used by: [+Support:](#support)

#### Hull Integrity Offsets:

Optional. Designates the offsets from the gauge's upper left corner for the for the Damage gauge Hull integrity textbox(?)

- Syntax: **Integer** horizontal offset, **Integer** vertical offset
- Units: Pixels

Used by: [+Damage:](#damage)

#### Hull Integrity Value X-offset:

Optional. Designates the offsets from the hull integrity textbox's upper left corner for the hull integrity value. (?)

- Syntax: **Integer** horizontal offset
- Units: Pixels

Used by: [+Damage:](#damage)

#### Top Background Height:

Optional. Designates the height of the Top background (?)

- Syntax: **Integer**
- Units: Pixels

Used by: [+Damage:](#damage)

#### Left Background Width:

Optional. Defines the width of the Left Background.

- Syntax: **Integer**
- Units: Pixels

Used by: [+Wingman Status:](#wingman-status)

#### Entry Width:

Optional. Defines the width of the Entry Background (?)

- Syntax: **Integer**
- Units: Pixels

Used by: [+Wingman Status:](#wingman-status)

#### Subsystem Entry Height:

Optional. Designates the height of the Subsystem entry (?)

- Syntax: **Integer**
- Units: Pixels

Used by: [+Damage:](#damage)

#### Subsystem List Start Offsets:

Optional. Designates the offsets from the gauge's upper left corner for the for the start of the Subsytem List (?)

- Syntax: **Integer** horizontal offset, **Integer** vertical offset
- Units: Pixels

Used by: [+Damage:](#damage)

#### Subsystem Entry Value X-offset:

Optional. Designates the horizontal offset from the Subsystem List's position (?)

- Syntax: **Integer**
- Units: Pixels

Used by: [+Damage:](#damage)

#### Single Wing Offsets:

Optional. Designates the offset from the gauge's upper left corner for a single wing (?)

- Syntax: **Integer** horizontal offset, **Integer** vertical offset
- Units: Pixels

Used by: [+Wingman Status:](#wingman-status)

#### Multiple Wing Start Offsets:

Optional. Designates the offset from the gauge's upper left corner for multiple wings (?)

- Syntax: **Integer** horizontal offset, **Integer** vertical offset
- Units: Pixels

Used by: [+Wingman Status:](#wingman-status)

#### Wing Name Offsets:

Optional. Designates the offset from the gauge's upper left corner for the Support Ship header (?)

- Syntax: **Integer** horizontal offset, **Integer** vertical offset
- Units: Pixels

Used by: [+Wingman Status:](#wingman-status)

#### Dot Offsets:

Optional. Designates the offset from the gauge's upper left corner for the All wingman status icons.
- Syntax:
**Integer** x, **Integer** y ;; Wingman 1
**Integer** x, **Integer** y ;; Wingman 2
**Integer** x, **Integer** y ;; Wingman 3
**Integer** x, **Integer** y ;; Wingman 4
**Integer** x, **Integer** y ;; Wingman 5
**Integer** x, **Integer** y ;; Wingman 6
- Units: Pixels

> **Warning:** All 6 wingman positions must be defined

Used by: [+Wingman Status:](#wingman-status)

#### Expansion Mode:

Optional. Denotes which direction (in relation to the player's wing) that the other wings should be appended to the gauge. If not present the gauge will expand to the left.

- Syntax: **String**
- Valid Values:
  - Right
  - Down

Used by: [+Wingman Status:](#wingman-status)

#### Use Full Wing Names:

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg) Optional. If enabled the wingmen gauge draws the full wing name and does not alter the capitalization. Default behavior is to draw the first three letters of the wing in all lower-case.

- Syntax: **Boolean**
- Default Value: **False**

Used by: [+Wingman Status:](#wingman-status)



#### Wingname Align Mode:

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) Optional. How wing names are aligned: **Center**, **Left** or **Right**. Anything else is an error and is ignored.

- Syntax: **String**

Used by: [+Wingman Status:](#wingman-status)

#### Key Line X Offset:

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) Optional. How far key bindings shown in directives are moved right.

- Syntax: **Integer**, pixels

Used by: [+Directives:](#directives)

#### Use Expanded Colors:

![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg) Optional. If enabled the wingmen gauge status dots use red, yellow, or green color to designate wingmen health (green full to greater thon two-thirds, yellow two-thirds to greater than one-third, and red less than or equal to one-third). Default is to use red for less than half health and gauge color otherwise.

- Syntax: **Boolean**
- Default Value: **False**

Used by: [+Wingman Status:](#wingman-status)

#### Auto Offsets:

Optional. Defines the screen coordinates for the "Auto" text label.

- Syntax: **Integer** horizontal position, **Integer** vertical position
- Units: Pixels

Used by: [+Auto Speed:](#auto-speed), [+Auto Target:](#auto-target)

#### Speed Offsets:

Optional. Defines the screen coordinates for the "Speed" text label.

- Syntax: **Integer** horizontal position, **Integer** vertical position
- Units: Pixels

Used by: [+Auto Speed:](#auto-speed)

#### Target Offsets:

Optional. Defines the screen coordinates for the "Target" text label.

- Syntax: **Integer** horizontal position, **Integer** vertical position
- Units: Pixels

Used by: [+Auto Target:](#auto-target)

#### Animation Offsets:

Optional. Defines the offsets for the animation graphics.

- Syntax: **Integer** horizontal offset, **Integer** vertical offset
- Units: Pixels

Used by: [+Talking Head:](#talking-head)

#### Animation Background Size:

Deprecated. Originally, this set the size of the black box that's drawn behind the animation. Since Git revision 556e9a4 (2015-11-01), it has the same effect as [Animation Size:](#animation-size).

- Syntax: **Integer** horizontal size, **Integer** vertical size
- Units: Pixels

Used by: [+Talking Head:](#talking-head)

#### Animation Size:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) Optional. Defines the size of the animation. All animations will be scaled to match this size.

- Syntax: **Integer** horizontal size, **Integer** vertical size
- Units: Pixels

Used by: [+Talking Head:](#talking-head)

#### List Start Offsets:

Optional. Defines the screen coordinate offsets for the start of the Directives list.

- Syntax: **Integer** x, **Integer** y
- Units: Pixels

Used by: [+Directives:](hud_guages.tbl#directives)

#### Entry Height:

Optional. Defines the height of each Directive

- Syntax: **Integer**
- Units: Pixels

Used by: [+Directives:](hud_guages.tbl#directives)

#### Alt Ballistic Offsets:

Opetional. Defines the screen coordinate offsets for headers of ballistic weapons

- Syntax: **Integer** horizontal offset, **Integer** vertical offset
- Units: Pixels

Used by: `+Header Offset:`

#### Top Primary Background X-offset:

Optional. Defines the screen coordinate offset for the Top Primary Background

- Syntax: **Integer**
- Units: Pixels
- Valid Sub-entries:
  - [Alt Ballistic X-offset:](#alt-ballistic-x-offset)

Used by: [+Weapons:](#weapons)

#### Text X-offset:

(Optional) Defines the screen coordinate X-offset for weapon labels

- Syntax: **Integer**
- Units: Pixels
- Valid Sub-entries:
  - [Alt Ballistic X-offset:](#alt-ballistic-x-offset)

Used by: [+Weapons:](#weapons)

#### Alt Ballistic X-offset:

Optional. Defines the screen coordinate X-offset for ballistic weapons

- Syntax: **Integer**
- Units: Pixels

Used by: [Top Primary Background X-offset:](#top-primary-background-x-offset), [Text X-offset:](#text-x-offset)

#### Top Primary Frame Height:

Used by: [+Weapons:](#weapons)

#### Top Secondary Frame Height:

Used by: [+Weapons:](#weapons)

#### Primary List Start Y-offset:

Used by: [+Weapons:](#weapons)

#### Secondary List Start Y-offset:

Used by: [+Weapons:](#weapons)

#### Primary Weapon Ammo X-offset:

Used by: [+Weapons:](#weapons)

#### Primary Weapon Link X-offset:

Used by: [+Weapons:](#weapons)

#### Primary Weapon Name X-offset:

Used by: [+Weapons:](#weapons)

#### Secondary Weapon Ammo X-offset:

Used by: [+Weapons:](#weapons)

#### Secondary Weapon Unlinked X-offset:

Used by: [+Weapons:](#weapons)

#### Secondary Weapon Linked X-offset:

Used by: [+Weapons:](#weapons)

#### Secondary Weapon Name X-offset:

Used by: [+Weapons:](#weapons)

#### Secondary Weapon Reload X-offset:

Used by: [+Weapons:](#weapons)

#### Primary Weapon Entry Height:

Used by: [+Weapons:](#weapons)

#### Secondary Weapon Entry Height:

Used by: [+Weapons:](#weapons)

#### 3 Digit Hull Offsets:

Designates the screen position offsets for the hull damage % when it is the 3 digit range (i.e. >= 100%)

- Syntax: **Integer** x offset, **Integer** y offset
- Units: Pixels

Used by: [+Mini Target Shields:](#mini-target-shields)

#### 2 Digit Hull Offsets:

Designates the screen position offsets for the hull damage % when it is the 2 digit range (i.e. 100% > hull damage%  >= 10%)

- Syntax: **Integer** x offset, **Integer** y offset
- Units: Pixels

Used by: [+Mini Target Shields:](#mini-target-shields)

#### 1 Digit Hull Offsets:

Designates the screen position offsets for the hull damage % when it is the 1 digit range (i.e. 10% > hull damage%  >= 0%)

- Syntax: **Integer** x offset, **Integer** y offset
- Units: Pixels

Used by: [+Mini Target Shields:](#mini-target-shields)

#### Objective Text Y-offset:

Designates the screen position offset for the Objective label

- Syntax: **Integer** y offset
- Units: Pixels

Used by: [+Objective notify:](#objective-notify)

#### Objective Value Y-offset

Designates the screen position offset for the Objective value

- Syntax: **Integer** y offset
- Units: Pixels

Used by: [+Objective notify:](#objective-notify)

#### Subspace Text Y-offset:

Designates the screen position offset for the Subspace label

- Syntax: **Integer** y offset
- Units: Pixels

Used by: [+Objective notify:](#objective-notify)

#### Subspace Value Y-offset

Designates the screen position offset for the Subspace value (?)

- Syntax: **Integer** y offset
- Units: Pixels

Used by: [+Objective notify:](#objective-notify)

#### Red Alert Text Y-offset

Designates the screen position offset for the Red Alert label

- Syntax: **Integer** y offset
- Units: Pixels

Used by: [+Objective notify:](#objective-notify)

#### Red Alert Value Y-offset

Designates the screen position offset for the Red Alert value (?)

- Syntax: **Integer** y offset
- Units: Pixels

Used by: [+Objective notify:](#objective-notify)

#### Command X-offset:

- Syntax: **Integer** x offset
- Units: Pixels

Used by: [+Squad Message:](#squad-message)

#### Page Up Offsets:

- Syntax: **Integer** x offset, **Integer** y offset
- Units: Pixels

Used by: [+Squad Message:](#squad-message)

#### Page Down Offsets:

- Syntax: **Integer** x offset, ''' Integer y offset
- Units: Pixels

Used by: [+Squad Message:](#squad-message)

#### Entry Height:

Used by: [+Escort View:](#escort-view)

#### Entry Stagger Width:

Used by: [+Escort View:](#escort-view)

#### Header Text:

Used by: [+Escort View:](#escort-view)

#### Header Offsets:

Used by: [+Escort View:](#escort-view)

#### List Start Offsets:

Used by: [+Escort View:](#escort-view)

#### Hull X-offset:

Used by: [+Escort View:](#escort-view)

#### Name X-offset:

Used by: [+Escort View:](#escort-view)

#### Status X-offset:

Used by: [+Escort View:](#escort-view)

#### Ship Name Max Width:

Used by: [+Escort View:](#escort-view)

#### Right-Align Ship Names:

Used by: [+Escort View:](#escort-view)

#### Foreground Clip Height:

- Sets the height, in pixels, of the gauge.
  - For the [Throttle](#throttle), sets the height of the gauge that represents 0% to 100% throttle.  The total height of the gauge is the sum of this value and the [Afterburner Clip Height](#afterburner-clip-height) value.
- Syntax: **Integer**

Used by: [+ETS Retail:](#ets-retail), [+ETS Weapons](#ets-weapons), [+ETS Engines](#ets-engines), [+ETS Shields](#ets-shields), [+Target Monitor](#target-monitor), [+Afterburner Energy](#afterburner-energy),  [+Weapon Energy](#weapon-energy), [+Throttle](#throttle)

#### Letter Offsets:

Used by: [+ETS Retail:](#ets-retail)

#### Top Offsets:

Used by: [+ETS Retail:](#ets-retail)

#### Bottom Offsets:

Used by: [+ETS Retail:](#ets-retail)

#### Gauge Offset:

- Distance between the individual weapon/shield/engine ETS gauges that comprise this gauge
- Syntax: **Integer**

Used by: [+ETS Retail:](#ets-retail)

#### Monitor Filename:

Used by: [+Target Monitor:](#target-monitor)

#### Integrity Bar Filename:

Used by: [+Target Monitor:](#target-monitor)

#### Viewport Offsets:

Used by: [+Target Monitor:](#target-monitor)

#### Viewport Size:

Used by: [+Target Monitor:](#target-monitor)

#### Integrity Bar Offsets:

Used by: [+Target Monitor:](#target-monitor)

#### Integrity Bar Foreground Clip Height:

Used by: [+Target Monitor:](#target-monitor)

#### Status Offsets:

Used by: [+Target Monitor:](#target-monitor)

#### Name Offsets:

Used by: [+Target Monitor:](#target-monitor)

#### Class Offsets:

Used by: [+Target Monitor:](#target-monitor)

#### Distance Offsets:

Used by: [+Target Monitor:](#target-monitor)

#### Speed Offsets:

Used by: [+Target Monitor:](#target-monitor)

#### Hull Offsets:

Used by: [+Target Monitor:](#target-monitor)

#### Cargo Contents Offsets:

Used by: [+Target Monitor:](#target-monitor)


#### Cargo Scan Type:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) Optional. How the scan lines are drawn while scanning cargo: **default**, **dualscanlines** or **discoscanlines**. An unknown type logs a warning and uses default.

- Syntax: **String**

Used by: [+Target Monitor:](#target-monitor)

#### Cargo Scan Start Offsets:

Used by: [+Target Monitor:](#target-monitor)

#### Cargo Scan Size:

Used by: [+Target Monitor:](#target-monitor)

#### Bracket Offsets:

Used by: [+Extra Target Data:](#extra-target-data)

#### Dock Offsets:

Used by: [+Extra Target Data:](#extra-target-data)

#### Dock Max Width:

Used by: [+Extra Target Data:](#extra-target-data)

#### Order Offsets:

Used by: [+Extra Target Data:](#extra-target-data)

#### Order Max Width:

Used by: [+Extra Target Data:](#extra-target-data)

#### Time Offsets:

Used by: [+Extra Target Data:](#extra-target-data)

#### Radar Center Offsets:

Defines the radar's plot origin as an offset, in pixels, from this gauge's [Position](#position) coordinates
- Syntax: **Integer** horizontal offset, **Integer** vertical offset

Used by: [+Radar:](#radar)

#### Radar Size:

Size, in pixels, of the radar plot area.

Syntax: **Integer** horizontal size, **Integer** vertical size

Used by: [+Radar:](#radar)

#### Infinity Distance Offsets:

Used by: [+Radar:](#radar)

Offset to draw the infinity radar range symbol.

#### Long Distance Offsets:

Used by: [+Radar:](#radar)

Offset to draw the long range radar text (i.e. 10k).

#### Short Distance Offsets:

Used by: [+Radar:](#radar)

Offset to draw the short range radar text (i.e. 2k).

#### Size:

Used by: [+Radar BSG:](#radar-bsg)

#### XY Disc Filename:

Used by: [+Radar BSG:](#radar-bsg)

#### XZ YZ Disc Filename:

Used by: [+Radar BSG:](#radar-bsg)

#### Sweep Disc Filename:

Used by: [+Radar BSG:](#radar-bsg)

#### Default Contact Filename:

Used by: [+Radar BSG:](#radar-bsg)

#### Unknown Contact Filename:

Used by: [+Radar BSG:](#radar-bsg)

#### Loop Volume:

Used by: [+Radar BSG](#radar-bsg)

#### Minimum Beep Delay:

Used by: [+Radar BSG](#radar-bsg)

#### Firepoint display:

Displays the positions of the gun firepoints on your ship
- Syntax: **Boolean**, YES or NO

NOTE: The position of the firepoints is calculated from the Eye Point position in the POF. You may encounter weird results if you have no Eye Point defined, but no debug warnings will be raised.

Used by: [+Center Reticle](#center-reticle)

#### Firepoint size:

Diameter of the gunpoint circles.  **Must be 4 or greater** in order to work properly.
- Syntax: **Integer**

Used by: [+Center Reticle](#center-reticle)

#### Firepoint X coordinate multiplier:

Used by: [+Center Reticle](#center-reticle)

#### Firepoint Y coordinate multiplier:

Used by: [+Center Reticle](#center-reticle)


#### Autoaim Frame:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) Optional. The frame of the reticle animation shown while autoaim is locked on.

- Syntax: **Integer**

Used by: [+Center Reticle:](#center-reticle)


#### Flight Cursor Frame:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) Optional. The frame of the reticle animation used for the flight cursor.

- Syntax: **Integer**

Used by: [+Center Reticle:](#center-reticle)

#### Foreground Clip Bottom Y-offset:

Used by: [+Throttle:](#throttle)

#### Foreground Clip Width:

Used by: [+Throttle:](#throttle)

#### Afterburner Clip Height:

- Sets the area of the throttle gauge that represents the Afterburner segment (which is on top of the 0% to 100% area).  The total height of the gauge is determined by the sum of this value and [Foreground Clip Height:](#foreground-clip-height)

Used by: [+Throttle:](#throttle)

#### Show Background:

Used by: [+Throttle:](#throttle)

#### Max Speed Label Offsets:

Used by: [+Throttle:](#throttle)

#### Min Speed Label Offsets:

Used by: [+Throttle:](#throttle)

#### Orbit Center Offsets:

Used by: [+Throttle:](#throttle)

#### Orbit Radius:

Used by: [+Throttle:](#throttle)

#### Arc Filename:

- Filename for the background graphic for this gauge
- Syntax: **Filename**

Needs to be an [ani](https://wiki.hard-light.net/index.php/Multimedia_Files#ANI) with a least 2 frames (but only the 2nd frame is used ?!?!? for threat & weapon linking so far)

Used by: [+Threat Indicator:](#threat-indicator)

#### Dumbfire Filename:

- Filename for the background graphic for this gauge
- Syntax: **Filename**

Needs to be an [ani](https://wiki.hard-light.net/index.php/Multimedia_Files#ANI) with a least 2 frames. The 1st frame is "no warning", the rest of the frames are cycled through when a warning is active.

Internally FSO also refers to this as the "laser threat warning".

Used by: [+Threat Indicator:](#threat-indicator)

#### Lock Filename:

- Filename for the background graphic for this gauge
- Syntax: **Filename**

Needs to be an [ani](https://wiki.hard-light.net/index.php/Multimedia_Files#ANI) with a least 2 frames. The 1st frame is "no warning", the rest of the frames are cycled through when a warning is active. Frames cycle twice as fast when a lock has been acquired, compared to when a lock is being acquired.

Used by: [+Threat Indicator:](#threat-indicator)

#### Dumbfire Offsets:

- Filename for the background graphic for this gauge
- Syntax: **Filename**

Used by: [+Threat Indicator:](#threat-indicator)

#### Lock Offsets:

- Filename for the background graphic for this gauge
- Syntax: **Filename**

Used by: [+Threat Indicator:](#threat-indicator)

#### Lock Filename:

Used by: [+Lock Indicator:](#lock-indicator)

#### Locked Filename:

Used by: [+Lock Indicator:](#lock-indicator)

#### Lock Center Offsets:

Used by: [+Lock Indicator:](#lock-indicator)

#### Locked Center Offsets:

Used by: [+Lock Indicator:](#lock-indicator)

#### Loop Locked Animation:

Controls whether the animation should loop after locking on, like in FS1.

Used by: [+Lock Indicator:](#lock-indicator)

#### Blink Locked Animation:

Controls whether the animation should blink after locking on, like in FS2.

Used by: [+Lock Indicator:](#lock-indicator)

#### Radius:

Defines the distance from the center of the screen to the base of the Hostile Triangle gauge.

- Syntax: **Integer**
- Default value:
- Units: Pixels

Used by: [+Hostile Triangle:](#hostile-triangle)

#### Triangle Base:

Defines the width of the Hostile Triangle's base.

- Syntax: **Float**
- Default value:
- Units: Pixels

Used by: [+Hostile Triangle:](#hostile-triangle)

#### Triangle Height:

Defines the height of the Hostile Triangle.

- Syntax: **Float**
Default value:
- Units: Pixels

Used by: [+Hostile Triangle:](#hostile-triangle)

#### Center Offsets:

Used by: [+Lead Indicator:](#lead-indicator)

#### Single Primary Filename:

Designates the file to use for primary linking for ships with a single primary bank.

Needs to be an [ani](https://wiki.hard-light.net/index.php/Multimedia_Files#ANI) with 2 frames; 1st is bank 1 not selected (not that this should ever be able to happen), 2nd is bank 1 selected.

Used by: [+Weapon Linking:](#weapon-linking)

#### Double Primary Filename:

Designates the file to use for primary linking for ships with two primary banks.

Needs to be an [ani](https://wiki.hard-light.net/index.php/Multimedia_Files#ANI) with 4 frames:
- Frame 0 is no banks selected
- Frame 1 is 1st bank selected
- Frame 2 is 2nd bank selected
- Frame 3 is both banks selected/linked

Used by: [+Weapon Linking:](#weapon-linking)

#### Single Secondary Filename:

Designates the file to use for selected secondary bank for ships with a single secondary bank.

Needs to be an [ani](https://wiki.hard-light.net/index.php/Multimedia_Files#ANI) with 2 frames; 1st is bank 1 not selected, 2nd is bank 1 selected.

Used by: [+Weapon Linking:](#weapon-linking)

#### Double Secondary Filename:

Designates the file to use for selected secondary bank for ships with two secondary banks.

Needs to be an [ani](https://wiki.hard-light.net/index.php/Multimedia_Files#ANI) with 3 frames; 1st is no banks selected, 2nd is bank 1 selected, 3rd is bank 2 selected.

Used by: [+Weapon Linking:](#weapon-linking)

#### Triple Secondary Filename:

Designates the file to use for selected secondary bank for ships with two secondary banks.

Needs to be an [ani](https://wiki.hard-light.net/index.php/Multimedia_Files#ANI) with 4 frames; 1st is no banks selected, 2nd is bank 1 selected, 3rd is bank 2 selected, 4th is bank 3 selected.

Used by: [+Weapon Linking:](#weapon-linking)

#### Single Primary Offsets:

Used by: [+Weapon Linking:](#weapon-linking)

#### Double Primary Offsets:

Used by: [+Weapon Linking:](#weapon-linking)

#### Single Secondary Offsets:

Used by: [+Weapon Linking:](#weapon-linking)

#### Double Secondary Offsets:

Used by: [+Weapon Linking:](#weapon-linking)

#### Triple Secondary Offsets:

Used by: [+Weapon Linking:](#weapon-linking)

#### Hidden By Comms Menu:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) Defines whether the messages gauge may be drawn at the same time as the wingman comms menu.
- Syntax: **Boolean**
- Default Value: **false** (messages are not visible while the comms menu is open)
Used by: [+Messages:](#messages)

#### Bottom Background Offset:

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) Defines the vertical pixel offset added to the position of the bottom background frame.
- Syntax: **Integer** vertical offset
- Units: Pixels
Used by: [+Damage:](#damage)


#### Always Display:

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) Optional. Shows the damage gauge even when nothing is damaged.

- Syntax: **Boolean**

Used by: [+Damage:](#damage)

#### Center Text On Gauge X-Position:

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) Sets whether to position the center of the text to the gauge's X-position.
- Syntax **Boolean**
- Default Value: **false** if the gauge is explicitly given a position, **true** otherwise
Used by: [+Fixed Messages:](#fixed-messages)

#### Ngon Sides:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) Defines the number of sides a regular polygon (N-gon) radar has.  If defined, the standard radar's edge is drawn as an N-gon, instead of the standard ellipsoid.  Blips that are projected outside of the N-gon are snapped to the edge: the blip's angle stays the same but the blip's radius to the center of the plot area is reduced.
- Syntax **Integer**
- Default Value: **0** A value of 0 means to use the standard ellipsoid radar
- Minimum value: **3** (which is a triangle)
Used by: [+Radar:](#radar)

#### Ngon Offset:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) Defines the angular offset to apply to the N-gon perimeter.  The first vertex is at the 3 o'clock position, positive offsets spin this vertex counter-clockwise, negative offsets spin this clockwise.
- Syntax **Float** (radians)
- Default Value: **0**
Used by: [+Radar:](#radar)
