/*
 * Copyright (C) Freespace Open 2013.  All rights reserved.
 *
 * All source code herein is the property of Freespace Open. You may not sell
 * or otherwise commercially exploit the source or things you created based on the
 * source.
 */

#ifndef _CHECKPOINTFILE_H
#define _CHECKPOINTFILE_H

#include "globalincs/pstypes.h"
#include "mission/missioncheckpoint.h"

namespace checkpoint {

// Written into every checkpoint so a stray file of another type is rejected outright.
static const unsigned int CHECKPOINT_FILE_ID = 0x5f4b4843;   // "CHK_"

// Bump ONLY for a structural change: a new or removed section, or a change to what an
// existing key means.  Adding or removing individual fields needs no bump -- the reader
// defaults anything the file does not carry, and ignores anything it does not recognise.
static const unsigned int CHECKPOINT_VERSION = 1;

// Build the on-disk name for a slot.  Slot names come from mission designers, so they are
// sanitised down to a safe filename character set.
SCP_string checkpoint_filename(const SCP_string& slot);

// Write a checkpoint.  Returns false and logs on failure; a failed write leaves any previous
// checkpoint for that slot untouched.
bool checkpoint_write(const checkpoint_data& data);

// Read a checkpoint.  Returns false if the file is missing, unreadable, or not a checkpoint.
// A mission fingerprint mismatch is NOT an error here -- it is reported through
// checkpoint_matches_current_mission() so the caller can decide what to do about it.
bool checkpoint_read(const SCP_string& slot, checkpoint_data& data);

// Does this checkpoint belong to the mission that is currently loaded?
bool checkpoint_matches_current_mission(const checkpoint_data& data);

// Delete a checkpoint file, if it exists.
void checkpoint_delete_file(const SCP_string& slot);

} // namespace checkpoint

#endif // _CHECKPOINTFILE_H
