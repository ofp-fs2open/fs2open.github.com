#include "EventGraphView.h"
#include "ui/Theme.h"
#include "ui/widgets/sexp_tree_view.h" // convertNodeImageToIcon for the event icons
#include "missioneditor/sexp_tree_model.h" // NodeImage

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsPathItem>
#include <QFontMetricsF>
#include <QHash>
#include <QHBoxLayout>
#include <QIcon>
#include <QImage>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPair>
#include <QPolygonF>
#include <QRadioButton>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTextOption>
#include <QShowEvent>
#include <QSignalBlocker>
#include <QSlider>
#include <QStringList>
#include <QToolButton>
#include <QTransform>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QWidgetAction>
#include <QtMath>

#include <algorithm>
#include <cmath>
#include <functional>
#include <utility>

namespace fso::fred {

// Distance between adjacent radial rings; a graph setting that persists for the
// session (across object and view changes), adjustable via the settings gear.
static qreal s_ringSpacing = 210.0;

// Basic-view node arrangement, remembered for the session like s_ringSpacing.
// Custom honors saved per-node positions; Auto/Compact are computed layouts that
// ignore them (non-destructive -- saved positions stay in the file, just unused
// while a computed preset is active). A manual drag switches back to Custom.
enum class BasicLayout { Custom = 0, Auto = 1, Compact = 2 };
static BasicLayout s_basicLayout = BasicLayout::Custom;

// Session-wide graph chrome settings (gear menu). Shared across modes except
// where noted.
enum class RefLineMode { Always = 0, OnSelect = 1, Off = 2 };
static RefLineMode s_refMode = RefLineMode::Always; // relationship/flow edge visibility
static bool s_focusFade = false;    // dim nodes/edges not connected to the selection
static bool s_showLegend = false;   // color-key panel
static bool s_showMinimap = true;   // bottom-left overview
static int  s_basicDepth = 8;       // Basic: max operator nesting depth to render
static constexpr int kBasicDepthAll = 8; // the slider's top stop, labelled "all": no limit
static bool s_basicCombineObjects = true; // Basic: one shared node per object vs. duplicate per reference

// Stable identity key for a swimlanes object row (kind + case-insensitive name).
static QString swimRowKey(RefObjectKind kind, const QString& name)
{
	return QString::number(static_cast<int>(kind)) + QLatin1Char('|') + name.toLower();
}

// ---------------------------------------------------------------------------
// Style
// ---------------------------------------------------------------------------

EventGraphStyle EventGraphStyle::makeStyle(bool dark)
{
	EventGraphStyle s; // light defaults are the struct initializers
	if (dark) {
		s.bgColor     = QColor(31, 29, 26);
		s.gridMinor   = QColor(51, 47, 40);
		s.objectFill  = QColor(42, 40, 36);
		s.eventFill   = QColor(59, 42, 24);
		s.condFill    = QColor(29, 42, 61);
		s.actionFill  = QColor(45, 29, 43);
		s.cardBorder  = QColor(74, 70, 62);
		s.nodeText    = QColor(233, 230, 222);
		s.nodeSubText = QColor(180, 175, 163);
		s.eventChip   = QColor(150, 150, 150);
		s.eventBadge  = QColor(190, 160, 110);
		s.ringColor   = QColor(74, 70, 62);
	}
	// Chip accent colors (blue/purple/orange) are vivid enough for both themes.
	return s;
}

QColor EventGraphStyle::colorFor(RefObjectKind kind) const
{
	switch (kind) {
	// "objects": the spatial first-class types share the orange accent.
	case RefObjectKind::Ship:
	case RefObjectKind::Wing:
	case RefObjectKind::Prop:
	case RefObjectKind::Waypoint:
	case RefObjectKind::JumpNode:
	case RefObjectKind::CoordinatePoint:
		return entity;
	case RefObjectKind::Message:   return message;
	case RefObjectKind::Variable:  return variable;
	case RefObjectKind::Container: return container;
	// Everything else (goal, team, event name, ...) is generic "data".
	default:                       return dataColor;
	}
}

// ---------------------------------------------------------------------------
// Items
// ---------------------------------------------------------------------------

namespace graphdetail {

class RefEdgeItem; // defined below; cards keep back-pointers to their edges

static constexpr qreal kNodeW = 224.0;
static constexpr qreal kEventW = 158.0;
static constexpr qreal kObjectW = 224.0;

// Running Z so a clicked card jumps above all others (overlaps are fine).
static qreal s_cardTopZ = 100.0;

// Layout metrics (item pixels).
namespace metric {
constexpr qreal padX = 12.0, padTop = 7.0, padBottom = 8.0;
constexpr qreal chipH = 14.0, titleH = 20.0, lineH = 15.0, subH = 14.0, toggleH = 15.0;
} // namespace metric

// The event card's main icon reuses the tree's tinted dot/chain assets: blue dot
// (event), red dot (directive), chain, red chain (chain + directive).
inline NodeImage eventMainImage(bool chained, bool directive)
{
	if (chained)
		return directive ? NodeImage::CHAIN_DIRECTIVE : NodeImage::CHAIN;
	return directive ? NodeImage::ROOT_DIRECTIVE : NodeImage::ROOT;
}

inline void drawEventMainIcon(QPainter* p, bool chained, bool directive, const QRectF& box)
{
	const QIcon icon = sexp_tree_view::convertNodeImageToIcon(eventMainImage(chained, directive));
	p->drawPixmap(box.toRect(), icon.pixmap(box.size().toSize()));
}

// Small hand-drawn glyphs for the "extra" event flags (no tree asset for these).
enum class StatusGlyph { Repeat, Log };
inline void drawStatusGlyph(QPainter* p, StatusGlyph g, const QRectF& box, const QColor& col)
{
	if (g == StatusGlyph::Repeat) { // circular arrow
		const QRectF ar = box.adjusted(2.0, 2.0, -2.0, -2.0);
		p->setPen(QPen(col, 1.3));
		p->setBrush(Qt::NoBrush);
		p->drawArc(ar, 30 * 16, 280 * 16);
		const QPointF c = ar.center();
		const double a0 = 30.0 * M_PI / 180.0;
		const QPointF tip(c.x() + (ar.width() / 2.0) * std::cos(a0), c.y() - (ar.height() / 2.0) * std::sin(a0));
		QPolygonF head;
		head << tip << (tip + QPointF(-3.0, -1.5)) << (tip + QPointF(0.5, 3.0));
		p->setPen(Qt::NoPen);
		p->setBrush(col);
		p->drawPolygon(head);
	} else { // logging: a lined document
		const QRectF doc = box.adjusted(2.0, 1.0, -2.0, -1.0);
		p->setPen(QPen(col, 1.1));
		p->setBrush(Qt::NoBrush);
		p->drawRoundedRect(doc, 1.5, 1.5);
		p->setPen(QPen(col, 0.8));
		for (int i = 1; i <= 3; ++i) {
			const qreal ly = doc.top() + doc.height() * i / 4.0;
			p->drawLine(QPointF(doc.left() + 2.0, ly), QPointF(doc.right() - 2.0, ly));
		}
	}
}

// A card: a role/kind chip (top-left), an optional event-name badge (top-right),
// a bold title, an optional bulleted body (the operator's arguments), an
// optional subtitle, and -- when the body is long -- an expand/collapse toggle.
class CardItem : public QGraphicsItem {
  public:
	CardItem(qreal width, QColor fill, QColor chipColor, QString chip, QString title, QVector<QString> lines,
		QString subtitle, QString cornerText, QColor cornerColor, bool expandable, int collapsedMax,
		const EventGraphStyle& style)
		: m_width(width), m_fill(fill), m_chipColor(chipColor), m_chip(std::move(chip)), m_title(std::move(title)),
		  m_lines(std::move(lines)), m_subtitle(std::move(subtitle)), m_cornerText(std::move(cornerText)),
		  m_cornerColor(cornerColor), m_expandable(expandable), m_collapsedMax(collapsedMax), m_style(style)
	{
		setFlag(ItemIsSelectable, true);
		setFlag(ItemSendsGeometryChanges, true); // so moving the card can update its edges
		// Card paint is heavy (fonts, elided text, icons); cache it to a pixmap so
		// panning/zooming reuses it. update() (selection, annotation, order, status)
		// invalidates the cache, so it stays correct.
		setCacheMode(DeviceCoordinateCache);
		// Show the normal pointer over cards instead of the pan hand.
		setCursor(Qt::ArrowCursor);
		setAcceptHoverEvents(true);
	}

	QRectF boundingRect() const override
	{
		const qreal h = contentHeight();
		return {-m_width / 2, -h / 2, m_width, h};
	}

	// Register an edge attached to this card so it can be redrawn when the card
	// moves (see itemChange).
	void addEdge(RefEdgeItem* e)
	{
		if (e)
			m_edges.push_back(e);
	}

	// Drop a soon-to-be-deleted edge so a later move doesn't dereference it.
	void removeEdge(RefEdgeItem* e) { m_edges.removeAll(e); }

	// Raise above every other card; called on any click.
	void bringToFront() { setZValue(s_cardTopZ += 1.0); }

	QString titleText() const { return m_title; }

	// The node's annotation key (tree_nodes index, or rootKey for event roots),
	// -1 when the card has no single annotation target (aggregate object cards).
	// Used to target the right-click comment/color menu.
	void setAnnotationKey(int key) { m_annKey = key; }
	int annotationKey() const { return m_annKey; }

	// Top-left corner marker: a 1-based order among sibling sexp nodes (0 = none),
	// or an asterisk when the card aggregates several nodes (aggregate wins).
	void setOrder(int order) { m_order = order; update(); }
	void setAggregate(bool aggregate) { m_aggregate = aggregate; update(); }

	// Inline literal-arg bullets: the tree node behind each body line, so a click on
	// a bullet can target that specific arg for editing. bulletNodeAt maps a scene
	// position to the arg node under it (-1 if not on a bullet, or no node known).
	void setArgNodes(QVector<int> nodes) { m_argNodes = std::move(nodes); }
	int bulletNodeAt(const QPointF& scenePos) const
	{
		const QPointF lp = mapFromScene(scenePos);
		for (int i = 0; i < m_bulletRects.size(); ++i)
			if (m_bulletRects[i].contains(lp))
				return (i < m_argNodes.size()) ? m_argNodes[i] : -1;
		return -1;
	}

	// Case-insensitive substring match against the card's visible text, for the
	// graph search box. An empty query matches everything.
	bool matchesQuery(const QString& q) const
	{
		if (q.isEmpty())
			return true;
		if (m_title.contains(q, Qt::CaseInsensitive) || m_cornerText.contains(q, Qt::CaseInsensitive)
			|| m_chip.contains(q, Qt::CaseInsensitive) || m_subtitle.contains(q, Qt::CaseInsensitive))
			return true;
		return std::any_of(m_lines.begin(), m_lines.end(),
			[&q](const QString& l) { return l.contains(q, Qt::CaseInsensitive); });
	}

	// Distinct highlight for a swimlanes filter item (independent of selection).
	void setFocusHighlight(bool on) { m_focusHighlight = on; update(); }

	// A node's comment/color annotation: color draws a thin edge stripe, a comment
	// shows a small badge and is folded into the tooltip.
	void setAnnotation(const QColor& annColor, const QString& comment)
	{
		m_annColor = annColor;
		m_hasComment = !comment.isEmpty();
		if (m_hasComment) {
			const QString existing = toolTip();
			setToolTip(existing.isEmpty() ? comment : comment + QStringLiteral("\n\n") + existing);
		}
		update();
	}

	// If the click landed on the expand/collapse toggle, flip it. Returns true
	// if it consumed the click.
	bool handleToggleClick(const QPointF& scenePos)
	{
		if (!hasToggle())
			return false;
		if (m_toggleRect.contains(mapFromScene(scenePos))) {
			prepareGeometryChange();
			m_expanded = !m_expanded;
			update();
			return true;
		}
		return false;
	}

	// Card body expansion (only cards with more bullets than the collapsed limit
	// can expand). Used by the graph's "Expand Card" context-menu action.
	bool canExpand() const { return hasToggle() && !m_expanded; }
	void setExpanded(bool on)
	{
		if (!hasToggle() || m_expanded == on)
			return;
		prepareGeometryChange();
		m_expanded = on;
		update();
	}

	void paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) override
	{
		p->setRenderHint(QPainter::Antialiasing, true);
		const QRectF r = boundingRect();

		// Filter focus (red) wins over tree selection (orange) wins over normal.
		QColor borderColor = m_style.cardBorder;
		qreal borderW = 1.4;
		if (m_focusHighlight) {
			borderColor = QColor(214, 40, 40);
			borderW = 2.8;
		} else if (isSelected()) {
			borderColor = QColor(217, 79, 42);
			borderW = 2.4;
		}
		p->setPen(QPen(borderColor, borderW));
		p->setBrush(m_fill);
		p->drawRoundedRect(r, m_style.nodeRadius, m_style.nodeRadius);

		// Annotation color: a thin stripe down the card's left edge.
		if (m_annColor.isValid()) {
			p->setPen(Qt::NoPen);
			p->setBrush(m_annColor);
			p->drawRoundedRect(QRectF(r.left() + 2.0, r.top() + 4.0, 4.5, r.height() - 8.0), 2.0, 2.0);
		}

		const QRectF inner = r.adjusted(metric::padX, metric::padTop, -10.0, -metric::padBottom);
		qreal y = inner.top();

		// Every text section sizes itself from this one base font. Deriving each from
		// p->font() would chain off whichever optional section drew last, so a card
		// with a badge or order marker got a smaller title and body than one without.
		const QFont base = p->font();

		// Top-right event badge.
		if (!m_cornerText.isEmpty()) {
			QFont bf = base;
			bf.setPointSizeF(std::max(6.0, bf.pointSizeF() - 2.0));
			QFontMetricsF fm(bf);
			const qreal maxW = m_width * 0.55;
			const QString bt = fm.elidedText(m_cornerText, Qt::ElideRight, maxW - 10.0);
			const qreal bw = std::min(maxW, fm.horizontalAdvance(bt) + 10.0);
			const QRectF badge(inner.right() - bw, y - 1.0, bw, 15.0);
			QColor fillc = m_cornerColor;
			fillc.setAlpha(38);
			p->setPen(QPen(m_cornerColor, 1.1));
			p->setBrush(fillc);
			p->drawRoundedRect(badge, 3.0, 3.0);
			p->setFont(bf);
			p->setPen(m_cornerColor);
			p->drawText(badge, Qt::AlignCenter, bt);
		}

		// Top-left order marker: sibling order number (or "*" for an aggregate).
		qreal chipX = inner.left();
		if (m_aggregate || m_order > 0) {
			const QString mark = m_aggregate ? QStringLiteral("*") : QString::number(m_order);
			QFont mf = base;
			mf.setBold(true);
			mf.setPointSizeF(std::max(6.0, mf.pointSizeF() - 1.0));
			p->setFont(mf);
			p->setPen(m_style.nodeSubText);
			const qreal markW = std::max(10.0, QFontMetricsF(mf).horizontalAdvance(mark) + 3.0);
			p->drawText(QRectF(inner.left(), y, markW, metric::chipH), Qt::AlignLeft | Qt::AlignVCenter, mark);
			chipX = inner.left() + markW + 3.0;
		}

		// Chip (role/kind) top-left (after the order marker, if any).
		QFont chipFont = base;
		chipFont.setPointSizeF(std::max(6.0, base.pointSizeF() - 1.5));
		p->setFont(chipFont);
		p->setPen(m_chipColor);
		p->drawText(QRectF(chipX, y, inner.left() + inner.width() * 0.45 - chipX, metric::chipH),
			Qt::AlignLeft | Qt::AlignVCenter, m_chip.toUpper());
		y += metric::chipH + 2.0;

		// Title.
		if (!m_title.isEmpty()) {
			QFont titleFont = base;
			titleFont.setBold(true);
			titleFont.setPointSizeF(std::max(6.0, base.pointSizeF() - 0.5));
			p->setFont(titleFont);
			p->setPen(m_style.nodeText);
			const QString t = QFontMetricsF(titleFont).elidedText(m_title, Qt::ElideRight, inner.width());
			p->drawText(QRectF(inner.left(), y, inner.width(), metric::titleH),
				Qt::AlignLeft | Qt::AlignVCenter, t);
			y += metric::titleH;
		}

		// Bulleted body lines.
		QFont lineFont = base;
		lineFont.setPointSizeF(std::max(6.0, base.pointSizeF() - 0.5));
		p->setFont(lineFont);
		p->setPen(m_style.nodeSubText);
		QFontMetricsF lfm(lineFont);
		const int shown = visibleLineCount();
		m_bulletRects.clear();
		for (int i = 0; i < shown; ++i) {
			const QRectF lineRect(inner.left(), y, inner.width(), metric::lineH);
			const QString line = QString(QChar(0x2022)) + QLatin1Char(' ') + m_lines[i]; // bullet
			const QString el = lfm.elidedText(line, Qt::ElideRight, inner.width());
			// Hit rect spans only the actual arg text, so the empty area past it still
			// behaves as draggable card (cursor + click).
			m_bulletRects.push_back(QRectF(inner.left(), y, lfm.horizontalAdvance(el), metric::lineH));
			p->drawText(lineRect, Qt::AlignLeft | Qt::AlignVCenter, el);
			y += metric::lineH;
		}

		// Expand/collapse toggle.
		if (hasToggle()) {
			m_toggleRect = QRectF(inner.left(), y, inner.width(), metric::toggleH);
			const int hidden = m_lines.size() - m_collapsedMax;
			const QString label = m_expanded ? QStringLiteral("show less")
											 : QStringLiteral("+%1 more").arg(hidden);
			QFont tf = p->font();
			tf.setItalic(true);
			tf.setPointSizeF(std::max(6.0, tf.pointSizeF() - 1.5));
			p->setFont(tf);
			p->setPen(m_chipColor);
			p->drawText(m_toggleRect, Qt::AlignLeft | Qt::AlignVCenter, label);
			y += metric::toggleH;
		}

		// Subtitle.
		if (!m_subtitle.isEmpty()) {
			QFont subFont = p->font();
			subFont.setItalic(true);
			subFont.setPointSizeF(std::max(6.0, subFont.pointSizeF() - 1.5));
			p->setFont(subFont);
			p->setPen(m_style.nodeSubText);
			const QString sub = QFontMetricsF(subFont).elidedText(m_subtitle, Qt::ElideRight, inner.width());
			p->drawText(QRectF(inner.left(), y, inner.width(), metric::subH),
				Qt::AlignLeft | Qt::AlignVCenter, sub);
		}

		// Comment badge (small note glyph, bottom-right); the text is in the tooltip.
		if (m_hasComment) {
			const QRectF note(r.right() - 15.0, r.bottom() - 13.0, 9.0, 8.0);
			p->setPen(QPen(m_style.nodeSubText, 1.0));
			p->setBrush(m_style.eventFill);
			p->drawRoundedRect(note, 1.5, 1.5);
			p->setPen(QPen(m_style.nodeSubText, 0.8));
			p->drawLine(QPointF(note.left() + 2.0, note.center().y() - 1.5),
				QPointF(note.right() - 2.0, note.center().y() - 1.5));
			p->drawLine(QPointF(note.left() + 2.0, note.center().y() + 1.5),
				QPointF(note.right() - 2.0, note.center().y() + 1.5));
		}
	}

