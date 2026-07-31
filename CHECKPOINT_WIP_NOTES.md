# In-Engine Mission Checkpoint System for FreeSpace Open

## Context

Mods currently get mid-mission checkpoints only through the `shipsaveload` Lua script
(`shipsaveloadv2sct.tbm` + `shipsaveloadv2sexp.tbm`). It marshals a hand-picked subset of
ship state through the Lua API into a JSON blob under `data/players/<hash>_checkpoint_data.cfg`
and re-applies it by splicing strings into `mn.evaluateSEXP("(change-ship-class ...)")`.
It works, but it is inherently approximate:

- Only what the Lua API happens to expose can be saved. AI goals/modes/targets, docking,
  wing wave state, `Mission_events`/`Mission_goals`, the mission log, arrival/departure
  queues, and every engine timestamp are invisible to it.
- Mission time is faked via `The_mission.HUD_timer_padding` (`code/mission/missionparse.h:221`).
  The HUD clock reads right, but `Missiontime` does not — so `has-time-elapsed` and every
  event timestamp are wrong after a load.
- The designer must enumerate each ship and variable by hand, and the SEXP is documented as
  handling "only about 50 ships at a time".
- Data is untyped positional JSON arrays keyed by a hash of the mod title, so any layout
  change silently misreads old saves.

**Goal:** an in-engine replacement that captures the mission's actual state, restores it
exactly, and survives both engine updates and mod/table changes.

**Confirmed scope decisions:**
- Full-fidelity snapshot is the target; "all durable state, skipping transient in-flight
  projectiles/debris/particles" is an acceptable first milestone.
- Restore = **mission reload + bash**, not hot in-place restore.
- Checkpoints live **on disk, per pilot + per campaign + per mission**, surviving a quit.
- Load must support **overriding player/wingman ship class and loadout**.
- Player score, per-ship-class kill counts, and mission time must round-trip.
- Out of scope: multiplayer, automatic checkpoint-on-death.

---

## Key existing infrastructure to reuse (do not reinvent)

| Need | Existing facility |
|---|---|
| Versioned, sectioned save file | `pilot::FileHandler` (`code/pilotfile/FileHandler.h`) with `BinaryFileHandler` / `JSONFileHandler` backends; jansson already linked (`code/CMakeLists.txt:75`) |
| Modern usage example | `code/pilotfile/plr.cpp` — `handler->startSectionWrite()`, named fields, `while (handler->hasMoreSections())` + `default:` skip (`plr.cpp:1156-1200`) |
| Mod-change resilience pattern | `csg_read_info()`/`csg_write_info()` (`code/pilotfile/csg.cpp:65-263`) — write full ship/weapon **name lists**, resolve to current indices on load, `-1` if the mod dropped one; `index_list_t` at `pilotfile.h:45-49` |
| Snapshot-then-reload prior art | Red alert: `red_alert_store_ship_status()` (`code/missionui/redalert.cpp:734`), `red_alert_bash_ship_status()` (`:881`), the sequence at `:1215-1250`, and the p_object variants at `:552, 657, 1043-1137` |
| Restore hook point | `game_post_level_init()` — `freespace2/freespace.cpp:1482-1484`, exactly where red alert bashes. After `mission_load()` and object creation, before `freespace_mission_load_stuff()` and `OnMissionStart` |
| Name-keyed ship lookup incl. not-yet-arrived and exited | `Ship_registry` / `Ship_registry_map` (`code/ship/ship.h:1016-1049`), `ShipStatus{NOT_YET_PRESENT, PRESENT, DEATH_ROLL, EXITED}` |
| **Global clock rebase** | `timestamp_adjust_microseconds(delta, TIMER_DIRECTION)` (`code/io/timer.cpp:631-649`) and `timestamp_offset_mission_time(float)` (`:708`) |
| Reproducing an animation at an arbitrary point in its timeline | `ModelAnimation::start(..., multiOverrideTime)` (`code/model/animation/modelanimation.cpp:264`); the multiplayer sync at `code/network/multimsgs.cpp:8055-8100` is a working end-to-end example |
| Loadout representation + application | `wss_unit` / `Wss_slots` / `Player_loadout` (`code/missionui/missionscreencommon.h:178-214`); `wl_bash_ship_weapons()` (`code/missionui/missionweaponchoice.cpp:3553`), `wl_update_parse_object_weapons()` (`:3414`), `create_wings()` (`missionshipchoice.cpp:2429`) |
| Mission fingerprint | `The_mission.modified` (`missionparse.h:198`) + `Current_file_checksum` (`missionparse.cpp:125`); precedent in `wss_maybe_restore_loadout()` (`missionscreencommon.cpp:1136`) |
| Blocking mid-gameplay choice | `popup()` with `PF_RUN_STATE` (`code/popup/popup.h`); `popupdead` (`code/popup/popupdead.cpp:131, 453, 476`) is the model for posting `GS_EVENT_START_GAME` from a popup |

### The single most important finding: time

`Missiontime` is *derived*, not tracked — `game_update_missiontime()` (`freespace.cpp:4657`)
is just `Missiontime = timestamp_get_mission_time()`, which is
`timestamp_get_microseconds() - Timestamp_microseconds_at_mission_start`.

Meanwhile the engine stores **absolute** timestamps in hundreds of places: `ship_weapon`
fire stamps (`ship.h:115-164`), `ship_subsys` turret stamps (`ship.h:369-424`), ~19 in
`ai_info` (`code/ai/ai.h:252-461`), `wing::wave_delay_timestamp` (`ship.h:1700`),
`mission_event::{timestamp, satisfied_time, born_on_date}` (`missiongoals.h:100-127`).

Converting all of those to deltas is infeasible and would break on every engine update.
Instead: **save raw stamps and rebase the global clock on restore.** `timestamp_adjust_*`
already shifts `Timestamp_offset_from_counter`/`Timestamp_paused_at_counter` so the entire
timestamp space jumps and every stored stamp stays self-consistent. It's exactly what the
pre-player-entry skip does today (`freespace.cpp:4643`). This makes
`is-event-true-delay` (`sexp.cpp:19081`) and every AI/turret timer correct for free, and it
means **new timestamp fields added by future engine versions cost nothing** — they just work.

---

## Design

### 1. Module layout

New files:
- `code/mission/missioncheckpoint.h` / `.cpp` — public API, snapshot/apply orchestration,
  the pending-load state machine.
