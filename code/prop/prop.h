#pragma once

#include "globalincs/pstypes.h"

#include "mission/mission_flags.h"
#include "mission/missionparse.h"
#include "object/object.h"
#include "object/objcollide.h"
#include "prop/prop_flags.h"
#include "ship/ship.h"

#define MAX_PROP_DETAIL_LEVELS    MAX_SHIP_DETAIL_LEVELS

typedef struct prop_info {
	SCP_string name;                                            // Prop name
	int category_index;                                         // Index of the category this prop belongs to, -1 if it doesn't belong to any category
	SCP_string pof_file;                                        // Pof filename
	vec3d closeup_pos;                                          // position for camera when using prop in closeup view (eg briefing and techroom)
	float closeup_zoom;                                         // zoom when using prop in closeup view (eg briefing and techroom)
	int model_num = -1;                                         // The model number of the loaded POF
	int num_detail_levels;                                      // Detail levels of the model
	int detail_distance[MAX_PROP_DETAIL_LEVELS];                // distance to change detail levels at
	SCP_unordered_map<int, int> glowpoint_bank_override_map;    // Glow point bank overrides currently unused; values index glowpoint_bank_overrides
	flagset<Prop::Info_Flags> flags;                            // Info flags
	SCP_map<SCP_string, SCP_string> custom_data;                // Custom data for this prop
	SCP_vector<custom_string> custom_strings;                   // Custom strings for this prop
	SCP_vector<texture_replace> replacement_textures;           // Class-level texture replacements (from_table == true)
} prop_info;

typedef struct prop {
	char prop_name[NAME_LENGTH];
	int objnum;
	int prop_info_index;
	int model_instance_num;
	uint create_time;                     // time prop was created, set by gettime()
	fix time_created;
	float alpha_mult;
	SCP_string fred_layer = "Default";
	bool fred_locked = false;	// FRED transform lock: position and orientation can't be edited
	int fred_groups = 0;		// FRED selection groups, a bitmask with group N as bit N-1
	// glow points
	SCP_deque<bool> glow_point_bank_active;
	flagset<Prop::Prop_Flags> flags;
	SCP_vector<texture_replace> replacement_textures;  // Class + instance texture replacements; only from_table == false entries are saved
	// spawn/despawn cues, analogous to ship arrival/departure (spawn is only meaningful in FRED
	// once the prop exists; despawn is evaluated at runtime).  Delays follow the ship convention:
	// stored as -seconds in-game (timer not yet set), positive seconds in FRED.
	int spawn_cue = -1;
	int spawn_delay = 0;
	int despawn_cue = -1;
	int despawn_delay = 0;
} prop;

typedef struct prop_category {
	SCP_string name;
	color list_color;
} prop_category;

typedef struct parsed_prop {
	char name[NAME_LENGTH];
	int prop_info_index;
	matrix orientation;
	vec3d position;
	flagset<Mission::Parse_Object_Flags> flags;
	SCP_string fred_layer = "Default";
	bool fred_locked = false;
	int fred_groups = 0;
	SCP_vector<texture_replace> replacement_textures;
	// spawn/despawn cues (default to Locked_sexp_true/false in parse_prop).  In-game these props
	// stay pending until the spawn cue fires; 'spawned' tracks whether the object has been created.
	int spawn_cue = -1;
	int spawn_delay = 0;
	int despawn_cue = -1;
	int despawn_delay = 0;
	bool spawned = false;
} parsed_prop;

extern bool Props_inited;

// Global prop info
extern SCP_vector<prop_info> Prop_info;

// Global prop categories
extern SCP_vector<prop_category> Prop_categories;

// Global prop objects. Vector of optionals so that we can have stable indices
// and still be able to remove props. Deleted props are set to std::nullopt
// so any access should check if the optional has a value first.
// The vector is cleared at the end of each mission, never during.
extern SCP_vector<std::optional<prop>> Props;

inline int prop_info_size()
{
	return static_cast<int>(Prop_info.size());
}

// Load all props from table
void prop_init();

// Object management
int prop_create(const matrix* orient, const vec3d* pos, int prop_type, const char* name = nullptr);
void prop_delete(object* obj);
void prop_render(object* obj, model_draw_list* scene);

void props_level_init();
void props_level_close();

int prop_info_lookup(const char* token);
int prop_name_lookup(const char* name);
prop* prop_id_lookup(int id);

void change_prop_type(int n, int prop_type);

// (Re)build the prop's model instance texture-replace slots from its replacement_textures vector
void prop_apply_replacement_textures(prop* propp);

prop_category* prop_get_category(int index);

int prop_check_collision(object* prop_obj, object* other_obj, vec3d* hitpos, collision_info_struct* prop_hit_info);