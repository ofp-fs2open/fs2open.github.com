#include "MissionEventsDialog.h"
#include "ui_MissionEventsDialog.h"

#include <QLocale>
#include <QShortcut>
#include <QStringList>
#include <cmath>
#include <functional>
#include "ui/Theme.h"
#include "ui/widgets/EventGraphView.h"
#include "ui/widgets/MissionTextHighlighter.h"
#include "ui/util/default_dir.h"
#include "ui/util/SignalBlockers.h"
#include "ui/util/KeyboardNavigation.h"
#include "ui/util/NoteDialog.h"
#include "ui/dialogs/EventEditor/HeadAnimationPickerDialog.h"
#include "mission/commands/FredCommands.h"

#include "mission/util.h"

#include <sound/audiostr.h>
#include <localization/localize.h>

#include <ui/util/DialogUndo.h>

#include <QButtonGroup>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextBlock>
#include <QTextCursor>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QKeyEvent>
#include <QVector>
#include <mission/missionmessage.h>
#include <globalincs/linklist.h>
#include <object/object.h>
#include <object/waypoint.h>
#include <ship/ship.h>
#include <prop/prop.h>
#include <jumpnode/jumpnode.h>
#include <coordinate_points/coordinate_point.h>
#include <missioneditor/missionsave.h>
#include <mod_table/mod_table.h>

