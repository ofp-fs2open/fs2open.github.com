#pragma once


#include "CameraController.h"
#include "CameraSexpPreview.h"
#include "FredRenderer.h"
#include "Editor.h"
#include "IDialogProvider.h"
#include "ui/ThemeMode.h"
#include "ViewportHandle.h"

#include <object/object.h>

#include <QByteArray>
#include <QString>

#include <functional>

namespace fso::fred {

namespace dialogs {
class BackgroundEditorDialogModel;
class VolumetricNebulaDialogModel;
class AsteroidEditorDialogModel;
}

// Eye-space distance at which background-element handles are placed for
// projection and picking. Backgrounds are effectively at infinity, so only the
// direction matters; any positive value projects to the same screen point. Both
// the picker (EditorViewport) and the renderer (FredRenderer) use this so the
// drawn handle and its click target stay in sync.
constexpr float BG_HANDLE_DISTANCE = 1000.0f;

// Defined in Editor.h. Forward-declared here because Editor.h and
// EditorViewport.h include each other: when a TU enters Editor.h first, this
// header is pulled in before Editor.h's full enum definition. Only used as a
// return type in a declaration below, so a forward declaration suffices.
enum class EnvironmentObject;

// The camera sexp selected in a sexp tree, shown in the viewport (the frustum, a handle for its
// point, and the Cutscene Camera viewpoint). Editor-only and never saved; the tree that set it
// clears it again.
struct CameraGizmo {
	const void* owner = nullptr;
	// Works the selected camera sexp out again from its tree. Called every frame, so a host or
	// target that moves is followed; false once the tree no longer has a camera sexp selected.
	std::function<bool(CameraSexpPreview&)> evaluate;
	// Writes the operator's point (camera position or facing point) back into its arguments
	std::function<void(const vec3d&)> movePoint;
	// Sets the selected sexp from a view: a position's position, a rotation's angles, or a
	// facing's point
	std::function<void(const vec3d&, const matrix&)> setFromView;
	// Writes a field of view (engine zoom units) into the selected set-camera-fov
	std::function<void(float)> setFov;
	// The tree's events, when it holds events (the Events editor), and choosing an event's
	// starts-after event; empty functions otherwise
	std::function<SCP_vector<CameraEventInfo>()> events;
	std::function<void(int, const SCP_string&)> setStartsAfter;
};

struct Marking_box {
	int x1 = 0;
	int y1 = 0;
	int x2 = 0;
	int y2 = 0;
};

enum class CreateKind {
	Ship,
	Prop,
	Other,
};

enum class OtherKind {
	Waypoint,
	JumpNode,
	CoordinatePoint,
};

enum class DataMenuStyle {
	Auto = 0,
	Columns = 1,
	Searchable = 2,
};

struct ViewSettings {
	bool Universal_heading = false;
	bool Show_stars = true;
	bool Show_horizon = false;
	bool Show_grid = true;
	bool Show_distances = true;
	bool Show_coordinates = false;
	bool Show_outlines = false;
	bool Draw_outlines_on_selected_ships = true;
	bool Draw_outline_at_warpin_position = false;
	bool Show_grid_positions = true;
	bool Show_dock_points = false;
	bool Show_bay_paths = false;
	bool Show_starts = true;
	bool Show_ships = true;
	SCP_vector<bool> Show_iff;
	bool Show_ship_info = true;
	bool Show_ship_models = true;
	bool Show_paths_fred = false;
	bool Lighting_on = false;
	bool FullDetail = false;
	bool Show_waypoints = true;
	bool Show_props = true;
	bool Show_jump_nodes = true;
	bool Show_coordinate_points = true;
	bool Show_compass = true;
	bool Show_camera_gizmo = true;
	bool Highlight_selectable_subsys = false;
	int Outline_lod = 1;
	float Label_font_scale = 1.0f;  // multiplier applied to viewport text labels

	ViewSettings();
};

class EditorViewport {
	std::unique_ptr<FredRenderer> _renderer; //!< Internal, owned pointer

	int Last_cursor_over = -1;

	void process_system_keys();
	void level_object(matrix* orient);

	void initialSetup();

	SCP_vector<SCP_string> _layerNames;
	SCP_vector<bool> _layerVisibility;
	std::unordered_map<int, size_t> _objectLayers;

