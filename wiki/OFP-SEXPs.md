> **Revision information**
> OFP Revision: [#36](https://github.com/ofp-fs2open/fs2open.github.com/pull/36)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

This page lists the SEXP operators that exist only in OFP, and the OFP changes to existing SCP operators. Every other operator works in OFP exactly as documented on the [FreeSpace Wiki SEXP page](https://wiki.hard-light.net/index.php/SEXP). Only the differences are listed here.

A mission that uses anything on this page needs an OFP build. Stock FreeSpace Open will report the new operators as unknown when it loads the mission.

## Contents

1. [New operators](#new-operators)
   1. [point-targeted](#point-targeted)
   2. [alter-point-flag](#alter-point-flag)
   3. [Ship custom data operators](#ship-custom-data-operators)
2. [Coordinate points in existing operators](#coordinate-points-in-existing-operators)
3. [Props in existing operators](#props-in-existing-operators)
   1. [replace-texture](#replace-texture)
   2. [activate-glow-maps / deactivate-glow-maps](#activate-glow-maps--deactivate-glow-maps)
   3. [Props that haven't spawned yet](#props-that-havent-spawned-yet)

---

## New operators

`point-targeted` and `alter-point-flag` work on coordinate points: named locations in a mission that can be drawn in-game as a shape and targeted by the player. See [coordinate_points.tbl](coordinate_points.tbl) for their shapes and mission-file fields. The [ship custom data operators](#ship-custom-data-operators) read and change named values stored on ships.

Category and subcategory are where each operator appears in the editor's operator menus.

| Operator | Type | Category | Subcategory |
|---|---|---|---|
| [point-targeted](#point-targeted) | Boolean | Training | *(none; Training has no subcategories)* |
| [alter-point-flag](#alter-point-flag) | Action | Change | Special Effects |
| [ship-custom-data-is](#ship-custom-data-is) | Boolean | Status | Ship Status |
| [ship-custom-data-has-key](#ship-custom-data-has-key) | Boolean | Status | Ship Status |
| [ship-custom-data-get-int](#ship-custom-data-get-int) | Number | Status | Ship Status |
| [ship-custom-data-get-bool](#ship-custom-data-get-bool) | Boolean | Status | Ship Status |
| [ship-custom-data-set](#ship-custom-data-set) | Action | Change | Ship Status |
| [ship-custom-data-set-int](#ship-custom-data-set-int) | Action | Change | Ship Status |
| [ship-custom-data-set-bool](#ship-custom-data-set-bool) | Action | Change | Ship Status |
| [ship-custom-data-clear](#ship-custom-data-clear) | Action | Change | Ship Status |

### point-targeted

**OFP, 26.1:**

**Category:** Training (no subcategory)

```
( point-targeted <coordinate point> [seconds] )
```

True while the player has the coordinate point targeted. This is the coordinate-point version of `targeted` and `node-targeted`.

| # | Argument | Description |
|---|---|---|
| 1 | Coordinate point | The point to check. Only coordinate points are accepted. **Required.** |
| 2 | Seconds | Optional. The point must have been the player's target for at least this long. |

- False if the point doesn't exist or the player has no target.
- Target-in-front only picks up points that are drawn in-game (the `visible_in_mission` flag, set in the mission file or with [alter-point-flag](#alter-point-flag)).

### alter-point-flag

**OFP, 26.1:**

**Category:** Change → Special Effects

```
( alter-point-flag <flag> <true|false> <coordinate point> [<coordinate point> ...] )
```

Sets or clears a flag on one or more coordinate points. This is the coordinate-point version of `alter-ship-flag`.

| # | Argument | Description |
|---|---|---|
| 1 | Flag | A coordinate point flag name (see below). The editor lists the valid names. **Required.** |
| 2 | Set | `true` to set the flag, `false` to clear it. **Required.** |
| Rest | Coordinate points | One or more points to change. At least one is **required.** |

| Flag | Effect |
|---|---|
| `visible_in_mission` | The point is drawn in-game and target-in-front can pick it up. A hidden point is drawn only in the editor, can't stay targeted (clearing the flag drops it as the player's target), and isn't shown on the escort list. Same as `VisibleInMission` in [Lua](OFP-Lua-Scripting#coordinatepoint-object). |
| `always_render_labels` | While the point is visible, the HUD draws its display name and category beside it even when it isn't targeted. Same as `AlwaysRenderLabels` in [Lua](OFP-Lua-Scripting#coordinatepoint-object). |

- The flags are the same ones the mission file's `+Flags:` field uses (see [coordinate_points.tbl](coordinate_points.tbl#using-shapes-in-a-mission)).
- In multiplayer, the change is sent to all clients.
- This operator replaced `toggle-point-visibility`. A mission that used it needs editing by hand: `( toggle-point-visibility ( true ) "P" )` becomes `( alter-point-flag "visible_in_mission" ( true ) "P" )`.

### Ship custom data operators

These read and change a ship's [custom data](editor.tbl): named values stored on an individual ship, set in qtFRED's Ship Editor or by these operators. Values are always stored as text; the `-int` and `-bool` variants read and write that text as a number or true/false.

Common rules:

- The **key** argument offers the `#ShipCustomData` keys from [editor.tbl](editor.tbl), plus keys already used on ships in the mission, as a dropdown, but any key can be typed. Keys are case-sensitive and are never checked against the table while the mission runs, so a mistyped key simply reads as missing.
- **Reads** (`-is`, `-has-key`, `-get-int`, `-get-bool`) wait until the ship has arrived, and stop being evaluated once it has left.
- **Writes** (`-set`, `-set-int`, `-set-bool`, `-clear`) to a ship that hasn't arrived yet are stored and apply when it arrives. Writes to a ship that has left are ignored. In multiplayer, writes are sent to all clients.
- Scripts have the same access through [Lua](OFP-Lua-Scripting#ship-custom-data).

#### ship-custom-data-is

**OFP, 26.1:**

**Category:** Status → Ship Status

```
( ship-custom-data-is <ship> <key> <value> )
```

True if the ship has the key and its stored text exactly matches the value (case-sensitive). For true/false fields, use [ship-custom-data-get-bool](#ship-custom-data-get-bool), which ignores case.

#### ship-custom-data-has-key

**OFP, 26.1:**

**Category:** Status → Ship Status

```
( ship-custom-data-has-key <ship> <key> )
```

True if the ship has the key, whatever its value.

#### ship-custom-data-get-int

**OFP, 26.1:**

**Category:** Status → Ship Status

```
( ship-custom-data-get-int <ship> <key> )
```

The key's value as a number. Returns `0` if the key is missing or its value isn't a number.

#### ship-custom-data-get-bool

**OFP, 26.1:**

**Category:** Status → Ship Status

```
( ship-custom-data-get-bool <ship> <key> )
```

True if the key's value is `true`, in any capitalization. False for any other value, or if the key is missing.

#### ship-custom-data-set

**OFP, 26.1:**

**Category:** Change → Ship Status

```
( ship-custom-data-set <ship> <key> <value> )
```

Sets the key to the given text, overwriting any existing value.

#### ship-custom-data-set-int

**OFP, 26.1:**

**Category:** Change → Ship Status

```
( ship-custom-data-set-int <ship> <key> <number> )
```

Sets the key to a number.

#### ship-custom-data-set-bool

**OFP, 26.1:**

**Category:** Change → Ship Status

```
( ship-custom-data-set-bool <ship> <key> <true|false> )
```

Sets the key to exactly `true` or `false`, matching what qtFRED writes.

#### ship-custom-data-clear

**OFP, 26.1:**

**Category:** Change → Ship Status

```
( ship-custom-data-clear <key> <ship> [<ship> ...] )
```

Removes the key from each listed ship. Ships without the key are unaffected. To remove every key from a ship, use Lua's `ship:clearCustomData()`.

---

## Coordinate points in existing operators

**OFP, 26.1:**

Operators whose arguments take a ship, wing or waypoint also accept a coordinate point's name. The point is used as a position, just as a waypoint is.

| Operator | What a coordinate point can be used as |
|---|---|
| `distance` | Either object |
| `distance-to-center` | Either object |
| `distance-to-bbox` | Either object |
| `distance-center-to-subsystem` | The object measured from |
| `distance-bbox-to-subsystem` | The object measured from |
| `get-object-x` / `get-object-y` / `get-object-z` | The object whose coordinate is returned |
| `angle-facing-object` | The target object |
| `is-in-box` | The object tested |
| `set-object-position` | The object moved. The move is sent to clients in multiplayer. |
| `set-object-facing-object` | The object to face only. A point given as the object to turn is ignored, as with waypoints. |
| `set-camera-facing-object` | The object the camera faces |
| `set-camera-host` | The camera host |
| `set-camera-target` | The camera target |

- If a name matches both a waypoint and a coordinate point, the waypoint wins. Give points names that no other object uses.

---

## Props in existing operators

### replace-texture

**OFP, 26.1:**

The objects the texture is replaced on (the third argument onward) can include props as well as ships and wings.

- On a prop that is in the mission, the change applies immediately to the prop's model, as it does for a ship already in the mission.
- On a prop that hasn't spawned yet, the replacement is queued and applied when the prop spawns.
- For a replacement set in the table instead, see [props.tbl: Texture replacement](props.tbl#texture-replace).

### activate-glow-maps / deactivate-glow-maps

**OFP, 26.1:**

These accept props as well as ships. The prop must be in the mission. A prop that hasn't spawned yet is skipped.

### Props that haven't spawned yet

**OFP, 26.1:**

A prop with a spawn cue exists in the mission file before it is actually in the mission. In-game, any operator argument that takes a prop now accepts the name of a prop that hasn't spawned yet, instead of rejecting it as invalid. What the operator does with such a prop is up to the operator (see above).

This applies to `prop-vanish`, `replace-texture`, the glow map operators, and the collision group operators (`add-to-collision-group`, `remove-from-collision-group`, their `-new` versions, and `get-collision-group`).

The ship-or-prop arguments of the glow map and collision group operators likewise accept a ship that hasn't arrived yet.
