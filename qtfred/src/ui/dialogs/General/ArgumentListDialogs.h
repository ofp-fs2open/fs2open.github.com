#pragma once

#include <missioneditor/sexp_tree_model.h>

#include <QDialog>
#include <QHash>
#include <QString>

#include <functional>

class QLabel;
class QListWidget;
class QPlainTextEdit;
class QPushButton;
class QTabWidget;

namespace fso::fred {
class FlagListWidget;
}

namespace fso::fred::dialogs {

// Edit Arguments As Text: an argument list (any-of and the others that list their values) as one
// value per line, in mission file syntax. Lines that can't be used are highlighted and keep OK
// disabled.
class ArgumentListTextDialog : public QDialog {
	Q_OBJECT
  public:
	// Reads one line; returns false for an empty line (no error) or one it can't use (error set)
	using LineParser = std::function<bool(const SCP_string& line, ArgumentListValue* value, SCP_string* error)>;

	ArgumentListTextDialog(QWidget* parent, const QString& title, const SCP_vector<ArgumentListValue>& values, LineParser parser);

	// the list, valid once the dialog is accepted
	const SCP_vector<ArgumentListValue>& values() const { return _values; }

  private:
	void validate();

	QPlainTextEdit* _edit = nullptr;
	QLabel* _status = nullptr;
	QPushButton* _ok = nullptr;
	LineParser _parser;
	SCP_vector<ArgumentListValue> _values;
};

// One tab of Choose Arguments: the values that can be checked there
struct ChooseArgumentsTab {
	QString title;
	SCP_vector<ArgumentListValue> options;
};

// Choose Arguments: tabs of checklists (ships, wings, messages...) beside the list itself, in
// order. Checking a name adds it to the end of the list and clearing it removes it; the list can be
// reordered by dragging or Move Up / Move Down, and values removed from it. Values that aren't in
// any tab (typed-in strings, numbers) show in the list too.
class ChooseArgumentsDialog : public QDialog {
	Q_OBJECT
  public:
	ChooseArgumentsDialog(QWidget* parent, const QString& title, SCP_vector<ChooseArgumentsTab> tabs, const SCP_vector<ArgumentListValue>& current);

	// the list, in the order shown
	SCP_vector<ArgumentListValue> values() const;

  private:
	void updateTabTitle(int tab);
	void appendItem(const QString& key);
	void addToList(const QString& key);
	void removeFromList(const QString& key);
	void removeSelected();
	void moveSelected(int step);
	// checks or clears a name in every tab that has it
	void setCheckedInTabs(const QString& key, bool checked);

	SCP_vector<ChooseArgumentsTab> _tabs;
	QHash<QString, ArgumentListValue> _byKey; // every value either side shows, by its text
	QTabWidget* _tabWidget = nullptr;
	SCP_vector<FlagListWidget*> _lists;
	QListWidget* _list = nullptr;
};

} // namespace fso::fred::dialogs
