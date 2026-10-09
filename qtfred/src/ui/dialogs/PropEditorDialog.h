#pragma once

#include <QDialog>

#include <mission/dialogs/PropEditorDialogModel.h>
#include <missioneditor/sexp_tree_model.h>
#include <ui/FredView.h>

namespace Ui {
class PropEditorDialog;
}

namespace fso::fred::dialogs {

class PropEditorDialog : public QDialog, public SexpTreeEditorInterface {
	Q_OBJECT

 public:
	PropEditorDialog(FredView* parent, EditorViewport* viewport);
	~PropEditorDialog() override;

 protected:
	void changeEvent(QEvent* e) override;

 private slots:
	void on_propNameLineEdit_editingFinished();
	void on_nextButton_clicked();
	void on_prevButton_clicked();
	void on_layerCombo_currentIndexChanged(int index);
	void on_transformLockCheck_clicked();
	void on_groupsButton_clicked();
	// the objects the lock checkbox covers (a waypoint stands for its path)
	SCP_vector<int> transformLockObjects() const;
	void on_propClassCombo_currentIndexChanged(int index);
	void on_textureReplacementButton_clicked();

	void on_spawnCueTree_modified();
	void on_spawnCueTree_helpChanged(const QString&);
	void on_spawnCueTree_miniHelpChanged(const QString&);
	void on_spawnDelaySpinBox_valueChanged(int);
	void on_despawnCueTree_modified();
	void on_despawnCueTree_helpChanged(const QString&);
	void on_despawnCueTree_miniHelpChanged(const QString&);
	void on_despawnDelaySpinBox_valueChanged(int);

 private: // NOLINT(readability-redundant-access-specifiers)
	FredView*       _fredView;
	EditorViewport* _viewport;
	std::unique_ptr<::Ui::PropEditorDialog> ui;
	std::unique_ptr<PropEditorDialogModel> _model;

	void initializeUi();
	void updateUi();
	void updateCues();
};

}
