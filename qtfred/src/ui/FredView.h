#pragma once

#include <functional>

#include <QMainWindow>
#include <QAction>
#include <QActionGroup>
#include <QElapsedTimer>
#include <QTimer>
#include <QUndoGroup>
#include <QUndoStack>
#include <math/vecmat.h>
#include <QtGui/QSurfaceFormat>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtGui/QSurface>
#include <QCloseEvent>

#include "missioneditor/missionsave.h"

#include <mission/FredRenderer.h>
#include <mission/IDialogProvider.h>

#include <memory>
#include <ui/widgets/ObjectComboBox.h>

class waypoint_list;
class QDateTime;

namespace fso {
namespace fred {

class Editor;
class RenderWidget;
struct ErrorTarget;

namespace dialogs {
class ErrorCheckerDialog;
class ShipEditorDialog;
class WingEditorDialog;
class PropEditorDialog;
}

class SceneBrowserPanel;

namespace Ui {
class FredView;
}

class FredView: public QMainWindow, public IDialogProvider {
 Q_OBJECT

 public:
	explicit FredView(QWidget* parent = nullptr);
	~FredView() override;
	void setEditor(Editor* editor, EditorViewport* viewport);

	void loadMissionFile(const QString& pathName, int flags = 0);

	QSurface* getRenderSurface();
	RenderWidget* getRenderWidget();

	void showContextMenu(const QPoint& globalPos);
	void showContextMenu(int objNum, const QPoint& globalPos);
	void showWingContextMenu(int wingIndex, const QPoint& globalPos);
	void showWaypointPathContextMenu(int pathIndex, const QPoint& globalPos);

	// Opens the Volumetric Nebula editor. Shared by the menu action,
	// the handle double-click, and the environment context menu.
	void editVolumetricNebula();

	// Opens the Asteroid Field editor. Shared by the menu action, the
	// handle double-click, and the environment context menu.
	void editAsteroidField();

	void restartAutosaveTimer();

 public slots:
	void openLoadMissionDialog();

	void newMission();

	// this can be triggered by the loadout dialog and so needs to be public
	void on_actionVariables_triggered(bool);

 private slots:
	 void on_actionSave_As_triggered(bool);
	 void on_actionSave_triggered(bool);
	void on_actionExit_triggered(bool);
	void on_actionRevert_triggered(bool);
	void on_actionLoad_Template_triggered(bool);
	void on_actionSave_As_Template_triggered(bool);
	void on_actionFS2_Open_triggered(bool);
	void on_actionFS2_Retail_triggered(bool);
	void on_actionFS2_Compatibility_triggered(bool);
	void on_actionFS1_Mission_triggered(bool);
	void on_actionRun_FreeSpace_2_Open_triggered(bool);

	void on_actionConstrainX_triggered(bool enabled);
	void on_actionConstrainXY_triggered(bool enabled);
	void on_actionConstrainY_triggered(bool enabled);
	void on_actionConstrainZ_triggered(bool enabled);
	void on_actionConstrainXZ_triggered(bool enabled);
	void on_actionConstrainYZ_triggered(bool enabled);

	void on_actionSelect_triggered(bool enabled);
	void on_actionSelectMove_triggered(bool enabled);
	void on_actionSelectRotate_triggered(bool enabled);

	void on_actionLock_Marked_Objects_triggered(bool enabled);
	void on_actionUnlock_All_Objects_triggered(bool enabled);

	void on_actionSelect_All_triggered(bool enabled);
	void on_actionSelect_None_triggered(bool enabled);
	void on_actionInvert_Selection_triggered(bool enabled);

	void on_actionx1_triggered(bool enabled);
	void on_actionx2_triggered(bool enabled);
	void on_actionx3_triggered(bool enabled);
	void on_actionx5_triggered(bool enabled);
	void on_actionx8_triggered(bool enabled);
	void on_actionx10_triggered(bool enabled);
	void on_actionx50_triggered(bool enabled);
	void on_actionx100_triggered(bool enabled);

