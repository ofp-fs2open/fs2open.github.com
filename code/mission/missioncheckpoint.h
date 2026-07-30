/*
 * Copyright (C) Freespace Open 2013.  All rights reserved.
 *
 * All source code herein is the property of Freespace Open. You may not sell
 * or otherwise commercially exploit the source or things you created based on the
 * source.
 */

#ifndef _MISSIONCHECKPOINT_H
#define _MISSIONCHECKPOINT_H

#include "globalincs/pstypes.h"
#include "globalincs/vmallocator.h"
#include "math/vecmat.h"

/*
 * Mission checkpoints: save the state of a mission in progress and restore it later.
 *
 * A checkpoint is captured live (mission_checkpoint_store) and restored by reloading the
 * mission from scratch and bashing the saved state on top of the freshly created objects
 * (mission_checkpoint_apply, called from game_post_level_init).  Reloading rather than
 * restoring in place means the engine is always in a known-clean state; it is the same
 * approach the red alert code takes.
 *
 * Everything that crosses the file is keyed by name -- ship names, class names, subsystem
 * names -- never by a runtime index, so a checkpoint survives table changes and engine
 * updates.  See checkpointfields.h for how the per-struct field lists work.
 */

class object;
class ship;
class ship_subsys;
class ship_weapon;
struct wing;

namespace checkpoint {

// Options for a load, chosen by the mission designer on the load-checkpoint SEXP.
enum class LoadFlags : uint32_t {
	None = 0,

	// Leave the player's ship class and weapon banks as the fresh mission load produced them
	// instead of restoring what the checkpoint recorded.  Lets a player retry with a
	// different fit.
	KeepPlayerLoadout = 1 << 0,

	// As above, but for the rest of the player's wing.
	KeepWingLoadout = 1 << 1,

	// Go through briefing and ship/weapon select before resuming, rather than dropping
	// straight back into the mission.
	ReopenLoadout = 1 << 2,

