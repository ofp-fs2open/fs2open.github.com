
#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#pragma once

#include <cstdint>
#include <memory>

#include "globalincs/pstypes.h"

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
// This suppresses a GCC bug where it thinks that the Cutscenes from the enum class below shadows a global variable
#pragma GCC diagnostic ignored "-Wshadow"
#endif

// sections of a pilot file. includes both plr and csg sections
enum class Section {
	Invalid = -1,
	Unnamed = 0, //!< Special unnamed section
	Flags = 0x0001,
	Info = 0x0002,
	Loadout = 0x0003,
	Controls = 0x0004,
	Multiplayer = 0x0005,
	Scoring = 0x0006,
	ScoringMulti = 0x0007,
	Techroom = 0x0008,
	HUD = 0x0009,
	Settings = 0x0010,
	RedAlert = 0x0011,
	Variables = 0x0012,
	Missions = 0x0013,
	Cutscenes = 0x0014,
	LastMissions = 0x0015,
	Containers = 0x0016,

	// checkpoint file sections (see mission/missioncheckpoint.h)
	CheckpointInfo = 0x0017,
	CheckpointClock = 0x0018,
	CheckpointShips = 0x0019,
	CheckpointWings = 0x001A,
	CheckpointScoring = 0x001B
};
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

namespace pilot {
class FileHandler {
 public:
	virtual ~FileHandler() {}

	virtual void writeByte(const char* name, std::int8_t value) = 0;

	virtual void writeUByte(const char* name, std::uint8_t value) = 0;

	virtual void writeShort(const char* name, std::int16_t value) = 0;

	virtual void writeInt(const char* name, std::int32_t value) = 0;

	virtual void writeUInt(const char* name, std::uint32_t value) = 0;

	virtual void writeFloat(const char* name, float value) = 0;

	virtual void writeString(const char* name, const char* str) = 0;

	virtual void beginWritingSections() = 0;

	virtual void startSectionWrite(Section id) = 0;

	virtual void endSectionWrite() = 0;

	virtual void endWritingSections() = 0;

	virtual void startArrayWrite(const char* name, size_t size, bool short_length = false) = 0;

	virtual void endArrayWrite() = 0;

	virtual void flush() = 0;

	virtual std::int8_t readByte(const char* name) = 0;

	virtual std::uint8_t readUByte(const char* name) = 0;

	virtual std::int16_t readShort(const char* name) = 0;

	virtual std::int32_t readInt(const char* name) = 0;

	virtual std::uint32_t readUInt(const char* name) = 0;

	virtual float readFloat(const char* name) = 0;

	virtual SCP_string readString(const char* name) = 0;

	// Does the current element contain a value with this name?  Handlers whose format is
	// positional rather than self-describing (i.e. the binary handler) always answer false.
	virtual bool hasField(const char* name) = 0;

	// Tolerant reads: return the default when the field is absent.  These are what allow a
	// file written by a different build to be read without version gates -- a field added
	// later is simply missing, and a field removed later is simply ignored.  Only meaningful
	// on a self-describing handler; on a positional one they always yield the default.
	std::int8_t readByteOr(const char* name, std::int8_t def) { return hasField(name) ? readByte(name) : def; }

	std::uint8_t readUByteOr(const char* name, std::uint8_t def) { return hasField(name) ? readUByte(name) : def; }

	std::int16_t readShortOr(const char* name, std::int16_t def) { return hasField(name) ? readShort(name) : def; }

	std::int32_t readIntOr(const char* name, std::int32_t def) { return hasField(name) ? readInt(name) : def; }

	std::uint32_t readUIntOr(const char* name, std::uint32_t def) { return hasField(name) ? readUInt(name) : def; }

	float readFloatOr(const char* name, float def) { return hasField(name) ? readFloat(name) : def; }

	SCP_string readStringOr(const char* name, const SCP_string& def) { return hasField(name) ? readString(name) : def; }

	bool readBoolOr(const char* name, bool def) { return hasField(name) ? (readByte(name) != 0) : def; }

	void writeBool(const char* name, bool value) { writeByte(name, value ? (std::int8_t)1 : (std::int8_t)0); }

	virtual void readString(const char* name, char* dest, size_t max_size) {
		auto string = readString(name);
		Assertion(string.size() < max_size, "String in file is too long! Maximum is "
			SIZE_T_ARG
			" but the file contained a string with length "
			SIZE_T_ARG
			"!", max_size, string.size());
		strcpy_s(dest, max_size, string.c_str());
	}

	virtual void beginSectionRead() = 0;

	virtual bool hasMoreSections() = 0;

	virtual Section nextSection() = 0;

	virtual void endSectionRead() = 0;

	virtual size_t startArrayRead(const char* name, bool short_index = false) = 0;

	virtual void nextArraySection() = 0;

	virtual void endArrayRead() = 0;
};
}

#endif // FILEHANDLER_H