- `code/mission/checkpointfile.h` / `.cpp` — file IO on top of `pilot::FileHandler`,
  version constants, index tables, section readers/writers.
- `code/mission/checkpointfields.h` — the field registry macros (see §3).

Touched files:
- `freespace2/freespace.cpp` — restore call in `game_post_level_init()` (~`:1483`);
  end-of-frame pending-reload check in the game-play do-frame.
- `code/parse/sexp.h` / `sexp.cpp` — new operators (§6).
- `code/pilotfile/FileHandler.h` — add optional-read primitives (§2).
- `code/pilotfile/JSONFileHandler.cpp` / `BinaryFileHandler.cpp` — implement them.
- `code/scripting/api/libs/mission.cpp` — `mn.` bindings (§7).
- `code/source_groups.cmake` — register new files.
- `qtfred` sexp tree registration (find the equivalent of the old `fred2/sexp_tree.cpp`
  category tables in `qtfred/` — FRED2 is not in this tree).

### 2. File format

**Separate file, not a new `.csg` section.** Red-alert data alone already caused "huge pilot
file" complaints (see the size asserts at `redalert.cpp:772, 808, 834`); a full mission
snapshot is an order of magnitude bigger, and stuffing it into the campaign save risks
corrupting campaign progress on a bad write. One file per pilot+campaign+mission:

```
data/players/checkpoints/<callsign>.<campaign>.<mission>.chk
```
opened with `cfopen(name, "rb"/"wb", CF_TYPE_PLAYERS, false,
CF_LOCATION_ROOT_USER | CF_LOCATION_ROOT_GAME | CF_LOCATION_TYPE_ROOT)` — same call shape as
`csg.cpp:1705`. Register the `.chk` extension in the `CF_TYPE_PLAYERS` row of the path table
(`code/cfile/cfile.cpp:73-78`). Multiple named slots live inside one file so a mission can
have several checkpoints without a file explosion.

**Backend: `JSONFileHandler`.** Binary is smaller, but `BinaryFileHandler` is *positional* —
it cannot skip a field it doesn't recognise, which is precisely the resilience property we
need. JSON is self-describing, every field already carries a `const char* name`, it is
diffable and hand-inspectable when a mod reports a bad restore, and `json_dump_cfile` is
already wired up (`JSONFileHandler.cpp:184`). Use `JSON_COMPACT` rather than `JSON_INDENT(4)`
for checkpoints.

**Required `FileHandler` extension.** Today `JSONFileHandler::ensureExists()` calls
`Error()` on a missing key (`JSONFileHandler.cpp:207-213`) — fatal, not tolerant. Add:

```cpp
virtual bool hasField(const char* name) = 0;
virtual std::int32_t readIntOr(const char* name, std::int32_t def);
virtual float        readFloatOr(const char* name, float def);
virtual SCP_string   readStringOr(const char* name, const SCP_string& def);
```
Default implementations sit in `FileHandler` as `hasField(name) ? readX(name) : def`.
`JSONFileHandler::hasField` is a `json_object_get` null check; `BinaryFileHandler::hasField`
returns `false` (so the binary backend degrades to "old file, use defaults" and is not used
for checkpoints).

**Field registry.** Adding a saved field must be a one-line change. Define each captured
struct's fields in an X-macro table:

```cpp
// checkpointfields.h
#define CKPT_SHIP_FIELDS(F)                                        \
    F(float, hull,          objp->hull_strength,        0.0f)      \
    F(float, ab_fuel,       shipp->afterburner_fuel,    0.0f)      \
    F(float, weapon_energy, shipp->weapon_energy,       0.0f)      \
    F(int,   cmeasure_count,shipp->cmeasure_count,      0)         \
    /* ... */
```
One expansion generates the writer (`h->writeFloat("hull", ...)`), one generates the reader
(`... = h->readFloatOr("hull", default)`). Consequences that fall out for free:
- A field added in build N+1 is simply absent from an old checkpoint → default is used.
- A field removed in build N+1 is ignored in an old checkpoint → JSON key skipped.
- No version gate needed for field-level changes at all; the version constant is reserved
  for *structural* changes (new sections, changed semantics of an existing key).

`CHECKPOINT_VERSION` bump policy, mirroring `pilotfile.h:20-22`: bump only when the meaning
of an existing key changes or a section's layout changes — never for adding or removing
fields.

**Never serialize a runtime index.** Every cross-reference is written by *name*:
- ship class, weapon class, IFF/team, armor type, subsystem, wing, waypoint list, persona,
  warp params, ship object references (AI target, guard, dock partner, support ship).
- The Info section still writes the full ship-class/weapon-class name lists (the
  `csg_read_info()` pattern) so the file is self-describing even if a mod is uninstalled.
- Object references are stored as ship *names* and re-resolved through `Ship_registry_map`
  on load — `obj_signature` is regenerated per load (`object.cpp:462, 643`) and `objnum` is
  meaningless across a reload.
- Enum values that come from tables (subsystem types, AI modes, goal modes) get a
  name↔value mapping table; a value that no longer exists resolves to a safe default and
  logs a warning rather than erroring.

**Mission fingerprint.** Store `The_mission.modified`, `Current_file_checksum`, the mission
filename, the campaign name, and the pilot callsign. On load, mismatched checksum →
`checkpoint-exists` returns false and a warning is logged, following the precedent of
`wss_maybe_restore_loadout()` (`missionscreencommon.cpp:1136`). Optionally allow a "lenient"
flag that applies what it can.

Every section is read inside `try { ... } catch` with a `default:` skip case, exactly as
`plr.cpp:1156-1200` does, so an unknown section never aborts the load.

### 3. What to capture, in tiers

**Tier 1 — milestone 1, shippable on its own.**

