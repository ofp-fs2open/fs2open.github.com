#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QTabWidget>
#include <QTreeWidget>

namespace fso::fred::dialogs {

class BriefingEditorDialogModel;

// Make Icon From: picks a ship or wing (Ships tab) or a placed prop (Props tab) to make a briefing icon from.
class IconFromObjectDialog : public QDialog {
	Q_OBJECT

public:
	explicit IconFromObjectDialog(QWidget* parent, BriefingEditorDialogModel* model);

	enum class SelectionKind {
		None,
		Ship,
		Wing,
		Prop
	};

	SelectionKind selectedKind() const;
	// index into Ships, Wings or Props, matching selectedKind()
	int selectedIndex() const;

private slots:
	void onFilterTextChanged(const QString& text);
	void onItemDoubleClicked(QTreeWidgetItem* item, int column);

private: // NOLINT(readability-redundant-access-specifiers)
	void populateShipTree();
	void populatePropTree();
	void applyFilter(const QString& text);
	bool selectItem(const QTreeWidgetItem* item);

	BriefingEditorDialogModel* _model;
	QTabWidget* _tabs;
	QTreeWidget* _shipTree;
	QTreeWidget* _propTree;
	QLineEdit* _filter;
	SelectionKind _selectedKind = SelectionKind::None;
	int _selectedIndex = -1;
};

} // namespace fso::fred::dialogs
