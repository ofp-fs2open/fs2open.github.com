#pragma once

#include "mission/dialogs/MissionEventsDialogModel.h"
#include "mission/dialogs/EventReferenceIndex.h"
#include "ui/FredView.h"

#include <QDialog>
#include <QListWidget>
#include <QUndoStack>
#include <QPoint>

#include "ui/widgets/sexp_tree_view.h"

#include <mission/missiongoals.h>
#include <mission/missionmessage.h>

#include <memory>

class QButtonGroup;
class QCheckBox;

namespace fso::fred {
struct GraphEventMeta; // defined in ui/widgets/EventGraphView.h (used by-value in the .cpp)
}

namespace fso::fred::dialogs {

namespace Ui {
class MissionEventsDialog;
}

class MissionEventsDialog: public QDialog, public SexpTreeEditorInterface, public CameraEventSource {
	Q_OBJECT

  public:
	explicit MissionEventsDialog(FredView* parent, EditorViewport* viewport);
	~MissionEventsDialog() override;

	void accept() override;
	void reject() override;

	// Shows an event in the tree view and selects it (e.g. from the error checker's
	// "Go to Event"). index is a Mission_events index and name that event's name; see the
	// definition for how a list that has changed since is handled.
	void focusEvent(const SCP_string& name, int index);

	SCP_vector<SCP_string> getMessages() override;
	bool hasDefaultMessageParameter() override;
	SCP_vector<SCP_string> getMissionEvents(const SCP_string& reference_name) override;
	bool hasDefaultEvent(int operator_value) override;
	int getRootReturnType() const override;

	// Cutscene camera preview: the working events, and choosing an event's starts-after event
	SCP_vector<CameraEventInfo> cameraEventInfo() const override;
	void setCameraStartsAfter(int eventIndex, const SCP_string& name) override;

  protected:
	void closeEvent(QCloseEvent* event) override;

private slots:
	void on_okAndCancelButtons_accepted();
	void on_okAndCancelButtons_rejected();

	void on_btnNewEvent_clicked();
    void on_btnInsertEvent_clicked();
	void on_btnDeleteEvent_clicked();
	void on_eventMoveTopBtn_clicked();
	void on_eventUpBtn_clicked();
	void on_eventDownBtn_clicked();
	void on_eventMoveBottomBtn_clicked();

	void on_repeatCountBox_valueChanged(int value);
	void on_triggerCountBox_valueChanged(int value);
	void on_intervalTimeBox_valueChanged(int value);
	void on_chainedCheckBox_toggled(bool checked);
	void on_chainDelayBox_valueChanged(int value);
	void on_useMsecsCheckBox_toggled(bool checked);
	void on_btnEventFlags_clicked();
	void on_scoreBox_valueChanged(int value);
	void on_teamCombo_currentIndexChanged(int index);

	void on_editDirectiveText_textChanged(const QString& text);
	void on_editDirectiveKeypressText_textChanged(const QString& text);

	void on_checkLogTrue_toggled(bool checked);
	void on_checkLogFalse_toggled(bool checked);
	void on_checkLogPrevious_toggled(bool checked);
	void on_checkLogAlwaysFalse_toggled(bool checked);
	void on_checkLogFirstRepeat_toggled(bool checked);
	void on_checkLogLastRepeat_toggled(bool checked);
	void on_checkLogFirstTrigger_toggled(bool checked);
	void on_checkLogLastTrigger_toggled(bool checked);

	void on_eventSearchEdit_textChanged(const QString& text);
	void on_messageSearchEdit_textChanged(const QString& text);

	void on_btnValidateEvents_clicked();
	void on_btnFindPrev_clicked();
	void on_btnFindNext_clicked();

	void on_messageList_currentRowChanged(int row);
	void on_messageList_itemDoubleClicked(QListWidgetItem* item);

	void on_btnNewMsg_clicked();
	void on_btnInsertMsg_clicked();
	void on_btnDeleteMsg_clicked();
	void on_msgMoveTopBtn_clicked();
	void on_msgUpBtn_clicked();
	void on_msgDownBtn_clicked();
	void on_msgMoveBottomBtn_clicked();

	void on_messageName_textChanged(const QString& text);
	void on_messageContent_textChanged();
	void on_btnMsgNote_clicked();
	void on_aniCombo_editingFinished();
	void on_aniCombo_currentIndexChanged(int index);
	void on_btnAniBrowse_clicked();
	void on_waveCombo_editingFinished();
	void on_waveCombo_currentIndexChanged(int index);
	void on_btnBrowseWave_clicked();
	void on_btnWavePlay_clicked();
	void on_personaCombo_currentIndexChanged(int index);
	void on_btnUpdateStuff_clicked();
	void on_messageTeamCombo_currentIndexChanged(int index);


private: // NOLINT(readability-redundant-access-specifiers)
	// Indices into eventViewStack; also the QButtonGroup ids of the toggle buttons.
	enum EventViewIndex {
		TreeViewIndex = 0,
		GraphViewIndex = 1,
		AdvancedViewIndex = 2,
	};