| Data | Source |
|---|---|
| Header: version, mission fingerprint, pilot, campaign, slot name, real-world save time | `The_mission`, `Player` |
| Index tables: ship classes, weapon classes, IFFs | `Ship_info`, `Weapon_info`, `Iff_info` |
| Clock: `Missiontime`, `The_mission.HUD_timer_padding`, `Game_time_compression` | `systemvars.h:74`, `missionparse.h:221` |
| Per-ship (status `PRESENT`/`DEATH_ROLL`): class, team, `ship_name`, display name, hull + `ship_max_hull_strength`, shield quadrants + `ship_max_shield_strength`, `subsys_list` (by subsystem name: `current_hits`, `max_hits`, flags, rotation angle, turret target), `ship_weapon` banks (class name, `ammo`, `ammo_max`, linked/dual-fire, fire stamps), `cmeasure_count`/`current_cmeasure`, `afterburner_fuel`, `weapon_energy`, ETS indices, `flags`, `cargo1`/`cargo_title`, `orders_accepted`, `wingnum`, `arrival_*`/`departure_*`, `persona_index`, `alt_type_index`, `callsign_index`, `escort_priority`, `hotkey`, `score` | `class ship` (`ship.h:613-947`) |
| Per-ship object: `pos`, `orient`, `phys_info` (velocity, rotvel, desired_*, forward_thrust, etc.), `radius`, `flags` | `code/object/object.h`, `code/physics/physics.h` |
| Ship registry statuses + `Ships_exited` | `ship.h:971-1049` |
| Wings: `current_wave`, `total_arrived_count`, `current_count`, `ship_index[]` (as names), `total_destroyed/departed/vanished`, `time_gone`, `flags`, `wave_delay_timestamp`, `red_alert_skipped_ships` | `wing` (`ship.h:1657-1723`) |
| SEXP variables + containers | `Sexp_variables[]`, `Block_variables[]`, `get_all_sexp_containers()` (`sexp_container.h:198`) |
| `Mission_goals`, `Mission_events` (result, counts, flags, the three TIMESTAMPs, `previous_result`, log buffers), `Mission_goal_timestamp` | `missiongoals.h:53-130` |
| `Log_entries` | `missionlog.h:57-72` |
| Player scoring: `m_score`, `m_kill_count`, `m_kill_count_ok`, `m_assists`, `m_bonehead_kills`, `m_okKills[]` (by ship-class **name**), shots fired/hit, friendly hits | `scoring_struct` (`code/stats/scoring.h:120-137`) |
| Not-yet-arrived parse objects: which are still on `Ship_arrival_list`, plus any bashed `initial_hull`/`Subsys_status` overrides | `Parse_objects`, `Ship_arrival_list` (`missionparse.h:550-554`) |

**Tier 2 — milestone 2.**
`ai_info` (`ai.h:246-461`): `mode`, `previous_mode`, `submode`, `submode_start_time`,
`target_objnum`→name, `goal_objnum`→name, `guard_objnum`/`guard_wingnum`→name,
`goals[MAX_AI_GOALS]` (with ship/wing references by name), `ai_flags`, `ai_class`,
`support_ship_objnum`, and the ~19 raw timestamps. Docking chains from the `object` dock
lists (re-established with `ai_do_objects_docked_stuff()`, `ai.h:610`).
Model animation state and moveables (see below) — restore these *after* docking, since
`Docking_Stage*`/`Docked` animations are keyed to dock state.
`Sexp_nodes[].value` / `.flags` (`sexp.h:1397-1424`) — needed so `when`/`every-time` and
`SEXP_KNOWN_TRUE` short-circuits don't re-fire. `Player_loadout`/`Wss_slots`.
Support-ship state, cargo-known, `mission_hotkey` assignments, HUD escort list,
event music state, autopilot state, asteroid field, `Player` flags/`ci`, camera state.

**Model animation state — Tier 2, and cheaper than it looks.**

This is not a cosmetic detail. `ModelAnimationTriggerType::Scripted` (`modelanimation.h:48`)
is how designers drive the big set-piece animations — portals opening, cannons unfolding,
hangar doors, station arms — and a checkpoint that drops them restores a mission into a
visibly wrong world. `DockBayDoor`, `Docking_Stage1-3`/`Docked`, and `Afterburner` matter
too, because a mid-dock or mid-launch restore with the doors snapped shut is worse than
wrong, it's stuck.

The engine already solves this problem for multiplayer, and the checkpoint can reuse the
solution verbatim. `process_animation_triggered_packet()` (`code/network/multimsgs.cpp:8055-8100`)
reproduces an animation on a remote client from exactly five values:

```
animationId (uint) | target object | direction | {forced, instant, pause} | time offset (ms)
```
and applies it with `animation->second->start(pmi, direction, forced, instant, pause, &delay)`
(`multimsgs.cpp:8091`). `ModelAnimation::start()` takes that `multiOverrideTime` parameter
specifically to place an animation at an arbitrary point in its timeline
(`modelanimation.cpp:264, 296`).

Crucially, `animationId` is **already a content hash, not a runtime index**:

```cpp
// modelanimation.cpp:1460
unsigned int ModelAnimationParseHelper::getUniqueAnimationID(
        const SCP_string& animName, char uniquePrefix, const SCP_string& parentName) {
    return hash_fnv1a(animName + uniquePrefix) ^ hash_fnv1a(parentName);
}
```
It is derived from the animation name and the ship class name, so it is stable across runs,
across table reordering, and across engine versions — it satisfies resilience rule #2 as-is.
Store it alongside the animation's trigger type + name as a human-readable fallback key.

Per ship we capture, for each entry in that ship's `polymodel_instance` running-animation
list, the full `ModelAnimation::instance_data` (`modelanimation.h:212-219`):
`state` (UNTRIGGERED/RUNNING/PAUSED/NEED_RECALC), `canonicalDirection`, `time`, `duration`,
`instance_flags`, `speed`. Restore is one `start()` call per animation with
`force = true` and the saved time as `multiOverrideTime`, then `setSpeed()`/`setFlag()`.

Two small engine additions are needed, both contained:
- `ModelAnimationSet::s_runningAnimations` (`modelanimation.h:302`) and
  `ModelAnimation::m_instances` (`:223`) are private. Add
  `static SCP_vector<std::pair<unsigned int, ModelAnimation::instance_data>>
  getAnimationStates(int pmi_id)` and a matching `applyAnimationState(pmi, id, instance_data)`
  on `ModelAnimationSet`. `getTime(pmi_id)` (`:268`) already exists as precedent for reaching
  into instance data.
- **Moveables** (`ModelAnimationMoveable`, `modelanimation.h:276-291`) are the one genuinely
  awkward case: they store their *configuration* (`m_velocity`, `m_defaultOffset` in
  `modelanimation_moveables.h`) but their current *target* lives in a segment built on the
  fly by `update()`, and the `SCP_vector<std::any>` args are not retained. Fix by caching the
  numeric args per (pmi, moveable name) at the `updateMoveable()` call site — restore then
  replays `updateMoveable()`, or calls the existing `advanceMoveableToFinal()`
  (`modelanimation.h:373`) when the moveable had already settled.

