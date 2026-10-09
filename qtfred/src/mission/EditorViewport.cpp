#include <globalincs/linklist.h>
#include <globalincs/systemvars.h>
#include <io/timer.h>
#include <object/object.h>
#include <render/3d.h>
#include <ship/ship.h>
#include "ui/ControlBindings.h"
#include "ui/Theme.h"

#include "object.h"

#include "EditorViewport.h"
#include "mission/commands/FredCommands.h"

#include <QUndoStack>
#include <QSettings>
#include <math/fvi.h>
#include <coordinate_points/coordinate_point.h>
#include <coordinate_points/coordinate_point_render.h>
#include <jumpnode/jumpnode.h>
#include <asteroid/asteroid.h>
#include <mission/missionparse.h>
#include <missioneditor/common.h>
#include <camera/camera.h>
#include <parse/sexp.h>
#include <nebula/volumetrics.h>
#include <prop/prop.h>
#include <FredApplication.h>
#include "mission/dialogs/BackgroundEditorDialogModel.h"
#include "mission/dialogs/VolumetricNebulaDialogModel.h"
#include "mission/dialogs/AsteroidEditorDialogModel.h"
#include "mission/dialogs/EnvEditCommand.h"

#include <algorithm>
#include <cfloat>

namespace {

constexpr auto SETTINGS_GROUP = "Preferences";

const float REDUCER = 100.0f;

void align_vector_to_axis(vec3d* v) {
	float x, y, z;

	x = v->xyz.x;
	if (x < 0) {
		x = -x;
	}

	y = v->xyz.y;
	if (y < 0) {
		y = -y;
	}

	z = v->xyz.z;
	if (z < 0) {
		z = -z;
	}

	if ((x > y) && (x > z)) { // x axis
		if (v->xyz.x < 0) // negative x
			vm_vec_make(v, -1.0f, 0.0f, 0.0f);
		else // positive x
			vm_vec_make(v, 1.0f, 0.0f, 0.0f);
	} else if (y > z) { // y axis
		if (v->xyz.y < 0) // negative y
			vm_vec_make(v, 0.0f, -1.0f, 0.0f);
		else // positive y
			vm_vec_make(v, 0.0f, 1.0f, 0.0f);
	} else { // z axis
		if (v->xyz.z < 0) // negative z
			vm_vec_make(v, 0.0f, 0.0f, -1.0f);
		else // positive z
			vm_vec_make(v, 0.0f, 0.0f, 1.0f);
	}
}
void verticalize_object(matrix* orient) {
	align_vector_to_axis(&orient->vec.fvec);
	align_vector_to_axis(&orient->vec.uvec);
	align_vector_to_axis(&orient->vec.rvec);
	vm_fix_matrix(orient); // just in case something odd occurs.
}

}

namespace fso::fred {

const char* EditorViewport::DefaultLayerName = "Default";

EditorViewport::ViewportControlLock::ViewportControlLock(EditorViewport* viewport) : _viewport(viewport)
{
	if (_viewport != nullptr) {
		_viewport->lockControls();
	}
}

EditorViewport::ViewportControlLock::~ViewportControlLock()
{
	if (_viewport != nullptr) {
		_viewport->unlockControls();
	}
}

EditorViewport::ViewportControlLock::ViewportControlLock(ViewportControlLock&& other) noexcept
	: _viewport(other._viewport)
{
	other._viewport = nullptr;
}

EditorViewport::ViewportControlLock& EditorViewport::ViewportControlLock::operator=(
	ViewportControlLock&& other) noexcept
{
	if (this == &other) {
		return *this;
	}

	if (_viewport != nullptr) {
		_viewport->unlockControls();
	}

	_viewport = other._viewport;
	other._viewport = nullptr;
	return *this;
}

EditorViewport::EditorViewport(Editor* in_editor, std::unique_ptr<FredRenderer>&& in_renderer) :
	_renderer(std::move(in_renderer)), editor(in_editor) {
	renderer = _renderer.get();

	_renderer->setViewport(this);

	vm_vec_make(&Constraint, 1.0f, 0.0f, 1.0f);
	vm_vec_make(&Anticonstraint, 0.0f, 1.0f, 0.0f);
	reset();
	resetObjectViewFov();

	_layerNames.emplace_back(DefaultLayerName);
	_layerVisibility.push_back(true);
	syncMissionLayerNames();

	loadSettings();

	fredApp->runAfterInit([this]() { initialSetup(); });
}

void EditorViewport::loadSettings() {
	QSettings settings;
	settings.beginGroup(SETTINGS_GROUP);
	toolbar_icon_size                  = settings.value("toolbar_icon_size",                  toolbar_icon_size).toInt();
	sexp_number_every_n                = settings.value("sexp_number_every_n",                sexp_number_every_n).toInt();
	Offer_autosave_recovery            = settings.value("offer_autosave_recovery",            Offer_autosave_recovery).toBool();
	autosave_interval_seconds         = settings.value("autosave_interval_seconds",          autosave_interval_seconds).toInt();
	Create_bak_on_save                 = settings.value("create_bak_on_save",                 Create_bak_on_save).toBool();
	undo_stack_depth                   = settings.value("undo_stack_depth",                   undo_stack_depth).toInt();
	Move_ships_when_undocking          = settings.value("move_ships_when_undocking",          Move_ships_when_undocking).toBool();
	Always_save_display_names          = settings.value("always_save_display_names",          Always_save_display_names).toBool();
	Error_checker_checks_potential_issues = settings.value("error_checker_checks_potential_issues", Error_checker_checks_potential_issues).toBool();
	Error_checker_apply_auto_corrections  = settings.value("error_checker_apply_auto_corrections",  Error_checker_apply_auto_corrections).toBool();
	Show_sexp_help_mission_events      = settings.value("show_sexp_help_mission_events",      Show_sexp_help_mission_events).toBool();
	Show_sexp_help_mission_goals       = settings.value("show_sexp_help_mission_goals",       Show_sexp_help_mission_goals).toBool();
	Show_sexp_help_mission_cutscenes   = settings.value("show_sexp_help_mission_cutscenes",   Show_sexp_help_mission_cutscenes).toBool();
	Show_sexp_help_ship_editor         = settings.value("show_sexp_help_ship_editor",         Show_sexp_help_ship_editor).toBool();
	Show_sexp_help_wing_editor         = settings.value("show_sexp_help_wing_editor",         Show_sexp_help_wing_editor).toBool();
	Show_sexp_help_prop_editor         = settings.value("show_sexp_help_prop_editor",         Show_sexp_help_prop_editor).toBool();
	// Handles its own group, since main.cpp reads it before the viewport exists.
	Theme_mode                         = readThemeModeSetting();
	{
		// Fall back to the pre-rename key so an existing choice carries over.
		const int legacyStyle = settings.value("sexp_data_menu_style", static_cast<int>(Data_menu_style)).toInt();
		const int rawStyle    = settings.value("data_menu_style", legacyStyle).toInt();
		if (rawStyle >= 0 && rawStyle <= static_cast<int>(DataMenuStyle::Searchable)) {
			Data_menu_style = static_cast<DataMenuStyle>(rawStyle);
		}
	}

	view.Universal_heading                 = settings.value("view_universal_heading",                 view.Universal_heading).toBool();
	view.Show_stars                        = settings.value("view_show_stars",                        view.Show_stars).toBool();
	view.Show_horizon                      = settings.value("view_show_horizon",                      view.Show_horizon).toBool();
	view.Show_grid                         = settings.value("view_show_grid",                         view.Show_grid).toBool();
	view.Show_distances                    = settings.value("view_show_distances",                    view.Show_distances).toBool();
	view.Show_coordinates                  = settings.value("view_show_coordinates",                  view.Show_coordinates).toBool();
	view.Show_outlines                     = settings.value("view_show_outlines",                     view.Show_outlines).toBool();
	view.Draw_outlines_on_selected_ships   = settings.value("view_draw_outlines_on_selected_ships",   view.Draw_outlines_on_selected_ships).toBool();
	view.Draw_outline_at_warpin_position   = settings.value("view_draw_outline_at_warpin_position",   view.Draw_outline_at_warpin_position).toBool();
	view.Show_grid_positions               = settings.value("view_show_grid_positions",               view.Show_grid_positions).toBool();
	view.Show_dock_points                  = settings.value("view_show_dock_points",                  view.Show_dock_points).toBool();
	view.Show_bay_paths                    = settings.value("view_show_bay_paths",                    view.Show_bay_paths).toBool();
	view.Show_starts                       = settings.value("view_show_starts",                       view.Show_starts).toBool();
	view.Show_ships                        = settings.value("view_show_ships",                        view.Show_ships).toBool();
	view.Show_ship_info                    = settings.value("view_show_ship_info",                    view.Show_ship_info).toBool();
	view.Show_ship_models                  = settings.value("view_show_ship_models",                  view.Show_ship_models).toBool();
	view.Show_paths_fred                   = settings.value("view_show_paths_fred",                   view.Show_paths_fred).toBool();
	view.Lighting_on                       = settings.value("view_lighting_on",                       view.Lighting_on).toBool();
	view.FullDetail                        = settings.value("view_full_detail",                       view.FullDetail).toBool();
	view.Show_waypoints                    = settings.value("view_show_waypoints",                    view.Show_waypoints).toBool();
	view.Show_coordinate_points            = settings.value("view_show_coordinate_points",            view.Show_coordinate_points).toBool();
	view.Show_compass                      = settings.value("view_show_compass",                      view.Show_compass).toBool();
	view.Show_camera_gizmo                 = settings.value("view_show_camera_gizmo",                 view.Show_camera_gizmo).toBool();
	view.Highlight_selectable_subsys       = settings.value("view_highlight_selectable_subsys",       view.Highlight_selectable_subsys).toBool();
	view.Outline_lod                       = settings.value("view_outline_lod",                       view.Outline_lod).toInt();
	view.Label_font_scale                  = settings.value("view_label_font_scale",                  view.Label_font_scale).toFloat();
	camera.setInvertOrbitX(settings.value("camera_invert_orbit_x", camera.getInvertOrbitX()).toBool());
	camera.setInvertOrbitY(settings.value("camera_invert_orbit_y", camera.getInvertOrbitY()).toBool());
	settings.endGroup();
}

void EditorViewport::saveSettings() const {
	QSettings settings;
	settings.beginGroup(SETTINGS_GROUP);
	settings.setValue("toolbar_icon_size",                   toolbar_icon_size);
	settings.setValue("sexp_number_every_n",                 sexp_number_every_n);
	settings.setValue("offer_autosave_recovery",             Offer_autosave_recovery);
	settings.setValue("autosave_interval_seconds",          autosave_interval_seconds);
	settings.setValue("create_bak_on_save",                  Create_bak_on_save);
	settings.setValue("undo_stack_depth",                    undo_stack_depth);
	settings.setValue("move_ships_when_undocking",           Move_ships_when_undocking);
	settings.setValue("always_save_display_names",           Always_save_display_names);
	settings.setValue("error_checker_checks_potential_issues", Error_checker_checks_potential_issues);
	settings.setValue("error_checker_apply_auto_corrections",  Error_checker_apply_auto_corrections);
	settings.setValue("show_sexp_help_mission_events",       Show_sexp_help_mission_events);
	settings.setValue("show_sexp_help_mission_goals",        Show_sexp_help_mission_goals);
	settings.setValue("show_sexp_help_mission_cutscenes",    Show_sexp_help_mission_cutscenes);
	settings.setValue("show_sexp_help_ship_editor",          Show_sexp_help_ship_editor);
	settings.setValue("show_sexp_help_wing_editor",          Show_sexp_help_wing_editor);
	settings.setValue("show_sexp_help_prop_editor",          Show_sexp_help_prop_editor);
	writeThemeModeSetting(Theme_mode);
	settings.setValue("data_menu_style",                     static_cast<int>(Data_menu_style));

	settings.setValue("view_universal_heading",                 view.Universal_heading);
	settings.setValue("view_show_stars",                        view.Show_stars);
	settings.setValue("view_show_horizon",                      view.Show_horizon);
	settings.setValue("view_show_grid",                         view.Show_grid);
	settings.setValue("view_show_distances",                    view.Show_distances);
	settings.setValue("view_show_coordinates",                  view.Show_coordinates);
	settings.setValue("view_show_outlines",                     view.Show_outlines);
	settings.setValue("view_draw_outlines_on_selected_ships",   view.Draw_outlines_on_selected_ships);
	settings.setValue("view_draw_outline_at_warpin_position",   view.Draw_outline_at_warpin_position);
	settings.setValue("view_show_grid_positions",               view.Show_grid_positions);
	settings.setValue("view_show_dock_points",                  view.Show_dock_points);
	settings.setValue("view_show_bay_paths",                    view.Show_bay_paths);
	settings.setValue("view_show_starts",                       view.Show_starts);
	settings.setValue("view_show_ships",                        view.Show_ships);
	settings.setValue("view_show_ship_info",                    view.Show_ship_info);
	settings.setValue("view_show_ship_models",                  view.Show_ship_models);
	settings.setValue("view_show_paths_fred",                   view.Show_paths_fred);
	settings.setValue("view_lighting_on",                       view.Lighting_on);
	settings.setValue("view_full_detail",                       view.FullDetail);
	settings.setValue("view_show_waypoints",                    view.Show_waypoints);
	settings.setValue("view_show_coordinate_points",            view.Show_coordinate_points);
	settings.setValue("view_show_compass",                      view.Show_compass);
	settings.setValue("view_show_camera_gizmo",                 view.Show_camera_gizmo);
	settings.setValue("view_highlight_selectable_subsys",       view.Highlight_selectable_subsys);
	settings.setValue("view_outline_lod",                       view.Outline_lod);
	settings.setValue("view_label_font_scale",                  view.Label_font_scale);
	settings.setValue("camera_invert_orbit_x",                  camera.getInvertOrbitX());
	settings.setValue("camera_invert_orbit_y",                  camera.getInvertOrbitY());
	settings.endGroup();
}
void EditorViewport::needsUpdate() {
	_renderer->scheduleUpdate();
}

bool EditorViewport::areControlsLocked() const
{
	return _controlLockCount > 0;
}

EditorViewport::ViewportControlLock EditorViewport::acquireControlLock()
{
	return ViewportControlLock(this);
}

void EditorViewport::lockControls()
{
	++_controlLockCount;
}

void EditorViewport::unlockControls()
{
	Assertion(_controlLockCount > 0, "Mismatched unlock on EditorViewport controls");
	--_controlLockCount;
}

bool EditorViewport::incMissionTime() {
	const fix MAX_FRAMETIME = (F1_0 / 4);
	const fix MIN_FRAMETIME = (F1_0 / 120);

	fix thistime = timer_get_fixed_seconds();
	fix time_diff;
	if (!_lasttime) {
		time_diff = F1_0 / 30;
	} else {
		time_diff = thistime - _lasttime;
	}

	if (time_diff > MAX_FRAMETIME) {
		time_diff = MAX_FRAMETIME;
	} else if (time_diff < MIN_FRAMETIME) {
		return false;
	}

	Frametime = time_diff;
	Missiontime += Frametime;
	_lasttime = thistime;

	return true;
}
bool EditorViewport::isObjectSelectable(const object* ptr) const {
	if (ptr->flags.any_of(Object::Object_Flags::Hidden, Object::Object_Flags::Locked_from_editing))
		return false;
	if (!isObjectVisibleInLayer(ptr))
		return false;

	switch (ptr->type) {
	case OBJ_WAYPOINT:
		return Show_waypoints;
	case OBJ_START:
		return view.Show_starts && view.Show_ships;
	case OBJ_SHIP:
		return view.Show_ships && view.Show_iff[Ships[ptr->instance].team];
	case OBJ_PROP:
		return view.Show_props;
	case OBJ_JUMP_NODE:
		return view.Show_jump_nodes;
	case OBJ_COORDINATE_POINT:
		return view.Show_coordinate_points;
	default:
		return true;
	}
}

void EditorViewport::select_objects(const Marking_box& box) {
	int x, y, valid;
	vertex v;
	object* ptr;

	// Copy this so we can modify it
	auto marking_box = box;

	if (marking_box.x1 > marking_box.x2) {
		x = marking_box.x1;
		marking_box.x1 = marking_box.x2;
		marking_box.x2 = x;
	}

	if (marking_box.y1 > marking_box.y2) {
		y = marking_box.y1;
		marking_box.y1 = marking_box.y2;
		marking_box.y2 = y;
	}

	ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list)) {
		Assert(ptr->type != OBJ_NONE);
		valid = isObjectSelectable(ptr) ? 1 : 0;

		g3_rotate_vertex(&v, &ptr->pos);
		if (!(v.codes & CC_BEHIND) && valid) {
			if (!(g3_project_vertex(&v) & PF_OVERFLOW)) {
				x = (int) v.screen.xyw.x;
				y = (int) v.screen.xyw.y;

				if (x >= marking_box.x1 && x <= marking_box.x2 && y >= marking_box.y1 && y <= marking_box.y2) {
					if (ptr->flags[Object::Object_Flags::Marked]) {
						editor->unmarkObject(OBJ_INDEX(ptr));
					} else {
						editor->markObject(OBJ_INDEX(ptr));
					}
				}
			}
		}

		ptr = GET_NEXT(ptr);
	}

	needsUpdate();
}

