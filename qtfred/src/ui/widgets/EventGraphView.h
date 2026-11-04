#pragma once

// Events editor graph view -- a visualizer over the working sexps, with Radial
// (one object and every event that references it), Swimlanes, and the editable
// Basic (dataflow) modes. Modeled on CampaignMissionGraph. It never changes mission
// data itself: edits go out as signals, and the dialog applies them with undo.

#include "mission/dialogs/EventReferenceIndex.h"

#include <QGraphicsView>
#include <QVector>
#include <QString>
#include <QColor>
#include <QHash>
#include <QSet>
#include <QTransform>
#include <QPointF>
#include <QPair>

class QGraphicsScene;
class QComboBox;
class QButtonGroup;
class QWidget;
class QMenu;

namespace fso::fred {

namespace graphdetail {
class CardItem;
class ObjectNodeItem;
class EventNodeItem;
class SexpNodeItem;
class RefEdgeItem;
class MinimapWidget;
class LegendWidget;
} // namespace graphdetail

// Theme-adaptive visual constants shared by the view and its items. Card fills
// are subtle whole-card tints (from the design wireframe), with both light and
// dark values set by makeStyle().
struct EventGraphStyle {
	QColor bgColor{251, 249, 244};
	QColor gridMinor{231, 226, 212};

	// Card fills by role/kind.
	QColor objectFill{242, 239, 230};
	QColor eventFill{255, 246, 232};   // cream
	QColor condFill{233, 242, 255};    // light blue
	QColor actionFill{247, 236, 246};  // light pink
	QColor cardBorder{207, 201, 187};

	QColor nodeText{26, 26, 26};
	QColor nodeSubText{74, 74, 74};

	// Chip (small kind/role label) colors.
	QColor condChip{47, 111, 179};
	QColor actionChip{124, 74, 168};
	QColor eventChip{120, 120, 120};

	// Event badge (the small event-name box on cond/action cards).
	QColor eventBadge{150, 120, 70};

	// Object-kind accent colors (chip on the center object card).
	QColor entity{217, 131, 42};    // "objects": ship / wing / waypoint path / jump node / prop / coord point
	QColor message{198, 54, 47};    // red
	QColor dataColor{179, 74, 124}; // "data": any other OPF data kind (goal, team, event name, ...)
	QColor variable{138, 106, 42};
	QColor container{38, 145, 130};  // teal, distinct from the variable brown
	QColor eventAccent{90, 90, 90};
	QColor chainColor{70, 150, 100}; // event-chain connection line (green, distinct from ref edges)

	// Tier ring guide (dashed circles).
	QColor ringColor{170, 165, 152};

	qreal edgeWidth{1.8};
	qreal nodeRadius{7.0};

	static EventGraphStyle makeStyle(bool dark);
	QColor colorFor(RefObjectKind kind) const;
};

// A pickable object in the radial selector.
struct GraphObject {
	RefObjectKind kind = RefObjectKind::Unknown;
	QString       name;
};

// Per-event display flags (presence only) shown as icons on the event card, plus
// the annotation key for the event's own comment/color. Fed by the dialog.
struct GraphEventMeta {
	bool chained = false;   // chain_delay >= 0 (chained to the previous event)
	bool directive = false; // objective_text non-empty
	bool repeats = false;   // repeat_count != 1 or using a trigger count
	bool logging = false;   // any mission_log_flags set
	int  annotationKey = -1; // rootKey(formula) for this event's annotation
};

// A node's comment/color annotation, resolved by the dialog and looked up by the
// annotation key (tree_nodes index for nodes, rootKey for events).
struct GraphAnnotation {
	QString comment;
	QColor  color; // invalid QColor when the node has no color set
};

class EventGraphView final : public QGraphicsView {
	Q_OBJECT
  public:
	explicit EventGraphView(QWidget* parent = nullptr);
	~EventGraphView() override;

	bool eventFilter(QObject* watched, QEvent* event) override;