	void on_actionRotx1_triggered(bool enabled);
	void on_actionRotx5_triggered(bool enabled);
	void on_actionRotx12_triggered(bool enabled);
	void on_actionRotx25_triggered(bool enabled);
	void on_actionRotx50_triggered(bool enabled);

	void on_actionCamera_triggered(bool enabled);
	void on_actionCurrent_Ship_triggered(bool enabled);
	void on_actionCutscene_Camera_triggered(bool enabled);
	void on_actionSet_Camera_From_View_triggered(bool);
	void on_actionToggle_Viewpoint_triggered(bool);

	void on_actionMission_Events_triggered(bool);
	void on_actionMission_Cutscenes_triggered(bool);
	void on_actionAsteroid_Field_triggered(bool);
	void on_actionVolumetric_Nebula_triggered(bool);
	void on_actionBriefing_triggered(bool);
	void on_actionMission_Specs_triggered(bool);
	void on_actionWaypoint_Paths_triggered(bool);
	void on_actionCoordinate_Points_triggered(bool);
	void on_actionJump_Nodes_triggered(bool);
	void on_actionObject_Orientation_triggered(bool);
	void on_actionShips_triggered(bool);
	void on_actionWings_triggered(bool);
	void on_actionProps_triggered(bool);
	void on_actionCampaign_triggered(bool);
	void on_actionCommand_Briefing_triggered(bool);
	void on_actionDebriefing_triggered(bool);
	void on_actionReinforcements_triggered(bool);
	void on_actionLoadout_triggered(bool);

	void on_actionSelectionLock_triggered(bool enabled);

	void on_actionWingForm_triggered(bool enabled);
	void on_actionWingDisband_triggered(bool enabled);

	void on_actionZoomSelected_triggered(bool);
	void on_actionZoomExtents_triggered(bool);

	void on_actionSceneBrowser_triggered(bool);

	void on_actionOrbitSelected_triggered(bool enabled);


	void on_actionSave_Camera_Pos_triggered(bool);
	void on_actionRestore_Camera_Pos_triggered(bool);

	void on_actionClone_Marked_Objects_triggered(bool);
	void on_actionDelete_triggered(bool);
	void on_actionDelete_Wing_triggered(bool);

	void on_actionControl_Object_triggered(bool);
	void on_actionLevel_Object_triggered(bool);
	void on_actionAlign_Object_triggered(bool);

	void on_actionNext_Subsystem_triggered(bool);
	void on_actionPrev_Subsystem_triggered(bool);
	void on_actionCancel_Subsystem_triggered(bool);

	void on_actionNext_Object_triggered(bool);
	void on_actionPrev_Object_triggered(bool);
	void on_actionMark_Wing_triggered(bool);

	void on_actionError_Checker_triggered(bool);

	void on_actionHelp_Topics_triggered(bool);
	void on_actionAbout_triggered(bool);
	void on_actionMission_Statistics_triggered(bool);
	void on_actionBackground_triggered(bool);
	void on_actionShield_System_triggered(bool);
	void on_actionSet_Global_Ship_Flags_triggered(bool);
	void on_actionVoice_Acting_Manager_triggered(bool);
	void on_actionFiction_Viewer_triggered(bool);
	void on_actionMission_Goals_triggered(bool);
	void on_actionMusic_Player_triggered(bool);
	void on_actionCalculate_Relative_Coordinates_triggered(bool);
	void on_actionWaypointPathGenerator_triggered(bool);
	void on_actionReorder_Objects_triggered(bool);
 signals:
	/**
	 * @brief Special version of FredApplication::onIdle which is limited to the lifetime of this object
	 */
	void viewIdle();