float EditorViewport::viewFov() const {
	// Any viewpoint other than the basic editor camera shows what the game would
	if (camera.getViewpoint() == CutsceneCameraViewpoint) {
		CameraSexpPreview preview;
		if (cameraPreview(&preview)) {
			// A sexp can ask for anything; keep the projection sane (it is zoom * PROJ_FOV_FACTOR)
			return std::clamp(preview.shot.fov, 0.05f, 2.2f);
		}
	}
	return (camera.getViewpoint() != 0) ? _objectViewFov : FRED_DEFAULT_HTL_FOV;
}

void EditorViewport::setObjectViewFov(float fov) {
	_objectViewFov = std::clamp(fov, MinObjectViewFov, MaxObjectViewFov);
	needsUpdate();
}

void EditorViewport::resetObjectViewFov() {
	// The in-game FOV: the Graphics.FOV option (0.75 unless the mod changes its default)
	_objectViewFov = std::clamp(g3_get_hfov(VIEWER_ZOOM_DEFAULT), MinObjectViewFov, MaxObjectViewFov);
}

void EditorViewport::reset() {
	camera.resetView();
	camera.resetViewPhysics();
	The_grid = create_default_grid();
	maybe_create_new_grid(The_grid, &camera.view_pos, &camera.view_orient, 1);
}

///////////////////////////////////////////////////
void EditorViewport::process_system_keys() {
	auto& bindings = ControlBindings::instance();
	if (areControlsLocked()) {
		return;
	}
	if (bindings.takeTriggered(ControlAction::ToggleSelectionLock)) {
		Selection_lock = !Selection_lock;
	}

}

void EditorViewport::game_do_frame(const int cur_object_index) {
	int cmode;
	vec3d control_pos;
	object* objp;
	matrix control_orient;

	if (!incMissionTime()) {
		return;
	}

	// sync all timestamps across the entire frame
	timer_start_frame();

	if ((camera.getViewpoint() == 1) && !query_valid_object(camera.getViewObj())) {
		camera.setViewpoint(0);
	}
	CameraSexpPreview cameraView;
	if ((camera.getViewpoint() == CutsceneCameraViewpoint) && !cameraPreview(&cameraView)) {
		// The camera sexp was deselected or its dialog closed
		camera.setViewpoint(0);
		needsUpdate();
	}
	if (_camFlying && camera.getViewpoint() != CutsceneCameraViewpoint) {
		commitCameraFly(); // left the camera's view mid-flight
	}
	if (_camPlayback && camera.getViewpoint() != CutsceneCameraViewpoint) {
		stopCameraPlayback(); // playback only lives while looking through the camera
	}

	process_system_keys();
	const auto controlsLocked = areControlsLocked();
	cmode = camera.getControlMode();
	if ((camera.getViewpoint() == 1) && !cmode) {
		cmode = 2;
	}
	if ((camera.getViewpoint() == CutsceneCameraViewpoint) && !cmode) {
		cmode = 3;
	}

	control_pos = Last_control_pos;
	control_orient = Last_control_orient;

	switch (cmode) {
	case 0: //	Control the viewer's location and orientation
		if (!controlsLocked && camera.processControls(&camera.view_pos, &camera.view_orient, f2fl(Frametime), true)) {
			needsUpdate();
		}
		control_pos = camera.view_pos;
		control_orient = camera.view_orient;
		break;

	case 2: // Control viewpoint object
		if (!controlsLocked && !Objects[camera.getViewObj()].flags[Object::Object_Flags::Locked_from_editing] &&
			!Editor::isTransformHeld(camera.getViewObj())) {
			object* viewed = &Objects[camera.getViewObj()];
			const vec3d old_pos = viewed->pos;
			const matrix old_orient = viewed->orient;
			const bool input = camera.processControls(&viewed->pos, &viewed->orient, f2fl(Frametime), false);
			noteObjectFly(viewed, old_pos, old_orient, input);
			object_moved(&Objects[camera.getViewObj()]);
			control_pos = Objects[camera.getViewObj()].pos;
			control_orient = Objects[camera.getViewObj()].orient;
		}
		break;

	case 1: //	Control the current object's location and orientation
		if (!controlsLocked && query_valid_object(cur_object_index) && !Objects[cur_object_index].flags[Object::Object_Flags::Locked_from_editing] &&
			!Editor::isTransformHeld(cur_object_index)) {
			object* leader = &Objects[cur_object_index];
			const vec3d leader_old_pos = leader->pos;
			const matrix leader_old_orient = leader->orient;

			const bool input = camera.processControls(&leader->pos, &leader->orient, f2fl(Frametime), false);
			control_pos = leader->pos;
			control_orient = leader->orient;
			noteObjectFly(leader, leader_old_pos, leader_old_orient, input); // before the followers move

			follow_leader(leader, leader_old_pos, leader_old_orient, camera.getLastRotMat());

			objp = GET_FIRST(&obj_used_list);
			while (objp != END_OF_LIST(&obj_used_list)) {
				if (objp->flags[Object::Object_Flags::Marked]) {
					object_moved(objp);
				}

				objp = GET_NEXT(objp);
			}

			editor->missionChanged();
		}

		break;

	case 3: // Looking through a cutscene camera: the controls fly it only the ways the selected sexp
	        // can take (a position moves, a rotation or facing turns), like dragging its handle,
	        // and the sexp takes the new place once the controls are still
		if (!controlsLocked) {
			vec3d pos = cameraView.shot.pos;
			matrix orient = cameraView.shot.orient;
			const bool canMove = cameraView.op == OP_CUTSCENES_SET_CAMERA_POSITION && cameraView.pointNodes[0] >= 0;
			const bool canTurn = (cameraView.op == OP_CUTSCENES_SET_CAMERA_ROTATION && cameraView.angleNodes[0] >= 0) ||
				(cameraView.op == OP_CUTSCENES_SET_CAMERA_FACING && cameraView.pointNodes[0] >= 0);
			if ((canMove || canTurn) && _camPlayback) {
				// Flying ends playback; the flight starts next frame from the selected sexp's pose
				vec3d probePos = pos;
				matrix probeOrient = orient;
				if (camera.processControls(&probePos, &probeOrient, f2fl(Frametime), true)) {
					stopCameraPlayback();
					camera.resetViewPhysics();
				}
			} else if ((canMove || canTurn) && camera.processControls(&pos, &orient, f2fl(Frametime), true)) {
				if (!canMove)
					pos = cameraView.shot.pos;
				if (!canTurn)
					orient = cameraView.shot.orient;
				_camFlying = true;
				_camFlyPos = pos;
				_camFlyOrient = orient;
				_camFlyLastMove = timer_get_milliseconds();
				cameraView.shot.pos = pos;
				cameraView.shot.orient = orient;
				needsUpdate();
			}
		}
		if (_camFlying && timer_get_milliseconds() - _camFlyLastMove >= 300) {
			commitCameraFly();
		}
		break;

	default:
		Assert(0);
	}

	// one undo step per flight, once the object has settled (or the controls went elsewhere)
	if (_objFlying && (timer_get_milliseconds() - _objFlyLastMove >= 300 || (cmode != 1 && cmode != 2))) {
		commitObjectFly();
	}

	if (camera.getLookatMode() && query_valid_object(cur_object_index)) {
		float dist;

		dist = vm_vec_dist(&camera.view_pos, &Objects[cur_object_index].pos);
		vm_vec_scale_add(&camera.view_pos, &Objects[cur_object_index].pos, &camera.view_orient.vec.fvec, -dist);
	}

	switch (camera.getViewpoint()) {
	case 0:
		camera.eye_pos = camera.view_pos;
		camera.eye_orient = camera.view_orient;
		break;

	case 1:
		camera.eye_pos = Objects[camera.getViewObj()].pos;
		camera.eye_orient = Objects[camera.getViewObj()].orient;
		break;

	case CutsceneCameraViewpoint:
		camera.eye_pos = cameraView.shot.pos;
		camera.eye_orient = cameraView.shot.orient;
		break;

	default:
		Assert(0);
	}

	maybe_create_new_grid(The_grid, &camera.eye_pos, &camera.eye_orient);

	if (Cursor_over != Last_cursor_over) {
		Last_cursor_over = Cursor_over;
		needsUpdate();
	}

	// Same change-detection for the hovered viewport handle, so its hover
	// balloon appears/updates on mouse-move exactly like an object's Cursor_over
	// infobox (mouse-move sets the state but does not itself schedule a frame).
	if (_hovered_handle.group_index != _last_hovered_handle.group_index ||
		_hovered_handle.handle_index != _last_hovered_handle.handle_index) {
		_last_hovered_handle = _hovered_handle;
		needsUpdate();
	}

	// redraw screen if controlled object moved or rotated
	if (vm_vec_cmp(&control_pos, &Last_control_pos) || vm_matrix_cmp(&control_orient, &Last_control_orient)) {
		needsUpdate();
		Last_control_pos = control_pos;
		Last_control_orient = control_orient;
	}

	// redraw screen if current viewpoint moved or rotated
	if (camera.hasEyeMoved()) {
		needsUpdate();
	}
}

void EditorViewport::level_controlled() {
	int cmode, count = 0;
	object* objp;

	cmode = camera.getControlMode();
	if ((camera.getViewpoint() == 1) && !cmode) {
		cmode = 2;
	}
	if ((camera.getViewpoint() == CutsceneCameraViewpoint) && !cmode) {
		return; // the camera sexps place it
	}

	switch (cmode) {
	case 0: //	Control the viewer's location and orientation
		level_object(&camera.view_orient);
		break;

	case 2: // Control viewpoint object
		if (!Objects[camera.getViewObj()].flags[Object::Object_Flags::Locked_from_editing] &&
			!Editor::isTransformHeld(camera.getViewObj())) {
			level_object(&Objects[camera.getViewObj()].orient);
			object_moved(&Objects[camera.getViewObj()]);
			///! \todo Notify.
			editor->missionChanged();
		}
		break;

	case 1: //	Control the current object's location and orientation
		objp = GET_FIRST(&obj_used_list);
		while (objp != END_OF_LIST(&obj_used_list)) {
			if (objp->flags[Object::Object_Flags::Marked] && !Editor::isTransformHeld(OBJ_INDEX(objp))) {
				level_object(&objp->orient);
			}

			objp = GET_NEXT(objp);
		}

		objp = GET_FIRST(&obj_used_list);
		while (objp != END_OF_LIST(&obj_used_list)) {
			if (objp->flags[Object::Object_Flags::Marked]) {
				object_moved(objp);
				count++;
			}

			objp = GET_NEXT(objp);
		}

		///! \todo Notify.
		if (count) {
			editor->missionChanged();
		}

		break;
	}

	return;
}

void EditorViewport::verticalize_controlled() {
	int cmode, count = 0;
	object* objp;

	cmode = camera.getControlMode();
	if ((camera.getViewpoint() == 1) && !cmode) {
		cmode = 2;
	}
	if ((camera.getViewpoint() == CutsceneCameraViewpoint) && !cmode) {
		return; // the camera sexps place it
	}

	switch (cmode) {
	case 0: //	Control the viewer's location and orientation
		verticalize_object(&camera.view_orient);
		break;

	case 2: // Control viewpoint object
		if (!Objects[camera.getViewObj()].flags[Object::Object_Flags::Locked_from_editing] &&
			!Editor::isTransformHeld(camera.getViewObj())) {
			verticalize_object(&Objects[camera.getViewObj()].orient);
			object_moved(&Objects[camera.getViewObj()]);
			///! \todo notify.
			editor->missionChanged();
		}
		break;

	case 1: //	Control the current object's location and orientation
		objp = GET_FIRST(&obj_used_list);
		while (objp != END_OF_LIST(&obj_used_list)) {
			if (objp->flags[Object::Object_Flags::Marked] && !Editor::isTransformHeld(OBJ_INDEX(objp))) {
				verticalize_object(&objp->orient);
			}

			objp = GET_NEXT(objp);
		}

		objp = GET_FIRST(&obj_used_list);
		while (objp != END_OF_LIST(&obj_used_list)) {
			if (objp->flags[Object::Object_Flags::Marked]) {
				object_moved(objp);
				count++;
			}

			objp = GET_NEXT(objp);
		}

		///! \todo Notify.
		if (count) {
			editor->missionChanged();
		}

		break;
	}

	return;
}

void EditorViewport::level_object(matrix* orient) {
	vec3d u;

	u = orient->vec.uvec = The_grid->gmatrix.vec.uvec;
	if (u.xyz.x) // y-z plane
	{
		orient->vec.fvec.xyz.x = orient->vec.rvec.xyz.x = 0.0f;
	} else if (u.xyz.y) { // x-z plane
		orient->vec.fvec.xyz.y = orient->vec.rvec.xyz.y = 0.0f;
	} else if (u.xyz.z) { // x-y plane
		orient->vec.fvec.xyz.z = orient->vec.rvec.xyz.z = 0.0f;
	}

	vm_fix_matrix(orient);
}

