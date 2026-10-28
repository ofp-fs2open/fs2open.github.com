> **Revision information**
> OFP Revision: [#135](https://github.com/ofp-fs2open/fs2open.github.com/pull/135)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

The **default_settings.tbl** sets the default values of the game's options (the in-game options menu): the values used on first launch, or for any option the player hasn't changed and saved. It can also lock an option to its default and hide it from the menu.

This table is one of the Modular Tables and can be extended with **xxx-defs.tbm**.

Code: [/options/default_settings_table.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/options/default_settings_table.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in.

## Contents

1. [Format](#format)
2. [$Option Key:](#option-key)
3. [Options](#options)
4. [Sample](#sample)

---

## Format

- The table begins with **#DEFAULT SETTINGS** (optional) and ends with **#END**.
- In between, any number of `$Option Key:` entries, in any order.
- An option set again by a later table replaces the earlier value.

## $Option Key:

- The key of the option, from the [list below](#options). Not case sensitive.
- A key that doesn't exist is only noted in the log and skipped. This is on purpose: an option may be missing in some builds or modes (for example, the window mode option in VR) and the table still works.
- An option that can't take a default value (listed at the end of the [options](#options)) logs a warning and is skipped.
- Syntax: **String**

### +Value:

- Required. The default value, in the format the option's row shows. A value that isn't valid logs a warning and the option keeps its built-in default.

### +Enforce

- Optional, after `+Value:`. Also accepted as `+Enforced`.
- Locks the option to this value: the player's saved value is ignored and the option is removed from the options menu.
- Options marked *retail* below are also in the original game's options screen, which can't hide them, so they can't be enforced; trying logs a warning.

## Options

Values:
- **Boolean**: YES or NO.
- **Detail list**: four integers from 0 to 4 in parentheses, the value this option takes in each of the detail presets, from lowest to highest: `( 1 2 3 4 )`. A value outside 0 to 4 logs a warning and that preset keeps its default.
- **Text** values are not case sensitive.

| Key | Option in the menu | Value | Version |
|---|---|---|---|
| Audio.Effects | Effects (*retail*) | **Float**, 0 to 1 (clamped) | 25.0 |
| Audio.Music | Music (*retail*) | **Float**, 0 to 1 (clamped) | 25.0 |
| Audio.Voice | Voice (*retail*) | **Float**, 0 to 1 (clamped) | 25.0 |
| Game.FlightCursorDeadzone | Flight Cursor Deadzone | **Float**, radians, 0 to 0.349 (clamped) | 25.0 |
| Game.FlightCursorExtent | Flight Cursor Extent | **Float**, radians, 0 to 0.698 (clamped) | 25.0 |
| Game.FlightMode | Flight Mode | **Text**: ship locked, flight cursor | 25.0 |
| Graphics.3DShockwaves | Shockwaves | **Boolean** | 25.0 |
| Graphics.AAMode | Anti Aliasing | **Text**: none, fxaa low, fxaa medium, fxaa high, smaa low, smaa medium, smaa high, smaa ultra | 25.0 |
| Graphics.BloomIntensity | Bloom intensity | **Integer**, 0 to 200 (clamped) | 25.0 |
| Graphics.CockpitFOV | Cockpit Field Of View | **Float**, radians, 0.436 to 1.5708 (25 to 90 degrees, clamped) | 26.2 |
| Graphics.CockpitFOVToggle | Cockpit FOV Toggle | **Boolean** | 26.2 |
| Graphics.DeferredCockpitLighting | Deferred Cockpit Lighting | **Boolean** | 25.0 |
| Graphics.DeferredLighting | Deferred Lighting | **Boolean** | 25.0 |
| Graphics.Detail | Model Detail (*retail*) | **Detail list** | 25.0 |
| Graphics.FOV | Field Of View | **Float**, radians, 0.436 to 1.5708 (25 to 90 degrees, clamped) | 25.0 |
| Graphics.Gamma | Brightness (*retail*) | **Float**, 0.1 to 5.0 in steps of 0.05 | 25.0 |
| Graphics.HDR | HDR Output | **Boolean** | 26.2 |
| Graphics.HDRPaperWhite | HDR Paper White | **Float**, nits | 26.2 |
| Graphics.HDRPeakLuminance | HDR Peak Luminance | **Float**, nits | 26.2 |
| Graphics.Lighting | Lighting (*retail*) | **Detail list** | 25.0 |
| Graphics.Lightshafts | Lightshafts | **Boolean** | 25.0 |
| Graphics.MSAASamples | Multisample Anti Aliasing | **Text**: off, 4 samples, 8 samples | 25.0 |
| Graphics.MotionDebris | Motion Debris | **Boolean** | 25.0 |
| Graphics.NebulaDetail | Nebula Detail (*retail*) | **Detail list** | 25.0 |
| Graphics.Particles | Particles (*retail*) | **Detail list** | 25.0 |
| Graphics.PostProcessing | Post processing | **Boolean** | 25.0 |
| Graphics.RenderAPI | Render API | **Text**: opengl, vulkan | 26.2 |
| Graphics.ShadowRenderMethod | Shadow Method | **Text**: shadow map (or shadow maps), raytraced | 26.2 |
| Graphics.Shadows | Shadow Quality | **Text**: disabled, low, medium, high, ultra | 25.0 |
| Graphics.ShieldEffects | Shield Hit Effects (*retail*) | **Detail list** | 25.0 |
| Graphics.SmallDebris | Impact Effects (*retail*) | **Detail list** | 25.0 |
| Graphics.SoftParticles | Soft Particles | **Boolean** | 25.0 |
| Graphics.Stars | Stars (*retail*) | **Detail list** | 25.0 |
| Graphics.Sunglare | Sunglare | **Boolean** | 25.0 |
| Graphics.Texture | 3D Hardware Textures (*retail*) | **Detail list** | 25.0 |
| Graphics.TextureFilter | Texture Filtering | **Text**: bilinear, trilinear | 25.0 |
| Graphics.VSync | Vertical Sync | **Boolean** | 25.0 |
| Graphics.WindowMode | Window Mode | **Text**: windowed, borderless, fullscreen | 25.0 |

These options exist but can't take a default value from this table: Audio.BriefingVoice, Audio.CaptureDevice, Audio.EnableEFX, Audio.PlaybackDevice, Game.BriefIconScaleFactor, Game.Discord, Game.FontScaleFactor, Game.IffAccessibility, Game.Language, Graphics.Anisotropy, Graphics.Display, Graphics.MaxRtShadowLights, Graphics.MaxRtShadowLocalLights, Graphics.RTShadowQuality, Graphics.Resolution, Graphics.ResolutionVR, Graphics.RtShadowBiasMax, Graphics.RtShadowBiasMin, HUD.Radar2dIconMode, Input.FFStrength, Input.ForceFeedback, Input.HitEffect, Input.Joystick, Input.Joystick1, Input.Joystick2, Input.Joystick3, Input.JoystickDeadZone, Input.JoystickSensitivity, Input.MouseSensitivity, Input.UseMouse, Multi.FlushCache, Multi.LocalBroadcast, Multi.TogglePXO, Multi.TransferMissions, Speech.Briefing, Speech.Ingame, Speech.Multi, Speech.Rate, Speech.Techroom, Speech.Volume.

## Sample

```
#DEFAULT SETTINGS

$Option Key: Graphics.AAMode
+Value: smaa high

$Option Key: Audio.Music
+Value: 0.6

$Option Key: Graphics.Lighting
+Value: ( 1 2 3 4 )

$Option Key: Game.FlightMode
+Value: flight cursor
+Enforce

#END
```
