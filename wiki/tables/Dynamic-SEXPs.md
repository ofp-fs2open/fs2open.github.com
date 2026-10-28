> **Revision information**
> OFP Revision: [#141](https://github.com/ofp-fs2open/fs2open.github.com/pull/141)
> *Set the revision to your PR when your PR changes what this page documents. Leave it alone for edits that only fix or improve the documentation.*

![FSO 3.8](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-3.8.svg)

**Dynamic SEXPs** are custom SEXPs a mod adds, whose effect is written in Lua: new SEXP operators (Lua SEXPs), new AI goals and player orders (Lua AI), and the enum lists their arguments can use.

There is no main table: they are only read from modular tables named **xxx-sexp.tbm**.

Code: [/parse/sexp/sexp_lookup.cpp](https://github.com/ofp-fs2open/fs2open.github.com/blob/master/code/parse/sexp/sexp_lookup.cpp) (with LuaSEXP.cpp and LuaAISEXP.cpp next to it). See [Tables](Tables) for every table.

Badges show the version a field or behavior was added in.

## Contents

1. [Table syntax](#table-syntax)
2. [Lua interface](#lua-interface)

---

## Table syntax

This section will describe the syntax and effects of the individual options.

- The three sections are each optional, but must be in this order: `#Lua Enums`, `#Lua SEXPs`, `#Lua AI`. Each ends with `#End`.
- The fields of each entry must be in the order listed here.

### #Lua Enums

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg) Specifies lists of dynamic Enums that can be referenced in Lua SEXPs. This section must end with `#End`

#### $Name:

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- The name of this enum list. Must be unique; a name that already exists logs a warning and the list is skipped.
- A list with no items logs a warning and is ignored.
- Syntax: ***String***

#### +Enum:

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- The enum item itself.
- No limit to the number of enums listed.
- An item can't contain `( ) " ' \ /`; one that does logs a warning and is skipped. An item already in the list (not case sensitive) is skipped too, and one of 32 characters or more is cut short.
- Syntax: ***String***

### #Lua SEXPs

Specifies a list of dynamic SEXPs implemented using a Lua function (see [Lua interface](#lua-interface) for how to use the implementation function). This section must end with `#End`

#### $Operator:

- Adds a new SEXP operator with the specified name. The name may not contain the following characters: `()\"'`. Furthermore, any whitespace (tab characters or spaces) are also not permitted. This is the name under which this operator will appear in the SEXP code. While the name is mostly up to you it would be a good idea to keep it in line with how the other SEXPs are named. To avoid conflicts with other custom SEXPs you should also use a unique prefix. For example, if you want to  have some SEXP operators that control a cloaking script then you could use `cloak-` as the common prefix for all the operators that are related to this system. A SEXP that disables the cloak system could be named something like `cloak-disable`.
- This name must be unique among all operators that are known to the SEXP system. That includes built-in SEXPs and operators added before. If there is a conflict then the new operator definition will be ignored (with a warning). Built-in operators always take precedence.
- Syntax: ***String***

#### $Category:

- Required. Specifies in which category this SEXP will appear in the context menu in the FRED event editor. ![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg) A name that isn't one of the built-in categories creates a new category. The built-in categories are:
  - Objectives
  - Time
  - Logical
  - Arithmetic
  - Status
  - Change
  - Conditionals
  - Ai goals
  - Event/Goals
  - Training
- Syntax: ***String***

#### $Subcategory:

- Optional. Specifies into which subcategory this SEXP should be put. This is the second level in the context menu of the FRED event editor. New subcategories can be added by specifying a new name here but all subcategory names must be globally unique (you can't have the same subcategory name under two different categories).
- Syntax: ***String***

#### $Minimum Arguments:

- Specifies what minimum number of parameters this SEXP needs to work. If fewer parameters are specified, then the SEXP parser will treat this as an error.
- Must be at least 0.
- Syntax: ***Integer***

#### $Maximum Arguments:

- Specifies the maximum number of parameters this SEXP can process.
- Optional. If this option is not present then the SEXP can process an unlimited number of parameters.
- Must be at least 0 and greater or equal to $Minimum Arguments.
- Syntax: ***Integer***

#### $Return Type:

- Specifies what data type this SEXP will return when invoked. Can be one of the following:
  - **number**: The SEXP will return an integer. The scripting function needs to return a number value.
  - **boolean**: The SEXP will return a boolean. The scripting function must return a boolean value.
  - **nothing**: The SEXP will return nothing. The scripting function may not return anything.
- Optional. Defaults to *nothing*. An unknown type logs a warning and *nothing* is used.
- Syntax: ***String***

#### $Description:

- Sets the help text displayed in the FRED help box. This should be the generic description of what this SEXP does but should not include the documentation for the individual parameters.
- Syntax: ***String***

#### $Repeat

- This option must be combined with the parameter documentation below to specify a repeating pattern in a SEXP with a variable number of parameters. See the example below for how exactly this will work. All parameters specified after this option will be grouped together. The script will receive these groups of parameters as a single entity (see [Lua interface](#lua-interface)) to make it easier to use the values without having to separate them again in the script.

#### $Parameter:

- Adds documentation for the parameters and specifies what type they need to have. This option may appear multiple times and should match the number of parameters specified by `$Minimum Arguments` and `$Maximum Arguments`.
- This option has no value.

#### +Description:

- Sets the description of this parameter. This will be used for the documentation text.
- Syntax: ***String***

#### +Type:

- Sets the type this parameter requires. The currently supported types are:
  - **boolean**: This parameter is a boolean. The SEXP system will evaluate the expression for this parameter (which may be another SEXP operator) and pass the result to the script.
  - **number**: This parameter is a number. The SEXP system will evaluate the expression for this parameter (which may be another SEXP operator) and pass the result to the script.
  - **ship**: This parameter is the name of a ship. The script will receive this value as a `ship` handle. If the name provided by the mission is invalid (if the ship has not arrived yet or if it is not present anymore) then the script will receive an invalid `ship` handle.
  - **shipname**: This parameter is the name of a ship. Unlike the **ship** parameter, the script will receive this value as a `string`. This provides a way to refer to ships which may not be present yet or which may have exited the mission.
  - **string**: This parameter is a generic string with no special format. In Unicode mode this string may contain Unicode characters.
  - **team**: This parameter is a handle to a specific team. The script will receive this value as a `team` handle.
  - **waypointpath**: This parameter is a reference to a specific waypoint path. The script will receive this as a `waypointlist` handle.
  - **variable**: This is a reference to a SEXP variable. This is a reference to the actual variable and not its contents. The script will receive this as a `sexpvariable` handle.
  - **message**: This is a reference to a mission message. The script will receive it as a `message` handle.
  - **wing**: This is a reference to a wing in the mission. The script will receive it as a `wing` handle.
  - **shipclass**: This is a reference to a table ship class. The script will receive it as a `shipclass` handle.
  - **weaponclass**: This is a reference to a table weapon class. The script will receive it as a `weaponclass` handle.
  - **soundentry**: This is a reference to a table defined game sound. The script will receive it as a `soundentry` handle.

![FSO 22.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.0.svg)

- **ship+waypoint**: This can be a reference to either a ship, both present and not yet present, or a waypoint. The script will receive it as an `oswpt` handle.
  - **ship+wing**: This can be a reference to either a ship or a wing, both present and not yet present. The script will receive it as an `oswpt` handle.
  - **ship+wing+team**: This can be a reference to either a ship, a wing or a whole team, both present and not yet present. The script will receive it as an `oswpt` handle.
  - **ship+wing+ship_on_team+waypoint**: This can be a reference to either a ship, a wing, any ship on a team, or a waypoint, both present and not yet present. The script will receive it as an `oswpt` handle.
  - **ship+wing+waypoint**: This can be a reference to either a ship, a wing, or a waypoint, both present and not yet present. The script will receive it as an `oswpt` handle.
  - **ship+wing+waypoint+none**: This can be a reference to either a ship, a wing, a waypoint, both present and not yet present, or none. The script will receive it as an `oswpt` handle.

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- **enum**: A pre-defined enumeration list. The script will receive it as a string.

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- **hudgauge**: This is a reference to any built-in or custom hud gauge. The script will receive it as a `hudgauge` handle.
  - **event**: This is a reference to existing mission events. The script will receive it as an `event` handle.
  - **subsystem**: This is a reference to a ship's subsystems and requires [+Parent Parameter Index](#parent-parameter-index). The script will receive it as a `subsystem` handle.
  - **dockpoint**: This is a reference to a ship's docking points and requires [+Parent Parameter Index](#parent-parameter-index). The script will receive it as a `dockingbay` handle.

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- **waypoint**: This is a reference to a waypoint. The script will receive it as an `oswpt` handle.
  - **child_enum**: A pre-defined enumeration list, but gets the list name from [+Parent Parameter Index](#parent-parameter-index). Can optionally use [+Suffix](#suffix). The script will receive it as a string.

![FSO 24.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.2.svg)

- **custom_string**: One of the mission's custom strings. The script will receive a table with the fields `Name`, `Value` and `String`.

![FSO 25.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-25.0.svg)

- **subsystemname**: Like **subsystem**, but the script receives the subsystem's name as a `string` instead of a handle. Requires [+Parent Parameter Index](#parent-parameter-index).
- **goal**: A reference to a mission goal. The script will receive it as a `goal` handle.

![FSO 26.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-26.2.svg)

- **prop**: The name of a prop. The script will receive it as a `prop` handle.
- **propname**: The name of a prop. The script will receive it as a `string`.
- **propclass**: A prop class. The script will receive it as a `propclass` handle.

The type names are not case sensitive. An unknown type logs a warning.

- Syntax: ***String***

#### +Enum name:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- Required if the +Type is an enum.
- The name of the enum list.

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- Can reference a pre-parsed enum list.

- Syntax: ***String***

#### +Enum:

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

- An item to add to the enum list.
- As many +Enum: entries can be specified as needed.

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- If +Enum Name references a pre-parsed enum list and this field is used, then it will add to that list just for this specific sexp.

- Syntax: ***String***

#### +Parent Parameter Index:

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

- The index of the parent parameter that should be used for types that require it.  For example, **subsystem** parameters need to refer to a parent **ship** parameter so that the SEXP system knows which subsystems are relevant to that ship.  This index counts the parameters starting at 1.
- If [$Repeat](#repeat) is used then the last parsed index is the one that will be repeated.
- An index setting of -1 will always return the immediately previous SEXP argument, even if [$Repeat](#repeat) is active.
- Defaults to -1 if this is not provided for a type that requires it
- Syntax: ***Integer***

#### +Suffix:

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- An arbitrary string to suffix to the enum parent's name.
- For example, if the parent enum is a ship type, you can add the suffix and the enum list that will be used will be SHIPNAME + suffix
- Syntax: ***string***

![FSO 22.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.2.svg)

### #Lua AI

Specifies new SEXPs that can be used as AI-goals in FRED, which execute custom Lua script.

#### $Operator:

- Adds a new SEXP operator with the specified name. The name may not contain the following characters: `()\"'`. Furthermore, any whitespace (tab characters or spaces) are also not permitted. This is the name under which this operator will appear in the SEXP code.
- This name must be unique among all operators that are known to the SEXP system. That includes built-in SEXPs and operators added before. If there is a conflict then the new operator definition will be ignored. Built-in operators always take precedence.
- Syntax: ***String***

#### $Description:

- Sets the help text displayed in the FRED help box. This should be the generic description of what this AI Goal does.
- Syntax: ***String***

#### +HUD String:

- If set, this string will be displayed when targeting a ship executing this AI goal, similar to "attacking" when given a chase goal.
- Optional.
- Syntax: ***String***

#### $Target Parameter:

- If set, this AI mode will take a target as the first parameter of the AI goal SEXP.
- Optional.

##### +Description:

- Required. The description of the target parameter
- Syntax: ***String***

##### +Type:

- Required. The type the target parameter requires. The script will receive all of the following parameter types as an `oswpt` handle. Only the following are permitted:
  - **ship**: This parameter is the name of a ship.
  - **wing**: This is a reference to a wing in the mission.
  - **ship+waypoint**: This can be a reference to either a ship, both present and not yet present, or a waypoint.
  - **ship+wing**: This can be a reference to either a ship or a wing, both present and not yet present.
  - **ship+wing+team**: This can be a reference to either a ship, a wing or a whole team, both present and not yet present.
  - **ship+wing+ship_on_team+waypoint**: This can be a reference to either a ship, a wing, any ship on a team, or a waypoint, both present and not yet present.
  - **ship+wing+waypoint**: This can be a reference to either a ship, a wing, or a waypoint, both present and not yet present.
  - **ship+wing+waypoint+none**: This can be a reference to either a ship, a wing, a waypoint, both present and not yet present, or none.
- Syntax: ***String***

#### $Player Order:

- If set, this AI mode will be available as a player order (in the comms menu). ![FSO 22.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-22.2.svg) There is no limit on how many can be registered. Any player order registered like this also needs to be added as a valid player order in the [objecttypes.tbl](objecttypes.tbl) table. Player orders must have either no target parameter at all, or a target parameter of the ship type.
- Optional.

##### +General Order:

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- If YES, the order is a general order: it is listed in its own category in the comms menu and isn't available as a SEXP in the mission editor.
- Optional.
- Syntax: ***Boolean***

###### +Category:

![FSO 24.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-24.0.svg)

- Optional, after `+General Order:`. The comms menu category the general order is listed in. Default: General.
- Syntax: ***String***

##### +Display String:

- Required. The name of the order shown to the player in the comms menu
- Syntax: ***String***

##### +Parse String:

- The name of the order as it needs to be entered in the [objecttypes.tbl](objecttypes.tbl).
- Optional. Equal to the display string by default.
- Syntax: ***String***

##### +Target Restrictions:

- Requirements a ship must fulfill to be a valid target for this order. This may be fine-tuned by registering a `TargetRestrict` function.
  - **All**: Any ship is a valid target.
  - **Allies**: The target must be of a team that is not attacked by the player.
  - **Own Team**: The target must be of the same team as the player.
  - **Hostiles**: The target must be of a team that is attacked by the player.
  - **Same Wing**: The target and the ship receiving the order must be on the same wing.
  - **Player Wing**: The target and the player must be on the same wing.
  - **Capitals**: The ship must be a capital ship (i.e. any large ship, including a transport or freighter).
  - **Allied Capitals**: The ship must be an ally and a capital ship.
  - **Enemy Capitals**: The ship must be a hostile and a capital ship.
  - **Not Self**: The ship must not be the ship receiving the order.
- Optional. All by default. An unknown value logs a warning and All is used.
- **Known issue:** *Allied Capitals* is read as *All* (the engine matches "All" against the start of the text), and *Not Self* isn't recognized at all; both leave the rest of the line unread, which breaks the parsing of the operator. Use a `TargetRestrict` function in Lua instead.
- Syntax: ***String***

![FSO 23.2](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.2.svg)

##### +Ship Restrictions:

- Requirements on what can receive this order.
  - **Any**: Any ship or wing can receive this order.
  - **On Player Wing**: Only a ship on the player's wing can receive this order.
  - **Wing**: This order may only be sent to wings, not individual ships.
  - **Player Wing**: This order may only be sent to the player's wing.
- Optional. Any by default. An unknown value logs a warning and Any is used.
- Syntax: ***String***

##### +Acknowledge Message:

- Once the player gives the player order, this message will be played. Can be any of the messages listed [here](messages.tbl#name-1).
- Optional, the "Yes Sir!"-message (`yes`) by default.
- Syntax: ***String***

![FSO 23.0](https://raw.githubusercontent.com/wiki/ofp-fs2open/fs2open.github.com/images/badges/fso-23.0.svg)

#### $Minimum Additional Arguments:

- Specifies what minimum number of parameters this SEXP needs to work. If fewer parameters are specified, then the SEXP parser will treat this as an error.
- Optional. 0 by default. Must be at least 0.
- Syntax: ***Integer***

#### $Maximum Additional Arguments:

- Specifies the maximum number of parameters this SEXP can process.
- Optional. If this option is not present then the SEXP can process an unlimited number of parameters.
- Must be at least 0 and greater or equal to $Minimum Additional Arguments.
- Syntax: ***Integer***

#### $Repeat

- This option must be combined with the additional parameter specification below to specify a repeating pattern in a SEXP with a variable number of parameters. See the example below for how exactly this will work. All parameters specified after this option will be grouped together. The script will receive these groups of parameters as a single entity (see [Lua interface](#lua-interface)) to make it easier to use the values without having to separate them again in the script.

#### $Additional Parameter:

- Adds additional parameters beyond the target and specifies what type they need to have. This option may appear multiple times and should match the number of parameters specified by `$Minimum Additional Arguments` and `$Maximum Additional Arguments`. When called from a player order, all Additional Parameters beyond the Target Parameter will be nil
- This option has no value.

#### +Description:

- Sets the description of this parameter. This will be used for the documentation text.
- Syntax: ***String***

#### +Type:

- Sets the type this parameter requires. The currently supported types are:
  - **boolean**: This parameter is a boolean. The SEXP system will evaluate the expression for this parameter (which may be another SEXP operator) and pass the result to the script.
  - **number**: This parameter is a number. The SEXP system will evaluate the expression for this parameter (which may be another SEXP operator) and pass the result to the script.
  - **ship**: This parameter is the name of a ship. The script will receive this value as a `ship` handle. If the name provided by the mission is invalid (if the ship has not arrived yet or if it is not present anymore) then the script will receive an invalid `ship` handle.
  - **shipname**: This parameter is the name of a ship. Unlike the **ship** parameter, the script will receive this value as a `string`. This provides a way to refer to ships which may not be present yet or which may have exited the mission.
  - **string**: This parameter is a generic string with no special format. In Unicode mode this string may contain Unicode characters.
  - **team**: This parameter is a handle to a specific team. The script will receive this value as a `team` handle.
  - **waypointpath**: This parameter is a reference to a specific waypoint path. The script will receive this as a `waypointlist` handle.
  - **variable**: This is a reference to a SEXP variable. This is a reference to the actual variable and not its contents. The script will receive this as a `sexpvariable` handle.
  - **message**: This is a reference to a mission message. The script will receive it as a `message` handle.
  - **wing**: This is a reference to a wing in the mission. The script will receive it as a `wing` handle.
  - **shipclass**: This is a reference to a table ship class. The script will receive it as a `shipclass` handle.
  - **weaponclass**: This is a reference to a table weapon class. The script will receive it as a `weaponclass` handle.
  - **soundentry**: This is a reference to a table defined game sound. The script will receive it as a `soundentry` handle.
  - **ship+waypoint**: This can be a reference to either a ship, both present and not yet present, or a waypoint. The script will receive it as an `oswpt` handle.
  - **ship+wing**: This can be a reference to either a ship or a wing, both present and not yet present. The script will receive it as an `oswpt` handle.
  - **ship+wing+team**: This can be a reference to either a ship, a wing or a whole team, both present and not yet present. The script will receive it as an `oswpt` handle.
  - **ship+wing+ship_on_team+waypoint**: This can be a reference to either a ship, a wing, any ship on a team, or a waypoint, both present and not yet present. The script will receive it as an `oswpt` handle.
  - **ship+wing+waypoint**: This can be a reference to either a ship, a wing, or a waypoint, both present and not yet present. The script will receive it as an `oswpt` handle.
  - **ship+wing+waypoint+none**: This can be a reference to either a ship, a wing, a waypoint, both present and not yet present, or none. The script will receive it as an `oswpt` handle.
- Syntax: ***String***

### Example

```
#Lua SEXPs

$Operator: dynamic-sexp
$Category: Change
$Subcategory: Scripted
$Minimum Arguments: 3
; No maximum arguments means that it accepts a variable number of arguments
$Return Type: Nothing
$Description: This is a dynamic parameter test SEXP
$Parameter:
	+Description: Number parameter
	+Type: Number
$Parameter:
	+Description: Ship parameter
	+Type: Ship
$Parameter:
	+Description: Variable name parameter
	+Type: variable
$Repeat ; This starts the part of the parameter list where the parameter pattern repeats
$Parameter:
	+Description: Variable string parameter
	+Type: string
$Parameter:
	+Description: Variable number parameter
	+Type: Number

#End
```

## Lua interface

### Lua SEXPs

So far, all the SEXP table has done is specify that a new Lua SEXP exists but there is no actual action which will be executed when the SEXP is called. For the Lua language this can be done using the `mn.LuaSEXPs` variable. This is a variable which can be indexed using the SEXP name. The returned value is of type `LuaSEXP` which only has a single field, `Action`. This is the function that will be executed once the SEXP is called in a mission. You can use it to retrieve the function of a LuaSEXP but the more common use case is that it is written in the `$On Game Init` hook to set the action function.

The action function will receive the values as normal parameters. If the SEXP will receive a variable number of parameters then you can use `...` as the last parameter to also capture these values. If a parameter is optional and not specified by the SEXP invocation then it will be `nil` in the script. Parameters in the repeat pattern of the parameter list will be grouped into a array of arrays where each array instance contains the individual values. See below for an example of how this can be handled in the Lua code.

```
#Conditional Hooks

$Application: FS2_Open
$On Game Init: [
mn.LuaSEXPs["dynamic-sexp"].Action = function (arg1, arg2, arg3, ...)
	ba.print(tostring(arg1))
	ba.print(arg2.Name)
	ba.print(tostring(arg3.Value))

	for _, val in ipairs(arg) do
		ba.print(val[1])
		ba.print(tostring(val[2]))
	end
end
]

#End
```

### Lua AI

LuaAI operators are more complicated than standard Lua operators. They are defined by two main functions and two optional functions.

The `ActionEnter` and `ActionFrame` functions define the operator's behavior. `ActionEnter` is called only once, when a ship receives the order; it may be used for any needed setup. `ActionFrame` is called once per frame. Both functions receive an `ai_helper` object, the target (if any), and any additional arguments; they should return `true` if the order is complete, and `false` otherwise.

The `Achievability` function defines whether the order is currently achievable. It accepts the ship executing the order, the target (if any), and any additional arguments. It may return `LUAAI_ACHIEVABLE` if the order can currently be achieved, `LUAAI_UNACHIEVABLE` if the order cannot be achieved and should be removed from the ship, and `LUAAI_NOT_YET_ACHIEVABLE` if the order cannot currently be achieved, but may become possible later; this allows an order which doesn't make sense right now to be suspended until later.

Lastly, the `TargetRestrict` function may be used to fine-tune the order's targeting. It accepts a ship and a nilable OSWPT handle representing the target, and returns whether or not the target is valid. This is checked only if the target already satisfies the `+Target Restrictions:`.

Example:

```
#Conditional Hooks

$Application: FS2_Open
$On Game Init: [
mn.LuaAISEXPs["ai-ram"].ActionEnter = function(ai_helper, target)
	--Print info to the log
	ba.print("Targeting " .. target:get().Name .. " from " .. ai_helper.Ship.Name .. "!")
	return false
end

mn.LuaAISEXPs["ai-ram"].ActionFrame = function(ai_helper, target)
	--Turn towards and fly into the target
	ai_helper:turnTowardsPoint(target:get().Position)
	helper.ForwardThrust = 1
	return false
end

mn.LuaAISEXPs["ai-ram"].Achievability = function(ship, target)
	if ship.Disabled then
		-- Contrived example
		return LUAAI_NOT_YET_ACHIEVABLE
	elseif target:isValid() then
		return LUAAI_ACHIEVABLE
	else
		return LUAAI_UNACHIEVABLE
	end
end
]

#End
```