vec3d EditorViewport::orbitCameraGetPivot()
{
	vec3d pivot;

	if (query_valid_object(editor->currentObject)) {
		// Pivot on current object
		pivot = Objects[editor->currentObject].pos;
	} else if (!The_grid) {
		// Pivot on the origin, if no grid
		pivot = ZERO_VECTOR;
	} else {
		// Intersect camera forward ray with the grid plane
		vec3d *grid_normal = &The_grid->gmatrix.vec.uvec;
		float denom = vm_vec_dot(grid_normal, &camera.view_orient.vec.fvec);

		if (fl_abs(denom) > 0.0001f) {
			float t = -(vm_vec_dot(grid_normal, &camera.view_pos) + The_grid->planeD) / denom;
			if (t > 0.0f) {
				vm_vec_scale_add(&pivot, &camera.view_pos, &camera.view_orient.vec.fvec, t);
			} else {
				pivot = The_grid->center;
			}
		} else {
			// Camera is parallel to grid plane; fall back to grid center
			pivot = The_grid->center;
		}
	}
	return pivot;
}

int EditorViewport::object_check_collision(object* objp, vec3d* p0, vec3d* p1, vec3d* hitpos) {
	mc_info mc;

	if (objp->type == OBJ_NONE) {
		return 0;
	}

	if ((objp->type == OBJ_WAYPOINT) && !view.Show_waypoints) {
		return 0;
	}

	if ((objp->type == OBJ_START) && !view.Show_starts) {
		return 0;
	}

	if ((objp->type == OBJ_SHIP) || (objp->type == OBJ_START)) {
		if (!view.Show_ships) {
			return 0;
		}

		if (!view.Show_iff[Ships[objp->instance].team]) {
			return 0;
		}
	}

	if ((objp->type == OBJ_PROP) && !view.Show_props) {
		return 0;
	}

	if ((objp->type == OBJ_JUMP_NODE) && !view.Show_jump_nodes) {
		return 0;
	}

	if ((objp->type == OBJ_COORDINATE_POINT) && !view.Show_coordinate_points) {
		return 0;
	}

	if (objp->flags.any_of(Object::Object_Flags::Hidden, Object::Object_Flags::Locked_from_editing)) {
		return 0;
	}
	if (!isObjectVisibleInLayer(objp)) {
		return 0;
	}

	mc.model_instance_num = -1;

	if ((view.Show_ship_models || view.Show_outlines) && (objp->type == OBJ_SHIP || objp->type == OBJ_START)) {
		auto& shp = Ships[objp->instance];
		mc.model_num = Ship_info[shp.ship_info_index].model_num;			// Fill in the model to check
		mc.model_instance_num = shp.model_instance_num;
	} else if ((view.Show_ship_models || view.Show_outlines) && (objp->type == OBJ_PROP)) {
		auto& prp = Props[objp->instance].value();
		mc.model_num = Prop_info[prp.prop_info_index].model_num;			// Fill in the model to check
		mc.model_instance_num = prp.model_instance_num;
	} else {
		return fvi_ray_sphere(hitpos, p0, p1, &objp->pos, (objp->radius > 0.1f) ? objp->radius : LOLLIPOP_SIZE);
	}
	mc.orient = &objp->orient; // The object's orient
	mc.pos = &objp->pos; // The object's position
	mc.p0 = p0; // Point 1 of ray to check
	mc.p1 = p1; // Point 2 of ray to check
	mc.flags = MC_CHECK_MODEL | MC_CHECK_RAY; // flags
	model_collide(&mc);
	*hitpos = mc.hit_point_world;
	if (mc.num_hits < 1) {
		// check shield
		mc.orient = &objp->orient; // The object's orient
		mc.pos = &objp->pos; // The object's position
		mc.p0 = p0; // Point 1 of ray to check
		mc.p1 = p1; // Point 2 of ray to check
		mc.flags = MC_CHECK_SHIELD; // flags
		model_collide(&mc);
		*hitpos = mc.hit_point_world;
	}

	return mc.num_hits;
}

int EditorViewport::select_object(int cx, int cy) {
	int best = -1;
	double dist, best_dist = 9e99;
	vec3d p0, p1, v, hitpos;
	vertex vt;

	/*	gr_reset_clip();
	g3_start_frame(0); ////////////////
	g3_set_view_matrix(&eye_pos, &eye_orient, 0.5f);*/

	// Mouse events can arrive when no frame is active (G3_count == 0) or when
	// another renderer, such as the briefing map widget, has altered the frame state
	// In those cases we cannot do a valid screen to world conversion
	if (g3_in_frame() != 1) {
		return -1;
	}

	//	Get 3d vector specified by mouse cursor location.
	g3_point_to_vec(&v, cx, cy);

	//	g3_end_frame();
	if (!v.xyz.x && !v.xyz.y && !v.xyz.z) { // zero vector {
		return -1;
	}

	p0 = camera.view_pos;
	vm_vec_scale_add(&p1, &p0, &v, 100.0f);

	for (auto objp = GET_FIRST(&obj_used_list); objp != END_OF_LIST(&obj_used_list); objp = GET_NEXT(objp)) {
		if (object_check_collision(objp, &p0, &p1, &hitpos)) {
			hitpos.xyz.x = objp->pos.xyz.x - camera.view_pos.xyz.x;
			hitpos.xyz.y = objp->pos.xyz.y - camera.view_pos.xyz.y;
			hitpos.xyz.z = objp->pos.xyz.z - camera.view_pos.xyz.z;
			dist = hitpos.xyz.x * hitpos.xyz.x + hitpos.xyz.y * hitpos.xyz.y + hitpos.xyz.z * hitpos.xyz.z;
			if (dist < best_dist) {
				best = OBJ_INDEX(objp);
				best_dist = dist;
			}
		}
	}

	if (best >= 0) {
		if ((Selection_lock && !Objects[best].flags[Object::Object_Flags::Marked]) || Objects[best].flags[Object::Object_Flags::Locked_from_editing]) {
			return -1;
		}
		return best;
	}

	for (auto objp = GET_FIRST(&obj_used_list); objp != END_OF_LIST(&obj_used_list); objp = GET_NEXT(objp)) {
		if (!isObjectVisibleInLayer(objp)) {
			continue;
		}
		g3_rotate_vertex(&vt, &objp->pos);
		if (!(vt.codes & CC_BEHIND)) {
			if (!(g3_project_vertex(&vt) & PF_OVERFLOW)) {
				hitpos.xyz.x = vt.screen.xyw.x - cx;
				hitpos.xyz.y = vt.screen.xyw.y - cy;
				dist = hitpos.xyz.x * hitpos.xyz.x + hitpos.xyz.y * hitpos.xyz.y;

				// Coordinate points render at a per-instance scaled size; widen the click
				// tolerance to match the visible shape rather than using the default 2.8-pixel
				// grace area.
				double threshold = 8.0;
				if (objp->type == OBJ_COORDINATE_POINT) {
					auto* cp = find_coordinate_point_by_objnum(OBJ_INDEX(objp));
					if (cp != nullptr) {
						const float world_radius = get_coordinate_point_world_radius(*cp, camera.eye_pos);
						vec3d offset_pos = objp->pos;
						vm_vec_scale_add2(&offset_pos, &camera.eye_orient.vec.rvec, world_radius);

						vertex v_offset;
						g3_rotate_vertex(&v_offset, &offset_pos);
						if (!(v_offset.codes & CC_BEHIND) &&
							!(g3_project_vertex(&v_offset) & PF_OVERFLOW)) {
							const double dx = v_offset.screen.xyw.x - vt.screen.xyw.x;
							const double dy = v_offset.screen.xyw.y - vt.screen.xyw.y;
							const double radius_sq = dx * dx + dy * dy;
							// Keep at least the default tolerance so tiny / very-distant points
							// stay clickable.
							threshold = std::max(8.0, radius_sq);
						}
					}
				}

				if ((dist < threshold) && (dist < best_dist)) {
					best = OBJ_INDEX(objp);
					best_dist = dist;
				}
			}
		}
	}

	if ((Selection_lock && !Objects[best].flags[Object::Object_Flags::Marked]) || Objects[best].flags[Object::Object_Flags::Locked_from_editing]) {
		return -1;
	}

	return best;
}

size_t EditorViewport::getLayerIndex(const SCP_string& name) const {
	for (size_t i = 0; i < _layerNames.size(); ++i) {
		if (stricmp(_layerNames[i].c_str(), name.c_str()) == 0) {
			return i;
		}
	}
	return static_cast<size_t>(-1);
}

size_t EditorViewport::getObjectLayerIndex(int objectIndex) const {
	const auto found = _objectLayers.find(objectIndex);
	if (found == _objectLayers.end() || found->second >= _layerNames.size()) {
		return 0;
	}
	return found->second;
}

bool EditorViewport::isLayerVisible(size_t layerIndex) const {
	if (layerIndex >= _layerVisibility.size()) {
		return true;
	}
	return _layerVisibility[layerIndex];
}

void EditorViewport::syncMissionLayerNames() const {
	The_mission.fred_layers = _layerNames;
}

void EditorViewport::setObjectLayerByIndex(int objectIndex, size_t layerIndex) {
	_objectLayers[objectIndex] = layerIndex;

	const auto& layerName = _layerNames[layerIndex];
	if (Objects[objectIndex].type == OBJ_SHIP || Objects[objectIndex].type == OBJ_START) {
		Ships[Objects[objectIndex].instance].fred_layer = layerName;
	} else if (Objects[objectIndex].type == OBJ_PROP) {
		auto* prop = prop_id_lookup(Objects[objectIndex].instance);
		if (prop != nullptr) {
			prop->fred_layer = layerName;
		}
	} else if (Objects[objectIndex].type == OBJ_JUMP_NODE) {
		auto* jn = jumpnode_get_by_objnum(objectIndex);
		if (jn != nullptr) {
			jn->SetFredLayer(layerName);
		}
	} else if (Objects[objectIndex].type == OBJ_COORDINATE_POINT) {
		auto* cp = find_coordinate_point_by_objnum(objectIndex);
		if (cp != nullptr) {
			cp->fred_layer = layerName;
		}
	} else if (Objects[objectIndex].type == OBJ_WAYPOINT) {
		// Layer is tracked at the path level; sync all waypoints in the path to the same layer
		auto* wl = find_waypoint_list_with_instance(Objects[objectIndex].instance, nullptr);
		if (wl != nullptr) {
			wl->set_fred_layer(layerName);
			for (const auto& wpt : wl->get_waypoints()) {
				_objectLayers[wpt.get_objnum()] = layerIndex;
			}
		}
	}
}

SCP_vector<SCP_string> EditorViewport::getLayerNames() const {
	return _layerNames;
}

bool EditorViewport::addLayer(const SCP_string& name, SCP_string* errorMessage) {
	if (name.empty()) {
		if (errorMessage != nullptr) {
			*errorMessage = "Layer name cannot be empty.";
		}
		return false;
	}
	if (getLayerIndex(name) != static_cast<size_t>(-1)) {
		if (errorMessage != nullptr) {
			*errorMessage = "Layer names must be unique.";
		}
		return false;
	}

	_layerNames.push_back(name);
	_layerVisibility.push_back(true);
	syncMissionLayerNames();
	editor->notifyLayerStructureChanged();
	editor->notifyLayerListChanged();
	return true;
}

bool EditorViewport::deleteLayer(const SCP_string& name, SCP_string* errorMessage) {
	const auto layerIndex = getLayerIndex(name);
	if (layerIndex == static_cast<size_t>(-1)) {
		if (errorMessage != nullptr) {
			*errorMessage = "Layer does not exist.";
		}
		return false;
	}
	if (layerIndex == 0) {
		if (errorMessage != nullptr) {
			*errorMessage = "The default layer cannot be deleted.";
		}
		return false;
	}

	_layerNames.erase(_layerNames.begin() + static_cast<SCP_vector<SCP_string>::difference_type>(layerIndex));
	_layerVisibility.erase(_layerVisibility.begin() + static_cast<SCP_vector<bool>::difference_type>(layerIndex));

	std::vector<int> toReassign;
	for (auto& objectLayer : _objectLayers) {
		if (objectLayer.second == layerIndex) {
			toReassign.push_back(objectLayer.first);
		} else if (objectLayer.second > layerIndex) {
			--objectLayer.second;
		}
	}
	for (int objIdx : toReassign) {
		setObjectLayerByIndex(objIdx, 0);
	}
	syncMissionLayerNames();
	editor->notifyLayerStructureChanged();
	editor->notifyLayerListChanged();
	return true;
}

bool EditorViewport::renameLayer(const SCP_string& oldName, const SCP_string& newName, SCP_string* errorMessage) {
	if (newName.empty()) {
		if (errorMessage != nullptr) {
			*errorMessage = "Layer name cannot be empty.";
		}
		return false;
	}

	const auto layerIndex = getLayerIndex(oldName);
	if (layerIndex == static_cast<size_t>(-1)) {
		if (errorMessage != nullptr) {
			*errorMessage = "Layer does not exist.";
		}
		return false;
	}
	if (layerIndex == 0) {
		if (errorMessage != nullptr) {
			*errorMessage = "The default layer cannot be renamed.";
		}
		return false;
	}

	// Reject collisions with a different layer; a case-only rename resolves to the same index and is allowed.
	const auto existingIndex = getLayerIndex(newName);
	if (existingIndex != static_cast<size_t>(-1) && existingIndex != layerIndex) {
		if (errorMessage != nullptr) {
			*errorMessage = "Layer names must be unique.";
		}
		return false;
	}

	_layerNames[layerIndex] = newName;

	// Rewrite the per-object fred_layer string on every object assigned to this layer.
	std::vector<int> toResync;
	for (const auto& objectLayer : _objectLayers) {
		if (objectLayer.second == layerIndex) {
			toResync.push_back(objectLayer.first);
		}
	}
	for (int objIdx : toResync) {
		setObjectLayerByIndex(objIdx, layerIndex);
	}

	syncMissionLayerNames();
	editor->notifyLayerStructureChanged();
	editor->notifyLayerListChanged();
	return true;
}

bool EditorViewport::setLayerVisibility(const SCP_string& name, bool visible, SCP_string* errorMessage) {
	const auto layerIndex = getLayerIndex(name);
	if (layerIndex == static_cast<size_t>(-1)) {
		if (errorMessage != nullptr) {
			*errorMessage = "Layer does not exist.";
		}
		return false;
	}

	_layerVisibility[layerIndex] = visible;
	if (!visible) {
		for (auto objp = GET_FIRST(&obj_used_list); objp != END_OF_LIST(&obj_used_list); objp = GET_NEXT(objp)) {
			if (getObjectLayerIndex(OBJ_INDEX(objp)) == layerIndex && objp->flags[Object::Object_Flags::Marked]) {
				editor->unmarkObject(OBJ_INDEX(objp));
			}
		}
	}

	needsUpdate();
	editor->notifyLayerVisibilityChanged();
	return true;
}

bool EditorViewport::getLayerVisibility(const SCP_string& name, bool* visible, SCP_string* errorMessage) const {
	const auto layerIndex = getLayerIndex(name);
	if (layerIndex == static_cast<size_t>(-1)) {
		if (errorMessage != nullptr) {
			*errorMessage = "Layer does not exist.";
		}
		return false;
	}

	if (visible != nullptr) {
		*visible = isLayerVisible(layerIndex);
	}
	return true;
}

void EditorViewport::showAllLayers() {
	std::fill(_layerVisibility.begin(), _layerVisibility.end(), true);
	needsUpdate();
}

int EditorViewport::getHiddenLayerCount() const {
	return static_cast<int>(std::count(_layerVisibility.begin(), _layerVisibility.end(), false));
}