  protected:
	// Redraw attached edges when the card is dragged to a new position. Defined
	// out of line below, once RefEdgeItem is a complete type.
	QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

	// Over an interactive spot (an arg bullet, or a subclass's control like the
	// event collapse box), show the normal pointer instead of the move cursor a
	// draggable card otherwise uses.
	void hoverMoveEvent(QGraphicsSceneHoverEvent* e) override
	{
		if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
			setCursor(Qt::OpenHandCursor); // Shift = pan mode, even over a card
		else if (flags() & ItemIsMovable)
			setCursor(wantsPointerAt(e->scenePos()) ? Qt::ArrowCursor : Qt::SizeAllCursor);
		else
			setCursor(Qt::ArrowCursor);
		QGraphicsItem::hoverMoveEvent(e);
	}
	virtual bool wantsPointerAt(const QPointF& scenePos) const { return bulletNodeAt(scenePos) >= 0; }

	bool hasToggle() const { return m_expandable && m_lines.size() > m_collapsedMax; }

	int visibleLineCount() const
	{
		if (!hasToggle() || m_expanded)
			return m_lines.size();
		return m_collapsedMax;
	}

	qreal contentHeight() const
	{
		qreal h = metric::padTop + metric::chipH + 2.0;
		if (!m_title.isEmpty())
			h += metric::titleH;
		h += visibleLineCount() * metric::lineH;
		if (hasToggle())
			h += metric::toggleH;
		if (!m_subtitle.isEmpty())
			h += metric::subH;
		h += metric::padBottom;
		return h;
	}

	qreal   m_width;
	QColor  m_fill;
	QColor  m_chipColor;
	QString m_chip;
	QString m_title;
	QVector<QString> m_lines;
	QString m_subtitle;
	QString m_cornerText;
	QColor  m_cornerColor;
	bool    m_expandable;
	int     m_collapsedMax;
	bool    m_expanded = false;
	bool    m_focusHighlight = false;
	QColor  m_annColor;            // annotation color (invalid = none)
	bool    m_hasComment = false;  // annotation comment present
	int     m_annKey = -1;         // annotation key for the right-click menu (-1 = none)
	int     m_order = 0;           // 1-based sibling order for the corner marker (0 = none)
	bool    m_aggregate = false;   // card stands for many nodes -> corner shows "*"
	QVector<int> m_argNodes;       // tree node per body line (inline literal args)
	QVector<QRectF> m_bulletRects; // per-line hit rects, filled in paint()
	QRectF  m_toggleRect;
	EventGraphStyle m_style;
	QVector<RefEdgeItem*> m_edges; // edges attached to this card (for live redraw on move)
};

// The center object (ship/wing).
class ObjectNodeItem final : public CardItem {
  public:
	enum { Type = UserType + 1 };
	ObjectNodeItem(QString chip, QColor chipColor, QString title, QString subtitle, const EventGraphStyle& style)
		: CardItem(kObjectW, style.objectFill, chipColor, std::move(chip), std::move(title), {},
			  std::move(subtitle), QString(), QColor(), /*expandable=*/false, 0, style)
	{
	}
	int type() const override { return Type; }
};

// A referencing event (tier 1): chip + name, plus a row of presence-only status
// icons (chained / directive / repeats / logging) in the top-right.
class EventNodeItem final : public CardItem {
  public:
	enum { Type = UserType + 2 };
	EventNodeItem(int eventIndex, QString title, const EventGraphStyle& style)
		: CardItem(kEventW, style.eventFill, style.eventChip, QStringLiteral("event"), std::move(title), {},
			  QString(), QString(), QColor(), /*expandable=*/false, 0, style),
		  m_eventIndex(eventIndex)
	{
	}
	int type() const override { return Type; }
	int eventIndex() const { return m_eventIndex; }

	void setStatusIcons(bool chained, bool directive, bool repeats, bool logging)
	{
		m_chained = chained;
		m_directive = directive;
		m_repeats = repeats;
		m_logging = logging;
		update();
	}

	// Show a collapse box (Basic view, events with a subtree) and its state.
	void setCollapseState(bool show, bool collapsed)
	{
		m_showCollapse = show;
		m_collapsed = collapsed;
		update();
	}
	bool isCollapsed() const { return m_showCollapse && m_collapsed; }
	bool collapseToggleAt(const QPointF& scenePos) const
	{
		return m_showCollapse && m_collapseRect.contains(mapFromScene(scenePos));
	}

	void paint(QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w) override
	{
		CardItem::paint(p, o, w);
		drawStatusIcons(p);
		if (m_showCollapse)
			drawCollapseBox(p);
	}

  protected:
	// Also treat the collapse box as a pointer target (not a drag handle).
	bool wantsPointerAt(const QPointF& scenePos) const override
	{
		return collapseToggleAt(scenePos) || CardItem::wantsPointerAt(scenePos);
	}

  private:
	// Main tree icon (event / directive / chain, tinted) plus small repeat/log
	// glyphs, right-aligned in the chip row.
	void drawStatusIcons(QPainter* p) const
	{
		const QRectF r = boundingRect();
		const qreal sz = 13.0, gap = 3.0;
		const qreal top = r.top() + 4.0;
		const int count = 1 + (m_repeats ? 1 : 0) + (m_logging ? 1 : 0); // main icon is always shown
		const qreal totalW = count * sz + (count - 1) * gap;
		qreal x = r.right() - 8.0 - totalW;

		p->save();
		p->setRenderHint(QPainter::Antialiasing, true);
		p->setRenderHint(QPainter::SmoothPixmapTransform, true);
		drawEventMainIcon(p, m_chained, m_directive, QRectF(x, top, sz, sz));
		x += sz + gap;
		const QColor col = m_style.nodeSubText;
		if (m_repeats) {
			drawStatusGlyph(p, StatusGlyph::Repeat, QRectF(x, top, sz, sz), col);
			x += sz + gap;
		}
		if (m_logging)
			drawStatusGlyph(p, StatusGlyph::Log, QRectF(x, top, sz, sz), col);
		p->restore();
	}

	// Small [-] / [+] box on the right edge: collapse or expand the event subtree.
	void drawCollapseBox(QPainter* p)
	{
		const QRectF r = boundingRect();
		const qreal s = 12.0;
		m_collapseRect = QRectF(r.right() - s - 4.0, r.center().y() - s / 2.0, s, s);
		p->save();
		p->setRenderHint(QPainter::Antialiasing, true);
		p->setPen(QPen(m_style.nodeSubText, 1.1));
		p->setBrush(m_style.bgColor);
		p->drawRoundedRect(m_collapseRect, 2.5, 2.5);
		const QPointF c = m_collapseRect.center();
		p->drawLine(QPointF(c.x() - 3.0, c.y()), QPointF(c.x() + 3.0, c.y())); // minus (always)
		if (m_collapsed)
			p->drawLine(QPointF(c.x(), c.y() - 3.0), QPointF(c.x(), c.y() + 3.0)); // + when collapsed
		p->restore();
	}

	int  m_eventIndex;
	bool m_chained = false;
	bool m_directive = false;
	bool m_repeats = false;
	bool m_logging = false;
	bool m_showCollapse = false;
	bool m_collapsed = false;
	QRectF m_collapseRect;
};

// A condition/action operator node (tier 2/3). Title = operator name; body =
// its arguments as a bulleted list; the event it belongs to is a top-right badge.
class SexpNodeItem final : public CardItem {
  public:
	enum { Type = UserType + 3 };
	SexpNodeItem(int treeNode, int eventIndex, bool isCond, QString opName, QVector<QString> args,
		QString eventName, const QString& fullExpr, const EventGraphStyle& style)
		: CardItem(kNodeW, isCond ? style.condFill : style.actionFill, isCond ? style.condChip : style.actionChip,
			  isCond ? QStringLiteral("cond") : QStringLiteral("action"), std::move(opName), std::move(args),
			  QString(), std::move(eventName), style.eventBadge, /*expandable=*/true, /*collapsedMax=*/3, style),
		  m_treeNode(treeNode), m_eventIndex(eventIndex), m_isCond(isCond)
	{
		if (!fullExpr.isEmpty())
			setToolTip(fullExpr);
	}
	int type() const override { return Type; }
	int treeNode() const { return m_treeNode; }
	int eventIndex() const { return m_eventIndex; }
	bool isCond() const { return m_isCond; }

