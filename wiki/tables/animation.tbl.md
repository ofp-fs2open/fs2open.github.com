> **Revision information**
> OFP Revision: [#106](https://github.com/ofp-fs2open/fs2open.github.com/pull/106)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

Animation tables define subobject animations and moveables for models: sequences of rotations, movements, sounds and particles that play on a trigger, and submodels whose targets can be changed during a mission. Ships, weapons and skyboxes then list the animations they use (see [Using animations](#using-animations)).

There is no animation.tbl itself: animations are only read from modular tables named **xxx-anim.tbm**. Animation and moveable names must be unique across all of them.

Code: [/model/animation/modelanimation.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/model/animation/modelanimation.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in.

## Contents

1. [#Animations](#animations)
2. [#Moveables](#moveables)
3. [Using animations](#using-animations)
4. [Driven animations](#driven-animations)
5. [Example](#example)

---

## #Animations

Animations defined here are a pre-defined sequence of movement and actions, that you can then trigger to play/reverse during a mission.

### $Name:

- The name used to identify the animation when adding it to a model. Must be unique.

### $Type:

- The type of trigger that will start this animation
  - **initial**
    - Animation is started (and completed) when the mission begins.
  - **on-spawn**
    - Animation is started once the ship or modelled weapon spawns.
  - **docking-stage-1**
    - Animation code is started when the ship follows the docking path.
  - **docking-stage-2**
    - Animation code is started when the ship moves to the second to last point in the docking path.
  - **docking-stage-3**
    - Animation code is started when the ship is moving onto the dockpoint itself.
  - **docked**
    - Animation code is started once the ship is docked.
  - **primary-bank**
    - Animation code is started when a primary bank is selected (armed).
  - **primary-fired**
    - Animation code is started when a primary bank has fired (Useful for recoil or similar things). Needs an auto-resetting flag (see [below](#flags))
  - **secondary-bank**
    - Animation code is started when a secondary bank is selected (armed).
  - **secondary-fired**
    - Animation code is started when a secondary bank has fired (Useful for recoil or similar things). Needs an auto-resetting flag (see [below](#flags))
  - **fighterbay**
    - Animation code starts when ship launches from or receives others through its fighterbay.
  - **afterburner**
    - Animation code is started when ships afterburner is engaged.
  - **turret-firing**
    - Animation code is started while a turret is firing. Gets triggered once a turret starts firing and ends once it has no more targets to shoot.
  - **turret-fired**
    - Animation code is started whenever a turret has fired (Useful for recoil or similar things). Triggers on each shot. Needs an auto-resetting flag (see [below](#flags))
  - **scripted**
    - Animation code is started when it is triggered with scripts or SEXPs (see [scripting.tbl](scripting.tbl))

Older names still work and log which name replaced them: `docking` (now `docking-stage-2`), `primary_bank`, `secondary_bank`, `door` (now `fighterbay`) and `turret firing`. The misspelling `inital` works with a warning, and type names in quotes work with a note to remove the quotes. A trigger type that needs an auto-resetting flag (`primary-fired`, `secondary-fired`, `turret-fired`) logs a warning without one.

#### +Triggered By:

- Optional more accurate specification of when the animation triggers. Form and value depends on the trigger type. No `+Triggered By:` usually means "in every applicable case".
  - **initial**, **on-spawn**, **afterburner**
    - No further specification is possible, `+Triggered By:` must not be specified
  - **docking-stage-1**, **docking-stage-2**, **docking-stage-3**, **docked**
    - The docking port number or name on the ship
  - **primary-bank**, **primary-fired**, **secondary-bank**, **secondary-fired**
    - The number of the weapon bank
  - **fighterbay**
    - The number or name of the fighterbay path to open. Can be prefixed with a `NOT` to trigger when any fighterbay path except for the specified is used.
  - **turret-firing**, **turret-fired**
    - The name of the turret subsystem that has fired
  - **scripted**
    - An arbitrary, user-defined name to specify in a triggering SEXP or Lua method

### $Flags:

- Syntax: ***( "String" "String" )***, names of the flags assigned to the ship
  - Some flags are so-called "Auto-resetting" flags. These will cause an animation to not stay in their completed state, but in their untriggered state. This can happen by automatically reversing the animation, resetting it once it is complete, or else, depending on the flag used.

#### "auto reverse"

- Once the animation completes, automatically reverse it with no explicit trigger to do so
  - Is an auto-resetting flag

#### "reset at completion"

- Once the animation ends, reset its state to the start of the animation. Only useful if the animation ends in the same position as it starts in.
  - Is an auto-resetting flag

#### "loop"

- The animation will automatically reverse when complete, and then restart itself once it's back in it's original position, repeating infinitely
  - If used together with the "reset at completion" flag, it will instantly snap back to the start and resume playing instead of reversing the animation at completion.

#### "random starting phase"

- The animation will start at a random point of its duration instead of at the beginning. Useful if you trigger multiple looping animations at once, but don't want the animations to synchronize to each other

#### "pause on reverse"

- The animation will pause instead of starting in reverse when triggered to play in reverse. This is *only ever* useful when using a looping animation with some sort of engine-trigger, like "afterburner"

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

#### "seamless with startup x"

- A flag for any animation that is a seamless animation which has a startup prepended. Replace x with the time from which the animation needs to be looped to be seamless. If the animation is started forwards, it'll play the startup animation, and from then on will repeat from the specified time, playing the animation seamlessly. If reversed, it will play until the point where the seamless animation starts and then repeat back to the animations end. When stop-looping-animation is used, the animation will play until the next time it reaches the point where the seamless part starts, and will then play the startup part in reverse to then stop in its initial state.

### Segment

- Here, the actual animation is specified. The segment that is defined here, needs to be one of the following:

#### Coordinate modes

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

`$Set Orientation:`, `$Set Offset:`, `$Rotation:` and `$Translation:` take an optional mode right after their angle or vector:

- `+Relative` (the default): the value is an amount to rotate or move by, from wherever the submodel is.
- `+Absolute`: the value is the target, measured from the submodel's current state, including every animation playing on it.
- `+Local`: the value is the target, measured only from what earlier segments of this same animation have done. Other animations playing on the same submodel don't shift it. This is how `+Absolute` behaved before 23.0.

#### $Set Orientation:

- This segment instantly sets the orientation of a submodel. Especially useful for initial-type animations.

##### +Angle:

- Sets the angle this submodel should be at.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in degrees (equal to pitch, heading, bank)

##### +Relative, +Local or +Absolute

- ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `+Relative`, `+Local` or `+Absolute` (optional; see [coordinate modes](#coordinate-modes)). Without one, the angle is what the submodel is rotated by from its current orientation.

##### +Submodel:

- The name of the submodel this segment applies to.
  - If the target submodel is part of a multipart turret, use `+Turret Base:` or `+Turret Arm:` respectively instead of `+Submodel:`, followed by the *subsystem* name of the turret.
    - Note that multipart turrets may only be rotated along their axis. Any other rotation or translation is possible (for cutscenes, for example), but will cause undefined behavior once the turret starts aiming on its own again. Use carefully.

---

#### $Set Offset:

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- This segment instantly moves a submodel to an offset. The position counterpart of `$Set Orientation:`, and useful for initial-type animations.

##### +Target:

- The offset to move the submodel to.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in meters

##### +Relative, +Local or +Absolute

- ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `+Relative`, `+Local` or `+Absolute` (optional; see [coordinate modes](#coordinate-modes)). Without one, the submodel is moved by the vector from its current position.

##### +Submodel:

[See above](#submodel)

---

#### $Set Angle:

- This segment instantly rotates a submodel on its rotation axis. Useful for initial-type animations, especially on multipart turrets.

##### +Angle:

- Sets the angle this submodel should be at.
  - Syntax: **float**, angle in degrees

##### +Submodel:

[See above](#submodel)

---

#### $Rotation:

- This segment rotates a submodel. Out of `+Angle:`, `+Velocity:` and `+Time:`, exactly two must be defined. The third will be automatically calculated to match the two specified properties.

##### +Angle:

- Sets the angle this submodel should rotate by.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in degrees (equal to pitch, heading, bank)

###### +Relative, +Local or +Absolute

- ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `+Relative`, `+Local` or `+Absolute` (optional; see [coordinate modes](#coordinate-modes)). Without one, the angle is what the submodel is rotated by from its current orientation.
- With `+Local` or `+Absolute`, acceleration values almost certainly need to be non-zero and large enough in all three axes for the movement, unless acceleration is left out for pure linear motion.

##### +Velocity:

- Sets the velocity this submodel should rotate at. The sign is only relevant in **Velocity+Time**-mode, where it defines rotation direction. In the other cases, the sign is ignored.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in degrees per second (equal to pitch, heading, bank)

##### +Time:

- Sets the time this submodel takes to complete the rotation.
  - Syntax: **float**, seconds

##### +Acceleration:

- **Optional**: Sets the acceleration of this submodel rotation. The sign is irrelevant.
  - Note: If the acceleration is too small, the rotation might not be fully completed. Depends on rotation mode:
    - **Angle+Time**-mode: The acceleration is too small to reach the target angle within the rotation time.
    - **Angle+Velocity**-mode: The acceleration is too small to reach the target velocity until the angle is reached.
    - **Velocity+Time**-mode: The acceleration is too small to reach the target velocity within the rotation time.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in degrees per second squared (equal to pitch, heading, bank)

##### +Submodel:

[See above](#submodel)

---

#### $Axis Rotation:

- This segment rotates a submodel around a specified axis. Out of `+Angle:`, `+Velocity:` and `+Time:`, exactly two must be defined. The third will be automatically calculated to match the two specified properties.

##### +Axis:

- Specified the axis this submodel will rotate around. Does not need to be normalized
  - Syntax: **Vector**, three *floats*, x, y, z respectively

##### +Angle:

- Sets the angle this submodel should rotate by.
  - Syntax: **float**, in degrees

##### +Velocity:

- Sets the velocity this submodel should rotate at. The sign is only relevant in **Velocity+Time**-mode, where it defines rotation direction. In the other cases, the sign is ignored.
  - Syntax: **float**, in degrees per second

##### +Time:

- Sets the time this submodel takes to complete the rotation.
  - Syntax: **float**, seconds

##### +Acceleration:

- **Optional**: Sets the acceleration of this submodel rotation. The sign is irrelevant.
  - Note: If the acceleration is too small, the rotation might not be fully completed. Depends on rotation mode:
    - **Angle+Time**-mode: The acceleration is too small to reach the target angle within the rotation time.
    - **Angle+Velocity**-mode: The acceleration is too small to reach the target velocity until the angle is reached.
    - **Velocity+Time**-mode: The acceleration is too small to reach the target velocity within the rotation time.
  - Syntax: **float**, in degrees per second squared

##### +Submodel:

[See above](#submodel)

---

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

#### $Translation:

- This segment moves a submodel. Out of `+Vector:`, `+Velocity:` and `+Time:`, exactly two must be defined. The third will be automatically calculated to match the two specified properties.

##### +Vector:

- Sets the movement vector this submodel should move by.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in meters

###### +Relative, +Local or +Absolute

- ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) `+Relative`, `+Local` or `+Absolute` (optional; see [coordinate modes](#coordinate-modes)). Without one, the vector is what the submodel is moved by from its current position.
- With `+Local` or `+Absolute`, acceleration values almost certainly need to be non-zero and large enough in all three axes for the movement, unless acceleration is left out for pure linear motion.

##### +Velocity:

- Sets the velocity this submodel should move at. The sign is only relevant in **Velocity+Time**-mode, where it defines movement direction. In the other cases, the sign is ignored.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in meters per second

##### +Time:

- Sets the time this submodel takes to complete the movement.
  - Syntax: **float**, seconds

##### +Acceleration:

- **Optional**: Sets the acceleration of this translation. The sign is irrelevant.
  - Note: If the acceleration is too small, the translation might not be fully completed. Depends on translation mode:
    - **Vector+Time**-mode: The acceleration is too small to reach the target vector within the rotation time.
    - **Vector+Velocity**-mode: The acceleration is too small to reach the target velocity until the movement is completed.
    - **Velocity+Time**-mode: The acceleration is too small to reach the target velocity within the translation time.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in meters per second squared

##### +Coordinate System:

- **Optional**: Sets the coordinate system this translation operates in. One of:
  - **Parent** - The translation is in the coordinate system of its parent submodel (so, unless the parent submodel is rotated, the same as the ship's local coordinates).
  - **Local at start** - The translation is in the coordinate system of this submodel as it is once the translation starts, not considering other animations.
  - **Local current** - The translation is in the coordinate system of this submodel as it is currently. This includes rotations parallel to the translation, even from other animations.
- Defaults to **Parent** when not set.

##### +Submodel:

[See above](#submodel)

---

#### $Inverse Kinematics:

- This segment performs inverse kinematics to position a submodel by rotating individual parts of a chain of submodels. It is highly recommended to not run any other animation in parallel to an inverse kinematic segment that operates on one of the chain's submodules.

##### +Target Position:

- Sets the position the last link in the submodule chain should be after the IK segment is complete. Relative to the first link in the chain.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in meters

##### +Target Orientation:

- **Optional**: Sets the orientation the last link in the submodule chain should be after the IK segment is complete. If left out, it will be in the same orientation as the penultimate link. Relative to the first link in the chain.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in meters

##### +Time:

- Sets the time the movement should take.
  - Syntax: **float**, seconds

##### $Chain Link:

- Specifies the individual links. This and all following properties of this segment are repeated for each chain link. There need to be at least two chain links for the IK segment.
- Each new chain link's submodel *must* be a direct child of the preceding chain link's submodel in the pof it is applied to

###### +Submodel:

[See above](#submodel)

###### +Acceleration:

- **Optional**: Sets the acceleration of this submodel rotation. The sign is irrelevant.
  - Note: Make *absolutely* sure that the acceleration is sufficient for the submodel to reach where it needs to go in the time specified, or the chain will not end up where you specified.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in degrees per second squared (equal to pitch, heading, bank)

###### +Constraint:

- **Optional**: Can be used to constrain the movement of the current chain link relative to its parent. Either of:
  - **Window**, used to specify a maximum x, y, z rotation of this submodel
    - **+Window Size:** Specifies this window, **Vector**, three *floats*, x, y, z respectively, in degrees
  - **Hinge**, used to constrain the chain link to rotate on an axis (note, can cause problems, especially as the first link. If issues occur, report them please)
    - **+Axis:** Specifies the rotation axis, **Vector**, three *floats*, x, y, z respectively

---

#### $Sound During:

- Plays a sound during another segment

##### +Submodel:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- **Optional**: If set, a 3D-Sound played with this segment will be anchored to the submodels position. If not set, any sounds will be played as 2D-Sounds
  - Syntax: [See above](#submodel)

##### +Start:

- **Optional**: A sound that should be played once the child segment starts

##### +Loop:

- **Optional**: A sound that should be looped while the child segment runs

##### +End:

- **Optional**: A sound that should be played once the child segment ends

##### +Radius:

- The additional radius for a 3D-Sound playing.
  - Syntax: **float**,

##### +Position:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- **Optional**: The additional offset of the sound position from the submodel center. Only valid if `+Submodel:` is set
  - Syntax: **Vector**, three *floats*, x, y, z respectively

##### +Flip When Reversed

- **Optional**: If set, when the animation is played in reverse, the start sound will play first (so technically at the end of the animation), and the end sound will play once the segment has completely reset.

##### +Don't Interrupt Playing Sounds

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- **Optional**: If set, playing sounds will not be stopped once they start playing. This means start sounds will not be interrupted by end sounds or the end of the segment.

##### Child Segment

Specify a child segment here by normal [segment definition rules](#segment), during which the sound should play.

---

![FSO 26.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.0.svg)

#### $Particles During:

- Spawns a particle effect at a specified time during another animation segment.

##### +Submodel:

- **Optional**: If set, a particle effect played with this segment will be anchored to the submodels position. If not set, it will be anchored to the object if such an object exists, or to world 0, 0, 0 if the animation is not played on an object
  - Syntax: [See above](#submodel)

##### +Effect:

- The particle effect to spawn. Must reference a valid particle effect defined in [Particle_Effects](https://wiki.hard-light.net/index.php/Particle_Effects)).
  - Syntax: *String*, the name of the particle effect

##### +At Time:

- The time in seconds at which the particle effect should be triggered during the animation.
  - Syntax: *float*, seconds

##### +Position:

- **Optional**: An offset position from the anchor point (submodel or object origin).
  - Syntax: **Vector**, three 'floats', x, y, z respectively, in meters

##### +Orientation:

- **Optional**: The orientation of the particle effect. If not set, uses the identity matrix.
  - Syntax: *Vector*, three 'floats', x, y, z respectively, in degrees (pitch, heading, bank)

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

##### +Limit Direction:

- **Optional**: If set, will spawn the particles only if the animation is currently playing forwards or backwards respectively.
  - Syntax: **FWD** or **RWD**

##### Child Segment

Specify a child segment here by normal [segment definition rules](#segment), during which the sound should play.

---

#### $Wait:

- A segment to wait for a specific time.

##### +Time:

- Wait for a specified time
  - Syntax: **float**, seconds

---

#### $Segment Sequential:

- Perform different segments one after another. The length of this segment is the sum of the length of the child segments.

##### +Submodel:

- **Optional**: If a submodel is specified here, it is automatically applied to the child segments, which then don't need to specify a submodel themselves. Explicit definitions by child segments override this setting.
[See above for how to specify a submodel](#submodel)

##### Child Segments

Specify any number of child segments here by normal [segment definition rules](#segment) that should be played one after another.

##### +End Segment

Once the last child segment of this sequence is specified, `+End Segment` must follow.

---

#### $Segment Parallel:

- Perform different segments at the same time. The length of this segment is defined by the longest child segment.

##### +Submodel:

- **Optional**: If a submodel is specified here, it is automatically applied to the child segments, which then don't need to specify a submodel themselves. Explicit definitions by child segments override this setting.
[See above for how to specify a submodel](#submodel)

##### Child Segments

Specify any number of child segments here by normal [segment definition rules](#segment) that should be played at the same time.

##### +End Segment

Once the last child segment of this sequence is specified, `+End Segment` must follow.

## #Moveables

Unlike normal Animations, moveables can only define one specific type of movement, but its movement targets can be set at runtime through SEXP.

### $Name:

- The name used to identify the animation when adding it to a model. Must be unique.

#### $Type: Orientation

This can be used to set the orientation of a submodel during a mission.
The **update-moveable-animation** SEXP expects three numbers after the moveable name: the pitch, heading and bank in degrees the submodels orientation should be set to.

##### +Angle:

- Sets the angle this submodel should point to at the start of the mission.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in degrees (equal to pitch, heading, bank)

##### +Submodel:

[See above](#submodel)

---

#### $Type: Rotation

This can be used to slowly rotate a submodel to a new target orientation during a mission.
The **update-moveable-animation** SEXP expects three numbers after the moveable name: the pitch, heading and bank in degrees the submodel should then rotate towards.

##### +Angle:

- Sets the angle this submodel should point to at the start of the mission.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in degrees (equal to pitch, heading, bank)

##### +Velocity:

- Sets the velocity this submodel should rotate at. The sign is ignored. Must not be 0 in any axis.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in degrees per second (equal to pitch, heading, bank)

##### +Acceleration:

- **Optional**: Sets the acceleration of this translation. The sign is irrelevant. Must not be 0 in any axis.
  - Note: If the acceleration is too small, the defined velocity might not be reached until the target rotation is achieved.

##### +Submodel:

[See above](#submodel)

---

#### $Type: Axis Rotation

This can be used to slowly rotate a submodel around an axis during a mission.
The **update-moveable-animation** SEXP expects one number after the moveable name: the new angle this submodule should rotate towards.

##### +Axis:

- Sets the axis that the moveable should rotate around
  - Syntax: **Vector**, three *floats*, x, y, z respectively, does not need to be normalized

##### +Velocity:

- Sets the velocity this submodel should rotate at. The sign is ignored. Must not be 0.
  - Syntax: **float**, in degrees per second (equal to pitch, heading, bank)

##### +Acceleration:

- **Optional**: Sets the acceleration of this translation. The sign is irrelevant. Must not be 0 if specified
  - Note: If the acceleration is too small, the defined velocity might not be reached until the target rotation is achieved.

##### +Submodel:

[See above](#submodel)

---

#### $Type: Translation

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg) This can be used to slowly translate a submodel to a new target position during a mission.
The **update-moveable-animation** SEXP expects three numbers after the moveable name: the x, y and z in 100th's of a meter the submodel should move towards.

##### +Target:

- Sets the distance this submodel should travel.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in meters

##### +Velocity:

- Sets the velocity this submodel should move at. The sign is ignored. Must not be 0 in any axis.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in meters per second squared

##### +Acceleration:

- **Optional**: Sets the acceleration of this translation. The sign is irrelevant. Must not be 0 in any axis.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in meters per second
  - Note: If the acceleration is too small, the defined velocity might not be reached until the target rotation is achieved.

##### +Submodel:

[See above](#submodel)

---

#### $Type: Inverse Kinematics

This moveable performs inverse kinematics to position a submodel by rotating individual parts of a chain of submodels
The **update-moveable-animation** SEXP expects three numbers after the moveable name: The x, y and z position of the target, in 0.01 meters. Optionally, add the pitch, heading and bank in degrees to specify the end effector orientation target.

##### +Time:

- Sets the time any movement should take.
  - Syntax: **float**, seconds

##### $Chain Link:

- Specifies the individual links. This and all following properties of this segment are repeated for each chain link. There need to be at least two chain links for the IK segment.
- Each new chain link's submodel *must* be a direct child of the preceding chain link's submodel in the pof it is applied to

###### +Submodel:

[See above](#submodel)

###### +Acceleration:

- **Optional**: Sets the acceleration of this submodel rotation. The sign is irrelevant.
  - Note: Make *absolutely* sure that the acceleration is sufficient for the submodel to reach where it needs to go in the time specified, or the chain will not end up where you specified.
  - Syntax: **Vector**, three *floats*, x, y, z respectively, in degrees per second squared (equal to pitch, heading, bank)

###### +Constraint:

- **Optional**: Can be used to constrain the movement of the current chain link relative to its parent. Either of:
  - **Window**, used to specify a maximum x, y, z rotation of this submodel
    - **+Window Size:** Specifies this window, **Vector**, three *floats*, x, y, z respectively, in degrees
  - **Hinge**, used to constrain the chain link to rotate on an axis (note, can cause problems, especially as the first link. If issues occur, report them please)
    - **+Axis:** Specifies the rotation axis, **Vector**, three *floats*, x, y, z respectively


## Using animations

Animations and moveables are attached by name. Each takes a list of names, for example `$Animations: ( "Lid" "Wings" )`.

| Where | Animations | Driven animations | Moveables |
|---|---|---|---|
| [ships.tbl](ships.tbl) | `$Animations:` | `$Driven Animations:` ![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) | `$Animation Moveables:` |
| [ships.tbl](ships.tbl), cockpit | `$Cockpit Animations:` ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) | `$Driven Cockpit Animations:` ![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg) | `$Cockpit Moveables:` ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) |
| [weapons.tbl](weapons.tbl) | `$Animations:` | `$Driven Animations:` ![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg) | |
| Mission skybox | `$Skybox Model Animations:` ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) | | `$Skybox Model Moveables:` ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) |

A name that isn't defined in any xxx-anim.tbm logs an error and is skipped.

Ships can also still use the older animation syntax inside ships.tbl subsystem entries. The engine converts those into these animations, and logs a note suggesting xxx-anim.tbm instead; see [ships.tbl](ships.tbl).

---

## Driven animations

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

Instead of a list of names, `$Driven Animations:` takes one or more driver sets. A driver set ties animations to values from the game, such as the ship's speed or the player's controls:

```
$Driven Animations:
+Driver Set:
	+Time Remap:
		+Source: Speed
		+Curve: SpeedToFrame
	+Property Driver:
		+Source: ControlRotationPitch
		+Target: Speed
	+Startup Driver:
		+Source: Random 0 1
		+Target: Time
	+Affected Animations: ( "Wings" )
```

- `+Driver Set:` starts a set. Any number of sets can follow each other.
- `+Time Remap:` (optional, one per set) plays the animation by a value instead of by time: the animation's position is set from the source every frame, clamped to the animation's length.
- `+Property Driver:` (optional, any number) sets a property of the animation from the source every frame.
- `+Startup Driver:` (optional, any number) sets a property of the animation from the source once, when it starts.
- `+Curve:` (optional, after a source or target) passes the source value through a curve from [curves.tbl](curves.tbl) first.
- `+Target:` (property and startup drivers) is the property to set: `Speed` (the animation's playback speed) or `Time` (its position, clamped to its length; mostly useful as a startup driver).
- `+Affected Animations:` (required) lists the animations the set applies to.

Sources:

| Source | Value |
|---|---|
| `Random` *min* *max* | A random number between *min* and *max*, the same for the same object each time it's read |
| `ControlAccelForward`, `ControlAccelLateral`, `ControlAccelVertical` | The player's throttle and strafe controls |
| `ControlRotationPitch`, `ControlRotationBank`, `ControlRotationHeading` | The player's rotation controls |
| `SpeedForward` | The object's forward speed |
| `Speed` | The object's speed |
| `Pitch`, `Bank`, `Heading` | The object's orientation angles, in radians |
| `ETSShield`, `ETSEngine`, `ETSWeapons` | The ship's ETS energy for that system (ships only) |

The control sources read the player's controls whatever object the animation is on, so they're meant for the player's ship and cockpit. An unknown source logs an error and disables that driver.

---

## Example

```
#Animations

$Name: Lid
$Type: scripted
+Triggered By: openlid
$Rotation:
+Angle: 70,0,0
+Time: 5
+Acceleration: 17,0,0
+Submodel: lid

$Name: Wings
$Type: on-spawn
$Flags: ( "Loop" )
$Segment Parallel: 
	$Rotation:
	+Angle: 0,0,150
	+Time: 1.5
	+Acceleration: 0,0,50
	+Submodel: wingleft
	$Rotation:
	+Angle: 0,0,-150
	+Time: 1.5
	+Acceleration: 0,0,50
	+Submodel: wingright
+End Segment

$Name: HangarDoor3
  $Type: scripted
    +Triggered By: HangarDoor3_Toggle
  $Sound During:
    +Loop: 175
    +Radius: 100
  $Axis Rotation:
    +Axis: -0.5, 0.0, -0.866
    +Angle: -90
    +Time: 5.0
    +Submodel: HangarDoor03

#End

#Moveables

$Name: Canard
$Type: Rotation
+Angle: 0,0,0
+Velocity: 20,20,20
+Submodel: canard

#End
```
