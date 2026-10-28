> **Revision information**
> OFP Revision: [#77](https://github.com/ofp-fs2open/fs2open.github.com/pull/77)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

This page lists every table documented on this wiki, in alphabetical order. Table documentation is moving here from the [FreeSpace Wiki](https://wiki.hard-light.net/index.php/Tables) one table at a time; tables not listed yet are still documented there. Pages for tables that exist only in OFP say so at the top.

## Contents

1. [Version badges](#version-badges)
2. [Tables](#tables)

---

## Version badges

Table pages mark when a field or behavior was added with a version badge. Fields without a badge are from the original game.

**FreeSpace Open:**

![FSO 3.6.x](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.x.svg) ![FSO 3.6.10](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.10.svg) ![FSO 3.6.12](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.12.svg) ![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg) ![FSO 3.6.16](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.16.svg) ![FSO 3.7](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.svg) ![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg) ![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) ![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg) ![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg) ![FSO 20.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-20.4.svg) ![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) ![FSO 21.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.2.svg) ![FSO 21.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.4.svg) ![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg) ![FSO 22.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.2.svg) ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) ![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg) ![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) ![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg) ![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) ![FSO 26.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.0.svg) ![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

**OFP:**

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg)

---

## Tables

\* Notes Modular Tables, \*\* notes tables that only exist as modular tables

| Table | Modular extension | Code | Purpose |
|---|---|---|---|
| [ai.tbl](ai.tbl)\* | `xxx-aic.tbm` | [/ai/aicode.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/ai/aicode.cpp) | AI classes: skill values per difficulty level, and AI profile overrides |
| [ai_profiles.tbl](ai_profiles.tbl)\* | `xxx-aip.tbm`, `xxx-msp.tbm` | [/ai/ai_profiles.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/ai/ai_profiles.cpp) | AI profiles: difficulty scaling, AI behavior flags and bug-fix toggles a mission can pick |
| [Animation (-anim.tbm)](Animation)\*\* | `xxx-anim.tbm` | [/model/animation/modelanimation.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/model/animation/modelanimation.cpp) | Model subobject animations and moveables (modular tables only) |
| [armor.tbl](armor.tbl)\* | `xxx-amr.tbm` | [/ship/ship.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/ship/ship.cpp) | Armor types and how much damage each damage type does to them |
| [asteroid.tbl](asteroid.tbl)\* | `xxx-ast.tbm` | [/asteroid/asteroid.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/asteroid/asteroid.cpp) | Asteroids and debris pieces for asteroid and debris fields |
| [autopilot.tbl](autopilot.tbl)\* | `xxx-aplt.tbm` | [/autopilot/autopilot.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/autopilot/autopilot.cpp) | Autopilot link distance, interruptions and messages |
| [cheats.tbl](cheats.tbl)\* | `xxx-cht.tbm` | [/cheats_table/cheats_table.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/cheats_table/cheats_table.cpp) | Custom cheat codes that show a message, run a script hook or summon a ship |
| [colors.tbl](colors.tbl)\* | `xxx-clr.tbm` | [/globalincs/alphacolors.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/globalincs/alphacolors.cpp) | Named colors, team colors, interface colors and text color tags |
| [controlconfigdefaults.tbl](controlconfigdefaults.tbl) | (not modular) | [/controlconfig/controlsconfigcommon.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/controlconfig/controlsconfigcommon.cpp) | Default key, mouse and joystick bindings, control presets and controls menu settings |
| [coordinate_points.tbl](coordinate_points.tbl)\* | `xxx-cps.tbm` | [/coordinate_points/coordinate_shapes.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/coordinate_points/coordinate_shapes.cpp) | Custom 2D shapes for coordinate points (OFP only) |
| [credits.tbl](credits.tbl)\* | `xxx-crd.tbm` | [/menuui/credits.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/menuui/credits.cpp) | Credits screen text, music and artwork |
| [curves.tbl](curves.tbl)\* | `xxx-crv.tbm` | [/math/curve.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/math/curve.cpp) | Named curves (X to Y mappings) used by other tables |
| [cutscenes.tbl](cutscenes.tbl)\* | `xxx-csn.tbm` | [/cutscene/cutscenes.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/cutscene/cutscenes.cpp) | Tech room cutscenes: video files, names and descriptions |
| [Decals (-dcl.tbm)](Decals)\*\* | `xxx-dcl.tbm` | [/decals/decals.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/decals/decals.cpp) | Decals (surface marks such as weapon impacts) |
| [default_settings.tbl](default_settings.tbl)\* | `xxx-defs.tbm` | [/options/default_settings_table.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/options/default_settings_table.cpp) | Default values for the in-game options, optionally locked and hidden |
| [Dynamic SEXPs (-sexp.tbm)](Dynamic-SEXPs)\*\* | `xxx-sexp.tbm` | [/parse/sexp/sexp_lookup.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/parse/sexp/sexp_lookup.cpp) | Custom SEXPs written in Lua: new operators, AI goals and player orders, and enum lists (modular tables only) |
| [editor.tbl](editor.tbl)\* | `xxx-edt.tbm` | [/mission/missionparse.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/mission/missionparse.cpp) | Editor settings, and custom data fields for missions, campaigns and ships (OFP only, editor only) |
| [fireball.tbl](fireball.tbl)\* | `xxx-fbl.tbm` | [/fireball/fireballs.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/fireball/fireballs.cpp) | Explosion and warp effect animations |
| [fonts.tbl](fonts.tbl)\* | `xxx-fnt.tbm` | [/graphics/software/font.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/graphics/software/font.cpp) | Bitmap (VFNT) and TrueType/OpenType fonts |
| [game_settings.tbl](game_settings.tbl)\* | `xxx-mod.tbm` | [/mod_table/mod_table.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/mod_table/mod_table.cpp) | Global game options (also known as mod.tbl) |
| [generated_nebula.tbl](generated_nebula.tbl)\* | `xxx-gneb.tbm` | [/starfield/nebula.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/starfield/nebula.cpp) | Patterns and colors for the generated (FS1-style) background nebula (OFP only) |
| [glowpoints.tbl](glowpoints.tbl)\* | `xxx-gpo.tbm` | [/model/modelread.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/model/modelread.cpp) | Glowpoint presets: blinking, pulsing and light |
| [help.tbl](help.tbl)\* | `xxx-hlp.tbm` | [/gamehelp/contexthelp.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/gamehelp/contexthelp.cpp) | F1 help overlays for interface screens |
| [hud_gauges.tbl](hud_gauges.tbl)\* | `xxx-hdg.tbm` | [/hud/hudparse.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/hud/hudparse.cpp) | HUD layouts and gauges, and the HUD options screen |
| [icons.tbl](icons.tbl)\* | `xxx-ico.tbm` | [/mission/missionbriefcommon.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/mission/missionbriefcommon.cpp) | Briefing icons for each species and icon type |
| [iff_defs.tbl](iff_defs.tbl)\* | `xxx-iff.tbm` | [/iff_defs/iff_defs.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/iff_defs/iff_defs.cpp) | Teams (IFFs): who attacks whom, and team and HUD colors |
| [lighting_profiles.tbl](lighting_profiles.tbl)\* | `xxx-ltp.tbm` | [/lighting/lighting_profiles.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/lighting/lighting_profiles.cpp) | Tonemapping, exposure and light brightness profiles |
| [lightning.tbl](lightning.tbl)\* | `xxx-ltng.tbm` | [/nebula/neblightning.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/nebula/neblightning.cpp) | Nebula lightning bolts and storms |
| [mainhall.tbl](mainhall.tbl)\* | `xxx-hall.tbm` | [/menuui/mainhallmenu.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/menuui/mainhallmenu.cpp) | Main hall screens: backgrounds, doors, animations and sounds |
| [medals.tbl](medals.tbl)\* | `xxx-mdl.tbm` | [/stats/medals.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/stats/medals.cpp) | Medals, badges and the medal case |
| [messages.tbl](messages.tbl)\* | `xxx-msg.tbm` | [/mission/missionmessage.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/mission/missionmessage.cpp) | Built-in messages, personas, moods and message types |
| [mflash.tbl](mflash.tbl)\* | `xxx-mfl.tbm` | [/weapon/muzzleflash.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/weapon/muzzleflash.cpp) | Weapon muzzle flashes |
| [music.tbl](music.tbl)\* | `xxx-mus.tbm` | [/gamesnd/eventmusic.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/gamesnd/eventmusic.cpp) | Mission soundtracks and menu music |
| [nebula.tbl](nebula.tbl)\* | `xxx-neb.tbm` | [/nebula/neb.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/nebula/neb.cpp) | Full nebula background bitmaps and poofs |
| [objecttypes.tbl](objecttypes.tbl)\* | `xxx-obt.tbm` | [/ship/ship.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/ship/ship.cpp) | Ship types and turret target priority groups |
| [Particle Effects (-part.tbm)](Particle-Effects)\*\* | `xxx-part.tbm` | [/particle/ParticleParse.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/particle/ParticleParse.cpp) | Custom particle effects for impacts, muzzle flashes, trails and more (modular tables only) |
| [post_processing.tbl](post_processing.tbl) | (not modular) | [/graphics/post_processing.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/graphics/post_processing.cpp) | Post-processing screen effects, ship shader effects and light shafts |
| [props.tbl](props.tbl)\* | `xxx-prp.tbm` | [/prop/prop.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/prop/prop.cpp) | Prop classes: static, non-interactive objects placed in missions, and their editor categories |
| [rank.tbl](rank.tbl)\* | `xxx-rnk.tbm` | [/stats/scoring.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/stats/scoring.cpp) | Pilot ranks: points needed, insignia and promotion voice and text |
| [scripting.tbl](scripting.tbl)\* | `xxx-sct.tbm`, `xxx-sct.lua` | [/scripting/scripting.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/scripting/scripting.cpp) | Lua scripts: which hooks run them and under what conditions |
| [ships.tbl](ships.tbl)\* | `xxx-shp.tbm` | [/ship/ship.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/ship/ship.cpp) | Ship classes: models, physics, weapons, subsystems, effects and sounds; engine washes, templates and wing formations |
| [sounds.tbl](sounds.tbl)\* | `xxx-snd.tbm` | [/gamesnd/gamesnd.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/gamesnd/gamesnd.cpp) | Game, interface and flyby sounds, and reverb presets |
| [species.tbl](species.tbl)\* | `xxx-intl.tbm` | [/menuui/techmenu.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/menuui/techmenu.cpp) | Tech room intel entries (only OFP's additions documented so far) |