	size_t getLayerIndex(const SCP_string& name) const;
	size_t getObjectLayerIndex(int objectIndex) const;
	bool isLayerVisible(size_t layerIndex) const;
	void syncMissionLayerNames() const;
	void setObjectLayerByIndex(int objectIndex, size_t layerIndex);

 public:
	class ViewportControlLock {
	  public:
		explicit ViewportControlLock(EditorViewport* viewport);
		~ViewportControlLock();

		ViewportControlLock(const ViewportControlLock&) = delete;
		ViewportControlLock& operator=(const ViewportControlLock&) = delete;

		ViewportControlLock(ViewportControlLock&& other) noexcept;
		ViewportControlLock& operator=(ViewportControlLock&& other) noexcept;

	  private:
		EditorViewport* _viewport = nullptr;
	};

	static const char* DefaultLayerName;

	enum {
		DUP_DRAG_OF_WING = 2,
		// Ctrl+Shift+drag.  Same as a normal Ctrl+drag duplicate, except marked
		// waypoints insert a copy into their source path rather than starting a
		// new path.  Non-waypoint object types behave like a plain duplicate.
		DUP_DRAG_INSERT = 3,
	};

	EditorViewport(Editor* in_editor, std::unique_ptr<FredRenderer>&& in_renderer);

	void needsUpdate();
	bool areControlsLocked() const;
	[[nodiscard]] ViewportControlLock acquireControlLock();

	void reset();

	void select_objects(const Marking_box& box);

	void game_do_frame(const int cur_object_index);

	vec3d orbitCameraGetPivot();

	int object_check_collision(object* objp, vec3d* p0, vec3d* p1, vec3d* hitpos);

	int select_object(int cx, int cy);

	// --- Background element mouse editing --------------------------------
	// Set while the Background editor dialog is open so the viewport can draw
	// draggable handles for its suns/bitmaps and route clicks/drags to it.
	dialogs::BackgroundEditorDialogModel* backgroundEditModel() const { return _bgEditModel; }
	void setBackgroundEditModel(dialogs::BackgroundEditorDialogModel* model);

	// --- Environment gizmo editing ---------------------------------------
	// Set while the corresponding editor dialog is open. The always-on
	// volumetric/asteroid gizmos edit the mission globals directly either way;
	// with a dialog open they also sync the result into its model, so OK
	// applies the edit and Cancel reverts it.
	dialogs::VolumetricNebulaDialogModel* volumetricEditModel() const { return _volEditModel; }
	void setVolumetricEditModel(dialogs::VolumetricNebulaDialogModel* model);
	dialogs::AsteroidEditorDialogModel* asteroidEditModel() const { return _astEditModel; }
	void setAsteroidEditModel(dialogs::AsteroidEditorDialogModel* model);

	// One gizmo edit (a drag, or a transform-toolbar change) as a transaction.
	// begin snapshots the fields gizmos can change; commit records one undo step
	// if anything changed: on the open dialog's stack if there is one, else on
	// the main stack. cancel restores the snapshot and records nothing.
	void beginEnvEdit(EnvironmentObject env);
	void commitEnvEdit(const QString& text);
	void cancelEnvEdit();
	bool envEditActive() const { return !_env_edit_before.isEmpty(); }

	// Move the volumetric nebula (transform toolbar). Returns true if it moved.
	bool moveVolumetricTo(const vec3d& pos);

	// Pick the background element (sun or bitmap) whose projected handle is
	// nearest the cursor. Returns true and fills isSun/index on a hit.
	bool select_background_element(int cx, int cy, bool& isSun, int& index) const;

	// Begin/continue/finish dragging the given background element. Move mode
	// re-points it at the cursor; rotate mode adjusts a bitmap's bank.
	void begin_background_drag(bool isSun, int index);
	void drag_background_element(int x, int y);
	void rotate_background_element(int mouse_dx);
	void end_background_drag();

	// Viewport handle (non-object selectable marker) API. The environment
	// gizmos register one group each; the picking pre-pass below runs before
	// select_object() so a handle click never falls through into normal mission
	// object selection.
	HandleGroupId registerHandleGroup(std::vector<ViewportHandle> handles);
	void updateHandleGroup(HandleGroupId id, std::vector<ViewportHandle> handles);
	void unregisterHandleGroup(HandleGroupId id);
	const std::vector<std::vector<ViewportHandle>>& getHandleGroups() const { return _handle_groups; }

