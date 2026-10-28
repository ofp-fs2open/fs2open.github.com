#pragma once

#include <globalincs/pstypes.h>

class QString;
class QWidget;

namespace fso::fred::util {

// The voice actor note editor shared by messages and briefing, debriefing and
// command briefing stages. Returns false if the user cancels; on OK, `note`
// holds the edited text.
bool editNote(QWidget* parent, const QString& title, const QString& prompt, SCP_string& note);

// "Add Note" or "Edit Note", for the button that opens the editor.
QString noteButtonText(const SCP_string& note);

} // namespace fso::fred::util