  private:
	int  m_treeNode;
	int  m_eventIndex;
	bool m_isCond;
};

class RefEdgeItem final : public QGraphicsPathItem {
  public:
	RefEdgeItem(const QPointF& a, const QPointF& b, QColor edgeColor, const EventGraphStyle& style, bool chain = false)
		: m_isChain(chain)
	{
		// Chain (event-order) lines are solid and a touch heavier; reference/flow
		// edges are the lighter dashed default.
		QPen pen(edgeColor, chain ? style.edgeWidth + 0.7 : style.edgeWidth,
			chain ? Qt::SolidLine : Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
		setPen(pen);
		setZValue(chain ? -0.5 : -1.0);
		setAcceptedMouseButtons(Qt::NoButton);
		setPathBetween(a, b);
	}

	// True for event-chain lines, which the emphasis pass leaves always visible
	// (they are structure, not object references).
	bool isChain() const { return m_isChain; }

	// The two card items this edge connects. Registers the edge on each card so a
	// drag can redraw it (see CardItem::itemChange), and drives the emphasis pass.
	void setEndpoints(CardItem* a, CardItem* b)
	{
		m_endA = a;
		m_endB = b;
		if (a)
			a->addEdge(this);
		if (b)
			b->addEdge(this);
	}
	QGraphicsItem* endA() const { return m_endA; }
	QGraphicsItem* endB() const { return m_endB; }

	// Unregister from both endpoint cards before this edge is deleted, so their
	// m_edges lists don't retain a dangling pointer (a later card move would
	// dereference it). Safe only while the endpoint cards are still alive.
	void detachEndpoints()
	{
		if (m_endA)
			m_endA->removeEdge(this);
		if (m_endB)
			m_endB->removeEdge(this);
		m_endA = nullptr;
		m_endB = nullptr;
	}

	// Recompute the path from the endpoints' current positions (their scene
	// origins are the card centers).
	void updatePath()
	{
		if (m_endA && m_endB)
			setPathBetween(m_endA->pos(), m_endB->pos());
	}

	// Chevrons overshoot the stroke slightly; pad the bounds so they aren't clipped.
	QRectF boundingRect() const override { return QGraphicsPathItem::boundingRect().adjusted(-6, -6, 6, 6); }

	void paint(QPainter* p, const QStyleOptionGraphicsItem* opt, QWidget* w) override
	{
		QGraphicsPathItem::paint(p, opt, w);
		if (!m_isChain)
			return;

		// Subtle direction arrows along the chain line, in the line's color, pointing
		// from the earlier event toward the later (chained) one.
		const QPainterPath pth = path();
		const qreal len = pth.length();
		if (len < 26.0)
			return;
		p->save();
		p->setRenderHint(QPainter::Antialiasing, true);
		p->setPen(QPen(pen().color(), std::max(1.2, pen().widthF() - 0.6), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		const qreal spacing = 36.0, sz = 4.0;
		for (qreal d = spacing * 0.6; d < len - 4.0; d += spacing) {
			const QPointF pt = pth.pointAtPercent(pth.percentAtLength(d));
			const QPointF ahead = pth.pointAtPercent(pth.percentAtLength(std::min(len, d + 2.0)));
			QPointF u = ahead - pt;
			const qreal ul = qSqrt(u.x() * u.x() + u.y() * u.y());
			if (ul < 0.01)
				continue;
			u /= ul;
			const QPointF nrm(-u.y(), u.x());
			const QPointF tip = pt + u * (sz * 0.6);
			const QPointF back = pt - u * (sz * 0.6);
			p->drawLine(tip, back + nrm * sz);
			p->drawLine(tip, back - nrm * sz);
		}
		p->restore();
	}

  private:
	void setPathBetween(const QPointF& a, const QPointF& b)
	{
		QPainterPath path(a);
		const QPointF mid = (a + b) / 2.0;
		const QPointF off(-(b.y() - a.y()) * 0.10, (b.x() - a.x()) * 0.10); // gentle bow
		path.quadTo(mid + off, b);
		setPath(path);
	}

	CardItem* m_endA = nullptr;
	CardItem* m_endB = nullptr;
	bool m_isChain = false;
};

// Now that RefEdgeItem is complete, define the card's move handler.
QVariant CardItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if (change == ItemPositionHasChanged)
		for (RefEdgeItem* e : m_edges)
			if (e)
				e->updatePath();
	return QGraphicsItem::itemChange(change, value);
}

// Bottom-left overview of the whole graph with a current-viewport indicator.
// The scene is rendered once into a cached pixmap (regenerate()) and only that
// pixmap + the moving viewport rectangle are drawn per repaint, so panning and
// zooming stay cheap even on huge missions.
class MinimapWidget final : public QWidget {
  public:
	MinimapWidget(EventGraphView* view, QGraphicsScene* scene, const EventGraphStyle* style, QWidget* parent)
		: QWidget(parent), m_view(view), m_scene(scene), m_style(style)
	{
		setFixedSize(190, 130);
		setToolTip(QStringLiteral("Overview - drag to pan, scroll to zoom"));
		setCursor(Qt::ArrowCursor); // not the pan hand inherited from the viewport
		setMouseTracking(true);     // so hover updates the cursor without a button
	}

	// Re-render the scene into the cached pixmap. Call when the graph content
	// changes (rebuild) or the theme changes.
	void regenerate()
	{
		if (!s_showMinimap)
			return; // hidden: skip the expensive full-scene render until it's shown again
		QRectF src = m_scene->itemsBoundingRect();
		if (src.isEmpty()) {
			m_cache = QPixmap();
			m_src = QRectF();
			m_dst = QRectF();
			update();
			return;
		}
		src.adjust(-40, -40, 40, 40);

		const QRectF inner = QRectF(rect()).adjusted(4, 4, -4, -4);
		const qreal s = std::min(inner.width() / src.width(), inner.height() / src.height());
		QRectF dst(0, 0, src.width() * s, src.height() * s);
		dst.moveCenter(inner.center());
		m_src = src;
		m_dst = dst;

		const qreal dpr = devicePixelRatioF();
		QPixmap pm(QSize(qRound(width() * dpr), qRound(height() * dpr)));
		pm.setDevicePixelRatio(dpr);
		pm.fill(Qt::transparent);
		QPainter pp(&pm);
		pp.setRenderHint(QPainter::Antialiasing, true);
		pp.setClipRect(dst);
		m_scene->render(&pp, dst, src);
		m_cache = pm;
		update();
	}

  protected:
	void paintEvent(QPaintEvent*) override
	{
		QPainter p(this);
		p.setRenderHint(QPainter::Antialiasing, true);

		const QRectF frame = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
		p.setPen(Qt::NoPen);
		p.setBrush(m_style->bgColor);
		p.drawRoundedRect(frame, 4.0, 4.0);

		if (!m_cache.isNull())
			p.drawPixmap(0, 0, m_cache);

		m_boxRect = QRectF();
		if (!m_src.isEmpty() && m_dst.isValid()) {
			const qreal s = m_dst.width() / m_src.width();
			const QRectF viewScene = m_view->mapToScene(m_view->viewport()->rect()).boundingRect();
			QRectF vr(m_dst.left() + (viewScene.left() - m_src.left()) * s,
				m_dst.top() + (viewScene.top() - m_src.top()) * s, viewScene.width() * s,
				viewScene.height() * s);
			vr = vr.intersected(m_dst);
			m_boxRect = vr;
			p.setPen(QPen(QColor(217, 79, 42), 1.5));
			p.setBrush(Qt::NoBrush);
			p.drawRect(vr);
		}

		p.setPen(QPen(m_style->cardBorder, 1.0));
		p.setBrush(Qt::NoBrush);
		p.drawRoundedRect(frame, 4.0, 4.0);
	}

	void mousePressEvent(QMouseEvent* e) override
	{
		if (e->button() != Qt::LeftButton || !m_dst.isValid() || m_src.isEmpty())
			return;
		m_dragging = true;
		setCursor(Qt::ClosedHandCursor);
		recenterTo(e->pos());
	}

	void mouseMoveEvent(QMouseEvent* e) override
	{
		if (m_dragging) {
			recenterTo(e->pos());
			return;
		}
		// Hover affordance: open hand over the view box, arrow elsewhere.
		setCursor(m_boxRect.contains(QPointF(e->pos())) ? Qt::OpenHandCursor : Qt::ArrowCursor);
	}

	void mouseReleaseEvent(QMouseEvent* e) override
	{
		if (e->button() != Qt::LeftButton)
			return;
		m_dragging = false;
		setCursor(m_boxRect.contains(QPointF(e->pos())) ? Qt::OpenHandCursor : Qt::ArrowCursor);
	}

	void wheelEvent(QWheelEvent* e) override
	{
		const int dy = e->angleDelta().y();
		if (dy != 0)
			m_view->zoomStep(dy > 0);
		e->accept();
	}

  private:
	// Recenter the main view on the scene point under the given minimap position.
	void recenterTo(const QPoint& pos)
	{
		if (!m_dst.isValid() || m_src.isEmpty())
			return;
		const qreal s = m_dst.width() / m_src.width();
		const qreal x = m_src.left() + (qBound(m_dst.left(), qreal(pos.x()), m_dst.right()) - m_dst.left()) / s;
		const qreal y = m_src.top() + (qBound(m_dst.top(), qreal(pos.y()), m_dst.bottom()) - m_dst.top()) / s;
		m_view->centerOn(x, y);
	}

	EventGraphView* m_view;
	QGraphicsScene* m_scene;
	const EventGraphStyle* m_style;
	QPixmap m_cache;
	QRectF m_src, m_dst, m_boxRect;
	bool m_dragging = false;
};

// A small color-key panel (top-right) explaining the card/edge colors. Reads the
// live style so it re-themes with the view.
class LegendWidget final : public QWidget {
  public:
	LegendWidget(const EventGraphStyle* style, QWidget* parent) : QWidget(parent), m_style(style)
	{
		setCursor(Qt::ArrowCursor); // normal pointer, not the viewport's pan hand
		relayout();
	}

	// The object kinds present in the current graph; the legend lists only the
	// object rows that actually appear (the cond/action/event rows are always shown).
	void setPresentKinds(const QSet<int>& kinds)
	{
		m_kinds = kinds;
		relayout();
		update();
	}

	// Size follows the taller of the two columns (colors vs. icons).
	void relayout()
	{
		const int n = std::max(static_cast<int>(rows().size()), iconRows());
		setFixedSize(kColW * 2, kTop + n * kRowH + 8);
	}

  protected:
	// Swallow mouse events so hovering/clicking the legend keeps the normal cursor
	// and never starts a viewport pan. The legend is inert -- no click actions.
	void mousePressEvent(QMouseEvent* e) override { e->accept(); }
	void mouseMoveEvent(QMouseEvent* e) override { e->accept(); }
	void mouseReleaseEvent(QMouseEvent* e) override { e->accept(); }

	void paintEvent(QPaintEvent*) override
	{
		QPainter p(this);
		p.setRenderHint(QPainter::Antialiasing, true);
		p.setRenderHint(QPainter::SmoothPixmapTransform, true);

		const QRectF frame = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
		p.setPen(QPen(m_style->cardBorder, 1.0));
		p.setBrush(m_style->bgColor);
		p.drawRoundedRect(frame, 4.0, 4.0);

		QFont title = p.font();
		title.setBold(true);
		title.setPointSizeF(std::max(6.0, title.pointSizeF() - 1.0));
		p.setFont(title);
		p.setPen(m_style->nodeText);
		p.drawText(QRectF(10, 4, width() - 14, kTop - 4), Qt::AlignLeft | Qt::AlignVCenter, tr("Legend"));

		QFont lf = p.font();
		lf.setBold(false);
		lf.setPointSizeF(std::max(6.0, lf.pointSizeF() - 1.0));
		p.setFont(lf);

		// Left column: color key.
		int y = kTop;
		for (const auto& r : rows()) {
			p.setBrush(r.first);
			p.setPen(QPen(m_style->cardBorder, 1.0));
			p.drawRoundedRect(QRectF(10, y + 3, 12, 12), 2.0, 2.0);
			p.setPen(m_style->nodeSubText);
			p.drawText(QRectF(28, y, kColW - 32, kRowH), Qt::AlignLeft | Qt::AlignVCenter, r.second);
			y += kRowH;
		}

		// Right column: event-icon key.
		y = kTop;
		const qreal ix = kColW + 6.0;
		for (int i = 0; i < iconRows(); ++i) {
			drawIcon(&p, i, QRectF(ix, y + 2.0, 13.0, 13.0));
			p.setPen(m_style->nodeSubText);
			p.drawText(QRectF(ix + 18.0, y, kColW - 24.0, kRowH), Qt::AlignLeft | Qt::AlignVCenter, iconLabel(i));
			y += kRowH;
		}
	}

  private:
	static int iconRows() { return 5; }

	static QString iconLabel(int i)
	{
		switch (i) {
		case 0: return tr("event");
		case 1: return tr("directive");
		case 2: return tr("chained");
		case 3: return tr("repeats");
		default: return tr("logged");
		}
	}

	void drawIcon(QPainter* p, int i, const QRectF& box) const
	{
		switch (i) {
		case 0: drawEventMainIcon(p, false, false, box); break; // event (blue dot)
		case 1: drawEventMainIcon(p, false, true, box); break;  // directive (red dot)
		case 2: drawEventMainIcon(p, true, false, box); break;  // chained (chain)
		case 3: drawStatusGlyph(p, StatusGlyph::Repeat, box, m_style->nodeSubText); break;
		default: drawStatusGlyph(p, StatusGlyph::Log, box, m_style->nodeSubText); break;
		}
	}

	QVector<QPair<QColor, QString>> rows() const
	{
		// Structural colors are always present; event first.
		QVector<QPair<QColor, QString>> r{
			{m_style->eventChip, tr("event")},
			{m_style->condChip, tr("condition")},
			{m_style->actionChip, tr("action")},
		};
		auto has = [this](RefObjectKind k) { return m_kinds.contains(static_cast<int>(k)); };
		if (has(RefObjectKind::Ship) || has(RefObjectKind::Wing) || has(RefObjectKind::Prop)
			|| has(RefObjectKind::Waypoint) || has(RefObjectKind::JumpNode)
			|| has(RefObjectKind::CoordinatePoint))
			r.append({m_style->entity, tr("objects")});
		if (has(RefObjectKind::Message))
			r.append({m_style->message, tr("message")});
		if (has(RefObjectKind::Variable))
			r.append({m_style->variable, tr("variable")});
		if (has(RefObjectKind::Container))
			r.append({m_style->container, tr("container")});
		// "data" = any present kind that isn't one of the specific groups above.
		if (hasDataKind())
			r.append({m_style->dataColor, tr("data")});
		return r;
	}

	// True if any present kind maps to the generic "data" color (goal, team,
	// event name, and anything else not in the specific groups).
	bool hasDataKind() const
	{
		return std::any_of(m_kinds.begin(), m_kinds.end(), [](int k) {
			switch (static_cast<RefObjectKind>(k)) {
			case RefObjectKind::Unknown:
			case RefObjectKind::Ship:
			case RefObjectKind::Wing:
			case RefObjectKind::Prop:
			case RefObjectKind::Waypoint:
			case RefObjectKind::JumpNode:
			case RefObjectKind::CoordinatePoint:
			case RefObjectKind::Message:
			case RefObjectKind::Variable:
			case RefObjectKind::Container:
				return false;
			default:
				return true;
			}
		});
	}

	static constexpr int kTop = 22;
	static constexpr int kRowH = 18;
	static constexpr int kColW = 132; // per-column width (colors | icons)
	const EventGraphStyle* m_style;
	QSet<int> m_kinds;
};

// Rounded background for the top-left control strip. Like the legend, it consumes
// mouse events so the gaps between the controls don't start a viewport pan.
class OverlayPanel final : public QWidget {
  public:
	OverlayPanel(const EventGraphStyle* style, QWidget* parent) : QWidget(parent), m_style(style)
	{
		setCursor(Qt::ArrowCursor);
	}

  protected:
	void mousePressEvent(QMouseEvent* e) override { e->accept(); }
	void mouseMoveEvent(QMouseEvent* e) override { e->accept(); }
	void mouseReleaseEvent(QMouseEvent* e) override { e->accept(); }

	void paintEvent(QPaintEvent*) override
	{
		QPainter p(this);
		p.setRenderHint(QPainter::Antialiasing, true);
		const QRectF frame = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
		p.setPen(QPen(m_style->cardBorder, 1.0));
		p.setBrush(m_style->bgColor);
		p.drawRoundedRect(frame, 5.0, 5.0);
	}

  private:
	const EventGraphStyle* m_style;
};

} // namespace graphdetail

// ---------------------------------------------------------------------------
// View
// ---------------------------------------------------------------------------

EventGraphView::EventGraphView(QWidget* parent) : QGraphicsView(parent)
{
	const bool dark = qApp->palette().window().color().value() < 128;
	m_style = EventGraphStyle::makeStyle(dark);

	m_scene = new QGraphicsScene(this);
	setScene(m_scene);

	// Selecting an event card syncs the selection outward (e.g. to the tree).
	// Only emit for a genuine event selection so a rebuild's clear() doesn't
	// clobber the outward selection with -1.
	connect(m_scene, &QGraphicsScene::selectionChanged, this, [this] {
		if (m_suppressSelectionSignal)
			return; // programmatic (re)selection during a rebuild
		applyEmphasis(); // update on-select edges / focus fade for the new selection
		for (QGraphicsItem* it : m_scene->selectedItems()) {
			if (auto* ev = qgraphicsitem_cast<graphdetail::EventNodeItem*>(it)) {
				Q_EMIT eventSelected(ev->eventIndex());
				break;
			}
			if (auto* node = qgraphicsitem_cast<graphdetail::SexpNodeItem*>(it)) {
				Q_EMIT eventSelected(node->eventIndex()); // sync-select the containing event
				break;
			}
		}
		// The event-property controls are only meaningful for an event card, so
		// tell the dialog to re-evaluate on every selection change (this fires for
		// object / empty selections too, which emit no eventSelected).
		Q_EMIT graphSelectionChanged();
	});

	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	setCacheMode(QGraphicsView::CacheBackground); // grid is static; don't redraw it every frame
	setBackgroundBrush(m_style.bgColor);
	setFrameShape(QFrame::NoFrame);
	setAlignment(Qt::AlignCenter);
	setFocusPolicy(Qt::StrongFocus); // receive Esc to clear the swimlanes filter
	applyModeViewport();

	buildOverlay();

	m_minimap = new graphdetail::MinimapWidget(this, m_scene, &m_style, viewport());
	m_minimap->raise();
	m_legend = new graphdetail::LegendWidget(&m_style, viewport());
	m_legend->raise();
	positionOverlay();

	qApp->installEventFilter(this);
}

EventGraphView::~EventGraphView()
{
	// The scene (a child) is torn down after this body, deleting its cards. If one
	// was selected, that fires selectionChanged -> graphSelectionChanged into the
	// already-destructing parent dialog (a use-after-destroy assert). Sever the
	// scene's connections to this view first so nothing echoes out during teardown.
	if (m_scene)
		disconnect(m_scene, nullptr, this, nullptr);
}

bool EventGraphView::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::ApplicationPaletteChange)
		applyTheme(qApp->palette().window().color().value() < 128);
	return QGraphicsView::eventFilter(watched, event);
}

void EventGraphView::buildOverlay()
{
	// A rounded background panel that also swallows mouse events (so the gaps
	// between controls never start a viewport pan) and shows the normal pointer.
	m_overlay = new graphdetail::OverlayPanel(&m_style, viewport());
	auto* lay = new QHBoxLayout(m_overlay);
	lay->setContentsMargins(8, 6, 8, 6);
	lay->setSpacing(4);

	// Settings gear (leading control) -- global + per-mode graph settings. The
	// menu is rebuilt on each open so it reflects the current mode.
	m_settingsMenu = new QMenu(this);
	connect(m_settingsMenu, &QMenu::aboutToShow, this, &EventGraphView::rebuildSettingsMenu);
	auto* gear = new QToolButton(m_overlay);
	gear->setToolTip(tr("Graph settings"));
	gear->setPopupMode(QToolButton::InstantPopup);
	gear->setMenu(m_settingsMenu);
	bindCustomIcon(gear, CustomIcon::Settings);
	lay->addWidget(gear);

	m_modeGroup = new QButtonGroup(this);
	m_modeGroup->setExclusive(true);

	auto addModeButton = [&](const QString& label, Mode mode, bool enabled, const QString& tip) {
		auto* btn = new QToolButton(m_overlay);
		btn->setText(label);
		btn->setCheckable(true);
		btn->setChecked(mode == m_mode);
		btn->setEnabled(enabled);
		if (!tip.isEmpty())
			btn->setToolTip(tip);
		m_modeGroup->addButton(btn, static_cast<int>(mode));
		lay->addWidget(btn);
	};
	addModeButton(tr("Basic"), Mode::Basic, true, tr("Whole-mission dataflow graph (drag to arrange)"));
	addModeButton(tr("Radial"), Mode::Radial, true, QString());
	addModeButton(tr("Swimlanes"), Mode::Swimlanes, true, QString());

	connect(m_modeGroup, &QButtonGroup::idClicked, this, [this](int id) { setMode(static_cast<Mode>(id)); });

	auto* sep = new QLabel(QStringLiteral("  "), m_overlay);
	lay->addWidget(sep);

	m_objectCombo = new QComboBox(m_overlay);
	m_objectCombo->setMinimumWidth(180);
	m_objectCombo->setToolTip(tr("Object to inspect"));
	lay->addWidget(m_objectCombo);

	connect(m_objectCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
		if (m_mode == Mode::Radial) {
			rebuildRadial();
		} else if (m_mode == Mode::Swimlanes) {
			bool ok = false;
			const int k = m_objectCombo->currentData().toInt(&ok);
			m_swimKind = (ok && k >= 0) ? static_cast<RefObjectKind>(k) : RefObjectKind::Unknown;
			clearSwimFocus(); // a focused object may be filtered out by the kind change
			m_hasFramed = false;
			rebuildSwimlanes();
		} else if (m_mode == Mode::Basic) {
			bool ok = false;
			const int v = m_objectCombo->currentData().toInt(&ok);
			if (ok)
				s_basicLayout = static_cast<BasicLayout>(v);
			m_hasFramed = false; // re-fit to the new arrangement
			rebuildBasic();
		}
	});

	positionOverlay();
}