	// Data feeds (owned by the dialog). Call reload() after any of these change.
	void setReferenceIndex(const EventReferenceIndex* index) { m_index = index; }
	void setEventNames(QVector<QString> names) { m_eventNames = std::move(names); }
	void setObjects(QVector<GraphObject> objects);
	// Per-event status flags (indexed by event) and the node comment/color
	// snapshot (keyed by annotation key). Both fed on every refresh.
	void setEventMeta(QVector<GraphEventMeta> meta) { m_eventMeta = std::move(meta); }
	void setAnnotations(QHash<int, GraphAnnotation> annotations) { m_annotations = std::move(annotations); }
	// 1-based order of each sexp node among its siblings (tree_nodes index -> order),
	// shown as a corner marker on the card. Fed on every refresh.
	void setSiblingOrders(QHash<int, int> orders) { m_siblingOrders = std::move(orders); }
	// Events whose subtree is collapsed to just the event card (Basic view). Fed on
	// every refresh from the saved collapse annotations.
	void setCollapsedEvents(QSet<int> events) { m_collapsedEvents = std::move(events); }
	// Lightweight refresh of one event card's status icons + chain lines after a
	// property edit, without a full rebuild.
	void updateEventCard(int eventIndex, GraphEventMeta meta);
	// True when a currently-selected node is an event card (gates the property
	// controls in the dialog).
	bool isEventNodeSelected() const;
	// The event index of the selected event card, or -1 if none is selected.
	int selectedEventIndex() const;
	// How many different events have a selected card (box select can pick several).
	int selectedEventCount() const;
	// The card with this annotation key has collapsed bullets to reveal (gates the
	// graph's "Expand Card" menu item); expandNode reveals them.
	bool nodeExpandable(int key) const;
	void expandNode(int key);
	// True when the Basic sub-mode is active (gates "New Event" in the dialog).
	bool isBasicMode() const { return m_mode == Mode::Basic; }
	// Pan/zoom to an event's card and select it (used after creating a new event).
	void focusEvent(int eventIndex);
	// The whole-mission dataflow graph for the Basic view, with node positions
	// already resolved from saved annotations by the dialog.
	void setBasicGraph(BasicGraph graph) { m_basicGraph = std::move(graph); }

	// Highlight nodes whose text matches the query (dimming the rest), across all
	// modes. Empty query clears the highlight. Driven by the shared search box.
	void setSearchText(const QString& text);
	// Step to the next/previous matching node (reading order) and pan+zoom to it.
	void focusNextMatch(bool forward);
	// True while a non-empty search is active (for enabling the next/prev arrows).
	bool hasSearch() const { return !m_searchText.isEmpty(); }

	// Repopulate the selector (preserving the current object where possible) and
	// rebuild the current view's scene from the current data.
	void reload();

	void zoomToFitAll(qreal margin = 40.0);
	// Zoom one step in/out, centered on the current view center (used by the
	// minimap wheel, which is away from the content).
	void zoomStep(bool zoomIn);

  signals:
	// Single-click (select) an event or node card -> sync-select that event
	// elsewhere (e.g. the tree view) without switching views.
	void eventSelected(int eventIndex);
	// Double-click an event card -> request a jump to that event in the tree view.
	void eventActivated(int eventIndex);
	// Double-click a condition/action node card -> jump to that specific tree node.
	void nodeActivated(int treeNode);
	// Basic view: one or more cards were dragged. Each entry is (annotation key,
	// new scene position). A group drag reports the whole selection so it persists
	// as a single undo step.
	void nodesMoved(const QVector<QPair<int, QPointF>>& moves);
	// Any node selection change (including to an object or empty). The dialog uses
	// it to re-evaluate whether the event-property controls should be live.
	void graphSelectionChanged();
	// The Basic/Radial/Swimlanes sub-mode changed (the dialog re-evaluates which
	// event-management buttons apply).
	void modeChanged();
	// Right-click on an editable card (event / operator node / single-ref object).
	// `key` is the node's annotation key; the dialog opens the tree's context menu
	// at that node. Aggregate cards (radial center, swimlanes rows, combined
	// objects) carry no key and don't emit this.
	void nodeContextMenuRequested(int key, const QPoint& globalPos);
	// Double-click on an inline literal-arg bullet: open the editor for that arg node
	// (a number opens the quick-search; a string, the text dialog).
	void nodeEditRequested(int treeNode, const QPoint& globalPos);
	// The user clicked an event card's collapse box (Basic view).
	void eventCollapseToggled(int eventIndex);

