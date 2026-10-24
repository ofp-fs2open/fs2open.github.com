#include "ui/dialogs/General/ArgumentListDialogs.h"

#include "ui/util/DialogEnterGuard.h"
#include "ui/widgets/FlagList.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSet>
#include <QShortcut>
#include <QSplitter>
#include <QTabBar>
#include <QTabWidget>
#include <QTextBlock>
#include <QVBoxLayout>

#include <algorithm>

namespace fso::fred::dialogs {

namespace {

QString valueKey(const ArgumentListValue& value)
{
	return QString::fromStdString(SexpTreeModel::argument_list_value_text(value));
}

} // namespace

// ---------------------------------------------------------------------------
// ArgumentListTextDialog
// ---------------------------------------------------------------------------

ArgumentListTextDialog::ArgumentListTextDialog(QWidget* parent,
	const QString& title,
	const SCP_vector<ArgumentListValue>& values,
	LineParser parser)
	: QDialog(parent), _parser(std::move(parser))
{
	setWindowTitle(title);
	setSizeGripEnabled(true);
	resize(360, 420);

	auto* layout = new QVBoxLayout(this);

	auto* help = new QLabel(tr("One argument per line. A variable is @name and a container &&name; quote a "
							   "value that starts with @, && or a quote mark."),
		this);
	help->setWordWrap(true);
	layout->addWidget(help);

	_edit = new QPlainTextEdit(this);
	_edit->setTabChangesFocus(true);
	_edit->setLineWrapMode(QPlainTextEdit::NoWrap);
	QStringList lines;
	for (const auto& value : values)
		lines << valueKey(value);
	_edit->setPlainText(lines.join('\n'));
	layout->addWidget(_edit, 1);

	_status = new QLabel(this);
	_status->setWordWrap(true);
	layout->addWidget(_status);

	auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	_ok = buttons->button(QDialogButtonBox::Ok);
	layout->addWidget(buttons);
	connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

	connect(_edit, &QPlainTextEdit::textChanged, this, &ArgumentListTextDialog::validate);
	validate();
}

void ArgumentListTextDialog::validate()
{
	_values.clear();
	QList<QTextEdit::ExtraSelection> marks;
	QString firstError;
	int problems = 0;

	QColor problemColor(Qt::red);
	problemColor.setAlpha(60);

	for (auto block = _edit->document()->begin(); block.isValid(); block = block.next()) {
		// A line of the box can itself hold line breaks: pasted text, or Shift+Enter, can bring in
		// soft line separators (U+2028) and lone carriage returns. Each piece is its own value, and
		// QString::trimmed() also drops Unicode spaces such as no-break spaces.
		QString text = block.text();
		text.replace(QChar::LineSeparator, QLatin1Char('\n')).replace(QChar::ParagraphSeparator, QLatin1Char('\n')).replace(QLatin1Char('\r'), QLatin1Char('\n'));

		bool blockHasProblem = false;
		for (const auto& piece : text.split(QLatin1Char('\n'))) {
			ArgumentListValue value;
			SCP_string error;
			if (_parser(piece.trimmed().toUtf8().constData(), &value, &error)) {
				_values.push_back(std::move(value));
			} else if (!error.empty()) {
				++problems;
				blockHasProblem = true;
				if (firstError.isEmpty())
					firstError = tr("Line %1: %2").arg(block.blockNumber() + 1).arg(QString::fromStdString(error));
			}
		}

		if (blockHasProblem) {
			QTextEdit::ExtraSelection mark;
			mark.cursor = QTextCursor(block);
			mark.format.setBackground(problemColor);
			mark.format.setProperty(QTextFormat::FullWidthSelection, true);
			marks << mark;
		}
	}
	_edit->setExtraSelections(marks);

	if (problems > 0) {
		_status->setText(problems == 1 ? firstError : tr("%1 lines can't be used. %2").arg(problems).arg(firstError));
	} else {
		_status->setText(_values.size() == 1 ? tr("1 argument") : tr("%1 arguments").arg(_values.size()));
	}
	_ok->setEnabled(problems == 0);
}

// ---------------------------------------------------------------------------
// ChooseArgumentsDialog
// ---------------------------------------------------------------------------

ChooseArgumentsDialog::ChooseArgumentsDialog(QWidget* parent,
	const QString& title,
	SCP_vector<ChooseArgumentsTab> tabs,
	const SCP_vector<ArgumentListValue>& current)
	: QDialog(parent), _tabs(std::move(tabs))
{
	fso::fred::util::allowEnterToAccept(this);
	setWindowTitle(title);
	setSizeGripEnabled(true);

	// every value either side can show, by its text
	for (const auto& value : current)
		_byKey.insert(valueKey(value), value);
	for (const auto& tab : _tabs) {
		for (const auto& option : tab.options)
			_byKey.insert(valueKey(option), option);
	}

	auto* layout = new QVBoxLayout(this);
	auto* splitter = new QSplitter(Qt::Horizontal, this);
	layout->addWidget(splitter, 1);

	// left: a checklist per kind of name
	_tabWidget = new QTabWidget(splitter);
	// every tab stays in view: the tab bar never scrolls, so the dialog is at least as wide as it
	_tabWidget->setUsesScrollButtons(false);
	_tabWidget->tabBar()->setExpanding(false);

	QSet<QString> currentKeys;
	for (const auto& value : current)
		currentKeys.insert(valueKey(value));

	for (int t = 0; t < static_cast<int>(_tabs.size()); ++t) {
		QVector<std::pair<QString, int>> flags;
		for (const auto& option : _tabs[t].options) {
			const QString key = valueKey(option);
			flags.append({key, currentKeys.contains(key) ? Qt::Checked : Qt::Unchecked});
		}
		auto* list = new FlagListWidget(_tabWidget);
		list->setFlags(flags);
		_lists.push_back(list);
		_tabWidget->addTab(list, _tabs[t].title);
		updateTabTitle(t);
		connect(list, &FlagListWidget::flagsChanged, this, [this, t]() { updateTabTitle(t); });
		// checking adds to the end of the list, clearing removes it, so values go in click order
		connect(list, &FlagListWidget::flagToggled, this, [this](const QString& key, int checked) {
			if (checked == Qt::Checked)
				addToList(key);
			else
				removeFromList(key);
		});
	}

	// right: the list itself, in order
	auto* listPanel = new QWidget(splitter);
	auto* listLayout = new QVBoxLayout(listPanel);
	listLayout->setContentsMargins(0, 0, 0, 0);
	auto* listLabel = new QLabel(tr("Arguments, in order. Drag to reorder."), listPanel);
	listLabel->setWordWrap(true);
	listLayout->addWidget(listLabel);

	_list = new QListWidget(listPanel);
	_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
	_list->setDragDropMode(QAbstractItemView::InternalMove);
	_list->setDefaultDropAction(Qt::MoveAction);
	for (const auto& value : current)
		appendItem(valueKey(value));
	listLayout->addWidget(_list, 1);

	auto* listButtons = new QHBoxLayout();
	auto* upButton = new QPushButton(tr("Move Up"), listPanel);
	auto* downButton = new QPushButton(tr("Move Down"), listPanel);
	auto* removeButton = new QPushButton(tr("Remove"), listPanel);
	listButtons->addWidget(upButton);
	listButtons->addWidget(downButton);
	listButtons->addStretch(1);
	listButtons->addWidget(removeButton);
	listLayout->addLayout(listButtons);
	connect(upButton, &QPushButton::clicked, this, [this]() { moveSelected(-1); });
	connect(downButton, &QPushButton::clicked, this, [this]() { moveSelected(1); });
	connect(removeButton, &QPushButton::clicked, this, &ChooseArgumentsDialog::removeSelected);

	auto* deleteShortcut = new QShortcut(QKeySequence::Delete, _list);
	deleteShortcut->setContext(Qt::WidgetShortcut);
	connect(deleteShortcut, &QShortcut::activated, this, &ChooseArgumentsDialog::removeSelected);

	auto updateButtons = [this, upButton, downButton, removeButton]() {
		const bool any = !_list->selectedItems().isEmpty();
		upButton->setEnabled(any);
		downButton->setEnabled(any);
		removeButton->setEnabled(any);
	};
	connect(_list, &QListWidget::itemSelectionChanged, this, updateButtons);
	updateButtons();

	splitter->addWidget(_tabWidget);
	splitter->addWidget(listPanel);
	splitter->setStretchFactor(0, 3);
	splitter->setStretchFactor(1, 1);

	auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	layout->addWidget(buttons);
	connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

	// wide by default, and never narrower than the tab bar (with room for checked counts) plus the list
	const int tabsWidth = _tabWidget->tabBar()->sizeHint().width() + 80;
	resize(std::max(1400, tabsWidth + 360), 560);
	splitter->setSizes({std::max(1040, tabsWidth), 360});
}

void ChooseArgumentsDialog::appendItem(const QString& key)
{
	auto* item = new QListWidgetItem(key, _list);
	item->setData(Qt::UserRole, key);
	item->setFlags((item->flags() | Qt::ItemIsDragEnabled) & ~Qt::ItemIsDropEnabled);
}

void ChooseArgumentsDialog::addToList(const QString& key)
{
	if (_list->findItems(key, Qt::MatchExactly | Qt::MatchCaseSensitive).isEmpty())
		appendItem(key);
	// the same name in another tab shows checked too
	setCheckedInTabs(key, true);
}

void ChooseArgumentsDialog::removeFromList(const QString& key)
{
	for (auto* item : _list->findItems(key, Qt::MatchExactly | Qt::MatchCaseSensitive))
		delete item;
	setCheckedInTabs(key, false);
}

void ChooseArgumentsDialog::removeSelected()
{
	QSet<QString> keys;
	for (auto* item : _list->selectedItems())
		keys.insert(item->data(Qt::UserRole).toString());
	for (const auto& key : keys)
		removeFromList(key);
}

void ChooseArgumentsDialog::moveSelected(int step)
{
	// move each selected row one place, keeping a block of selected rows together
	QList<int> rows;
	for (auto* item : _list->selectedItems())
		rows << _list->row(item);
	std::sort(rows.begin(), rows.end());
	if (rows.isEmpty() || (step < 0 && rows.first() == 0) || (step > 0 && rows.last() == _list->count() - 1))
		return;
	if (step > 0)
		std::reverse(rows.begin(), rows.end());

	for (const int row : rows) {
		auto* item = _list->takeItem(row);
		_list->insertItem(row + step, item);
		item->setSelected(true);
	}
	_list->scrollToItem(_list->item(rows.first() + step));
}

void ChooseArgumentsDialog::setCheckedInTabs(const QString& key, bool checked)
{
	const int state = checked ? Qt::Checked : Qt::Unchecked;
	for (auto* list : _lists) {
		auto flags = list->getFlags();
		bool changed = false;
		for (auto& flag : flags) {
			if (flag.first == key && flag.second != state) {
				flag.second = state;
				changed = true;
			}
		}
		// setFlags() rebuilds the list without flagToggled, so this doesn't feed back
		if (changed)
			list->setFlags(flags);
	}
}

void ChooseArgumentsDialog::updateTabTitle(int tab)
{
	int checked = 0;
	for (const auto& flag : _lists[tab]->getFlags()) {
		if (flag.second == Qt::Checked)
			++checked;
	}
	const QString& title = _tabs[tab].title;
	_tabWidget->setTabText(tab, checked > 0 ? tr("%1 (%2)").arg(title).arg(checked) : title);
}

SCP_vector<ArgumentListValue> ChooseArgumentsDialog::values() const
{
	SCP_vector<ArgumentListValue> result;
	for (int row = 0; row < _list->count(); ++row) {
		const auto it = _byKey.constFind(_list->item(row)->data(Qt::UserRole).toString());
		if (it != _byKey.constEnd())
			result.push_back(it.value());
	}
	return result;
}

} // namespace fso::fred::dialogs