	// Returns {group_index, handle_index} or {-1, -1} if nothing within pick
	// radius. group_index is the slot in _handle_groups, NOT the generation id.
	struct HandlePick { int group_index = -1; int handle_index = -1; };
	HandlePick pick_handle(int cx, int cy) const;

	// Begin/continue/end a handle drag. begin_handle_drag records the anchor
	// point on the constraint plane; drag_handle delivers per-tick deltas via
	// the handle's on_drag callback. Returns false if the active handle was
	// invalidated (e.g. its group was unregistered mid-drag).
	bool begin_handle_drag(HandlePick pick, int cx, int cy);
	bool drag_handle(int cx, int cy);
	void end_handle_drag();
	bool has_active_handle_drag() const { return _active_handle.group_index >= 0; }

	// Hovered-handle tracking for the in-scene hover balloon. RenderWidget sets
	// this on mouse-move; FredRenderer reads it to draw the infobox.
	void setHoveredHandle(HandlePick pick) { _hovered_handle = pick; }
	HandlePick getHoveredHandle() const { return _hovered_handle; }

	// Viewport-owned volumetric nebula gizmo, present whenever the mission has
	// an enabled volumetric with a hull, so the nebula can be dragged with or
	// without its dialog open. refreshVolumetricHandle() rebuilds it from
	// The_mission when its state actually changes (cheap no-op otherwise); it
	// is called each frame from the renderer.
	void refreshVolumetricHandle();

	// Viewport-owned asteroid-field gizmos (outer box, and inner box when
	// enabled): 6 face + 8 corner + 1 center handle per box, rebuilt from
	// Asteroid_field. Always on, like the volumetric handle. Drags keep each box
	// at least 400 thick and the inner box 400 inside the outer one, the same
	// rules the dialog checks on OK. Called each frame from the renderer
	// (dirty-checked).
	void refreshAsteroidHandles();

	// Which environment entity (if any) a picked handle belongs to. The
	// viewport-owned volumetric and asteroid gizmos map to one; anything else
	// returns None. Used by the widget to drive environment selection.
	EnvironmentObject handleEnvironment(HandlePick pick) const;

	// --- Cutscene camera preview -------------------------------------------
	// Set by a sexp tree while one of its camera sexps is selected; clearCameraGizmo() only
	// clears it for the owner that set it.
	void setCameraGizmo(CameraGizmo gizmo);
	void clearCameraGizmo(const void* owner);
	bool hasCameraGizmo() const { return static_cast<bool>(_cameraGizmo.evaluate); }
	// The selected camera sexp worked out now; false if there is none
	bool cameraPreview(CameraSexpPreview* out) const;
	// Whether the selected camera sexp can take the view (Set Camera SEXP from View), and doing
	// it with the current eye
	bool canSetCameraFromView() const;
	void setCameraFromView();
	// Whether the selected camera sexp is a set-camera-fov that can take a field of view, and
	// writing one (engine zoom units)
	bool canSetCameraFov() const;
	void setCameraFov(float fov);

	// Playing the selected sexp's shot. Stopped, the preview is the shot's start or end (see
	// CameraSexpPreview); once started (Play, |< or >|) it is the shot at the playback time. It
	// ends when the selection or the tree changes, the view leaves the cutscene camera, or the
	// camera is edited (flown, or its FOV changed).
	bool cameraPlaybackActive() const { return _camPlayback; }
	bool cameraPlaying() const { return _camPlaying; }
	float cameraPlaybackTime() const { return _camTime; }
	void playCamera();
	void pauseCamera();
	void rewindCamera();
	void cameraToEnd();
	void stopCameraPlayback();
	// Moves playback on by dt seconds; true while it still plays
	bool advanceCameraPlayback(float dt);
	// Stops playback once another camera sexp is selected; called on each idle tick
	void syncCameraPlayback();

	// The selecting tree's events (empty unless it holds events), and choosing the selected
	// sexp's event's starts-after event (empty = automatic, SEXP_NONE_STRING = a new camera)
	SCP_vector<CameraEventInfo> cameraEvents() const;
	void setCameraStartsAfter(const SCP_string& name);
	// The handle for the selected sexp's point, rebuilt each frame from the renderer. A drag
	// moves the handle only; its release writes the sexp once, so it is one undo step.
	void refreshCameraHandle();
	bool isCameraHandle(HandlePick pick) const;
	void beginCameraDrag();
	// Where the handle is while it is being dragged
	bool cameraDragPoint(vec3d* out) const;
	void commitCameraDrag();
	void cancelCameraDrag();

