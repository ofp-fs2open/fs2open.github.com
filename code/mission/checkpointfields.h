/*
 * Copyright (C) Freespace Open 2013.  All rights reserved.
 *
 * All source code herein is the property of Freespace Open. You may not sell
 * or otherwise commercially exploit the source or things you created based on the
 * source.
 */

#ifndef _CHECKPOINTFIELDS_H
#define _CHECKPOINTFIELDS_H

/*
 * Field registries for the mission checkpoint system.
 *
 * Each list below names the runtime-mutable fields of one struct that a checkpoint captures.
 * The lists are expanded twice -- once to write and once to read -- so adding a saved field
 * is a one-line change in exactly one place, and the write and read sides can never drift
 * apart.
 *
 * The reader always supplies the freshly-created value as the default, so a field that is
 * absent from the file (because it was added to the engine after the checkpoint was written)
 * simply keeps whatever the mission load produced.  Likewise a field the file still carries
 * but the engine no longer has just disappears from the list and is ignored on read.  That is
 * what lets a checkpoint survive an engine update without any version gate.
 *
 * Only add a field here if it is genuinely mutable during a mission.  Anything derived from
 * the ship class, the model, or the mission file is reproduced by the mission load itself and
 * must not be captured -- writing it back would defeat table changes and waste space.
 *
 * Do NOT add pointers, handles, or indices into runtime arrays (objnum, model_instance_num,
 * sound handles, subsystem pointers).  Those are meaningless across a mission reload and are
 * handled by name elsewhere in missioncheckpoint.cpp.
 */

// ------------------------------------------------------------------
// physics_info -- see code/physics/physics.h
// ------------------------------------------------------------------

// Scalars that describe how the object is currently moving.  The limits (max_vel, rotdamp,
// the various time constants) are all re-derived from the ship class on load and so are
// deliberately absent.
#define CKPT_PHYSICS_FLOATS(F)                                                                 \
	F(speed)                                                                                   \
	F(fspeed)                                                                                  \
	F(heading)                                                                                 \
	F(cur_glide_cap)

#define CKPT_PHYSICS_VECS(F)                                                                   \
	F(vel)                                                                                     \
	F(rotvel)                                                                                  \
	F(desired_vel)                                                                             \
	F(desired_rotvel)                                                                          \
	F(prev_ramp_vel)                                                                           \
	F(linear_thrust)                                                                           \
	F(rotational_thrust)                                                                       \
	F(acceleration)

// ------------------------------------------------------------------
// ship -- see code/ship/ship.h
// ------------------------------------------------------------------

// Consumables and other simple per-ship state.  Hull and shields live on the object, not the
// ship, and are handled separately.
#define CKPT_SHIP_FLOATS(F)                                                                    \
	F(ship_max_hull_strength)                                                                  \
	F(ship_max_shield_strength)                                                                \
	F(afterburner_fuel)                                                                        \
	F(weapon_energy)                                                                           \
	F(target_shields_delta)                                                                    \
	F(target_weapon_energy_delta)                                                              \
	F(total_damage_received)                                                                   \
	F(emp_intensity)                                                                           \
	F(emp_decr)                                                                                \
	F(tag_total)                                                                               \
	F(tag_left)                                                                                \
	F(level2_tag_total)                                                                        \
	F(level2_tag_left)

#define CKPT_SHIP_INTS(F)                                                                      \
	F(cmeasure_count)                                                                          \
	F(current_cmeasure)                                                                        \
	F(cmeasure_fire_stamp)                                                                     \
	F(shield_recharge_index)                                                                   \
	F(weapon_recharge_index)                                                                   \
	F(engine_recharge_index)                                                                   \
	F(next_manage_ets)                                                                         \
	F(escort_priority)                                                                         \
	F(respawn_priority)                                                                        \
	F(score)                                                                                   \
	F(hotkey)                                                                                  \
	F(persona_index)                                                                           \
	F(alt_type_index)                                                                          \
	F(callsign_index)                                                                          \
	F(ship_guardian_threshold)                                                                 \
	F(subsys_disrupted_flags)                                                                  \
	F(subsys_disrupted_check_timestamp)                                                        \
	F(wash_timestamp)                                                                          \
	F(lightning_stamp)                                                                         \
	F(num_swarm_missiles_to_fire)                                                              \
	F(next_swarm_fire)                                                                         \
	F(swarm_missile_bank)                                                                      \
	F(num_corkscrew_to_fire)                                                                   \
	F(corkscrew_missile_bank)                                                                  \
	F(next_corkscrew_fire)                                                                     \
	F(primitive_sensor_range)                                                                  \
	F(current_viewpoint)                                                                       \
	F(arrival_distance)                                                                        \
	F(arrival_delay)                                                                           \
	F(departure_delay)                                                                         \
	F(arrival_path_mask)                                                                       \
	F(departure_path_mask)

