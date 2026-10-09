//

#include "SceneBrowserPanel.h"
#include "ui_SceneBrowserPanel.h"

#include <mission/dialogs/SceneBrowserModel.h>
#include <mission/object.h>
#include <ui/FredView.h>

#include <QApplication>
#include <QInputDialog>
#include <QLayout>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QStyledItemDelegate>

namespace fso::fred {

namespace {

// Draws a row normally, then, for a row the search matched by class rather than name, the class in
// a dimmed color after the name. The item's text stays the plain name.
class ClassSuffixDelegate final : public QStyledItemDelegate {
  public:
	ClassSuffixDelegate(QObject* parent, int role) : QStyledItemDelegate(parent), _role(role) {}

	void paint(QPainter* p, const QStyleOptionViewItem& option, const QModelIndex& index) const override
	{
		QStyledItemDelegate::paint(p, option, index);

		const QString suffix = index.data(_role).toString();
		if (suffix.isEmpty())
			return;

		QStyleOptionViewItem opt(option);
		initStyleOption(&opt, index);
		const QWidget* w = opt.widget;
		const QStyle* style = w != nullptr ? w->style() : QApplication::style();
		const QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, w);

		const QFontMetrics fm(opt.font);
		const int nameWidth = fm.horizontalAdvance(fm.elidedText(opt.text, opt.textElideMode, textRect.width()));
		const int gap = fm.horizontalAdvance(QStringLiteral("   "));
		QRect suffixRect = textRect;
		suffixRect.setLeft(textRect.left() + nameWidth + gap);
		if (suffixRect.width() <= 0)
			return;

		const bool selected = (opt.state & QStyle::State_Selected) != 0;
		QColor color = opt.palette.color(selected ? QPalette::HighlightedText : QPalette::Text);
		color.setAlphaF(0.6f);

		p->save();
		p->setFont(opt.font);
		p->setPen(color);
		p->drawText(suffixRect, Qt::AlignLeft | Qt::AlignVCenter, fm.elidedText(suffix, Qt::ElideRight, suffixRect.width()));
		p->restore();
	}

  private:
	int _role;
};

} // namespace

SceneBrowserPanel::SceneBrowserPanel(FredView* fredView, EditorViewport* viewport)
	: QDockWidget(tr("Scene Browser"), fredView)
	, ui(new ::Ui::SceneBrowserPanel())
	, _fredView(fredView)
{
	setObjectName("SceneBrowserPanel");  // Required for saveState/restoreState

	// Model
	_model = new dialogs::SceneBrowserModel(this, viewport);
	connect(_model, &dialogs::SceneBrowserModel::modelChanged,
	        this, &SceneBrowserPanel::onModelChanged);
	connect(_model, &dialogs::SceneBrowserModel::treeStructureChanged,
	        this, &SceneBrowserPanel::onTreeStructureChanged);

	// Set up content widget from UI file
	auto* container = new QWidget(this);
	ui->setupUi(container);
	setWidget(container);

	_searchBar = ui->searchBar;
	_tree = ui->browserTree;
	_tree->setItemDelegate(new ClassSuffixDelegate(_tree, MatchedClassRole));
	_iffFilterWidget = ui->iffFilterWidget;
	_selectAllButton = ui->selectAllButton;
	_clearButton = ui->clearButton;
	_invertButton = ui->invertButton;

	// Connections
	connect(_searchBar, &QLineEdit::textChanged, this, &SceneBrowserPanel::onSearchTextChanged);
	connect(_selectAllButton, &QPushButton::clicked, _model, &dialogs::SceneBrowserModel::selectAll);
	connect(_clearButton, &QPushButton::clicked, _model, &dialogs::SceneBrowserModel::clearSelection);
	connect(_invertButton, &QPushButton::clicked, _model, &dialogs::SceneBrowserModel::invertSelection);
	connect(_tree, &QTreeWidget::itemChanged, this, &SceneBrowserPanel::onItemChanged);
	connect(_tree, &QTreeWidget::itemSelectionChanged, this, &SceneBrowserPanel::onItemSelectionChanged);
	connect(_tree, &QTreeWidget::itemDoubleClicked, this, &SceneBrowserPanel::onItemDoubleClicked);
	connect(_tree, &QTreeWidget::customContextMenuRequested,
	        this, &SceneBrowserPanel::onCustomContextMenuRequested);
	connect(this, &QDockWidget::topLevelChanged, this, &SceneBrowserPanel::updateFloatingMargins);
	updateFloatingMargins(isFloating());
	// Do NOT call rebuildTree() here — mission data is not initialized at construction time.
	// The tree is populated when missionLoaded fires, propagating via treeStructureChanged.
}

