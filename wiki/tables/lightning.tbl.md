> **Revision information**
> OFP Revision: [#126](https://github.com/ofp-fs2open/fs2open.github.com/pull/126)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **lightning.tbl** is used to define the different storms seen in game (such as nebula conditions): the lightning bolt types, and the storms that fire them.

This table is one of the Modular Tables and can be extended with **xxx-ltng.tbm**.

Code: [/nebula/neblightning.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/nebula/neblightning.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in. Fields without a badge are from the original game.

## Contents

1. [Format](#format)
2. [Bolts](#bolts)
3. [Storms](#storms)
4. [Sample](#sample)

---

## Format

- Lightning.tbl consists of two sections **#Bolts begin** that defines the various lightning bolt types used and the **#Storms begin** that defines the method how the different lightning bolts are used in the storms. Bolts sections ends with **#Bolts end** and storm section with **#Storms end**.
- Both sections are required in every file, modular tables included (a modular table that only adds storms still needs an empty `#Bolts begin` / `#Bolts end`).
- Each entry's fields are optional, but must be in the order listed here. A field that isn't given keeps its default (or, when editing an existing entry, its current value).

![FSO 3.6.14](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.6.14.svg)

- The hardcoded limit of a maximum of 10 different bolts and storm types has been removed in favor of a dynamic system. This allows the creation of a wider range of nebula lightning and storm types within a campaign.

## Bolts

### $Bolt:

- Used to define the name of the bolt.
- A name used twice in lightning.tbl is an error. In a modular table, a name that already exists edits that bolt.
- Syntax: **String**

### +nocreate

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Optional. Goes directly after the name.
- In a modular table, edits the bolt only if it already exists, instead of creating a new one. Using it in lightning.tbl itself logs a warning.

### +b_scale:

- Defines the 'spread' of the child forks. Smaller values cause tighter bolts.
- Syntax: **Float**
- Default: 0.5

### +b_shrink:

- Defines the length of the 'child' forks, as a fraction (from 0 to 1) of the distance to the point they aim at
- Syntax: **Float**
- Default: 0.3

### +b_poly_pct:

- Defines the width of the lightning compared to the length of the lightning in percents. Large values are not recommended, retail values are from 0.002 to 0.0075.
- Syntax: **Float**
- Default: 0.005

### +b_rand:

- Sets the probability (from 0 to 1) of generating 'child lightnings' for the 'nodes' of the main lightning. High values may cause the game to run out of lightning nodes.
- Syntax: **Float**
- Default: 0.3

### +b_add:

- Sets the drawing distance (in meters) of the lightning glow from the actual lightning towards the viewpoint, same as the z_add used with beams. Changing this value with lightnings is not recommended.
- Syntax: **Float**
- Default: 2.0

### +b_strikes:

- Defines the number of times the bolt strikes
- Syntax: **Integer**
- Default: 3

### +b_lifetime:

- Defines the total lifetime of the bolt in milliseconds. Lenght of individual strikes is **+b_lifetime:** divided by **+b_strikes:**
- Syntax: **Integer**
- Default: 250

### +b_noise:

- Defines the 'noise' of a multi striking lightning (from 0 to 1). Cause the lightning to 'imitate' movement. Large values are not recommended.
- Syntax: **Float**
- Default: 0.03

### +b_emp:

- Sets the EMP effect caused by the lightning. First the EMP intensity followed by time the EMP affects the ships in seconds.
- Syntax: **Float** **Float**
- Default: 0.0 0.0 (no EMP)

### +b_texture:

- Defines the texture used for the main bolt texture. ![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) Specifying *none* or *<none>* will use no texture, resulting in an invisible bolt.
- A texture that can't be loaded logs a warning.
- Syntax: **String**
- Default: none

### +b_glow:

- Defines the texture used for the main bolt glow. ![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg) Specifying *none* or *<none>* will omit the glow.
- A texture that can't be loaded logs a warning.
- Syntax: **String**
- Default: none

### +b_bright:

- Defines the brightness of the bolts. Higher values cause screen to flash brighter
- Syntax: **Float**
- Default: 0.2

## Storms

### $Storm:

- Defines the name of the storm. This is the name chosen in the mission editor.
- A name used twice in lightning.tbl is an error. In a modular table, a name that already exists edits that storm.
- Syntax: **String**

### +nocreate

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Optional. Goes directly after the name.
- In a modular table, edits the storm only if it already exists, instead of creating a new one. Using it in lightning.tbl itself logs a warning.

### +Clear bolts

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Will remove all previously parsed bolts from the storm. Without it, `+bolt:` lines in a modular table are added to the storm's existing bolts.
- Must come before the storm's `+bolt:` lines.
- Can only be used in a modular table; in lightning.tbl it logs a warning and is ignored.

### +bolt:

- Each storm may have up to 10 bolts defined. One bolt type may be listed under a same storm more than once to change the odds of that lightnings appearance.
- Type of the lightning is randomly determined from the listed bolts.
- A storm must end up with at least one bolt; one without any is an error.
- Bolt names are checked after every table has been read, so a storm can use a bolt from a later modular table. An unknown bolt logs a warning and is skipped.
- **Known issue:** an 11th `+bolt:` logs an error, but its bolt name is left unread, which breaks the parsing of everything after it in that file. Keep each storm to 10 lines.
- Syntax: **String**

### +flavor:

- Flavor defines a vector that is added to the lightnings. So it forces the lightnings to be aligned along the vector. Smaller values have lesser effect while extremely large values (longer vector) cause linear lightning bolts.
- Syntax: **Float** **Float** **Float**
- Default: 0.0 0.0 0.0 (completely random)

### +random_freq:

- Defines the **minimum** and **maximum** for the random lightning 'firings' in milliseconds
- Syntax: **Integer** **Integer**
- Default: 600 1200

### +random_count:

Defines the **minimum** and **maximum** number of lightnings fired at each timestamp
- Syntax: **Integer** **Integer**
- Default: 1 3

## Sample

- Retail FreeSpace 2 table

```
;  Man, do I like this table :)

; individual bolt definitions. can have up to 10 different bolt types
#Bolts begin

;; individual lightning bolt types
$Bolt:		b_standard
	; this is a percentage of the length of a given beam. envision a circle in the plane of where the bolt
	; strikes. the radius of this circle is the length of the beam * b_scale. when generating child forks on the
	; bolt, a point is picked randomly within ths circle and that's where thr fork "aims". smaller values produce a
	; tighter bolt
	+b_scale:					0.5
	
	; when generating the child forks, this is % of the distance from the start of the fork to the point its aiming at
	; that will be its endpoint. So if you had a value of 0.25, each child fork would be 25% the distance to the strike plane
	+b_shrink:					0.3			

	; this is the thickness of the main trunk of a bolt relative to the length of the bolt. so 0.003 means the bolt is 0.3% as
	; wide as it is long
	+b_poly_pct:				0.002	

	; this is the % chance that any given node on the main trunk will spawn a child node. BE CAREFUL WITH THIS! too high of
	; a percentage will generate mega-lightning bolts which will run out of nodes. =o
	+b_rand:						0.24

	; this should probably never be messed with. its just a fudge factor which ensures that the glow of a beam is drawn closer
	; to the eye that the bolt itself (so its always "over" the bolt). in meters.
	+b_add:						2.0

	; the # of times the bolt "strikes" 
	+b_strikes:					2

	; the total lifetime of the bolt (in ms). the length of an individual strike is b_lifetime / b_strikes
	+b_lifetime:				280

	; the "noise" in a multi-strike bolt. each of the nodes in the bolt is perturbed a bit between strikes to make it seem
	; like its moving. large values don't look too good
	+b_noise:					0.015

	; the EMP effect caused by proximity to this lightning bolt. max intensity is 500.0. max time is 30 seconds
	+b_emp:						0.0 0.0

	; main bolt texture
	+b_texture:					lightning

	; main bolt glow
	+b_glow:						beam-dblue			

	; brightness of the bolt. 0.0 to 1.0. higher values result in brighter screen flashes
	+b_bright:					0.3

$Bolt:		b_red
	+b_scale:					0.2	
	+b_shrink:					0.3				
	+b_poly_pct:				0.0075		
	+b_rand:						0.45
	+b_add:						2.0	
	+b_strikes:					4	
	+b_lifetime:				200	
	+b_noise:					0.04	
	+b_emp:						0.0	0.0	
	+b_texture:					lightning	
	+b_glow:						beam-red				
	+b_bright:					0.2

$Bolt:		b_green
	+b_scale:					0.3	
	+b_shrink:					0.3				
	+b_poly_pct:				0.0075		
	+b_rand:					0.2
	+b_add:						2.0	
	+b_strikes:					3	
	+b_lifetime:				240	
	+b_noise:					0.03	
	+b_emp:						0.0	0.0	
	+b_texture:					lightning	
	+b_glow:						beam-green3		
	+b_bright:					0.14

$Bolt:		b_emp
	+b_scale:					0.6	
	+b_shrink:					0.3				
	+b_poly_pct:				0.0075		
	+b_rand:						0.3
	+b_add:						2.0	
	+b_strikes:					5	
	+b_lifetime:				300	
	+b_noise:					0.04	
	+b_emp:						350.0	5.0	
	+b_texture:					lightning	
	+b_glow:						beam-dblue		
	+b_bright:					0.4

#Bolts end

; storm types
#Storms begin

; this is what the mission designers will choose in fred. can have up to 10 different storm varieties
$Storm:							s_standard
	; can have a max of 10 different bolt types. one will be randomly chosen. if you want to make bolts happen more or 
	; less often, add them multiple times. So if you had 4 b_standard bolts and 1 b_red bolt, the b_red bolt would happen
	; 1/5 as often as the b_standard
	+bolt:						b_standard	
	+bolt:						b_standard	
	+bolt:						b_red
	+bolt:						b_green

	; direction vector value. flavor of the storm. if all 3 values are 0, its completely random. otherwise lightning bolts will tend to
	; align themselves to the vector. the longer the flavor vector, the more tightly aligned the bolts will be
	+flavor:						0.0 0.0 0.0

	; how often random bolts are fired, min-max range, in ms
	+random_freq:				1500 1750

	; min/max. how many bolts are spewed each random timestamp
	+random_count:				1 2

$Storm:							s_active
	; can have a max of 10 different bolt types. one will be randomly chosen. if you want to make bolts happen more or 
	; less often, add them multiple times. So if you had 4 b_standard bolts and 1 b_red bolt, the b_red bolt would happen
	; 1/5 as often as the b_standard
	+bolt:						b_standard	
	+bolt:						b_standard	
	+bolt:						b_standard	
	+bolt:						b_standard		
	+bolt:						b_red

	; direction vector value. flavor of the storm. if all 3 values are 0, its completely random. otherwise lightning bolts will tend to
	; align themselves to the vector. the longer the flavor vector, the more tightly aligned the bolts will be
	+flavor:						0.0 500.0 0.0

	; how often random bolts are fired, min-max range, in ms
	+random_freq:				200 400

	; min/max. how many bolts are spewed each random timestamp
	+random_count:				1 4

$Storm:							s_emp
	+bolt:						b_standard	
	+bolt:						b_standard	
	+bolt:						b_standard	
	+bolt:						b_red	
	+bolt:						b_red
	+bolt:						b_emp
	+bolt:						b_red	
	+bolt:						b_red	
	
	+flavor:						0.0 500.0 0.0

	; how often random bolts are fired, min-max range, in ms
	+random_freq:				300 700

	; min/max. how many bolts are spewed each random timestamp
	+random_count:				1 4

$Storm:							s_medium
	+bolt:						b_standard	
	+bolt:						b_standard	
	+bolt:						b_red	
	+bolt:						b_red
	+bolt:						b_standard	
	+bolt:						b_standard	
	+bolt:						b_green

	+flavor:						0.1 1.0 0.2

	; how often random bolts are fired, min-max range, in ms
	+random_freq:				900 1850

	; min/max. how many bolts are spewed each random timestamp
	+random_count:				1 3

#Storms end
```
