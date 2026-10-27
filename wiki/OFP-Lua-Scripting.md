> **Revision information**
> OFP Revision: [#36](https://github.com/ofp-fs2open/fs2open.github.com/pull/36)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

This page covers the Lua scripting API that OFP adds on top of stock FreeSpace Open. The full, always-current reference is the scripting documentation the engine writes itself (run with `-output_scripting`). This page adds context and examples for the OFP-specific parts. Everything else works as described on the [FreeSpace Wiki scripting pages](https://wiki.hard-light.net/index.php/Scripting.tbl).

## Contents

1. [Coordinate points](#coordinate-points)
   1. [mn.CoordinatePoints](#mncoordinatepoints)
   2. [coordinatepoint object](#coordinatepoint-object)
   3. [Coordinate points in other API types](#coordinate-points-in-other-api-types)
2. [Ship custom data](#ship-custom-data)
   1. [ship:getCustomData](#shipgetcustomdata)
   2. [ship:setCustomData](#shipsetcustomdata)
   3. [ship:hasCustomData](#shiphascustomdata)
   4. [ship:clearCustomData](#shipclearcustomdata)
3. [Intel entry tech models](#intel-entry-tech-models)
   1. [intelentry:renderTechModel](#intelentryrendertechmodel)
   2. [intelentry:renderTechModel2](#intelentryrendertechmodel2)
4. [Documentation corrections](#documentation-corrections)
5. [Sample](#sample)

---

## Coordinate points

Coordinate points are an OFP object type: named locations in a mission that can be drawn in-game as a shape and targeted by the player. See [coordinate_points.tbl](coordinate_points.tbl) for their shapes and mission-file fields.

### mn.CoordinatePoints

**OFP, 26.1:**

A library (part of `mn`) for accessing the mission's coordinate points.

| Member | Returns | Description |
|---|---|---|
| `mn.CoordinatePoints[IndexOrName]` | `coordinatepoint` | Gets a point by name, or by 1-based index. Returns an invalid handle if not found. |
| `#mn.CoordinatePoints` | `number` | The number of coordinate points in the mission. |

- A name is tried first, then a number. So `mn.CoordinatePoints["2"]` returns a point *named* `2` if there is one, and otherwise the second point.

### coordinatepoint object

**OFP, 26.1:**

A handle to one coordinate point. It **derives from `object`**, so everything available on an object handle also works on it: `Position`, `Orientation`, `isValid()`, `getBreedName()` and so on. `tostring()` on it gives `"<name> coordinate point"`.

Every property below can be read and set. On an invalid handle, reading returns the default shown in the table.

| Member | Type | Description | Invalid-handle value |
|---|---|---|---|
| `Name` | `string` | The point's name. SEXPs refer to the point by this name. | `""` |
| `DisplayName` | `string` | The name shown on the HUD (target box, brackets, escort list) instead of `Name`. Doesn't have to be unique. Empty means the HUD shows `Name`, so set it to `""` to go back to that. | `""` |
| `Category` | `string` | Free-text label, shown on the HUD target box and brackets in place of a class name. | `""` |
| `EscortPriority` | `number` | `0` = not on the escort list. Any value above 0 means it is on the list, and higher values sort higher. Setting a negative value stores 0. | `0` |
| `VisibleInMission` | `boolean` | Whether the shape is drawn in-game (not just in the editor). Also allows target-in-front targeting. A hidden point can't stay targeted and isn't shown on the escort list. Same as the mission flag `visible_in_mission`. | `false` |
| `AlwaysRenderLabels` | `boolean` | While the point is visible, the HUD draws its display name and category beside it even when it isn't targeted. Same as the mission flag `always_render_labels`. | `false` |
| `MultiTeam` | `number` | `-1` = seen by all teams. `0` or higher = seen only by that team in team-vs-team multiplayer. Single player ignores it. Out-of-range values become `-1`. | `-1` |

| Function | Returns | Description |
|---|---|---|
| `addToEscortList()` | `boolean` | Adds the point to the player's escort list. Does nothing, and returns `false`, unless `EscortPriority` is above 0. Returns `true` if the point was added or was already on the list. |
| `removeFromEscortList()` | `boolean` | Removes the point from the escort list. Does nothing if it isn't on it. Returns `false` only for an invalid handle. |

Colour, shape, size and angle are not exposed to Lua. They come from the mission file.

### Coordinate points in other API types

**OFP, 26.1:**

- **`object`:** any API that hands back a generic object (for example, the player's current target) returns a `coordinatepoint` handle when the object is a coordinate point.
- **`oswpt`:** SEXP arguments that accept a ship/wing/waypoint/team can also hold a coordinate point. On such an argument, `getType()` returns `"coordinatepoint"` and `get()` returns the `coordinatepoint` handle.

---

## Ship custom data

Ships can carry named [custom data](editor.tbl) values, set in qtFRED's Ship Editor, by [SEXPs](OFP-SEXPs#ship-custom-data-operators), or by these methods. Values are always strings. A key exists only if the mission saved it or a SEXP or script set it; editor.tbl defaults are editor-only and are not applied at runtime.

### ship:getCustomData

**OFP, 26.1:**

```
string ship:getCustomData(string key, [string defaultValue = ""])
```

- Returns the stored value, or `defaultValue` if the key isn't set.
- Returns `defaultValue` for an invalid ship handle.

### ship:setCustomData

**OFP, 26.1:**

```
boolean ship:setCustomData(string key, [string value])
```

- Sets the key to `value`. With no value, removes the key.
- Returns `false` for an invalid ship handle, otherwise `true`.

### ship:hasCustomData

**OFP, 26.1:**

```
boolean ship:hasCustomData([string key])
```

- With a key: whether that key is set. Without: whether the ship has any custom data.
- Returns `false` for an invalid ship handle.

### ship:clearCustomData

**OFP, 26.1:**

```
boolean ship:clearCustomData([string key])
```

- With a key: removes it. Without: removes every key.
- Returns `false` for an invalid ship handle, otherwise `true`.

Changes made through Lua are local: unlike the SEXP writes, they are not sent to other players in multiplayer.

---

## Intel entry tech models

Intel entries can have a 3D model through `$Tech Model:` in species.tbl (see [species.tbl](species.tbl#intel-entry-tech-models)). These functions draw that model from a script, for example in a custom tech room UI.

### intelentry:renderTechModel

**OFP, 26.1:**

```
boolean intelentry:renderTechModel(number X1, number Y1, number X2, number Y2,
    [number RotationPercent = 40, number PitchPercent = 0, number BankPercent = 0,
     number Zoom = 1.3, boolean Lighting = true])
```

- Draws the entry's tech model inside the screen rectangle `X1,Y1` to `X2,Y2`, using the entry's `+Closeup_pos:` and `+Closeup_zoom:`.
- `RotationPercent`, `PitchPercent` and `BankPercent` set the model's orientation, each as a percentage of a full turn (0 to 100).
- `Lighting = false` uses flat lighting.
- Returns `false` if the entry has no tech model.

This matches `shipclass:renderTechModel`.

### intelentry:renderTechModel2

**OFP, 26.1:**

```
boolean intelentry:renderTechModel2(number X1, number Y1, number X2, number Y2,
    [orientation Orientation = nil, number Zoom = 1.3])
```

- Same as `renderTechModel`, but the orientation is given as an `orientation` object. Useful when a script tracks rotation itself.
- Returns `false` if the entry has no tech model.

This matches `shipclass:renderTechModel2`.

---

## Documentation corrections

OFP corrects the documented defaults for `shipclass:renderTechModel`, `weaponclass:renderTechModel`, `propclass:renderTechModel` and `ui.Briefing.renderBriefingModel`. The docs used to list `RotationPercent = 0, BankPercent = 40`, but the engine has always used **`RotationPercent = 40, BankPercent = 0`**. Only the documentation changed, so existing scripts behave as before.

---

## Sample

List every coordinate point, and put the visible ones on the escort list:

```lua
for i = 1, #mn.CoordinatePoints do
    local cp = mn.CoordinatePoints[i]
    if cp:isValid() then
        ba.print(string.format("%s [%s] at %s\n", cp.Name, cp.Category, tostring(cp.Position)))

        if cp.VisibleInMission and cp.EscortPriority == 0 then
            cp.EscortPriority = 10
            cp:addToEscortList()
        end
    end
end
```

Read a ship's custom data, with a fallback when it isn't set:

```lua
local ship = mn.Ships["Alpha 1"]
if ship:isValid() then
    local bounty = tonumber(ship:getCustomData("Bounty", "0"))
    if ship:getCustomData("Veteran") == "true" then
        bounty = bounty * 2
    end
    ship:setCustomData("Bounty", tostring(bounty))
end
```

Reveal a point by name when an event fires:

```lua
local rally = mn.CoordinatePoints["Rally Alpha"]
if rally:isValid() then
    rally.VisibleInMission = true
end
```