	/**
	 * @brief This is emitted when the view window is activated after being deactivated
	 */
	void viewWindowActivated();
 protected:
 bool event(QEvent* event) override;
	bool eventFilter(QObject* watched, QEvent* event) override;
	void changeEvent(QEvent* event) override;
 	void closeEvent(QCloseEvent* event) override;

	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;

	void mouseDoubleClickEvent(QMouseEvent* event) override;

 private:
	bool saveMissionToCurrentPath();
	bool saveMissionAs();
	void openAndRunErrorChecker();
	// Error checker "Go to ...": select the target and open it in its editor.
	void showErrorTarget(const ErrorTarget& target);
	void autoRunErrorChecker();
	// Runs the error checker before a save (no mutations, no potential issues).
	// If errors are found, shows the error checker in PreSave mode and applies
	// auto-corrections if the designer chooses "Fix and Save".
	// Returns false if the save should be cancelled.
	// If outFixCount is provided it is set to the number of issues auto-corrected
	// (0 if Fix and Save was chosen but nothing could be fixed, -1 if not attempted).
	bool performPreSaveCheck(int* outFixCount = nullptr);
	void saveAsTemplate();
	void loadTemplate();
	bool maybePromptToSaveMissionChanges(const QString& actionDescription);
	bool isMissionModified() const;

	void on_mission_loaded(const std::string& filepath);

	void connectActionToViewSetting(QAction* option, bool* destination);
	void connectActionToViewSetting(QAction* option, std::vector<bool>* vector, size_t idx);

	void on_actionControlModeCamera_triggered(bool enabled);
	void on_actionControlModeCurrentShip_triggered(bool enabled);

	void addToRecentFiles(const QString& path);
	void updateRecentFileList();

	// Makes filepath the file this window is editing: title, platform file path, Save target,
	// autosave name and recent files. Empty means an Untitled mission.
	void setCurrentFile(const QString& filepath);

	void recentFileOpened();

	/**
	 * @brief Synchronize the view options in the renderer and the state of the view check boxes in the menu
	 */
	void syncViewOptions();
	void updateUI();

	void initializeStatusBar();
	void updateUndoStatusIndicator();
	void initializePopupMenus();
	void populateMoveToLayerMenu(int targetObject, QMenu* targetMenu = nullptr);
	// Fills a context menu's Set Group submenu for the marked objects: a group they're all in is
	// checked, and clicking a group adds them all to it, or takes them all out if it was checked
	void populateSetGroupMenu(QMenu* dest);
	void populateCreateShipSubmenu();
	void populateCreatePropSubmenu();
	void openLayerManagerDialog();
	void ensureViewportFocus();
	void enforceSideDockAreas();

	void onGroupSelected(int group);
	// Select > Select Layer / IFF / Ship Type / Object Type: each fills its submenu when it opens,
	// one item per set with how many objects it would select
	void populateSelectByMenus();
	// Replaces the selection with the objects a click could select (see
	// EditorViewport::isObjectSelectable) that match
	void selectMatching(const std::function<bool(const object&)>& matches);
	int countSelectable(const std::function<bool(const object&)>& matches) const;

	QLabel* _statusBarObjectCount = nullptr;
	QLabel* _statusBarLastSaved   = nullptr;
	QLabel* _statusBarViewmode    = nullptr;
	QLabel* _statusBarUnitsLabel  = nullptr;
	QLabel* _statusBarUndoScope   = nullptr;

	// Updates the "Last Saved" status bar label: pass an empty time to show "Never".
	void setLastSaved(const QDateTime& when);

	// Sweeps a brief white gleam across the status bar to celebrate a save.
	void triggerSaveShine();

	SceneBrowserPanel* _browserPanel = nullptr;

	QMenu* _viewPopup = nullptr;
	QMenu* _createSubmenu = nullptr;
	QMenu* _createShipSubmenu = nullptr;
	QMenu* _createPropSubmenu = nullptr;
	QPoint _lastContextMenuLocalPos;

