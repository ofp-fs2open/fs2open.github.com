#pragma once
#include <QDialog>
#include <mission/dialogs/CoordinatePointEditorDialogModel.h>
#include <mission/commands/FredCommands.h>
#include <ui/FredView.h>

namespace fso::fred::dialogs {

namespace Ui {
class CoordinatePointEditorDialog;
}

class CoordinatePointEditorDialog : public QDialog {
	Q_OBJECT
public:
	CoordinatePointEditorDialog(FredView* parent, EditorViewport* viewport);
	~CoordinatePointEditorDialog() override;

protected:
	void changeEvent(QEvent* e) override;

private slots:
	void on_prevPointButton_clicked();
	void on_nextPointButton_clicked();
	void on_nameEdit_editingFinished();
	void on_displayNameEdit_editingFinished();
	void on_categoryEdit_editingFinished();
	void on_shapeNGonRadio_toggled(bool checked);
	void on_shapeStarRadio_toggled(bool checked);
	void on_shapeCustomRadio_toggled(bool checked);
	void on_shapeCombo_currentIndexChanged(int index);
	void on_sidesSpinBox_valueChanged(int value);
	void on_pointsSpinBox_valueChanged(int value);
	void on_innerRadiusSpinBox_valueChanged(double value);
	void on_angleSpinBox_valueChanged(double value);
	void on_sizeSpinBox_valueChanged(double value);
	void on_escortPrioritySpinBox_valueChanged(int value);
	void on_multiTeamCombo_currentIndexChanged(int index);
	void on_layerCombo_currentIndexChanged(int index);
	void on_transformLockCheck_clicked();
	void on_groupsButton_clicked();
	// the objects the lock checkbox covers (a waypoint stands for its path)
	SCP_vector<int> transformLockObjects() const;
	void on_flagsButton_clicked();
	void on_colorRSpinBox_valueChanged(int value);
	void on_colorGSpinBox_valueChanged(int value);
	void on_colorBSpinBox_valueChanged(int value);
	void on_colorASpinBox_valueChanged(int value);

private: // NOLINT(readability-redundant-access-specifiers)
	FredView*       _fredView;
	EditorViewport* _viewport;
	std::unique_ptr<Ui::CoordinatePointEditorDialog> ui;
	std::unique_ptr<CoordinatePointEditorDialogModel> _model;

	void initializeUi();
	void updateUi();
	void updateColorSwatch();

	// Applies a single-field edit through the model (which writes every selected point), then
	// pushes a merging FieldEditCommand to the main stack so the change is undoable. read/write
	// operate on one mission_coordinate_point; applyModel invokes the matching model setter.
	template<typename T, typename ReadFn, typename WriteFn, typename ApplyFn>
	void pushCoordinatePointField(int fieldId, const QString& text, ReadFn read, WriteFn write, ApplyFn applyModel);
};

} // namespace fso::fred::dialogs