	std::unique_ptr<Ui::MissionEventsDialog> ui;
	EditorViewport* _viewport;
	FredView*       _fredView    = nullptr;
	QUndoStack*     _dialogStack = nullptr;
	QButtonGroup*   _viewGroup   = nullptr;
	std::unique_ptr<MissionEventsDialogModel> _model;

	int m_last_message_node = -1;
	QString m_last_message_name;

	// Running before-state for tree-edit snapshots: the tree mutates before
	// modified() fires, so the cache holds the pre-edit capture.
	QByteArray _workingStateCache;
	bool _suppressTreeUndo = false;

	void onEventTreeModified();
	void pushEventStateSnapshot(const QByteArray& before, const QString& label);
	// Shows the newly created (current) event and gives its view keyboard focus
	void focusNewEvent();
	void insertEventBelow();
	void pushMessageStateSnapshot(const QByteArray& before, const QString& label, int mergeId = -1);
	void pushEventLogFlagCommand(int fieldConst, int mask, bool checked);
	void onMessageNameEditingFinished();
	void showMessageNameConflict(const SCP_string& reason);
	void changeMessageAni(const SCP_string& name);
	void changeMessageWave(const SCP_string& name);
	void syncEventRootLabel(int eventIndex);
	void updateEventBitmapAt(int eventIndex);

	void initViewToggle();
	void requestViewChange(int index);
	void setCurrentEventView(int index);
	bool canLeaveEventView(int index);
	void applyViewChrome(int index);

	// Graph view (relationship visualizer).
	void initGraphView();
	void rebuildReferenceIndex();
	void refreshGraphView();
	void refreshGraphIfCurrent(); // refreshGraphView() only when the graph is shown
	// Right-click a graph card: reuse the tree's context menu at that node. `key`
	// is the card's annotation key; treeItemForAnnotationKey resolves it to the
	// tree item (a tree_nodes[] node, or an event's labeled root) for the menu.
	void showGraphNodeMenu(int key, const QPoint& globalPos);
	void editGraphNode(int treeNode, const QPoint& globalPos); // double-click a literal-arg bullet
	void toggleEventCollapse(int eventIndex); // collapse/expand an event's subtree (Basic)
	QTreeWidgetItem* treeItemForAnnotationKey(int key) const;
	void selectEventInTree(int eventIndex); // select the event's root, no view switch
	void jumpToEventInTree(int eventIndex);  // switch to tree view, then select
	void jumpToNodeInTree(int treeNode);     // switch to tree view, then hilite the node
	GraphEventMeta buildEventMeta(int eventIndex) const; // status/annotation-key for one event
	void syncGraphAfterEventUi();            // reflect property edits on the graph card (or full rebuild)
	void refreshEventAfterFieldUndo(int eventIndex); // undo/redo of an event field, which may not be the selected event

	EventReferenceIndex _refIndex;
	bool _graphDirty = true; // graph needs a rebuild (events changed since last build)
	int  _graphEventCount = -1; // event count at last graph build, to detect add/remove

	// Advanced Edit view helpers.
	void loadAdvancedText();
	void showAdvancedResults(const SCP_vector<SCP_string>& errors, const SCP_vector<SCP_string>& warnings,
		const SCP_vector<int>& errorLines = {});
	// Extra selections on the advanced editor: the Validate error markers and the
	// bracket matching the one at the cursor.
	void updateAdvancedSelections();
	// Find-in-text for the advanced editor. incremental=true re-searches from
	// the start of the current match (used while typing) so the selection
	// refines in place; otherwise it steps to the next/previous match.
	void findInAdvancedText(bool forward, bool incremental);

	// Baseline text shown when the Advanced view was last (re)loaded; an
	// unchanged editor commits nothing.
	QString _advancedBaseline;
	// Lines (1-based) the last Validate/commit reported errors on; cleared on edit.
	SCP_vector<int> _advancedErrorLines;
	// Cached text and code mask (outside strings/comments) for bracket matching,
	// recomputed after an edit.
	QString _advancedText;
	QVector<bool> _advancedCodeMask;
	bool _advancedMaskDirty = true;

	void applyEventFilter();
	void applyMessageFilter();

	void updateEventUi();
	void updateEventMoveButtons();
	void updateEventCreateButtons(); // per-view/-mode New/Insert/Delete enablement
	bool eventControlsAvailable() const; // the event-controls column is live in this view and selection
	void setEventLogEnabled(bool enable);
	void updateMessageUi();
	void updateWaveInfo();
	void updateMessageMoveButtons();

	void initMessageList();
	void initHeadCombo();
	void initWaveFilenames();
	void initPersonas();

	void initMessageTeams();
	void initEventTeams();

	void rootNodeDeleted(int node);
	void rootNodeRenamed(int node);
	void rootNodeFormulaChanged(int old, int node);
	void rootNodeSelectedByFormula(int formula);

	void rebuildMessageList();
	void initMessageWidgets();
	void initEventWidgets();
	void updateEventBitmap();

	static SCP_vector<int> read_root_formula_order(sexp_tree_view* tree);
};

} // namespace fso::fred::dialogs