// ------------------------------------------------------------------
// ship_subsys -- see code/ship/ship.h
// ------------------------------------------------------------------

#define CKPT_SUBSYS_FLOATS(F)                                                                  \
	F(current_hits)                                                                            \
	F(max_hits)                                                                                \
	F(awacs_intensity)                                                                         \
	F(awacs_radius)                                                                            \
	F(turret_time_enemy_in_range)                                                              \
	F(turret_inaccuracy)                                                                       \
	F(optimum_range)                                                                           \
	F(favor_current_facing)                                                                    \
	F(points_to_target)                                                                        \
	F(base_rotation_rate_pct)                                                                  \
	F(gun_rotation_rate_pct)                                                                   \
	F(rof_scaler)

#define CKPT_SUBSYS_INTS(F)                                                                    \
	F(subsys_guardian_threshold)                                                               \
	F(turret_next_enemy_check_stamp)                                                           \
	F(turret_next_fire_stamp)                                                                  \
	F(turret_next_fire_pos)                                                                    \
	F(turret_pick_big_attack_point_timestamp)                                                  \
	F(turret_swarm_num)                                                                        \
	F(disruption_timestamp)                                                                    \
	F(rotation_timestamp)                                                                      \
	F(subsys_cargo_name)

// ------------------------------------------------------------------
// ship_weapon -- see code/ship/ship.h
// ------------------------------------------------------------------

// Bank contents are captured per bank (see checkpoint_weapon_bank); these are the fields that
// describe the weapon system as a whole.
#define CKPT_WEAPONS_INTS(F)                                                                   \
	F(current_primary_bank)                                                                    \
	F(current_secondary_bank)                                                                  \
	F(current_tertiary_bank)                                                                   \
	F(previous_primary_bank)                                                                   \
	F(previous_secondary_bank)                                                                 \
	F(next_tertiary_fire_stamp)                                                                \
	F(tertiary_bank_ammo)                                                                      \
	F(tertiary_bank_start_ammo)                                                                \
	F(tertiary_bank_capacity)                                                                  \
	F(tertiary_bank_rearm_time)                                                                \
	F(remote_detonaters_active)                                                                \
	F(detonate_weapon_time)

// ------------------------------------------------------------------
// wing -- see code/ship/ship.h
// ------------------------------------------------------------------

#define CKPT_WING_INTS(F)                                                                      \
	F(current_wave)                                                                            \
	F(total_arrived_count)                                                                     \
	F(current_count)                                                                           \
	F(total_destroyed)                                                                         \
	F(total_departed)                                                                          \
	F(total_vanished)                                                                          \
	F(red_alert_skipped_ships)                                                                 \
	F(arrival_distance)                                                                        \
	F(arrival_delay)                                                                           \
	F(departure_delay)                                                                         \
	F(wave_delay_min)                                                                          \
	F(wave_delay_max)

// ------------------------------------------------------------------
// scoring_struct, mission-scoped fields only -- see code/stats/scoring.h
// ------------------------------------------------------------------

// Per-ship-class kills (m_okKills) are indexed by ship class and so are written by class
// name separately; everything else in the mission scope is a plain counter.
#define CKPT_SCORING_INTS(F)                                                                   \
	F(m_score)                                                                                 \
	F(m_kill_count)                                                                            \
	F(m_kill_count_ok)                                                                         \
	F(m_assists)                                                                               \
	F(m_bonehead_kills)                                                                        \
	F(m_player_deaths)                                                                         \
	F(mp_shots_fired)                                                                          \
	F(mp_shots_hit)                                                                            \
	F(mp_bonehead_hits)                                                                        \
	F(ms_shots_fired)                                                                          \
	F(ms_shots_hit)                                                                            \
	F(ms_bonehead_hits)

#endif // _CHECKPOINTFIELDS_H