// Rebuilt each time the gear is opened so it shows only the settings that apply
// to the current mode (plus the shared General section).
void EventGraphView::rebuildSettingsMenu()
{
	m_settingsMenu->clear();

	// --- General (all modes): a checkbox panel, so toggling keeps the menu open ---
	m_settingsMenu->addSection(tr("General"));
	{
		auto* panel = new QWidget(m_settingsMenu);
		auto* col = new QVBoxLayout(panel);
		col->setContentsMargins(12, 4, 12, 6);
		col->setSpacing(4);

		auto addCheck = [&](const QString& label, bool checked, const std::function<void(bool)>& onToggle) {
			auto* cb = new QCheckBox(label, panel);
			cb->setChecked(checked);
			connect(cb, &QCheckBox::toggled, this, [onToggle](bool on) { onToggle(on); });
			col->addWidget(cb);
		};

		// Reference-line visibility: always / only for the selected node / off.
		col->addWidget(new QLabel(tr("Reference lines"), panel));
		auto* refGroup = new QButtonGroup(panel);
		auto addRadio = [&](const QString& label, RefLineMode mode) {
			auto* rb = new QRadioButton(label, panel);
			rb->setChecked(s_refMode == mode);
			refGroup->addButton(rb);
			col->addWidget(rb);
			connect(rb, &QRadioButton::toggled, this, [this, mode](bool on) {
				if (on) {
					s_refMode = mode;
					rebuildCurrent();
				}
			});
		};
		addRadio(tr("Always"), RefLineMode::Always);
		addRadio(tr("On selection"), RefLineMode::OnSelect);
		addRadio(tr("Off"), RefLineMode::Off);

		addCheck(tr("Focus: fade non-connected"), s_focusFade, [this](bool on) {
			s_focusFade = on;
			applyEmphasis();
		});
		addCheck(tr("Legend"), s_showLegend, [this](bool on) {
			s_showLegend = on;
			updateChromeVisibility();
		});
		addCheck(tr("Minimap"), s_showMinimap, [this](bool on) {
			s_showMinimap = on;
			updateChromeVisibility();
		});

		auto* wa = new QWidgetAction(m_settingsMenu);
		wa->setDefaultWidget(panel);
		m_settingsMenu->addAction(wa);
	}

	// --- Slider helper (label + value + range) as a QWidgetAction ---
	// fmt formats the value label (also for its initial text). With live off, the
	// label follows the drag but onChange only runs on release, for settings that
	// rebuild the whole graph.
	auto addSlider = [this](const QString& title, int lo, int hi, int val, const std::function<QString(int)>& fmt,
						 bool live, const std::function<void(int, QLabel*)>& onChange) {
		auto* panel = new QWidget(m_settingsMenu);
		auto* col = new QVBoxLayout(panel);
		col->setContentsMargins(10, 4, 10, 6);
		col->addWidget(new QLabel(title, panel));

		auto* row = new QHBoxLayout;
		auto* slider = new QSlider(Qt::Horizontal, panel);
		slider->setRange(lo, hi);
		slider->setValue(val);
		slider->setMinimumWidth(160);
		auto* value = new QLabel(fmt(val), panel);
		value->setMinimumWidth(28);
		row->addWidget(slider);
		row->addWidget(value);
		col->addLayout(row);

		slider->setTracking(live);
		if (!live)
			connect(slider, &QSlider::sliderMoved, value, [value, fmt](int v) { value->setText(fmt(v)); });
		connect(slider, &QSlider::valueChanged, this,
			[value, onChange](int v) { onChange(v, value); });

		auto* wa = new QWidgetAction(m_settingsMenu);
		wa->setDefaultWidget(panel);
		m_settingsMenu->addAction(wa);
	};

	// --- Mode-specific ---
	if (m_mode == Mode::Radial) {
		m_settingsMenu->addSection(tr("Radial"));
		const auto plain = [](int v) { return QString::number(v); };
		addSlider(tr("Ring spacing"), 120, 600, static_cast<int>(s_ringSpacing), plain, /*live=*/false,
			[this, plain](int v, QLabel* value) {
				s_ringSpacing = v;
				value->setText(plain(v));
				rebuildRadial();
			});
	} else if (m_mode == Mode::Basic) {
		m_settingsMenu->addSection(tr("Basic"));
		{
			auto* panel = new QWidget(m_settingsMenu);
			auto* col = new QVBoxLayout(panel);
			col->setContentsMargins(12, 4, 12, 6);
			auto* combine = new QCheckBox(tr("Combine shared objects"), panel);
			combine->setChecked(s_basicCombineObjects);
			combine->setToolTip(tr("On: one shared node per object.\nOff: duplicate a node beside each operator that uses it."));
			connect(combine, &QCheckBox::toggled, this, [this](bool on) {
				s_basicCombineObjects = on;
				m_hasFramed = false; // topology changed; re-fit
				rebuildBasic();
			});
			col->addWidget(combine);
			auto* wa = new QWidgetAction(m_settingsMenu);
			wa->setDefaultWidget(panel);
			m_settingsMenu->addAction(wa);
		}
		const auto depthLabel = [](int v) { return v >= kBasicDepthAll ? EventGraphView::tr("all") : QString::number(v); };
		addSlider(tr("Sexp depth"), 0, kBasicDepthAll, s_basicDepth, depthLabel, /*live=*/true,
			[this, depthLabel](int v, QLabel* value) {
				s_basicDepth = v;
				value->setText(depthLabel(v));
				rebuildBasic();
			});
	}

	// --- Export (all modes) ---
	m_settingsMenu->addSeparator();
	m_settingsMenu->addAction(tr("Export image..."), this, [this]() { exportImage(); });
}

// Render the whole graph scene to a PNG the user picks. Independent of the
// current zoom/pan; overlays (minimap/legend) are widgets, not scene items, so
// they aren't included.
void EventGraphView::exportImage()
{
	// The empty-state message is itself a scene item, so test for real cards.
	bool hasCards = false;
	if (m_scene) {
		for (QGraphicsItem* gi : m_scene->items()) {
			if (dynamic_cast<graphdetail::CardItem*>(gi)) {
				hasCards = true;
				break;
			}
		}
	}
	if (!hasCards) {
		QMessageBox::information(this, tr("Export Graph Image"), tr("There is nothing in the graph to export."));
		return;
	}

	const QString path = QFileDialog::getSaveFileName(this, tr("Export Graph Image"),
		QStringLiteral("events-graph.png"), tr("PNG Image (*.png)"));
	if (path.isEmpty())
		return;

	QRectF rect = m_scene->itemsBoundingRect().adjusted(-24, -24, 24, 24);
	if (rect.isEmpty())
		return;

	// 2x for crisp text, capped so a huge graph doesn't blow up memory.
	qreal scale = 2.0;
	const qreal longest = std::max(rect.width(), rect.height()) * scale;
	const qreal maxDim = 8000.0;
	if (longest > maxDim)
		scale *= maxDim / longest;

	QImage image(QSize(qCeil(rect.width() * scale), qCeil(rect.height() * scale)), QImage::Format_ARGB32);
	if (image.isNull()) {
		// Up to 8000x8000 ARGB is ~256 MB, which can fail to allocate.
		QMessageBox::warning(this, tr("Export Graph Image"), tr("Not enough memory to render the graph image."));
		return;
	}
	image.fill(m_style.bgColor);
	QPainter painter(&image);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	m_scene->render(&painter, QRectF(image.rect()), rect);
	painter.end();
	if (!image.save(path, "PNG"))
		QMessageBox::warning(this, tr("Export Graph Image"), tr("Could not save the image to:\n%1").arg(path));
}

// Keep the overlay hugging the top-left corner, sized to its (variable-width)
// contents, and on top of the scene. Called on build, resize, show, and after
// the selector is repopulated.
void EventGraphView::positionOverlay()
{
	if (m_overlay) {
		m_overlay->adjustSize();
		m_overlay->move(8, 8);
		m_overlay->raise();
		m_overlay->show();
	}
	if (m_minimap) {
		m_minimap->move(8, viewport()->height() - m_minimap->height() - 8);
		m_minimap->raise();
		// Visibility only. This runs on every scroll step, and the minimap draws the
		// viewport rectangle live in its paintEvent; its cached scene render is
		// refreshed on real content changes (rebuilds, drags, card expand) and when
		// the minimap is switched back on (updateChromeVisibility).
		m_minimap->setVisible(s_showMinimap);
	}
	if (m_legend) {
		m_legend->move(viewport()->width() - m_legend->width() - 8, 8);
		m_legend->raise();
		m_legend->setVisible(s_showLegend);
	}
}

void EventGraphView::updateChromeVisibility()
{
	if (m_minimap) {
		m_minimap->setVisible(s_showMinimap);
		if (s_showMinimap)
			m_minimap->regenerate(); // regenerates were skipped while hidden; refresh now
	}
	if (m_legend)
		m_legend->setVisible(s_showLegend);
}

void EventGraphView::setObjects(QVector<GraphObject> objects)
{
	// The combo stores m_objects indices, and a new list can reorder them (a ship
	// added in the viewport shifts everything after it). Capture the current
	// object against the old list before replacing it.
	const int row = selectedObjectRow();
	if (row >= 0) {
		m_selectorIdentity = m_objects[row];
		m_hasSelectorIdentity = true;
	}
	m_objects = std::move(objects);
}

int EventGraphView::selectedObjectRow() const
{
	if (!m_objectCombo || m_objectCombo->currentIndex() < 0)
		return -1;
	bool ok = false;
	const int idx = m_objectCombo->currentData().toInt(&ok); // stored m_objects index
	return (ok && idx >= 0 && idx < m_objects.size()) ? idx : -1;
}

// Rebuild the combo from m_objects, listing only objects that are actually
// referenced (with live counts) and keeping the current object selected where
// it still exists. Each item stores its m_objects index as user data, so combo
// rows need not align 1:1 with m_objects.
void EventGraphView::populateSelector()
{
	auto countOf = [this](const GraphObject& o) {
		return m_index ? m_index->eventReferenceCount(o.kind, SCP_string(o.name.toUtf8().constData())) : 0;
	};

	// Remember the current object's identity so a rebuild doesn't reset it. If
	// setObjects() replaced the list since, the combo's indices point into the old
	// one, so use the identity it captured instead.
	GraphObject prev;
	bool hadPrev = false;
	if (m_hasSelectorIdentity) {
		prev = m_selectorIdentity;
		hadPrev = true;
		m_hasSelectorIdentity = false;
	} else {
		const int prevIdx = selectedObjectRow();
		if (prevIdx >= 0) {
			prev = m_objects[prevIdx];
			hadPrev = true;
		}
	}

	QSignalBlocker blocker(m_objectCombo);
	m_objectCombo->clear();

	int restoreRow = -1;
	for (int i = 0; i < m_objects.size(); ++i) {
		const GraphObject& obj = m_objects[i];
		const int count = countOf(obj);
		if (count <= 0)
			continue; // an unreferenced object would just clutter the list

		const int comboRow = m_objectCombo->count();
		m_objectCombo->addItem(QStringLiteral("%1  (%2, %3 ref%4)")
			.arg(obj.name)
			.arg(QString::fromLatin1(EventReferenceIndex::kindLabel(obj.kind)))
			.arg(count)
			.arg(count == 1 ? QString() : QStringLiteral("s")), QVariant(i));

		if (hadPrev && obj.kind == prev.kind && obj.name == prev.name)
			restoreRow = comboRow;
	}

	const int target = (restoreRow >= 0) ? restoreRow : (m_objectCombo->count() > 0 ? 0 : -1);
	if (target >= 0)
		m_objectCombo->setCurrentIndex(target);
}

// The combo drives the object (radial) or the kind filter (swimlanes); the Basic
// view is whole-mission, so the combo is a disabled placeholder there.
void EventGraphView::populateSelectorForMode()
{
	if (m_mode == Mode::Swimlanes) {
		populateKindFilter();
		m_objectCombo->setToolTip(tr("Filter by object kind"));
	} else if (m_mode == Mode::Radial) {
		populateSelector();
		m_objectCombo->setToolTip(tr("Object to inspect"));
	} else {
		// Basic: the combo picks the node arrangement.
		QSignalBlocker blocker(m_objectCombo);
		m_objectCombo->clear();
		m_objectCombo->addItem(tr("Custom"), static_cast<int>(BasicLayout::Custom));
		m_objectCombo->addItem(tr("Auto layout"), static_cast<int>(BasicLayout::Auto));
		m_objectCombo->addItem(tr("Compact"), static_cast<int>(BasicLayout::Compact));
		const int row = m_objectCombo->findData(static_cast<int>(s_basicLayout));
		m_objectCombo->setCurrentIndex(row >= 0 ? row : 0);
		m_objectCombo->setToolTip(tr("Node layout"));
	}
	m_objectCombo->setEnabled(true);
}

void EventGraphView::reload()
{
	populateSelectorForMode();
	// The legend lists only the object kinds that actually occur in the events.
	if (m_legend) {
		QSet<int> kinds;
		if (m_index)
			for (const EventObjectRef& r : m_index->allReferences())
				kinds.insert(static_cast<int>(r.kind));
		m_legend->setPresentKinds(kinds);
	}
	positionOverlay(); // the combo width changes with the labels; legend size may change too
	rebuildCurrent();
}

// All modes use RubberBandDrag (left-drag on empty = box select) and pan manually
// (middle / right / shift+left), matching the main viewport. The cursor is the
// normal arrow unless panning (closed hand) or Shift is held (open hand).
void EventGraphView::applyModeViewport()
{
	// Left-drag on empty canvas rubber-band selects; cards drag/select normally.
	// Panning is handled manually (middle / right / shift+left), matching the
	// viewport. Left-drag on a movable card still moves it.
	setDragMode(QGraphicsView::RubberBandDrag);
	updatePanCursor();
}

void EventGraphView::updatePanCursor()
{
	if (m_panning && m_panMoved)
		viewport()->setCursor(Qt::ClosedHandCursor);
	else if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
		viewport()->setCursor(Qt::OpenHandCursor);
	else
		viewport()->unsetCursor();
}

void EventGraphView::setMode(Mode mode)
{
	if (mode == m_mode)
		return;

	// Each mode keeps its own view: remember the current mode's zoom/center,
	// selection, and object/filter settings before switching, then restore the
	// target mode's remembered state below.
	saveModeMemory();
	m_scene->clearSelection(); // the rebuild shouldn't re-guess from the old scene

	m_mode = mode;
	m_panning = false;
	applyModeViewport();

	const GraphModeMemory& mem = m_modeMemory[static_cast<int>(mode)];

	// Restore this mode's filters before the rebuild so it reproduces the same
	// content the mode last showed.
	if (mem.valid && mode == Mode::Swimlanes) {
		m_swimKind = mem.swimKind;
		m_focusObjects = mem.focusObjects;
		m_focusEvents = mem.focusEvents;
	}
	populateSelectorForMode();
	const bool selectorRestored = mem.valid && restoreSelectorSelection(mode, mem);

	positionOverlay();

	m_hasFramed = false; // rebuild fits the whole graph; we override below if remembered
	rebuildCurrent();

	// Resume the remembered viewport + selection for this mode. Radial only if the
	// remembered object still exists (otherwise keep the fresh fit-all).
	const bool restoreView = mem.valid && (mode != Mode::Radial || selectorRestored);
	if (restoreView) {
		setTransform(mem.transform);
		centerOn(mem.center);
		m_currentScale = transform().m11();
		m_hasFramed = true;
		switch (mode) {
		case Mode::Radial:
			m_framedKey = mem.radialObjectKey;
			break;
		case Mode::Swimlanes:
			m_framedKey = QStringLiteral("swim");
			break;
		case Mode::Basic:
			m_framedKey = QStringLiteral("basic");
			break;
		}
		applySavedSelection(mem);
	}

	Q_EMIT modeChanged();
}

// Snapshot the mode we're leaving so returning to it resumes where we left off.
void EventGraphView::saveModeMemory()
{
	GraphModeMemory& mem = m_modeMemory[static_cast<int>(m_mode)];
	mem.valid = true;
	mem.transform = transform();
	mem.center = mapToScene(viewport()->rect().center());

	mem.selKind = 0;
	mem.selEvent = -1;
	mem.selOp.clear();
	mem.selCond = false;
	for (QGraphicsItem* gi : m_scene->selectedItems()) {
		if (auto* nd = qgraphicsitem_cast<graphdetail::SexpNodeItem*>(gi)) {
			mem.selKind = 2;
			mem.selEvent = nd->eventIndex();
			mem.selOp = nd->titleText();
			mem.selCond = nd->isCond();
			break;
		}
		if (auto* ev = qgraphicsitem_cast<graphdetail::EventNodeItem*>(gi)) {
			mem.selKind = 1;
			mem.selEvent = ev->eventIndex();
			break;
		}
	}

	if (m_mode == Mode::Radial) {
		const int row = selectedObjectRow();
		mem.radialObjectKey = (row >= 0)
			? QStringLiteral("%1|%2").arg(static_cast<int>(m_objects[row].kind)).arg(m_objects[row].name)
			: QString();
	} else if (m_mode == Mode::Swimlanes) {
		mem.swimKind = m_swimKind;
		mem.focusObjects = m_focusObjects;
		mem.focusEvents = m_focusEvents;
	}
}