	QMenu* _editPopup = nullptr;
	QAction* _editObjectAction = nullptr;
	QAction* _editOrientPositionAction = nullptr;
	QAction* _editWingAction = nullptr;
	QAction* _selectWingAction = nullptr;
	QMenu* _moveToLayerMenu = nullptr;
	QMenu* _setGroupMenu = nullptr;
	QAction* _viewZoomSelectedAction = nullptr;

	QMenu* _controlModeMenu = nullptr;
	QAction* _controlModeCamera = nullptr;
	QAction* _controlModeCurrentShip = nullptr;

	QString saveName = nullptr;
	MissionFormat _missionSaveFormat = MissionFormat::STANDARD;

	std::unique_ptr<Ui::FredView> ui;

	ObjectComboBox* _shipClassBox = nullptr;
	ObjectComboBox* _propClassBox = nullptr;
	ObjectComboBox* _otherClassBox = nullptr;

	QUndoGroup* _undoGroup    = nullptr;
	QUndoStack* _mainStack    = nullptr;
	QUndoStack* _cameraStack  = nullptr;
	QAction*    _undoAction        = nullptr;
	QAction*    _redoAction        = nullptr;
	QAction*    _undoCameraAction  = nullptr;
	QAction*    _redoCameraAction  = nullptr;

	QTimer* _cameraIdleTimer             = nullptr;
	vec3d   _cameraPosBeforeGesture      = {};
	matrix  _cameraOrientBeforeGesture   = {};

	Editor* fred = nullptr;
	EditorViewport* _viewport = nullptr;

	fso::fred::dialogs::ErrorCheckerDialog* _errorCheckerDialog = nullptr;
	fso::fred::dialogs::ShipEditorDialog* _shipEditorDialog = nullptr;
	fso::fred::dialogs::WingEditorDialog* _wingEditorDialog = nullptr;
	fso::fred::dialogs::PropEditorDialog* _propEditorDialog = nullptr;

	bool _inKeyPressHandler = false;
	bool _inKeyReleaseHandler = false;
	bool _missionModified = false;
	// The mission was loaded from an autosave; the next save to its own file asks first
	bool _recoveredFromAutosave = false;

	void onUpdateConstrains();
	// Axis constraint by index: 0 X, 1 Y, 2 Z, 3 XZ, 4 XY, 5 YZ. Applies it to the viewport and,
	// in Move or Rotate mode, remembers it for that mode.
	void setConstraint(int index);
	int _constraintMove   = 3; // XZ
	int _constraintRotate = 3;
	void onUpdateEditingMode();
	void onUpdateViewSpeeds();
	void onUpdateCameraControlActions();
	void onUpdateSelectionLock();
	void onUpdateShipClassBox();
	void onUpdatePropClassBox();
	void onUpdateOtherClassBox();
	void onUpdateEditorActions();
	void onUpdateWingActionStatus();

	void initializeContextToolbar();
	void onUpdateContextToolbar();
	void quickRenameCurrentObject();

	void initializeTransformBar();
	void onUpdateTransformBar();
	// axis: 0, 1 or 2 for the X/Y/Z (or heading/pitch/bank) box that was edited
	void onTransformEditingFinished(int axis);
	// Applies a pivot mode to the viewport, the toolbar button, and the current mode's memory
	void setPivotMode(PivotMode mode);

	QToolBar* _contextToolBar = nullptr;
	QLabel*   _contextLabel   = nullptr;

	// Cached selection state... buttons only rebuild when these change
	int            _ctxCachedObj                = -2;     // -2 = uninitialized
	int            _ctxCachedMarked             = -1;
	int            _ctxCachedObjType            = -1;     // single: actual type; multi: common type (-1=mixed)
	bool           _ctxCachedInWing             = false;
	int            _ctxCachedSharedWing         = -2;     // multi-select: shared wing index (-1=none, -2=N/A)
	waypoint_list* _ctxCachedSharedWaypointList = nullptr;
	int            _ctxCachedEnv                = -1;      // EnvironmentObject as int; -1 = uninitialized