void EditorViewport::reloadLayersFromMission() {
	_layerNames.clear();
	_layerVisibility.clear();
	_objectLayers.clear();

	if (The_mission.fred_layers.empty()) {
		_layerNames.emplace_back(DefaultLayerName);
	} else {
		_layerNames = The_mission.fred_layers;
	}

	if (_layerNames.empty() || _layerNames.front() != DefaultLayerName) {
		_layerNames.insert(_layerNames.begin(), DefaultLayerName);
	}

	_layerVisibility.resize(_layerNames.size(), true);
	syncMissionLayerNames();
	editor->notifyLayerListChanged();

	for (int objectIndex = 0; objectIndex < MAX_OBJECTS; ++objectIndex) {
		auto* objp = &Objects[objectIndex];
		if (objp->type == OBJ_NONE) {
			continue;
		}

		size_t layerIndex = 0;
		if (objp->type == OBJ_SHIP || objp->type == OBJ_START) {
			const auto found = getLayerIndex(Ships[objp->instance].fred_layer);
			layerIndex = found == static_cast<size_t>(-1) ? 0 : found;
		} else if (objp->type == OBJ_PROP) {
			auto* prop = prop_id_lookup(objp->instance);
			if (prop != nullptr) {
				const auto found = getLayerIndex(prop->fred_layer);
				layerIndex = found == static_cast<size_t>(-1) ? 0 : found;
			}
		} else if (objp->type == OBJ_JUMP_NODE) {
			auto* jn = jumpnode_get_by_objnum(objectIndex);
			if (jn != nullptr) {
				const auto found = getLayerIndex(jn->GetFredLayer());
				layerIndex = found == static_cast<size_t>(-1) ? 0 : found;
			}
		} else if (objp->type == OBJ_WAYPOINT) {
			auto* wl = find_waypoint_list_with_instance(objp->instance, nullptr);
			if (wl != nullptr) {
				const auto found = getLayerIndex(wl->get_fred_layer());
				layerIndex = found == static_cast<size_t>(-1) ? 0 : found;
			}
		} else if (objp->type == OBJ_COORDINATE_POINT) {
			auto* cp = find_coordinate_point_by_objnum(objectIndex);
			if (cp != nullptr) {
				const auto found = getLayerIndex(cp->fred_layer);
				layerIndex = found == static_cast<size_t>(-1) ? 0 : found;
			}
		}

		setObjectLayerByIndex(objectIndex, layerIndex);
	}

	needsUpdate();
}

void EditorViewport::registerObjectInLayer(int objectIndex) {
	if (objectIndex < 0 || objectIndex >= MAX_OBJECTS) {
		return;
	}
	auto* objp = &Objects[objectIndex];
	if (objp->type == OBJ_NONE) {
		return;
	}

	SCP_string layerName;
	switch (objp->type) {
	case OBJ_SHIP:
	case OBJ_START:
		layerName = Ships[objp->instance].fred_layer;
		break;
	case OBJ_PROP:
		if (auto* p = prop_id_lookup(objp->instance)) {
			layerName = p->fred_layer;
		}
		break;
	case OBJ_JUMP_NODE:
		if (auto* jn = jumpnode_get_by_objnum(objectIndex)) {
			layerName = jn->GetFredLayer();
		}
		break;
	case OBJ_WAYPOINT:
		if (auto* wl = find_waypoint_list_with_instance(objp->instance, nullptr)) {
			layerName = wl->get_fred_layer();
		}
		break;
	case OBJ_COORDINATE_POINT:
		if (auto* cp = find_coordinate_point_by_objnum(objectIndex)) {
			layerName = cp->fred_layer;
		}
		break;
	default:
		return;
	}

	auto layerIndex = getLayerIndex(layerName);
	if (layerIndex == static_cast<size_t>(-1)) {
		layerIndex = 0;
	}
	_objectLayers[objectIndex] = layerIndex;
}

SCP_string EditorViewport::getObjectLayerName(int objectIndex) const {
	const auto layerIndex = getObjectLayerIndex(objectIndex);
	if (layerIndex >= _layerNames.size()) {
		return DefaultLayerName;
	}
	return _layerNames[layerIndex];
}


bool EditorViewport::moveObjectToLayer(int objectIndex, const SCP_string& layerName, SCP_string* errorMessage) {
	const auto layerIndex = getLayerIndex(layerName);
	if (layerIndex == static_cast<size_t>(-1)) {
		if (errorMessage != nullptr) {
			*errorMessage = "Layer does not exist.";
		}
		return false;
	}

	setObjectLayerByIndex(objectIndex, layerIndex);
	if (!isLayerVisible(layerIndex)) {
		editor->unmarkObject(objectIndex);
	}
	needsUpdate();
	editor->notifyLayerStructureChanged();
	return true;
}

void EditorViewport::moveMarkedObjectsToLayer(const SCP_string& layerName, SCP_string* errorMessage) {
	const auto layerIndex = getLayerIndex(layerName);
	if (layerIndex == static_cast<size_t>(-1)) {
		if (errorMessage != nullptr) {
			*errorMessage = "Layer does not exist.";
		}
		return;
	}

	for (auto objp = GET_FIRST(&obj_used_list); objp != END_OF_LIST(&obj_used_list); objp = GET_NEXT(objp)) {
		if (objp->flags[Object::Object_Flags::Marked]) {
			setObjectLayerByIndex(OBJ_INDEX(objp), layerIndex);
			if (!isLayerVisible(layerIndex)) {
				editor->unmarkObject(OBJ_INDEX(objp));
			}
		}
	}
	needsUpdate();
	editor->notifyLayerStructureChanged();
}

bool EditorViewport::isObjectVisibleInLayer(const object* objp) const {
	if (objp == nullptr) {
		return true;
	}
	return isLayerVisible(getObjectLayerIndex(OBJ_INDEX(objp)));
}

void EditorViewport::drag_rotate_save_backup() {
	object* objp;

	/*
	if (Cur_bitmap != -1)
		bitmap_matrix_backup = Starfield_bitmaps[Cur_bitmap].m;
		*/

	objp = GET_FIRST(&obj_used_list);
	while (objp != END_OF_LIST(&obj_used_list)) {
		Assert(objp->type != OBJ_NONE);
		if (objp->flags[Object::Object_Flags::Marked]) {
			rotation_backup[OBJ_INDEX(objp)].pos = objp->pos;
			rotation_backup[OBJ_INDEX(objp)].orient = objp->orient;
		}

		objp = GET_NEXT(objp);
	}
}

int EditorViewport::create_object_on_grid(int x, int y, int waypoint_instance) {
	return create_object_on_grid(x, y, waypoint_instance, CreateKind::Ship);
}

int EditorViewport::create_object_on_grid(int x, int y, int waypoint_instance, CreateKind kind) {
	float fallbackDist = 200.0f;
	if (kind == CreateKind::Prop) {
		if (cur_prop_index >= 0 && cur_prop_index < prop_info_size()) {
			prop_info* pip = &Prop_info[cur_prop_index];
			if (pip->model_num >= 0) {
				fallbackDist = model_get_radius(pip->model_num) * 1.5f;
			} else if (VALID_FNAME(pip->pof_file)) {
				int modelNum = model_load(pip->pof_file.c_str());
				if (modelNum >= 0) {
					fallbackDist = model_get_radius(modelNum) * 1.5f;
					model_unload(modelNum);
				}
			}
		}
	} else if (kind == CreateKind::Ship && cur_model_index >= 0 && cur_model_index < (int)Ship_info.size() &&
		Ship_info[cur_model_index].model_num >= 0) {
		fallbackDist = model_get_radius(Ship_info[cur_model_index].model_num) * 1.5f;
	}

	vec3d pos = getCreatePosition(x, y, fallbackDist);
	editor->unmark_all();
	int obj = create_object(&pos, waypoint_instance, kind);
	if (obj >= 0) {
		editor->markObject(obj);

		editor->missionChanged();

	} else if (obj == -1) {
		dialogProvider->showButtonDialog(DialogType::Error, "Error", "Maximum ship limit reached.  Can't add any more ships.", { DialogButton::Ok });
	}

	return obj;
}
int EditorViewport::create_object(vec3d* pos, int waypoint_instance, CreateKind kind) {

	int obj, n;
	if (kind == CreateKind::Prop) {
		if (cur_prop_index < 0 || cur_prop_index >= prop_info_size()) {
			return -1;
		}

		obj = prop_create(nullptr, pos, cur_prop_index);
		if (obj == -1) {
			return -1;
		}
	} else if (kind == CreateKind::Other) {
		switch (cur_other_kind) {
		case OtherKind::Waypoint:
			obj = editor->create_waypoint(pos, waypoint_instance);
			break;
		case OtherKind::JumpNode: {
			CJumpNode jnp(pos);
			obj = jnp.GetSCPObjectNumber();
			Jump_nodes.push_back(std::move(jnp));
			break;
		}
		case OtherKind::CoordinatePoint:
			obj = editor->create_coordinate_point(pos);
			break;
		default:
			obj = -1;
			break;
		}
	} else {  // CreateKind::Ship
		if (cur_model_index < 0 || cur_model_index >= (int)Ship_info.size() ||
			Ship_info[cur_model_index].flags[Ship::Info_Flags::No_fred]) {
			obj = -1;
		} else {
			obj = editor->create_ship(nullptr, pos, cur_model_index);
			if (obj == -1)
				return -1;

			n = Objects[obj].instance;
			Ships[n].arrival_cue = alloc_sexp("true", SEXP_ATOM, SEXP_ATOM_OPERATOR, -1, -1);
			Ships[n].departure_cue = alloc_sexp("false", SEXP_ATOM, SEXP_ATOM_OPERATOR, -1, -1);
			Ships[n].cargo1 = 0;
		}
	}

	if (obj < 0)
		return obj;

	obj_merge_created_list();

	needsUpdate();
	return obj;
}
vec3d EditorViewport::getCreatePosition(int x, int y, float fallbackDist) {
	vec3d dir, pos;
	g3_point_to_vec_delayed(&dir, x, y);
	if (fvi_ray_plane(&pos, &The_grid->center, &The_grid->gmatrix.vec.uvec, &camera.view_pos, &dir, 0.0f) >= 0.0f) {
		return pos;
	}
	vm_vec_scale_add(&pos, &camera.view_pos, &camera.view_orient.vec.fvec, fallbackDist);
	return pos;
}

int EditorViewport::createShipAtScreenPos(int x, int y, int modelIndex) {
	if (modelIndex < 0 || modelIndex >= (int)Ship_info.size() ||
		Ship_info[modelIndex].flags[Ship::Info_Flags::No_fred]) {
		return -1;
	}
	int savedModelIndex = cur_model_index;
	cur_model_index = modelIndex;
	int obj = create_object_on_grid(x, y, -1, CreateKind::Ship);
	cur_model_index = savedModelIndex;
	return obj;
}

int EditorViewport::createPropAtScreenPos(int x, int y, int propIndex) {
	if (propIndex < 0 || propIndex >= prop_info_size() ||
		Prop_info[propIndex].flags[Prop::Info_Flags::No_fred]) {
		return -1;
	}
	int savedPropIndex = cur_prop_index;
	cur_prop_index = propIndex;
	int obj = create_object_on_grid(x, y, -1, CreateKind::Prop);
	cur_prop_index = savedPropIndex;
	return obj;
}

int EditorViewport::createWaypointAtScreenPos(int x, int y, int waypoint_instance) {
	OtherKind savedKind = cur_other_kind;
	cur_other_kind = OtherKind::Waypoint;
	int obj = create_object_on_grid(x, y, waypoint_instance, CreateKind::Other);
	cur_other_kind = savedKind;
	return obj;
}

int EditorViewport::createJumpNodeAtScreenPos(int x, int y) {
	OtherKind savedKind = cur_other_kind;
	cur_other_kind = OtherKind::JumpNode;
	int obj = create_object_on_grid(x, y, -1, CreateKind::Other);
	cur_other_kind = savedKind;
	return obj;
}

int EditorViewport::createCoordinatePointAtScreenPos(int x, int y) {
	OtherKind savedKind = cur_other_kind;
	cur_other_kind = OtherKind::CoordinatePoint;
	int obj = create_object_on_grid(x, y, -1, CreateKind::Other);
	cur_other_kind = savedKind;
	return obj;
}

void EditorViewport::initialSetup() {
	cur_model_index = get_default_player_ship_index();
	cur_other_kind = OtherKind::Waypoint;
	for (int i = 0; i < prop_info_size(); ++i) {
		if (!Prop_info[i].flags[Prop::Info_Flags::No_fred]) {
			cur_prop_index = i;
			break;
		}
	}
}

int EditorViewport::duplicate_marked_objects(bool insert_waypoints)
{
	int z, cobj, flag;
	object *objp, *ptr;

	cobj = Duped_wing = -1;
	flag = 0;

	int duping_waypoint_list = -1;

	objp = GET_FIRST(&obj_used_list);
	while (objp != END_OF_LIST(&obj_used_list))	{
		Assert(objp->type != OBJ_NONE);
		if (objp->flags[Object::Object_Flags::Marked]) {
			if ((objp->type == OBJ_SHIP) || (objp->type == OBJ_START)) {
				z = Ships[objp->instance].wingnum;
				if (!flag)
					Duped_wing = z;
				else if (Duped_wing != z)
					Duped_wing = -1;

			} else {
				Duped_wing = -1;
			}

			flag = 1;

			if (insert_waypoints && objp->type == OBJ_WAYPOINT) {
				// Insert a new waypoint into the source path right after this one.
				// No new list is created, so no list-property copy is needed.
				z = waypoint_add(&objp->pos, objp->instance, false);
				if (z < 0) {
					cobj = -1;
					break;
				}
				Objects[z].pos = objp->pos;
				Objects[z].orient = objp->orient;
				Objects[z].flags.set(Object::Object_Flags::Temp_marked);
				registerObjectInLayer(z);
				if (editor->currentObject == OBJ_INDEX(objp))
					cobj = z;
			} else {
				// make sure we dup as many waypoint lists as we have
				if (objp->type == OBJ_WAYPOINT) {
					int this_list = calc_waypoint_list_index(objp->instance);
					if (duping_waypoint_list != this_list) {
						editor->dup_object(nullptr);  // reset waypoint list
						duping_waypoint_list = this_list;
					}
				}

				z = editor->dup_object(objp);
				if (z == -1) {
					cobj = -1;
					break;
				}

				if (editor->currentObject == OBJ_INDEX(objp))
					cobj = z;
			}
		}

		objp = GET_NEXT(objp);
	}

	obj_merge_created_list();

	// I think this code is to catch the case where an object wasn't created for whatever reason;
	// in this case just delete the remaining objects we just created
	if (cobj == -1) {
		objp = GET_FIRST(&obj_used_list);
		while (objp != END_OF_LIST(&obj_used_list))	{
			ptr = GET_NEXT(objp);
			if (objp->flags [Object::Object_Flags::Temp_marked])
				editor->delete_object(OBJ_INDEX(objp));

			objp = ptr;
		}

		button_down = false;
		return -1;
	}

	editor->unmark_all();

	objp = GET_FIRST(&obj_used_list);
	while (objp != END_OF_LIST(&obj_used_list))	{
		if (objp->flags [Object::Object_Flags::Temp_marked]) {
			objp->flags.remove(Object::Object_Flags::Temp_marked);
			editor->markObject(OBJ_INDEX(objp));
		}

		objp = GET_NEXT(objp);
	}

	editor->selectObject(cobj);
	return 0;
}