	// The specific handle the transform-toolbar spinboxes act on (for the
	// asteroid field, which has many handles). Set on a viewport handle click;
	// cleared to fall back to the field's outer-box center.
	void setSelectedHandle(HandlePick pick) { _selected_handle = pick; }
	void clearSelectedHandle() { _selected_handle = HandlePick{}; }

	// Read/write the currently targeted asteroid handle for the spinboxes.
	// asteroidSpinboxTarget fills the handle's world position and its editable
	// axis bitmask, defaulting to the outer-box center; returns false if there
	// is no asteroid field. applyAsteroidSpinbox moves that handle so its
	// position becomes new_pos (delta routed through the handle's on_drag, so
	// clamping and mission-modified marking happen there).
	bool asteroidSpinboxTarget(vec3d* out_pos, int* out_movable_axes) const;
	void applyAsteroidSpinbox(const vec3d& new_pos);

	SCP_vector<SCP_string> getLayerNames() const;
	bool addLayer(const SCP_string& name, SCP_string* errorMessage = nullptr);
	bool deleteLayer(const SCP_string& name, SCP_string* errorMessage = nullptr);
	bool renameLayer(const SCP_string& oldName, const SCP_string& newName, SCP_string* errorMessage = nullptr);
	bool setLayerVisibility(const SCP_string& name, bool visible, SCP_string* errorMessage = nullptr);
	bool getLayerVisibility(const SCP_string& name, bool* visible, SCP_string* errorMessage = nullptr) const;
	void showAllLayers();
	int getHiddenLayerCount() const;
	void reloadLayersFromMission();

	SCP_string getObjectLayerName(int objectIndex) const;
	bool moveObjectToLayer(int objectIndex, const SCP_string& layerName, SCP_string* errorMessage = nullptr);
	void moveMarkedObjectsToLayer(const SCP_string& layerName, SCP_string* errorMessage = nullptr);

	void registerObjectInLayer(int objectIndex);

	bool isObjectVisibleInLayer(const object* objp) const;
	// Whether the user could select ptr in the viewport: not hidden or locked from editing, on a
	// shown layer, and of a type (and for ships an IFF) the Layer Manager's filters show. Box select and
	// Select > Select All / Invert Selection use it.
	bool isObjectSelectable(const object* ptr) const;


	// viewpoint -> attach camera to current ship.
	// cur_obj -> ship viewed.
	void level_controlled();
	void verticalize_controlled();

	void drag_rotate_save_backup();

	int create_object_on_grid(int x, int y, int waypoint_instance);
	int create_object_on_grid(int x, int y, int waypoint_instance, CreateKind kind);

	int	create_object(vec3d *pos, int waypoint_instance = -1, CreateKind kind = CreateKind::Ship);

	vec3d getCreatePosition(int x, int y, float fallbackDist);
	int createShipAtScreenPos(int x, int y, int modelIndex);
	int createPropAtScreenPos(int x, int y, int propIndex);
	int createWaypointAtScreenPos(int x, int y, int waypoint_instance = -1);
	int createJumpNodeAtScreenPos(int x, int y);
	int createCoordinatePointAtScreenPos(int x, int y);

	// When `insert_waypoints` is true, marked waypoints get a new waypoint
	// inserted into their source path (right after the source waypoint) instead
	// of being duplicated into a fresh path.  Other object types are duplicated
	// either way.  Triggered from Ctrl+Shift+drag in the viewport.
	int duplicate_marked_objects(bool insert_waypoints = false);
	int drag_objects(int x, int y);

	int drag_rotate_objects(int mouse_dx, int mouse_dy);
	// Carries the other marked objects along after the leader moved and turned. rotmat is the
	// leader's turn (new orient = vm_matrix_x_matrix(old orient, rotmat)); the old pos/orient are
	// the leader's from before the change. Follows Pivot_mode.
	void follow_leader(const object* leader, const vec3d& leader_old_pos, const matrix& leader_old_orient,
		const matrix& rotmat) const;
	void cancel_drag();

	void view_universe(bool just_marked);

	void view_object(int obj_num);

	CameraController camera;

	// Viewpoints (CameraController::getViewpoint()): 0 the editor camera, 1 through an object,
	// 2 through the selected cutscene camera sexp
	static constexpr int CutsceneCameraViewpoint = 2;