// Point the selector at the mode's remembered subject (blocked so it doesn't
// trigger its own rebuild). Returns false for Radial when the object is gone.
bool EventGraphView::restoreSelectorSelection(Mode mode, const GraphModeMemory& mem)
{
	if (!m_objectCombo)
		return false;
	QSignalBlocker blocker(m_objectCombo);
	if (mode == Mode::Radial) {
		if (mem.radialObjectKey.isEmpty())
			return false;
		for (int i = 0; i < m_objectCombo->count(); ++i) {
			bool ok = false;
			const int idx = m_objectCombo->itemData(i).toInt(&ok);
			if (!ok || idx < 0 || idx >= m_objects.size())
				continue;
			const QString key =
				QStringLiteral("%1|%2").arg(static_cast<int>(m_objects[idx].kind)).arg(m_objects[idx].name);
			if (key == mem.radialObjectKey) {
				m_objectCombo->setCurrentIndex(i);
				return true;
			}
		}
		return false;
	}
	if (mode == Mode::Swimlanes) {
		const int row = m_objectCombo->findData(static_cast<int>(mem.swimKind));
		m_objectCombo->setCurrentIndex(row >= 0 ? row : 0);
		return true;
	}
	return false; // Basic: the combo is the layout dropdown, nothing to restore
}

// Re-select the mode's remembered card on the freshly rebuilt scene.
void EventGraphView::applySavedSelection(const GraphModeMemory& mem)
{
	if (mem.selKind == 0)
		return;
	for (QGraphicsItem* gi : m_scene->items()) {
		if (mem.selKind == 1) {
			auto* ev = qgraphicsitem_cast<graphdetail::EventNodeItem*>(gi);
			if (ev && ev->eventIndex() == mem.selEvent) {
				ev->setSelected(true);
				return;
			}
		} else if (mem.selKind == 2) {
			auto* nd = qgraphicsitem_cast<graphdetail::SexpNodeItem*>(gi);
			if (nd && nd->eventIndex() == mem.selEvent && nd->isCond() == mem.selCond
				&& nd->titleText() == mem.selOp) {
				nd->setSelected(true);
				return;
			}
		}
	}
}

// Swimlanes: fill the combo with "All object types" + each kind that is
// actually referenced. Selecting a kind filters the rows to it.
void EventGraphView::populateKindFilter()
{
	if (!m_objectCombo)
		return;
	QSignalBlocker blocker(m_objectCombo);
	m_objectCombo->clear();
	m_objectCombo->addItem(tr("All object types"), -1);

	// Distinct kinds present, in enum order.
	QVector<int> present;
	if (m_index) {
		for (const EventObjectRef& r : m_index->allReferences()) {
			const int k = static_cast<int>(r.kind);
			if (!present.contains(k))
				present.push_back(k);
		}
	}
	std::sort(present.begin(), present.end());
	for (int k : present)
		m_objectCombo->addItem(QString::fromLatin1(EventReferenceIndex::kindLabel(static_cast<RefObjectKind>(k))), k);

	// Restore the current kind filter if still present.
	int row = 0;
	if (m_swimKind != RefObjectKind::Unknown) {
		const int idx = m_objectCombo->findData(static_cast<int>(m_swimKind));
		if (idx >= 0)
			row = idx;
		else
			m_swimKind = RefObjectKind::Unknown;
	}
	m_objectCombo->setCurrentIndex(row);
}

void EventGraphView::toggleSwimObjectFocus(const QString& objectKey, bool add)
{
	if (add) {
		if (m_focusObjects.contains(objectKey))
			m_focusObjects.remove(objectKey);
		else
			m_focusObjects.insert(objectKey);
	} else {
		const bool wasSole = m_focusEvents.isEmpty() && m_focusObjects.size() == 1
			&& m_focusObjects.contains(objectKey);
		m_focusObjects.clear();
		m_focusEvents.clear();
		if (!wasSole)
			m_focusObjects.insert(objectKey);
	}
}

void EventGraphView::toggleSwimEventFocus(int eventIndex, bool add)
{
	if (add) {
		if (m_focusEvents.contains(eventIndex))
			m_focusEvents.remove(eventIndex);
		else
			m_focusEvents.insert(eventIndex);
	} else {
		const bool wasSole = m_focusObjects.isEmpty() && m_focusEvents.size() == 1
			&& m_focusEvents.contains(eventIndex);
		m_focusObjects.clear();
		m_focusEvents.clear();
		if (!wasSole)
			m_focusEvents.insert(eventIndex);
	}
}

void EventGraphView::clearSwimFocus()
{
	m_focusObjects.clear();
	m_focusEvents.clear();
}

void EventGraphView::rebuildCurrent()
{
	if (m_mode == Mode::Swimlanes)
		rebuildSwimlanes();
	else if (m_mode == Mode::Basic)
		rebuildBasic();
	else
		rebuildRadial();
}

// The default scene rect grows to the union of everything ever shown and never
// shrinks, so a large swimlanes layout would leave a later radial/basic scene
// stuck against a corner (and mismatched with the minimap). Pin it to the
// current content with a margin of panning slack.
void EventGraphView::updateSceneRect()
{
	if (!m_scene)
		return;
	const QRectF b = m_scene->itemsBoundingRect();
	m_scene->setSceneRect(b.isEmpty() ? QRectF() : b.adjusted(-400, -400, 400, 400));
}

void EventGraphView::showEmptyMessage(const QString& text)
{
	auto* label = m_scene->addSimpleText(text);
	label->setBrush(m_style.nodeSubText);
	label->setPos(-label->boundingRect().width() / 2, 60.0);
	updateSceneRect();
	zoomToFitAll();
	if (m_minimap)
		m_minimap->regenerate(); // content changed -> re-render the cached overview
}

void EventGraphView::setSearchText(const QString& text)
{
	if (m_searchText == text)
		return;
	m_searchText = text;
	m_matchIndex = -1; // restart next/prev navigation for the new query
	applyEmphasis();
}

// Step through the matching cards in reading order (top-to-bottom, then
// left-to-right), panning and zooming to each and selecting it.
void EventGraphView::focusNextMatch(bool forward)
{
	if (m_searchText.isEmpty())
		return;

	QVector<graphdetail::CardItem*> matches;
	for (QGraphicsItem* it : m_scene->items()) {
		auto* c = dynamic_cast<graphdetail::CardItem*>(it);
		if (c && c->matchesQuery(m_searchText))
			matches.push_back(c);
	}
	if (matches.isEmpty())
		return;
	std::sort(matches.begin(), matches.end(), [](graphdetail::CardItem* a, graphdetail::CardItem* b) {
		const QPointF pa = a->pos(), pb = b->pos();
		if (!qFuzzyCompare(pa.y(), pb.y()))
			return pa.y() < pb.y();
		return pa.x() < pb.x();
	});

	const int n = matches.size();
	if (m_matchIndex < 0)
		m_matchIndex = forward ? 0 : n - 1;
	else
		m_matchIndex = ((m_matchIndex + (forward ? 1 : -1)) % n + n) % n;
	graphdetail::CardItem* cur = matches[m_matchIndex];

	// Pan + zoom so the match sits centered with a little surrounding context.
	fitInView(cur->sceneBoundingRect().adjusted(-260, -180, 260, 180), Qt::KeepAspectRatio);
	m_currentScale = transform().m11();

	// Select it: an orange border marks the current match and syncs the event out.
	m_scene->clearSelection();
	cur->setSelected(true);
	if (m_minimap)
		m_minimap->update();
}

// Node emphasis: search highlight (all modes) plus reference-line on-select
// visibility and focus-fade dimming (radial/basic, which have node edges;
// swimlanes has its own cross-filter focus). Runs after each rebuild and on
// every real selection change.
void EventGraphView::applyEmphasis()
{
	const bool searchActive = !m_searchText.isEmpty();
	auto matches = [this](QGraphicsItem* it) {
		auto* c = dynamic_cast<graphdetail::CardItem*>(it);
		return c && c->matchesQuery(m_searchText);
	};

	// Selection emphasis only applies where there are node edges to reason about.
	const bool selMode = (m_mode != Mode::Swimlanes);
	QSet<QGraphicsItem*> selected, active;
	QVector<graphdetail::RefEdgeItem*> edges;
	bool hasSel = false;
	if (selMode) {
		for (QGraphicsItem* it : m_scene->selectedItems())
			if (dynamic_cast<graphdetail::CardItem*>(it))
				selected.insert(it);
		hasSel = !selected.isEmpty();
		active = selected;
		for (QGraphicsItem* it : m_scene->items()) {
			auto* e = dynamic_cast<graphdetail::RefEdgeItem*>(it);
			if (!e)
				continue;
			edges.push_back(e);
			if (hasSel && !e->isChain()) {
				if (e->endA() && selected.contains(e->endA()) && e->endB())
					active.insert(e->endB());
				if (e->endB() && selected.contains(e->endB()) && e->endA())
					active.insert(e->endA());
			}
		}
	}

	const bool fade = s_focusFade && hasSel;
	const qreal dim = 0.2;

	for (graphdetail::RefEdgeItem* e : edges) {
		// Chain lines are structure, not object references: always visible, never dimmed.
		if (e->isChain()) {
			e->setVisible(true);
			e->setOpacity(1.0);
			continue;
		}
		const bool incident = hasSel && ((e->endA() && selected.contains(e->endA())) ||
										 (e->endB() && selected.contains(e->endB())));
		const bool visible = (s_refMode != RefLineMode::OnSelect) || incident;
		e->setVisible(visible);
		if (!visible)
			continue;
		// Dim edges the focus or search excludes (search keeps an edge lit if
		// either endpoint matches).
		bool edgeDim = fade && !incident;
		if (searchActive && !matches(e->endA()) && !matches(e->endB()))
			edgeDim = true;
		e->setOpacity(edgeDim ? dim : 1.0);
	}

	for (QGraphicsItem* it : m_scene->items()) {
		auto* c = dynamic_cast<graphdetail::CardItem*>(it);
		if (!c)
			continue;
		bool cardDim = fade && !active.contains(it);
		if (searchActive && !c->matchesQuery(m_searchText))
			cardDim = true;
		c->setOpacity(cardDim ? dim : 1.0);
	}
}

// Apply a node's comment/color annotation (looked up by key) to a card.
void EventGraphView::applyAnnotation(graphdetail::CardItem* card, int key)
{
	if (!card)
		return;
	card->setAnnotationKey(key); // record even when unset, so the menu can add one
	card->setOrder(m_siblingOrders.value(key, 0)); // corner order marker (0 = no parent)
	const auto it = m_annotations.constFind(key);
	if (it != m_annotations.constEnd())
		card->setAnnotation(it->color, it->comment);
}

// Apply the event status icons (and the event's own annotation) to an event card.
void EventGraphView::applyEventStatus(graphdetail::EventNodeItem* card, int eventIndex)
{
	if (!card || eventIndex < 0 || eventIndex >= m_eventMeta.size())
		return;
	const GraphEventMeta& m = m_eventMeta[eventIndex];
	card->setStatusIcons(m.chained, m.directive, m.repeats, m.logging);
	applyAnnotation(card, m.annotationKey);
	card->setOrder(eventIndex + 1); // events are top-level siblings; show their order
}

bool EventGraphView::isEventNodeSelected() const
{
	const auto sel = m_scene->selectedItems();
	return std::any_of(sel.begin(), sel.end(),
		[](QGraphicsItem* it) { return qgraphicsitem_cast<graphdetail::EventNodeItem*>(it) != nullptr; });
}

// Swimlanes shows an operator card once per object row, so a key can have several
// cards. They are the same node, so both of these act on every copy.
int EventGraphView::selectedEventIndex() const
{
	for (QGraphicsItem* it : m_scene->selectedItems())
		if (auto* ev = qgraphicsitem_cast<graphdetail::EventNodeItem*>(it))
			return ev->eventIndex();
	return -1;
}

int EventGraphView::selectedEventCount() const
{
	QSet<int> events;
	for (QGraphicsItem* it : m_scene->selectedItems())
		if (auto* ev = qgraphicsitem_cast<graphdetail::EventNodeItem*>(it))
			events.insert(ev->eventIndex());
	return static_cast<int>(events.size());
}

bool EventGraphView::nodeExpandable(int key) const
{
	if (key == -1)
		return false;
	const auto all = m_scene->items();
	return std::any_of(all.begin(), all.end(), [key](QGraphicsItem* gi) {
		auto* c = dynamic_cast<graphdetail::CardItem*>(gi);
		return c && c->annotationKey() == key && c->canExpand();
	});
}

void EventGraphView::expandNode(int key)
{
	bool expanded = false;
	for (QGraphicsItem* gi : m_scene->items()) {
		if (auto* c = dynamic_cast<graphdetail::CardItem*>(gi); c && c->annotationKey() == key && c->canExpand()) {
			c->setExpanded(true);
			expanded = true;
		}
	}
	if (expanded && m_minimap)
		m_minimap->regenerate(); // cards changed size
}

// Pan/zoom to an event's card and select it (e.g. right after creating it).
void EventGraphView::focusEvent(int eventIndex)
{
	const auto it = m_eventCards.constFind(eventIndex);
	if (it == m_eventCards.constEnd())
		return;
	graphdetail::EventNodeItem* card = it.value();
	fitInView(card->sceneBoundingRect().adjusted(-260, -180, 260, 180), Qt::KeepAspectRatio);
	m_currentScale = transform().m11();
	m_scene->clearSelection();
	card->setSelected(true);
	if (m_minimap)
		m_minimap->update();
}

// Lightweight refresh after an event-property edit: update the one card's icons
// (its annotation is unchanged) and redraw the chain lines (chained may have
// toggled). No full rebuild, so positions and the rest of the scene are kept.
void EventGraphView::updateEventCard(int eventIndex, GraphEventMeta meta)
{
	if (eventIndex >= 0 && eventIndex < m_eventMeta.size()) {
		// This runs after every event-UI refresh, including plain selection clicks.
		// Nothing to redraw unless the event's status actually changed.
		const GraphEventMeta& old = m_eventMeta[eventIndex];
		if (old.chained == meta.chained && old.directive == meta.directive && old.repeats == meta.repeats &&
			old.logging == meta.logging && old.annotationKey == meta.annotationKey)
			return;
		m_eventMeta[eventIndex] = meta;
	}
	const auto it = m_eventCards.constFind(eventIndex);
	if (it != m_eventCards.constEnd())
		it.value()->setStatusIcons(meta.chained, meta.directive, meta.repeats, meta.logging);
	refreshChainLines();
	if (m_minimap)
		m_minimap->regenerate(); // icons/chain changed -- refresh the overview
}

// Remove and redraw the event-chain lines from the current event cards + meta.
// Skipped in radial (it doesn't show every event, so a chain target is often
// absent). A chained event links back to the previous event.
void EventGraphView::refreshChainLines()
{
	if (m_mode == Mode::Radial)
		return;

	QVector<graphdetail::RefEdgeItem*> stale;
	for (QGraphicsItem* it : m_scene->items())
		if (auto* e = dynamic_cast<graphdetail::RefEdgeItem*>(it); e && e->isChain())
			stale.push_back(e);
	for (graphdetail::RefEdgeItem* e : stale) {
		e->detachEndpoints(); // cards persist here; drop their dangling refs first
		m_scene->removeItem(e);
		delete e;
	}

	for (int ev = 1; ev < m_eventMeta.size(); ++ev) {
		if (!m_eventMeta[ev].chained)
			continue;
		const auto a = m_eventCards.constFind(ev - 1);
		const auto b = m_eventCards.constFind(ev);
		if (a == m_eventCards.constEnd() || b == m_eventCards.constEnd())
			continue;
		auto* edge = new graphdetail::RefEdgeItem(a.value()->pos(), b.value()->pos(), m_style.chainColor, m_style,
			/*chain=*/true);
		edge->setEndpoints(a.value(), b.value());
		m_scene->addItem(edge);
	}
}