//	If cur_object_index references a valid object, drag it from its current
//	location to the new cursor location specified by "point".
//	It is dragged relative to the main grid.  Its y coordinate is not changed.
//	Return value: 0/1 = didn't/did move object all the way to goal.
int EditorViewport::drag_objects(int x, int y)
{
	int rval = 1;
	float r;
	float	distance_moved = 0.0f;
	vec3d cursor_dir, int_pnt;
	vec3d movement_vector;
	vec3d obj;
	vec3d vec1, vec2;
	object *objp;
	// starfield_bitmaps *bmp;

	/*
	if (Bg_bitmap_dialog) {
		if (Cur_bitmap < 0)
			return -1;

		bmp = &Starfield_bitmaps[Cur_bitmap];
		if (Single_axis_constraint && Constraint.z) {
			bmp->dist *= 1.0f + mouse_dx / -800.0f;
			calculate_bitmap_points(bmp, 0.0f);

		} else {
			g3_point_to_vec_delayed(&bmp->m.fvec, marking_box.x2, marking_box.y2);
			vm_orthogonalize_matrix(&bmp->m);
			calculate_bitmap_points(bmp, 0.0f);
		}
		return rval;
	}
	*/

	// Do not move ships that we are currently centered around (Lookat_mode). The vector math will start going haywire and return NAN
	if (!query_valid_object(editor->currentObject) || camera.getLookatMode())
		return -1;

	if (Dup_drag == DUP_DRAG_INSERT) {
		// inserting points changes a path's shape, which its lock holds
		for (auto* p = GET_FIRST(&obj_used_list); p != END_OF_LIST(&obj_used_list); p = GET_NEXT(p)) {
			if (p->flags[Object::Object_Flags::Marked] && p->type == OBJ_WAYPOINT && Editor::isTransformLocked(OBJ_INDEX(p))) {
				editor->reportTransformHeld(OBJ_INDEX(p));
				return -1;
			}
		}
	}

	if (Dup_drag == 1 || Dup_drag == DUP_DRAG_INSERT) {
		const bool insert_waypoints = (Dup_drag == DUP_DRAG_INSERT);
		if (duplicate_marked_objects(insert_waypoints) < 0)
			return -1;

		if (Duped_wing != -1)
			Dup_drag = DUP_DRAG_OF_WING;  // indication for later that we duped objects in a wing
		else
			Dup_drag = 0;

		drag_rotate_save_backup();

		editor->missionChanged();
	}

	// Blender style: locked objects stay put, but if the one being dragged is held, nothing moves
	if (Editor::isTransformHeld(editor->currentObject)) {
		editor->reportTransformHeld(editor->currentObject);
		return -1;
	}

	objp = &Objects[editor->currentObject];
	Assert(objp->type != OBJ_NONE);
	obj = int_pnt = objp->pos;

	//	Get 3d vector specified by mouse cursor location.
	g3_point_to_vec_delayed(&cursor_dir, x, y);
	if (Single_axis_constraint)	{
//		if (fvi_ray_plane(&int_pnt, &obj, &view_orient.fvec, &view_pos, &cursor_dir, 0.0f) >= 0.0f )	{
//			vm_vec_add(&p1, &obj, &Constraint);
//			find_nearest_point_on_line(&nearest_point, &obj, &p1, &int_pnt);
//			int_pnt = nearest_point;
//			distance_moved = vm_vec_dist(&obj, &int_pnt);
//		}

		vec3d tmpAnticonstraint = Anticonstraint;
		vec3d tmpObject = obj;

		tmpAnticonstraint.xyz.x = 0.0f;
		r = fvi_ray_plane(&int_pnt, &tmpObject, &tmpAnticonstraint, &camera.view_pos, &cursor_dir, 0.0f);

		//	If intersected behind viewer, don't move.  Too confusing, not what user wants.
		vm_vec_sub(&vec1, &int_pnt, &camera.view_pos);
		vm_vec_sub(&vec2, &obj, &camera.view_pos);
		if ((r>=0.0f) && (vm_vec_dot(&vec1, &vec2) >= 0.0f))	{
			vec3d tmp1;
			vm_vec_sub( &tmp1, &int_pnt, &obj );
			tmp1.xyz.x *= Constraint.xyz.x;
			tmp1.xyz.y *= Constraint.xyz.y;
			tmp1.xyz.z *= Constraint.xyz.z;
			vm_vec_add( &int_pnt, &obj, &tmp1 );

			distance_moved = vm_vec_dist(&obj, &int_pnt);
		}


	} else {  // Move in x-z plane, defined by grid.  Preserve height.
		r = fvi_ray_plane(&int_pnt, &obj, &Anticonstraint, &camera.view_pos, &cursor_dir, 0.0f);

		//	If intersected behind viewer, don't move.  Too confusing, not what user wants.
		vm_vec_sub(&vec1, &int_pnt, &camera.view_pos);
		vm_vec_sub(&vec2, &obj, &camera.view_pos);
		if ((r>=0.0f) && (vm_vec_dot(&vec1, &vec2) >= 0.0f))
			distance_moved = vm_vec_dist(&obj, &int_pnt);
	}

	//	If moved too far, then move max distance along vector.
	vm_vec_sub(&movement_vector, &int_pnt, &obj);
/*	if (distance_moved > MAX_MOVE_DISTANCE)	{
		vm_vec_normalize(&movement_vector);
		vm_vec_scale(&movement_vector, MAX_MOVE_DISTANCE);
	} */

	if (distance_moved) {
		objp = GET_FIRST(&obj_used_list);
		while (objp != END_OF_LIST(&obj_used_list))	{
			Assert(objp->type != OBJ_NONE);
			if (objp->flags[Object::Object_Flags::Marked] && !Editor::isTransformHeld(OBJ_INDEX(objp))) {
				vm_vec_add(&objp->pos, &objp->pos, &movement_vector);
				if (objp->type == OBJ_WAYPOINT) {
					waypoint *wpt = find_waypoint_with_instance(objp->instance);
					Assert(wpt != NULL);
					wpt->set_pos(&objp->pos);
				}
			}

			objp = GET_NEXT(objp);
		}

		objp = GET_FIRST(&obj_used_list);
		while (objp != END_OF_LIST(&obj_used_list)) {
			if (objp->flags[Object::Object_Flags::Marked])
				object_moved(objp);

			objp = GET_NEXT(objp);
		}
	}

	editor->missionChanged();
	return rval;
}
int EditorViewport::drag_rotate_objects(int mouse_dx, int mouse_dy) {
	int rval = 1;
	vec3d int_pnt, obj;
	angles a;
	matrix newmat, rotmat;
	object *leader, *objp;
	// starfield_bitmaps *bmp;

	needsUpdate();
	/*
    if (Bg_bitmap_dialog) {
        if (Cur_bitmap < 0)
            return -1;

        bmp = &Starfield_bitmaps[Cur_bitmap];
        calculate_bitmap_points(bmp, mouse_dx / -300.0f);
        return rval;
    }
    */

	if (!query_valid_object(editor->currentObject)){
		return -1;
	}
	if (Editor::isTransformHeld(editor->currentObject)) {
		editor->reportTransformHeld(editor->currentObject);
		return -1;
	}

	objp = &Objects[editor->currentObject];
	Assert(objp->type != OBJ_NONE);
	obj = int_pnt = objp->pos;

	memset(&a, 0, sizeof(angles));
	if (Single_axis_constraint) {
		if (Constraint.xyz.x)
			a.p = mouse_dy / REDUCER;
		else if (Constraint.xyz.y)
			a.h = mouse_dx / REDUCER;
		else if (Constraint.xyz.z)
			a.b = -mouse_dx / REDUCER;

	} else {
		if (!Constraint.xyz.x) {				// yz
			a.b = -mouse_dx / REDUCER;
			a.h = mouse_dy / REDUCER;
		} else if (!Constraint.xyz.y) {	// xz
			a.p = mouse_dy / REDUCER;
			a.b = -mouse_dx / REDUCER;
		} else if (!Constraint.xyz.z) {	// xy
			a.p = mouse_dy / REDUCER;
			a.h = mouse_dx / REDUCER;
		}
	}

	leader = &Objects[editor->currentObject];
	const matrix leader_old_orient = leader->orient;

	vm_angles_2_matrix(&rotmat, &a);
	vm_matrix_x_matrix(&newmat, &leader->orient, &rotmat);
	leader->orient = newmat;

	follow_leader(leader, leader->pos, leader_old_orient, rotmat);

	objp = GET_FIRST(&obj_used_list);
	while (objp != END_OF_LIST(&obj_used_list)) {
		if (objp->flags[Object::Object_Flags::Marked])
			object_moved(objp);

		objp = GET_NEXT(objp);
	}

	editor->missionChanged();
	return rval;
}
void EditorViewport::follow_leader(const object* leader, const vec3d& leader_old_pos, const matrix& leader_old_orient,
	const matrix& rotmat) const {
	vec3d delta_pos;
	vm_vec_sub(&delta_pos, &leader->pos, &leader_old_pos);
	matrix leader_old_transpose, rot_trans;
	vm_copy_transpose(&leader_old_transpose, &leader_old_orient);
	vm_copy_transpose(&rot_trans, &rotmat);

	for (object* objp = GET_FIRST(&obj_used_list); objp != END_OF_LIST(&obj_used_list); objp = GET_NEXT(objp)) {
		Assert(objp->type != OBJ_NONE);
		// locked objects, and ships docked to one, stay put
		if (!objp->flags[Object::Object_Flags::Marked] || objp == leader || Editor::isTransformHeld(OBJ_INDEX(objp)))
			continue;
		matrix tmp;
		switch (Pivot_mode) {
		case PivotMode::Group: {
			// Orbit: rotate the offset from the leader by the leader's turn, in the leader's own
			// frame, then turn the object with it
			vec3d tmpv1, tmpv2;
			vm_vec_sub(&tmpv1, &objp->pos, &leader_old_pos);
			vm_vec_rotate(&tmpv2, &tmpv1, &leader_old_orient);
			vm_vec_rotate(&tmpv1, &tmpv2, &rot_trans);
			vm_vec_rotate(&tmpv2, &tmpv1, &leader_old_transpose);
			vm_vec_add(&objp->pos, &leader->pos, &tmpv2);

			vm_matrix_x_matrix(&tmp, &objp->orient, &rotmat);
			vm_orthogonalize_matrix(&tmp); // safety check
			objp->orient = tmp;
			break;
		}
		case PivotMode::Individual:
			vm_vec_add2(&objp->pos, &delta_pos);
			vm_matrix_x_matrix(&tmp, &objp->orient, &rotmat);
			objp->orient = tmp;
			break;
		case PivotMode::Align:
			vm_vec_add2(&objp->pos, &delta_pos);
			objp->orient = leader->orient;
			break;
		}
	}
}
void EditorViewport::cancel_drag() {
	if (!button_down) {
		return;
	}

	auto objp = GET_FIRST(&obj_used_list);
	while (objp != END_OF_LIST(&obj_used_list)) {
		Assert(objp->type != OBJ_NONE);
		if (objp->flags[Object::Object_Flags::Marked]) {
			const auto obj_index = OBJ_INDEX(objp);
			if (!IS_VEC_NULL(&rotation_backup[obj_index].orient.vec.rvec) && !IS_VEC_NULL(&rotation_backup[obj_index].orient.vec.uvec)
				&& !IS_VEC_NULL(&rotation_backup[obj_index].orient.vec.fvec)) {
				objp->pos = rotation_backup[obj_index].pos;
				objp->orient = rotation_backup[obj_index].orient;
			}
		}

		objp = GET_NEXT(objp);
	}

	// The drag carried docked partners along, and they were never backed up: snap them back
	// to the restored ships (docking geometry fixes a partner's place exactly)
	for (objp = GET_FIRST(&obj_used_list); objp != END_OF_LIST(&obj_used_list); objp = GET_NEXT(objp)) {
		if (objp->flags[Object::Object_Flags::Marked])
			object_moved(objp);
	}

	button_down = false;
	moved = false;
	Dup_drag = 0;
	needsUpdate();
}
void EditorViewport::view_universe(bool just_marked) {
	int max = 0;
	float dist, largest = 20.0f;
	vec3d center, p1, p2;
	vertex v;
	object *ptr;

	if (just_marked)
		ptr = &Objects[editor->currentObject];
	else
		ptr = GET_FIRST(&obj_used_list);

	p1.xyz.x = p2.xyz.x = ptr->pos.xyz.x;
	p1.xyz.y = p2.xyz.y = ptr->pos.xyz.y;
	p1.xyz.z = p2.xyz.z = ptr->pos.xyz.z;

	ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list)) {
		if (!just_marked || (ptr->flags[Object::Object_Flags::Marked])) {
			center = ptr->pos;
			if (center.xyz.x < p1.xyz.x)
				p1.xyz.x = center.xyz.x;
			if (center.xyz.x > p2.xyz.x)
				p2.xyz.x = center.xyz.x;
			if (center.xyz.y < p1.xyz.y)
				p1.xyz.y = center.xyz.y;
			if (center.xyz.y > p2.xyz.y)
				p2.xyz.y = center.xyz.y;
			if (center.xyz.z < p1.xyz.z)
				p1.xyz.z = center.xyz.z;
			if (center.xyz.z > p2.xyz.z)
				p2.xyz.z = center.xyz.z;
		}

		ptr = GET_NEXT(ptr);
	}

	vm_vec_avg(&center, &p1, &p2);
	ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list)) {
		if (!just_marked || (ptr->flags[Object::Object_Flags::Marked])) {
			dist = vm_vec_dist_squared(&center, &ptr->pos);
			if (dist > largest)
				largest = dist;

			if (OBJ_INDEX(ptr) > max)
				max = OBJ_INDEX(ptr);
		}

		ptr = GET_NEXT(ptr);
	}

	dist = fl_sqrt(largest) + 1.0f;
	vm_vec_scale_add(&camera.view_pos, &center, &camera.view_orient.vec.fvec, -dist);
	g3_set_view_matrix(&camera.view_pos, &camera.view_orient, viewFov());

	ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list)) {
		if (!just_marked || (ptr->flags[Object::Object_Flags::Marked])) {
			g3_rotate_vertex(&v, &ptr->pos);
			Assert(!(v.codes & CC_BEHIND));
			if (g3_project_vertex(&v) & PF_OVERFLOW)
				Int3();

			while (v.codes & CC_OFF) {
				dist += 5.0f;
				vm_vec_scale_add(&camera.view_pos, &center, &camera.view_orient.vec.fvec, -dist);
				g3_set_view_matrix(&camera.view_pos, &camera.view_orient, viewFov());
				g3_rotate_vertex(&v, &ptr->pos);
				if (g3_project_vertex(&v) & PF_OVERFLOW)
					Int3();
			}
		}

		ptr = GET_NEXT(ptr);
	}

	dist *= 1.1f;
	vm_vec_scale_add(&camera.view_pos, &center, &camera.view_orient.vec.fvec, -dist);
	g3_set_view_matrix(&camera.view_pos, &camera.view_orient, viewFov());

	needsUpdate();
}
void EditorViewport::view_object(int obj_num) {
	vm_vec_scale_add(&camera.view_pos, &Objects[obj_num].pos, &camera.view_orient.vec.fvec,
	                 Objects[obj_num].radius * -3.0f);

	needsUpdate();
}

