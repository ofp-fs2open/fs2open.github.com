#include "NoteDialog.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QString>
#include <QTextEdit>
#include <QVBoxLayout>

namespace fso::fred::util {

bool editNote(QWidget* parent, const QString& title, const QString& prompt, SCP_string& note)
{
	QDialog dlg(parent);
	dlg.setWindowTitle(title);
	auto* layout = new QVBoxLayout(&dlg);
	auto* label = new QLabel(prompt, &dlg);
	auto* edit = new QTextEdit(&dlg);
	edit->setPlainText(QString::fromUtf8(note.c_str()));
	edit->setMinimumSize(700, 500); // big!
	auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);

	layout->addWidget(label);
	layout->addWidget(edit, 1);
	layout->addWidget(buttons);

	QObject::connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
	QObject::connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

	if (dlg.exec() != QDialog::Accepted)
		return false;

	note = edit->toPlainText().toUtf8().constData();
	return true;
}

QString noteButtonText(const SCP_string& note)
{
	return note.empty() ? QObject::tr("Add Note") : QObject::tr("Edit Note");
}

} // namespace fso::fred::util