void EventGraphView::rebuildRadial()
{
	// Capture the current viewport and selection so a rebuild (after an edit,
	// or on return from another view) can resume where we left off instead of
	// re-fitting the whole graph.
	const QTransform savedTransform = transform();
	const QPointF savedCenter = mapToScene(viewport()->rect().center());
	int selKind = 0; // 0 none, 1 event, 2 node
	int selEvent = -1;
	QString selOp;
	bool selCond = false;
	for (QGraphicsItem* gi : m_scene->selectedItems()) {
		if (auto* nd = qgraphicsitem_cast<graphdetail::SexpNodeItem*>(gi)) {
			selKind = 2; selEvent = nd->eventIndex(); selOp = nd->titleText(); selCond = nd->isCond();
			break;
		}
		if (auto* ev = qgraphicsitem_cast<graphdetail::EventNodeItem*>(gi)) {
			selKind = 1; selEvent = ev->eventIndex();
			break;
		}
	}

	m_suppressSelectionSignal = true; // programmatic selection below shouldn't echo out
	m_scene->clear();
	m_eventCards.clear(); // pointers just died with the scene

	const int row = selectedObjectRow();
	if (row < 0 || !m_index) {
		m_suppressSelectionSignal = false;
		showEmptyMessage(tr("No object selected."));
		return;
	}

	const GraphObject& obj = m_objects[row];
	const SCP_string objName(obj.name.toUtf8().constData());
	const SCP_vector<EventObjectRef> sites = m_index->referenceSites(obj.kind, objName);

	// Group the referencing sites by event, split into condition / action.
	QVector<int> eventOrder; // distinct events in first-seen order
	struct Bucket { QVector<const EventObjectRef*> conds, actions; };
	QHash<int, Bucket> buckets;
	for (const EventObjectRef& s : sites) {
		if (!buckets.contains(s.eventIndex))
			eventOrder.push_back(s.eventIndex);
		Bucket& b = buckets[s.eventIndex];
		if (s.role == RefRole::Condition)
			b.conds.push_back(&s);
		else
			b.actions.push_back(&s); // actions + anything outside a when-structure
	}

	// Center object card.
	auto* center = new graphdetail::ObjectNodeItem(
		QString::fromLatin1(EventReferenceIndex::kindLabel(obj.kind)), m_style.colorFor(obj.kind), obj.name,
		tr("%1 references in %2 events").arg(static_cast<int>(sites.size())).arg(eventOrder.size()), m_style);
	center->setPos(0, 0);
	center->setZValue(2.0);
	m_scene->addItem(center);

	if (eventOrder.isEmpty()) {
		m_suppressSelectionSignal = false;
		showEmptyMessage(tr("No events reference %1.").arg(obj.name));
		return;
	}

	const int n = eventOrder.size();
	// Give each event enough arc length on the inner ring for its card.
	const qreal rEvent = std::max(250.0, n * (graphdetail::kNodeW + 70.0) / (2.0 * M_PI));
	const qreal rCond = rEvent + s_ringSpacing;
	const qreal rAction = rCond + s_ringSpacing;
	const qreal wedge = 2.0 * M_PI / n;

	auto polar = [](qreal radius, qreal angle) {
		return QPointF(radius * std::cos(angle), radius * std::sin(angle));
	};

	// Place a group of node cards on a ring, spread within their event's wedge.
	auto placeRing = [&](const QVector<const EventObjectRef*>& group, qreal centerAngle, qreal radius,
						 const QPointF& from, graphdetail::EventNodeItem* fromItem, const QColor& edgeColor,
						 bool isCond) {
		const int m = group.size();
		for (int j = 0; j < m; ++j) {
			const EventObjectRef* s = group[j];
			const qreal spread = wedge * 0.8;
			const qreal off = (m == 1) ? 0.0 : (j - (m - 1) / 2.0) * (spread / m);
			const QPointF pos = polar(radius, centerAngle + off);

			const QString evName = (s->eventIndex >= 0 && s->eventIndex < m_eventNames.size())
				? m_eventNames[s->eventIndex] : tr("<event %1>").arg(s->eventIndex);
			QVector<QString> argList;
			argList.reserve(static_cast<int>(s->args.size()));
			for (const auto& a : s->args)
				argList.push_back(QString::fromStdString(a));
			auto* card = new graphdetail::SexpNodeItem(s->operatorNode, s->eventIndex, isCond,
				QString::fromStdString(s->operatorText), argList, evName,
				QString::fromStdString(s->expression), m_style);
			card->setPos(pos);
			card->setZValue(1.0);
			if (selKind == 2 && s->eventIndex == selEvent && isCond == selCond
				&& QString::fromStdString(s->operatorText) == selOp)
				card->setSelected(true);
			applyAnnotation(card, s->operatorNode);
			m_scene->addItem(card);
			if (s_refMode != RefLineMode::Off) {
				auto* edge = new graphdetail::RefEdgeItem(from, pos, edgeColor, m_style);
				edge->setEndpoints(fromItem, card);
				m_scene->addItem(edge);
			}
		}
	};

	for (int i = 0; i < n; ++i) {
		const int ev = eventOrder[i];
		const qreal angle = -M_PI / 2.0 + i * wedge;
		const QPointF evPos = polar(rEvent, angle);

		const QString name = (ev >= 0 && ev < m_eventNames.size())
			? m_eventNames[ev] : tr("<event %1>").arg(ev);
		auto* evCard = new graphdetail::EventNodeItem(ev, name, m_style);
		evCard->setPos(evPos);
		evCard->setZValue(1.0);
		if (selKind == 1 && ev == selEvent)
			evCard->setSelected(true);
		applyEventStatus(evCard, ev);
		m_scene->addItem(evCard);
		m_eventCards.insert(ev, evCard);
		if (s_refMode != RefLineMode::Off) {
			auto* edge = new graphdetail::RefEdgeItem(center->pos(), evPos, m_style.entity, m_style);
			edge->setEndpoints(center, evCard);
			m_scene->addItem(edge);
		}

		const Bucket& b = buckets[ev];
		placeRing(b.conds, angle, rCond, evPos, evCard, m_style.condChip, /*isCond=*/true);
		placeRing(b.actions, angle, rAction, evPos, evCard, m_style.actionChip, /*isCond=*/false);
	}

	// Dashed tier guide rings.
	for (qreal r : {rEvent, rCond, rAction}) {
		auto* ring = m_scene->addEllipse(QRectF(-r, -r, 2 * r, 2 * r),
			QPen(m_style.ringColor, 1.0, Qt::DashLine));
		ring->setZValue(-2.0);
	}

	// If we're still on the same object, resume the prior zoom/center; otherwise
	// frame the whole graph.
	updateSceneRect();
	const QString curKey = QStringLiteral("%1|%2").arg(static_cast<int>(obj.kind)).arg(obj.name);
	if (m_hasFramed && curKey == m_framedKey) {
		setTransform(savedTransform);
		centerOn(savedCenter);
		m_currentScale = transform().m11();
	} else {
		zoomToFitAll();
		m_framedKey = curKey;
		m_hasFramed = true;
	}

	m_suppressSelectionSignal = false;
	applyEmphasis();
	if (m_minimap)
		m_minimap->regenerate(); // content changed -> re-render the cached overview
}

void EventGraphView::rebuildSwimlanes()
{
	// Viewport + selection continuity (same pattern as radial).
	const QTransform savedTransform = transform();
	const QPointF savedCenter = mapToScene(viewport()->rect().center());
	int selEvent = -1;
	for (QGraphicsItem* gi : m_scene->selectedItems()) {
		if (auto* ev = qgraphicsitem_cast<graphdetail::EventNodeItem*>(gi)) { selEvent = ev->eventIndex(); break; }
		if (auto* nd = qgraphicsitem_cast<graphdetail::SexpNodeItem*>(gi)) { selEvent = nd->eventIndex(); break; }
	}

	m_suppressSelectionSignal = true;
	m_scene->clear();
	m_eventCards.clear(); // pointers just died with the scene

	if (!m_index) {
		m_suppressSelectionSignal = false;
		showEmptyMessage(tr("No data."));
		return;
	}
	const SCP_vector<EventObjectRef>& refs = m_index->allReferences();
	if (refs.empty()) {
		m_suppressSelectionSignal = false;
		showEmptyMessage(tr("No object references in events."));
		return;
	}

	const bool allKinds = (m_swimKind == RefObjectKind::Unknown);

	// Distinct rows (objects) and columns (events), over the kind-allowed refs.
	QVector<std::pair<RefObjectKind, QString>> rows;
	QHash<QString, int> rowSeen;
	QVector<int> cols;
	QHash<int, int> colSeen;
	for (const EventObjectRef& r : refs) {
		if (!allKinds && r.kind != m_swimKind)
			continue;
		const QString name = QString::fromStdString(r.name);
		const QString rk = swimRowKey(r.kind, name);
		if (!rowSeen.contains(rk)) { rowSeen.insert(rk, 0); rows.push_back({r.kind, name}); }
		if (!colSeen.contains(r.eventIndex)) { colSeen.insert(r.eventIndex, 0); cols.push_back(r.eventIndex); }
	}
	std::sort(rows.begin(), rows.end(), [](const std::pair<RefObjectKind, QString>& a,
											 const std::pair<RefObjectKind, QString>& b) {
		if (a.first != b.first)
			return static_cast<int>(a.first) < static_cast<int>(b.first);
		return a.second.toLower() < b.second.toLower();
	});
	std::sort(cols.begin(), cols.end());

	QVector<QString> rowKeyOf(rows.size());
	QHash<QString, int> rowIndex;
	for (int i = 0; i < rows.size(); ++i) {
		rowKeyOf[i] = swimRowKey(rows[i].first, rows[i].second);
		rowIndex.insert(rowKeyOf[i], i);
	}
	QHash<int, int> colIndex;
	for (int j = 0; j < cols.size(); ++j)
		colIndex.insert(cols[j], j);

	// Gather the referencing operator nodes per (object row, event column),
	// deduped by node. A node that names several objects appears in each object's
	// row (option #1); the column connector ties the copies together.
	struct Site {
		int treeNode = -1;
		int eventIndex = -1;
		bool isCond = false;
		QString opName;
		QVector<QString> args;
		QString expr;
	};
	QHash<qint64, QVector<Site>> cellSites;
	for (const EventObjectRef& r : refs) {
		if (!allKinds && r.kind != m_swimKind)
			continue;
		const int ri = rowIndex.value(swimRowKey(r.kind, QString::fromStdString(r.name)));
		const int ci = colIndex.value(r.eventIndex);
		QVector<Site>& sites = cellSites[static_cast<qint64>(ri) * 1000000 + ci];
		bool dup = false;
		for (const Site& s : sites)
			if (s.treeNode == r.operatorNode) { dup = true; break; }
		if (dup)
			continue;
		Site s;
		s.treeNode = r.operatorNode;
		s.eventIndex = r.eventIndex;
		s.isCond = (r.role == RefRole::Condition);
		s.opName = QString::fromStdString(r.operatorText);
		s.expr = QString::fromStdString(r.expression);
		for (const auto& a : r.args)
			s.args.push_back(QString::fromStdString(a));
		sites.push_back(std::move(s));
	}

	// Cross-filter: a focused object restricts the visible columns to its events;
	// a focused event restricts the visible rows to its objects. The two compose,
	// and rows/columns left empty are dropped.
	const bool objFocus = !m_focusObjects.isEmpty();
	const bool evFocus = !m_focusEvents.isEmpty();
	QSet<int> eventsOfFocusObj;
	QSet<QString> objsOfFocusEv;
	if (objFocus || evFocus) {
		for (auto it = cellSites.constBegin(); it != cellSites.constEnd(); ++it) {
			const int ri = static_cast<int>(it.key() / 1000000);
			const int ci = static_cast<int>(it.key() % 1000000);
			if (objFocus && m_focusObjects.contains(rowKeyOf[ri]))
				eventsOfFocusObj.insert(cols[ci]);
			if (evFocus && m_focusEvents.contains(cols[ci]))
				objsOfFocusEv.insert(rowKeyOf[ri]);
		}
	}
	auto colPasses = [&](int ci) { return !objFocus || eventsOfFocusObj.contains(cols[ci]); };
	auto rowPasses = [&](int ri) { return !evFocus || objsOfFocusEv.contains(rowKeyOf[ri]); };

	QSet<int> rowsWithCell, colsWithCell;
	for (auto it = cellSites.constBegin(); it != cellSites.constEnd(); ++it) {
		const int ri = static_cast<int>(it.key() / 1000000);
		const int ci = static_cast<int>(it.key() % 1000000);
		if (rowPasses(ri) && colPasses(ci)) { rowsWithCell.insert(ri); colsWithCell.insert(ci); }
	}
	QVector<int> keepRows, keepCols;
	for (int i = 0; i < rows.size(); ++i)
		if (rowPasses(i) && rowsWithCell.contains(i)) keepRows.push_back(i);
	for (int j = 0; j < cols.size(); ++j)
		if (colPasses(j) && colsWithCell.contains(j)) keepCols.push_back(j);

	if (keepRows.isEmpty() || keepCols.isEmpty()) {
		m_suppressSelectionSignal = false;
		showEmptyMessage(tr("No references match the current filter (Esc to clear)."));
		return;
	}
	QHash<int, int> rowPos, colPos;
	for (int p = 0; p < keepRows.size(); ++p) rowPos.insert(keepRows[p], p);
	for (int p = 0; p < keepCols.size(); ++p) colPos.insert(keepCols[p], p);

	// Create the node cards per kept cell and measure their stacked height so each
	// row can grow to fit. Cards carry the event-name badge (top-right), same as
	// the other views.
	const qreal leftGutter = 244.0, topGutter = 66.0, colW = 252.0, cardGap = 10.0, rowPad = 16.0, minRowH = 60.0;
	QHash<qint64, QVector<graphdetail::SexpNodeItem*>> cellCards;
	QHash<qint64, qreal> cellHeight;
	QVector<qreal> rowHeight(keepRows.size(), minRowH);
	for (auto it = cellSites.constBegin(); it != cellSites.constEnd(); ++it) {
		const int ri = static_cast<int>(it.key() / 1000000);
		const int ci = static_cast<int>(it.key() % 1000000);
		if (!rowPos.contains(ri) || !colPos.contains(ci))
			continue;
		QVector<graphdetail::SexpNodeItem*> cards;
		qreal h = 0.0;
		for (const Site& s : it.value()) {
			const QString evName = (s.eventIndex >= 0 && s.eventIndex < m_eventNames.size())
				? m_eventNames[s.eventIndex] : QString();
			auto* card = new graphdetail::SexpNodeItem(s.treeNode, s.eventIndex, s.isCond, s.opName, s.args,
				evName, s.expr, m_style);
			applyAnnotation(card, s.treeNode);
			cards.push_back(card);
			h += card->boundingRect().height();
		}
		if (cards.size() > 1)
			h += cardGap * (cards.size() - 1);
		cellCards.insert(it.key(), cards);
		cellHeight.insert(it.key(), h);
		rowHeight[rowPos[ri]] = std::max(rowHeight[rowPos[ri]], h + rowPad);
	}

	// Cumulative row tops (variable heights).
	QVector<qreal> rowTop(keepRows.size());
	qreal yCursor = topGutter;
	for (int p = 0; p < keepRows.size(); ++p) { rowTop[p] = yCursor; yCursor += rowHeight[p]; }
	const qreal contentW = leftGutter + keepCols.size() * colW;
	auto colXc = [&](int pos) { return leftGutter + pos * colW + colW / 2.0; };
	auto rowCenter = [&](int pos) { return rowTop[pos] + rowHeight[pos] / 2.0; };

	// Alternating lane stripes.
	for (int p = 0; p < keepRows.size(); ++p) {
		QColor tint = m_style.cardBorder;
		tint.setAlpha(p % 2 ? 40 : 0);
		auto* stripe = m_scene->addRect(QRectF(0, rowTop[p], contentW, rowHeight[p]), QPen(Qt::NoPen), QBrush(tint));
		stripe->setZValue(-3.0);
	}

	// Position node cards; stack them centered in each row band; track the lowest
	// card per column for the connector line.
	QHash<int, qreal> colBottom;
	for (auto it = cellCards.constBegin(); it != cellCards.constEnd(); ++it) {
		const int ri = static_cast<int>(it.key() / 1000000);
		const int ci = static_cast<int>(it.key() % 1000000);
		const int rp = rowPos[ri], cp = colPos[ci];
		const qreal stackH = cellHeight.value(it.key());
		qreal top = rowCenter(rp) - stackH / 2.0;
		for (graphdetail::SexpNodeItem* card : it.value()) {
			const qreal ch = card->boundingRect().height();
			card->setPos(colXc(cp), top + ch / 2.0);
			card->setZValue(1.0);
			if (card->eventIndex() == selEvent)
				card->setSelected(true);
			m_scene->addItem(card);
			top += ch + cardGap;
		}
		const qreal bottom = rowCenter(rp) + stackH / 2.0;
		if (!colBottom.contains(cp) || bottom > colBottom[cp]) colBottom[cp] = bottom;
	}

	// Vertical connector per column: header down to the lowest card.
	if (s_refMode != RefLineMode::Off) {
		for (auto it = colBottom.constBegin(); it != colBottom.constEnd(); ++it) {
			auto* line = m_scene->addLine(colXc(it.key()), topGutter / 2.0, colXc(it.key()), it.value(),
				QPen(m_style.entity, 1.6, Qt::DashLine));
			line->setZValue(-1.0);
		}
	}

	// Column headers (events).
	for (int p = 0; p < keepCols.size(); ++p) {
		const int ev = cols[keepCols[p]];
		const QString name = (ev >= 0 && ev < m_eventNames.size()) ? m_eventNames[ev] : tr("<event %1>").arg(ev);
		auto* hdr = new graphdetail::EventNodeItem(ev, name, m_style);
		hdr->setPos(colXc(p), topGutter / 2.0);
		hdr->setZValue(1.0);
		if (m_focusEvents.contains(ev))
			hdr->setFocusHighlight(true); // filter item (red)
		else if (ev == selEvent)
			hdr->setSelected(true); // tree-synced selection (orange)
		applyEventStatus(hdr, ev);
		m_scene->addItem(hdr);
		m_eventCards.insert(ev, hdr);
	}

	// Chain lines between consecutive event columns (only where both are present).
	refreshChainLines();

	// Row headers (objects); carry their key for the cross-filter click handler.
	for (int p = 0; p < keepRows.size(); ++p) {
		const int ri = keepRows[p];
		auto* hdr = new graphdetail::ObjectNodeItem(
			QString::fromLatin1(EventReferenceIndex::kindLabel(rows[ri].first)), m_style.colorFor(rows[ri].first),
			rows[ri].second, QString(), m_style);
		hdr->setPos(leftGutter / 2.0, rowCenter(p));
		hdr->setZValue(1.0);
		hdr->setData(0, rowKeyOf[ri]);
		if (m_focusObjects.contains(rowKeyOf[ri]))
			hdr->setFocusHighlight(true); // filter item (red)
		m_scene->addItem(hdr);
	}

	updateSceneRect();
	const QString curKey = QStringLiteral("swim");
	if (m_hasFramed && curKey == m_framedKey) {
		setTransform(savedTransform);
		centerOn(savedCenter);
		m_currentScale = transform().m11();
	} else {
		zoomToFitAll();
		m_framedKey = curKey;
		m_hasFramed = true;
	}

	m_suppressSelectionSignal = false;
	applyEmphasis(); // re-apply any active search highlight
	if (m_minimap)
		m_minimap->regenerate();
}