**Tier 3 — full fidelity, later.**
Realistic and worth doing eventually: weapons in flight (needs homing-target re-resolution by
name, parent object, swarm/corkscrew/turret linkage), beams (`beam_info` + parent turret),
fireballs, warp effects in progress.
Genuinely not worth it: particles, decals, trails, ship sparks, sound handles, RNG stream
state. These are frame-scale visual noise with no gameplay meaning, they churn heavily
between engine versions, and they'd dominate file size. Milestone 1 resets them so the
restored frame starts visually clean.

Hidden dependencies to respect throughout: `obj_signature` and net signatures are
regenerated per load; `model_instance_num` and `cockpit_model_instance` are runtime handles
that must be re-derived from the ship class, never restored (animation state is keyed to the
*new* `polymodel_instance`, looked up via `object_get_model_instance_num()`, exactly as
`multimsgs.cpp:8089` does); `subsys_list_indexer` must be rebuilt, not written to;
`ship::warpin_effect`/`warpout_effect` are `unique_ptr`s.

**Subsystem identity: fix red alert's positional bug.** `red_alert_store_subsys_status()`
(`redalert.cpp:703-726`) stores subsystems by *list position*, which breaks the moment a
model's subsystem list changes or the ship class differs. Store by subsystem name
(`ss->system_info->subobj_name`), matched case-insensitively, with a positional fallback
only when names are ambiguous (duplicate engine subsystems get a name + ordinal key).

### 4. Restore mechanics

Store: `mission_checkpoint_store(slot_name, flags)` runs synchronously inside SEXP eval —
it only reads state, so it is safe there. It builds a `checkpoint_data` in memory and writes
the file immediately.

Load: the reload must **not** happen while SEXP evaluation is on the stack.

1. SEXP sets `Checkpoint_pending_load = { slot, flags }` (a global that `game_level_init()`
   does not clear) and returns.
2. At end of frame in the gameplay do-frame, if a load is pending, post
   `GS_EVENT_START_GAME_QUICK` (`gamesequence.h:28` — loads the mission and goes straight to
   the game state, skipping briefing/loadout). Use `GS_EVENT_START_GAME` instead when the
   caller asked to re-open the loadout screen (§5).
3. `game_start_mission()` → `game_level_init()` (resets everything, zeroes `Missiontime`,
   calls `timestamp_start_mission()`) → `mission_load()` → objects created from
   `Parse_objects`.
4. In `game_post_level_init()`, immediately after the red-alert bash at `freespace.cpp:1483`:
   `mission_checkpoint_apply()`.
   Order inside it:
   a. **Rebase the clock first.** `timestamp_adjust_microseconds(saved_missiontime_us,
      TIMER_DIRECTION::FORWARD)` so the whole stamp space matches the checkpoint, then
      `timestamp_offset_mission_time()` to pin `Missiontime` exactly. Guard the
      `Assertion` at `timer.cpp:634` (both offsets must be non-zero — they are, post
      `game_time_level_init()`). Note `UI_TIMESTAMP`s live on a separate clock and are
      deliberately *not* rebased.
   b. Reconcile ship existence: for each registry entry, compare fresh-load status against
      checkpoint status. Ship present at checkpoint but not yet arrived in the fresh load →
      force-create it from its parse object (the `parse_create_object()` path red alert uses
      for wave skipping). Ship absent at checkpoint but present now → remove it via the
      `red_alert_delete_ship()` pattern (`redalert.cpp:839, 863`) using
      `SHIP_DESTROYED_REDALERT`/`SHIP_DEPARTED_REDALERT` cleanup modes. Ship exited at
      checkpoint → replay into `Ships_exited` and mark the registry entry `EXITED`.
   c. Bash class/team first (`change_ship_type()`, `ship_change_iff` equivalents), because
      that reallocates the subsystem list and weapon banks.
   d. Bash per-ship scalar state, subsystems by name, weapon banks, object position/orient/
      physics.
   e. Rebuild wings, then docking chains, then AI (AI last — it references ships and
      docking).
   f. Restore mission goals/events/log/SEXP variables/containers/`Sexp_nodes` state.
   g. Restore scoring into `Player->stats`.
   h. Re-point `Player_obj`, `Player_ship`, `Player_ai` — if the player's ship changed class
      or was recreated, re-run the same fixups `change_ship_type()` does for the player.
   i. Apply loadout overrides (§5).
5. `freespace_mission_load_stuff()` and the `OnMissionStart` hook then run normally, and the
   first `mission_eval_goals()` (`missiongoals.cpp:1094`) sees restored event state, so
   nothing re-fires.

Because `Mission_goal_timestamp` and `mission_event::timestamp` are restored *and* the clock
was rebased in step (a), `is-event-true-delay` and chained events behave correctly.

### 5. Loadout / ship-class override

`load-checkpoint` takes a flags argument (bitmask or a repeating string-flag list, matching
how newer SEXPs take flag lists):

- `Restore player loadout` (default on) — bash the player's ship class and banks from the
  checkpoint.
