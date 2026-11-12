#include "IconFromObjectDialog.h"
#include "ui/util/DialogEnterGuard.h"

#include "mission/dialogs/BriefingEditorDialogModel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>

namespace fso::fred::dialogs {
namespace {
constexpr int RoleKind = Qt::UserRole;
constexpr int RoleIndex = Qt::UserRole + 1;
constexpr int KindShip = 0;
constexpr int KindWing = 1;
constexpr int KindProp = 2;
} // namespace

IconFromObjectDialog::IconFromObjectDialog(QWidget* parent, BriefingEditorDialogModel* model)
	: QDialog(parent), _model(model)
{
	// A picker: Enter in a field accepts, as users expect (see DialogEnterGuard).
	fso::fred::util::allowEnterToAccept(this);

	setWindowTitle("Make Icon From");
	setMinimumSize(350, 450);

	auto* layout = new QVBoxLayout(this);

	// Search/filter, shared by both tabs
	auto* filterLayout = new QHBoxLayout();
	filterLayout->addWidget(new QLabel("Filter:", this));
	_filter = new QLineEdit(this);
	_filter->setPlaceholderText("Search...");
	_filter->setClearButtonEnabled(true);
	filterLayout->addWidget(_filter);
	layout->addLayout(filterLayout);

	_tabs = new QTabWidget(this);

	_shipTree = new QTreeWidget(_tabs);
	_shipTree->setHeaderLabel("Ships");
	_shipTree->setSelectionMode(QAbstractItemView::SingleSelection);
	_tabs->addTab(_shipTree, "Ships");

	_propTree = new QTreeWidget(_tabs);
	_propTree->setHeaderLabel("Props");
	_propTree->setSelectionMode(QAbstractItemView::SingleSelection);
	_propTree->setRootIsDecorated(false);
	_tabs->addTab(_propTree, "Props");

	layout->addWidget(_tabs);

	// Buttons
	auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	layout->addWidget(buttons);

	populateShipTree();
	populatePropTree();

	connect(_filter, &QLineEdit::textChanged, this, &IconFromObjectDialog::onFilterTextChanged);
	connect(_shipTree, &QTreeWidget::itemDoubleClicked, this, &IconFromObjectDialog::onItemDoubleClicked);
	connect(_propTree, &QTreeWidget::itemDoubleClicked, this, &IconFromObjectDialog::onItemDoubleClicked);
	connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
		auto* tree = (_tabs->currentWidget() == _propTree) ? _propTree : _shipTree;
		if (selectItem(tree->currentItem()))
			accept();
	});
	connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

	setLayout(layout);
}

IconFromObjectDialog::SelectionKind IconFromObjectDialog::selectedKind() const
{
	return _selectedKind;
}

int IconFromObjectDialog::selectedIndex() const
{
	return _selectedIndex;
}

bool IconFromObjectDialog::selectItem(const QTreeWidgetItem* item)
{
	if (!item)
		return false;
	const auto kindData = item->data(0, RoleKind);
	const auto indexData = item->data(0, RoleIndex);
	const int kind = kindData.isValid() ? kindData.toInt() : -1;
	const int index = indexData.isValid() ? indexData.toInt() : -1;
	if (index < 0)
		return false;

	switch (kind) {
	case KindShip:
		_selectedKind = SelectionKind::Ship;
		break;
	case KindWing:
		_selectedKind = SelectionKind::Wing;
		break;
	case KindProp:
		_selectedKind = SelectionKind::Prop;
		break;
	default:
		return false;
	}
	_selectedIndex = index;
	return true;
}

void IconFromObjectDialog::populateShipTree()
{
	_shipTree->clear();

	auto wingTree = BriefingEditorDialogModel::getWingShipTree();

	for (const auto& entry : wingTree) {
		if (entry.wingName.empty()) {
			// Ungrouped ships, at root level
			for (const auto& ship : entry.ships) {
				auto* item = new QTreeWidgetItem(_shipTree);
				item->setText(0, QString::fromStdString(ship.name));
				item->setData(0, RoleKind, KindShip);
				item->setData(0, RoleIndex, ship.shipIndex);
			}
		} else {
			// Wing group
			auto* wingItem = new QTreeWidgetItem(_shipTree);
			wingItem->setText(0, QString::fromStdString(entry.wingName) + " (Wing)");
			wingItem->setData(0, RoleKind, KindWing);
			wingItem->setData(0, RoleIndex, entry.wingIndex);

			for (const auto& ship : entry.ships) {
				auto* shipItem = new QTreeWidgetItem(wingItem);
				shipItem->setText(0, QString::fromStdString(ship.name));
				shipItem->setData(0, RoleKind, KindShip);
				shipItem->setData(0, RoleIndex, ship.shipIndex);
			}

			wingItem->setExpanded(true);
		}
	}
}

void IconFromObjectDialog::populatePropTree()
{
	_propTree->clear();

	for (const auto& prop : BriefingEditorDialogModel::getPlacedPropList()) {
		auto* item = new QTreeWidgetItem(_propTree);
		item->setText(0, QString::fromStdString(prop.name) + " (" + QString::fromStdString(prop.className) + ")");
		item->setData(0, RoleKind, KindProp);
		item->setData(0, RoleIndex, prop.propIndex);
	}
}

void IconFromObjectDialog::applyFilter(const QString& text)
{
	for (int i = 0; i < _shipTree->topLevelItemCount(); ++i) {
		auto* topItem = _shipTree->topLevelItem(i);

		if (topItem->childCount() == 0) {
			// Root-level ship
			bool matches = text.isEmpty() ||
				topItem->text(0).contains(text, Qt::CaseInsensitive);
			topItem->setHidden(!matches);
		} else {
			// Wing group: show if any child matches
			bool anyChildVisible = false;
			for (int j = 0; j < topItem->childCount(); ++j) {
				auto* child = topItem->child(j);
				bool matches = text.isEmpty() ||
					child->text(0).contains(text, Qt::CaseInsensitive);
				child->setHidden(!matches);
				if (matches)
					anyChildVisible = true;
			}
			// Also show wing if wing name itself matches
			if (!anyChildVisible && !text.isEmpty()) {
				anyChildVisible = topItem->text(0).contains(text, Qt::CaseInsensitive);
				if (anyChildVisible) {
					// Show all children
					for (int j = 0; j < topItem->childCount(); ++j)
						topItem->child(j)->setHidden(false);
				}
			}
			topItem->setHidden(!anyChildVisible);
			if (anyChildVisible)
				topItem->setExpanded(true);
		}
	}

	// Props match on name or class, both of which are in the item text
	for (int i = 0; i < _propTree->topLevelItemCount(); ++i) {
		auto* item = _propTree->topLevelItem(i);
		item->setHidden(!text.isEmpty() && !item->text(0).contains(text, Qt::CaseInsensitive));
	}
}

void IconFromObjectDialog::onFilterTextChanged(const QString& text)
{
	applyFilter(text);
}

void IconFromObjectDialog::onItemDoubleClicked(QTreeWidgetItem* item, int /*column*/)
{
	if (selectItem(item))
		accept();
}

} // namespace fso::fred::dialogs