void EventGraphView::setBasicLayoutToCustom()
{
	if (s_basicLayout == BasicLayout::Custom)
		return;
	s_basicLayout = BasicLayout::Custom;
	// Reflect it in the dropdown without triggering a rebuild (the dragged card is
	// already where the user dropped it).
	if (m_objectCombo) {
		QSignalBlocker blocker(m_objectCombo);
		const int row = m_objectCombo->findData(static_cast<int>(BasicLayout::Custom));
		if (row >= 0)
			m_objectCombo->setCurrentIndex(row);
	}
}

void EventGraphView::rebuildBasic()
{
	// Viewport continuity (same pattern as the other modes).
	const QTransform savedTransform = transform();
	const QPointF savedCenter = mapToScene(viewport()->rect().center());

	m_dragItem = nullptr;
	m_suppressSelectionSignal = true;
	m_scene->clear();
	m_eventCards.clear(); // pointers just died with the scene

	const BasicGraph& g = m_basicGraph;
	if (g.ops.empty()) {
		m_suppressSelectionSignal = false;
		showEmptyMessage(tr("No events to display."));
		return;
	}

	const int nOps = static_cast<int>(g.ops.size());
	const int nObj = static_cast<int>(g.objects.size());
	const int nEvents = static_cast<int>(g.events.size());

	// --- Deterministic seed layout: events stack vertically, each event's
	// operator tree expands horizontally to the right by depth. Each leaf op
	// takes one vertical slot; a parent centers on its children's slots. ---
	// Custom honors saved positions; Auto/Compact are pure computed layouts.
	const bool useSaved = (s_basicLayout == BasicLayout::Custom);
	const bool compact = (s_basicLayout == BasicLayout::Compact);
	// Combine: one shared node per object (right-hand column). Duplicate: a node
	// per reference, laid out as a leaf child beside its operator.
	const bool combine = s_basicCombineObjects;
	const qreal colStep = graphdetail::kNodeW + (compact ? 44.0 : 96.0);
	const qreal rowStep = compact ? 104.0 : 132.0;
	const qreal eventGap = compact ? 0.6 : 1.0; // extra slots between events

	// Duplicate-mode object placements: one per (operator, reference).
	struct DupPlacement {
		int op = -1, objectIndex = -1, leafTreeNode = -1, depth = 0;
		double slot = 0.0;
		bool placed = false, hasPos = false;
		float posX = 0.0f, posY = 0.0f;
	};
	QVector<DupPlacement> dups;
	QVector<QVector<int>> opDupIdx(nOps);
	if (!combine) {
		for (int oi = 0; oi < nOps; ++oi) {
			for (const BasicObjRef& r : g.ops[oi].objectRefs) {
				DupPlacement d;
				d.op = oi;
				d.objectIndex = r.objectIndex;
				d.leafTreeNode = r.leafTreeNode;
				d.hasPos = r.hasPos;
				d.posX = r.posX;
				d.posY = r.posY;
				opDupIdx[oi].push_back(static_cast<int>(dups.size()));
				dups.push_back(d);
			}
		}
	}

	QVector<double> opSlot(nOps, 0.0);
	QVector<int>    opDepth(nOps, 0);
	QVector<bool>   opPlaced(nOps, false);
	double slotCursor = 0.0;
	int maxDepth = 0;

	std::function<double(int, int)> assign = [&](int oi, int depth) -> double {
		opDepth[oi] = depth;
		maxDepth = std::max(maxDepth, depth);
		opPlaced[oi] = true;
		const BasicOpNode& op = g.ops[oi];
		const bool expandObjs = !combine && !op.objectRefs.empty();
		// Sexp-depth collapse: at the limit, render this node but not its subtree.
		const bool atDepthLimit = s_basicDepth < kBasicDepthAll && depth >= s_basicDepth;
		if ((op.childOps.empty() && !expandObjs) || atDepthLimit) {
			const double s = slotCursor;
			slotCursor += 1.0;
			opSlot[oi] = s;
			return s;
		}
		QVector<double> kidSlots;
		for (int c : op.childOps) {
			if (c < 0 || c >= nOps || opPlaced[c])
				continue; // guard against a malformed/shared child
			kidSlots.push_back(assign(c, depth + 1));
		}
		if (expandObjs) {
			// Duplicate object nodes are leaf children in the column to the right.
			for (int di : opDupIdx[oi]) {
				const double s = slotCursor;
				slotCursor += 1.0;
				dups[di].slot = s;
				dups[di].depth = depth + 1;
				dups[di].placed = true;
				maxDepth = std::max(maxDepth, depth + 1);
				kidSlots.push_back(s);
			}
		}
		double s;
		if (kidSlots.isEmpty()) {
			s = slotCursor;
			slotCursor += 1.0;
		} else {
			double sum = 0.0;
			for (double k : kidSlots)
				sum += k;
			s = sum / kidSlots.size();
		}
		opSlot[oi] = s;
		return s;
	};

	for (int i = 0; i < nEvents; ++i) {
		const int root = g.events[i].rootOp;
		if (root < 0 || root >= nOps || opPlaced[root])
			continue;
		assign(root, 0);
		slotCursor += eventGap; // vertical gap before the next event
	}
	// opPlaced now marks the rendered ops; anything left is collapsed by the depth
	// limit (or unreachable) and is skipped below.

	// Collapsed events: their layout position is still computed (so the event card
	// stays exactly where it would be when expanded), but the subtree is not
	// rendered - unplace those ops so every render/visibility check below skips them.
	for (int oi = 0; oi < nOps; ++oi)
		if (opPlaced[oi] && m_collapsedEvents.contains(g.ops[oi].eventIndex))
			opPlaced[oi] = false;

	auto opSeed = [&](int oi) { return QPointF(opDepth[oi] * colStep, opSlot[oi] * rowStep); };

	// Combine mode places shared objects in a column to the right of the tree.
	const qreal objColX = (maxDepth + 1) * colStep + 40.0;
	QVector<bool> objVisible(nObj, false);
	QVector<QPointF> objPos(nObj);
	if (combine) {
		// Seed each shared object near the average Y of its referencing operators.
		QVector<double> objSeedY(nObj, 0.0);
		QVector<int>    objRefCount(nObj, 0);
		for (int oi = 0; oi < nOps; ++oi) {
			if (!opPlaced[oi])
				continue; // a collapsed op contributes no object references
			const QPointF p = opSeed(oi);
			for (const BasicObjRef& r : g.ops[oi].objectRefs) {
				if (r.objectIndex < 0 || r.objectIndex >= nObj)
					continue;
				objSeedY[r.objectIndex] += p.y();
				objRefCount[r.objectIndex] += 1;
			}
		}
		QVector<int> objOrder;
		for (int o = 0; o < nObj; ++o) {
			if (objRefCount[o] <= 0)
				continue; // shown only if a rendered operator still references it
			objVisible[o] = true;
			objSeedY[o] = objSeedY[o] / objRefCount[o];
			objOrder.push_back(o);
		}
		std::sort(objOrder.begin(), objOrder.end(), [&](int a, int b) { return objSeedY[a] < objSeedY[b]; });
		// Stack them with a minimum vertical gap; saved positions override.
		const qreal minObjGap = 78.0;
		double lastY = -1e9;
		for (int o : objOrder) {
			const double y = std::max(objSeedY[o], lastY + minObjGap);
			lastY = y;
			objPos[o] = (useSaved && g.objects[o].hasPos) ? QPointF(g.objects[o].posX, g.objects[o].posY)
														  : QPointF(objColX, y);
		}
	}

	// Operator positions (saved override in Custom).
	QVector<QPointF> opPos(nOps);
	for (int oi = 0; oi < nOps; ++oi)
		opPos[oi] = (useSaved && g.ops[oi].hasPos) ? QPointF(g.ops[oi].posX, g.ops[oi].posY) : opSeed(oi);

	// Duplicate-mode object positions (leaf children beside their operator).
	QVector<QPointF> dupPos(dups.size());
	for (int di = 0; di < dups.size(); ++di) {
		if (!dups[di].placed)
			continue;
		const QPointF seed(dups[di].depth * colStep, dups[di].slot * rowStep);
		dupPos[di] = (useSaved && dups[di].hasPos) ? QPointF(dups[di].posX, dups[di].posY) : seed;
	}

	// Event nodes anchor each event to the left of its root operator.
	QVector<QPointF> eventPos(nEvents);
	double degenerateY = 0.0;
	for (int i = 0; i < nEvents; ++i) {
		const BasicEventNode& en = g.events[i];
		if (useSaved && en.hasPos) {
			eventPos[i] = QPointF(en.posX, en.posY);
		} else if (en.rootOp >= 0 && en.rootOp < nOps) {
			// opPos is computed for every op (collapsed or not), so a collapsed event
			// keeps the same position as when expanded.
			eventPos[i] = QPointF(opPos[en.rootOp].x() - colStep, opPos[en.rootOp].y());
		} else {
			eventPos[i] = QPointF(-colStep, degenerateY); // event with no operator root (rare)
			degenerateY += rowStep;
		}
	}

	// --- Cards first (so edges can reference the created card items); edges sit
	// behind them via their negative Z regardless of insertion order. ---
	QVector<graphdetail::ObjectNodeItem*> objItem(nObj, nullptr);       // combine mode
	QVector<graphdetail::ObjectNodeItem*> dupItem(dups.size(), nullptr); // duplicate mode
	QVector<graphdetail::SexpNodeItem*> opItem(nOps, nullptr);
	QVector<graphdetail::EventNodeItem*> evItem(nEvents, nullptr);

	// posKey drives drag-position persistence + annotation display; menuKey is the
	// tree node the right-click menu targets (-1 = aggregate, no single node).
	auto makeObjectCard = [&](const BasicObjNode& ob, const QPointF& pos, int posKey, int menuKey) {
		auto* card = new graphdetail::ObjectNodeItem(QString::fromLatin1(EventReferenceIndex::kindLabel(ob.kind)),
			m_style.colorFor(ob.kind), QString::fromStdString(ob.name), QString(), m_style);
		card->setPos(pos);
		card->setZValue(1.5);
		card->setFlag(QGraphicsItem::ItemIsMovable, true);
		card->setCursor(Qt::SizeAllCursor);
		card->setData(0, posKey);
		applyAnnotation(card, posKey); // sets display + annotationKey + order from posKey
		card->setAnnotationKey(menuKey); // override the menu target
		card->setAggregate(menuKey == -1); // combined multi-ref: show "*" instead of an order
		m_scene->addItem(card);
		return card;
	};

	// Object cards: one shared node per object (combine), or one per reference
	// keyed on its own leaf (duplicate). A combined node that actually has a single
	// reference is still directly editable (targets that one leaf); only true
	// multi-reference cards are aggregates with no menu.
	if (combine) {
		for (int o = 0; o < nObj; ++o) {
			if (objVisible[o]) {
				const BasicObjNode& ob = g.objects[o];
				const int menuKey = (ob.refTreeNodes.size() == 1) ? ob.refTreeNodes[0] : -1;
				objItem[o] = makeObjectCard(ob, objPos[o], ob.posKey, menuKey);
			}
		}
	} else {
		for (int di = 0; di < dups.size(); ++di) {
			// opPlaced[op] is now false for collapsed events, so their dup cards drop.
			if (dups[di].placed && dups[di].op >= 0 && dups[di].op < nOps && opPlaced[dups[di].op] &&
				dups[di].objectIndex >= 0 && dups[di].objectIndex < nObj) {
				dupItem[di] = makeObjectCard(
					g.objects[dups[di].objectIndex], dupPos[di], dups[di].leafTreeNode, dups[di].leafTreeNode);
			}
		}
	}

	// Operator cards.
	for (int oi = 0; oi < nOps; ++oi) {
		if (!opPlaced[oi])
			continue; // collapsed by the depth limit
		const BasicOpNode& op = g.ops[oi];
		QVector<QString> argList;
		QVector<int> argNodes;
		argList.reserve(static_cast<int>(op.inlineArgs.size()));
		argNodes.reserve(static_cast<int>(op.inlineArgs.size()));
		for (const auto& a : op.inlineArgs) {
			argList.push_back(QString::fromStdString(a.text));
			argNodes.push_back(a.treeNode);
		}
		const QString evName = (op.eventIndex >= 0 && op.eventIndex < m_eventNames.size())
			? m_eventNames[op.eventIndex] : QString();
		auto* card = new graphdetail::SexpNodeItem(op.treeNode, op.eventIndex, op.isCond,
			QString::fromStdString(op.opText), argList, evName, QString::fromStdString(op.expression), m_style);
		card->setPos(opPos[oi]);
		card->setZValue(2.0);
		card->setFlag(QGraphicsItem::ItemIsMovable, true);
		card->setCursor(Qt::SizeAllCursor);
		card->setData(0, op.posKey);
		applyAnnotation(card, op.posKey);
		card->setArgNodes(std::move(argNodes)); // enable per-bullet editing (Basic)
		m_scene->addItem(card);
		opItem[oi] = card;
	}

	// Event cards (the trigger node for each event), like the other two modes.
	for (int i = 0; i < nEvents; ++i) {
		const BasicEventNode& en = g.events[i];
		const QString name = (en.eventIndex >= 0 && en.eventIndex < m_eventNames.size()) ? m_eventNames[en.eventIndex]
																   : tr("<event %1>").arg(en.eventIndex);
		auto* card = new graphdetail::EventNodeItem(en.eventIndex, name, m_style);
		card->setPos(eventPos[i]);
		card->setZValue(2.0);
		card->setFlag(QGraphicsItem::ItemIsMovable, true);
		card->setCursor(Qt::SizeAllCursor);
		card->setData(0, en.posKey);
		applyEventStatus(card, en.eventIndex);
		// Collapse box on events that actually have a subtree to hide.
		card->setCollapseState(en.rootOp >= 0, m_collapsedEvents.contains(en.eventIndex));
		m_scene->addItem(card);
		evItem[i] = card;
		m_eventCards.insert(en.eventIndex, card);
	}

	// Chain lines: a chained event points back to the previous event's node.
	refreshChainLines();

	// Edges, carrying their endpoint card items for the selection-emphasis pass.
	if (s_refMode != RefLineMode::Off) {
		auto addEdge = [&](const QPointF& a, const QPointF& b, const QColor& edgeColor, graphdetail::CardItem* ia,
						   graphdetail::CardItem* ib) {
			auto* edge = new graphdetail::RefEdgeItem(a, b, edgeColor, m_style);
			edge->setEndpoints(ia, ib);
			m_scene->addItem(edge);
		};
		for (int i = 0; i < nEvents; ++i) {
			const int root = g.events[i].rootOp;
			if (root >= 0 && root < nOps && opPlaced[root])
				addEdge(eventPos[i], opPos[root], m_style.entity, evItem[i], opItem[root]);
		}
		for (int oi = 0; oi < nOps; ++oi) {
			if (!opPlaced[oi])
				continue;
			for (int c : g.ops[oi].childOps)
				if (c >= 0 && c < nOps && opPlaced[c])
					addEdge(opPos[oi], opPos[c], m_style.ringColor, opItem[oi], opItem[c]);
			// Combine: one edge per distinct object this op references.
			if (combine) {
				QSet<int> seen;
				for (const BasicObjRef& r : g.ops[oi].objectRefs) {
					if (r.objectIndex < 0 || r.objectIndex >= nObj || !objVisible[r.objectIndex])
						continue;
					if (seen.contains(r.objectIndex))
						continue;
					seen.insert(r.objectIndex);
					addEdge(opPos[oi], objPos[r.objectIndex], m_style.colorFor(g.objects[r.objectIndex].kind),
						opItem[oi], objItem[r.objectIndex]);
				}
			}
		}
		// Duplicate: one edge per placement, operator to its local object node.
		if (!combine) {
			for (int di = 0; di < dups.size(); ++di) {
				const int oi = dups[di].op;
				// Skip collapsed (unrendered) ops/dups so no edge dangles to a null card.
				if (!dups[di].placed || oi < 0 || oi >= nOps || !opItem[oi] || !dupItem[di] ||
					dups[di].objectIndex < 0 || dups[di].objectIndex >= nObj)
					continue;
				addEdge(opPos[oi], dupPos[di], m_style.colorFor(g.objects[dups[di].objectIndex].kind), opItem[oi],
					dupItem[di]);
			}
		}
	}

	// Frame the graph, or resume the prior zoom/center if we're staying in Basic.
	updateSceneRect();
	const QString curKey = QStringLiteral("basic");
	if (m_hasFramed && curKey == m_framedKey) {
		setTransform(savedTransform);
		centerOn(savedCenter);
		m_currentScale = transform().m11();
	} else {
		zoomToFitAll();
		m_framedKey = curKey;
		m_hasFramed = true;
	}

	m_suppressSelectionSignal = false;
	applyEmphasis();
	if (m_minimap)
		m_minimap->regenerate();
}