  protected:
	void wheelEvent(QWheelEvent* e) override;
	void drawBackground(QPainter* painter, const QRectF& rect) override;
	void mousePressEvent(QMouseEvent* e) override;
	void mouseMoveEvent(QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	void mouseDoubleClickEvent(QMouseEvent* e) override;
	void contextMenuEvent(QContextMenuEvent* e) override;
	void keyPressEvent(QKeyEvent* e) override;
	void resizeEvent(QResizeEvent* e) override;
	void showEvent(QShowEvent* e) override;
	void scrollContentsBy(int dx, int dy) override;

  private:
	enum class Mode { Radial, Swimlanes, Basic };

	// Remembered view state per graph mode, so switching Basic/Radial/Swimlanes
	// and back restores that mode's zoom/center, selection, and object/filter
	// settings instead of resetting. Indexed by static_cast<int>(Mode).
	struct GraphModeMemory {
		bool valid = false;
		QTransform transform;
		QPointF center;
		// Selection identity (re-applied to the fresh scene after a rebuild).
		int selKind = 0; // 0 none, 1 event, 2 node
		int selEvent = -1;
		QString selOp; // node title (kind 2)
		bool selCond = false;
		// Per-mode subject/filters.
		QString radialObjectKey; // Radial: "kind|name" of the selected object
		RefObjectKind swimKind = RefObjectKind::Unknown; // Swimlanes kind filter
		QSet<QString> focusObjects;                      // Swimlanes cross-filter
		QSet<int> focusEvents;
	};
	GraphModeMemory m_modeMemory[3];
	// Snapshot the current mode's state; restore a mode's remembered selector row
	// (returns false when the saved subject is gone) and its saved selection.
	void saveModeMemory();
	bool restoreSelectorSelection(Mode mode, const GraphModeMemory& mem);
	void applySavedSelection(const GraphModeMemory& mem);

	void buildOverlay();
	void rebuildSettingsMenu();
	void exportImage(); // render the whole scene to a PNG the user picks
	void positionOverlay();
	void updateChromeVisibility();
	// Drag mode + pan cursor derived from the current mode. Called on construct
	// and on every mode change, so the default view is a one-line change.
	void applyModeViewport();
	// Set the viewport cursor: closed hand while panning, open hand while Shift is
	// held (pan-ready), otherwise the normal arrow.
	void updatePanCursor();
	// One-time re-fit once the viewport has a real size (the graph may have been
	// built while the dialog was still hidden, framing against a zero-size viewport).
	void maybeInitialFrame();
	void applyTheme(bool dark);
	void populateSelector();
	void populateKindFilter();
	void populateSelectorForMode();
	// Basic: a manual node drag flips the layout dropdown to Custom.
	void setBasicLayoutToCustom();
	void setMode(Mode mode);
	void rebuildCurrent();
	void rebuildRadial();
	void rebuildSwimlanes();
	void rebuildBasic();
	// Redraw the event-chain lines (Basic + Swimlanes) from the current event
	// cards + meta; used on rebuild and after a chain toggle.
	void refreshChainLines();
	// Shrink the scene rect back to the current content (+margin). The default
	// scene rect only grows, so without this a big swimlanes layout leaves radial
	// / basic pinned to a corner and out of sync with the minimap.
	void updateSceneRect();
	// Selection-driven emphasis: reference-line on-select visibility + focus fade.
	void applyEmphasis();
	// Apply a node's comment/color annotation (by key), and event status icons.
	void applyAnnotation(graphdetail::CardItem* card, int key);
	void applyEventStatus(graphdetail::EventNodeItem* card, int eventIndex);
	// Swimlanes cross-filter: focus an object row / event column (Ctrl adds).
	void toggleSwimObjectFocus(const QString& objectKey, bool add);
	void toggleSwimEventFocus(int eventIndex, bool add);
	void clearSwimFocus();
	void showEmptyMessage(const QString& text);
	int  selectedObjectRow() const;

	QGraphicsScene* m_scene = nullptr;
	const EventReferenceIndex* m_index = nullptr;
	QVector<QString> m_eventNames;
	QVector<GraphObject> m_objects;
	// The Radial object picked before setObjects() replaced the list, so
	// populateSelector() can find it again by identity (the combo stores indices).
	GraphObject m_selectorIdentity;
	bool m_hasSelectorIdentity = false;
	QVector<GraphEventMeta> m_eventMeta;      // per-event status flags (event icons)
	QHash<int, GraphAnnotation> m_annotations; // node comment/color by annotation key
	QHash<int, int> m_siblingOrders;           // tree node -> 1-based sibling order (corner marker)
	QSet<int> m_collapsedEvents;               // event indices collapsed in Basic view
	BasicGraph m_basicGraph;
	QString m_searchText; // graph search highlight query
	int m_matchIndex = -1; // current match for next/prev navigation

	// Basic view drag tracking: the card grabbed on press and its starting scene
	// position, so a real move (not a click) emits nodesMoved on release.
	graphdetail::CardItem* m_dragItem = nullptr;
	QPointF m_dragStartPos;
	// Basic view uses NoDrag (so cards drag individually), so panning on empty
	// canvas is handled manually.
	// Panning (all modes): middle / right / shift+left drag. m_panButton is the
	// button that started it; m_panMoved gates the right-click menu (a right-drag
	// that panned should not open the menu).
	bool          m_panning = false;
	bool          m_panMoved = false;
	Qt::MouseButton m_panButton = Qt::NoButton;
	QPoint        m_panLastPos;
	QPoint        m_panPressPos;

	EventGraphStyle m_style;
	Mode m_mode = Mode::Basic;

	// Top-left overlay controls.
	QWidget*      m_overlay = nullptr;
	QButtonGroup* m_modeGroup = nullptr;
	QComboBox*    m_objectCombo = nullptr;
	QMenu*        m_settingsMenu = nullptr;

	// Bottom-left minimap.
	graphdetail::MinimapWidget* m_minimap = nullptr;
	// Top-right color-key legend.
	graphdetail::LegendWidget* m_legend = nullptr;
	// Event card by event index (for lightweight per-card refresh + chain lines);
	// rebuilt each rebuild (pointers die with the cleared scene).
	QHash<int, graphdetail::EventNodeItem*> m_eventCards;

	qreal m_currentScale = 1.0;
	const qreal kMinScale = 0.2;
	const qreal kMaxScale = 3.0;

	// View continuity across rebuilds: keep zoom/center (and re-select) when the
	// same object is still shown, instead of re-fitting to the whole graph.
	QString m_framedKey;
	bool    m_hasFramed = false;
	// True once the content has been fit against a real (sized) viewport. Guards the
	// one-time re-fit when the graph was built before the dialog was shown.
	bool    m_framedWithSize = false;
	bool    m_suppressSelectionSignal = false;

	// Swimlanes filters. m_swimKind == Unknown means "all kinds".
	RefObjectKind m_swimKind = RefObjectKind::Unknown;
	QSet<QString> m_focusObjects; // object row keys
	QSet<int>     m_focusEvents;
	// Track an empty-space press so a click (not a pan-drag) on empty canvas
	// clears the swimlanes filter on release.
	bool   m_pressOnEmpty = false;
	QPoint m_pressPos;
};

} // namespace fso::fred
