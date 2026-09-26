#pragma once

#include <QtWidgets/QComboBox>
#include <QtGui/QStandardItemModel>
#include <FredApplication.h>
namespace fso::fred {
class PersonaColorComboBox : public QComboBox {
	Q_OBJECT
  public:
	PersonaColorComboBox(QWidget* parent);

  protected:
	void changeEvent(QEvent* event) override;

  private:
	void refreshItemColors();
	static QStandardItemModel* getPersonaModel();
};
} // namespace fso::fred