	// Apply whatever the checkpoint contains even if the mission file has changed since it
	// was written.  Off by default because a mission edit invalidates SEXP node indices.
	IgnoreFingerprint = 1 << 3,
};

inline LoadFlags operator|(LoadFlags a, LoadFlags b)
{
	return static_cast<LoadFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline LoadFlags& operator|=(LoadFlags& a, LoadFlags b)
{
	a = a | b;
	return a;
}

inline bool any(LoadFlags value, LoadFlags test)
{
	return (static_cast<uint32_t>(value) & static_cast<uint32_t>(test)) != 0;
}

// ------------------------------------------------------------------
// Captured state
// ------------------------------------------------------------------

// One weapon bank.  weapon_class is empty when the bank holds nothing.
struct weapon_bank {
	SCP_string weapon_class;
	int ammo = 0;
	int start_ammo = 0;
	int capacity = 0;
	int next_slot = 0;
	int next_fire_stamp = 0;
	int last_fire_stamp = 0;
	int rearm_time = 0;
	int burst_counter = 0;
	int burst_seed = 0;
};

struct weapon_state {
	SCP_vector<weapon_bank> primary_banks;
	SCP_vector<weapon_bank> secondary_banks;
	SCP_string tertiary_class;

	// Named flags from ship_weapon::flags; see Weapon_flag_names in missioncheckpoint.cpp.
	SCP_vector<SCP_string> flags;

	// Scalars from CKPT_WEAPONS_INTS.
	SCP_map<SCP_string, int> scalars;
};

// A subsystem is identified by its model subobject name.  Ships routinely carry several
// subsystems with the same name (engine01, engine01, ...), so an ordinal disambiguates them
// within that name.  Matching by name rather than by list position -- which is what the red
// alert code does -- means the restore survives a model whose subsystem list has changed.
struct subsystem_state {
	SCP_string name;
	int ordinal = 0;

	SCP_string sub_name;         // WMC's per-instance name override, if any
	SCP_string cargo_title;
	SCP_string turret_target;    // ship name this turret was firing on, if any

	SCP_vector<SCP_string> flags;
	SCP_map<SCP_string, float> floats;
	SCP_map<SCP_string, int> ints;

	// Turrets have their own weapon banks.
	weapon_state weapons;
	bool has_weapons = false;
};

// What had become of a ship at the moment the checkpoint was taken.  Mirrors ShipStatus, but
// is written by name so the file does not depend on the enum's ordering.
enum class ShipDisposition {
	Present,      // in the mission, alive
	NotYetHere,   // still on the arrival list
	Destroyed,
	Departed,
	Vanished,
};

struct ship_state {
	SCP_string name;
	ShipDisposition disposition = ShipDisposition::Present;

	// --- only meaningful when disposition == Present ---

	SCP_string ship_class;
	SCP_string team;
	SCP_string display_name;
	SCP_string wing_name;
	SCP_string cargo_title;
	SCP_string countermeasure_class;

	vec3d pos = vmd_zero_vector;
	matrix orient = vmd_identity_matrix;

	float hull = 0.0f;
	float max_hull = 0.0f;
	// Sized to match object::shield_quadrant, which is not fixed at four for every model.
	SCP_vector<float> shield_quadrants;

	char cargo1 = 0;

	SCP_vector<SCP_string> flags;
	SCP_vector<SCP_string> object_flags;
	SCP_map<SCP_string, float> floats;
	SCP_map<SCP_string, int> ints;
	SCP_map<SCP_string, float> physics_floats;
	SCP_map<SCP_string, vec3d> physics_vecs;

	SCP_vector<subsystem_state> subsystems;
	weapon_state weapons;

	// --- only meaningful when the ship had already left ---
	fix exit_time = 0;
};

struct wing_state {
	SCP_string name;
	SCP_map<SCP_string, int> ints;
	SCP_vector<SCP_string> ship_names;   // wing::ship_index, resolved to names
	fix time_gone = 0;
	int wave_delay_timestamp = 0;
};

struct variable_state {
	SCP_string name;
	bool is_number = false;
	SCP_string value;
};

struct scoring_state {
	SCP_map<SCP_string, int> ints;
	// Per-ship-class kills, keyed by class name so a table change cannot misattribute them.
	SCP_map<SCP_string, int> class_kills;
};

struct checkpoint_data {
	// --- identity and validity ---
	int version = 0;
	SCP_string slot;
	SCP_string mission_filename;
	SCP_string mission_modified;   // The_mission.modified
	uint mission_checksum = 0;     // Current_file_checksum
	SCP_string campaign;
	SCP_string pilot;
	SCP_string mod_title;

	// --- clock ---
	// Missiontime is a fix; microseconds is what the timestamp clock is actually rebased
	// with, and is stored separately so we do not lose precision through the fix conversion.
	fix mission_time = 0;
	std::uint64_t mission_time_microseconds = 0;
	int hud_timer_padding = 0;

	// --- state ---
	SCP_vector<ship_state> ships;
	SCP_vector<wing_state> wings;
	SCP_vector<variable_state> variables;
	scoring_state scoring;

	bool loaded = false;
};

} // namespace checkpoint

// ------------------------------------------------------------------
// Public API
// ------------------------------------------------------------------

// Capture the current mission state and write it to the named slot.  Returns false (and logs)
// if the file could not be written.  Safe to call from inside SEXP evaluation.
bool mission_checkpoint_store(const SCP_string& slot);

// Does a usable checkpoint exist for this pilot, campaign, mission and slot?  A checkpoint
// whose mission fingerprint no longer matches counts as absent.
bool mission_checkpoint_exists(const SCP_string& slot);

// Remove a checkpoint.  Silently does nothing if there was none.
void mission_checkpoint_delete(const SCP_string& slot);

// Request a load.  This does NOT reload the mission itself -- doing that while SEXP
// evaluation is on the stack would tear the level down underneath the caller.  It records the
// request; mission_checkpoint_process_pending_load() acts on it at the end of the frame.
void mission_checkpoint_request_load(const SCP_string& slot, checkpoint::LoadFlags flags);

// Is a load queued?
bool mission_checkpoint_load_pending();

// Called once per frame at the end of the gameplay loop.  If a load is queued, posts the
// mission restart that will eventually land in mission_checkpoint_apply().
void mission_checkpoint_process_pending_load();

// Called from game_post_level_init(), after the mission has been parsed and its objects
// created but before the mission actually starts running.  Applies a checkpoint if one is
// being restored; otherwise does nothing.
void mission_checkpoint_apply();

// Discard any queued or in-flight restore.  Called when leaving a mission by any route other
// than a checkpoint load, so a stale request cannot leak into the next mission.
void mission_checkpoint_clear_pending();

// Parse a designer-supplied flag name into a LoadFlags bit.  Returns false if unrecognised.
bool mission_checkpoint_parse_load_flag(const char* name, checkpoint::LoadFlags& out);

#endif // _MISSIONCHECKPOINT_H