// --- Background element mouse editing ---------------------------------------
// The handle placement distance (BG_HANDLE_DISTANCE) is shared with the
// renderer via EditorViewport.h so picking and drawing stay aligned.

void EditorViewport::setBackgroundEditModel(dialogs::BackgroundEditorDialogModel* model)
{
	_bgEditModel = model;
	_bgDragIndex = -1;
	needsUpdate();
}

void EditorViewport::setVolumetricEditModel(dialogs::VolumetricNebulaDialogModel* model)
{
	_volEditModel = model;
	needsUpdate();
}

void EditorViewport::setAsteroidEditModel(dialogs::AsteroidEditorDialogModel* model)
{
	_astEditModel = model;
	needsUpdate();
}

bool EditorViewport::select_background_element(int cx, int cy, bool& isSun, int& index) const
{
	if (_bgEditModel == nullptr) {
		return false;
	}

	// Projection requires an active frame (same guard as select_object).
	if (g3_in_frame() != 1) {
		return false;
	}

	// The element getters read global background state and are static; the
	// _bgEditModel pointer is only a gate for "is the dialog open".
	using BgModel = dialogs::BackgroundEditorDialogModel;

	// pixel radius (squared) within which a handle counts as clicked
	double best_dist = 16.0 * 16.0;
	int best_index = -1;
	bool best_is_sun = false;

	auto test_list = [&](bool sun, int count) {
		for (int i = 0; i < count; i++) {
			vec3d dir;
			const bool ok = sun ? BgModel::getSunDirection(i, dir)
			                    : BgModel::getBitmapDirection(i, dir);
			if (!ok) {
				continue;
			}

			vec3d world;
			vm_vec_scale_add(&world, &camera.eye_pos, &dir, BG_HANDLE_DISTANCE);

			vertex vt;
			g3_rotate_vertex(&vt, &world);
			if (vt.codes & CC_BEHIND) {
				continue;
			}
			if (g3_project_vertex(&vt) & PF_OVERFLOW) {
				continue;
			}

			const double dx = vt.screen.xyw.x - cx;
			const double dy = vt.screen.xyw.y - cy;
			const double dist = dx * dx + dy * dy;
			if (dist < best_dist) {
				best_dist = dist;
				best_index = i;
				best_is_sun = sun;
			}
		}
	};

	// Test suns first so they win an exact tie: they are the smaller target and
	// usually sit in front of large bitmaps (the pick uses a strict < compare).
	test_list(true, BgModel::getSunCount());
	test_list(false, BgModel::getBitmapCount());

	if (best_index < 0) {
		return false;
	}

	isSun = best_is_sun;
	index = best_index;
	return true;
}

void EditorViewport::begin_background_drag(bool isSun, int index)
{
	_bgDragIsSun = isSun;
	_bgDragIndex = index;
}

void EditorViewport::drag_background_element(int x, int y)
{
	if (_bgEditModel == nullptr || _bgDragIndex < 0) {
		return;
	}

	// Mouse ray direction becomes the element's new pointing direction.
	vec3d dir;
	g3_point_to_vec_delayed(&dir, x, y);
	if (!dir.xyz.x && !dir.xyz.y && !dir.xyz.z) {
		return;
	}

	if (_bgDragIsSun) {
		_bgEditModel->setSunDirectionFromViewport(_bgDragIndex, dir);
	} else {
		_bgEditModel->setBitmapDirectionFromViewport(_bgDragIndex, dir);
	}

	needsUpdate();
}

void EditorViewport::rotate_background_element(int mouse_dx)
{
	// Only bitmaps carry a meaningful bank (suns are radially symmetric).
	if (_bgEditModel == nullptr || _bgDragIndex < 0 || _bgDragIsSun) {
		return;
	}

	_bgEditModel->nudgeBitmapBankFromViewport(_bgDragIndex, static_cast<float>(mouse_dx) * -0.2f);
	needsUpdate();
}

void EditorViewport::end_background_drag()
{
	_bgDragIndex = -1;
	_bgDragIsSun = false;
}

// ---------------------------------------------------------------------------
// Viewport handle subsystem
// ---------------------------------------------------------------------------

// Pick radius for handles, in squared screen pixels. The existing object
// fallback in select_object() uses 8 (~2.8px); handles get a more generous
// radius so they feel grabbable even at distance.
static constexpr double kHandlePickRadiusSquared = 144.0; // 12px

HandleGroupId EditorViewport::registerHandleGroup(std::vector<ViewportHandle> handles) {
	// Always a new slot. An empty slot can't be told apart from a registered
	// group that currently has no handles (a gizmo whose entity went away), so
	// reusing one would make two groups share an index.
	_handle_groups.push_back(std::move(handles));
	_handle_group_generations.push_back(1);
	HandleGroupId id;
	id.index = static_cast<int>(_handle_groups.size() - 1);
	id.generation = 1;
	needsUpdate();
	return id;
}

void EditorViewport::updateHandleGroup(HandleGroupId id, std::vector<ViewportHandle> handles) {
	if (id.index < 0 || id.index >= static_cast<int>(_handle_groups.size())) {
		return;
	}
	if (_handle_group_generations[id.index] != id.generation) {
		return; // stale id
	}
	_handle_groups[id.index] = std::move(handles);
	needsUpdate();
}

void EditorViewport::unregisterHandleGroup(HandleGroupId id) {
	if (id.index < 0 || id.index >= static_cast<int>(_handle_groups.size())) {
		return;
	}
	if (_handle_group_generations[id.index] != id.generation) {
		return; // stale id; already replaced
	}
	// If a drag is in progress on this group, cancel it.
	if (_active_handle.group_index == id.index && _active_handle_generation == id.generation) {
		end_handle_drag();
	}
	_handle_groups[id.index].clear();
	// Bump generation so any leftover id pointing at this slot is now stale.
	_handle_group_generations[id.index]++;
	needsUpdate();
}

EditorViewport::HandlePick EditorViewport::pick_handle(int cx, int cy) const {
	HandlePick best{};
	double best_dist = kHandlePickRadiusSquared;

	if (g3_in_frame() != 1) {
		return best;
	}

	for (size_t gi = 0; gi < _handle_groups.size(); ++gi) {
		const auto& group = _handle_groups[gi];
		for (size_t hi = 0; hi < group.size(); ++hi) {
			const auto& handle = group[hi];
			if (handle.is_enabled && !handle.is_enabled()) {
				continue;
			}
			vertex vt;
			vec3d pos_copy = handle.world_pos;
			g3_rotate_vertex(&vt, &pos_copy);
			if (vt.codes & CC_BEHIND) {
				continue;
			}
			if (g3_project_vertex(&vt) & PF_OVERFLOW) {
				continue;
			}
			double dx = static_cast<double>(vt.screen.xyw.x) - cx;
			double dy = static_cast<double>(vt.screen.xyw.y) - cy;
			double dist = dx * dx + dy * dy;
			if (dist < best_dist) {
				best_dist = dist;
				best.group_index = static_cast<int>(gi);
				best.handle_index = static_cast<int>(hi);
			}
		}
	}
	return best;
}

bool EditorViewport::screen_to_constraint_plane(int cx, int cy, const vec3d& anchor, vec3d* out_world) const {
	vec3d cursor_dir, int_pnt, vec1, vec2;
	g3_point_to_vec_delayed(&cursor_dir, cx, cy);

	float r;
	if (Single_axis_constraint) {
		// Same trick drag_objects() uses: zero the X-component of Anticonstraint
		// so the plane stays roughly perpendicular to the view's right.
		vec3d tmpAnticonstraint = Anticonstraint;
		tmpAnticonstraint.xyz.x = 0.0f;
		vec3d tmpAnchor = anchor;
		r = fvi_ray_plane(&int_pnt, &tmpAnchor, &tmpAnticonstraint, &camera.view_pos, &cursor_dir, 0.0f);
	} else {
		r = fvi_ray_plane(&int_pnt, &anchor, &Anticonstraint, &camera.view_pos, &cursor_dir, 0.0f);
	}

	if (r < 0.0f) {
		return false;
	}
	vm_vec_sub(&vec1, &int_pnt, &camera.view_pos);
	vm_vec_sub(&vec2, &anchor, &camera.view_pos);
	if (vm_vec_dot(&vec1, &vec2) < 0.0f) {
		// Intersection landed behind the viewer; ignore.
		return false;
	}

	if (Single_axis_constraint) {
		// Re-apply the single-axis component mask drag_objects() applies.
		vec3d tmp;
		vm_vec_sub(&tmp, &int_pnt, &anchor);
		tmp.xyz.x *= Constraint.xyz.x;
		tmp.xyz.y *= Constraint.xyz.y;
		tmp.xyz.z *= Constraint.xyz.z;
		vm_vec_add(&int_pnt, &anchor, &tmp);
	}

	*out_world = int_pnt;
	return true;
}

bool EditorViewport::begin_handle_drag(HandlePick pick, int cx, int cy) {
	if (pick.group_index < 0 || pick.group_index >= static_cast<int>(_handle_groups.size())) {
		return false;
	}
	const auto& group = _handle_groups[pick.group_index];
	if (pick.handle_index < 0 || pick.handle_index >= static_cast<int>(group.size())) {
		return false;
	}
	const auto& handle = group[pick.handle_index];
	if (!handle.on_drag) {
		return false;
	}

	vec3d anchor;
	if (!screen_to_constraint_plane(cx, cy, handle.world_pos, &anchor)) {
		return false;
	}
	_active_handle = pick;
	_active_handle_generation = _handle_group_generations[pick.group_index];
	_active_handle_last_world = anchor;
	return true;
}

bool EditorViewport::drag_handle(int cx, int cy) {
	if (_active_handle.group_index < 0) {
		return false;
	}
	if (_active_handle.group_index >= static_cast<int>(_handle_groups.size())) {
		end_handle_drag();
		return false;
	}
	if (_handle_group_generations[_active_handle.group_index] != _active_handle_generation) {
		end_handle_drag();
		return false;
	}
	auto& group = _handle_groups[_active_handle.group_index];
	if (_active_handle.handle_index < 0 || _active_handle.handle_index >= static_cast<int>(group.size())) {
		end_handle_drag();
		return false;
	}
	const auto& handle = group[_active_handle.handle_index];
	if (!handle.on_drag) {
		end_handle_drag();
		return false;
	}

	// IMPORTANT: the on_drag callback edits the mission, which can rebuild the
	// handle group and destroy the ViewportHandle (and its std::function!) we're
	// holding a reference to. So copy everything we need into locals up front
	// before invoking the callback.
	auto on_drag_copy = handle.on_drag;
	const auto handle_kind = handle.kind;
	const vec3d handle_axis = handle.axis;

	vec3d new_world;
	if (!screen_to_constraint_plane(cx, cy, _active_handle_last_world, &new_world)) {
		return true; // keep the drag alive; the user will move back into a valid region
	}

	vec3d delta;
	vm_vec_sub(&delta, &new_world, &_active_handle_last_world);

	// Face handles snap motion to their single axis so dragging the +X face
	// only changes max_bound.x even with no toolbar lock. The on_drag callback
	// may further reject the move (e.g. clamping min < max).
	if (handle_kind == ViewportHandle::Kind::Face) {
		float along = vm_vec_dot(&delta, &handle_axis);
		vm_vec_copy_scale(&delta, &handle_axis, along);
	}

	if (delta.xyz.x == 0.0f && delta.xyz.y == 0.0f && delta.xyz.z == 0.0f) {
		return true;
	}

	const vec3d applied = on_drag_copy(delta);
	// Do NOT touch `handle` past here — the rebuild from on_drag may have
	// invalidated it. The active-handle indices themselves are still valid
	// (the rebuild produces a same-shape vector) so the next tick re-looks it
	// up by index from the top.
	//
	// Anchor on what was actually applied, not on the cursor: when a clamp
	// stops the handle, the unapplied part stays pending, so the handle waits
	// for the cursor to come back to it instead of moving off immediately.
	vm_vec_add2(&_active_handle_last_world, &applied);
	needsUpdate();
	return true;
}

void EditorViewport::end_handle_drag() {
	_active_handle = {};
	_active_handle_generation = 0;
}

EnvironmentObject EditorViewport::handleEnvironment(HandlePick pick) const {
	if (pick.group_index >= 0 && _volumetric_handle_group.valid() &&
		pick.group_index == _volumetric_handle_group.index) {
		return EnvironmentObject::VolumetricNebula;
	}
	if (pick.group_index >= 0 && _asteroid_handle_group.valid() &&
		pick.group_index == _asteroid_handle_group.index) {
		return EnvironmentObject::AsteroidField;
	}
	return EnvironmentObject::None;
}

// ---------------------------------------------------------------------------
// Cutscene camera preview
// ---------------------------------------------------------------------------

void EditorViewport::setCameraGizmo(CameraGizmo gizmo) {
	if (_cameraDragActive && gizmo.owner != _cameraGizmo.owner) {
		cancelCameraDrag();
	}
	// A new selection or edit mid-flight: the flight's place may no longer fit the selected
	// sexp's event, so drop it rather than write it there. Playback ends too, so the preview
	// shows the newly selected sexp.
	_camFlying = false;
	stopCameraPlayback();
	_cameraGizmo = std::move(gizmo);
	needsUpdate();
}

void EditorViewport::clearCameraGizmo(const void* owner) {
	if (owner == nullptr || _cameraGizmo.owner != owner) {
		return;
	}
	if (_cameraDragActive) {
		cancelCameraDrag();
	}
	_cameraGizmo = CameraGizmo();
	_camFlying = false;
	stopCameraPlayback();
	needsUpdate();
}

bool EditorViewport::rawCameraPreview(CameraSexpPreview* out) const {
	if (!_cameraGizmo.evaluate) {
		return false;
	}
	CameraSexpPreview preview;
	if (!_cameraGizmo.evaluate(preview)) {
		return false;
	}
	if (out != nullptr) {
		*out = preview;
	}
	return true;
}

bool EditorViewport::cameraPreview(CameraSexpPreview* out) const {
	CameraSexpPreview preview;
	if (!rawCameraPreview(&preview)) {
		return false;
	}
	// Playing, the camera is where the shot has got to; flying, where it was flown until the
	// sexp takes it
	if (_camPlayback && preview.opNode == _camPlayOpNode) {
		preview.shot = preview.play.at(_camTime);
	}
	if (_camFlying) {
		preview.shot.pos = _camFlyPos;
		preview.shot.orient = _camFlyOrient;
	}
	if (out != nullptr) {
		*out = preview;
	}
	return true;
}

bool EditorViewport::canSetCameraFromView() const {
	// Looking through the cutscene camera, the view is the camera already
	if (!_cameraGizmo.setFromView || camera.getViewpoint() == CutsceneCameraViewpoint || _camPlayback) {
		return false;
	}
	CameraSexpPreview preview;
	if (!cameraPreview(&preview)) {
		return false;
	}
	switch (preview.op) {
	case OP_CUTSCENES_SET_CAMERA_POSITION:
	case OP_CUTSCENES_SET_CAMERA_FACING:
		return preview.pointNodes[0] >= 0;
	case OP_CUTSCENES_SET_CAMERA_ROTATION:
		return preview.angleNodes[0] >= 0;
	default:
		return false;
	}
}