void EventGraphView::zoomStep(bool zoomIn)
{
	const qreal step = zoomIn ? 1.10 : (1.0 / 1.10);
	// A fit-to-view can land outside [kMinScale, kMaxScale], and clamping from there
	// would jump the wrong way (a "zoom out" from 0.05 would snap up to 0.2). Widen
	// the range to include the current scale so a step only moves toward it.
	const qreal next =
		qBound(std::min(kMinScale, m_currentScale), m_currentScale * step, std::max(kMaxScale, m_currentScale));
	const qreal factor = next / m_currentScale;
	if (qFuzzyCompare(factor, 1.0))
		return;
	const QPointF center = mapToScene(viewport()->rect().center());
	scale(factor, factor);
	m_currentScale = next;
	centerOn(center); // keep the view centered (the wheel came from the minimap)
}

void EventGraphView::zoomToFitAll(qreal margin)
{
	if (!m_scene || m_scene->items().isEmpty())
		return;
	const QRectF rect = m_scene->itemsBoundingRect().adjusted(-margin, -margin, margin, margin);
	if (!rect.isEmpty()) {
		fitInView(rect, Qt::KeepAspectRatio);
		m_currentScale = transform().m11();
		if (isVisible()) // a fit against a real, on-screen viewport is a good frame
			m_framedWithSize = true;
	}
}

// The graph may be built before the dialog is shown (opening straight onto the
// graph view), when the viewport has no real size and the fit is bogus. Once the
// view is actually on screen with real geometry, fit the content once so it's
// visible; after that the user's zoom/pan is left alone.
void EventGraphView::maybeInitialFrame()
{
	if (!m_framedWithSize && isVisible() && m_scene && !m_scene->items().isEmpty()
		&& viewport()->width() > 1 && viewport()->height() > 1)
		zoomToFitAll(); // sets m_framedWithSize (we're visible)
}

void EventGraphView::wheelEvent(QWheelEvent* e)
{
	const QPoint deg = e->angleDelta();
	if (deg.isNull()) {
		e->ignore();
		return;
	}
	const qreal step = (deg.y() > 0) ? 1.10 : (1.0 / 1.10);
	const qreal next = // widened like zoomStep(): never jump away from the range
		qBound(std::min(kMinScale, m_currentScale), m_currentScale * step, std::max(kMaxScale, m_currentScale));
	const qreal factor = next / m_currentScale;
	if (!qFuzzyCompare(factor, 1.0)) {
		scale(factor, factor);
		m_currentScale = next;
		if (m_minimap)
			m_minimap->update();
	}
	e->accept();
}

void EventGraphView::drawBackground(QPainter* p, const QRectF& rect)
{
	p->save();
	p->setRenderHint(QPainter::Antialiasing, false);
	p->fillRect(rect, m_style.bgColor);

	const qreal step = 28.0;
	QPen gridPen(m_style.gridMinor, 1.0);
	p->setPen(gridPen);
	const qreal left = std::floor(rect.left() / step) * step;
	const qreal top = std::floor(rect.top() / step) * step;
	for (qreal x = left; x < rect.right(); x += step)
		p->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
	for (qreal y = top; y < rect.bottom(); y += step)
		p->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
	p->restore();
}

void EventGraphView::mousePressEvent(QMouseEvent* e)
{
	const QPointF sp = mapToScene(e->pos());

	// Pan (all modes): middle / right / shift+left drag. A right-click without a
	// drag still opens the context menu (see contextMenuEvent). The closed-hand
	// cursor appears once the drag actually moves.
	if (e->button() == Qt::MiddleButton || e->button() == Qt::RightButton
		|| (e->button() == Qt::LeftButton && (e->modifiers() & Qt::ShiftModifier))) {
		m_panning = true;
		m_panButton = e->button();
		m_panMoved = false;
		m_panLastPos = e->pos();
		m_panPressPos = e->pos();
		e->accept();
		return;
	}

	// Swimlanes cross-filter: a plain left-click on a row/column header drives the
	// filter; a click on empty canvas (handled on release) clears it.
	m_pressOnEmpty = false;
	if (m_mode == Mode::Swimlanes && e->button() == Qt::LeftButton) {
		const bool ctrl = e->modifiers().testFlag(Qt::ControlModifier);
		bool onNode = false;
		for (QGraphicsItem* it : m_scene->items(sp)) {
			if (auto* obj = qgraphicsitem_cast<graphdetail::ObjectNodeItem*>(it)) {
				toggleSwimObjectFocus(obj->data(0).toString(), ctrl);
				m_hasFramed = false;
				rebuildSwimlanes();
				e->accept();
				return;
			}
			if (auto* ev = qgraphicsitem_cast<graphdetail::EventNodeItem*>(it)) {
				toggleSwimEventFocus(ev->eventIndex(), ctrl);
				m_hasFramed = false;
				rebuildSwimlanes();
				e->accept();
				return;
			}
			if (qgraphicsitem_cast<graphdetail::SexpNodeItem*>(it)) {
				onNode = true; // node cards fall through to normal selection / jump
				break;
			}
		}
		if (!onNode) {
			m_pressOnEmpty = true;
			m_pressPos = e->pos();
		}
	}

	// Card-specific handling for a plain left-press (collapse box, arg bullet, and
	// remembering a grabbed movable card so the move persists).
	m_dragItem = nullptr;
	for (QGraphicsItem* it : m_scene->items(sp)) {
		if (auto* card = dynamic_cast<graphdetail::CardItem*>(it)) {
			card->bringToFront(); // clicked card jumps above overlaps
			if (auto* ev = qgraphicsitem_cast<graphdetail::EventNodeItem*>(card);
				ev && ev->collapseToggleAt(sp)) {
				Q_EMIT eventCollapseToggled(ev->eventIndex());
				e->accept();
				return;
			}
			if (card->handleToggleClick(sp)) {
				if (m_minimap)
					m_minimap->regenerate(); // the card changed size
				e->accept();
				return;
			}
			// An arg bullet is a click/edit target: select the card, never drag it.
			if (card->bulletNodeAt(sp) >= 0) {
				if (!card->isSelected()) {
					m_scene->clearSelection();
					card->setSelected(true);
				}
				e->accept();
				return;
			}
			if (m_mode == Mode::Basic && (card->flags() & QGraphicsItem::ItemIsMovable)) {
				m_dragItem = card;
				m_dragStartPos = card->pos();
			}
			break;
		}
	}

	// Empty canvas rubber-band selects, a card selects/moves - all via QGraphicsView
	// (RubberBandDrag mode).
	QGraphicsView::mousePressEvent(e);
}

void EventGraphView::mouseMoveEvent(QMouseEvent* e)
{
	if (m_panning) {
		if (!m_panMoved && (e->pos() - m_panPressPos).manhattanLength() > 3) {
			m_panMoved = true;
			viewport()->setCursor(Qt::ClosedHandCursor);
		}
		const QPoint delta = e->pos() - m_panLastPos;
		m_panLastPos = e->pos();
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
		verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
		e->accept();
		return;
	}
	QGraphicsView::mouseMoveEvent(e);
	// Hovering empty canvas: reflect the Shift pan-ready cursor (cards set their own).
	if (e->buttons() == Qt::NoButton)
		updatePanCursor();
}

void EventGraphView::mouseReleaseEvent(QMouseEvent* e)
{
	// End a pan (middle / right / shift+left).
	if (m_panning && e->button() == m_panButton) {
		m_panning = false;
		m_panButton = Qt::NoButton;
		updatePanCursor();
		e->accept();
		return;
	}

	// Basic: if a card was dragged (not just clicked), persist its new position.
	if (m_mode == Mode::Basic && m_dragItem && e->button() == Qt::LeftButton) {
		graphdetail::CardItem* item = m_dragItem;
		m_dragItem = nullptr;
		QGraphicsView::mouseReleaseEvent(e); // let the item finish its move first
		const QPointF now = item->pos();
		if ((now - m_dragStartPos).manhattanLength() > 2) {
			QVector<QPair<int, QPointF>> moves;
			QSet<int> seen;
			auto addCard = [&moves, &seen](QGraphicsItem* gi) {
				auto* c = dynamic_cast<graphdetail::CardItem*>(gi);
				if (c == nullptr)
					return;
				bool ok = false;
				const int key = c->data(0).toInt(&ok);
				if (ok && !seen.contains(key)) {
					seen.insert(key);
					moves.push_back({key, c->pos()});
				}
			};
			if (s_basicLayout != BasicLayout::Custom) {
				// Leaving a computed layout (Auto/Compact) for Custom. The other cards
				// sit at computed positions that were never saved, so the next rebuild
				// in Custom would move them. Persist every card, not just the dragged ones.
				for (QGraphicsItem* gi : m_scene->items())
					addCard(gi);
			} else {
				// Qt moves the grabbed card together with the whole selection, even when
				// a Ctrl+press left the grabbed card itself unselected, so persist both.
				addCard(item);
				for (QGraphicsItem* gi : m_scene->selectedItems())
					addCard(gi);
			}
			// One undo step for the whole drag.
			if (!moves.isEmpty())
				Q_EMIT nodesMoved(moves);
			setBasicLayoutToCustom(); // a manual move puts us in the Custom arrangement
			if (m_minimap)
				m_minimap->regenerate(); // the cards moved
		}
		return;
	}

	// A click (not a pan-drag) on empty canvas clears the swimlanes filter.
	const bool emptyClick = m_pressOnEmpty && e->button() == Qt::LeftButton
		&& (e->pos() - m_pressPos).manhattanLength() < 6
		&& (!m_focusObjects.isEmpty() || !m_focusEvents.isEmpty());
	m_pressOnEmpty = false;

	QGraphicsView::mouseReleaseEvent(e); // finish selection / rubber-band first

	if (emptyClick) {
		clearSwimFocus();
		m_hasFramed = false;
		rebuildSwimlanes();
	}
}

void EventGraphView::keyPressEvent(QKeyEvent* e)
{
	if (m_mode == Mode::Swimlanes && e->key() == Qt::Key_Escape
		&& (!m_focusObjects.isEmpty() || !m_focusEvents.isEmpty())) {
		clearSwimFocus();
		m_hasFramed = false;
		rebuildSwimlanes();
		e->accept();
		return;
	}
	QGraphicsView::keyPressEvent(e);
}

void EventGraphView::mouseDoubleClickEvent(QMouseEvent* e)
{
	const QPointF sp = mapToScene(e->pos());
	for (QGraphicsItem* it : m_scene->items(sp)) {
		if (auto* ev = qgraphicsitem_cast<graphdetail::EventNodeItem*>(it)) {
			Q_EMIT eventActivated(ev->eventIndex());
			e->accept();
			return;
		}
		if (auto* node = qgraphicsitem_cast<graphdetail::SexpNodeItem*>(it)) {
			// Double-click an inline literal-arg bullet to edit that arg; elsewhere on
			// the card, jump to it in the tree as before.
			const int bulletNode = node->bulletNodeAt(sp);
			if (bulletNode >= 0)
				Q_EMIT nodeEditRequested(bulletNode, e->globalPosition().toPoint());
			else
				Q_EMIT nodeActivated(node->treeNode());
			e->accept();
			return;
		}
		// Basic: a shared object card jumps to its first reference in the tree.
		// (Its data(0) is that reference's leaf tree node; unset/non-int in the
		// other modes, where object cards don't jump.)
		if (auto* obj = qgraphicsitem_cast<graphdetail::ObjectNodeItem*>(it)) {
			bool ok = false;
			const int treeNode = obj->data(0).toInt(&ok);
			if (ok && treeNode >= 0) {
				Q_EMIT nodeActivated(treeNode);
				e->accept();
				return;
			}
		}
	}
	QGraphicsView::mouseDoubleClickEvent(e);
}

void EventGraphView::contextMenuEvent(QContextMenuEvent* e)
{
	// A right-drag that panned the view should not also open the menu. Only a
	// mouse-triggered menu can follow a pan; a keyboard menu (Menu key / Shift+F10)
	// must never be swallowed by a stale pan flag, so clear it either way.
	const bool pannedIntoMenu = m_panMoved;
	m_panMoved = false;
	if (pannedIntoMenu && e->reason() == QContextMenuEvent::Mouse)
		return;

	// Right-click a card: an editable card opens the tree's context menu (via the
	// dialog). An aggregate card stands for many nodes, so there's nothing to edit
	// directly - show a short disabled note rather than ignoring the click.
	for (QGraphicsItem* it : items(e->pos())) {
		if (auto* card = dynamic_cast<graphdetail::CardItem*>(it)) {
			card->bringToFront();
			// Only one node is editable at a time, so a right-click reduces any
			// multi-selection to just the clicked card (its border shows while the
			// menu is open), then shows that card's menu.
			if (m_scene->selectedItems().size() != 1 || !card->isSelected()) {
				m_scene->clearSelection();
				card->setSelected(true);
			}
			// A right-click on an inline literal-arg bullet targets that specific arg
			// node; otherwise the card's own node.
			const int bulletNode = card->bulletNodeAt(mapToScene(e->pos()));
			const int key = (bulletNode >= 0) ? bulletNode : card->annotationKey();
			if (key != -1) {
				Q_EMIT nodeContextMenuRequested(key, e->globalPos());
			} else {
				QMenu menu(this);
				QAction* note =
					menu.addAction(tr("This card represents multiple nodes and cannot be edited directly"));
				note->setEnabled(false);
				menu.exec(e->globalPos());
			}
			e->accept();
			return;
		}
	}
}

void EventGraphView::resizeEvent(QResizeEvent* e)
{
	QGraphicsView::resizeEvent(e);
	positionOverlay();
	maybeInitialFrame();
}

void EventGraphView::showEvent(QShowEvent* e)
{
	QGraphicsView::showEvent(e);
	positionOverlay();
	maybeInitialFrame();
}

void EventGraphView::scrollContentsBy(int dx, int dy)
{
	QGraphicsView::scrollContentsBy(dx, dy);
	positionOverlay(); // keep overlays pinned to the corner, not the content
	if (m_minimap)
		m_minimap->update(); // viewport rectangle moved
}

void EventGraphView::applyTheme(bool dark)
{
	m_style = EventGraphStyle::makeStyle(dark);
	setBackgroundBrush(m_style.bgColor);
	resetCachedContent(); // re-render the cached grid with the new colors
	if (m_legend)
		m_legend->update();
	if (m_overlay)
		m_overlay->update();
	reload();
}

} // namespace fso::fred