SceneBrowserPanel::~SceneBrowserPanel() = default;

// ---------------------------------------------------------------------------
// Tree construction
// ---------------------------------------------------------------------------

void SceneBrowserPanel::updateFloatingMargins(bool floating)
{
	auto* content = widget();
	if (content == nullptr || content->layout() == nullptr) {
		return;
	}

	// Give a little breathing room around the panel contents when floating.
	// Keep docked layout tight.
	content->layout()->setContentsMargins(floating ? 8 : 0, floating ? 8 : 0, floating ? 8 : 0, floating ? 8 : 0);
}

void SceneBrowserPanel::rememberExpansionState()
{
	for (int li = 0; li < _tree->topLevelItemCount(); li++) {
		auto* layerItem = _tree->topLevelItem(li);
		if (!layerItem->data(0, IsEnvironmentRootRole).isNull()) {
			_expansionState[QStringLiteral("E")] = layerItem->isExpanded();
			continue;
		}
		const auto layerName = layerItem->data(0, LayerNameRole).toString();
		if (layerName.isEmpty()) continue;

		_expansionState[QString("L|%1").arg(layerName)] = layerItem->isExpanded();

		for (int ci = 0; ci < layerItem->childCount(); ci++) {
			auto* catItem = layerItem->child(ci);
			const auto catName = catItem->text(0);
			_expansionState[QString("C|%1|%2").arg(layerName, catName)] = catItem->isExpanded();

			for (int oi = 0; oi < catItem->childCount(); oi++) {
				auto* objItem = catItem->child(oi);
				const auto varWing = objItem->data(0, WingIndexRole);
				if (!varWing.isNull()) {
					_expansionState[QString("W|%1|%2").arg(layerName).arg(varWing.toInt())] = objItem->isExpanded();
					continue;
				}

				const auto varPath = objItem->data(0, WptListIndexRole);
				if (!varPath.isNull()) {
					_expansionState[QString("P|%1|%2").arg(layerName).arg(varPath.toInt())] = objItem->isExpanded();
				}
			}
		}
	}
}

bool SceneBrowserPanel::expandedStateOrDefault(const QString& key, bool defaultExpanded) const
{
	const auto it = _expansionState.constFind(key);
	return it == _expansionState.constEnd() ? defaultExpanded : it.value();
}

