/*
 * Copyright (C) Freespace Open 2013.  All rights reserved.
 *
 * All source code herein is the property of Freespace Open. You may not sell
 * or otherwise commercially exploit the source or things you created based on the
 * source.
 */

#include "mission/checkpointfile.h"

#include "cfile/cfile.h"
#include "mission/missioncampaign.h"
#include "mission/missionparse.h"
#include "mod_table/mod_table.h"
#include "pilotfile/FileHandler.h"
#include "pilotfile/JSONFileHandler.h"
#include "playerman/player.h"

#include <memory>

// Defined in freespace.cpp; declared here rather than pulling in the whole freespace header,
// which is the same thing scripting.cpp does.
extern char Game_current_mission_filename[];

namespace {

// Slot names are written by mission designers and end up in a filename, so restrict them to
// something every filesystem will accept.  Anything else becomes an underscore.
SCP_string sanitize_for_filename(const SCP_string& in)
{
	SCP_string out;
	out.reserve(in.size());

	for (char ch : in) {
		if (isalnum(static_cast<unsigned char>(ch)) || ch == '-' || ch == '_') {
			out += static_cast<char>(tolower(static_cast<unsigned char>(ch)));
		} else {
			out += '_';
		}
	}

	if (out.empty()) {
		out = "default";
	}

	return out;
}

// Strip the extension from a mission or campaign filename so it can go into a checkpoint
// filename without a second dot confusing anything.
SCP_string base_name(const char* filename)
{
	SCP_string out(filename != nullptr ? filename : "");

	auto dot = out.rfind('.');
	if (dot != SCP_string::npos) {
		out.erase(dot);
	}

	return sanitize_for_filename(out);
}

// ------------------------------------------------------------------
// Small helpers for the repetitive name/value maps
// ------------------------------------------------------------------

void write_string_list(pilot::FileHandler* handler, const char* name, const SCP_vector<SCP_string>& values)
{
	handler->startArrayWrite(name, values.size());
	for (const auto& value : values) {
		handler->startSectionWrite(Section::Unnamed);
		handler->writeString("v", value.c_str());
		handler->endSectionWrite();
	}
	handler->endArrayWrite();
}

void read_string_list(pilot::FileHandler* handler, const char* name, SCP_vector<SCP_string>& values)
{
	values.clear();

	if (!handler->hasField(name)) {
		return;
	}

	auto count = handler->startArrayRead(name);
	for (size_t i = 0; i < count; i++, handler->nextArraySection()) {
		values.push_back(handler->readStringOr("v", ""));
	}
	handler->endArrayRead();
}

// Name/value maps go out as an array of {k, v} objects.  An array rather than a JSON object
// keyed by the field name because the handler's read side iterates arrays but cannot enumerate
// the keys of an arbitrary object.
template <typename T, typename ReadFn>
void read_named_map(pilot::FileHandler* handler, const char* name, SCP_map<SCP_string, T>& values, ReadFn read)
{
	values.clear();

	if (!handler->hasField(name)) {
		return;
	}

	auto count = handler->startArrayRead(name);
	for (size_t i = 0; i < count; i++, handler->nextArraySection()) {
		auto key = handler->readStringOr("k", "");
		if (key.empty()) {
			continue;
		}
		values[key] = read(handler);
	}
	handler->endArrayRead();
}

void write_int_map(pilot::FileHandler* handler, const char* name, const SCP_map<SCP_string, int>& values)
{
	handler->startArrayWrite(name, values.size());
	for (const auto& entry : values) {
		handler->startSectionWrite(Section::Unnamed);
		handler->writeString("k", entry.first.c_str());
		handler->writeInt("v", entry.second);
		handler->endSectionWrite();
	}
	handler->endArrayWrite();
}

void read_int_map(pilot::FileHandler* handler, const char* name, SCP_map<SCP_string, int>& values)
{
	read_named_map<int>(handler, name, values, [](pilot::FileHandler* h) { return h->readIntOr("v", 0); });
}

void write_float_map(pilot::FileHandler* handler, const char* name, const SCP_map<SCP_string, float>& values)
{
	handler->startArrayWrite(name, values.size());
	for (const auto& entry : values) {
		handler->startSectionWrite(Section::Unnamed);
		handler->writeString("k", entry.first.c_str());
		handler->writeFloat("v", entry.second);
		handler->endSectionWrite();
	}
	handler->endArrayWrite();
}

void read_float_map(pilot::FileHandler* handler, const char* name, SCP_map<SCP_string, float>& values)
{
	read_named_map<float>(handler, name, values, [](pilot::FileHandler* h) { return h->readFloatOr("v", 0.0f); });
}

void write_vec_map(pilot::FileHandler* handler, const char* name, const SCP_map<SCP_string, vec3d>& values)
{
	handler->startArrayWrite(name, values.size());
	for (const auto& entry : values) {
		handler->startSectionWrite(Section::Unnamed);
		handler->writeString("k", entry.first.c_str());
		handler->writeFloat("x", entry.second.xyz.x);
		handler->writeFloat("y", entry.second.xyz.y);
		handler->writeFloat("z", entry.second.xyz.z);
		handler->endSectionWrite();
	}
	handler->endArrayWrite();
}

void read_vec_map(pilot::FileHandler* handler, const char* name, SCP_map<SCP_string, vec3d>& values)
{
	read_named_map<vec3d>(handler, name, values, [](pilot::FileHandler* h) {
		vec3d out;
		out.xyz.x = h->readFloatOr("x", 0.0f);
		out.xyz.y = h->readFloatOr("y", 0.0f);
		out.xyz.z = h->readFloatOr("z", 0.0f);
		return out;
	});
}

void write_vector(pilot::FileHandler* handler, const char* prefix_x, const char* prefix_y, const char* prefix_z,
                  const vec3d& value)
{
	handler->writeFloat(prefix_x, value.xyz.x);
	handler->writeFloat(prefix_y, value.xyz.y);
	handler->writeFloat(prefix_z, value.xyz.z);
}

void read_vector(pilot::FileHandler* handler, const char* prefix_x, const char* prefix_y, const char* prefix_z,
                 vec3d& value)
{
	value.xyz.x = handler->readFloatOr(prefix_x, value.xyz.x);
	value.xyz.y = handler->readFloatOr(prefix_y, value.xyz.y);
	value.xyz.z = handler->readFloatOr(prefix_z, value.xyz.z);
}

// ------------------------------------------------------------------
// Weapon banks
// ------------------------------------------------------------------

void write_weapon_banks(pilot::FileHandler* handler, const char* name, const SCP_vector<checkpoint::weapon_bank>& banks)
{
	handler->startArrayWrite(name, banks.size());
	for (const auto& bank : banks) {
		handler->startSectionWrite(Section::Unnamed);
		handler->writeString("class", bank.weapon_class.c_str());
		handler->writeInt("ammo", bank.ammo);
		handler->writeInt("start_ammo", bank.start_ammo);
		handler->writeInt("capacity", bank.capacity);
		handler->writeInt("next_slot", bank.next_slot);
		handler->writeInt("next_fire_stamp", bank.next_fire_stamp);
		handler->writeInt("last_fire_stamp", bank.last_fire_stamp);
		handler->writeInt("rearm_time", bank.rearm_time);
		handler->writeInt("burst_counter", bank.burst_counter);
		handler->writeInt("burst_seed", bank.burst_seed);
		handler->endSectionWrite();
	}
	handler->endArrayWrite();
}

void read_weapon_banks(pilot::FileHandler* handler, const char* name, SCP_vector<checkpoint::weapon_bank>& banks)
{
	banks.clear();

	if (!handler->hasField(name)) {
		return;
	}

	auto count = handler->startArrayRead(name);
	for (size_t i = 0; i < count; i++, handler->nextArraySection()) {
		checkpoint::weapon_bank bank;
		bank.weapon_class = handler->readStringOr("class", "");
		bank.ammo = handler->readIntOr("ammo", 0);
		bank.start_ammo = handler->readIntOr("start_ammo", 0);
		bank.capacity = handler->readIntOr("capacity", 0);
		bank.next_slot = handler->readIntOr("next_slot", 0);
		bank.next_fire_stamp = handler->readIntOr("next_fire_stamp", 0);
		bank.last_fire_stamp = handler->readIntOr("last_fire_stamp", 0);
		bank.rearm_time = handler->readIntOr("rearm_time", 0);
		bank.burst_counter = handler->readIntOr("burst_counter", 0);
		bank.burst_seed = handler->readIntOr("burst_seed", 0);
		banks.push_back(std::move(bank));
	}
	handler->endArrayRead();
}

void write_weapon_state(pilot::FileHandler* handler, const checkpoint::weapon_state& weapons)
{
	write_weapon_banks(handler, "primary_banks", weapons.primary_banks);
	write_weapon_banks(handler, "secondary_banks", weapons.secondary_banks);
	handler->writeString("tertiary_class", weapons.tertiary_class.c_str());
	write_string_list(handler, "flags", weapons.flags);
	write_int_map(handler, "scalars", weapons.scalars);
}

void read_weapon_state(pilot::FileHandler* handler, checkpoint::weapon_state& weapons)
{
	read_weapon_banks(handler, "primary_banks", weapons.primary_banks);
	read_weapon_banks(handler, "secondary_banks", weapons.secondary_banks);
	weapons.tertiary_class = handler->readStringOr("tertiary_class", "");
	read_string_list(handler, "flags", weapons.flags);
	read_int_map(handler, "scalars", weapons.scalars);
}

// ------------------------------------------------------------------
// Ship dispositions, written by name so the file does not depend on enum order
// ------------------------------------------------------------------

const char* disposition_name(checkpoint::ShipDisposition disposition)
{
	switch (disposition) {
	case checkpoint::ShipDisposition::Present:
		return "present";
	case checkpoint::ShipDisposition::NotYetHere:
		return "not_yet_here";
	case checkpoint::ShipDisposition::Destroyed:
		return "destroyed";
	case checkpoint::ShipDisposition::Departed:
		return "departed";
	case checkpoint::ShipDisposition::Vanished:
		return "vanished";
	}
	return "present";
}

checkpoint::ShipDisposition disposition_value(const SCP_string& name)
{
	if (name == "not_yet_here") {
		return checkpoint::ShipDisposition::NotYetHere;
	}
	if (name == "destroyed") {
		return checkpoint::ShipDisposition::Destroyed;
	}
	if (name == "departed") {
		return checkpoint::ShipDisposition::Departed;
	}
	if (name == "vanished") {
		return checkpoint::ShipDisposition::Vanished;
	}
	return checkpoint::ShipDisposition::Present;
}

// ------------------------------------------------------------------
// Sections
// ------------------------------------------------------------------

void write_info(pilot::FileHandler* handler, const checkpoint::checkpoint_data& data)
{
	handler->startSectionWrite(Section::CheckpointInfo);

	handler->writeString("slot", data.slot.c_str());
	handler->writeString("mission_filename", data.mission_filename.c_str());
	handler->writeString("mission_modified", data.mission_modified.c_str());
	handler->writeUInt("mission_checksum", data.mission_checksum);
	handler->writeString("campaign", data.campaign.c_str());
	handler->writeString("pilot", data.pilot.c_str());
	handler->writeString("mod_title", data.mod_title.c_str());

	handler->endSectionWrite();
}

void read_info(pilot::FileHandler* handler, checkpoint::checkpoint_data& data)
{
	data.slot = handler->readStringOr("slot", "");
	data.mission_filename = handler->readStringOr("mission_filename", "");
	data.mission_modified = handler->readStringOr("mission_modified", "");
	data.mission_checksum = handler->readUIntOr("mission_checksum", 0);
	data.campaign = handler->readStringOr("campaign", "");
	data.pilot = handler->readStringOr("pilot", "");
	data.mod_title = handler->readStringOr("mod_title", "");
}

void write_clock(pilot::FileHandler* handler, const checkpoint::checkpoint_data& data)
{
	handler->startSectionWrite(Section::CheckpointClock);

	handler->writeInt("mission_time", static_cast<std::int32_t>(data.mission_time));
	// A fix is 16.16 and the microsecond count does not fit in 32 bits for a long mission, so
	// it goes out as two halves.
	handler->writeUInt("mission_time_us_hi", static_cast<std::uint32_t>(data.mission_time_microseconds >> 32));
	handler->writeUInt("mission_time_us_lo", static_cast<std::uint32_t>(data.mission_time_microseconds & 0xFFFFFFFFu));
	handler->writeInt("hud_timer_padding", data.hud_timer_padding);

	handler->endSectionWrite();
}

void read_clock(pilot::FileHandler* handler, checkpoint::checkpoint_data& data)
{
	data.mission_time = static_cast<fix>(handler->readIntOr("mission_time", 0));

	std::uint64_t hi = handler->readUIntOr("mission_time_us_hi", 0);
	std::uint64_t lo = handler->readUIntOr("mission_time_us_lo", 0);
	data.mission_time_microseconds = (hi << 32) | lo;

	data.hud_timer_padding = handler->readIntOr("hud_timer_padding", 0);
}

void write_subsystems(pilot::FileHandler* handler, const SCP_vector<checkpoint::subsystem_state>& subsystems)
{
	handler->startArrayWrite("subsystems", subsystems.size());
	for (const auto& subsys : subsystems) {
		handler->startSectionWrite(Section::Unnamed);

		handler->writeString("name", subsys.name.c_str());
		handler->writeInt("ordinal", subsys.ordinal);
		handler->writeString("sub_name", subsys.sub_name.c_str());
		handler->writeString("cargo_title", subsys.cargo_title.c_str());
		handler->writeString("turret_target", subsys.turret_target.c_str());

		write_string_list(handler, "flags", subsys.flags);
		write_float_map(handler, "floats", subsys.floats);
		write_int_map(handler, "ints", subsys.ints);

		handler->writeBool("has_weapons", subsys.has_weapons);
		if (subsys.has_weapons) {
			write_weapon_state(handler, subsys.weapons);
		}

		handler->endSectionWrite();
	}
	handler->endArrayWrite();
}

void read_subsystems(pilot::FileHandler* handler, SCP_vector<checkpoint::subsystem_state>& subsystems)
{
	subsystems.clear();

	if (!handler->hasField("subsystems")) {
		return;
	}

	auto count = handler->startArrayRead("subsystems");
	for (size_t i = 0; i < count; i++, handler->nextArraySection()) {
		checkpoint::subsystem_state subsys;

		subsys.name = handler->readStringOr("name", "");
		subsys.ordinal = handler->readIntOr("ordinal", 0);
		subsys.sub_name = handler->readStringOr("sub_name", "");
		subsys.cargo_title = handler->readStringOr("cargo_title", "");
		subsys.turret_target = handler->readStringOr("turret_target", "");

		read_string_list(handler, "flags", subsys.flags);
		read_float_map(handler, "floats", subsys.floats);
		read_int_map(handler, "ints", subsys.ints);

		subsys.has_weapons = handler->readBoolOr("has_weapons", false);
		if (subsys.has_weapons) {
			read_weapon_state(handler, subsys.weapons);
		}

		subsystems.push_back(std::move(subsys));
	}
	handler->endArrayRead();
}

void write_ships(pilot::FileHandler* handler, const checkpoint::checkpoint_data& data)
{
	handler->startSectionWrite(Section::CheckpointShips);

	handler->startArrayWrite("ships", data.ships.size());
	for (const auto& ship_data : data.ships) {
		handler->startSectionWrite(Section::Unnamed);

		handler->writeString("name", ship_data.name.c_str());
		handler->writeString("disposition", disposition_name(ship_data.disposition));

		if (ship_data.disposition == checkpoint::ShipDisposition::Present) {
			handler->writeString("class", ship_data.ship_class.c_str());
			handler->writeString("team", ship_data.team.c_str());
			handler->writeString("display_name", ship_data.display_name.c_str());
			handler->writeString("wing", ship_data.wing_name.c_str());
			handler->writeString("cargo_title", ship_data.cargo_title.c_str());
			handler->writeString("countermeasure_class", ship_data.countermeasure_class.c_str());
			handler->writeInt("cargo1", ship_data.cargo1);

			write_vector(handler, "pos_x", "pos_y", "pos_z", ship_data.pos);
			write_vector(handler, "fvec_x", "fvec_y", "fvec_z", ship_data.orient.vec.fvec);
			write_vector(handler, "uvec_x", "uvec_y", "uvec_z", ship_data.orient.vec.uvec);
			write_vector(handler, "rvec_x", "rvec_y", "rvec_z", ship_data.orient.vec.rvec);

			handler->writeFloat("hull", ship_data.hull);
			handler->writeFloat("max_hull", ship_data.max_hull);

			handler->startArrayWrite("shields", ship_data.shield_quadrants.size());
			for (float quadrant : ship_data.shield_quadrants) {
				handler->startSectionWrite(Section::Unnamed);
				handler->writeFloat("v", quadrant);
				handler->endSectionWrite();
			}
			handler->endArrayWrite();

			write_string_list(handler, "flags", ship_data.flags);
			write_string_list(handler, "object_flags", ship_data.object_flags);
			write_float_map(handler, "floats", ship_data.floats);
			write_int_map(handler, "ints", ship_data.ints);
			write_float_map(handler, "physics_floats", ship_data.physics_floats);
			write_vec_map(handler, "physics_vecs", ship_data.physics_vecs);

			write_subsystems(handler, ship_data.subsystems);
			write_weapon_state(handler, ship_data.weapons);
		} else {
			handler->writeInt("exit_time", static_cast<std::int32_t>(ship_data.exit_time));
		}

		handler->endSectionWrite();
	}
	handler->endArrayWrite();

	handler->endSectionWrite();
}

void read_ships(pilot::FileHandler* handler, checkpoint::checkpoint_data& data)
{
	data.ships.clear();

	if (!handler->hasField("ships")) {
		return;
	}

	auto count = handler->startArrayRead("ships");
	for (size_t i = 0; i < count; i++, handler->nextArraySection()) {
		checkpoint::ship_state ship_data;

		ship_data.name = handler->readStringOr("name", "");
		ship_data.disposition = disposition_value(handler->readStringOr("disposition", "present"));

		if (ship_data.disposition == checkpoint::ShipDisposition::Present) {
			ship_data.ship_class = handler->readStringOr("class", "");
			ship_data.team = handler->readStringOr("team", "");
			ship_data.display_name = handler->readStringOr("display_name", "");
			ship_data.wing_name = handler->readStringOr("wing", "");
			ship_data.cargo_title = handler->readStringOr("cargo_title", "");
			ship_data.countermeasure_class = handler->readStringOr("countermeasure_class", "");
			ship_data.cargo1 = static_cast<char>(handler->readIntOr("cargo1", 0));

			read_vector(handler, "pos_x", "pos_y", "pos_z", ship_data.pos);
			read_vector(handler, "fvec_x", "fvec_y", "fvec_z", ship_data.orient.vec.fvec);
			read_vector(handler, "uvec_x", "uvec_y", "uvec_z", ship_data.orient.vec.uvec);
			read_vector(handler, "rvec_x", "rvec_y", "rvec_z", ship_data.orient.vec.rvec);

			ship_data.hull = handler->readFloatOr("hull", 0.0f);
			ship_data.max_hull = handler->readFloatOr("max_hull", 0.0f);

			if (handler->hasField("shields")) {
				auto quadrants = handler->startArrayRead("shields");
				for (size_t q = 0; q < quadrants; q++, handler->nextArraySection()) {
					ship_data.shield_quadrants.push_back(handler->readFloatOr("v", 0.0f));
				}
				handler->endArrayRead();
			}

			read_string_list(handler, "flags", ship_data.flags);
			read_string_list(handler, "object_flags", ship_data.object_flags);
			read_float_map(handler, "floats", ship_data.floats);
			read_int_map(handler, "ints", ship_data.ints);
			read_float_map(handler, "physics_floats", ship_data.physics_floats);
			read_vec_map(handler, "physics_vecs", ship_data.physics_vecs);

			read_subsystems(handler, ship_data.subsystems);
			read_weapon_state(handler, ship_data.weapons);
		} else {
			ship_data.exit_time = static_cast<fix>(handler->readIntOr("exit_time", 0));
		}

		data.ships.push_back(std::move(ship_data));
	}
	handler->endArrayRead();
}

void write_wings(pilot::FileHandler* handler, const checkpoint::checkpoint_data& data)
{
	handler->startSectionWrite(Section::CheckpointWings);

	handler->startArrayWrite("wings", data.wings.size());
	for (const auto& wing_data : data.wings) {
		handler->startSectionWrite(Section::Unnamed);

		handler->writeString("name", wing_data.name.c_str());
		handler->writeInt("time_gone", static_cast<std::int32_t>(wing_data.time_gone));
		handler->writeInt("wave_delay_timestamp", wing_data.wave_delay_timestamp);
		write_int_map(handler, "ints", wing_data.ints);
		write_string_list(handler, "ships", wing_data.ship_names);

		handler->endSectionWrite();
	}
	handler->endArrayWrite();

	// SEXP variables ride along in this section rather than getting one of their own; they are
	// small and always wanted together with the rest of the mission's logical state.
	handler->startArrayWrite("variables", data.variables.size());
	for (const auto& var : data.variables) {
		handler->startSectionWrite(Section::Unnamed);
		handler->writeString("name", var.name.c_str());
		handler->writeBool("is_number", var.is_number);
		handler->writeString("value", var.value.c_str());
		handler->endSectionWrite();
	}
	handler->endArrayWrite();

	handler->endSectionWrite();
}

void read_wings(pilot::FileHandler* handler, checkpoint::checkpoint_data& data)
{
	data.wings.clear();
	data.variables.clear();

	if (handler->hasField("wings")) {
		auto count = handler->startArrayRead("wings");
		for (size_t i = 0; i < count; i++, handler->nextArraySection()) {
			checkpoint::wing_state wing_data;

			wing_data.name = handler->readStringOr("name", "");
			wing_data.time_gone = static_cast<fix>(handler->readIntOr("time_gone", 0));
			wing_data.wave_delay_timestamp = handler->readIntOr("wave_delay_timestamp", 0);
			read_int_map(handler, "ints", wing_data.ints);
			read_string_list(handler, "ships", wing_data.ship_names);

			data.wings.push_back(std::move(wing_data));
		}
		handler->endArrayRead();
	}

	if (handler->hasField("variables")) {
		auto count = handler->startArrayRead("variables");
		for (size_t i = 0; i < count; i++, handler->nextArraySection()) {
			checkpoint::variable_state var;

			var.name = handler->readStringOr("name", "");
			var.is_number = handler->readBoolOr("is_number", false);
			var.value = handler->readStringOr("value", "");

			data.variables.push_back(std::move(var));
		}
		handler->endArrayRead();
	}
}

void write_scoring(pilot::FileHandler* handler, const checkpoint::checkpoint_data& data)
{
	handler->startSectionWrite(Section::CheckpointScoring);

	write_int_map(handler, "ints", data.scoring.ints);
	write_int_map(handler, "class_kills", data.scoring.class_kills);

	handler->endSectionWrite();
}

void read_scoring(pilot::FileHandler* handler, checkpoint::checkpoint_data& data)
{
	read_int_map(handler, "ints", data.scoring.ints);
	read_int_map(handler, "class_kills", data.scoring.class_kills);
}

} // namespace