bool EditorViewport::canSetCameraFov() const {
	CameraSexpPreview preview;
	return _cameraGizmo.setFov && cameraPreview(&preview) && preview.fovNode >= 0;
}

void EditorViewport::setCameraFov(float fov) {
	if (!canSetCameraFov()) {
		return;
	}
	stopCameraPlayback(); // editing ends playback
	// Copied: the call edits the tree, which hands the viewport a new gizmo
	const auto setFov = _cameraGizmo.setFov;
	setFov(fov);
	needsUpdate();
}

void EditorViewport::setCameraFromView() {
	if (!canSetCameraFromView()) {
		return;
	}
	// Copied: the call edits the tree, which hands the viewport a new gizmo
	const auto setFromView = _cameraGizmo.setFromView;
	setFromView(camera.eye_pos, camera.eye_orient);
	needsUpdate();
}

void EditorViewport::refreshCameraHandle() {
	CameraSexpPreview preview;
	const bool present = view.Show_camera_gizmo && camera.getViewpoint() != CutsceneCameraViewpoint &&
		!_camPlayback && _cameraGizmo.movePoint && cameraPreview(&preview) && preview.pointNodes[0] >= 0;
	vec3d pos = vmd_zero_vector;
	if (present) {
		pos = _cameraDragActive ? _cameraDragPoint : preview.point;
	}
	const int op = present ? preview.op : -1;

	// Only touch the registry on a real change (see refreshVolumetricHandle)
	if (present == _cam_handle_cached_present &&
		(!present || (pos == _cam_handle_cached_pos && op == _cam_handle_cached_op))) {
		return;
	}
	_cam_handle_cached_present = present;
	_cam_handle_cached_pos = pos;
	_cam_handle_cached_op = op;

	std::vector<ViewportHandle> handles;
	if (present) {
		ViewportHandle h;
		h.kind = ViewportHandle::Kind::Center;
		h.world_pos = pos;
		h.color_r = 0;
		h.color_g = 200;
		h.color_b = 255;
		h.info_label = (op == OP_CUTSCENES_SET_CAMERA_POSITION) ? "Camera position" : "Camera facing point";
		h.show_coords = true;
		h.show_grid_position = true;
		h.on_drag = [this](const vec3d& delta) {
			if (!_cameraDragActive) {
				return vmd_zero_vector;
			}
			vm_vec_add2(&_cameraDragPoint, &delta);
			return delta;
		};
		handles.push_back(std::move(h));
	}

	if (_camera_handle_group.valid()) {
		updateHandleGroup(_camera_handle_group, std::move(handles));
	} else {
		_camera_handle_group = registerHandleGroup(std::move(handles));
	}
}

bool EditorViewport::isCameraHandle(HandlePick pick) const {
	return pick.group_index >= 0 && _camera_handle_group.valid() && pick.group_index == _camera_handle_group.index;
}

void EditorViewport::beginCameraDrag() {
	CameraSexpPreview preview;
	if (!cameraPreview(&preview) || !preview.hasPoint) {
		return;
	}
	_cameraDragActive = true;
	_cameraDragPoint = preview.point;
}

bool EditorViewport::cameraDragPoint(vec3d* out) const {
	if (!_cameraDragActive) {
		return false;
	}
	*out = _cameraDragPoint;
	return true;
}

void EditorViewport::commitCameraDrag() {
	if (!_cameraDragActive) {
		return;
	}
	_cameraDragActive = false;

	CameraSexpPreview preview;
	if (_cameraGizmo.movePoint && cameraPreview(&preview) && preview.hasPoint && !(preview.point == _cameraDragPoint)) {
		// Copied: the call edits the tree, which hands the viewport a new gizmo
		const auto movePoint = _cameraGizmo.movePoint;
		movePoint(_cameraDragPoint);
	}
	needsUpdate();
}

void EditorViewport::cancelCameraDrag() {
	_cameraDragActive = false;
	needsUpdate();
}

void EditorViewport::noteObjectFly(const object* flown, const vec3d& oldPos, const matrix& oldOrient, bool input) {
	// The controls drive the object through physics, so it coasts to a stop after the keys are
	// released; once they are, movement under about a centimeter counts as still
	const bool flew = input || vm_vec_dist_squared(&oldPos, &flown->pos) > 1e-4f ||
		vm_vec_dist_squared(&oldOrient.vec.fvec, &flown->orient.vec.fvec) > 1e-8f ||
		vm_vec_dist_squared(&oldOrient.vec.uvec, &flown->orient.vec.uvec) > 1e-8f;
	if (!flew)
		return;
	_objFlyLastMove = timer_get_milliseconds();
	if (_objFlying)
		return;

	// Start of a flight: the flown object as it was, and every marked object (followers move
	// after this). Docked partners aren't needed: undo re-snaps them to the restored ships.
	_objFlying = true;
	_objFlyStart.clear();
	_objFlyStart.push_back({flown->signature, oldPos, oldOrient});
	for (auto* objp = GET_FIRST(&obj_used_list); objp != END_OF_LIST(&obj_used_list); objp = GET_NEXT(objp)) {
		if (objp->flags[Object::Object_Flags::Marked] && objp != flown)
			_objFlyStart.push_back({objp->signature, objp->pos, objp->orient});
	}
}

void EditorViewport::commitObjectFly() {
	_objFlying = false;
	SCP_vector<ObjectTransform> transforms;
	for (const auto& start : _objFlyStart) {
		const int objnum = obj_get_by_signature(start.signature);
		if (objnum < 0)
			continue;
		const object& obj = Objects[objnum];
		if (vm_vec_cmp(&start.pos, &obj.pos) == 0 && vm_matrix_cmp(&start.orient, &obj.orient) == 0)
			continue;
		transforms.push_back({start.signature, start.pos, start.orient, obj.pos, obj.orient});
	}
	_objFlyStart.clear();
	// pushing runs redo(), which puts the objects where they already are
	if (!transforms.empty() && editor->undoStack() != nullptr)
		editor->undoStack()->push(new MoveObjectsCommand(std::move(transforms), editor, this));
}

void EditorViewport::commitCameraFly() {
	_camFlying = false;
	camera.resetViewPhysics(); // don't carry the flight's speed into the next view
	if (_cameraGizmo.setFromView) {
		// Copied: the call edits the tree, which hands the viewport a new gizmo
		const auto setFromView = _cameraGizmo.setFromView;
		setFromView(_camFlyPos, _camFlyOrient);
	}
	needsUpdate();
}

void EditorViewport::playCamera() {
	CameraSexpPreview preview;
	if (!rawCameraPreview(&preview)) {
		return;
	}
	if (!_camPlayback || preview.opNode != _camPlayOpNode || _camTime >= preview.duration) {
		_camTime = 0.0f;
	}
	_camFlying = false;
	_camPlayback = true;
	_camPlaying = preview.duration > 0.0f;
	_camPlayOpNode = preview.opNode;
	needsUpdate();
}

void EditorViewport::pauseCamera() {
	_camPlaying = false;
	needsUpdate();
}

void EditorViewport::rewindCamera() {
	CameraSexpPreview preview;
	if (!rawCameraPreview(&preview)) {
		return;
	}
	_camFlying = false;
	_camPlayback = true;
	_camPlaying = false;
	_camTime = 0.0f;
	_camPlayOpNode = preview.opNode;
	needsUpdate();
}

void EditorViewport::cameraToEnd() {
	CameraSexpPreview preview;
	if (!rawCameraPreview(&preview)) {
		return;
	}
	_camFlying = false;
	_camPlayback = true;
	_camPlaying = false;
	_camTime = preview.duration;
	_camPlayOpNode = preview.opNode;
	needsUpdate();
}

void EditorViewport::stopCameraPlayback() {
	if (!_camPlayback) {
		return;
	}
	_camPlayback = false;
	_camPlaying = false;
	_camTime = 0.0f;
	_camPlayOpNode = -1;
	needsUpdate();
}

bool EditorViewport::advanceCameraPlayback(float dt) {
	if (!_camPlaying) {
		return false;
	}
	CameraSexpPreview preview;
	if (!rawCameraPreview(&preview) || preview.opNode != _camPlayOpNode) {
		stopCameraPlayback();
		return false;
	}
	_camTime += dt;
	if (_camTime >= preview.duration) {
		_camTime = preview.duration;
		_camPlaying = false;
	}
	needsUpdate();
	return _camPlaying;
}

void EditorViewport::syncCameraPlayback() {
	if (!_camPlayback) {
		return;
	}
	CameraSexpPreview preview;
	if (!rawCameraPreview(&preview) || preview.opNode != _camPlayOpNode) {
		stopCameraPlayback();
	}
}

SCP_vector<CameraEventInfo> EditorViewport::cameraEvents() const {
	if (!_cameraGizmo.events) {
		return {};
	}
	return _cameraGizmo.events();
}

void EditorViewport::setCameraStartsAfter(const SCP_string& name) {
	CameraSexpPreview preview;
	if (!_cameraGizmo.setStartsAfter || !rawCameraPreview(&preview) || preview.eventIndex < 0) {
		return;
	}
	// Copied: the call edits the dialog's events, which can hand the viewport a new gizmo
	const auto setStartsAfter = _cameraGizmo.setStartsAfter;
	setStartsAfter(preview.eventIndex, name);
	needsUpdate();
}

// ---------------------------------------------------------------------------
// Asteroid-field gizmos (edit the global Asteroid_field; an open dialog is
// synced afterwards by the caller)
// ---------------------------------------------------------------------------

namespace {

enum class AstBox { Outer, Inner };
enum class AstCorner { Min, Max };

// The rules AsteroidEditorDialogModel::validate_data() checks on OK: each box
// at least this thick on every axis, and the inner box at least this far
// inside the outer one. Enforced while dragging so a drag can't produce a
// field the dialog would then refuse.
constexpr float kAstMinThickness = 400.0f;

vec3d& astMin(AstBox box) { return box == AstBox::Outer ? Asteroid_field.min_bound : Asteroid_field.inner_min_bound; }
vec3d& astMax(AstBox box) { return box == AstBox::Outer ? Asteroid_field.max_bound : Asteroid_field.inner_max_bound; }

// The range one min/max component may take. When the current field already
// breaks the rules on this axis (a hand-edited mission, say), only the box's
// own thickness is enforced.
void astComponentRange(AstBox box, AstCorner corner, int axis, float* lo, float* hi) {
	const float t = kAstMinThickness;
	const bool inner = Asteroid_field.has_inner_bound;
	*lo = -FLT_MAX;
	*hi = FLT_MAX;
	if (box == AstBox::Outer) {
		if (corner == AstCorner::Min) {
			*hi = astMax(AstBox::Outer).a1d[axis] - t;
			if (inner) *hi = std::min(*hi, Asteroid_field.inner_min_bound.a1d[axis] - t);
		} else {
			*lo = astMin(AstBox::Outer).a1d[axis] + t;
			if (inner) *lo = std::max(*lo, Asteroid_field.inner_max_bound.a1d[axis] + t);
		}
	} else {
		if (corner == AstCorner::Min) {
			*lo = Asteroid_field.min_bound.a1d[axis] + t;
			*hi = astMax(AstBox::Inner).a1d[axis] - t;
		} else {
			*lo = astMin(AstBox::Inner).a1d[axis] + t;
			*hi = Asteroid_field.max_bound.a1d[axis] - t;
		}
	}
	if (*lo > *hi) {
		*lo = -FLT_MAX;
		*hi = FLT_MAX;
		if (corner == AstCorner::Min) *hi = astMax(box).a1d[axis] - t;
		else                          *lo = astMin(box).a1d[axis] + t;
	}
}

// Move one min/max component of a box by delta, clamped. Returns the delta
// actually applied.
float nudgeAstComponent(AstBox box, AstCorner corner, int axis, float delta) {
	if (delta == 0.0f || axis < 0 || axis > 2) {
		return 0.0f;
	}
	float& value = (corner == AstCorner::Min) ? astMin(box).a1d[axis] : astMax(box).a1d[axis];
	float lo, hi;
	astComponentRange(box, corner, axis, &lo, &hi);
	// Never let the clamp itself move the face the other way.
	const float target = std::clamp(value + delta, std::min(lo, value), std::max(hi, value));
	const float applied = target - value;
	value = target;
	return applied;
}

// Translate a whole box by delta. The inner box is part of the outer one, so
// moving the outer box carries the inner box along, unclamped. That includes a
// disabled inner box's stored bounds, so enabling it later puts it where it
// was relative to the outer box. Moving the inner
// box alone is clamped per axis so it stays inside the outer one. Returns the
// delta actually applied.
vec3d translateAst(AstBox box, const vec3d& delta) {
	if (box == AstBox::Outer) {
		vm_vec_add2(&Asteroid_field.min_bound, &delta);
		vm_vec_add2(&Asteroid_field.max_bound, &delta);
		vm_vec_add2(&Asteroid_field.inner_min_bound, &delta);
		vm_vec_add2(&Asteroid_field.inner_max_bound, &delta);
		return delta;
	}

	vec3d applied = vmd_zero_vector;
	vec3d& mn = astMin(AstBox::Inner);
	vec3d& mx = astMax(AstBox::Inner);
	for (int axis = 0; axis < 3; ++axis) {
		const float t = kAstMinThickness;
		const float lo = Asteroid_field.min_bound.a1d[axis] + t - mn.a1d[axis];
		const float hi = Asteroid_field.max_bound.a1d[axis] - t - mx.a1d[axis];
		float d = delta.a1d[axis];
		if (lo <= hi) {
			d = std::clamp(d, std::min(lo, 0.0f), std::max(hi, 0.0f));
		}
		mn.a1d[axis] += d;
		mx.a1d[axis] += d;
		applied.a1d[axis] = d;
	}
	return applied;
}

} // anonymous namespace

