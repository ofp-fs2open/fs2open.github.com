> **Revision information**
> OFP Revision: [#139](https://github.com/ofp-fs2open/fs2open.github.com/pull/139)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **scripting.tbl** attaches Lua scripts to the game: it says which scripts run, and when (the hooks) and where (the conditions). It is used for features such as custom HUDs, new game mechanics and interface changes.

This table is one of the Modular Tables and can be extended with **xxx-sct.tbm**. ![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg) Plain Lua files named **xxx-sct.lua** in the tables folder are also run, once, at game init.

Code: [/scripting/scripting.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/scripting/scripting.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in.

## Contents

1. [General Info](#general-info)
2. [Hook reference](#hook-reference)
3. [Script code](#script-code)
4. [#Conditional Hooks](#conditional-hooks)
5. [#Global Hooks](#global-hooks)
6. [Examples](#examples)

---

## General Info

- The table has two sections, both optional, in this order: `#Global Hooks` (deprecated) and `#Conditional Hooks`. Each ends with `#End`.
- Most scripting is done with conditional hooks: a set of conditions, followed by one or more hooks (actions) with the script to run when those conditions are true.
- Every table adds to the ones before it; a modular table never replaces another table's scripts.
- OFP's own scripting additions (new Lua functions, and hooks OFP adds) are listed on [OFP Lua Scripting](OFP-Lua-Scripting).

## Hook reference

The engine has around 90 hooks, and the list changes from version to version, so it isn't copied here. The engine writes a complete, current reference itself: run the game once with the **-output_scripting** command line option, and it writes **scripting.html** listing every hook with its description, the hook variables it provides (read in Lua through the `hv` table, for example `hv.Ship`), the conditions it accepts, whether it can be overridden, and which hooks are deprecated. The same reference is available as JSON with **-output_script_json**, and as Lua stubs for code editors with **-output_script_lua**.

A hook name is written in the table with a `$` in front and a `:` after, for example `$On Ship Arrive:` for the hook "On Ship Arrive".

- A deprecated hook logs a warning. If the mod targets the version the hook was deprecated in, or a later one (see `$Minimum version:` in [game_settings.tbl](game_settings.tbl)), using it is an error; a hook that has been removed is always an error.

## Script code

Every hook (and its `+Override:`) is followed by the script to run, in one of three forms:

- **`[` code `]`**: Lua code written in the table, between square brackets. It can span many lines. Line numbers in Lua error messages match the table's lines.
- **`[[` file name `]]`**: a Lua file from the **data/scripts** folder, for example `[[myhud.lua]]`. A file that can't be found logs a warning.
- Anything else: the rest of the line is used as one Lua expression (as if it were written `return ...`).

### +Override:

- Optional, directly after a hook's script. Only some hooks can be overridden (the reference above says which).
- A second script, run when the event happens. If it returns **true**, the game skips its own default behavior for that event (for example, the default HUD, or the default death of a ship). The hook's normal script still runs.
- The override only counts if the hook has a script of its own as well.
- Syntax: script code, as above

## #Conditional Hooks

Each conditional hook is any number of conditions, followed by one or more hooks. All conditions must be true for the hooks to run. A new set of conditions starts after the last hook of the previous set.

```
#Conditional Hooks
$Application: FS2_Open
$State: GS_STATE_GAME_PLAY
$On Frame: [
    -- runs every frame while in a mission
]
$On HUD Draw: [[myhud.lua]]
#End
```

There are two kinds of conditions:

- **Global conditions**, listed below, work with any hook. They are checked first, before Lua is run at all.
- **Hook conditions** belong to particular hooks: for example `$Ship class:` or `$Weapon class:` for the hooks that concern a ship or a weapon. Which hook conditions a hook accepts is listed in the [hook reference](#hook-reference). A hook condition the hook doesn't accept logs a warning, and **that hook then never runs**.

Conditions that come before several hooks apply to every one of those hooks. A set of conditions with no hook after it is an error.

### Global conditions

#### $Application:

- Which program the scripts run in: **FS2_Open** (also FS2Open, Freespace 2, Freespace) or **FRED2_Open** (also FRED2Open, FRED 2, FRED). Not case sensitive.

#### $State:

- The game state the game must be in, such as GS_STATE_MAIN_MENU or GS_STATE_GAME_PLAY. See the [List of Game States](https://wiki.hard-light.net/index.php/List_of_Game_States).

#### $Campaign:

- The file name of the current campaign (the .fc2 extension is optional).
- **Known issue:** the engine compares this value against the current *mission's* file name instead of the campaign's, so the condition doesn't work as described. Check the campaign in the script instead.

#### $Mission:

- The file name of the current mission (the .fs2 extension is optional). It stays true after the mission ends, until another mission starts.

#### $KeyPress:

- A key that must be held down, by its name in the engine's key list, with any of the modifiers Alt, Shift or Cheat in the name.

#### $Version:

![FSO 21.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-21.0.svg)

- The scripts run only on this exact engine version.
- **Known issue:** the version isn't read from this line but from the line after it, which is consumed. In practice the condition swallows the following line (usually the next condition or the hook) and the version almost never matches. Don't use it.

#### $Multi type:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- **Server** (or Master) to run only on the multiplayer host (and in single player); anything else, such as **Client**, to run only on multiplayer clients.

#### $VR device:

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- **VR** (or HMD, enabled) to run only when playing in VR; anything else to run only when not.

## #Global Hooks

**Deprecated.** These are older, fixed hooks; each one is the same as a conditional hook with no conditions. Use `#Conditional Hooks` instead. They must be in this order, and each takes [script code](#script-code):

- **$Global:** the same as `$On Frame:`.
- **$Splash:** the same as `$On Splash Screen:`.
- **$GameInit:** the same as `$On Game Init:`.
- **$Simulation:** the same as `$On Simulation:`.
- **$HUD:** the same as `$On HUD Draw:`.

## Examples

See the FreeSpace Wiki's [Scripting Examples category](https://wiki.hard-light.net/index.php/Category:Scripting_Examples).
