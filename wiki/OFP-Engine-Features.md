# Engine Features

Features that change what a mission or mod can do in the game. Many of them also add editor support, but they are listed here because the game uses them. See also [QtFRED Features](OFP-QtFRED-Features) and [Bug Fixes](OFP-Bug-Fixes).

| Feature | What it does | PR |
|---|---|---|
| **Models for intel entries** | Lets intel (tech-room) entries display 3D models. | [#3](https://github.com/ofp-fs2open/fs2open.github.com/pull/3) |
| **MP4 & OGG support for anims** | Video files are now a supported animation type in the UI | [#13](https://github.com/ofp-fs2open/fs2open.github.com/pull/13) |
| **APNGs support looping keyframes** | APNGs built with AnimStudio can have and fully support looping keyframes | [#17](https://github.com/ofp-fs2open/fs2open.github.com/pull/17) |
| **Coordinate Points Object Type** | New Coordinate Points objects are 2D rendered shapes that can be targeted by players | [#16](https://github.com/ofp-fs2open/fs2open.github.com/pull/16) |
| **Props Feature Upgrades** | Props fully support texture replacement and spawn/despawn sexps | [#15](https://github.com/ofp-fs2open/fs2open.github.com/pull/15) |
| **Ship Instance Custom Data** | Individual ships carry named custom data values (e.g. `Bounty = 75`), set in qtFRED and read or changed during the mission by [SEXPs](OFP-SEXPs#ship-custom-data-operators) and [Lua](OFP-Lua-Scripting#ship-custom-data) | [#36](https://github.com/ofp-fs2open/fs2open.github.com/pull/36) |
| **Ship Nameplates** | Each ship can carry its own name or hull marking on its hull, either as text the game draws (font, scale, letter spacing, color, offset and resolution) or as a texture file, set in QtFRED's Nameplate dialog with a live preview. Models opt in with a texture named `nameplate`; the `$nameplate_width` / `$nameplate_height` submodel properties set its default size (16 to 1024). Missions using it require 26.1; FSO Compatibility Mode saves texture nameplates as a plain texture replacement older builds read | [#64](https://github.com/ofp-fs2open/fs2open.github.com/pull/64) |
| **Generated Background Nebula** | FS1's background nebula is back as a modern generated backdrop: a soft cloud sphere behind the stars, picked per mission with the original +Nebula: / +Color: fields and turned on with a new +Generated Nebula: field (so older missions that only carry the FS1 fields are unchanged). Patterns and colors come from [generated_nebula.tbl](generated_nebula.tbl): procedural noise, placed clouds, or both; the built-in Nebula01-03 are laid out like the FS1 originals and the nine colors are FS1's exact palette colors. qtFRED's Background Editor shows it live. | [#77](https://github.com/ofp-fs2open/fs2open.github.com/pull/77) |