void SceneBrowserPanel::rebuildTree()
{
	QSignalBlocker treeBlocker(_tree);
	rememberExpansionState();
	_tree->clear();

	const auto& layers = _model->getTree();
	const auto marked = dialogs::SceneBrowserModel::getMarkedSet();

	// "Environment" node: a top-level sibling of the layers, always first, no
	// checkbox. Its children are non-object entities (volumetric nebula,
	// asteroid field). Only shown when at least one such entity exists.
	const bool hasVol = dialogs::SceneBrowserModel::hasVolumetricNebula();
	const bool hasAst = dialogs::SceneBrowserModel::hasAsteroidField();
	if (hasVol || hasAst) {
		auto* envItem = new QTreeWidgetItem(_tree);
		envItem->setText(0, tr("Environment"));
		envItem->setData(0, IsEnvironmentRootRole, true);
		// Header row: not selectable, but a layer-style visibility checkbox that
		// hides/shows every environment entity (nebula, field) in the viewport.
		envItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
		envItem->setCheckState(0, _model->environmentVisible() ? Qt::Checked : Qt::Unchecked);
		envItem->setExpanded(expandedStateOrDefault(QStringLiteral("E"), true));

		if (hasVol) {
			auto* volItem = new QTreeWidgetItem(envItem);
			volItem->setText(0, tr("Volumetric Nebula"));
			volItem->setData(0, EnvKindRole, static_cast<int>(EnvironmentObject::VolumetricNebula));
			volItem->setSelected(_model->currentEnvironment() == EnvironmentObject::VolumetricNebula);
		}
		if (hasAst) {
			auto* astItem = new QTreeWidgetItem(envItem);
			astItem->setText(0, tr("Asteroid Field"));
			astItem->setData(0, EnvKindRole, static_cast<int>(EnvironmentObject::AsteroidField));
			astItem->setSelected(_model->currentEnvironment() == EnvironmentObject::AsteroidField);
		}
	}

	for (const auto& layer : layers) {
		// Count total objects across all categories
		int totalObjects = 0;
		for (const auto& cat : layer.categories) {
			for (const auto& obj : cat.items) {
				if (obj.wingIndex >= 0) {
					totalObjects += obj.children.size();
				} else if (obj.waypointListIndex >= 0) {
					totalObjects += obj.children.size();
				} else {
					totalObjects += 1;
				}
			}
		}

		auto* layerItem = new QTreeWidgetItem(_tree);
		layerItem->setText(0, QString("%1 (%2)").arg(layer.name).arg(totalObjects));
		layerItem->setData(0, IsLayerItemRole, true);
		layerItem->setData(0, LayerNameRole, layer.name);
		layerItem->setFlags(layerItem->flags() | Qt::ItemIsUserCheckable);
		layerItem->setCheckState(0, layer.visible ? Qt::Checked : Qt::Unchecked);
		const auto layerKey = QString("L|%1").arg(layer.name);
		layerItem->setExpanded(expandedStateOrDefault(layerKey, true));

		for (const auto& cat : layer.categories) {
			auto* catItem = new QTreeWidgetItem(layerItem);
			catItem->setText(0, cat.name);
			// Category rows are not selectable — they're just headers
			catItem->setFlags(Qt::ItemIsEnabled);
			const auto catKey = QString("C|%1|%2").arg(layer.name, cat.name);
			catItem->setExpanded(expandedStateOrDefault(catKey, true));

			for (const auto& obj : cat.items) {
				if (obj.wingIndex >= 0) {
					// Wing header
					auto* wingItem = new QTreeWidgetItem(catItem);
					wingItem->setText(0, obj.displayName);
					wingItem->setData(0, WingIndexRole, obj.wingIndex);
					const auto wingKey = QString("W|%1|%2").arg(layer.name).arg(obj.wingIndex);
					wingItem->setExpanded(expandedStateOrDefault(wingKey, false));

					for (const auto& member : obj.children) {
						auto* memberItem = new QTreeWidgetItem(wingItem);
						memberItem->setText(0, member.displayName);
						memberItem->setData(0, ObjNumRole, member.objNum);
						memberItem->setSelected(marked.contains(member.objNum));
					}
				} else if (obj.waypointListIndex >= 0) {
					// Waypoint path header
					auto* pathItem = new QTreeWidgetItem(catItem);
					pathItem->setText(0, obj.displayName);
					pathItem->setData(0, WptListIndexRole, obj.waypointListIndex);
					const auto pathKey = QString("P|%1|%2").arg(layer.name).arg(obj.waypointListIndex);
					pathItem->setExpanded(expandedStateOrDefault(pathKey, false));

					for (const auto& wpt : obj.children) {
						auto* wptItem = new QTreeWidgetItem(pathItem);
						wptItem->setText(0, wpt.displayName);
						wptItem->setData(0, ObjNumRole, wpt.objNum);
						wptItem->setSelected(marked.contains(wpt.objNum));
					}
				} else {
					// Regular leaf (ship, prop, jump node)
					auto* leafItem = new QTreeWidgetItem(catItem);
					leafItem->setText(0, obj.displayName);
					leafItem->setData(0, ObjNumRole, obj.objNum);
					leafItem->setSelected(marked.contains(obj.objNum));
				}
			}
		}
	}

	applyFilter(_model->getNameFilter());
}