	// Field of view for this viewport's camera, as the engine's g3 zoom (radians; the degrees
	// shown to users are fl_degrees() of it, like the in-game option and the fov sexps). The
	// basic editor camera always uses FRED_DEFAULT_HTL_FOV; viewing through an object uses the
	// object-view FOV, which starts at the in-game FOV and can be changed for the session. The
	// cutscene camera viewpoint uses the camera sexps' FOV.
	float viewFov() const;
	float objectViewFov() const { return _objectViewFov; }
	void setObjectViewFov(float fov); // clamped to the range of the game's FOV option
	void resetObjectViewFov();        // back to the in-game FOV
	static constexpr float MinObjectViewFov = 0.436332f; // same range as the Graphics.FOV option
	static constexpr float MaxObjectViewFov = 1.5708f;

	ViewSettings view;

	int Cursor_over = -1;
	CursorMode Editing_mode = CursorMode::Moving;

	grid* The_grid;

	vec3d Constraint;
	vec3d Anticonstraint;
	bool Single_axis_constraint = false;

	bool Selection_lock = false;

	bool button_down = false;
	int on_object = -1;
	int Dup_drag = 0;

	int cur_model_index = 0;
	int cur_prop_index = -1;
	OtherKind cur_other_kind = OtherKind::Waypoint;

	object_orient_pos rotation_backup[MAX_OBJECTS];

	vec3d original_pos = vmd_zero_vector;

	bool moved = false;

	int Duped_wing;

	PivotMode Pivot_mode = PivotMode::Group;
	int  toolbar_icon_size = 24;  ///< Toolbar icon size in pixels (16, 24, or 32)
	int  sexp_number_every_n = 5; ///< Show a numbered badge on every Nth argument in sexp trees (0 = disabled)
	bool Offer_autosave_recovery   = true;
	int  autosave_interval_seconds = 300;  // 5 minutes; 0 = disabled
	bool Create_bak_on_save        = true;
	// QUndoStack limit; 0 = unlimited, but we default to 200 until memory impact
	// of large object/dialog commands on big missions has been measured.
	int  undo_stack_depth          = 200;
	bool Move_ships_when_undocking = true;
	bool Always_save_display_names = false;
	bool Error_checker_checks_potential_issues = true;
	bool Error_checker_apply_auto_corrections = true;
	// One-shot override: when set, the next auto-run of the error checker shows
	// the dialog and forces potential issues on regardless of the user's saved
	// preference. Consumed (cleared) by autoRunErrorChecker. Not persisted.
	bool Error_checker_force_display_potentials_once = false;

	bool Show_sexp_help_mission_events = true;
	bool Show_sexp_help_mission_goals = true;
	bool Show_sexp_help_mission_cutscenes = true;
	bool Show_sexp_help_ship_editor = false;
	bool Show_sexp_help_wing_editor = false;
	bool Show_sexp_help_prop_editor = false;

	ThemeMode Theme_mode = ThemeMode::System;

	DataMenuStyle Data_menu_style = DataMenuStyle::Auto;

	void saveSettings() const;

	Editor* editor = nullptr;
	FredRenderer* renderer = nullptr;
	IDialogProvider* dialogProvider = nullptr;

private:
	float _objectViewFov = 0.75f; // set from the in-game FOV by resetObjectViewFov()

	// Background editor integration (non-owning; valid only while the dialog lives)
	dialogs::BackgroundEditorDialogModel* _bgEditModel = nullptr;
	dialogs::VolumetricNebulaDialogModel* _volEditModel = nullptr;
	dialogs::AsteroidEditorDialogModel* _astEditModel = nullptr;
	// The open gizmo edit transaction (see beginEnvEdit). Empty = none.
	EnvironmentObject _env_edit_kind{}; // None; only forward-declared here, so no enumerator names
	QByteArray _env_edit_before;
	// After a gizmo changed the mission: sync the open dialog (if any), mark the
	// mission changed and repaint.
	void envGizmoChanged(EnvironmentObject env);
	// Active background drag: -1 = none, else index into suns/bitmaps of the
	// active background (which list is chosen by _bgDragIsSun).
	int  _bgDragIndex = -1;
	bool _bgDragIsSun = false;

	fix _lasttime = 0;
	vec3d Last_control_pos = vmd_zero_vector;
	matrix Last_control_orient = vmd_identity_matrix;
	int _controlLockCount = 0;