void EditorViewport::refreshAsteroidHandles() {
	// Gated on the Scene Browser "Environment" visibility toggle, like the
	// volumetric gizmo — hidden means no handles (and nothing to pick/drag).
	const bool present = editor != nullptr && editor->showEnvironment() &&
		Asteroid_field.num_initial_asteroids > 0;
	const bool inner = present && Asteroid_field.has_inner_bound;
	const bool selected = present && editor != nullptr &&
		editor->currentEnvironment == EnvironmentObject::AsteroidField;

	// Only the single selected/spinbox-target handle renders green. Its index is
	// deterministic: the outer box occupies [0..14] (6 faces, 8 corners, then
	// center at 14); the inner box, when present, follows. Default target is the
	// outer center; a viewport click overrides it via _selected_handle.
	constexpr int kOuterCenterIndex = 14;
	const int astCount = present ? (inner ? 30 : 15) : 0;
	int target = -1;
	if (selected && astCount > 0) {
		target = kOuterCenterIndex;
		const int astGroupIdx = _asteroid_handle_group.valid() ? _asteroid_handle_group.index : -1;
		if (_selected_handle.group_index == astGroupIdx &&
			_selected_handle.handle_index >= 0 && _selected_handle.handle_index < astCount) {
			target = _selected_handle.handle_index;
		}
	}

	// Dirty check: bounds + toggles + selection + which handle is the target.
	// Only touch the registry when something changed, or the per-frame refresh
	// would spin needsUpdate().
	const vec3d bounds[4] = {Asteroid_field.min_bound, Asteroid_field.max_bound,
		Asteroid_field.inner_min_bound, Asteroid_field.inner_max_bound};
	bool boundsSame = true;
	for (int i = 0; i < 4; ++i) {
		if (!(bounds[i] == _ast_handle_cached_bounds[i])) {
			boundsSame = false;
			break;
		}
	}
	if (present == _ast_handle_cached_present && inner == _ast_handle_cached_inner &&
		selected == _ast_handle_cached_selected && target == _ast_handle_cached_target &&
		(!present || boundsSame)) {
		return;
	}
	_ast_handle_cached_present = present;
	_ast_handle_cached_inner = inner;
	_ast_handle_cached_selected = selected;
	_ast_handle_cached_target = target;
	for (int i = 0; i < 4; ++i) {
		_ast_handle_cached_bounds[i] = bounds[i];
	}

	std::vector<ViewportHandle> handles;
	_asteroid_center_index = -1;

	auto axisAllowed = [this](int axis) {
		return [this, axis]() {
			switch (axis) {
			case 0: return Constraint.xyz.x != 0.0f;
			case 1: return Constraint.xyz.y != 0.0f;
			case 2: return Constraint.xyz.z != 0.0f;
			default: return true;
			}
		};
	};
	// Only a real change dirties the mission (a clamped-out drag changes nothing).
	auto afterEdit = [this](const vec3d& applied) {
		if (applied.xyz.x != 0.0f || applied.xyz.y != 0.0f || applied.xyz.z != 0.0f) {
			envGizmoChanged(EnvironmentObject::AsteroidField);
		}
		return applied;
	};

	auto buildBox = [&](AstBox box, const vec3d& mn, const vec3d& mx, bool isOuter,
	                    int fr, int fg, int fb, int cr, int cg, int cb, int mr, int mg, int mb) {
		const vec3d center{{{(mn.xyz.x + mx.xyz.x) * 0.5f, (mn.xyz.y + mx.xyz.y) * 0.5f,
			(mn.xyz.z + mx.xyz.z) * 0.5f}}};

		// Six faces — each editable only along its normal axis.
		struct FaceSpec { int axis; bool is_max; vec3d pos; vec3d normal; };
		const FaceSpec faces[6] = {
			{0, false, {{{mn.xyz.x, center.xyz.y, center.xyz.z}}}, {{{-1.0f, 0.0f, 0.0f}}}},
			{0, true,  {{{mx.xyz.x, center.xyz.y, center.xyz.z}}}, {{{ 1.0f, 0.0f, 0.0f}}}},
			{1, false, {{{center.xyz.x, mn.xyz.y, center.xyz.z}}}, {{{0.0f, -1.0f, 0.0f}}}},
			{1, true,  {{{center.xyz.x, mx.xyz.y, center.xyz.z}}}, {{{0.0f,  1.0f, 0.0f}}}},
			{2, false, {{{center.xyz.x, center.xyz.y, mn.xyz.z}}}, {{{0.0f, 0.0f, -1.0f}}}},
			{2, true,  {{{center.xyz.x, center.xyz.y, mx.xyz.z}}}, {{{0.0f, 0.0f,  1.0f}}}},
		};
		for (const auto& f : faces) {
			ViewportHandle h;
			h.kind = ViewportHandle::Kind::Face;
			h.world_pos = f.pos;
			h.axis = f.normal;
			h.color_r = fr; h.color_g = fg; h.color_b = fb;
			h.is_selected = (static_cast<int>(handles.size()) == target);
			h.show_coords = true;
			h.movable_axes = 1 << f.axis;
			h.is_enabled = axisAllowed(f.axis);
			const AstCorner corner = f.is_max ? AstCorner::Max : AstCorner::Min;
			const int axis = f.axis;
			h.on_drag = [box, corner, axis, afterEdit](const vec3d& delta) {
				vec3d applied = vmd_zero_vector;
				applied.a1d[axis] = nudgeAstComponent(box, corner, axis, delta.a1d[axis]);
				return afterEdit(applied);
			};
			handles.push_back(std::move(h));
		}

		// Eight corners — resize the three adjacent faces together.
		for (int xi = 0; xi < 2; ++xi) {
			for (int yi = 0; yi < 2; ++yi) {
				for (int zi = 0; zi < 2; ++zi) {
					ViewportHandle h;
					h.kind = ViewportHandle::Kind::Corner;
					h.world_pos = vec3d{{{xi ? mx.xyz.x : mn.xyz.x, yi ? mx.xyz.y : mn.xyz.y,
						zi ? mx.xyz.z : mn.xyz.z}}};
					h.axis = vec3d{{{xi ? 1.0f : -1.0f, yi ? 1.0f : -1.0f, zi ? 1.0f : -1.0f}}};
					h.color_r = cr; h.color_g = cg; h.color_b = cb;
					h.is_selected = (static_cast<int>(handles.size()) == target);
					h.show_coords = true;
					const AstCorner cx = xi ? AstCorner::Max : AstCorner::Min;
					const AstCorner cy = yi ? AstCorner::Max : AstCorner::Min;
					const AstCorner cz = zi ? AstCorner::Max : AstCorner::Min;
					h.on_drag = [box, cx, cy, cz, afterEdit](const vec3d& delta) {
						vec3d applied;
						applied.xyz.x = nudgeAstComponent(box, cx, 0, delta.xyz.x);
						applied.xyz.y = nudgeAstComponent(box, cy, 1, delta.xyz.y);
						applied.xyz.z = nudgeAstComponent(box, cz, 2, delta.xyz.z);
						return afterEdit(applied);
					};
					handles.push_back(std::move(h));
				}
			}
		}

		// Center — translates the whole box.
		{
			ViewportHandle h;
			h.kind = ViewportHandle::Kind::Center;
			h.world_pos = center;
			h.color_r = mr; h.color_g = mg; h.color_b = mb;
			h.is_selected = (static_cast<int>(handles.size()) == target);
			h.show_coords = true;
			h.show_grid_position = true;  // both centers drop a grid line
			if (isOuter) {
				h.info_label = "Asteroid Field";  // name shown only for the main center
				_asteroid_center_index = static_cast<int>(handles.size());
			}
			h.on_drag = [box, afterEdit](const vec3d& delta) {
				return afterEdit(translateAst(box, delta));
			};
			handles.push_back(std::move(h));
		}
	};

	if (present) {
		vec3d mn = Asteroid_field.min_bound, mx = Asteroid_field.max_bound;
		buildBox(AstBox::Outer, mn, mx, true, 255, 160, 64, 255, 200, 64, 255, 220, 96);
		if (inner) {
			vec3d imn = Asteroid_field.inner_min_bound, imx = Asteroid_field.inner_max_bound;
			buildBox(AstBox::Inner, imn, imx, false, 64, 220, 120, 96, 240, 140, 128, 255, 160);
		}
	}

	// A rebuild that changed the handle set can invalidate the spinbox target.
	if (_selected_handle.group_index >= 0 && _asteroid_handle_group.valid() &&
		_selected_handle.group_index == _asteroid_handle_group.index &&
		_selected_handle.handle_index >= static_cast<int>(handles.size())) {
		_selected_handle = HandlePick{};
	}

	if (_asteroid_handle_group.valid()) {
		updateHandleGroup(_asteroid_handle_group, std::move(handles));
	} else {
		_asteroid_handle_group = registerHandleGroup(std::move(handles));
	}
}

bool EditorViewport::asteroidSpinboxTarget(vec3d* out_pos, int* out_movable_axes) const {
	if (!_asteroid_handle_group.valid() ||
		_asteroid_handle_group.index >= static_cast<int>(_handle_groups.size())) {
		return false;
	}
	const auto& group = _handle_groups[_asteroid_handle_group.index];
	if (group.empty()) {
		return false;
	}

	int idx = _asteroid_center_index;
	// Prefer the explicitly clicked handle when it belongs to this field.
	if (_selected_handle.group_index == _asteroid_handle_group.index &&
		_selected_handle.handle_index >= 0 &&
		_selected_handle.handle_index < static_cast<int>(group.size())) {
		idx = _selected_handle.handle_index;
	}
	if (idx < 0 || idx >= static_cast<int>(group.size())) {
		return false;
	}
	if (out_pos != nullptr) {
		*out_pos = group[idx].world_pos;
	}
	if (out_movable_axes != nullptr) {
		*out_movable_axes = group[idx].movable_axes;
	}
	return true;
}

void EditorViewport::applyAsteroidSpinbox(const vec3d& new_pos) {
	if (!_asteroid_handle_group.valid() ||
		_asteroid_handle_group.index >= static_cast<int>(_handle_groups.size())) {
		return;
	}
	const auto& group = _handle_groups[_asteroid_handle_group.index];
	int idx = _asteroid_center_index;
	if (_selected_handle.group_index == _asteroid_handle_group.index &&
		_selected_handle.handle_index >= 0 &&
		_selected_handle.handle_index < static_cast<int>(group.size())) {
		idx = _selected_handle.handle_index;
	}
	if (idx < 0 || idx >= static_cast<int>(group.size())) {
		return;
	}
	// Copy before invoking: on_drag can rebuild the group (and this callback).
	auto on_drag_copy = group[idx].on_drag;
	vec3d delta;
	vm_vec_sub(&delta, &new_pos, &group[idx].world_pos);
	if (on_drag_copy) {
		on_drag_copy(delta);
	}
}

namespace {
QByteArray captureEnvGizmoState(EnvironmentObject env) {
	switch (env) {
	case EnvironmentObject::VolumetricNebula: return dialogs::VolumetricNebulaDialogModel::captureGizmoState();
	case EnvironmentObject::AsteroidField:    return dialogs::AsteroidEditorDialogModel::captureGizmoState();
	default:                                  return {};
	}
}
} // anonymous namespace

void EditorViewport::envGizmoChanged(EnvironmentObject env) {
	if (env == EnvironmentObject::VolumetricNebula && _volEditModel != nullptr) {
		_volEditModel->syncGizmoFromGlobals(false);
	} else if (env == EnvironmentObject::AsteroidField && _astEditModel != nullptr) {
		_astEditModel->syncGizmoFromGlobals(false);
	}
	if (editor != nullptr) {
		editor->missionChanged();
	}
	needsUpdate();
}

bool EditorViewport::moveVolumetricTo(const vec3d& pos) {
	if (!The_mission.volumetrics || The_mission.volumetrics->getPos() == pos) {
		return false;
	}
	The_mission.volumetrics->setPos(pos);
	envGizmoChanged(EnvironmentObject::VolumetricNebula);
	return true;
}

void EditorViewport::beginEnvEdit(EnvironmentObject env) {
	_env_edit_kind = env;
	_env_edit_before = captureEnvGizmoState(env);
}

void EditorViewport::commitEnvEdit(const QString& text) {
	if (!envEditActive()) {
		return;
	}
	const auto env = _env_edit_kind;
	const QByteArray before = _env_edit_before;
	_env_edit_kind = EnvironmentObject::None;
	_env_edit_before.clear();

	const QByteArray after = captureEnvGizmoState(env);
	if (after == before) {
		return;
	}

	// With the dialog open the edit belongs to its session, so it goes on the
	// dialog's stack: OK keeps it, Cancel drops it with everything else.
	if (env == EnvironmentObject::VolumetricNebula && _volEditModel != nullptr) {
		Q_EMIT _volEditModel->gizmoEditCommitted(before, after, text);
		return;
	}
	if (env == EnvironmentObject::AsteroidField && _astEditModel != nullptr) {
		Q_EMIT _astEditModel->gizmoEditCommitted(before, after, text);
		return;
	}

	if (editor != nullptr && editor->undoStack() != nullptr) {
		const auto kind = (env == EnvironmentObject::VolumetricNebula) ? dialogs::EnvEditCommand::Kind::VolumetricNebula
		                                                               : dialogs::EnvEditCommand::Kind::AsteroidField;
		editor->undoStack()->push(new dialogs::EnvEditCommand(kind, this, before, after, text));
	}
}

void EditorViewport::cancelEnvEdit() {
	if (!envEditActive()) {
		return;
	}
	const auto env = _env_edit_kind;
	const QByteArray before = _env_edit_before;
	_env_edit_kind = EnvironmentObject::None;
	_env_edit_before.clear();

	if (captureEnvGizmoState(env) == before) {
		return;
	}
	if (env == EnvironmentObject::VolumetricNebula) {
		dialogs::VolumetricNebulaDialogModel::restoreGizmoState(before);
	} else {
		dialogs::AsteroidEditorDialogModel::restoreGizmoState(before);
	}
	envGizmoChanged(env);
}

void EditorViewport::refreshVolumetricHandle() {
	// The gizmo is present whenever the mission has an enabled volumetric with a
	// hull (matching what the visualizer actually draws) and the environment is
	// not hidden via the Scene Browser toggle.
	const bool present = editor != nullptr && editor->showEnvironment() && The_mission.volumetrics &&
		The_mission.volumetrics->get_enabled() && !The_mission.volumetrics->getHullPof().empty();

	vec3d pos = vmd_zero_vector;
	SCP_string label;
	int cr = 255, cg = 255, cb = 255;
	if (present) {
		pos = The_mission.volumetrics->getPos();
		label = The_mission.volumetrics->getHullPof();
		const auto& col = The_mission.volumetrics->getNebulaColor();
		// nebulaColor components are 0..1; clamp the low end so the marker is
		// never near-black against the hull.
		cr = std::clamp(static_cast<int>(std::get<0>(col) * 255.0f), 64, 255);
		cg = std::clamp(static_cast<int>(std::get<1>(col) * 255.0f), 64, 255);
		cb = std::clamp(static_cast<int>(std::get<2>(col) * 255.0f), 64, 255);
	}
	const int packed_color = present ? ((cr << 16) | (cg << 8) | cb) : -1;
	const bool selected = present && editor != nullptr &&
		editor->currentEnvironment == EnvironmentObject::VolumetricNebula;

	// Only touch the registry when the rendered state actually changed —
	// updateHandleGroup calls needsUpdate(), and this runs every frame, so an
	// unconditional rebuild would spin the repaint loop.
	if (present == _vol_handle_cached_present &&
		(!present ||
			(pos == _vol_handle_cached_pos && label == _vol_handle_cached_label &&
				packed_color == _vol_handle_cached_color && selected == _vol_handle_cached_selected))) {
		return;
	}
	_vol_handle_cached_present = present;
	_vol_handle_cached_pos = pos;
	_vol_handle_cached_label = label;
	_vol_handle_cached_color = packed_color;
	_vol_handle_cached_selected = selected;

	std::vector<ViewportHandle> handles;
	if (present) {
		ViewportHandle h;
		h.kind = ViewportHandle::Kind::Center;
		h.world_pos = pos;
		h.color_r = cr;
		h.color_g = cg;
		h.color_b = cb;
		h.info_label = label;
		h.is_selected = selected;
		h.show_coords = true;
		h.show_grid_position = true;
		h.on_drag = [this](const vec3d& delta) {
			if (!The_mission.volumetrics) {
				return vmd_zero_vector;
			}
			vec3d p = The_mission.volumetrics->getPos();
			vm_vec_add2(&p, &delta);
			moveVolumetricTo(p);
			return delta;
		};
		handles.push_back(std::move(h));
	}

	if (_volumetric_handle_group.valid()) {
		updateHandleGroup(_volumetric_handle_group, std::move(handles));
	} else {
		_volumetric_handle_group = registerHandleGroup(std::move(handles));
	}
}

} // namespace fso::fred