// ---------------------------------------------------------------------------
// Sync (viewport → browser, selection only — no rebuild)
// ---------------------------------------------------------------------------

void SceneBrowserPanel::syncSelection()
{
	QSignalBlocker treeBlocker(_tree);
	const auto marked = dialogs::SceneBrowserModel::getMarkedSet();

	QTreeWidgetItemIterator it(_tree);
	QTreeWidgetItem* firstSelected = nullptr;
	while (*it) {
		auto varEnv = (*it)->data(0, EnvKindRole);
		if (!varEnv.isNull()) {
			bool sel = (_model->currentEnvironment() == static_cast<EnvironmentObject>(varEnv.toInt()));
			(*it)->setSelected(sel);
			if (sel && !firstSelected)
				firstSelected = *it;
			++it;
			continue;
		}
		auto varObjNum = (*it)->data(0, ObjNumRole);
		if (!varObjNum.isNull()) {
			bool sel = marked.contains(varObjNum.toInt());
			(*it)->setSelected(sel);
			if (sel && !firstSelected)
				firstSelected = *it;
		} else {
			// Wing/path headers: selected if any child is selected
			auto varWing = (*it)->data(0, WingIndexRole);
			if (!varWing.isNull()) {
				bool anyChild = false;
				for (int c = 0; c < (*it)->childCount(); c++) {
					auto cv = (*it)->child(c)->data(0, ObjNumRole);
					if (!cv.isNull() && marked.contains(cv.toInt())) { anyChild = true; break; }
				}
				(*it)->setSelected(anyChild);
			}
			auto varPath = (*it)->data(0, WptListIndexRole);
			if (!varPath.isNull()) {
				bool anyChild = false;
				for (int c = 0; c < (*it)->childCount(); c++) {
					auto cv = (*it)->child(c)->data(0, ObjNumRole);
					if (!cv.isNull() && marked.contains(cv.toInt())) { anyChild = true; break; }
				}
				(*it)->setSelected(anyChild);
			}
		}
		++it;
	}

	if (firstSelected)
		_tree->scrollToItem(firstSelected, QAbstractItemView::EnsureVisible);
}

void SceneBrowserPanel::syncLayerVisibility()
{
	QSignalBlocker treeBlocker(_tree);
	const auto& layers = _model->getTree();

	for (int i = 0; i < _tree->topLevelItemCount(); i++) {
		auto* item = _tree->topLevelItem(i);
		if (!item->data(0, IsEnvironmentRootRole).isNull()) {
			item->setCheckState(0, _model->environmentVisible() ? Qt::Checked : Qt::Unchecked);
			continue;
		}
		auto layerName = item->data(0, LayerNameRole).toString();
		for (const auto& layer : layers) {
			if (layer.name == layerName) {
				item->setCheckState(0, layer.visible ? Qt::Checked : Qt::Unchecked);
				break;
			}
		}
	}
}

// ---------------------------------------------------------------------------
// Search filter
// ---------------------------------------------------------------------------

