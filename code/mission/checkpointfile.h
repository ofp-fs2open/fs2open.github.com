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

// Build the on-disk name for a slot of the currently loaded mission.  Slot names come from
// mission designers, so they are sanitised down to a safe filename character set.
SCP_string checkpoint_filename(const SCP_string& slot);

// The "<pilot>.<campaign>.<mission>." part that every checkpoint for a mission shares.  Pass an
// empty mission name for the mission that is currently loaded.  Enumeration and the
// single-file path both go through this so they cannot disagree about the naming scheme.
SCP_string checkpoint_file_prefix(const SCP_string& mission_name);

// Every slot saved for a mission, for the current pilot and campaign.  Empty mission name
// means the mission that is currently loaded.  Does not validate fingerprints -- a stale
// checkpoint is still a checkpoint that exists on disk and can be deleted.
SCP_vector<SCP_string> checkpoint_list_slots(const SCP_string& mission_name);

// Delete every checkpoint saved for a mission, for the current pilot and campaign.  Returns
// how many files were removed.
int checkpoint_delete_all(const SCP_string& mission_name);

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