	bool incMissionTime();
	void loadSettings();

	void lockControls();
	void unlockControls();

	// Handle registry. Slots are never reused; _handle_group_generations[i]
	// increments when slot i is unregistered, so a stale HandleGroupId (or an
	// in-progress drag on that group) fails the generation check.
	std::vector<std::vector<ViewportHandle>> _handle_groups;
	std::vector<int> _handle_group_generations;

	// Active handle drag state. group_index = -1 means no drag in progress.
	HandlePick _active_handle{};
	int _active_handle_generation = 0;
	vec3d _active_handle_last_world = vmd_zero_vector;

	// Handle currently under the cursor (for the hover balloon). {-1,-1} = none.
	// _last_hovered_handle lets game_do_frame schedule a repaint when the hover
	// changes, mirroring Cursor_over / Last_cursor_over for objects.
	HandlePick _hovered_handle{};
	HandlePick _last_hovered_handle{};

	// Cutscene camera preview. The handle cache works like the volumetric one below; while the
	// handle is dragged, _cameraDragPoint is where it has been dragged to.
	CameraGizmo _cameraGizmo;
	HandleGroupId _camera_handle_group;
	bool _cam_handle_cached_present = false;
	vec3d _cam_handle_cached_pos = vmd_zero_vector;
	int _cam_handle_cached_op = -1;
	bool _cameraDragActive = false;
	vec3d _cameraDragPoint = vmd_zero_vector;

	// Flying the cutscene camera: where the controls have moved it. The selected sexp takes that
	// place once the controls have been still for a moment.
	bool _camFlying = false;
	vec3d _camFlyPos = vmd_zero_vector;
	matrix _camFlyOrient = vmd_identity_matrix;
	int _camFlyLastMove = 0; // timer_get_milliseconds()
	void commitCameraFly();

	// Flying an object with the camera controls (the current object, or the one being viewed
	// through) is one undo step per flight: where the objects were when it started, recorded once
	// the controls have been still for a moment
	struct ObjectFlyStart {
		int    signature;
		vec3d  pos;
		matrix orient;
	};
	bool _objFlying = false;
	SCP_vector<ObjectFlyStart> _objFlyStart;
	int _objFlyLastMove = 0; // timer_get_milliseconds()
	// flown: the object the controls move, with where it was before this frame's move; input:
	// whether a fly control was held this frame
	void noteObjectFly(const object* flown, const vec3d& oldPos, const matrix& oldOrient, bool input);
	void commitObjectFly();

	// Playback of the selected sexp's shot
	bool _camPlayback = false;
	bool _camPlaying = false;
	float _camTime = 0.0f;
	int _camPlayOpNode = -1;
	bool rawCameraPreview(CameraSexpPreview* out) const;

	// Viewport-owned volumetric gizmo state.
	HandleGroupId _volumetric_handle_group;
	// Cache so refreshVolumetricHandle() only touches the registry (and thus
	// requests a repaint) when the rendered state actually changes; otherwise
	// per-frame refresh would loop forever via needsUpdate().
	bool _vol_handle_cached_present = false;
	vec3d _vol_handle_cached_pos = vmd_zero_vector;
	SCP_string _vol_handle_cached_label;
	int _vol_handle_cached_color = -1;
	bool _vol_handle_cached_selected = false;

	// Viewport-owned asteroid gizmo state, plus a dirty cache (bounds + toggles
	// + selected) to avoid the per-frame repaint loop, and the outer-box center
	// handle index (spinbox default target).
	HandleGroupId _asteroid_handle_group;
	int _asteroid_center_index = -1;
	bool _ast_handle_cached_present = false;
	bool _ast_handle_cached_inner = false;
	bool _ast_handle_cached_selected = false;
	int _ast_handle_cached_target = -1;
	vec3d _ast_handle_cached_bounds[4] = {vmd_zero_vector, vmd_zero_vector, vmd_zero_vector, vmd_zero_vector};

	// The transform-toolbar's target handle across all groups. {-1,-1} = none.
	HandlePick _selected_handle{};

	// Compute the world-space point under the mouse cursor on the same
	// constraint plane that drag_objects() uses (centered on `anchor`).
	// Returns false if the intersection is behind the camera or invalid.
	bool screen_to_constraint_plane(int cx, int cy, const vec3d& anchor, vec3d* out_world) const;
};

} // namespace fso::fred