namespace fso::fred::dialogs {

// Mission text is Latin-1 unless Unicode text mode is on. The Advanced view
// rewrites every event when it commits, so decoding its text the wrong way
// would corrupt every non-ASCII character in the section at once.
static QString missionTextToQString(const SCP_string& text)
{
	return Unicode_text_mode ? QString::fromStdString(text)
	                         : QString::fromLatin1(text.c_str(), static_cast<qsizetype>(text.size()));
}

static SCP_string qStringToMissionText(const QString& text)
{
	return Unicode_text_mode ? text.toStdString() : text.toLatin1().toStdString();
}

// Enumerate every first-class object type for the graph's object selector:
// ships (incl. player starts), wings, props, waypoint paths, jump nodes,
// coordinate points. The selector then hides any with zero references.
static QVector<GraphObject> buildGraphObjects()
{
	QVector<GraphObject> objs;
	auto add = [&objs](RefObjectKind kind, const QString& name) {
		GraphObject o;
		o.kind = kind;
		o.name = name;
		objs.push_back(o);
	};

	for (object* objp = GET_FIRST(&obj_used_list); objp != END_OF_LIST(&obj_used_list);
	     objp = GET_NEXT(objp)) {
		if (objp->instance < 0)
			continue;
		// OBJ_START = player-start markers (Alpha 1, etc.); they are ships too,
		// indexing Ships[] just like OBJ_SHIP. Mirrors get_listing_opf_ship.
		if (objp->type == OBJ_SHIP || objp->type == OBJ_START)
			add(RefObjectKind::Ship, QString::fromUtf8(Ships[objp->instance].ship_name));
		else if (objp->type == OBJ_PROP && Props[objp->instance].has_value())
			add(RefObjectKind::Prop, QString::fromUtf8(Props[objp->instance]->prop_name));
	}
	for (int i = 0; i < Num_wings; ++i) {
		if (Wings[i].wave_count > 0)
			add(RefObjectKind::Wing, QString::fromUtf8(Wings[i].name));
	}
	for (const auto& wl : Waypoint_lists)
		add(RefObjectKind::Waypoint, QString::fromUtf8(wl.get_name()));
	for (const auto& jn : Jump_nodes)
		add(RefObjectKind::JumpNode, QString::fromUtf8(jn.GetName()));
	for (const auto& cp : Coordinate_points)
		add(RefObjectKind::CoordinatePoint, QString::fromStdString(cp.name));

	return objs;
}

// The events-side view (tree/graph/advanced) chosen last time the dialog was
// open; persists for the qtFRED session only.
static int s_lastEventViewIndex = 0; // MissionEventsDialog::TreeViewIndex

// Compute the annotation key for a Qt tree item. For regular tree nodes this is
// the tree_nodes[] index; for root labels (event name rows, which aren't stored
// in tree_nodes) it's the encoded root key from SexpAnnotationModel. Returns -1
// if the item maps to neither (e.g. a stray item with no FormulaDataRole).
static int annotation_key_for_qt_item(sexp_tree_view* tree, QTreeWidgetItem* h)
{
	if (!h)
		return -1;

	int node = tree->get_node(h);
	if (node >= 0)
		return node;

	if (!h->parent()) {
		const QVariant v = h->data(0, sexp_tree_view::FormulaDataRole);
		bool ok = false;
		const int formula = v.toInt(&ok);
		if (ok && formula >= 0)
			return SexpAnnotationModel::rootKey(formula);
	}

	return -1;
}

MissionEventsDialog::MissionEventsDialog(FredView* parent, EditorViewport* viewport) :
	QDialog(parent),
	SexpTreeEditorInterface({ TreeFlags::LabeledRoot, TreeFlags::RootDeletable, TreeFlags::RootEditable, TreeFlags::AnnotationsAllowed }),
	  ui(new Ui::MissionEventsDialog()), _viewport(viewport), _fredView(parent)
{
	_dialogStack = new QUndoStack(this);
	_fredView->undoGroup()->addStack(_dialogStack);

	ui->setupUi(this);
	// The Advanced view edits mission-file text, where Tab indents
	util::allowTabInput(ui->advancedTextEdit);

	util::setupDialogUndo(this, _fredView->undoGroup(), _dialogStack, tr("Mission Events"));

	// Give the tree panel preference on resize but let the user rebalance via the splitter.
	// Both panels get stretch>0 so QSplitter distributes extra space proportionally to
	// their current sizes; the higher factor on the tree side biases the initial layout
	// and any extra room from a larger dialog toward the tree.
	ui->mainSplitter->setStretchFactor(0, 3);
	ui->mainSplitter->setStretchFactor(1, 1);
	ui->mainSplitter->setSizes({600, 350});

	fso::fred::bindCustomIcon(ui->eventMoveTopBtn,    CustomIcon::MoveToTop);
	fso::fred::bindStandardIcon(ui->eventUpBtn,       QStyle::SP_ArrowUp);
	fso::fred::bindStandardIcon(ui->eventDownBtn,     QStyle::SP_ArrowDown);
	fso::fred::bindCustomIcon(ui->eventMoveBottomBtn, CustomIcon::MoveToBottom);
	fso::fred::bindCustomIcon(ui->msgMoveTopBtn,      CustomIcon::MoveToTop);
	fso::fred::bindStandardIcon(ui->msgUpBtn,         QStyle::SP_ArrowUp);
	fso::fred::bindStandardIcon(ui->msgDownBtn,       QStyle::SP_ArrowDown);
	fso::fred::bindCustomIcon(ui->msgMoveBottomBtn,   CustomIcon::MoveToBottom);
	fso::fred::bindStandardIcon(ui->btnWavePlay,      QStyle::SP_MediaPlay);
	ui->waveInfoLabel->setForegroundRole(QPalette::PlaceholderText); // secondary gray that follows the theme
	// keep the line's height when it's empty, so the form doesn't jump as messages change
	ui->waveInfoLabel->setMinimumHeight(ui->waveInfoLabel->fontMetrics().height());

	ui->editDirectiveText->setMaxLength(NAME_LENGTH - 1);
	ui->editDirectiveKeypressText->setMaxLength(NAME_LENGTH - 1);

	ui->eventTree->initializeEditor(viewport->editor, this, viewport, parent);
	ui->eventTree->setCameraEventSource(this);
	ui->eventTree->clear_tree();
	ui->eventTree->_model.post_load();

	// Construct the model with a direct reference to the shared sexp tree model
	_model = std::make_unique<MissionEventsDialogModel>(this, _viewport, ui->eventTree->_model);

	// Connect model signals to widget operations
	connect(_model.get(), &MissionEventsDialogModel::treeCleared, this, [this]() {
		ui->eventTree->clear();
	});

	connect(_model.get(), &MissionEventsDialogModel::subtreeAdded, this,
		[this](const SCP_string& name, NodeImage image, int formula) {
			auto h = ui->eventTree->insert(name.c_str(), image);
			h->setData(0, sexp_tree_view::FormulaDataRole, formula);
			ui->eventTree->add_sub_tree(formula, h);
		});

	connect(_model.get(), &MissionEventsDialogModel::defaultRootBuilt, this,
		[this](const SCP_string& name, int after_root_formula, int new_formula) {
			// Find the item to insert after (if any)
			QTreeWidgetItem* afterItem = nullptr;
			if (after_root_formula >= 0) {
				const int n = ui->eventTree->topLevelItemCount();
				for (int i = 0; i < n; ++i) {
					auto* it = ui->eventTree->topLevelItem(i);
					if (it && it->data(0, sexp_tree_view::FormulaDataRole).toInt() == after_root_formula) {
						afterItem = it;
						break;
					}
				}
			}

			// Insert the root item
			auto* root = ui->eventTree->insert(name.c_str(), NodeImage::ROOT, nullptr, afterItem);
			root->setData(0, sexp_tree_view::FormulaDataRole, new_formula);

			// Build the visual subtree from the model's tree_nodes
			ui->eventTree->add_sub_tree(new_formula, root);

			ui->eventTree->clearSelection();
			root->setSelected(true);
		});

	connect(_model.get(), &MissionEventsDialogModel::rootSelected, this, [this](int formula) {
		const int n = ui->eventTree->topLevelItemCount();
		for (int i = 0; i < n; ++i) {
			auto* it = ui->eventTree->topLevelItem(i);
			if (it && it->data(0, sexp_tree_view::FormulaDataRole).toInt() == formula) {
				ui->eventTree->setCurrentItem(it);
				break;
			}
		}
	});

	connect(_model.get(), &MissionEventsDialogModel::eventDeleteRequested, this, [this]() {
		// Walk to root before deleting
		auto item = ui->eventTree->currentItem();
		while (item && item->parent() != nullptr) {
			item = item->parent();
		}
		if (item) {
			ui->eventTree->setCurrentItem(item);
			ui->eventTree->deleteCurrentItem();
		}
	});

	connect(_model.get(), &MissionEventsDialogModel::topLevelIndexRequested, this,
		[this](int formula, int desired_index) {
			const int n = ui->eventTree->topLevelItemCount();
			for (int i = 0; i < n; ++i) {
				auto* it = ui->eventTree->topLevelItem(i);
				if (it && it->data(0, sexp_tree_view::FormulaDataRole).toInt() == formula) {
					int cur = ui->eventTree->indexOfTopLevelItem(it);
					if (cur != desired_index) {
						ui->eventTree->takeTopLevelItem(cur);
						ui->eventTree->insertTopLevelItem(desired_index, it);
					}
					break;
				}
			}
		});

	connect(_model.get(), &MissionEventsDialogModel::annotationApplied, this,
		[this](int key, const SCP_string& note, int r, int g, int b, bool has_color) {
			// Resolve the annotation key back to a Qt item: regular keys (>= 0) index
			// directly into tree_nodes[]; root keys (<= -2) are decoded to a formula
			// and matched against the top-level items via FormulaDataRole.
			QTreeWidgetItem* it = nullptr;
			if (SexpAnnotationModel::isRootKey(key)) {
				const int formula = SexpAnnotationModel::formulaFromRootKey(key);
				const int n = ui->eventTree->topLevelItemCount();
				for (int i = 0; i < n; ++i) {
					auto* candidate = ui->eventTree->topLevelItem(i);
					if (candidate && candidate->data(0, sexp_tree_view::FormulaDataRole).toInt() == formula) {
						it = candidate;
						break;
					}
				}
			} else if (key >= 0 && key < static_cast<int>(ui->eventTree->_model.tree_nodes.size())) {
				it = tree_item_handle(ui->eventTree->_model.tree_nodes[key]);
			}
			if (!it)
				return;

			const QString q = QString::fromStdString(note);
			it->setData(0, sexp_tree_view::NoteRole, q);
			it->setToolTip(0, q);
			// white means "no color" in the annotation data, so store no color rather than paint white
			it->setData(0, sexp_tree_view::BgColorRole, has_color ? QColor(r, g, b) : QColor());
			sexp_tree_view::applyVisuals(it);
		});

	// Load data now that all signals are connected, so tree-building signals
	// (treeCleared, subtreeAdded, annotationApplied) are received by the dialog.
	_model->initializeData();

	initMessageWidgets();

	initEventWidgets();

	initGraphView();

	initViewToggle();

	// The before-state for the next tree edit: the tree mutates before
	// modified() fires, so a fresh capture at handler time would already
	// contain the edit. indexChanged fires on every push/undo/redo, keeping
	// the cache aligned with the current working state.
	_workingStateCache = _model->captureEventWorkingState();
	connect(_dialogStack, &QUndoStack::indexChanged, this, [this](int) {
		// accept() moves _model into the ApplyDialogCommand and then clears
		// the stack, which emits indexChanged with no model left.
		if (_model)
			_workingStateCache = _model->captureEventWorkingState();
		// Any edit/undo/redo invalidates the relationship graph; it will
		// rebuild next time the graph view is shown (or now if it's current).
		_graphDirty = true;
	});
}

MissionEventsDialog::~MissionEventsDialog()
{
	// The tree outlives the model while the dialog's widgets are torn down
	ui->eventTree->setCameraEventSource(nullptr);
}

SCP_vector<CameraEventInfo> MissionEventsDialog::cameraEventInfo() const
{
	SCP_vector<CameraEventInfo> events;
	if (!_model)
		return events;
	const auto& list = _model->getEventList();
	events.reserve(list.size());
	for (int i = 0; i < static_cast<int>(list.size()); ++i) {
		CameraEventInfo info;
		info.name = list[i].name;
		info.formula = list[i].formula;
		info.chained = list[i].chain_delay >= 0;
		info.startsAfter = _model->getCameraStartsAfter(i);
		events.push_back(std::move(info));
	}
	return events;
}

void MissionEventsDialog::setCameraStartsAfter(int eventIndex, const SCP_string& name)
{
	if (!_model || _model->getCameraStartsAfter(eventIndex) == name)
		return;
	const QByteArray before = _model->captureEventWorkingState();
	_model->setCameraStartsAfterAt(eventIndex, name);
	pushEventStateSnapshot(before, tr("Set Camera Starts After"));
}

void MissionEventsDialog::initEventWidgets() {
	initEventTeams();

	ui->miniHelpBox->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
	ui->helpBox->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

	ui->miniHelpBox->setVisible(_viewport->Show_sexp_help_mission_events);
	ui->helpBox->setVisible(_viewport->Show_sexp_help_mission_events);

	// Shift+F1 toggles the sexp help panes for this session without changing the saved preference.
	auto* helpToggle = new QShortcut(QKeySequence(QStringLiteral("Shift+F1")), this);
	connect(helpToggle, &QShortcut::activated, this, [this] {
		const bool show = !ui->helpBox->isVisible();
		ui->miniHelpBox->setVisible(show);
		ui->helpBox->setVisible(show);
	});

	// connect the sexp tree stuff
	connect(ui->eventTree, &sexp_tree_view::modified, this, &MissionEventsDialog::onEventTreeModified);
	connect(ui->eventTree, &sexp_tree_view::rootNodeDeleted, this, &MissionEventsDialog::rootNodeDeleted);
	connect(ui->eventTree, &sexp_tree_view::rootNodeRenamed, this, &MissionEventsDialog::rootNodeRenamed);
	connect(ui->eventTree, &sexp_tree_view::rootNodeFormulaChanged, this, &MissionEventsDialog::rootNodeFormulaChanged);
	connect(ui->eventTree, &sexp_tree_view::miniHelpChanged, this, [this](const QString& help) { ui->miniHelpBox->setText(help); });
	connect(ui->eventTree, &sexp_tree_view::helpChanged, this, [this](const QString& help) { ui->helpBox->setPlainText(help); });
	connect(ui->eventTree, &sexp_tree_view::selectedRootChanged, this, [this](int formula) { MissionEventsDialog::rootNodeSelectedByFormula(formula); });

	connect(ui->eventTree, &sexp_tree_view::nodeAnnotationChanged, this, [this](void* h, const QString& note) {
		// Translate QTreeWidgetItem* to annotation key (tree_nodes[] index for regular
		// nodes, or rootKey(formula) for root labels).
		const int key = annotation_key_for_qt_item(ui->eventTree, static_cast<QTreeWidgetItem*>(h));
		if (key != -1) {
			SCP_string text = note.toUtf8().constData();
			const QByteArray before = _model->captureEventWorkingState();
			_model->setNodeAnnotation(key, text);
			pushEventStateSnapshot(before, tr("Edit Node Note"));
			refreshGraphIfCurrent();
		}
	});

	connect(ui->eventTree, &sexp_tree_view::nodeBgColorChanged, this, [this](void* h, const QColor& c) {
		const int key = annotation_key_for_qt_item(ui->eventTree, static_cast<QTreeWidgetItem*>(h));
		if (key != -1) {
			const QByteArray before = _model->captureEventWorkingState();
			_model->setNodeBgColor(key, c.red(), c.green(), c.blue(), c.isValid());
			pushEventStateSnapshot(before, tr("Change Node Color"));
			refreshGraphIfCurrent();
		}
	});

	connect(ui->eventTree, &sexp_tree_view::rootOrderChanged, this, [this] {
		SCP_vector<int> order;
		order.reserve(ui->eventTree->topLevelItemCount());
		for (int i = 0; i < ui->eventTree->topLevelItemCount(); ++i) {
			auto* it = ui->eventTree->topLevelItem(i);
			order.push_back(it->data(0, sexp_tree_view::FormulaDataRole).toInt());
		}
		// The widget has only moved items visually at this point (the
		// modified() it emitted first was a no-op capture), so a fresh
		// capture still holds the pre-reorder event order.
		const QByteArray before = _model->captureEventWorkingState();
		_model->reorderByRootFormulaOrder(order);
		m_last_message_node = -1;
		pushEventStateSnapshot(before, tr("Move Event"));
	});

	_model->setCurrentlySelectedEvent(-1);

	updateEventUi();
}

void MissionEventsDialog::initViewToggle()
{
	_viewGroup = new QButtonGroup(this);
	_viewGroup->setExclusive(true);
	_viewGroup->addButton(ui->btnViewTree, TreeViewIndex);
	_viewGroup->addButton(ui->btnViewGraph, GraphViewIndex);
	_viewGroup->addButton(ui->btnViewAdvanced, AdvancedViewIndex);

	connect(_viewGroup, &QButtonGroup::idClicked, this, &MissionEventsDialog::requestViewChange);

	// Monospace, matching the sexp help boxes, so the mission-file text lines up.
	const QFont mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	ui->advancedTextEdit->setFont(mono);
	ui->advancedErrorList->setFont(mono);

	// Syntax highlighting (owned by the document). Error markers from the last
	// Validate stay until the text changes; bracket matching follows the cursor.
	new fso::fred::MissionTextHighlighter(ui->advancedTextEdit);
	connect(ui->advancedTextEdit, &QPlainTextEdit::textChanged, this, [this] {
		_advancedErrorLines.clear();
		_advancedMaskDirty = true;
		updateAdvancedSelections();
	});
	connect(ui->advancedTextEdit, &QPlainTextEdit::cursorPositionChanged, this,
		&MissionEventsDialog::updateAdvancedSelections);

	// Find controls: the search box is dual-purpose (filter in tree view,
	// find-in-text in advanced view); the arrows step through matches.
	fso::fred::bindStandardIcon(ui->btnFindPrev, QStyle::SP_ArrowUp);
	fso::fred::bindStandardIcon(ui->btnFindNext, QStyle::SP_ArrowDown);

	connect(ui->eventSearchEdit, &QLineEdit::returnPressed, this, [this] {
		if (ui->eventViewStack->currentIndex() == AdvancedViewIndex)
			findInAdvancedText(/*forward=*/true, /*incremental=*/false);
		else if (ui->eventViewStack->currentIndex() == GraphViewIndex)
			ui->eventGraph->focusNextMatch(/*forward=*/true);
	});

	// Ctrl+F focuses the search box for whichever view is active.
	auto* findShortcut = new QShortcut(QKeySequence::Find, this);
	connect(findShortcut, &QShortcut::activated, this, [this] {
		if (ui->eventSearchEdit->isEnabled()) {
			ui->eventSearchEdit->setFocus();
			ui->eventSearchEdit->selectAll();
		}
	});

	// Excel-style row shortcuts on the event tree: Ctrl+Plus inserts an event above the
	// selected one, Ctrl+Alt+Plus below it (Delete already deletes). Ctrl+= and the keypad +
	// work too, so no Shift is needed. WidgetShortcut: only while the tree itself has focus,
	// never while an inline edit (a child editor) or another field is being typed in.
	auto bindEventShortcuts = [this](std::initializer_list<QKeySequence> keys, QPushButton* gate, const std::function<void()>& action) {
		for (const auto& key : keys) {
			auto* sc = new QShortcut(key, ui->eventTree);
			sc->setContext(Qt::WidgetShortcut);
			connect(sc, &QShortcut::activated, this, [gate, action] {
				if (gate->isEnabled())
					action();
			});
		}
	};
	const auto ctrl = Qt::KeyboardModifiers(Qt::ControlModifier);
	const auto ctrlAlt = Qt::ControlModifier | Qt::AltModifier;
	const auto pad = Qt::KeyboardModifiers(Qt::KeypadModifier);
	auto seq = [](Qt::KeyboardModifiers mods, Qt::Key key) { return QKeySequence(QKeyCombination(mods, key)); };
	bindEventShortcuts({seq(ctrl, Qt::Key_Plus), seq(ctrl, Qt::Key_Equal), seq(ctrl | pad, Qt::Key_Plus)},
		ui->btnInsertEvent, [this] { on_btnInsertEvent_clicked(); });
	bindEventShortcuts({seq(ctrlAlt, Qt::Key_Plus), seq(ctrlAlt, Qt::Key_Equal), seq(ctrlAlt | pad, Qt::Key_Plus)},
		ui->btnInsertEvent, [this] { insertEventBelow(); });
	ui->btnInsertEvent->setToolTip(tr("Insert an event above the selected one (Ctrl++; Ctrl+Alt++ inserts below)"));

	// Restore the view used last time this session. A view whose button is
	// disabled can't be restored into.
	int view = s_lastEventViewIndex;
	auto* btn = _viewGroup->button(view);
	if (!btn || !btn->isEnabled())
		view = TreeViewIndex;
	setCurrentEventView(view);
}

void MissionEventsDialog::requestViewChange(int index)
{
	const int current = ui->eventViewStack->currentIndex();
	if (index == current)
		return;

	if (!canLeaveEventView(current)) {
		// Veto: the clicked button already took the checked state, give it
		// back to the current view's button.
		if (auto* btn = _viewGroup->button(current)) {
			QSignalBlocker blocker(_viewGroup);
			btn->setChecked(true);
		}
		return;
	}

	setCurrentEventView(index);
}

// Gate for leaving a view. The advanced edit view refuses to yield (and to
// accept the dialog) until its hand-edited text parses and validates cleanly;
// a clean parse is committed to the working events as a single undo step.
bool MissionEventsDialog::canLeaveEventView(int index)
{
	if (index != AdvancedViewIndex)
		return true;

	const QString text = ui->advancedTextEdit->toPlainText();

	// Unchanged text: nothing to commit, always allowed to leave.
	if (text == _advancedBaseline)
		return true;

	SCP_vector<SCP_string> errors, warnings;
	const QByteArray before = _model->captureEventWorkingState();

	_suppressTreeUndo = true;
	SCP_vector<int> errorLines;
	const bool ok =
		_model->applyEventsText(qStringToMissionText(text), /*dryRun=*/false, errors, warnings, &errorLines);
	_suppressTreeUndo = false;

	showAdvancedResults(errors, warnings, errorLines);

	if (!ok)
		return false;

	// Commit the whole hand-edit as one undo entry, then refresh the baseline
	// so a subsequent no-op leave doesn't re-commit.
	pushEventStateSnapshot(before, tr("Advanced Edit Events"));
	_advancedBaseline = text;
	m_last_message_node = -1;
	applyEventFilter();
	updateEventUi();
	return true;
}

void MissionEventsDialog::setCurrentEventView(int index)
{
	ui->eventViewStack->setCurrentIndex(index);

	if (auto* btn = _viewGroup->button(index)) {
		QSignalBlocker blocker(_viewGroup);
		btn->setChecked(true);
	}

	// The search box is per-view (filter vs. find), so start each view visit
	// with an empty box rather than carrying the other view's term over.
	// Blocked so this doesn't run a filter/find before the view is set up.
	{
		QSignalBlocker blocker(ui->eventSearchEdit);
		ui->eventSearchEdit->clear();
	}
	applyEventFilter();

	// Regenerate the section text whenever the advanced view becomes current.
	if (index == AdvancedViewIndex)
		loadAdvancedText();

	// Rebuild the relationship graph when the graph view becomes current, but
	// only if the events changed since we last built it -- otherwise leave the
	// widget untouched so its zoom/scroll/selection carry over.
	if (index == GraphViewIndex) {
		ui->eventGraph->setSearchText(QString()); // start with no highlight
		if (_graphDirty)
			refreshGraphView();
	}

	s_lastEventViewIndex = index;
	applyViewChrome(index);
}

void MissionEventsDialog::initGraphView()
{
	ui->eventGraph->setReferenceIndex(&_refIndex);
	// Single-click selects the event in the tree (kept in sync for when the
	// tree view is next shown); double-click switches to the tree view.
	connect(ui->eventGraph, &EventGraphView::eventSelected, this, &MissionEventsDialog::selectEventInTree);
	connect(ui->eventGraph, &EventGraphView::eventActivated, this, &MissionEventsDialog::jumpToEventInTree);
	connect(ui->eventGraph, &EventGraphView::nodeActivated, this, &MissionEventsDialog::jumpToNodeInTree);
	// Re-evaluate the event-property controls whenever the graph selection changes
	// (they're live only while an event card is selected).
	connect(ui->eventGraph, &EventGraphView::graphSelectionChanged, this, &MissionEventsDialog::updateEventUi);
	// The New/Insert/Delete buttons depend on the graph sub-mode (New is Basic-only).
	connect(ui->eventGraph, &EventGraphView::modeChanged, this, &MissionEventsDialog::updateEventCreateButtons);
	// Right-click a node -> comment/color annotation menu (same undo path as the
	// tree's note/color edits).
	connect(ui->eventGraph, &EventGraphView::nodeContextMenuRequested, this, &MissionEventsDialog::showGraphNodeMenu);
	// Double-click an inline literal-arg bullet -> open its editor (number: quick-search; string: dialog).
	connect(ui->eventGraph, &EventGraphView::nodeEditRequested, this, &MissionEventsDialog::editGraphNode);
	// Collapse/expand an event's subtree (Basic view).
	connect(ui->eventGraph, &EventGraphView::eventCollapseToggled, this, &MissionEventsDialog::toggleEventCollapse);
	// Basic view: persist a dragged node's position on its annotation, as one
	// undo step (the same before/after snapshot pattern as note/color edits).
	connect(ui->eventGraph, &EventGraphView::nodesMoved, this, [this](const QVector<QPair<int, QPointF>>& moves) {
		if (moves.isEmpty())
			return;
		const QByteArray before = _model->captureEventWorkingState();
		for (const auto& m : moves)
			_model->setNodeGraphPos(m.first, static_cast<float>(m.second.x()), static_cast<float>(m.second.y()));
		pushEventStateSnapshot(before, moves.size() > 1 ? tr("Move Graph Nodes") : tr("Move Graph Node"));
	});
	// Data is (re)loaded when the graph view is first shown (refreshGraphView),
	// mirroring how the advanced view loads its text on entry. Like the tree view,
	// the graph is a working-copy view and does not react to external mission edits.
}

void MissionEventsDialog::rebuildReferenceIndex()
{
	_refIndex.rebuild(ui->eventTree->_model, _model->getEventList());
}

void MissionEventsDialog::refreshGraphView()
{
	rebuildReferenceIndex();

	QVector<QString> names;
	const auto& events = _model->getEventList();
	names.reserve(static_cast<int>(events.size()));
	for (const auto& e : events)
		names.push_back(QString::fromStdString(e.name));

	ui->eventGraph->setEventNames(std::move(names));
	ui->eventGraph->setObjects(buildGraphObjects());

	// Basic view: build the whole-mission dataflow graph and resolve each node's
	// saved position from the annotations (operator nodes key on their own tree
	// node; shared object nodes key on a representative reference, falling back to
	// any surviving reference if that one was deleted).
	BasicGraph bg = _refIndex.buildBasicGraph(ui->eventTree->_model, events);
	for (auto& en : bg.events) {
		float x = 0.0f, y = 0.0f;
		if (_model->getNodeGraphPos(en.posKey, x, y)) {
			en.hasPos = true;
			en.posX = x;
			en.posY = y;
		}
	}
	for (auto& op : bg.ops) {
		float x = 0.0f, y = 0.0f;
		if (_model->getNodeGraphPos(op.posKey, x, y)) {
			op.hasPos = true;
			op.posX = x;
			op.posY = y;
		}
		// Per-reference positions (used when the view duplicates object nodes).
		for (auto& r : op.objectRefs) {
			float rx = 0.0f, ry = 0.0f;
			if (_model->getNodeGraphPos(r.leafTreeNode, rx, ry)) {
				r.hasPos = true;
				r.posX = rx;
				r.posY = ry;
			}
		}
	}
	for (auto& ob : bg.objects) {
		float x = 0.0f, y = 0.0f;
		if (_model->getNodeGraphPos(ob.posKey, x, y)) {
			ob.hasPos = true;
			ob.posX = x;
			ob.posY = y;
		} else {
			for (int rn : ob.refTreeNodes) {
				if (_model->getNodeGraphPos(rn, x, y)) {
					ob.hasPos = true;
					ob.posX = x;
					ob.posY = y;
					ob.posKey = rn;
					break;
				}
			}
		}
	}
	ui->eventGraph->setBasicGraph(std::move(bg));

	// Per-event status flags (shown as icons on event cards).
	QVector<GraphEventMeta> meta;
	meta.reserve(static_cast<int>(events.size()));
	for (int i = 0; i < static_cast<int>(events.size()); ++i)
		meta.push_back(buildEventMeta(i));
	ui->eventGraph->setEventMeta(std::move(meta));
	_graphEventCount = static_cast<int>(events.size());

	// Node comment/color snapshot, keyed by annotation key.
	QHash<int, GraphAnnotation> annotations;
	for (const event_annotation& ea : _model->nodeAnnotations()) {
		if (ea.node_index == -1)
			continue;
		GraphAnnotation ga;
		ga.comment = QString::fromStdString(ea.comment);
		if (ea.r != 255 || ea.g != 255 || ea.b != 255)
			ga.color = QColor(ea.r, ea.g, ea.b);
		if (ga.comment.isEmpty() && !ga.color.isValid())
			continue; // e.g. a position-only annotation
		annotations.insert(ea.node_index, ga);
	}
	ui->eventGraph->setAnnotations(std::move(annotations));

	// Corner order markers: each sexp node's 1-based position among its siblings.
	QHash<int, int> siblingOrders;
	const auto& nodes = ui->eventTree->_model.tree_nodes;
	for (int p = 0; p < static_cast<int>(nodes.size()); ++p) {
		if (nodes[p].type == SEXPT_UNUSED) // skip free slots (stale child/next links)
			continue;
		int order = 1;
		for (int c = nodes[p].child; c != -1; c = nodes[c].next)
			siblingOrders.insert(c, order++);
	}
	ui->eventGraph->setSiblingOrders(std::move(siblingOrders));

	// Collapsed events (Basic view), from the saved per-event collapse annotations.
	QSet<int> collapsedEvents;
	for (int i = 0; i < static_cast<int>(events.size()); ++i)
		if (_model->getNodeCollapsed(SexpAnnotationModel::rootKey(events[i].formula)))
			collapsedEvents.insert(i);
	ui->eventGraph->setCollapsedEvents(std::move(collapsedEvents));

	ui->eventGraph->reload();

	_graphDirty = false;
}

// Map a graph card's annotation key to the tree item to target. Regular nodes use
// the key directly as a tree_nodes[] index; an event root's rootKey resolves
// through its formula to the top-level (labeled) item, which is not in
// tree_nodes[]. Null when it can't be resolved.
QTreeWidgetItem* MissionEventsDialog::treeItemForAnnotationKey(int key) const
{
	if (key >= 0)
		return ui->eventTree->handle(key);
	if (!SexpAnnotationModel::isRootKey(key))
		return nullptr;
	const int formula = SexpAnnotationModel::formulaFromRootKey(key);
	for (int i = 0; i < ui->eventTree->topLevelItemCount(); ++i) {
		auto* it = ui->eventTree->topLevelItem(i);
		if (it && it->data(0, sexp_tree_view::FormulaDataRole).toInt() == formula)
			return it;
	}
	return nullptr;
}

// Right-click a graph card: reuse the tree's own context menu at the matching
// node (add/replace/insert/delete/data, comment/color, and for event roots the
// name and event options), with irrelevant items disabled exactly as in the tree.
// The tree stays populated behind the graph, so its handlers operate normally.
void MissionEventsDialog::showGraphNodeMenu(int key, const QPoint& globalPos)
{
	QTreeWidgetItem* h = treeItemForAnnotationKey(key);
	if (h == nullptr)
		return;

	// In the graph, "Expand All" is repurposed to expand the clicked card's bullets.
	// The edit itself redraws the graph through the modification handlers (which is
	// necessary anyway, since the inline "Edit Data" editor commits asynchronously
	// after this returns).
	ui->eventTree->showContextMenuForItem(h, globalPos,
		[this, key]() { ui->eventGraph->expandNode(key); },
		ui->eventGraph->nodeExpandable(key));
}

// Double-clicking an inline literal-arg bullet: open that arg node's editor
// directly (the tree routes numeric slots to the quick-search, strings to the
// text dialog). The commit refreshes the graph via the modification handlers.
void MissionEventsDialog::editGraphNode(int treeNode, const QPoint& globalPos)
{
	QTreeWidgetItem* h = treeItemForAnnotationKey(treeNode); // treeNode >= 0 -> tree item
	if (h)
		ui->eventTree->editNodeExternally(h, globalPos);
}

// Toggle whether an event's subtree is collapsed in the Basic graph view. The
// state lives on the event's annotation, so it persists with the mission.
void MissionEventsDialog::toggleEventCollapse(int eventIndex)
{
	const auto& events = _model->getEventList();
	if (eventIndex < 0 || eventIndex >= static_cast<int>(events.size()))
		return;
	const int key = SexpAnnotationModel::rootKey(events[eventIndex].formula);
	const QByteArray before = _model->captureEventWorkingState();
	_model->setNodeCollapsed(key, !_model->getNodeCollapsed(key));
	pushEventStateSnapshot(before, tr("Toggle Event Collapse"));
	refreshGraphView();
}

// Redraw the relationship graph if it's the current view. Called after tree
// edits (structural, rename, annotation) so the cards reflect the change - even
// when the edit commits asynchronously, past the context menu's exec().
void MissionEventsDialog::refreshGraphIfCurrent()
{
	if (ui->eventViewStack->currentIndex() == GraphViewIndex)
		refreshGraphView();
}

// Select an event's root item in the tree without changing the current view.
// Keeps the tree selection (and the model's current event) in sync with a
// graph selection, so switching to the tree view lands on the same event.
void MissionEventsDialog::selectEventInTree(int eventIndex)
{
	const auto& events = _model->getEventList();
	if (eventIndex < 0 || eventIndex >= static_cast<int>(events.size()))
		return;
	const int formula = events[eventIndex].formula;

	for (int i = 0; i < ui->eventTree->topLevelItemCount(); ++i) {
		auto* it = ui->eventTree->topLevelItem(i);
		if (it && it->data(0, sexp_tree_view::FormulaDataRole).toInt() == formula) {
			ui->eventTree->setCurrentItem(it);
			ui->eventTree->scrollToItem(it);
			break;
		}
	}
}

// This dialog edits a working copy of the events. If it was already open with unapplied
// edits, the working copy can differ from Mission_events, so trust the index only when the
// name there still matches; otherwise take the first event with that name.
void MissionEventsDialog::focusEvent(const SCP_string& name, int index)
{
	const auto& events = _model->getEventList();
	int target = -1;
	if (index >= 0 && index < static_cast<int>(events.size()) && events[index].name == name) {
		target = index;
	} else {
		for (size_t i = 0; i < events.size(); ++i) {
			if (events[i].name == name) {
				target = static_cast<int>(i);
				break;
			}
		}
	}
	if (target < 0)
		return;

	// A name filter could be hiding the event, so clear it, as jumping from a message does.
	if (!ui->eventSearchEdit->text().isEmpty()) {
		{
			QSignalBlocker blocker(ui->eventSearchEdit);
			ui->eventSearchEdit->clear();
		}
		applyEventFilter();
	}

	jumpToEventInTree(target);
}

// Double-clicking an event in the graph brings the tree view forward and
// selects that event.
void MissionEventsDialog::jumpToEventInTree(int eventIndex)
{
	// Switch to the tree view (through the veto gate, though graph never vetoes).
	if (ui->eventViewStack->currentIndex() != TreeViewIndex) {
		requestViewChange(TreeViewIndex);
		if (ui->eventViewStack->currentIndex() != TreeViewIndex)
			return;
	}
	selectEventInTree(eventIndex);
}

// Double-clicking a condition/action node in the graph brings the tree view
// forward and highlights that exact sexp node.
void MissionEventsDialog::jumpToNodeInTree(int treeNode)
{
	if (ui->eventViewStack->currentIndex() != TreeViewIndex) {
		requestViewChange(TreeViewIndex);
		if (ui->eventViewStack->currentIndex() != TreeViewIndex)
			return;
	}
	if (treeNode >= 0)
		ui->eventTree->hilite_item(treeNode);
}

// (Re)populate the advanced editor from the current working events and reset
// its baseline and native undo history.
void MissionEventsDialog::loadAdvancedText()
{
	// Always Standard: this is an editing view, not the mission's output. Retail
	// drops every FSO-only field (annotations, log flags), which a commit would
	// then discard for real, and a Retail save also rewrites special tags in the
	// global messages and briefings.
	SCP_string text;
	const bool generated = _model->generateEventsSectionText(MissionFormat::STANDARD, text);
	_advancedBaseline = missionTextToQString(text);

	QSignalBlocker blocker(ui->advancedTextEdit);
	ui->advancedTextEdit->setPlainText(_advancedBaseline);
	// Without the real text there's nothing safe to edit: committing a blank or
	// partial section would replace the mission's events. Read-only keeps the
	// text equal to its (empty) baseline, so leaving the view commits nothing.
	ui->advancedTextEdit->setReadOnly(!generated);
	ui->advancedErrorList->clear();
	if (!generated) {
		ui->advancedErrorList->setPlainText(
			tr("ERROR: Couldn't generate the event text (the scratch save to the temp folder failed). "
			   "Use the Tree or Graph view to edit events."));
	}
	// textChanged was blocked above, so reset the markers and match cache here.
	_advancedErrorLines.clear();
	_advancedMaskDirty = true;
	updateAdvancedSelections();
}

void MissionEventsDialog::showAdvancedResults(const SCP_vector<SCP_string>& errors,
	const SCP_vector<SCP_string>& warnings, const SCP_vector<int>& errorLines)
{
	_advancedErrorLines = errorLines;
	updateAdvancedSelections();

	QString out;
	for (const auto& e : errors)
		out += QStringLiteral("ERROR: ") + QString::fromStdString(e) + QLatin1Char('\n');
	for (const auto& w : warnings)
		out += QStringLiteral("WARNING: ") + QString::fromStdString(w) + QLatin1Char('\n');
	if (errors.empty() && warnings.empty())
		out = tr("No problems found.");
	ui->advancedErrorList->setPlainText(out);
}

void MissionEventsDialog::on_btnValidateEvents_clicked()
{
	SCP_vector<SCP_string> errors, warnings;
	SCP_vector<int> errorLines;
	_model->applyEventsText(qStringToMissionText(ui->advancedTextEdit->toPlainText()),
		/*dryRun=*/true, errors, warnings, &errorLines);
	showAdvancedResults(errors, warnings, errorLines);
}

void MissionEventsDialog::updateAdvancedSelections()
{
	auto* edit = ui->advancedTextEdit;
	QList<QTextEdit::ExtraSelection> selections;
	const QColor errorColor(220, 50, 50);

	// Validate markers: a faint full-width tint plus a wavy underline under the text.
	for (int line : _advancedErrorLines) {
		const QTextBlock block = edit->document()->findBlockByNumber(line - 1);
		if (!block.isValid())
			continue;
		QTextEdit::ExtraSelection tint;
		tint.format.setBackground(QColor(errorColor.red(), errorColor.green(), errorColor.blue(), 40));
		tint.format.setProperty(QTextFormat::FullWidthSelection, true);
		tint.cursor = QTextCursor(block);
		selections.push_back(tint);

		QTextEdit::ExtraSelection underline;
		underline.format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
		underline.format.setUnderlineColor(errorColor);
		underline.cursor = QTextCursor(block);
		underline.cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		selections.push_back(underline);
	}

	// Bracket matching for a parenthesis at or just before the cursor. Parens in
	// strings and comments don't count, using the highlighter's own lexing rules.
	const QTextCursor cursor = edit->textCursor();
	if (!cursor.hasSelection()) {
		if (_advancedMaskDirty) {
			_advancedText = edit->toPlainText();
			_advancedCodeMask = fso::fred::MissionTextHighlighter::codeMask(_advancedText);
			_advancedMaskDirty = false;
		}
		const QString& text = _advancedText;
		auto isParen = [this, &text](int p) {
			return p >= 0 && p < text.size() && _advancedCodeMask[p] &&
				(text[p] == QLatin1Char('(') || text[p] == QLatin1Char(')'));
		};
		const int pos = cursor.position();
		const int at = isParen(pos) ? pos : (isParen(pos - 1) ? pos - 1 : -1);
		if (at >= 0) {
			const bool open = text[at] == QLatin1Char('(');
			int depth = 0;
			int match = -1;
			for (int p = at; p >= 0 && p < text.size(); p += open ? 1 : -1) {
				if (!_advancedCodeMask[p])
					continue;
				if (text[p] == QLatin1Char('('))
					depth += open ? 1 : -1;
				else if (text[p] == QLatin1Char(')'))
					depth += open ? -1 : 1;
				if (depth == 0) {
					match = p;
					break;
				}
			}
			QColor matchColor = edit->palette().highlight().color();
			matchColor.setAlpha(90);
			auto addBracket = [&selections, edit](int p, const QColor& background) {
				QTextEdit::ExtraSelection s;
				s.format.setBackground(background);
				s.cursor = QTextCursor(edit->document());
				s.cursor.setPosition(p);
				s.cursor.setPosition(p + 1, QTextCursor::KeepAnchor);
				selections.push_back(s);
			};
			if (match >= 0) {
				addBracket(at, matchColor);
				addBracket(match, matchColor);
			} else {
				// No partner: flag the unbalanced bracket.
				addBracket(at, QColor(errorColor.red(), errorColor.green(), errorColor.blue(), 110));
			}
		}
	}

	edit->setExtraSelections(selections);
}

// Each view declares which of the surrounding event controls make sense. Per
// the dialog's UI-stability rule the controls are disabled, never hidden, so
// the layout doesn't shift between views.
void MissionEventsDialog::applyViewChrome(int index)
{
	const bool treeView = (index == TreeViewIndex);
	const bool advancedView = (index == AdvancedViewIndex);
	const bool graphView = (index == GraphViewIndex);

	// The search box filters in tree view, finds-in-text in advanced view, and
	// highlights matching nodes in the graph view; its placeholder follows suit.
	ui->eventSearchEdit->setEnabled(treeView || advancedView || graphView);
	ui->eventSearchEdit->setPlaceholderText(advancedView ? tr("Find text...")
		: graphView         ? tr("Highlight nodes...")
							  : tr("Filter by name..."));

	// The find arrows step through advanced-text matches or, in the graph view,
	// through the highlighted nodes; hidden in the tree view. In the graph they're
	// only live while there is a query to step through.
	const bool findArrows = advancedView || graphView;
	ui->btnFindPrev->setVisible(findArrows);
	ui->btnFindNext->setVisible(findArrows);
	const bool arrowsEnabled = advancedView || (graphView && !ui->eventSearchEdit->text().isEmpty());
	ui->btnFindPrev->setEnabled(arrowsEnabled);
	ui->btnFindNext->setEnabled(arrowsEnabled);

	// The event-controls column acts on the selected event, which the graph view
	// also tracks (an event-node selection sets m_cur_event just like the tree).
	// So it's live in tree and graph; the reorder buttons stay tree-only via their
	// own gate in updateEventMoveButtons.
	ui->eventControlsContainer->setEnabled(treeView || graphView);

	// The sexp help panels describe the selected tree node, so they're
	// meaningless outside the tree view: clear them on the way out and
	// recompute for the current selection when the tree view returns.
	if (treeView) {
		ui->eventTree->update_help(ui->eventTree->currentItem());
	} else {
		ui->miniHelpBox->clear();
		ui->helpBox->clear();
	}

	// The per-event fields key off the tree selection; updateEventUi()
	// disables them whenever the tree view isn't current.
	updateEventUi();
}

int MissionEventsDialog::getRootReturnType() const
{
	return OPR_NULL;
}

void MissionEventsDialog::accept()
{
	// The active view gets to veto OK (e.g. advanced-edit text that doesn't
	// parse), same as it can veto a view switch.
	if (!canLeaveEventView(ui->eventViewStack->currentIndex()))
		return;

	QByteArray stateBefore = _model->captureState();
	if (_model->apply()) {
		QByteArray stateAfter = _model->captureState();
		_model->setParent(nullptr);
		_fredView->mainUndoStack()->push(
			new ApplyDialogCommand(std::move(_model), stateBefore, stateAfter,
			                       _viewport->editor, tr("Edit Mission Events")));
		_dialogStack->clear();
		_fredView->undoGroup()->setActiveStack(_fredView->mainUndoStack());
		QDialog::accept();
	}
}

void MissionEventsDialog::reject()
{
	if (!_model) {
		_dialogStack->clear();
		_fredView->undoGroup()->setActiveStack(_fredView->mainUndoStack());
		QDialog::reject();
		return;
	}
	if (rejectOrCloseHandler(this, _model.get(), _viewport)) {
		_dialogStack->clear();
		_fredView->undoGroup()->setActiveStack(_fredView->mainUndoStack());
		QDialog::reject();
	}
}

void MissionEventsDialog::onEventTreeModified()
{
	_model->setModified();
	if (_suppressTreeUndo)
		return;

	pushEventStateSnapshot(_workingStateCache, tr("Edit Event Formula"));
	refreshGraphIfCurrent(); // e.g. an inline "Edit Data" value edit that commits now
}

void MissionEventsDialog::pushEventStateSnapshot(const QByteArray& before, const QString& label)
{
	const QByteArray after = _model->captureEventWorkingState();
	if (after == before)
		return;

	_dialogStack->push(new DialogSnapshotCommand(before, after,
		[this](const QByteArray& blob) {
			_suppressTreeUndo = true;
			const auto expanded = ui->eventTree->captureExpansionState();
			// The selected node, by path, so it can be selected again after the rebuild (the
			// cutscene camera preview follows the selection, so this keeps it on screen)
			SCP_list<int> selectedPath;
			if (auto* item = ui->eventTree->currentItem()) {
				const int node = ui->eventTree->get_node(item);
				if (node >= 0)
					selectedPath = SexpAnnotationModel::buildPath(node, ui->eventTree->_model.tree_nodes, _model->getEventList());
			}
			// Rebuilds the tree widget through the model's treeCleared/
			// subtreeAdded/annotationApplied/rootSelected signals.
			_model->restoreEventWorkingState(blob);
			ui->eventTree->restoreExpansionState(expanded);
			// Only the tree view treats eventSearchEdit as a row filter; in graph/advanced
			// it holds a highlight/find term, and the tree is hidden and re-filtered on the
			// way back, so don't hide rows by the wrong term here.
			if (ui->eventViewStack->currentIndex() == TreeViewIndex)
				applyEventFilter();
			if (!selectedPath.empty() && ui->eventViewStack->currentIndex() == TreeViewIndex) {
				SCP_vector<int> identity(_model->getEventList().size());
				for (int i = 0; i < static_cast<int>(identity.size()); ++i)
					identity[i] = i;
				const int node = SexpAnnotationModel::resolveFromPath(selectedPath, ui->eventTree->_model.tree_nodes,
					_model->getEventList(), identity);
				if (node >= 0)
					ui->eventTree->hilite_item(node);
			}
			m_last_message_node = -1;
			updateEventUi();
			// If the advanced view is showing, the working events just changed
			// under it; regenerate the text (discarding any unparsed edits).
			if (ui->eventViewStack->currentIndex() == AdvancedViewIndex)
				loadAdvancedText();
			// Likewise keep the relationship graph in sync on undo/redo.
			if (ui->eventViewStack->currentIndex() == GraphViewIndex) {
				refreshGraphView();
				// The restore cleared the current event, but the rebuild quietly
				// re-selects cards by their old event index. Push the graph's pick
				// back through the tree so the fields, Delete and the graph agree.
				const int graphEvent = ui->eventGraph->selectedEventIndex();
				if (graphEvent >= 0) {
					selectEventInTree(graphEvent);
					// If that tree row was already current, no selection signal fired.
					if (_model->getCurrentlySelectedEvent() != graphEvent) {
						_model->setCurrentlySelectedEvent(graphEvent);
						updateEventUi();
					}
				}
				updateEventCreateButtons();
			}
			_suppressTreeUndo = false;
		},
		label));
}

void MissionEventsDialog::pushMessageStateSnapshot(const QByteArray& before, const QString& label, int mergeId)
{
	const QByteArray after = _model->captureMessageWorkingState();
	if (after == before)
		return;

	_dialogStack->push(new DialogSnapshotCommand(before, after,
		[this](const QByteArray& blob) {
			_model->restoreMessageWorkingState(blob);
			rebuildMessageList();
			updateMessageUi();
		},
		label, true, mergeId));
}

SCP_vector<SCP_string> MissionEventsDialog::getMessages()
{
	SCP_vector<SCP_string> out;
	const auto& msgs = _model->getMessageList();
	out.reserve(msgs.size());
	for (const auto& m : msgs) {
		out.emplace_back(m.name);
	}
	return out;
}

bool MissionEventsDialog::hasDefaultMessageParameter()
{
	return !_model->getMessageList().empty();
}

// Event names come from the working events, like messages above, not from Mission_events.
// Otherwise an event added or renamed in this editor wasn't a valid choice: the lists
// didn't offer it, and editing another argument of the same operator re-validated the
// arguments and replaced its name with the first saved event.
SCP_vector<SCP_string> MissionEventsDialog::getMissionEvents(const SCP_string& reference_name)
{
	if (!_model)
		return SexpTreeEditorInterface::getMissionEvents(reference_name);

	SCP_vector<SCP_string> out;
	const auto& events = _model->getEventList();
	out.reserve(events.size());
	for (const auto& ev : events) {
		out.emplace_back(ev.name, 0, NAME_LENGTH - 1);
	}
	return out;
}

bool MissionEventsDialog::hasDefaultEvent(int operator_value)
{
	if (!_model)
		return SexpTreeEditorInterface::hasDefaultEvent(operator_value);

	return (operator_value == OP_PREVIOUS_EVENT_TRUE) || (operator_value == OP_PREVIOUS_EVENT_FALSE)
		|| (operator_value == OP_PREVIOUS_EVENT_INCOMPLETE) || !_model->getEventList().empty();
}

void MissionEventsDialog::closeEvent(QCloseEvent* e)
{
	reject();
	// reject() hides the dialog when it actually closes. Let that close
	// proceed (so a dialog created with WA_DeleteOnClose is destroyed),
	// and only veto it when reject() decided to keep the dialog open (e.g.
	// the user cancelled the unsaved-changes prompt).
	if (isVisible()) {
		e->ignore();
	} else {
		e->accept();
	}
}

void MissionEventsDialog::initMessageWidgets() {
	initHeadCombo();
	initWaveFilenames();
	initPersonas();
	initMessageTeams();

	initMessageList();

	ui->messageName->setMaxLength(NAME_LENGTH - 1);
	connect(ui->messageName, &QLineEdit::editingFinished, this, &MissionEventsDialog::onMessageNameEditingFinished);

	if (auto* le = ui->aniCombo->lineEdit()) {
		le->setMaxLength(MAX_FILENAME_LEN - 1);
		connect(le, &QLineEdit::editingFinished, this, &MissionEventsDialog::on_aniCombo_editingFinished);
	}

	if (auto* le = ui->waveCombo->lineEdit()) {
		le->setMaxLength(MAX_FILENAME_LEN - 1);
		connect(le, &QLineEdit::editingFinished, this, &MissionEventsDialog::on_waveCombo_editingFinished);
	}

	updateMessageUi();
}

void MissionEventsDialog::rootNodeDeleted(int node) {
	// The widget emits before freeing the branch, so this capture still
	// serializes the deleted event's formula and annotations. The modified()
	// it emits afterwards is absorbed by the snapshot equality check.
	const QByteArray before = _model->captureEventWorkingState();
	_model->deleteRootNode(node);
	pushEventStateSnapshot(before, tr("Delete Event"));
	refreshGraphIfCurrent();
}

void MissionEventsDialog::rootNodeRenamed(int node) {
	QTreeWidgetItem* item = nullptr;
	for (int i = 0; i < ui->eventTree->topLevelItemCount(); ++i) {
		auto* it = ui->eventTree->topLevelItem(i);
		if (it && it->data(0, sexp_tree_view::FormulaDataRole).toInt() == node) {
			item = it;
			break;
		}
	}
	if (!item)
		return;

	SCP_string newText = item->text(0).toUtf8().constData();

	const auto& events = _model->getEventList();
	int index = -1;
	for (int i = 0; i < static_cast<int>(events.size()); ++i) {
		if (events[i].formula == node) {
			index = i;
			break;
		}
	}
	if (index < 0)
		return;

	const SCP_string before = events[index].name;
	_model->renameRootNode(node, newText);
	const SCP_string after = _model->getEventList()[index].name;
	if (before == after)
		return;

	auto* cmd = new FieldEditCommand<SCP_string>(
	    FieldId::Event_Name + index * FieldId::Event_FieldStride,
	    nullptr, tr("Rename Event"), true);
	cmd->addEntry(before, after, [this, index](const SCP_string& v) {
		_model->setEventNameAt(index, v);
		syncEventRootLabel(index);
		refreshGraphIfCurrent(); // undo/redo of a rename should update the graph card
	});
	_dialogStack->push(cmd);

	// The new name may change whether the event matches an active filter.
	applyEventFilter();
	refreshGraphIfCurrent(); // the inline name edit commits after the menu has closed
}

void MissionEventsDialog::syncEventRootLabel(int eventIndex)
{
	const auto& events = _model->getEventList();
	if (!SCP_vector_inbounds(events, eventIndex))
		return;

	const int formula = events[eventIndex].formula;
	for (int i = 0; i < ui->eventTree->topLevelItemCount(); ++i) {
		auto* item = ui->eventTree->topLevelItem(i);
		if (item && item->data(0, sexp_tree_view::FormulaDataRole).toInt() == formula) {
			item->setText(0, QString::fromStdString(events[eventIndex].name));
			break;
		}
	}

	// The restored name may change whether the event matches an active filter.
	applyEventFilter();
}

void MissionEventsDialog::rootNodeFormulaChanged(int old, int node) {
	_model->changeRootNodeFormula(old, node);
}

void MissionEventsDialog::rootNodeSelectedByFormula(int formula) {
	_model->setCurrentlySelectedEventByFormula(formula);
	updateEventUi();
}

void MissionEventsDialog::initMessageList() {
	rebuildMessageList();

	_model->setCurrentlySelectedMessage(_model->getMessageList().empty() ? -1 : 0);
}

void MissionEventsDialog::rebuildMessageList() {
	// Block signals so that the current item index isn't overwritten by this
	QSignalBlocker blocker(ui->messageList);

	const int curRow = _model->getCurrentlySelectedMessage();

	ui->messageList->clear();
	for (auto& msg : _model->getMessageList()) {
		auto item = new QListWidgetItem(msg.name, ui->messageList);
		ui->messageList->addItem(item);
	}

	if (curRow >= 0 && curRow < ui->messageList->count()) {
		ui->messageList->setCurrentRow(curRow);
	}

	applyMessageFilter();
}

// The log-state checkboxes only apply to a selected event, so gray them out (and
// clear their stale state) when nothing is selected.
void MissionEventsDialog::setEventLogEnabled(bool enable)
{
	for (auto* cb : {ui->checkLogTrue, ui->checkLogFalse, ui->checkLogPrevious, ui->checkLogAlwaysFalse,
			ui->checkLogFirstRepeat, ui->checkLogLastRepeat, ui->checkLogFirstTrigger, ui->checkLogLastTrigger}) {
		cb->setEnabled(enable);
		if (!enable)
			cb->setChecked(false);
	}
}

void MissionEventsDialog::updateEventUi() {
	util::SignalBlockers blockers(this);

	updateEventMoveButtons();
	updateEventCreateButtons();

	// The per-event fields act on the selected event. In the tree, selecting any
	// node selects its event; in the graph, only an event card counts (selecting an
	// operator / object / data node leaves the fields disabled, for clarity).
	const int curView = ui->eventViewStack->currentIndex();
	const bool controlsActive = (curView == TreeViewIndex)
		|| (curView == GraphViewIndex && ui->eventGraph->isEventNodeSelected());

	if (!_model->eventIsValid() || !controlsActive) {
		ui->repeatCountBox->setValue(1);
		ui->triggerCountBox->setValue(1);
		ui->intervalTimeBox->setValue(1);
		ui->chainDelayBox->setValue(0);
		ui->teamCombo->setCurrentIndex(0); // was MAX_TVT_TEAMS for none?
		ui->editDirectiveText->setText("");
		ui->editDirectiveKeypressText->setText("");

		ui->repeatCountBox->setEnabled(false);
		ui->triggerCountBox->setEnabled(false);
		ui->intervalTimeBox->setEnabled(false);
		ui->chainedCheckBox->setChecked(false);
		ui->chainedCheckBox->setEnabled(false);
		ui->chainDelayBox->setEnabled(false);
		ui->scoreBox->setValue(0);
		ui->scoreBox->setEnabled(false);
		ui->useMsecsCheckBox->setChecked(false);
		ui->useMsecsCheckBox->setEnabled(false);
		ui->teamCombo->setEnabled(false);
		ui->editDirectiveText->setEnabled(false);
		ui->editDirectiveKeypressText->setEnabled(false);
		setEventLogEnabled(false);
		syncGraphAfterEventUi();
		return;
	}

	ui->teamCombo->setCurrentIndex(ui->teamCombo->findData(_model->getEventTeam()));

	ui->repeatCountBox->setValue(_model->getRepeatCount());
	ui->triggerCountBox->setValue(_model->getTriggerCount());
	ui->intervalTimeBox->setValue(_model->getIntervalTime());
	ui->scoreBox->setValue(_model->getEventScore());
	if (_model->getChained()) {
		ui->chainedCheckBox->setChecked(true);
		ui->chainDelayBox->setValue(_model->getChainDelay());
		ui->chainDelayBox->setEnabled(true);
	} else {
		ui->chainedCheckBox->setChecked(false);
		ui->chainDelayBox->setValue(0);
		ui->chainDelayBox->setEnabled(false);
	}
	ui->useMsecsCheckBox->setChecked(_model->getUseMsecs());

	ui->editDirectiveText->setText(QString::fromStdString(_model->getEventDirectiveText()));
	ui->editDirectiveKeypressText->setText(QString::fromStdString(_model->getEventDirectiveKeyText()));

	ui->repeatCountBox->setEnabled(true);
	ui->triggerCountBox->setEnabled(true);

	if ((_model->getRepeatCount() > 1) || (_model->getRepeatCount() < 0) ||
		(_model->getTriggerCount() > 1) || (_model->getTriggerCount() < 0)) {
		ui->intervalTimeBox->setEnabled(true);
	} else {
		ui->intervalTimeBox->setValue(_model->getIntervalTime());
		ui->intervalTimeBox->setEnabled(false);
	}

	ui->scoreBox->setEnabled(true);
	ui->chainedCheckBox->setEnabled(true);
	ui->useMsecsCheckBox->setEnabled(true);
	ui->editDirectiveText->setEnabled(true);
	ui->editDirectiveKeypressText->setEnabled(true);
	ui->teamCombo->setEnabled(_model->getMissionIsMultiTeam());

	// handle event log flags
	setEventLogEnabled(true);
	ui->checkLogTrue->setChecked(_model->getLogTrue());
	ui->checkLogFalse->setChecked(_model->getLogFalse());
	ui->checkLogPrevious->setChecked(_model->getLogLogPrevious());
	ui->checkLogAlwaysFalse->setChecked(_model->getLogAlwaysFalse());
	ui->checkLogFirstRepeat->setChecked(_model->getLogFirstRepeat());
	ui->checkLogLastRepeat->setChecked(_model->getLogLastRepeat());
	ui->checkLogFirstTrigger->setChecked(_model->getLogFirstTrigger());
	ui->checkLogLastTrigger->setChecked(_model->getLogLastTrigger());

	syncGraphAfterEventUi();
}

// For an undo/redo of an event field: the event it changes needn't be the
// selected one, and syncGraphAfterEventUi() only refreshes the selected card.
void MissionEventsDialog::refreshEventAfterFieldUndo(int eventIndex)
{
	updateEventUi();
	if (ui->eventViewStack->currentIndex() == GraphViewIndex && eventIndex >= 0 &&
		eventIndex < static_cast<int>(_model->getEventList().size()))
		ui->eventGraph->updateEventCard(eventIndex, buildEventMeta(eventIndex));
}

// Reflect event-property edits onto the graph while it's the current view: a
// lightweight per-card status/chain update when only a field changed, or a full
// rebuild when the event count changed (event added/removed).
void MissionEventsDialog::syncGraphAfterEventUi()
{
	if (ui->eventViewStack->currentIndex() != GraphViewIndex)
		return;
	const int count = static_cast<int>(_model->getEventList().size());
	if (count != _graphEventCount) {
		refreshGraphView(); // sets _graphEventCount
		return;
	}
	if (_model->eventIsValid()) {
		const int cur = _model->getCurrentlySelectedEvent();
		ui->eventGraph->updateEventCard(cur, buildEventMeta(cur));
	}
}

// Build the per-event status/annotation-key metadata for one event.
GraphEventMeta MissionEventsDialog::buildEventMeta(int eventIndex) const
{
	GraphEventMeta m;
	const auto& events = _model->getEventList();
	if (eventIndex < 0 || eventIndex >= static_cast<int>(events.size()))
		return m;
	const mission_event& e = events[eventIndex];
	m.chained = (e.chain_delay >= 0);
	m.directive = !e.objective_text.empty();
	m.repeats = (e.repeat_count != 1) || (e.flags & MEF_USING_TRIGGER_COUNT);
	m.logging = (e.mission_log_flags != 0);
	m.annotationKey = SexpAnnotationModel::rootKey(e.formula);
	return m;
}

void MissionEventsDialog::updateEventMoveButtons()
{
	auto* cur = ui->eventTree->currentItem();

	const bool isRoot = (cur && !cur->parent());
	const int count = ui->eventTree->topLevelItemCount();

	// Moving while the tree is filtered would reorder relative to hidden
	// neighbors, which looks like nothing happened; disable the buttons.
	const bool filtered = !ui->eventSearchEdit->text().isEmpty();

	// The move buttons act on the tree order, so they need the tree in view.
	const bool treeViewActive = (ui->eventViewStack->currentIndex() == TreeViewIndex);

	bool canUp = false, canDown = false;

	if (isRoot && count > 1 && !filtered && treeViewActive) {
		const int idx = ui->eventTree->indexOfTopLevelItem(cur);
		canUp = (idx > 0);
		canDown = (idx >= 0 && idx < count - 1);
	}

	ui->eventMoveTopBtn->setEnabled(canUp);
	ui->eventUpBtn->setEnabled(canUp);
	ui->eventDownBtn->setEnabled(canDown);
	ui->eventMoveBottomBtn->setEnabled(canDown);
}

// New/Insert/Delete enablement per view. In the graph: New only in Basic (the new
// node needs a spatial home), Insert never (it needs the tree order), Delete in
// any mode once an event card is selected. The tree keeps them all live.
void MissionEventsDialog::updateEventCreateButtons()
{
	const int view = ui->eventViewStack->currentIndex();
	const bool tree = (view == TreeViewIndex);
	const bool graph = (view == GraphViewIndex);
	const bool basicGraph = graph && ui->eventGraph->isBasicMode();

	ui->btnNewEvent->setEnabled(tree || basicGraph);
	ui->btnInsertEvent->setEnabled(tree);
	ui->btnDeleteEvent->setEnabled(tree || (graph && ui->eventGraph->isEventNodeSelected()));
}

void MissionEventsDialog::initHeadCombo() {
	QSignalBlocker blocker(ui->aniCombo);

	auto list = _model->getHeadAniList();

	ui->aniCombo->clear();

	for (auto& head : list) {
		ui->aniCombo->addItem(QString().fromStdString(head));
	}
}

void MissionEventsDialog::initWaveFilenames() {
	QSignalBlocker blocker(ui->waveCombo);

	auto list = _model->getWaveList();

	ui->waveCombo->clear();

	for (auto& wave : list) {
		ui->waveCombo->addItem(QString().fromStdString(wave));
	}
}

void MissionEventsDialog::initPersonas() {
	QSignalBlocker blocker(ui->personaCombo);

	ui->personaCombo->clear();

	for (auto&& [name, id] : _model->getPersonaList()) {
		ui->personaCombo->addItem(QString::fromStdString(name), id);
	}
}

void MissionEventsDialog::initMessageTeams() {
	QSignalBlocker blocker(ui->messageTeamCombo);

	ui->messageTeamCombo->clear();

	for (const auto& team : _model->getTeamList()) {
		ui->messageTeamCombo->addItem(QString::fromStdString(team.first), team.second);
	}
}

void MissionEventsDialog::initEventTeams()
{
	QSignalBlocker blocker(ui->teamCombo);

	ui->teamCombo->clear();

	for (const auto& team : _model->getTeamList()) {
		ui->teamCombo->addItem(QString::fromStdString(team.first), team.second);
	}
}

void MissionEventsDialog::updateMessageUi()
{
	// Block child signals: setText/setPlainText/combo updates below would
	// otherwise re-enter the edit slots and mark the mission modified on a
	// mere selection change.
	util::SignalBlockers blockers(this);

	// The name field is reloaded from the model below, which only holds valid names
	showMessageNameConflict({});

	bool enable = true;

	if (!_model->messageIsValid()) {
		enable = false;

		ui->messageName->setText("");
		ui->messageContent->setPlainText("");
		ui->aniCombo->setEditText("");
		ui->personaCombo->setCurrentIndex(-1);
		ui->waveCombo->setEditText("");
		ui->messageTeamCombo->setCurrentIndex(-1);
		ui->btnMsgNote->setText("Add Note");
	} else {
		ui->messageName->setText(QString().fromStdString(_model->getMessageName()));
		ui->messageContent->setPlainText(QString().fromStdString(_model->getMessageText()));
		ui->aniCombo->setEditText(QString().fromStdString(_model->getMessageAni()));
		ui->personaCombo->setCurrentIndex(ui->personaCombo->findData(_model->getMessagePersona()));
		ui->waveCombo->setEditText(QString().fromStdString(_model->getMessageWave()));
		ui->messageTeamCombo->setCurrentIndex(ui->messageTeamCombo->findData(_model->getMessageTeam()));
		if (_model->getMessageNote().empty()) {
			ui->btnMsgNote->setText("Add Note");
		} else {
			ui->btnMsgNote->setText("Edit Note");
		}
	}

	ui->messageName->setEnabled(enable);
	ui->messageContent->setEnabled(enable);
	ui->aniCombo->setEnabled(enable);
	ui->btnAniBrowse->setEnabled(enable);
	ui->btnBrowseWave->setEnabled(enable);
	ui->btnWavePlay->setEnabled(enable);
	ui->waveCombo->setEnabled(enable);
	ui->btnDeleteMsg->setEnabled(enable);
	ui->personaCombo->setEnabled(enable);
	ui->messageTeamCombo->setEnabled(enable && _model->getMissionIsMultiTeam());
	ui->btnMsgNote->setEnabled(enable);

	updateWaveInfo();

	updateMessageMoveButtons();
}

// One line under the wave file: its length and format, or why the game can't play it
void MissionEventsDialog::updateWaveInfo()
{
	const auto wave = _model->messageIsValid() ? _model->getMessageWave() : SCP_string();
	if (wave.empty() || wave == "<None>") {
		ui->waveInfoLabel->clear();
		return;
	}

	const auto info = _model->getMessageWaveInfo();
	if (!info.found) {
		// the same red as the message name conflict border
		ui->waveInfoLabel->setStyleSheet(QStringLiteral("QLabel { color: #d9534f; }"));
		ui->waveInfoLabel->setText(tr("File not found"));
		return;
	}
	ui->waveInfoLabel->setStyleSheet(QString());

	QStringList parts;
	const double seconds = info.props.duration;
	if (seconds > 0.0 && std::isfinite(seconds)) {
		const int tenths = static_cast<int>(std::lround(seconds * 10.0));
		parts << QString("%1:%2.%3").arg(tenths / 600).arg((tenths / 10) % 60, 2, 10, QChar('0')).arg(tenths % 10);
	} else {
		parts << tr("Unknown length");
	}
	if (info.props.sample_rate > 0)
		parts << tr("%1 kHz").arg(info.props.sample_rate / 1000.0, 0, 'g', 3);
	if (info.props.num_channels == 1)
		parts << tr("mono");
	else if (info.props.num_channels == 2)
		parts << tr("stereo");
	else if (info.props.num_channels > 2)
		parts << tr("%1 channels").arg(info.props.num_channels);
	parts << QLocale().formattedDataSize(static_cast<qint64>(info.size), 1, QLocale::DataSizeTraditionalFormat);

	ui->waveInfoLabel->setText(parts.join(QStringLiteral(", ")));
}

void MissionEventsDialog::updateMessageMoveButtons()
{
	const int count = ui->messageList->count();
	const int row = ui->messageList->currentItem() ? ui->messageList->row(ui->messageList->currentItem()) : -1;

	// Same reasoning as updateEventMoveButtons: no reordering while filtered.
	const bool filtered = !ui->messageSearchEdit->text().isEmpty();

	const bool hasSel = (row >= 0) && !filtered;
	const bool canUp = hasSel && row > 0;
	const bool canDown = hasSel && row < count - 1;

	ui->msgMoveTopBtn->setEnabled(canUp);
	ui->msgUpBtn->setEnabled(canUp);
	ui->msgDownBtn->setEnabled(canDown);
	ui->msgMoveBottomBtn->setEnabled(canDown);
}

// Hide top-level event items whose names don't contain the search text.
// Hidden items keep their top-level indices, so the formula-order reads and
// move logic that iterate topLevelItem(i) are unaffected.
void MissionEventsDialog::applyEventFilter()
{
	const QString text = ui->eventSearchEdit->text();
	for (int i = 0; i < ui->eventTree->topLevelItemCount(); ++i) {
		auto* item = ui->eventTree->topLevelItem(i);
		item->setHidden(!item->text(0).contains(text, Qt::CaseInsensitive));
	}
	updateEventMoveButtons();
}

// Hide message rows that don't contain the search text. Rows are hidden in
// place (never removed) so the row == model-index mapping stays intact.
void MissionEventsDialog::applyMessageFilter()
{
	const QString text = ui->messageSearchEdit->text();
	for (int i = 0; i < ui->messageList->count(); ++i) {
		auto* item = ui->messageList->item(i);
		item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
	}
	updateMessageMoveButtons();
}

void MissionEventsDialog::on_eventSearchEdit_textChanged(const QString& /*text*/)
{
	// In the advanced view the box is a find-as-you-type field over the text.
	if (ui->eventViewStack->currentIndex() == AdvancedViewIndex) {
		findInAdvancedText(/*forward=*/true, /*incremental=*/true);
		return;
	}
	// In the graph view it highlights matching nodes (dims the rest); the arrows
	// step through those matches.
	if (ui->eventViewStack->currentIndex() == GraphViewIndex) {
		const QString q = ui->eventSearchEdit->text();
		ui->eventGraph->setSearchText(q);
		ui->btnFindPrev->setEnabled(!q.isEmpty());
		ui->btnFindNext->setEnabled(!q.isEmpty());
		return;
	}

	applyEventFilter();

	// Deselect if the current item's event got filtered out, so the event
	// controls on the right don't keep editing an invisible event.
	auto* root = ui->eventTree->currentItem();
	while (root && root->parent())
		root = root->parent();
	if (root && root->isHidden()) {
		ui->eventTree->setCurrentItem(nullptr);
		// selectedRootChanged(-1) can't resolve a formula, so clear the model
		// selection directly.
		_model->setCurrentlySelectedEvent(-1);
		updateEventUi();
	}
}

void MissionEventsDialog::findInAdvancedText(bool forward, bool incremental)
{
	auto* edit = ui->advancedTextEdit;
	const QString term = ui->eventSearchEdit->text();
	if (term.isEmpty()) {
		// Drop any highlighted match so an emptied box doesn't look "stuck".
		QTextCursor c = edit->textCursor();
		c.clearSelection();
		edit->setTextCursor(c);
		return;
	}

	// While typing, re-search from the start of the current match so the
	// selection grows in place instead of jumping to the next occurrence.
	if (incremental) {
		QTextCursor c = edit->textCursor();
		c.setPosition(c.selectionStart());
		edit->setTextCursor(c);
	}

	QTextDocument::FindFlags flags;
	if (!forward)
		flags |= QTextDocument::FindBackward;

	if (!edit->find(term, flags)) {
		// Wrap around: retry from the far end.
		QTextCursor c = edit->textCursor();
		c.movePosition(forward ? QTextCursor::Start : QTextCursor::End);
		edit->setTextCursor(c);
		edit->find(term, flags);
	}
}

void MissionEventsDialog::on_btnFindNext_clicked()
{
	if (ui->eventViewStack->currentIndex() == GraphViewIndex) {
		ui->eventGraph->focusNextMatch(/*forward=*/true);
		return;
	}
	findInAdvancedText(/*forward=*/true, /*incremental=*/false);
}

void MissionEventsDialog::on_btnFindPrev_clicked()
{
	if (ui->eventViewStack->currentIndex() == GraphViewIndex) {
		ui->eventGraph->focusNextMatch(/*forward=*/false);
		return;
	}
	findInAdvancedText(/*forward=*/false, /*incremental=*/true);
}

void MissionEventsDialog::on_messageSearchEdit_textChanged(const QString& /*text*/)
{
	applyMessageFilter();

	// Deselect if the selected message got filtered out.
	const int row = ui->messageList->currentRow();
	if (row >= 0 && ui->messageList->item(row)->isHidden()) {
		ui->messageList->setCurrentRow(-1);
	}
}

SCP_vector<int> MissionEventsDialog::read_root_formula_order(sexp_tree_view* tree)
{
	SCP_vector<int> order;
	order.reserve(tree->topLevelItemCount());
	for (int i = 0; i < tree->topLevelItemCount(); ++i) {
		auto* it = tree->topLevelItem(i);
		order.push_back(it->data(0, sexp_tree_view::FormulaDataRole).toInt());
	}
	return order;
}

void MissionEventsDialog::updateEventBitmap() {
	updateEventBitmapAt(_model->getCurrentlySelectedEvent());
}

void MissionEventsDialog::updateEventBitmapAt(int eventIndex) {
	const auto& events = _model->getEventList();
	if (!SCP_vector_inbounds(events, eventIndex))
		return;
	const auto& event = events[eventIndex];

	const bool chained = (event.chain_delay >= 0);
	const bool hasObjectiveText = !event.objective_text.empty();

	NodeImage bitmap;
	if (chained) {
		if (!hasObjectiveText) {
			bitmap = NodeImage::CHAIN;
		} else {
			bitmap = NodeImage::CHAIN_DIRECTIVE;
		}
	} else {
		if (!hasObjectiveText) {
			bitmap = NodeImage::ROOT;
		} else {
			bitmap = NodeImage::ROOT_DIRECTIVE;
		}
	}
	for (int i = 0; i < ui->eventTree->topLevelItemCount(); ++i) {
		auto item = ui->eventTree->topLevelItem(i);

		if (item->data(0, sexp_tree_view::FormulaDataRole).toInt() == event.formula) {
			// Keep NodeImageRole in sync so a later theme change re-renders the correct icon.
			item->setData(0, sexp_tree_view::NodeImageRole, static_cast<int>(bitmap));
			item->setIcon(0, sexp_tree_view::convertNodeImageToIcon(bitmap));
			return;
		}
	}
}

void MissionEventsDialog::on_okAndCancelButtons_accepted()
{
	accept();
}

void MissionEventsDialog::on_okAndCancelButtons_rejected()
{
	reject();
}

void MissionEventsDialog::on_btnNewEvent_clicked()
{
	// Clear any active filter so the new event is visible.
	ui->eventSearchEdit->clear();

	const QByteArray before = _model->captureEventWorkingState();
	_suppressTreeUndo = true;
	_model->createEvent();
	_suppressTreeUndo = false;

	updateEventUi(); // rebuilds the graph via syncGraphAfterEventUi (event count grew)
	focusNewEvent();
	pushEventStateSnapshot(before, tr("Add Event"));
}

// The clicked New/Insert button would otherwise keep keyboard focus, so Space pressed it
// again (adding yet another event) instead of editing the new event's name.
void MissionEventsDialog::focusNewEvent()
{
	switch (ui->eventViewStack->currentIndex()) {
	case TreeViewIndex:
		if (auto* cur = ui->eventTree->currentItem())
			ui->eventTree->scrollToItem(cur);
		ui->eventTree->setFocus(Qt::OtherFocusReason);
		break;
	case GraphViewIndex:
		// Pan/zoom to the new event's node and select it
		ui->eventGraph->focusEvent(_model->getCurrentlySelectedEvent());
		ui->eventGraph->setFocus(Qt::OtherFocusReason);
		break;
	default:
		break;
	}
}

void MissionEventsDialog::on_btnInsertEvent_clicked()
{
	// Clear any active filter so the new event is visible.
	ui->eventSearchEdit->clear();

	const QByteArray before = _model->captureEventWorkingState();
	_suppressTreeUndo = true;
	_model->insertEvent();
	_suppressTreeUndo = false;

	updateEventUi();
	focusNewEvent();
	pushEventStateSnapshot(before, tr("Insert Event"));
}

// Ctrl+Alt+Plus: like Insert Event, but below the selected event
void MissionEventsDialog::insertEventBelow()
{
	// Clear any active filter so the new event is visible.
	ui->eventSearchEdit->clear();

	const QByteArray before = _model->captureEventWorkingState();
	_suppressTreeUndo = true;
	_model->insertEventBelow();
	_suppressTreeUndo = false;

	updateEventUi();
	focusNewEvent();
	pushEventStateSnapshot(before, tr("Insert Event"));
}

void MissionEventsDialog::on_btnDeleteEvent_clicked()
{
	_model->deleteEvent();

	updateEventUi();
}

void MissionEventsDialog::on_eventMoveTopBtn_clicked()
{
	auto* cur = ui->eventTree->currentItem();
	if (!cur || cur->parent())
		return; // roots only
	const int idx = ui->eventTree->indexOfTopLevelItem(cur);
	if (idx <= 0)
		return; // already at top

	QTreeWidgetItem* dest = ui->eventTree->topLevelItem(0);
	ui->eventTree->move_root(cur, dest, /*insert_before=*/true); // visual move + modified()

	ui->eventTree->setCurrentItem(cur);
	ui->eventTree->scrollToItem(cur);
	updateEventMoveButtons();
}

void MissionEventsDialog::on_eventUpBtn_clicked()
{
	auto* cur = ui->eventTree->currentItem();
	if (!cur || cur->parent())
		return; // roots only
	const int idx = ui->eventTree->indexOfTopLevelItem(cur);
	if (idx <= 0)
		return; // already at top

	QTreeWidgetItem* dest = ui->eventTree->topLevelItem(idx - 1);
	ui->eventTree->move_root(cur, dest, /*insert_before=*/true); // visual move + modified()

	// Ensure it stays selected and visible
	ui->eventTree->setCurrentItem(cur);
	ui->eventTree->scrollToItem(cur);
	updateEventMoveButtons();
}

void MissionEventsDialog::on_eventDownBtn_clicked()
{
	auto* cur = ui->eventTree->currentItem();
	if (!cur || cur->parent())
		return; // roots only
	const int idx = ui->eventTree->indexOfTopLevelItem(cur);
	const int last = ui->eventTree->topLevelItemCount() - 1;
	if (idx < 0 || idx >= last)
		return; // already at bottom

	QTreeWidgetItem* dest = ui->eventTree->topLevelItem(idx + 1);
	ui->eventTree->move_root(cur, dest, /*insert_before=*/false); // visual move + modified()

	ui->eventTree->setCurrentItem(cur);
	ui->eventTree->scrollToItem(cur);
	updateEventMoveButtons();
}

void MissionEventsDialog::on_eventMoveBottomBtn_clicked()
{
	auto* cur = ui->eventTree->currentItem();
	if (!cur || cur->parent())
		return; // roots only
	const int idx = ui->eventTree->indexOfTopLevelItem(cur);
	const int last = ui->eventTree->topLevelItemCount() - 1;
	if (idx < 0 || idx >= last)
		return; // already at bottom

	QTreeWidgetItem* dest = ui->eventTree->topLevelItem(last);
	ui->eventTree->move_root(cur, dest, /*insert_before=*/false); // visual move + modified()

	ui->eventTree->setCurrentItem(cur);
	ui->eventTree->scrollToItem(cur);
	updateEventMoveButtons();
}

void MissionEventsDialog::on_repeatCountBox_valueChanged(int value)
{
	if (!_model->eventIsValid())
		return;

	const int index  = _model->getCurrentlySelectedEvent();
	const int before = _model->getRepeatCount();
	_model->setRepeatCount(value);
	const int after = _model->getRepeatCount();
	updateEventUi();
	if (before == after)
		return;

	auto* cmd = new FieldEditCommand<int>(
	    FieldId::Event_RepeatCount + index * FieldId::Event_FieldStride,
	    nullptr, tr("Change Repeat Count"), true);
	cmd->addEntry(before, after, [this, index](const int& v) { _model->setRepeatCountAt(index, v); refreshEventAfterFieldUndo(index); });
	_dialogStack->push(cmd);
}

void MissionEventsDialog::on_triggerCountBox_valueChanged(int value)
{
	if (!_model->eventIsValid())
		return;

	const int index  = _model->getCurrentlySelectedEvent();
	const int before = _model->getTriggerCount();
	_model->setTriggerCount(value);
	const int after = _model->getTriggerCount();
	updateEventUi();
	if (before == after)
		return;

	auto* cmd = new FieldEditCommand<int>(
	    FieldId::Event_TriggerCount + index * FieldId::Event_FieldStride,
	    nullptr, tr("Change Trigger Count"), true);
	cmd->addEntry(before, after, [this, index](const int& v) { _model->setTriggerCountAt(index, v); refreshEventAfterFieldUndo(index); });
	_dialogStack->push(cmd);
}

void MissionEventsDialog::on_intervalTimeBox_valueChanged(int value)
{
	if (!_model->eventIsValid())
		return;

	const int index  = _model->getCurrentlySelectedEvent();
	const int before = _model->getIntervalTime();
	_model->setIntervalTime(value);
	const int after = _model->getIntervalTime();
	if (before == after)
		return;

	auto* cmd = new FieldEditCommand<int>(
	    FieldId::Event_Interval + index * FieldId::Event_FieldStride,
	    nullptr, tr("Change Interval Time"), true);
	cmd->addEntry(before, after, [this, index](const int& v) { _model->setIntervalTimeAt(index, v); refreshEventAfterFieldUndo(index); });
	_dialogStack->push(cmd);
}

void MissionEventsDialog::on_chainedCheckBox_toggled(bool checked)
{
	if (!_model->eventIsValid())
		return;

	// Track chain_delay itself rather than a bool so undoing an uncheck
	// restores the delay the event had, not a reset-to-zero one.
	const int index  = _model->getCurrentlySelectedEvent();
	const int before = _model->getChainDelay();
	const int after  = checked ? 0 : -1;
	if (before == after)
		return;

	_model->setChained(checked);
	updateEventBitmap();
	updateEventUi();

	auto* cmd = new FieldEditCommand<int>(
	    FieldId::Event_Chained + index * FieldId::Event_FieldStride,
	    nullptr, tr("Toggle Event Chained"), true);
	cmd->addEntry(before, after, [this, index](const int& v) {
		_model->setChainDelayRawAt(index, v);
		updateEventBitmapAt(index);
		refreshEventAfterFieldUndo(index);
	});
	_dialogStack->push(cmd);
}

void MissionEventsDialog::on_chainDelayBox_valueChanged(int value)
{
	if (!_model->eventIsValid())
		return;

	const int index  = _model->getCurrentlySelectedEvent();
	const int before = _model->getChainDelay();
	_model->setChainDelay(value);
	const int after = _model->getChainDelay();
	if (before == after)
		return;

	auto* cmd = new FieldEditCommand<int>(
	    FieldId::Event_ChainDelay + index * FieldId::Event_FieldStride,
	    nullptr, tr("Change Chain Delay"), true);
	cmd->addEntry(before, after, [this, index](const int& v) { _model->setChainDelayAt(index, v); refreshEventAfterFieldUndo(index); });
	_dialogStack->push(cmd);
}

void MissionEventsDialog::on_useMsecsCheckBox_toggled(bool checked)
{
	if (!_model->eventIsValid())
		return;

	const int index   = _model->getCurrentlySelectedEvent();
	const bool before = _model->getUseMsecs();
	if (before == checked)
		return;

	_model->setUseMsecs(checked);

	auto* cmd = new FieldEditCommand<bool>(
	    FieldId::Event_UseMsecs + index * FieldId::Event_FieldStride,
	    nullptr, tr("Toggle Interval In Milliseconds"), true);
	cmd->addEntry(before, checked, [this, index](const bool& v) { _model->setUseMsecsAt(index, v); refreshEventAfterFieldUndo(index); });
	_dialogStack->push(cmd);
}

void MissionEventsDialog::on_scoreBox_valueChanged(int value)
{
	if (!_model->eventIsValid())
		return;

	const int index  = _model->getCurrentlySelectedEvent();
	const int before = _model->getEventScore();
	_model->setEventScore(value);
	const int after = _model->getEventScore();
	if (before == after)
		return;

	auto* cmd = new FieldEditCommand<int>(
	    FieldId::Event_Score + index * FieldId::Event_FieldStride,
	    nullptr, tr("Change Event Score"), true);
	cmd->addEntry(before, after, [this, index](const int& v) { _model->setEventScoreAt(index, v); refreshEventAfterFieldUndo(index); });
	_dialogStack->push(cmd);
}

void MissionEventsDialog::on_teamCombo_currentIndexChanged(int index)
{
	if (!_model->eventIsValid())
		return;

	const int eventIndex = _model->getCurrentlySelectedEvent();
	const int before     = _model->getEventTeam();
	_model->setEventTeam(ui->teamCombo->itemData(index).toInt());
	const int after = _model->getEventTeam();
	if (before == after)
		return;

	auto* cmd = new FieldEditCommand<int>(
	    FieldId::Event_Team + eventIndex * FieldId::Event_FieldStride,
	    nullptr, tr("Change Event Team"), true);
	cmd->addEntry(before, after, [this, eventIndex](const int& v) { _model->setEventTeamAt(eventIndex, v); refreshEventAfterFieldUndo(eventIndex); });
	_dialogStack->push(cmd);
}

void MissionEventsDialog::on_editDirectiveText_textChanged(const QString& text)
{
	if (!_model->eventIsValid())
		return;

	const int index         = _model->getCurrentlySelectedEvent();
	const SCP_string before = _model->getEventDirectiveText();
	_model->setEventDirectiveText(text.toUtf8().constData());
	// The setter localizes the text, so read the stored value back.
	const SCP_string after = _model->getEventDirectiveText();
	updateEventBitmap();
	// Refresh the graph card's directive icon without re-populating the fields
	// (updateEventUi would reset this text edit's cursor mid-type).
	syncGraphAfterEventUi();
	if (before == after)
		return;

	auto* cmd = new FieldEditCommand<SCP_string>(
	    FieldId::Event_DirectiveText + index * FieldId::Event_FieldStride,
	    nullptr, tr("Change Directive Text"), true);
	cmd->addEntry(before, after, [this, index](const SCP_string& v) {
		_model->setEventDirectiveTextAt(index, v);
		updateEventBitmapAt(index);
		refreshEventAfterFieldUndo(index);
	});
	_dialogStack->push(cmd);
}

void MissionEventsDialog::on_editDirectiveKeypressText_textChanged(const QString& text)
{
	if (!_model->eventIsValid())
		return;

	const int index         = _model->getCurrentlySelectedEvent();
	const SCP_string before = _model->getEventDirectiveKeyText();
	_model->setEventDirectiveKeyText(text.toUtf8().constData());
	const SCP_string after = _model->getEventDirectiveKeyText();
	if (before == after)
		return;

	auto* cmd = new FieldEditCommand<SCP_string>(
	    FieldId::Event_DirectiveKey + index * FieldId::Event_FieldStride,
	    nullptr, tr("Change Directive Keypress Text"), true);
	cmd->addEntry(before, after, [this, index](const SCP_string& v) {
		_model->setEventDirectiveKeyTextAt(index, v);
		refreshEventAfterFieldUndo(index);
	});
	_dialogStack->push(cmd);
}

void MissionEventsDialog::pushEventLogFlagCommand(int fieldConst, int mask, bool checked)
{
	if (!_model->eventIsValid())
		return;

	const int index   = _model->getCurrentlySelectedEvent();
	const bool before = (_model->getEventList()[index].mission_log_flags & mask) != 0;
	if (before == checked)
		return;

	_model->setEventLogFlagAt(index, mask, checked);
	updateEventUi(); // refresh dependent UI (and the graph's log icon) like the other field handlers

	auto* cmd = new FieldEditCommand<bool>(
	    fieldConst + index * FieldId::Event_FieldStride,
	    nullptr, tr("Toggle Event Log Flag"), true);
	cmd->addEntry(before, checked, [this, index, mask](const bool& v) { _model->setEventLogFlagAt(index, mask, v); refreshEventAfterFieldUndo(index); });
	_dialogStack->push(cmd);
}

void MissionEventsDialog::on_checkLogTrue_toggled(bool checked)
{
	pushEventLogFlagCommand(FieldId::Event_LogTrue, MLF_SEXP_TRUE, checked);
}

void MissionEventsDialog::on_checkLogFalse_toggled(bool checked)
{
	pushEventLogFlagCommand(FieldId::Event_LogFalse, MLF_SEXP_FALSE, checked);
}

void MissionEventsDialog::on_checkLogPrevious_toggled(bool checked)
{
	pushEventLogFlagCommand(FieldId::Event_LogPrevious, MLF_STATE_CHANGE, checked);
}

void MissionEventsDialog::on_checkLogAlwaysFalse_toggled(bool checked)
{
	pushEventLogFlagCommand(FieldId::Event_LogAlwaysFalse, MLF_SEXP_KNOWN_FALSE, checked);
}

void MissionEventsDialog::on_checkLogFirstRepeat_toggled(bool checked)
{
	pushEventLogFlagCommand(FieldId::Event_LogFirstRepeat, MLF_FIRST_REPEAT_ONLY, checked);
}

void MissionEventsDialog::on_checkLogLastRepeat_toggled(bool checked)
{
	pushEventLogFlagCommand(FieldId::Event_LogLastRepeat, MLF_LAST_REPEAT_ONLY, checked);
}

void MissionEventsDialog::on_checkLogFirstTrigger_toggled(bool checked)
{
	pushEventLogFlagCommand(FieldId::Event_LogFirstTrigger, MLF_FIRST_TRIGGER_ONLY, checked);
}

void MissionEventsDialog::on_checkLogLastTrigger_toggled(bool checked)
{
	pushEventLogFlagCommand(FieldId::Event_LogLastTrigger, MLF_LAST_TRIGGER_ONLY, checked);
}

void MissionEventsDialog::on_messageList_currentRowChanged(int row)
{
	_model->setCurrentlySelectedMessage(row);
	updateMessageUi();
}

void MissionEventsDialog::on_messageList_itemDoubleClicked(QListWidgetItem* item)
{
	if (!item || !ui->eventTree)
		return;

	// The jump targets the tree, so bring the tree view forward first. The
	// switch can be vetoed by the active view.
	if (ui->eventViewStack->currentIndex() != TreeViewIndex) {
		requestViewChange(TreeViewIndex);
		if (ui->eventViewStack->currentIndex() != TreeViewIndex)
			return;
	}

	// This jumps to the message's use in the event tree, so drop any event
	// filter that might be hiding the target event.
	ui->eventSearchEdit->clear();

	const QString name = item->text();
	if (name != m_last_message_name) {
		m_last_message_name = name;
		m_last_message_node = -1; // reset cycle when switching message
	}

	int nodes[MAX_SEARCH_MESSAGE_DEPTH];
	const int num = ui->eventTree->_model.find_text(name.toUtf8().constData(), nodes, MAX_SEARCH_MESSAGE_DEPTH);
	if (num <= 0) {
		QMessageBox::information(this, tr("Error"), tr("No events using message '%1'").arg(name));
		return;
	}

	// cycle to next
	int next = nodes[0];
	if (m_last_message_node != -1) {
		int pos = -1;
		for (int i = 0; i < num; ++i) {
			if (nodes[i] == m_last_message_node) {
				pos = i;
				break;
			}
		}
		next = (pos == -1 || pos == num - 1) ? nodes[0] : nodes[pos + 1];
	}

	m_last_message_node = next;
	ui->eventTree->hilite_item(next);
}

void MissionEventsDialog::on_btnNewMsg_clicked()
{
	// Clear any active filter so the new message is visible.
	ui->messageSearchEdit->clear();

	const QByteArray before = _model->captureMessageWorkingState();
	_model->createMessage();

	rebuildMessageList();
	updateMessageUi();
	pushMessageStateSnapshot(before, tr("Add Message"));

	// Let the user name the new message right away: focus the name field and
	// select its placeholder text so typing immediately replaces it.
	ui->messageName->setFocus();
	ui->messageName->selectAll();
}

void MissionEventsDialog::on_btnInsertMsg_clicked()
{
	// Clear any active filter so the new message is visible.
	ui->messageSearchEdit->clear();

	const QByteArray before = _model->captureMessageWorkingState();
	_model->insertMessage();

	rebuildMessageList();

	// Keep selection/visibility in sync
	const int sel = _model->getCurrentlySelectedMessage();
	if (auto* w = ui->messageList) {
		w->setCurrentRow(sel);
		if (auto* it = w->item(sel))
			w->scrollToItem(it);
	}
	updateMessageUi();
	pushMessageStateSnapshot(before, tr("Insert Message"));

	// Let the user name the new message right away: focus the name field and
	// select its placeholder text so typing immediately replaces it.
	ui->messageName->setFocus();
	ui->messageName->selectAll();
}

void MissionEventsDialog::on_btnDeleteMsg_clicked()
{
	const QByteArray before = _model->captureMessageWorkingState();
	_model->deleteMessage();

	rebuildMessageList();
	updateMessageUi();
	pushMessageStateSnapshot(before, tr("Delete Message"));
}

void MissionEventsDialog::on_msgMoveTopBtn_clicked()
{
	const QByteArray before = _model->captureMessageWorkingState();
	_model->moveMessageToTop();
	rebuildMessageList();
	const int sel = _model->getCurrentlySelectedMessage();
	if (auto* w = ui->messageList) {
		w->setCurrentRow(sel);
		if (auto* it = w->item(sel))
			w->scrollToItem(it);
	}
	updateMessageUi();
	pushMessageStateSnapshot(before, tr("Move Message"));
}

void MissionEventsDialog::on_msgUpBtn_clicked()
{
	const QByteArray before = _model->captureMessageWorkingState();
	_model->moveMessageUp();
	rebuildMessageList();
	const int sel = _model->getCurrentlySelectedMessage();
	if (auto* w = ui->messageList) {
		w->setCurrentRow(sel);
		if (auto* it = w->item(sel))
			w->scrollToItem(it);
	}
	updateMessageUi();
	pushMessageStateSnapshot(before, tr("Move Message"));
}

void MissionEventsDialog::on_msgDownBtn_clicked()
{
	const QByteArray before = _model->captureMessageWorkingState();
	_model->moveMessageDown();
	rebuildMessageList();
	const int sel = _model->getCurrentlySelectedMessage();
	if (auto* w = ui->messageList) {
		w->setCurrentRow(sel);
		if (auto* it = w->item(sel))
			w->scrollToItem(it);
	}
	updateMessageUi();
	pushMessageStateSnapshot(before, tr("Move Message"));
}

void MissionEventsDialog::on_msgMoveBottomBtn_clicked()
{
	const QByteArray before = _model->captureMessageWorkingState();
	_model->moveMessageToBottom();
	rebuildMessageList();
	const int sel = _model->getCurrentlySelectedMessage();
	if (auto* w = ui->messageList) {
		w->setCurrentRow(sel);
		if (auto* it = w->item(sel))
			w->scrollToItem(it);
	}
	updateMessageUi();
	pushMessageStateSnapshot(before, tr("Move Message"));
}

void MissionEventsDialog::on_messageName_textChanged(const QString& text)
{
	if (!_model->messageIsValid())
		return;

	// A clashing name is only marked while typing; a name on the way to a valid one (such as
	// "Help" on the way to "Helprin", Help being a builtin) mustn't interrupt. The model keeps
	// the last valid name until the text stops clashing; leaving the field with a clashing
	// name warns and reverts (onMessageNameEditingFinished).
	const SCP_string name = text.toUtf8().constData();
	const SCP_string conflict = _model->messageNameConflict(name);
	showMessageNameConflict(conflict);
	if (!conflict.empty())
		return;

	const int index         = _model->getCurrentlySelectedMessage();
	const SCP_string before = _model->getMessageName();
	_model->setMessageName(name);
	const SCP_string after = _model->getMessageName();
	if (before == after)
		return;

	rebuildMessageList();

	auto* cmd = new FieldEditCommand<SCP_string>(
	    FieldId::Msg_Name + index * FieldId::Msg_FieldStride,
	    nullptr, tr("Change Message Name"), true);
	cmd->addEntry(before, after, [this, index](const SCP_string& v) {
		_model->setMessageNameAt(index, v);
		rebuildMessageList();
		updateMessageUi();
	});
	_dialogStack->push(cmd);
}

// Leaving the name field (Enter or focus loss) with a name that still clashes: warn once and
// put back the last valid name, which the model still holds.
void MissionEventsDialog::onMessageNameEditingFinished()
{
	if (!_model->messageIsValid())
		return;

	const SCP_string conflict = _model->messageNameConflict(ui->messageName->text().toUtf8().constData());
	if (conflict.empty())
		return;

	// Revert before the dialog: the warning takes focus, which ends editing again
	{
		QSignalBlocker blocker(ui->messageName);
		ui->messageName->setText(QString::fromStdString(_model->getMessageName()));
	}
	showMessageNameConflict({});

	QMessageBox::warning(this, tr("Invalid Message Name"),
		QString::fromStdString(conflict) + QLatin1Char('\n') + tr("The previous name has been kept."));
}

// Marks the message name field while its text can't be used (reason non-empty), or clears it
void MissionEventsDialog::showMessageNameConflict(const SCP_string& reason)
{
	if (reason.empty()) {
		ui->messageName->setStyleSheet(QString());
		ui->messageName->setToolTip(QString());
	} else {
		ui->messageName->setStyleSheet(QStringLiteral("QLineEdit { border: 2px solid #d9534f; }"));
		ui->messageName->setToolTip(QString::fromStdString(reason));
	}
}

void MissionEventsDialog::on_messageContent_textChanged()
{
	if (!_model->messageIsValid())
		return;

	const int index         = _model->getCurrentlySelectedMessage();
	const SCP_string before = _model->getMessageText();
	_model->setMessageText(ui->messageContent->toPlainText().toUtf8().constData());
	// The setter truncates and localizes, so read the stored value back.
	const SCP_string after = _model->getMessageText();
	if (before == after)
		return;

	auto* cmd = new FieldEditCommand<SCP_string>(
	    FieldId::Msg_Text + index * FieldId::Msg_FieldStride,
	    nullptr, tr("Change Message Text"), true);
	cmd->addEntry(before, after, [this, index](const SCP_string& v) {
		_model->setMessageTextAt(index, v);
		updateMessageUi();
	});
	_dialogStack->push(cmd);
}

void MissionEventsDialog::on_btnMsgNote_clicked()
{
	if (!_model->messageIsValid())
		return;

	const int index         = _model->getCurrentlySelectedMessage();
	const SCP_string before = _model->getMessageNote();
	SCP_string note         = before;
	if (!util::editNote(this, tr("Message Note"), tr("Enter a note for this message:"), note))
		return;

	_model->setMessageNote(note);
	ui->btnMsgNote->setText(util::noteButtonText(note));

	if (before == note)
		return;

	auto* cmd = new FieldEditCommand<SCP_string>(
	    FieldId::Msg_Note + index * FieldId::Msg_FieldStride,
	    nullptr, tr("Edit Message Note"), true);
	cmd->setNoMerge(); // each note dialog visit is a discrete action
	cmd->addEntry(before, note, [this, index](const SCP_string& v) {
		_model->setMessageNoteAt(index, v);
		updateMessageUi();
	});
	_dialogStack->push(cmd);
}

// Shared by typing (editingFinished), dropdown selection, and the browse
// picker: applies the ani, refreshes the combo list, and pushes one merging
// command per message.
void MissionEventsDialog::changeMessageAni(const SCP_string& name)
{
	if (!_model->messageIsValid())
		return;

	const int index         = _model->getCurrentlySelectedMessage();
	const SCP_string before = _model->getMessageAni();
	_model->setMessageAni(name);
	// The setter normalizes "<None>"/empty, so read the stored value back.
	const SCP_string after = _model->getMessageAni();

	initHeadCombo();
	ui->aniCombo->setCurrentText(QString::fromStdString(after));

	if (before == after)
		return;

	auto* cmd = new FieldEditCommand<SCP_string>(
	    FieldId::Msg_Ani + index * FieldId::Msg_FieldStride,
	    nullptr, tr("Change Message Animation"), true);
	cmd->addEntry(before, after, [this, index](const SCP_string& v) {
		_model->setMessageAniAt(index, v);
		initHeadCombo();
		updateMessageUi();
	});
	_dialogStack->push(cmd);
}

// Wave changes are message-scope snapshots rather than field commands: the
// setter can auto-select a persona and head ani as a side effect, and undo
// must restore those too.
void MissionEventsDialog::changeMessageWave(const SCP_string& name)
{
	if (!_model->messageIsValid())
		return;

	const int index         = _model->getCurrentlySelectedMessage();
	const QByteArray before = _model->captureMessageWorkingState();
	_model->setMessageWave(name);

	initWaveFilenames();
	ui->waveCombo->setCurrentText(QString::fromStdString(_model->getMessageWave()));
	updateMessageUi(); // the persona and head ani may have changed

	pushMessageStateSnapshot(before, tr("Change Wave File"), FieldId::Msg_SnapWave + index);
}

void MissionEventsDialog::on_aniCombo_editingFinished()
{
	changeMessageAni(ui->aniCombo->currentText().toUtf8().constData());
}

void MissionEventsDialog::on_aniCombo_currentIndexChanged(int index)
{
	if (index < 0)
		return;
	changeMessageAni(ui->aniCombo->itemText(index).toUtf8().constData());
}

void MissionEventsDialog::on_btnAniBrowse_clicked()
{
	HeadAnimationPickerDialog dlg(this);

	QStringList headNames;
	for (const auto& head : _model->getHeadAniList()) {
		headNames << QString::fromStdString(head);
	}
	dlg.setHeadAnimationNames(headNames);
	dlg.setInitialSelection(QString::fromStdString(_model->getMessageAni()));

	if (dlg.exec() != QDialog::Accepted) {
		return;
	}

	const auto selected = dlg.selectedFile();
	const SCP_string selectedStd = selected.toUtf8().constData();

	// Permanently add the picked name to the session gallery so it appears in
	// the picker whenever this mission editor is open
	MissionEventsDialogModel::addExtraHeadAni(selectedStd);

	changeMessageAni(selectedStd);
}

void MissionEventsDialog::on_waveCombo_editingFinished()
{
	changeMessageWave(ui->waveCombo->currentText().toUtf8().constData());
}

void MissionEventsDialog::on_waveCombo_currentIndexChanged(int index)
{
	if (index < 0)
		return;
	changeMessageWave(ui->waveCombo->itemText(index).toUtf8().constData());
}

void MissionEventsDialog::on_btnBrowseWave_clicked()
{
	if (!_model->messageIsValid()) {
		return;
	}

	const int voiceCfType = (The_mission.game_type & MISSION_TYPE_TRAINING)
		? CF_TYPE_VOICE_TRAINING : CF_TYPE_VOICE_SPECIAL;

	const QString lastDir = util::getLastDir("missionEvents/waveFile", voiceCfType);

	const auto name = QFileDialog::getOpenFileName(this,
		tr("Select message animation"),
		lastDir,
		"Voice Files (*.ogg *.wav);;Ogg Vorbis Files (*.ogg);;Wave Files (*.wav);;All Files (*)");

	if (name.isEmpty()) {
		// Nothing was selected
		return;
	}

	QFileInfo info(name);
	util::saveLastDir("missionEvents/waveFile", name);

	SCP_string file_name = info.fileName().toUtf8().constData();

	changeMessageWave(file_name);
}

void MissionEventsDialog::on_btnWavePlay_clicked()
{
	_model->playMessageWave();
}

void MissionEventsDialog::on_personaCombo_currentIndexChanged(int index)
{
	if (!_model->messageIsValid())
		return;

	const int msgIndex = _model->getCurrentlySelectedMessage();
	const int before   = _model->getMessagePersona();
	_model->setMessagePersona(ui->personaCombo->itemData(index).toInt());
	const int after = _model->getMessagePersona();
	if (before == after)
		return;

	auto* cmd = new FieldEditCommand<int>(
	    FieldId::Msg_Persona + msgIndex * FieldId::Msg_FieldStride,
	    nullptr, tr("Change Message Persona"), true);
	cmd->addEntry(before, after, [this, msgIndex](const int& v) {
		_model->setMessagePersonaAt(msgIndex, v);
		updateMessageUi();
	});
	_dialogStack->push(cmd);
}

void MissionEventsDialog::on_btnUpdateStuff_clicked()
{
	if (!_model->messageIsValid())
		return;

	auto result = _viewport->dialogProvider->showButtonDialog(
		DialogType::Question,
		"Update Message Stuff",
		"This will update the message animation and persona to match the current mission settings. "
		   "Are you sure you want to do this?",
		{DialogButton::Yes, DialogButton::No});

	if (result == DialogButton::Yes) {
		const QByteArray before = _model->captureMessageWorkingState();
		_model->autoSelectPersona();
		updateMessageUi();
		pushMessageStateSnapshot(before, tr("Update Message Persona"));
	}
}

void MissionEventsDialog::on_messageTeamCombo_currentIndexChanged(int index)
{
	if (!_model->messageIsValid())
		return;

	const int msgIndex = _model->getCurrentlySelectedMessage();
	const int before   = _model->getMessageTeam();
	_model->setMessageTeam(ui->messageTeamCombo->itemData(index).toInt());
	const int after = _model->getMessageTeam();
	if (before == after)
		return;

	auto* cmd = new FieldEditCommand<int>(
	    FieldId::Msg_Team + msgIndex * FieldId::Msg_FieldStride,
	    nullptr, tr("Change Message Team"), true);
	cmd->addEntry(before, after, [this, msgIndex](const int& v) {
		_model->setMessageTeamAt(msgIndex, v);
		updateMessageUi();
	});
	_dialogStack->push(cmd);
}

} // namespace fso::fred::dialogs