void SceneBrowserPanel::applyFilter(const QString& filter)
{
	if (filter.isEmpty()) {
		showAllItems(_tree->invisibleRootItem());
		// no search, so no class suffixes
		QTreeWidgetItemIterator it(_tree, QTreeWidgetItemIterator::NoChildren);
		for (; *it; ++it) {
			if (!(*it)->data(0, MatchedClassRole).isNull())
				(*it)->setData(0, MatchedClassRole, QVariant());
		}
		return;
	}

	// First show all, then hide non-matching leaves
	showAllItems(_tree->invisibleRootItem());

	// An object matches by name or by its ship or prop class. A row matched only by class shows
	// the class after its name, so it's clear why it's listed.
	QTreeWidgetItemIterator it(_tree, QTreeWidgetItemIterator::NoChildren);
	while (*it) {
		auto varObjNum = (*it)->data(0, ObjNumRole);
		if (!varObjNum.isNull()) {
			const bool nameMatches = (*it)->text(0).contains(filter, Qt::CaseInsensitive);
			const QString cls = nameMatches ? QString() : dialogs::SceneBrowserModel::getObjectClassName(varObjNum.toInt());
			const bool classMatches = !cls.isEmpty() && cls.contains(filter, Qt::CaseInsensitive);
			(*it)->setHidden(!nameMatches && !classMatches);
			(*it)->setData(0, MatchedClassRole, classMatches ? QVariant(cls) : QVariant());
		}
		++it;
	}

	// Hide parent items that have all children hidden
	// Walk bottom-up: leaves are already handled, now handle wings, paths, categories, layers
	for (int li = 0; li < _tree->topLevelItemCount(); li++) {
		auto* layerItem = _tree->topLevelItem(li);
		// The Environment node has a different (2-level) shape than a layer and
		// is never hidden by the name filter.
		if (!layerItem->data(0, IsEnvironmentRootRole).isNull()) continue;
		bool anyLayerVisible = false;
		for (int ci = 0; ci < layerItem->childCount(); ci++) {
			auto* catItem = layerItem->child(ci);
			bool anyCatVisible = false;
			for (int oi = 0; oi < catItem->childCount(); oi++) {
				auto* objItem = catItem->child(oi);
				// Wing or path header: check its children
				bool hasVisibleChild = false;
				for (int mi = 0; mi < objItem->childCount(); mi++) {
					if (!objItem->child(mi)->isHidden()) { hasVisibleChild = true; break; }
				}
				if (objItem->childCount() > 0) {
					objItem->setHidden(!hasVisibleChild);
					if (hasVisibleChild) anyCatVisible = true;
				} else {
					if (!objItem->isHidden()) anyCatVisible = true;
				}
			}
			catItem->setHidden(!anyCatVisible);
			if (anyCatVisible) anyLayerVisible = true;
		}
		// Don't hide layer items — always show them even if empty
		Q_UNUSED(anyLayerVisible)
	}
}

