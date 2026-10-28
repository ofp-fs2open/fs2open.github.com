> **Revision information**
> OFP Revision: [#136](https://github.com/ofp-fs2open/fs2open.github.com/pull/136)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg)

The **post_processing.tbl** defines the post-processing effects available in FSO: the screen effects missions can turn on, the per-ship shader effects, and the light shaft settings. The engine has a built-in version (shown below), so the table is only needed to change them.

This table is not modular: a mod's post_processing.tbl replaces the built-in one entirely.

Code: [/graphics/post_processing.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/graphics/post_processing.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in.

## Contents

1. [Format](#format)
2. [#Effects](#effects)
3. [#Ship Effects](#ship-effects)
4. [#Light Shafts](#light-shafts)
5. [Sample](#sample)

---

## Format

- The table ends with `#End`. The three sections, `#Effects`, `#Ship Effects` and `#Light Shafts`, are each optional but must be in this order.
- Every field below is **required** and must be in the order listed, except `$RGB:`.
- Post-processing is turned on and off by the player in the options (Post processing); this table only defines the effects.

## #Effects

The effects that can be turned on and adjusted in a mission with the `set-post-effect` SEXP. At most 32; extra effects are noted in the log and ignored.

### $Name:

- Name of the effect. This is used to refer to the effect with the post-processing sexps.
- Syntax: ***String***

### $Uniform:

- Which value of the post-processing shader the effect controls. It must be one of the names the engine knows; anything else logs a warning:
  - *noise_amount*, *saturation*, *brightness*, *contrast*, *film_grain*, *tv_stripes*, *cutoff*, *dither*
  - *tint* (uses `$RGB:` as the color)
  - ![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) *custom_effect_float_a*, *custom_effect_float_b*, *custom_effect_vec3_a*, *custom_effect_vec3_b*: for a mod's own post-processing shader (the vec3 ones use `$RGB:`)
- Syntax: ***String***

### $Define:

- In the post-processing shader, each effects' code is encapsulated in a preprocessor define. Since the engine only uses one post-processing shader, the shader has to be recompiled every time an effect is activated or deactivated. To control which effects are in use, preprocessor defines are used. The name given here must match the name used in the shader.
- Syntax: ***String***

### $AlwaysOn:

- If set to true, the effect will be on by default.
- Syntax: ***Boolean***

### $Default:

- The value the effect starts with, used as is.
- Values set later with the `set-post-effect` SEXP are scaled by `$Div:` and `$Add:`: value = (SEXP value / div) + add. (The FreeSpace Wiki said this scaling applies to `$Default:`; it doesn't.)
- Syntax: ***Float***

### $Div:

- Values from the `set-post-effect` SEXP are divided by this (see `$Default:`).
- Syntax: ***Float***

### $Add:

- Added to values from the `set-post-effect` SEXP after dividing (see `$Default:`).
- Syntax: ***Float***

### $RGB:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

- Optional. A color for the effects that use one (*tint* and the vec3 custom effects).
- Syntax: ***Float***, ***Float***, ***Float***, red, green and blue

## #Ship Effects

This section is used to define effects for use with the ship-effect sexp or ship::addShipEffect() lua function.

The engine always adds one built-in ship effect first, *FS1 Ship select* (shader effect 0).

### $Name:

- Syntax: ***String***
- Defines a unique name for this effect, which is used as input for the sexp and lua function.

### $Shader Effect:

- Syntax: ***Integer***
- Defines a unique ID for this effect. This will be passed to the main shader as a uniform parameter. Note that IDs 0 and 1 are reserved for in-engine use.

### $Disables Rendering:

- Syntax: ***Boolean***
- If this is set to true, the ship will not be rendered after the effect is completed.
- Note: In order to reenable rendering, use an effect with this option set to false.

### $Invert timer:

- Syntax: ***Boolean***
- Controls the behavior of the timer passed to the shader as a uniform variable. Timer values are always in the [0.0, 1.0] range, and are computed as follows:
(Current time in milliseconds - effect start time in milliseconds) / (effect duration in milliseconds)
If this parameter is set to true, the computation is altered as follows:
1.0 - ((Current time in milliseconds - effect start time in milliseconds) / (effect duration in milliseconds))

## #Light Shafts

![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) This section is used to define the parameters for the light shaft effect. Without it, the defaults below are used.

### $AlwaysOn:

- If set to true, the effect will be on by default. The player can still turn light shafts off in the options.
- Default: YES
- Syntax: ***Boolean***

### $Density:

- This changes the spacing between the individual samples taken by the shader. A smaller number means shorter lightshafts but less noticeable color banding.
- Default: 0.5
- Syntax: ***Float***

### $Falloff:

- This allows for an exponential falloff of the light shaft intensity the farther it gets away from the sun.
- Default: 1.0
- Syntax: ***Float***

### $Weight:

- This changes the amount of light every sample adds to the final brightness. Change this and $Intensity to vary contrast.
- Default: 0.02
- Syntax: ***Float***

### $Intensity:

- This is the overall intensity of the effect after all other calculations are done.
- Default: 0.5
- Syntax: ***Float***

### $Sample Number:

- This is the amount of texture samples used for every pixel on screen. Increasing this improves quality but also slows down the shader. Low end hardware should go with 25. High end can go up to or over 100.
- Default: 50
- Syntax: ***Integer***

## Sample

This is the default table built into the engine, used when no post_processing.tbl is found. It has no ship effects or light shafts section.

```
#Effects

$Name:			distort noise
$Uniform:		noise_amount
$Define:		FLAG_DISTORT_NOISE
$AlwaysOn: 		false
$Default:		0.0
$Div:			20000
$Add:			0

$Name:			saturation
$Uniform:		saturation
$Define:		FLAG_SATURATION
$AlwaysOn: 		false
$Default:		0.9
$Div:			50
$Add:			0

$Name:			brightness
$Uniform:		brightness
$Define:		FLAG_BRIGHTNESS
$AlwaysOn: 		false
$Default:		1.11
$Div:			50
$Add:			0

$Name:			contrast
$Uniform:		contrast
$Define:		FLAG_CONTRAST
$AlwaysOn: 		false
$Default:		1.015
$Div:			50
$Add:			0

$Name:			film grain
$Uniform:		film_grain
$Define:		FLAG_GRAIN
$AlwaysOn: 		false
$Default:		0.1
$Div:			50
$Add:			0

$Name:			stripes
$Uniform:		tv_stripes
$Define:		FLAG_STRIPES
$AlwaysOn:		false
$Default:		0.0
$Div:			50
$Add:			0

$Name:			cutoff
$Uniform:		cutoff
$Define:		FLAG_CUTOFF
$AlwaysOn:		false
$Default:		2.0
$Div:			50
$Add:			0.0

$Name:			dithering
$Uniform:		dither
$Define:		FLAG_DITH
$AlwaysOn:		false
$Default:		0.0
$Div:			50
$Add:			0

$Name:			tint
$Uniform:		tint
$Define:		FLAG_TINT
$AlwaysOn: 		false
$Default:		0.0
$Div:			1
$Add:			0
$RGB:           0.2,0.2,0.2

#End
```

The other two sections look like this:

```
#Ship Effects

$Name:                  Cloak
$Shader Effect:         2
$Disables Rendering:    YES
$Invert timer:          YES

$Name:                  Decloak
$Shader Effect:         2
$Disables Rendering:    NO
$Invert timer:          NO

#Light Shafts
$AlwaysOn:              NO
$Density:               0.5
$Falloff:               1.0
$Weight:                0.02
$Intensity:             0.5
$Sample Number:         50
```