- `Keep current player loadout` — leave the freshly-created player ship's class and weapons
  as the mission/`Player_loadout` produced them; still restore hull/shields/ammo counts
  scaled to the current banks, or reset them to full (designer's choice via a sub-flag).
- `Keep current wing loadout` — same for ships flagged `From_player_wing`.
- `Re-open loadout screen` — post `GS_EVENT_START_GAME` instead of `..._QUICK` so the player
  passes through briefing/ship-select/weapon-select; `create_wings()`
  (`missionshipchoice.cpp:2429`) applies the new `Wss_slots` at commit, and
  `mission_checkpoint_apply()` then skips the player-wing ships entirely.

Implementation reuses `wl_bash_ship_weapons()` (`missionweaponchoice.cpp:3553`) and
`wl_update_parse_object_weapons()` (`:3414`) rather than writing new bank-application code.
The checkpoint also stores `Player_loadout` and `Wss_slots` so a plain restore reproduces the
exact fit the player committed to.

### 6. SEXPs and FRED

New subcategory `CHANGE_SUBCATEGORY_CHECKPOINTS` in `code/parse/sexp.h:234-254`.

| Operator | Args | Returns | Notes |
|---|---|---|---|
| `store-checkpoint` | 0–1: slot name (default `"auto"`) | nothing | Snapshots and writes immediately |
| `load-checkpoint` | 0–N: slot name, then zero or more flag strings | nothing | Defers the reload to end of frame |
| `prompt-user-checkpoint-load` | 0–N: slot name, prompt text, flags | boolean | Blocking `popup()` with `PF_RUN_STATE`; returns true if the player chose to load. Sets the pending-load and returns true, so the mission can branch before the frame ends |
| `checkpoint-exists` | 0–1: slot name | boolean | Mirrors `lua-save-file-exists` |
| `delete-checkpoint` | 0–1: slot name | nothing | Mirrors `lua-delete-save-file` |

Touch points per operator (function names verified by locating the enclosing function for
each `OP_CHANGE_SHIP_CLASS` occurrence):

- operator enum in `code/parse/sexp.h` (the `OP_*` list, ~`:626` region)
- `Operators[]` table entry — `sexp.cpp:610` is the template
- `eval_sexp()` — `sexp.cpp:28330`, the actual eval case
- `query_operator_return_type()` — `sexp.cpp:31574`
- `query_operator_argument_type()` — `sexp.cpp:32302`
- `get_category()` — `sexp.cpp:36850`
- `get_subcategory()` — `sexp.cpp:37577`
- FRED help text table — `sexp.cpp:41762` region
- qtfred sexp-tree category registration (`qtfred/`; FRED2/MFC is not in this tree)

`get_sexp()` (`sexp.cpp:4617`) and `multi_sexp_eval()` (`sexp.cpp:30963`) need no changes —
the former only handles operators with special parse fixups, and multiplayer is out of scope
(the operators should simply be a no-op with a log warning under `Game_mode & GM_MULTIPLAYER`).

`prompt-user-checkpoint-load` runs the popup synchronously — `popup()` with `PF_RUN_STATE`
already runs the underlying gameplay state beneath the modal, which is how `popupdead`
works. The reload itself is still deferred to end-of-frame, so we never re-enter
`game_level_init()` from inside SEXP eval.

### 7. Scripting API

Add to `code/scripting/api/libs/mission.cpp` (the `mn.` library, `ADE_LIB` at `:180`):
`mn.storeCheckpoint(slot)`, `mn.loadCheckpoint(slot, flags)`, `mn.checkpointExists(slot)`,
`mn.deleteCheckpoint(slot)`, `mn.getCheckpointInfo(slot)` (returns mission time, real-world
save time, mission fingerprint validity). This gives `shipsaveload` users a direct migration
path and lets SCPUI build a checkpoint-picker screen.

### 8. Resilience rules (the ones that actually matter)

1. Named fields with defaults everywhere — never positional. Enforced by the X-macro
   registry, so a contributor adding a field cannot accidentally break compatibility.
2. Never write a runtime index. Ship classes, weapons, IFFs, subsystems, wings, waypoints,
   personas, and all object references go by name.
3. Never write a raw enum whose values come from a table. Map to names.
4. Missing content (mod removed a ship class) → log, skip that entity, continue. Never
   `Error()`, never assert, in the load path.
5. Mission edited (checksum/`modified` mismatch) → treat the checkpoint as absent by
   default.
6. Version constant bumps only for structural/semantic changes, never field additions.
7. Unknown sections skipped via the `default:` case, same as `plr.cpp:1160`.
8. New timestamp fields in future engine versions need no work — the clock-rebase strategy
   covers them, and unsaved ones just start fresh.

**Testing.** Add a `test/src/mission/checkpoint.cpp` unit test (the repo has a `test/`
gtest target) covering: round-trip of the field registry; loading a checkpoint written with
a field removed; loading one with an unknown extra field; loading with an unknown ship class
name; clock-rebase arithmetic. For integration, a headless script that loads a stock
mission, stores, mutates state, loads, and diffs a canonical state dump — this is the check
that catches engine-update regressions.

### 9. Milestones

1. **File layer + Tier 1 ships and clock.** `FileHandler` optional-read primitives, the
   field registry, checkpoint file read/write, `store-checkpoint`/`load-checkpoint`/
   `checkpoint-exists`/`delete-checkpoint`, ships + subsystems + weapons + physics + wings +
   SEXP variables + scoring + `Missiontime` rebase. Already strictly better than the Lua
   script.
2. **Mission logic state.** `Mission_events`, `Mission_goals`, `Log_entries`,
   `Sexp_nodes[].value/.flags`, `Ships_exited`, arrival/departure reconciliation.
3. **AI, docking, and model animations.** `ai_info`, goals, docking chains, support ship,
   then model animation instance state + moveables (after docking, since dock-stage
   animations depend on it). Includes the two `ModelAnimationSet` accessors and the moveable
   arg cache.
4. **`prompt-user-checkpoint-load` + loadout overrides + `mn.` bindings.**
5. **Selective Tier 3**: weapons in flight and beams, if it proves worth the churn.

### 10. Risks and open questions

- **Docking chains.** Restoring `ai_do_objects_docked_stuff()` in the right order for
  multi-ship chains (and ships that started docked in the parse data but undocked during
  play) is the fiddliest part of Tier 2. Needs its own reconciliation pass after all ships
  exist.
- **Ships mid-death-roll or mid-warp at snapshot time.** Simplest correct answer for
  milestone 1: snapshot them as already-exited (destroyed/departed) rather than trying to
  resume a death roll. Document it.
- **Wave-name aliasing.** Red alert already has to rename ships so wave-N names match
  (`wing_bash_ship_name()`, `redalert.cpp`). Checkpoint restore hits the same problem and
  should reuse that helper.
- **`Sexp_nodes` index stability.** Node indices are only meaningful for an identical parse
  of an identical mission file. The fingerprint check makes this safe, but it is the reason
  the fingerprint must be strict by default.
- **File size.** A 100-ship mission with full subsystem and AI state in indented JSON could
  reach several MB. Compact JSON plus pruning old slots should keep it to hundreds of KB;
  worth measuring in milestone 1 before committing to JSON permanently. If it's a problem,
  the answer is a self-describing TLV binary backend behind the same `FileHandler` interface
  — not going positional.
- **Cross-mod portability.** Explicitly not supported; the fingerprint includes the mod.
- **Animation edge cases.** An animation renamed in the table changes its hash ID, so old
  checkpoints lose it — acceptable, and the trigger-type + name fallback key covers the
  common case. `Random_starting_phase` animations (`Animation_Flags`, `modelanimation.h:60`)
  must be restored by explicit time, never re-rolled. `Initial`-type animations are applied
  to the submodel base and are not kept in running memory (`modelanimation.h:256`), so they
  need no capture at all.

---

# Addendum: mission-entry resume prompt (post-milestone-1)

## Context

Milestone 1 shipped `prompt-user-checkpoint-load` as a SEXP. That is the wrong vehicle for the
main use case, and it also exposed a bug.

A SEXP is evaluated by `mission_eval_goals()`, inside `game_simulation_frame()`. It therefore
*cannot* run before the first frame — the earliest possible moment is during it. The natural
question "you have a checkpoint here, resume?" belongs at mission entry, which no SEXP can
reach. (The option originally agreed said "a blocking in-mission popup... **also a variant on
mission start**"; only the in-mission half was built.)

Tracing the entry sequence turned up something worse.

### The bug: the restore is overwritten on the default path

`game_start_mission()` — and therefore `game_post_level_init()`, where
`mission_checkpoint_apply()` currently sits — runs inside the `GS_STATE_START_GAME` enter
handler (`freespace.cpp:5982`), **before** the briefing, ship select, weapon select and commit.
Two things then run afterwards and stomp the restored player wing:

- `create_wings()` (`missionshipchoice.cpp:2429`), at commit, calls `change_ship_type()` and
  `wl_update_ship_weapons()` on every starting-wing ship — unconditionally, even when the class
  already matches — and *deletes* the object for an empty slot.
- `wss_direct_restore_loadout()` (`freespace.cpp:6173`), in the `GS_STATE_GAME_PLAY` enter
  handler, rewrites starting-wing classes and weapons from `Player_loadout` whenever
  `old_state` is `MAIN_MENU`, `DEATH_BLEW_UP` or **`GAME_PLAY`**.

That last condition is exactly the checkpoint quick-load path: the SEXP posts
`GS_EVENT_START_GAME_QUICK` from gameplay, so `old_state == GS_STATE_GAME_PLAY` and the saved
loadout overwrites the restored one. The `KeepPlayerLoadout` / `KeepWingLoadout` /
`ReopenLoadout` flags are effectively inert today for the same reason — they are applied before
the code that overwrites them.

## Design

### 1. Move the apply point — one funnel for every path

Move `mission_checkpoint_apply()` out of `game_post_level_init()` and into
`game_process_event()` case `GS_EVENT_ENTER_GAME` (`freespace.cpp:5026-5051`), immediately
before `scripting::hooks::OnGameplayStart->run(...)`.

Every route into gameplay funnels through `GS_EVENT_ENTER_GAME`: commit
(`missionshipchoice.cpp:2037`), red alert's Continue button (`redalert.cpp:231-243`, which
bypasses `commit_pressed()` entirely), and quick start (`freespace.cpp:4990`). At that point
`gameseq_set_state()` has fully returned, so `create_wings()`, `wss_direct_restore_loadout()`,
`Game_mode |= GM_IN_MISSION` and `game_start_time()` have all already happened — and the first
`game_do_frame()` has not. This is genuinely "before the first frame".

This single move fixes the stomp on both paths, makes the loadout-override flags actually work,
and gives the entry prompt somewhere to live. Leave the red-alert bash where it is;
`red_alert_bash_ship_status()` running earlier and the checkpoint winning later is the correct
precedence.

Check after moving: `game_post_level_init()` calls `HUD_init()`, `hud_setup_escort_list()` and
`mission_hotkey_set_defaults()` against pristine state. Verify the escort list and hotkeys look
right after a restore; re-run `hud_setup_escort_list()` at the end of apply if not.

### 2. Entry prompt, at the same point

In the same `GS_EVENT_ENTER_GAME` block, before the apply:

1. If `Pending_load.in_progress` is already set, a mid-mission SEXP load is being serviced —
   apply it and do **not** prompt. (The SEXP path re-enters through this same event, so without
   this guard it would prompt again.)
2. Otherwise, if the mission does not carry the opt-out flag and a valid checkpoint exists for
   the default slot, show the prompt.
3. On yes, populate `Pending_load.data` and set `in_progress`; the apply immediately below
   picks it up.

No second mission load: the mission was just parsed fresh, which is the only reason the
mid-mission path needs a restart at all. `Pending_load` (`missioncheckpoint.cpp`) already has
exactly the right shape — `in_progress` means "apply on the way in" — so this is a small change.

Precedent for a modal here is solid: `game_start_mission()` itself calls `popup()`
(`freespace.cpp:1541`, `:1545`) with the level loaded and gameplay not started, and
`missionbrief.cpp:337`/`:480` are post-load pre-gameplay yes/no choices.

### 3. Opt-out mission flag

Add `Mission_Flags::No_checkpoint_resume_prompt` at the end of the `FLAG_LIST` in
`code/mission/mission_flags.h`, plus one row in `Parse_mission_flags[]` and one in
`Parse_mission_flag_descriptions[]` (`missionparse.cpp:395`, `:430`), as
`{"No Checkpoint Resume Prompt", ..., true, false}`.

qtfred builds its Mission Specs flag list generically from `Parse_mission_flags`
(`MissionSpecDialogModel.cpp:483-497`, skipping `is_special` / `!in_use`), so the checkbox and
its tooltip appear with no FRED UI work. Flags are written to the `.fs2` by name, so this is
forward- and backward-compatible.

Automatic by default: a checkpoint file only exists if the designer called `store-checkpoint`,
so writing one is itself the opt-in. The flag exists to suppress the entry prompt for missions
where a mid-mission SEXP prompt is the intended flow.

### 4. Keep the SEXP

`prompt-user-checkpoint-load` stays, unchanged in behaviour, for designer-triggered mid-mission
prompts. That case genuinely needs the reload, because mission state has diverged from the
checkpoint. Update its FRED help text to point at the entry prompt for the resume-on-start case.

Checkpoint lifecycle is unchanged: nothing is deleted automatically. A checkpoint persists
until `delete-checkpoint`, the Lua equivalent, or a `store-checkpoint` overwrite.

---

# Final phase: Lua API

Exposed on the `mn.` library (`code/scripting/api/libs/mission.cpp`, `ADE_LIB` at `:180`).
Gives `shipsaveload` users a migration path and lets SCPUI build a checkpoint browser.

| Function | Returns | Notes |
|---|---|---|
| `mn.storeCheckpoint([slot])` | boolean | Slot defaults to `"default"` |
| `mn.loadCheckpoint([slot], [flags])` | nothing | Queues the load, same as the SEXP |
| `mn.checkpointExists([slot])` | boolean | Fingerprint-checked, same as the SEXP |
| `mn.deleteCheckpoint([slot])` | nothing | |
| `mn.getCheckpointInfo([slot])` | table or nil | `missionTime`, `savedAt`, `slot`, `missionFilename`, `valid` |
| `mn.getCheckpointSlots([missionName])` | table of strings | Every slot for that mission, current pilot + campaign |
| `mn.deleteAllCheckpoints([missionName])` | number deleted | The requested bulk delete |

`getCheckpointSlots` and `deleteAllCheckpoints` both work by enumerating
`CF_TYPE_CHECKPOINTS` with `cf_get_file_list(SCP_vector<SCP_string>&, pathtype, filter, ...)`
(`cfile.h:361`) and matching the `<pilot>.<campaign>.<mission>.` prefix that
`checkpoint_filename()` already builds — so the slot name is whatever remains before `.chk`.
Both default `missionName` to the current mission, and both are scoped to the current pilot and
campaign; there is deliberately no cross-pilot bulk delete.

Factor the prefix construction out of `checkpoint_filename()` in `checkpointfile.cpp` so the
enumeration and the single-file path cannot disagree about the naming scheme, and add
`checkpoint_list_slots()` / `checkpoint_delete_all()` there rather than in the Lua layer.

## Ordering

1. Move the apply point to `GS_EVENT_ENTER_GAME` — this is the bug fix, and it is worth doing
   and testing on its own before anything is layered on top.
2. Entry prompt + opt-out mission flag.
3. Lua API.

Milestone 2 (mission events, goals, log, `Sexp_nodes` state, SEXP containers) is independent of
all of this and still needed before any of it is usable for a real mission.

---

## Verification

- Build: `cmake --build` the `code` target and the `test` target; run the new gtest cases.
- Manual, single-player, stock FS2 campaign:
  1. Fly a mission, damage ships, kill a few fighters, let events fire, `store-checkpoint`.
  2. Continue, die or fail, `prompt-user-checkpoint-load`, choose yes.
  3. Confirm after reload: HUD mission clock matches the checkpoint time; the mission log
     shows the pre-checkpoint entries and nothing more; already-satisfied directives stay
     satisfied and do not re-announce; kill count and score match; hull/shield/ammo/subsystem
     damage matches; destroyed ships stay destroyed; wing wave counts match; a `when` that
     had already fired does not fire again; `has-time-elapsed` events downstream of the
     checkpoint fire at the right absolute time; a `Scripted` animation that was part-way
     through resumes from the same frame rather than snapping open or closed.
  4. Quit to desktop, relaunch, re-enter the mission, `checkpoint-exists` → true, load →
     same result.
  5. Load with `Keep current player loadout` and confirm the player keeps the new fit while
     everything else restores.
- Addendum-specific:
  1. **The stomp bug.** Before the fix: store a checkpoint after changing the player's ship
     class or firing off ammo, load it, and confirm the player wing comes back with the
     *briefing* loadout rather than the saved one. After the fix, the saved one wins.
  2. Enter a mission that has a checkpoint: the prompt appears after commit, before the first
     frame, with no second loading screen. Answer No and confirm a completely normal fresh
     start.
  3. Fire `prompt-user-checkpoint-load` mid-mission and confirm you are prompted exactly once,
     not again on the way back into the mission.
  4. Set the mission's `No Checkpoint Resume Prompt` flag in FRED and confirm the entry prompt
     is suppressed while `load-checkpoint` still works.
  5. Confirm the escort list, wingman status gauge and hotkey assignments are correct after a
     restore, since those are set up before the new apply point.
  6. Lua: `mn.getCheckpointSlots()` lists every slot written for the mission, and
     `mn.deleteAllCheckpoints()` removes exactly those and returns the count.
- Resilience: hand-edit the `.chk` JSON to delete a field, add a bogus field, and rename a
  ship class; confirm each loads with warnings and no crash. Edit the mission in FRED and
  confirm `checkpoint-exists` returns false.

---

# HANDOFF — read this first in a new session

Everything above is the *design*. This section is what actually exists, what deviates from the
design, and what is broken. A new session should read this before touching anything.

## Where the code is

Two commits on branch `claude/freespace-checkpoint-system-f0ocum`:

| Commit | Contents |
|---|---|
| `be59944` | Milestone 1: file layer, Tier 1 capture/restore, 5 SEXPs, clock rebase, 5 unit tests |
| `d75122f` | Apply-point move (bug fix), mission-entry resume prompt, opt-out mission flag, Lua API |

They live on **`ofp-fs2open/fs2open.github.com`** (public), not the personal fork — that session
was scoped to `ofp-fs2open` and the git proxy returns 403 on push to any other owner. To pick
them up:

```
git remote add ofp https://github.com/ofp-fs2open/fs2open.github.com
git fetch ofp claude/freespace-checkpoint-system-f0ocum
git checkout -b claude/freespace-checkpoint-system-f0ocum FETCH_HEAD
```

Do not delete the `ofp-fs2open` copy until the branch is confirmed pushed elsewhere; it is
currently the only copy.

## Status

**Done:** milestone 1 and the whole addendum + Lua phase. Builds clean, links, 225/225 unit
tests pass.

**Not done:** milestone 2 (`Mission_events`, `Mission_goals`, `Log_entries`,
`Sexp_nodes[].value/.flags`, SEXP containers), milestone 3 (AI, docking, model animations),
milestone 5 (Tier 3).

**Never run in the actual game.** Only the build and the unit tests, and the unit tests cover
the *file format*, not the restore. Every behavioural claim about restore is unverified.

Consequence: **this is not yet usable for a real mission.** Without milestone 2 a restored
mission has no memory of what already happened, so every `when` whose condition still holds
re-fires — messages replay, directives re-announce, the log restarts.

## Deviations from the design above

The plan was written before implementation; these four things ended up different, all
deliberately, but the text above was not updated.

1. **One file per slot**, not multiple slots inside one file. Name is
   `<pilot>.<campaign>.<mission>.<slot>.chk`. Every component is sanitised to
   `[a-z0-9_-]`, so the only dots are separators — which is what lets
   `checkpoint_list_slots()` recover a slot name by stripping the prefix. A bad write
   corrupts one slot rather than all of them, and delete is a file delete.
2. **New `CF_TYPE_CHECKPOINTS` (37)** at `data/players/checkpoints`, rather than adding
   `.chk` to the `CF_TYPE_PLAYERS` row. `CF_MAX_PATH_TYPES` went 37 → 38.
3. **No index tables.** The design called for the `csg_read_info()` pattern (write full
   ship/weapon class name lists, store indices into them). Implementation writes the class
   *name* inline at each field instead. More verbose on disk, but strictly more
   self-describing and it removes a whole class of index-drift bug. If file size ever forces
   the issue, that is where to claw it back.
4. **Still `JSON_INDENT(4)`**, not `JSON_COMPACT` as the design says. `json_dump_cfile` is
   called once in `JSONFileHandler::flush()` (`JSONFileHandler.cpp:189`) and is shared with
   pilot files, so switching it needs a per-instance flag rather than a one-word change.
   Worth doing before worrying about checkpoint file size.

Also: the design's `prompt-user-checkpoint-load` entry says `PF_RUN_STATE`. That was wrong and
the code does **not** do it — see the gotchas below.

## Bugs and gaps

1. **XSTR ID collision — fix this first, it is a real bug.** Three strings were added with IDs
   1830/1831/1832. **1830 is already taken by "SCP Options".** The highest ID actually in use
   across the tree is 2000, so these should be renumbered to **2001/2002/2003**. Locations:
   `code/parse/sexp.cpp:18778`, `code/mission/missioncheckpoint.cpp:1160-1161`.
2. **HUD escort list and mission hotkeys go stale after a restore.** They are built in
   `game_post_level_init()` from the pristine mission, which is now well before the apply
   point, so a ship added to the escort list during the saved run does not appear.
   `hud_setup_escort_list()` cannot simply be re-run there — it deliberately early-returns
   once `GM_IN_MISSION` is set, which it is by then. Belongs with the rest of the HUD state in
   milestone 2. Commented at the apply site in `missioncheckpoint.cpp`.
3. The entry prompt fires on **every** entry while a checkpoint exists, including after
   death → Restart (which posts `GS_EVENT_START_GAME` and lands on the same funnel). That is
   the intended behaviour given "keep checkpoints until explicitly deleted", but it has not
   been seen in practice and may feel wrong.

## Gotchas that will bite

- **Never restore `PF_RUN_STATE` to the checkpoint popups.** That flag makes `popup()` run the
  underlying state's do-frame, and both popups are reached from inside SEXP evaluation inside
  the simulation step — it recurses into itself. Plain `popup()` freezes the mission behind a
  saved screen and `popup_init()` stops the clock, which is also the behaviour you want.
- **`mission_checkpoint_apply()` must stay in `GS_EVENT_ENTER_GAME`.** Moving it back to
  `game_post_level_init()` silently reintroduces the loadout stomp, because `create_wings()`
  and `wss_direct_restore_loadout()` both run after that point and rewrite starting-wing ship
  classes and weapons.
- **`timestamp_adjust_microseconds()` asserts `Timestamp_paused_at_counter != 0`.** That is
  satisfied only because `game_init()` calls `game_stop_time()` once at startup. Undocumented
  dependency, shared with the existing pre-player-skip caller at `freespace.cpp:4643`.
- **Flags are written by name from hand-curated tables** in `missioncheckpoint.cpp`
  (`Ship_flag_table`, `Object_flag_table`, `Subsys_flag_table`, `Weapon_flag_table`). A flag
  not in a table is left alone by the restore rather than cleared. Adding a mutable flag means
  adding a row; never serialise raw bit positions, they shift between versions.
- **`test/test_data/data/*_settings.ini` and `test/test_data/players/` are generated** by
  running `unittests`. They are untracked and must not be committed — `git clean -fd
  test/test_data/` before staging.

## Build recipe

```
git submodule update --init --recursive        # required, config fails without it
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug \
      -DFSO_BUILD_TESTS=ON -DFSO_BUILD_FRED2=OFF -DFSO_BUILD_QTFRED=OFF
ninja -C build
build/bin/unittests --gtest_filter='CheckpointFileTest.*'
```
A full build is slow (~10 min on 4 cores); touching `mission_flags.h` or `sexp.h` rebuilds
most of the tree.

## Shared infrastructure that was modified

These are outside the checkpoint module and affect other systems, so they matter for review:

- `pilot::FileHandler` — added `hasField()`, `readByteOr/UByteOr/ShortOr/IntOr/UIntOr/FloatOr/
  StringOr/BoolOr`, `writeBool()`. Defaults live in the base class.
- `JSONFileHandler` — **nested array reads now work** (previously
  `Assertion(_arrayIndex == INVALID_SIZE, "Array nesting is not supported yet!")`), via an
  `_arrayIndexStack`. `readFloat()` now also accepts a JSON integer.
- `BinaryFileHandler::hasField()` returns false — the format is positional, so tolerant reads
  degrade to defaults there. Checkpoints therefore must use the JSON backend.
- `Section` enum gained `CheckpointInfo/Clock/Ships/Wings/Scoring` (0x0017–0x001B).
- `cfile` — `CF_TYPE_CHECKPOINTS`, `CF_MAX_PATH_TYPES` 37→38.
- `sexp` — 5 operators appended to the end of the `OP_*` enum, plus
  `OPF_CHECKPOINT_LOAD_FLAG`, `SEXP_CHECK_INVALID_CHECKPOINT_LOAD_FLAG`,
  `CHANGE_SUBCATEGORY_CHECKPOINTS`.
- `Mission_Flags::No_checkpoint_resume_prompt`.

## Suggested first moves in the new session

1. Renumber the XSTR IDs to 2001–2003.
2. Actually run the game and work through the Verification section, especially the stomp-bug
   check — it is the one thing you can see directly.
3. Then milestone 2, which is what makes this usable.

---