void SceneBrowserPanel::showAllItems(QTreeWidgetItem* root)
{
	for (int i = 0; i < root->childCount(); i++) {
		auto* child = root->child(i);
		child->setHidden(false);
		showAllItems(child);
	}
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------

void SceneBrowserPanel::onModelChanged()
{
	// Selection or layer visibility changed — fast sync, no full rebuild
	syncSelection();
	syncLayerVisibility();
	applyFilter(_model->getNameFilter());
}

void SceneBrowserPanel::onTreeStructureChanged()
{
	// Create IFF checkboxes on first call after missionLoaded, when Iff_info is populated
	if (_iffCheckBoxes.isEmpty() && dialogs::SceneBrowserModel::iffCount() > 0) {
		delete _iffFilterWidget->layout();  // remove any existing layout before setting a new one
		auto* layout = new FlowLayout(_iffFilterWidget, /*hSpacing=*/4, /*vSpacing=*/2);
		for (int i = 0; i < dialogs::SceneBrowserModel::iffCount(); i++) {
			auto* cb = new QCheckBox(dialogs::SceneBrowserModel::getIffName(i), _iffFilterWidget);
			cb->setChecked(true);
			const int team = i;
			connect(cb, &QCheckBox::toggled, this, [this, team](bool checked) {
				_model->setFilterIff(team, checked);
			});
			layout->addWidget(cb);
			_iffCheckBoxes.push_back(cb);
		}
	}

	// Full rebuild needed (objects added/removed/renamed/moved between layers)
	rebuildTree();
}

void SceneBrowserPanel::onItemChanged(QTreeWidgetItem* item, int column)
{
	if (column != 0) return;

	// Environment visibility checkbox (top-level node).
	auto varEnvRoot = item->data(0, IsEnvironmentRootRole);
	if (!varEnvRoot.isNull() && varEnvRoot.toBool()) {
		_model->setEnvironmentVisible(item->checkState(0) == Qt::Checked);
		return;
	}

	auto varLayer = item->data(0, IsLayerItemRole);
	if (varLayer.isNull() || !varLayer.toBool()) return;

	auto layerName = item->data(0, LayerNameRole).toString();
	_model->toggleLayerVisibility(layerName);
}

void SceneBrowserPanel::onItemSelectionChanged()
{
	if (_model->isUpdatingFromBrowser()) return;

	// Environment entities are single-select and mutually exclusive with
	// objects: if one is in the selection, select it and ignore the rest.
	for (auto* item : _tree->selectedItems()) {
		auto varEnv = item->data(0, EnvKindRole);
		if (!varEnv.isNull()) {
			_model->selectEnvironmentFromBrowser(static_cast<EnvironmentObject>(varEnv.toInt()));
			// Reconcile the tree with the single-select model (objects were
			// unmarked); harmless for a plain click, collapses a ctrl+click mix.
			syncSelection();
			return;
		}
	}

	QVector<int> selectedObjNums;
	QVector<int> selectedWings;

	for (auto* item : _tree->selectedItems()) {
		auto varObjNum = item->data(0, ObjNumRole);
		if (!varObjNum.isNull()) {
			selectedObjNums.push_back(varObjNum.toInt());
		} else {
			// Wing header selected
			auto varWing = item->data(0, WingIndexRole);
			if (!varWing.isNull()) {
				const auto wingIndex = varWing.toInt();
				if (!selectedWings.contains(wingIndex)) {
					selectedWings.push_back(wingIndex);
				}
			}
			// Waypoint path header: select all children
			auto varPath = item->data(0, WptListIndexRole);
			if (!varPath.isNull()) {
				for (int c = 0; c < item->childCount(); c++) {
					auto cv = item->child(c)->data(0, ObjNumRole);
					if (!cv.isNull()) selectedObjNums.push_back(cv.toInt());
				}
			}
		}
	}

	if (!selectedWings.isEmpty()) {
		for (auto wingIndex : selectedWings) {
			const auto wingMembers = dialogs::SceneBrowserModel::getWingMemberObjects(wingIndex);
			selectedObjNums += wingMembers;
		}
		_model->multiSelectFromBrowser(selectedObjNums);
	} else if (!selectedObjNums.isEmpty()) {
		_model->multiSelectFromBrowser(selectedObjNums);
	} else if (_model->currentEnvironment() != EnvironmentObject::None) {
		// The selected environment entity was ctrl+clicked off.
		_model->selectEnvironmentFromBrowser(EnvironmentObject::None);
	} else {
		_model->multiSelectFromBrowser({});
	}
}

// Double-clicking an object opens its editor, as in the viewport, and an environment row opens its
// editor. Layer, category, wing and path rows keep the tree's usual expand/collapse.
void SceneBrowserPanel::onItemDoubleClicked(QTreeWidgetItem* item, int /*column*/)
{
	if (item == nullptr)
		return;

	auto varEnv = item->data(0, EnvKindRole);
	if (!varEnv.isNull()) {
		const auto kind = static_cast<EnvironmentObject>(varEnv.toInt());
		_model->selectEnvironmentFromBrowser(kind);
		syncSelection();
		if (kind == EnvironmentObject::VolumetricNebula) {
			_fredView->editVolumetricNebula();
		} else if (kind == EnvironmentObject::AsteroidField) {
			_fredView->editAsteroidField();
		}
		return;
	}

	auto varObjNum = item->data(0, ObjNumRole);
	if (varObjNum.isNull())
		return;
	const int objNum = varObjNum.toInt();
	// the double-click's first click already selected the object
	if (query_valid_object(objNum))
		_fredView->handleObjectEditor(objNum);
}

void SceneBrowserPanel::onCustomContextMenuRequested(const QPoint& pos)
{
	auto* item = _tree->itemAt(pos);
	if (!item) return;

	const auto globalPos = _tree->viewport()->mapToGlobal(pos);

	// Environment child (volumetric nebula, asteroid later): select it, then
	// offer its editor, mirroring an object's right-click "Edit ...".
	auto varEnv = item->data(0, EnvKindRole);
	if (!varEnv.isNull()) {
		const auto kind = static_cast<EnvironmentObject>(varEnv.toInt());
		_model->selectEnvironmentFromBrowser(kind);
		syncSelection();
		QMenu menu(this);
		QAction* editAction = nullptr;
		if (kind == EnvironmentObject::VolumetricNebula) {
			editAction = menu.addAction(tr("Edit Volumetric Nebula"));
		} else if (kind == EnvironmentObject::AsteroidField) {
			editAction = menu.addAction(tr("Edit Asteroid Field"));
		}
		if (editAction != nullptr && menu.exec(globalPos) == editAction) {
			if (kind == EnvironmentObject::VolumetricNebula) {
				_fredView->editVolumetricNebula();
			} else if (kind == EnvironmentObject::AsteroidField) {
				_fredView->editAsteroidField();
			}
		}
		return;
	}

	// Layer header item: select everything on the layer, or rename it
	auto varLayer = item->data(0, IsLayerItemRole);
	if (!varLayer.isNull()) {
		const auto layerName = item->data(0, LayerNameRole).toString();
		// a hidden layer's objects can't be selected, so it has nothing to select
		const auto layerObjects = _model->getLayerObjects(layerName);

		QMenu menu;
		auto* selectAction = menu.addAction(tr("Select All"));
		selectAction->setEnabled(!layerObjects.isEmpty());
		menu.addSeparator();
		auto* renameAction = menu.addAction(tr("Rename Layer"));
		renameAction->setEnabled(!dialogs::SceneBrowserModel::isDefaultLayer(layerName));
		auto* chosen = menu.exec(globalPos);
		if (chosen == selectAction) {
			_model->multiSelectFromBrowser(layerObjects);
			syncSelection();
		} else if (chosen == renameAction) {
			renameLayer(layerName);
		}
		return;
	}

	// Category items are not actionable
	if (item->flags() == Qt::ItemIsEnabled) return;  // category item

	auto varObjNum = item->data(0, ObjNumRole);
	auto varWing = item->data(0, WingIndexRole);
	auto varPath = item->data(0, WptListIndexRole);

	if (!varWing.isNull()) {
		_fredView->showWingContextMenu(varWing.toInt(), globalPos);
	} else if (!varPath.isNull()) {
		_fredView->showWaypointPathContextMenu(varPath.toInt(), globalPos);
	} else if (!varObjNum.isNull()) {
		// Regular object: delegate to FredView's context menu (handles Edit + Move to Layer)
		_fredView->showContextMenu(varObjNum.toInt(), globalPos);
	}
}

void SceneBrowserPanel::renameLayer(const QString& layerName)
{
	bool ok = false;
	const auto newName = QInputDialog::getText(this, tr("Rename Layer"), tr("Layer name:"),
		QLineEdit::Normal, layerName, &ok).trimmed();
	if (!ok || newName == layerName) {
		if (ok && newName.isEmpty()) {
			QMessageBox::warning(this, tr("Layer Error"), tr("Layer name cannot be empty."));
		}
		return;
	}

	SCP_string error;
	if (!_model->renameLayer(layerName, newName, &error)) {
		QMessageBox::warning(this, tr("Layer Error"), QString::fromStdString(error));
	}
}

void SceneBrowserPanel::onSearchTextChanged(const QString& text)
{
	_model->setNameFilter(text);
}

} // namespace fso::fred