	QToolBar*       _transformToolBar    = nullptr;
	QLabel*         _transformLabel      = nullptr;
	QLabel*         _transformLabelA     = nullptr;
	QLabel*         _transformLabelB     = nullptr;
	QLabel*         _transformLabelC     = nullptr;
	QDoubleSpinBox* _transformA          = nullptr;
	QDoubleSpinBox* _transformB          = nullptr;
	QDoubleSpinBox* _transformC          = nullptr;
	QComboBox*      _transformMoveSpeedCombo = nullptr;
	QComboBox*      _transformRotSpeedCombo  = nullptr;
	// FOV of the current view in degrees; editable while viewing through an object, or through
	// a cutscene camera with a set-camera-fov selected (it writes that sexp)
	QDoubleSpinBox* _transformFovSpin        = nullptr;
	// Cutscene camera playback, shown while looking through a cutscene camera
	QList<QAction*> _cameraPlaybackActions;
	QComboBox*      _cameraStartsAfterCombo  = nullptr;
	QToolButton*    _cameraRewindBtn         = nullptr;
	QToolButton*    _cameraPlayBtn           = nullptr;
	QToolButton*    _cameraEndBtn            = nullptr;
	QLabel*         _cameraTimeLabel         = nullptr;
	QTimer*         _cameraPlaybackTimer     = nullptr;
	QElapsedTimer   _cameraPlaybackClock;
	QString         _cameraStartsAfterKey; // what the combo was last filled from
	void updateCameraPlaybackControls();
	// Last camera speeds written to QSettings; lets us persist on change instead of only on close.
	int             _lastSavedCameraSpeedMove = -1;
	int             _lastSavedCameraSpeedRot  = -1;
	QComboBox*      _transformIffCombo   = nullptr;
	QLabel*         _transformRadiusLabel = nullptr;
	QToolButton*    _transformPivotBtn   = nullptr;
	QAction*        _pivotActions[3]     = {};      // Group, Individual, Align, in PivotMode order
	QComboBox*      _transformLayerCombo = nullptr;
	bool            _tbLayerComboDirty   = true;  // rebuild layer combo only when layer structure changes
	bool            _tbIffPopulated      = false; // IFF items are populated lazily (tables load after init)
	PivotMode       _tbPivotMove         = PivotMode::Group; // remembered pivot mode while in move mode
	PivotMode       _tbPivotRotate       = PivotMode::Group; // remembered pivot mode while in rotate mode
	int             _tbCachedCursorMode  = -1;    // -1 forces per-mode restore on first update

	void onShipClassSelected(int ship_class);
	void onPropClassSelected(int prop_class);
	void onOtherKindSelected(int other_kind);

	void windowActivated();
	void windowDeactivated();

	void editObjectTriggered();
	void orientEditorTriggered();

 public:
	// Opens the editor for an object (the Ship Editor for several marked objects), as
	// double-clicking it in the viewport does; the Scene Browser's double-click uses it too
	void handleObjectEditor(int objNum);
	// Opens the group checklist for the objects an editor is showing and applies the result as one
	// undo step. A group only some of them are in starts partly checked and is left alone unless changed.
	void editSelectionGroups(const SCP_vector<int>& objnums, QWidget* parent);

	DialogButton showButtonDialog(DialogType type,
								  const SCP_string& title,
								  const SCP_string& message,
								  const flagset<DialogButton>& buttons) override;

	QUndoGroup* undoGroup()       const { return _undoGroup; }
	QUndoStack* mainUndoStack()   const { return _mainStack; }
	QUndoStack* cameraUndoStack() const { return _cameraStack; }

	std::unique_ptr<IDialog<dialogs::FormWingDialogModel>> createFormWingDialog() override;

	bool showModalDialog(IBaseDialog* dlg) override;
	void initializeGroupActions();
};

} // namespace fred
} // namespace fso