namespace checkpoint {

SCP_string checkpoint_filename(const SCP_string& slot)
{
	SCP_string filename;

	sprintf(filename,
	        "%s.%s.%s.%s.chk",
	        sanitize_for_filename(Player != nullptr ? Player->callsign : "").c_str(),
	        base_name(Campaign.filename).c_str(),
	        base_name(Game_current_mission_filename).c_str(),
	        sanitize_for_filename(slot).c_str());

	return filename;
}

bool checkpoint_write(const checkpoint_data& data)
{
	auto filename = checkpoint_filename(data.slot);

	cf_create_directory(CF_TYPE_CHECKPOINTS);

	auto fp = cfopen(filename.c_str(), "wb", CF_TYPE_CHECKPOINTS, false,
	                 CF_LOCATION_ROOT_USER | CF_LOCATION_ROOT_GAME | CF_LOCATION_TYPE_ROOT);
	if (fp == nullptr) {
		mprintf(("CHECKPOINT => Unable to open '%s' for writing!\n", filename.c_str()));
		return false;
	}

	// The handler takes ownership of the file and closes it in its destructor.
	std::unique_ptr<pilot::FileHandler> handler(new pilot::JSONFileHandler(fp, false));

	handler->writeUInt("signature", CHECKPOINT_FILE_ID);
	handler->writeUInt("version", CHECKPOINT_VERSION);

	handler->beginWritingSections();

	write_info(handler.get(), data);
	write_clock(handler.get(), data);
	write_ships(handler.get(), data);
	write_wings(handler.get(), data);
	write_scoring(handler.get(), data);

	handler->endWritingSections();

	handler->flush();

	mprintf(("CHECKPOINT => Wrote '%s' (%d ships, %d wings, %d variables)\n",
	         filename.c_str(),
	         static_cast<int>(data.ships.size()),
	         static_cast<int>(data.wings.size()),
	         static_cast<int>(data.variables.size())));

	return true;
}

bool checkpoint_read(const SCP_string& slot, checkpoint_data& data)
{
	data = checkpoint_data();

	auto filename = checkpoint_filename(slot);

	auto fp = cfopen(filename.c_str(), "rb", CF_TYPE_CHECKPOINTS, false,
	                 CF_LOCATION_ROOT_USER | CF_LOCATION_ROOT_GAME | CF_LOCATION_TYPE_ROOT);
	if (fp == nullptr) {
		mprintf(("CHECKPOINT => No checkpoint '%s'.\n", filename.c_str()));
		return false;
	}

	std::unique_ptr<pilot::FileHandler> handler;
	try {
		handler.reset(new pilot::JSONFileHandler(fp, true));
	} catch (const std::exception& e) {
		mprintf(("CHECKPOINT => Failed to parse '%s': %s\n", filename.c_str(), e.what()));
		return false;
	}

	if (handler->readUIntOr("signature", 0) != CHECKPOINT_FILE_ID) {
		mprintf(("CHECKPOINT => '%s' is not a checkpoint file!\n", filename.c_str()));
		return false;
	}

	data.version = static_cast<int>(handler->readUIntOr("version", 0));
	if (data.version > static_cast<int>(CHECKPOINT_VERSION)) {
		// Newer files may be structured in ways this build cannot interpret.  Individual
		// unknown fields and sections are fine, but a structural bump is not.
		mprintf(("CHECKPOINT => '%s' was written by a newer version (%d > %d); ignoring it.\n",
		         filename.c_str(),
		         data.version,
		         CHECKPOINT_VERSION));
		return false;
	}

	handler->beginSectionRead();
	while (handler->hasMoreSections()) {
		auto section_id = handler->nextSection();

		switch (section_id) {
		case Section::CheckpointInfo:
			read_info(handler.get(), data);
			break;

		case Section::CheckpointClock:
			read_clock(handler.get(), data);
			break;

		case Section::CheckpointShips:
			read_ships(handler.get(), data);
			break;

		case Section::CheckpointWings:
			read_wings(handler.get(), data);
			break;

		case Section::CheckpointScoring:
			read_scoring(handler.get(), data);
			break;

		default:
			// A section this build does not know about -- most likely written by a newer
			// engine.  Skipping it is the whole point of the sectioned layout.
			mprintf(("CHECKPOINT => Skipping unknown section 0x%04x.\n", static_cast<int>(section_id)));
			break;
		}
	}
	handler->endSectionRead();

	data.slot = slot;
	data.loaded = true;

	mprintf(("CHECKPOINT => Read '%s' (%d ships, %d wings, %d variables)\n",
	         filename.c_str(),
	         static_cast<int>(data.ships.size()),
	         static_cast<int>(data.wings.size()),
	         static_cast<int>(data.variables.size())));

	return true;
}

bool checkpoint_matches_current_mission(const checkpoint_data& data)
{
	if (stricmp(data.mission_filename.c_str(), Game_current_mission_filename) != 0) {
		return false;
	}

	// The checksum is the real test -- it changes whenever the mission file does, and an
	// edited mission invalidates the SEXP node indices the checkpoint depends on.
	if (data.mission_checksum != 0 && data.mission_checksum != Current_file_checksum) {
		return false;
	}

	return true;
}

void checkpoint_delete_file(const SCP_string& slot)
{
	auto filename = checkpoint_filename(slot);

	cf_delete(filename.c_str(), CF_TYPE_CHECKPOINTS, CF_LOCATION_ROOT_USER | CF_LOCATION_TYPE_ROOT);
}

} // namespace checkpoint
