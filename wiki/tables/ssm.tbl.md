> **Revision information**
> OFP Revision: [#145](https://github.com/ofp-fs2open/fs2open.github.com/pull/145)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

The **ssm.tbl** defines subspace missile (SSM) strikes: the weapons that warp in around a target, how many, and in what formation.

This table is one of the Modular Tables and can be extended with **xxx-ssm.tbm**.

Code: [/hud/hudartillery.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/hud/hudartillery.cpp). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in. Fields without a badge are from the original game.

## Contents

1. [Format](#format)
2. [$SSM:](#ssm)

---

## Format

- Table consists of independent **$SSM:** entries
- Table ends with **#end**
- The fields of a strike must be in the order listed here. Every field is optional unless noted.
- Strikes are used by the `call-ssm-strike` SEXP and by weapons with `$SSM:` in [weapons.tbl](weapons.tbl).

## $SSM:

- Names the SSM strike.

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg) The name itself is optional; if not present, it gets defaulted to "SSM <index>", allowing easy selection in FRED.

- A name already used is not checked: the strike is added again, and the first one is found by name. Use `+nocreate` to edit a strike.
- Syntax: **String**, name of the strike (as used by SEXPs and weapons)

### +nocreate

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- Optional, directly after the name. Edits the existing strike of that name instead of adding a new one; only the fields given are changed. If no strike has that name, the entry is ignored.

### +Weapon:

- Required in practice: a strike without a valid weapon is discarded after all tables are read. An unknown weapon logs a warning.
- Defines the weapon used in the strike.
- Syntax: **String**, name of the weapon

### +Count:

- (Optional) Defines number of the weapons fired by the strike.
- Syntax: **Integer**, defaults to 1

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

### +Min Count:

- (Optional) Can be used instead of "+Count:", and in combination with "+Max Count:", to allow for a random number of weapons to be in the strike.
- Syntax: **Integer**

### +Max Count:

- (Required if "+Min Count:" is used) Specifies the maximum number of weapons that can be fired by the strike.
- Syntax: **Integer**

### +WarpEffect:

![FSO 19.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-19.0.svg)

- (Optional) Defines which fireball to use for the warp effect. An index out of range, or an unknown ID, logs a warning and the default is kept. Can be either the fireball index or its [unique ID](fireball.tbl#unique-id).
- Syntax: **Integer** or **String**, defaults to "Warp Effect" (index 1)

### +WarpRadius:

- (Optional) Defines the radius of the warp effect.
- Syntax: **Float**, defaults to 50

### +WarpTime:

- (Optional) Defines the lifetime of the warp effect in seconds. Non-beam weapons fire halfway through this lifetime; beam weapons either fire at the start of the effect or at the appropriate time so that the midpoint of the beam is the same as the midpoint of the warp effect, whichever is later.
- Must be at least 4.0; a smaller value logs a warning and 4.0 is used.
- Syntax: **Float**, defaults to 4.0

### +Radius:

- (Optional) Defines the radius of the formation used by the weapons in the strike.
- Syntax: **Float**, defaults to 500

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

### +Min Radius:

- (Optional) Can be used instead of "+Radius", and in combination with "+Max Radius:", to allow for a random radius to be used by the strike.
- Syntax: **Float**

### +Max Radius:

- (Required if "+Min Radius:" is used) Specifies the maximum radius of the strike formation.
- Syntax: **Float**

### +Offset:

- (Optional) Defines the offset of the formation used by the weapons in the strike; generally works best with circular strikes.
- Syntax: **Float**, defaults to 0

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

### +Min Offset:

- (Optional) Can be used instead of "+Offset", and in combination with "+Max Offset:", to allow for a random offset to be used by the strike.
- Syntax: **Float**

### +Max Offset:

- (Required if "+Min Offset:" is used) Specifies the maximum offset of the strike formation.
- Syntax: **Float**

### +Shape:

![FSO 3.7.4](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.4.svg)

- (Optional) The shape used for the formation of weapons.
- Syntax: **Point**, **Circle**, or **Sphere**; defaults to Circle. Anything else is an error.

### +HUD Message:

- (Optional) Whether or not firing this SSM triggers a HUD message.
- Syntax: **Boolean**, defaults to true

### +Custom Message:

- (Optional) A custom HUD message shown instead of the default one when the SSM is fired.
- Syntax: **String**, the message text
- Note: By default, the message played is the one defined in strings.tbl index 1570, which is set to the generic "Firing Artillery"

### +Alarm Sound:

![FSO 3.7.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.7.2.svg)

- (Optional) Can be used to define a custom alarm sound to be played when the SSM is fired.
- Syntax: **Integer** or **String**, the number or name of a sound in [sounds.tbl](sounds.tbl)
