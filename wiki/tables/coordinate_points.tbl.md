> **Revision information**
> OFP Revision: [#32](https://github.com/ofp-fs2open/fs2open.github.com/pull/32)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg) This table exists only in OFP.

The **coordinate_points.tbl** defines custom 2D shapes that [coordinate points](#using-shapes-in-a-mission) can be drawn as.

Coordinate points are an OFP object type: named locations in a mission, placed in qtFRED, that can be shown in-game as a flat shape that always faces the camera, and that the player can target. Most points use one of the built-in shapes (a regular polygon or a star). This table adds new shapes on top of those.

This table is one of the Modular Tables and can be extended with **xxx-cps.tbm**.

Code: [/coordinate_points/coordinate_shapes.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/coordinate_points/coordinate_shapes.cpp). See [Tables](Tables) for every table.

The engine includes a built-in copy of this table that defines the **Cross** shape.

## Contents

1. [Format](#format)
2. [#Coordinate Shapes](#coordinate-shapes)
   1. [$Name:](#name)
   2. [$Vertices:](#vertices)
   3. [$Triangles:](#triangles)
3. [#End](#end)
4. [Loading rules](#loading-rules)
5. [Designing a shape](#designing-a-shape)
6. [Using shapes in a mission](#using-shapes-in-a-mission)
7. [Sample](#sample)

---

## Format

- The table starts with `#Coordinate Shapes`.
- It contains any number of shape entries, each beginning with `$Name:`.
- Every entry needs all three fields, in order: `$Name:`, `$Vertices:`, `$Triangles:`.
- The table ends with `#End`.
- A `*-cps.tbm` file uses exactly the same format.

---

## #Coordinate Shapes

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg)

- Section header. Required at the start of the table.

### $Name:

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg)

- The name of the shape. Missions refer to the shape by this name in a coordinate point's `+Shape:` field.
- Names are case-insensitive.
- Must be unique across the default table and all `*-cps.tbm` files. A duplicate is skipped with a warning, and the first definition wins.
- `NGon` and `Star` are reserved for the built-in shapes. An entry using either name is skipped with a warning.
- **Avoid** `Triangle`, `Diamond` and `Pentagon`. The table accepts them, but missions treat those names as the older aliases for the built-in 3-, 4- and 5-sided polygons, so a tabled shape with one of those names can never be selected.
- **Syntax:** *String*

### $Vertices:

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg)

- The corner points of the shape, as a flat list of `x y` pairs.
- Coordinates are in **unit radius**: `1.0` is the edge of the coordinate point's drawn radius. The built-in shapes fit inside -1 to 1 on both axes.
- `+x` is right and `+y` is up, as seen on screen. By convention shapes point up, the same way the built-in polygons and stars do.
- Vertices are numbered from **0** in the order listed. `$Triangles:` refers to them by that number.
- Must contain an even number of values, and at least one pair. Otherwise the entry is skipped with a warning.
- **Limit:** a shape can have at most **128** vertices. Larger shapes load without a warning but are never drawn.
- **Syntax:** *( Float Float  Float Float ... )*

### $Triangles:

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg)

- How the vertices are filled in, as a flat list of vertex numbers, three per triangle.
- List each triangle's vertices **counter-clockwise**, the convention used by the built-in shapes.
- Every number must refer to an existing vertex, from `0` up to one less than the number of vertices. Otherwise the entry is skipped with a warning.
- The list length must be a non-zero multiple of 3. Otherwise the entry is skipped with a warning.
- **Limit:** at most **384** numbers (128 triangles). Larger shapes load without a warning but are never drawn.
- Comments (`;`) can go inside the list to label groups of triangles.
- **Syntax:** *( Integer Integer Integer  ... )*

---

## #End

![OFP 26.1.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/ofp-26.1.0.svg)

- Required at the end of the table.

---

## Loading rules

1. **The default table loads first.** If the mod provides a `coordinate_points.tbl` of its own, that file is used *instead of* the built-in one. The built-in **Cross** shape is then gone unless the mod's table defines it again. To keep Cross, add shapes through a `*-cps.tbm` instead.
2. **Then every `*-cps.tbm` loads**, in the usual modular-table order.
3. **Modular tables can only add shapes.** They cannot change or remove an existing shape, and there is no `+nocreate`. A name that already exists is skipped with a warning.
4. **Broken entries are skipped with a warning** (reserved or duplicate name, bad vertex count, bad triangle list), and the rest of the table still loads.

---

## Designing a shape

- **Size.** The whole shape is scaled by the coordinate point's drawn radius. It grows with the square root of the camera's distance, so far-away points stay readable, and its radius never gets smaller than `2 × +Size` metres. Design within about -1 to 1 and set the final size per point in the mission with [+Size:](#using-shapes-in-a-mission).
- **Rotation.** A mission's `+Angle:` rotates any shape, including tabled ones. Design the shape pointing up and rotate it per point, rather than tabling a separate shape for each direction.
- **Overlapping triangles.** Points are drawn with alpha blending, and every triangle is drawn separately. With a translucent colour, any area covered by two triangles looks more solid. The built-in Cross does this where its two bars cross. To avoid it, split the shape so no triangles overlap.
- **Hidden behind objects.** Shapes are hidden behind ships and other geometry, but they don't block anything themselves (including lightshafts). They behave like a HUD marker, not a physical object.

---

## Using shapes in a mission

Coordinate points go in the mission file's `#Coordinate Points` section, after `#Props` (or `#Wings`) and before `#Events`. qtFRED writes this section for you. It's listed here so the shape-related fields are documented in one place.

| Field | Description | Default |
|---|---|---|
| `$Name:` | The point's name. SEXPs and scripts refer to the point by it. **Required.** | — |
| `+Display Name:` | The name the HUD shows (target box, brackets, escort list) instead of `$Name:`. Doesn't have to be unique. Saved as a translatable `XSTR`. | *(shows `$Name:`)* |
| `$Location:` | Position in the world. **Required.** | — |
| `+Category:` | A free-text label. It is shown on the HUD target box and target brackets where a ship's class name would normally appear. It has no other effect. Saved as a translatable `XSTR`. Called `+Group:` before OFP's selection groups took that name. | *(empty)* |
| `+Color:` | `( R G B )` or `( R G B A )`, each from 0 to 255. | `255 255 255 255` |
| `+Shape:` | `NGon`, `Star`, or the `$Name:` of a tabled shape. The older aliases `Triangle`, `Diamond` and `Pentagon` mean `NGon` with 3, 4 and 5 sides. An unknown name falls back to a triangle (`NGon`, 3 sides), with a warning. | `NGon` |
| `+Sides:` | Number of sides for `NGon`, from 3 to 32. | `4` |
| `+Points:` | Number of points for `Star`, from 3 to 32. | `5` |
| `+Inner Radius:` | How far in a `Star`'s inner corners sit, from 0.05 to 0.95. | `0.382` |
| `+Angle:` | Extra rotation in degrees. Works on all shapes, including tabled ones. | `0` |
| `+Size:` | Size multiplier, from 0.25 to 8.0. | `1.0` |
| `+Escort Priority:` | `0` = not on the escort list. Any value above 0 puts the point on the escort list, and higher values sort higher. | `0` |
| `+Multi Team:` | `-1` = seen by all teams. `0` or higher = seen only by that team in team-vs-team multiplayer. Single player ignores it. | `-1` |
| `+Flags:` | Any of: `"visible_in_mission"` makes the shape draw in-game, not just in the editor, and lets the player target it with target-in-front. A point without it can't stay targeted and isn't shown on the escort list. `"always_render_labels"` makes the HUD draw the point's display name and category beside it while it's visible, even when it isn't targeted. Change them during a mission with [alter-point-flag](OFP-SEXPs#alter-point-flag). | *(none)* |
| `+Layer:` | qtFRED view layer. Editor only. | `Default` |
| `+Group:` | qtFRED selection groups, as a number with one bit per group (group N is bit N-1). Editor only. | *(none)* |

Values outside the ranges above are clamped.

---

## Sample

`data/tables/mymod-cps.tbm`, adding an up-pointing arrow:

```
#Coordinate Shapes

$Name: Arrow
$Vertices: (
	 0.00  1.00    -0.60  0.20     0.60  0.20
	-0.20  0.20    -0.20 -1.00     0.20 -1.00     0.20  0.20
)
$Triangles: (
	; Head
	0 1 2
	; Shaft
	3 4 5     3 5 6
)

#End
```

The built-in Cross, from the engine's default `coordinate_points.tbl`:

```
#Coordinate Shapes

$Name: Cross
$Vertices: (
	-0.33  1.00     0.33  1.00     0.33  0.33     1.00  0.33
	 1.00 -0.33     0.33 -0.33     0.33 -1.00    -0.33 -1.00
	-0.33 -0.33    -1.00 -0.33    -1.00  0.33    -0.33  0.33
)
$Triangles: (
	;  Vertical bar
	 0  7  6      0  6  1
	; Horizontal bar
	10  9  4     10  4  3
)

#End
```

A mission coordinate point using the Arrow shape, rotated to point right:

```
#Coordinate Points

$Name: Rally Alpha
$Location: 1200.0, 0.0, 4500.0
+Category: Rally Point
+Color: ( 80 200 255 220 )
+Shape: Arrow
+Angle: -90
+Size: 1.5
+Flags: ( "visible_in_mission" )
```
