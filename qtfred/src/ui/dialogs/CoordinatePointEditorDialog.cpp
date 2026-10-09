#include "ui/dialogs/CoordinatePointEditorDialog.h"
#include "ui/dialogs/General/CheckBoxListDialog.h"
#include "ui/util/DialogUndo.h"
#include "ui/util/SignalBlockers.h"
#include "ui_CoordinatePointEditorDialog.h"

#include <QRadioButton>

#include <coordinate_points/coordinate_point.h>
#include <mission/commands/FredCommands.h>
#include <coordinate_points/coordinate_shapes.h>
#include <globalincs/globals.h>
#include <globalincs/linklist.h>
#include <mission/missionparse.h>
#include <mission/util.h>
#include <object/object.h>
#include <ui/util/menu.h>

namespace fso::fred::dialogs {

namespace {
// Merge-identity key for FieldEditCommand: signatures of the selected points.
// Edits of the same field merge only while the selection is unchanged; switching
// selection starts a new undo step instead of cross-wiring stale setters.
SCP_string cpSelectionKey(const SCP_vector<int>& objnums)
{
	SCP_string key;
	for (int objnum : objnums) {
		key += std::to_string(Objects[objnum].signature);
		key += ',';
	}
	return key;
}

// The shape kind and its resolved table index move together (a Custom shape carries both),
// so a shape edit captures/restores the pair as one value.
struct CpShapeState {
	CoordinatePointShapeKind kind;
	int                      tableIndex;
	bool operator==(const CpShapeState& o) const { return kind == o.kind && tableIndex == o.tableIndex; }
};
CpShapeState cpReadShape(mission_coordinate_point& cp) { return {cp.shape_kind, cp.shape_table_index}; }
void cpWriteShape(mission_coordinate_point& cp, const CpShapeState& v)
{
	cp.shape_kind        = v.kind;
	cp.shape_table_index = v.tableIndex;
}
} // namespace

template<typename T, typename ReadFn, typename WriteFn, typename ApplyFn>
void CoordinatePointEditorDialog::pushCoordinatePointField(int fieldId,
	const QString& text, ReadFn read, WriteFn write, ApplyFn applyModel)
{
	const auto& objnums = _model->getSelectedObjnums();

	// Snapshot each selected point's current value (keyed by signature) before the edit.
	SCP_vector<std::pair<int, T>> beforeList;
	for (int objnum : objnums) {
		auto* cp = find_coordinate_point_by_objnum(objnum);
		if (cp == nullptr) continue;
		beforeList.emplace_back(Objects[objnum].signature, read(*cp));
	}

	applyModel(); // model setter writes every selected point + refreshes the viewport

	auto* cmd = new FieldEditCommand<T>(fieldId, _viewport->editor, text, /*skipFirstRedo=*/true);
	cmd->setTargetKey(cpSelectionKey(objnums));
	for (const auto& [sig, before] : beforeList) {
		const int o = obj_get_by_signature(sig);
		if (o < 0) continue;
		auto* cp = find_coordinate_point_by_objnum(o);
		if (cp == nullptr) continue;
		T after = read(*cp);
		if (after == before) continue;
		// Init-capture sig (a structured binding can't be captured directly in C++17).
		cmd->addEntry(before, after, [sig = sig, write](const T& v) {
			const int o2 = obj_get_by_signature(sig);
			if (o2 < 0) return;
			auto* cp2 = find_coordinate_point_by_objnum(o2);
			if (cp2 != nullptr) write(*cp2, v);
		});
	}
	if (cmd->isEmpty()) { delete cmd; return; }
	_fredView->mainUndoStack()->push(cmd);
}

CoordinatePointEditorDialog::CoordinatePointEditorDialog(FredView* parent, EditorViewport* viewport) :
	QDialog(parent),
	_fredView(parent),
	_viewport(viewport),
	ui(new Ui::CoordinatePointEditorDialog()),
	_model(new CoordinatePointEditorDialogModel(this, viewport))
{
	this->setFocus();
	ui->setupUi(this);
	util::installMainStackUndoShortcuts(this, _fredView->mainUndoStack());

	ui->nameEdit->setMaxLength(NAME_LENGTH - 1);

	// -1 is the "mixed selection" sentinel; rendered blank via specialValueText.
	for (auto* sb : {ui->colorRSpinBox, ui->colorGSpinBox, ui->colorBSpinBox, ui->colorASpinBox}) {
		sb->setMinimum(-1);
		sb->setSpecialValueText(" ");
	}
	ui->escortPrioritySpinBox->setMinimum(-1);
	ui->escortPrioritySpinBox->setSpecialValueText(" ");
	// Angle's real range is [-360, 360]; drop the floor one step lower so that sentinel value can
	// render blank (specialValueText) when the selection's angles disagree.
	ui->angleSpinBox->setMinimum(-361.0);
	ui->angleSpinBox->setSpecialValueText(" ");
	// Sides/Points/InnerRadius carry their sentinel minimum in the .ui, but a whitespace-only
	// specialValueText there gets trimmed to empty (which disables the blank), so set it in code
	// like the spinboxes above. Their minimums (from the .ui) are already correct.
	ui->sidesSpinBox->setSpecialValueText(" ");
	ui->pointsSpinBox->setSpecialValueText(" ");
	ui->innerRadiusSpinBox->setSpecialValueText(" ");

	// Block combo signals during initial populate. Without this, addItem() fires
	// currentIndexChanged on the auto-connected slot and clobbers the model's value
	// (e.g. a freshly-created Diamond point gets overwritten with Triangle, the first
	// enum entry) before updateUi() ever reads the real state.
	{
		util::SignalBlockers blockers(this);

		// Multiplayer team combo: "Any" + one entry per TVT team. Mixed-selection state
		// is shown by setCurrentIndex(-1) (blank); selecting any real entry commits.
		ui->multiTeamCombo->clear();
		ui->multiTeamCombo->addItem("Any team", -1);
		for (const auto& team : Mission_event_teams_tvt) {
			ui->multiTeamCombo->addItem(QString::fromStdString(team.first), team.second);
		}

		ui->sizeSpinBox->setMinimum(-0.01);  // negative is the "mixed" sentinel
		ui->sizeSpinBox->setMaximum(static_cast<double>(COORDINATE_POINT_SIZE_MAX));
		ui->sizeSpinBox->setSingleStep(0.1);
		ui->sizeSpinBox->setSpecialValueText(" ");

		// The shape kind (NGon / Star / Custom) is chosen by the radio buttons; this combo lists
		// only the tabled shapes and is active when Custom is selected. UserRole carries the
		// table index (the model's "shape id" for a tabled shape, >= 0).
		ui->shapeCombo->clear();
		for (int i = 0; i < static_cast<int>(Coordinate_shapes.size()); ++i) {
			ui->shapeCombo->addItem(QString::fromStdString(Coordinate_shapes[i].name), i);
		}
	}

	initializeUi();
	updateUi();

	connect(_model.get(), &CoordinatePointEditorDialogModel::coordinatePointMarkingChanged, this, [this] {
		initializeUi();
		updateUi();
	});

	// "Select Coordinate Point" menu: jump the editor to any coordinate point in the
	// mission. Walking obj_used_list rather than Coordinate_points matches the other
	// object editors and yields the objnum directly; the two are kept in the same
	// order by resort_coordinate_points_in_obj_used_list().
	Editor* editor = viewport->editor;
	util::installSelectMenu(
		this,
		viewport,
		[]() {
			std::vector<util::SelectMenuEntry> entries;
			for (auto* ptr = GET_FIRST(&obj_used_list); ptr != END_OF_LIST(&obj_used_list); ptr = GET_NEXT(ptr)) {
				if (ptr->type != OBJ_COORDINATE_POINT) {
					continue;
				}
				const auto* cp = find_coordinate_point_by_objnum(OBJ_INDEX(ptr));
				if (cp != nullptr) {
					entries.push_back({QString::fromStdString(cp->name), OBJ_INDEX(ptr)});
				}
			}
			return entries;
		},
		[this, editor]() { return _model->hasMultipleSelection() ? -1 : editor->currentObject; },
		[editor](int objnum) {
			editor->unmark_all();
			editor->selectObject(objnum);
		},
		tr("&Select Coordinate Point"));

	resize(QDialog::sizeHint());
}

CoordinatePointEditorDialog::~CoordinatePointEditorDialog() = default;

void CoordinatePointEditorDialog::changeEvent(QEvent* e)
{
	if (e->type() == QEvent::ActivationChange && isActiveWindow())
		_fredView->undoGroup()->setActiveStack(_fredView->mainUndoStack());
	QDialog::changeEvent(e);
}

void CoordinatePointEditorDialog::initializeUi()
{
	util::SignalBlockers blockers(this);

	ui->layerCombo->clear();
	for (const auto& name : _viewport->getLayerNames()) {
		ui->layerCombo->addItem(QString::fromStdString(name), QString::fromStdString(name));
	}

	const bool enabled = _model->hasValidSelection();
	const bool hasAny = _model->hasAnyCoordinatePointsInMission();
	const bool multi = _model->hasMultipleSelection();

	ui->nameEdit->setEnabled(enabled && !multi);
	ui->displayNameEdit->setEnabled(enabled);
	ui->categoryEdit->setEnabled(enabled);
	// Shape kind radios + the tabled-shape combo are gated in updateUi(): NGon/Star follow the
	// selection, Custom additionally needs at least one tabled shape, and the combo is only live
	// for Custom. Baseline-disable here so they grey out when no point is selected.
	ui->shapeNGonRadio->setEnabled(false);
	ui->shapeStarRadio->setEnabled(false);
	ui->shapeCustomRadio->setEnabled(false);
	ui->shapeCombo->setEnabled(false);
	// Sides/Points/InnerRadius are gated per-kind in updateUi(); only the always-on rows are
	// touched here, plus a default-disabled baseline so they grey out when no point is selected.
	ui->sidesSpinBox->setEnabled(false);
	ui->pointsSpinBox->setEnabled(false);
	ui->innerRadiusSpinBox->setEnabled(false);
	ui->angleSpinBox->setEnabled(enabled);
	ui->sizeSpinBox->setEnabled(enabled);
	ui->escortPrioritySpinBox->setEnabled(enabled);
	// Team selector is greyed out when the mission isn't a team mission.
	ui->multiTeamCombo->setEnabled(enabled && CoordinatePointEditorDialogModel::missionIsMultiTeam());
	ui->flagsButton->setEnabled(enabled);
	ui->layerCombo->setEnabled(enabled);
	ui->transformLockCheck->setEnabled(enabled);
	ui->groupsButton->setEnabled(enabled);
	ui->colorRSpinBox->setEnabled(enabled);
	ui->colorGSpinBox->setEnabled(enabled);
	ui->colorBSpinBox->setEnabled(enabled);
	ui->colorASpinBox->setEnabled(enabled);
	ui->prevPointButton->setEnabled(hasAny);
	ui->nextPointButton->setEnabled(hasAny);

	if (multi) {
		setWindowTitle(QString("Edit %1 Coordinate Points").arg(_model->getSelectionCount()));
	} else {
		setWindowTitle("Coordinate Point Editor");
	}
}

void CoordinatePointEditorDialog::updateUi()
{
	util::SignalBlockers blockers(this);

	ui->nameEdit->setText(QString::fromStdString(_model->getCurrentName()));

	if (_model->isDisplayNameMixed()) {
		ui->displayNameEdit->setPlaceholderText("<mixed>");
		ui->displayNameEdit->setText("");
	} else {
		// Empty means the HUD falls back to Name; show that as the hint.
		ui->displayNameEdit->setPlaceholderText(_model->hasMultipleSelection()
			? tr("<Name>")
			: QString::fromStdString(_model->getCurrentName()));
		ui->displayNameEdit->setText(QString::fromStdString(_model->getDisplayName()));
	}

	if (_model->isCategoryMixed()) {
		ui->categoryEdit->setPlaceholderText("<mixed>");
		ui->categoryEdit->setText("");
	} else {
		ui->categoryEdit->setPlaceholderText("");
		ui->categoryEdit->setText(QString::fromStdString(_model->getCategory()));
	}

	// Shape kind is chosen by the NGon / Star / Custom radios; the combo lists the tabled shapes
	// and is only live for Custom. When the kind is mixed across a multi-selection, no radio is
	// checked (which needs a brief drop of auto-exclusivity to reach the all-off state).
	const bool kindMixed = _model->isShapeKindMixed();
	const auto kind = _model->getShapeKind();
	const bool selectionEnabled = _model->hasValidSelection();
	const bool hasTabled = !Coordinate_shapes.empty();
	const bool ngonChecked   = !kindMixed && (kind == CoordinatePointShapeKind::NGon);
	const bool starChecked   = !kindMixed && (kind == CoordinatePointShapeKind::Star);
	const bool customChecked = !kindMixed && (kind == CoordinatePointShapeKind::Tabled);

	const std::pair<QRadioButton*, bool> radios[] = {
		{ui->shapeNGonRadio, ngonChecked},
		{ui->shapeStarRadio, starChecked},
		{ui->shapeCustomRadio, customChecked},
	};
	for (const auto& [rb, on] : radios) {
		rb->setAutoExclusive(false);
		rb->setChecked(on);
		rb->setAutoExclusive(true);
	}
	ui->shapeNGonRadio->setEnabled(selectionEnabled);
	ui->shapeStarRadio->setEnabled(selectionEnabled);
	ui->shapeCustomRadio->setEnabled(selectionEnabled && hasTabled);

	// Point the combo at the resolved tabled shape; it's only interactive for Custom.
	if (customChecked) {
		ui->shapeCombo->setCurrentIndex(ui->shapeCombo->findData(_model->getShapeTableIndex()));
	}
	ui->shapeCombo->setEnabled(selectionEnabled && customChecked);

	// Per-kind parameter rows stay in place; we just disable the ones that don't apply to the
	// resolved kind. When kind is mixed across a multi-selection, all three per-kind rows are
	// disabled until the user picks a kind. Angle is always editable. Rows are also disabled
	// up front when there's no valid selection (handled by initializeUi).
	const bool ngonEnabled = selectionEnabled && ngonChecked;
	const bool starEnabled = selectionEnabled && starChecked;
	ui->sidesLabel->setEnabled(ngonEnabled);
	ui->sidesSpinBox->setEnabled(ngonEnabled);
	ui->pointsLabel->setEnabled(starEnabled);
	ui->pointsSpinBox->setEnabled(starEnabled);
	ui->innerRadiusLabel->setEnabled(starEnabled);
	ui->innerRadiusSpinBox->setEnabled(starEnabled);

	// Every value spinbox uses the same scheme as Size/Escort Priority: a below-range sentinel
	// value renders blank via the spinbox's specialValueText, the model setter rejects it and
	// clamps real edits, and each valueChanged handler re-runs updateUi() so a single selection
	// snaps back to the real value. So here we only ever push the sentinel (mixed) or the value.
	// For the mixed/blank case, push each spinbox's OWN minimum() rather than a literal sentinel:
	// specialValueText renders only when value() == minimum() exactly, and a hardcoded double can
	// miss that equality at the box's rounding precision (the QDoubleSpinBox blank-fails otherwise).
	ui->sidesSpinBox->setValue(_model->isSidesMixed() ? ui->sidesSpinBox->minimum() : _model->getSides());
	ui->pointsSpinBox->setValue(_model->isPointsMixed() ? ui->pointsSpinBox->minimum() : _model->getPoints());
	ui->innerRadiusSpinBox->setValue(_model->isInnerRadiusMixed() ? ui->innerRadiusSpinBox->minimum()
		: static_cast<double>(_model->getInnerRadius()));
	ui->angleSpinBox->setValue(_model->isAngleMixed() ? ui->angleSpinBox->minimum()
		: static_cast<double>(_model->getAngle()));

	ui->sizeSpinBox->setValue(_model->isSizeMixed() ? ui->sizeSpinBox->minimum() : static_cast<double>(_model->getSize()));
	ui->escortPrioritySpinBox->setValue(_model->isEscortPriorityMixed() ? ui->escortPrioritySpinBox->minimum() : _model->getEscortPriority());
	if (_model->isMultiTeamMixed()) {
		ui->multiTeamCombo->setCurrentIndex(-1);  // blank when values differ across selection
	} else {
		ui->multiTeamCombo->setCurrentIndex(ui->multiTeamCombo->findData(_model->getMultiTeam()));
	}

	ui->layerCombo->setCurrentIndex(ui->layerCombo->findData(QString::fromStdString(_model->getLayer())));
	{
		const auto lockState = transformLockState(transformLockObjects());
		ui->transformLockCheck->setTristate(lockState == Qt::PartiallyChecked);
		ui->transformLockCheck->setCheckState(lockState);
	}
	ui->groupsButton->setText(tr("Groups: %1").arg(selectionGroupsText(transformLockObjects())));

	ui->colorRSpinBox->setValue(_model->isColorRMixed() ? ui->colorRSpinBox->minimum() : _model->getColorR());
	ui->colorGSpinBox->setValue(_model->isColorGMixed() ? ui->colorGSpinBox->minimum() : _model->getColorG());
	ui->colorBSpinBox->setValue(_model->isColorBMixed() ? ui->colorBSpinBox->minimum() : _model->getColorB());
	ui->colorASpinBox->setValue(_model->isColorAMixed() ? ui->colorASpinBox->minimum() : _model->getColorA());

	updateColorSwatch();
}

void CoordinatePointEditorDialog::updateColorSwatch()
{
	if (_model->hasAnyColorMixed()) {
		ui->colorSwatch->setText("?");
		ui->colorSwatch->setAlignment(Qt::AlignCenter);
		ui->colorSwatch->setStyleSheet("background: #888; color: white;"
		                               "border: 1px solid #444; border-radius: 3px;");
		return;
	}
	ui->colorSwatch->setText("");
	ui->colorSwatch->setStyleSheet(QString("background: rgba(%1,%2,%3,%4);"
	                                       "border: 1px solid #444; border-radius: 3px;")
	        .arg(_model->getColorR())
	        .arg(_model->getColorG())
	        .arg(_model->getColorB())
	        .arg(_model->getColorA() / 255.0));
}

void CoordinatePointEditorDialog::on_prevPointButton_clicked()
{
	_model->selectPreviousPoint();
}

void CoordinatePointEditorDialog::on_nextPointButton_clicked()
{
	_model->selectNextPoint();
}

void CoordinatePointEditorDialog::on_nameEdit_editingFinished()
{
	const SCP_string oldName = _model->getCurrentName();
	const SCP_string typed   = ui->nameEdit->text().toUtf8().constData();
	const auto& objnums = _model->getSelectedObjnums();

	if (typed == oldName)
		return;

	if (!_model->setCurrentName(typed)) {
		util::SignalBlockers blockers(this);
		ui->nameEdit->setText(QString::fromStdString(_model->getCurrentName()));
		return;
	}

	const SCP_string newName = _model->getCurrentName();
	// setCurrentName only applies to a single selection; guard the command build accordingly.
	// RenameObjectCommand (skipFirstRedo) reverses the SEXP-reference rewrite on undo/redo too,
	// since its applyName routes coordinate points through Editor::rename_coordinate_point.
	if (newName != oldName && objnums.size() == 1) {
		_fredView->mainUndoStack()->push(
			new RenameObjectCommand(objnums.front(), oldName, newName, _viewport->editor, /*skipFirstRedo=*/true));
	}

	// Reflect the committed (trimmed) name and clear the line edit's own text-undo history so a
	// following Ctrl+Z hits the mission stack rather than the typing history.
	util::SignalBlockers blockers(this);
	ui->nameEdit->setText(QString::fromStdString(_model->getCurrentName()));
}

void CoordinatePointEditorDialog::on_displayNameEdit_editingFinished()
{
	// editingFinished also fires on plain focus-out; don't let that overwrite a <mixed> field.
	if (!ui->displayNameEdit->isModified())
		return;
	const SCP_string displayName = ui->displayNameEdit->text().trimmed().toUtf8().constData();
	pushCoordinatePointField<SCP_string>(FieldId::CP_DisplayName, tr("Change Coordinate Point Display Name"),
		[](mission_coordinate_point& cp) { return cp.display_name; },
		[](mission_coordinate_point& cp, const SCP_string& v) { cp.display_name = v; },
		[&] { _model->setDisplayName(displayName); });
	updateUi();
}

void CoordinatePointEditorDialog::on_categoryEdit_editingFinished()
{
	// editingFinished also fires on plain focus-out; don't let that overwrite a <mixed> field.
	if (!ui->categoryEdit->isModified())
		return;
	const SCP_string category = ui->categoryEdit->text().toUtf8().constData();
	pushCoordinatePointField<SCP_string>(FieldId::CP_Category, tr("Change Coordinate Point Category"),
		[](mission_coordinate_point& cp) { return cp.category; },
		[](mission_coordinate_point& cp, const SCP_string& v) { cp.category = v; },
		[&] { _model->setCategory(category); });
	updateUi();
}

void CoordinatePointEditorDialog::on_shapeNGonRadio_toggled(bool checked)
{
	if (!checked)
		return;
	pushCoordinatePointField<CpShapeState>(FieldId::CP_ShapeKind, tr("Change Coordinate Point Shape"),
		cpReadShape, cpWriteShape, [&] { _model->setShapeId(-2); });
	updateUi();
}

void CoordinatePointEditorDialog::on_shapeStarRadio_toggled(bool checked)
{
	if (!checked)
		return;
	pushCoordinatePointField<CpShapeState>(FieldId::CP_ShapeKind, tr("Change Coordinate Point Shape"),
		cpReadShape, cpWriteShape, [&] { _model->setShapeId(-1); });
	updateUi();
}

void CoordinatePointEditorDialog::on_shapeCustomRadio_toggled(bool checked)
{
	if (!checked)
		return;
	// Commit whichever tabled shape the combo currently shows. The combo lists the tabled shapes
	// and the Custom radio is only enabled when at least one exists, so there is a valid entry.
	int index = ui->shapeCombo->currentIndex();
	if (index < 0 && ui->shapeCombo->count() > 0)
		index = 0;
	if (index < 0)
		return;
	const int shape_id = ui->shapeCombo->itemData(index).toInt();
	pushCoordinatePointField<CpShapeState>(FieldId::CP_ShapeKind, tr("Change Coordinate Point Shape"),
		cpReadShape, cpWriteShape, [&] { _model->setShapeId(shape_id); });
	updateUi();
}

void CoordinatePointEditorDialog::on_shapeCombo_currentIndexChanged(int index)
{
	if (index < 0)
		return;
	// The combo only holds tabled shapes; its data is the table index (the model's shape id for
	// a Custom shape). It's only interactive while Custom is selected.
	const int shape_id = ui->shapeCombo->itemData(index).toInt();
	pushCoordinatePointField<CpShapeState>(FieldId::CP_ShapeKind, tr("Change Coordinate Point Shape"),
		cpReadShape, cpWriteShape, [&] { _model->setShapeId(shape_id); });
	// Toggle which per-kind parameter rows are visible.
	updateUi();
}

void CoordinatePointEditorDialog::on_sidesSpinBox_valueChanged(int value)
{
	pushCoordinatePointField<int>(FieldId::CP_Sides, tr("Change Coordinate Point Sides"),
		[](mission_coordinate_point& cp) { return cp.shape_sides; },
		[](mission_coordinate_point& cp, const int& v) { cp.shape_sides = v; },
		[&] { _model->setSides(value); });
	updateUi();
}

void CoordinatePointEditorDialog::on_pointsSpinBox_valueChanged(int value)
{
	pushCoordinatePointField<int>(FieldId::CP_Points, tr("Change Coordinate Point Points"),
		[](mission_coordinate_point& cp) { return cp.shape_points; },
		[](mission_coordinate_point& cp, const int& v) { cp.shape_points = v; },
		[&] { _model->setPoints(value); });
	updateUi();
}

void CoordinatePointEditorDialog::on_innerRadiusSpinBox_valueChanged(double value)
{
	pushCoordinatePointField<float>(FieldId::CP_InnerRadius, tr("Change Coordinate Point Inner Radius"),
		[](mission_coordinate_point& cp) { return cp.shape_inner_radius; },
		[](mission_coordinate_point& cp, const float& v) { cp.shape_inner_radius = v; },
		[&] { _model->setInnerRadius(static_cast<float>(value)); });
	updateUi();
}

void CoordinatePointEditorDialog::on_angleSpinBox_valueChanged(double value)
{
	pushCoordinatePointField<float>(FieldId::CP_Angle, tr("Change Coordinate Point Angle"),
		[](mission_coordinate_point& cp) { return cp.shape_angle_deg; },
		[](mission_coordinate_point& cp, const float& v) { cp.shape_angle_deg = v; },
		[&] { _model->setAngle(static_cast<float>(value)); });
	updateUi();
}

void CoordinatePointEditorDialog::on_sizeSpinBox_valueChanged(double value)
{
	pushCoordinatePointField<float>(FieldId::CP_Size, tr("Change Coordinate Point Size"),
		[](mission_coordinate_point& cp) { return cp.size_scale; },
		[](mission_coordinate_point& cp, const float& v) { cp.size_scale = v; },
		[&] { _model->setSize(static_cast<float>(value)); });
	updateUi();
}

void CoordinatePointEditorDialog::on_escortPrioritySpinBox_valueChanged(int value)
{
	pushCoordinatePointField<int>(FieldId::CP_EscortPriority, tr("Change Coordinate Point Escort Priority"),
		[](mission_coordinate_point& cp) { return cp.escort_priority; },
		[](mission_coordinate_point& cp, const int& v) { cp.escort_priority = v; },
		[&] { _model->setEscortPriority(value); });
	updateUi();
}

void CoordinatePointEditorDialog::on_multiTeamCombo_currentIndexChanged(int index)
{
	if (index < 0)
		return;
	const int team = ui->multiTeamCombo->itemData(index).toInt();
	pushCoordinatePointField<int>(FieldId::CP_MultiTeam, tr("Change Coordinate Point Team"),
		[](mission_coordinate_point& cp) { return cp.multi_team; },
		[](mission_coordinate_point& cp, const int& v) { cp.multi_team = v; },
		[&] { _model->setMultiTeam(team); });
	updateUi();
}

void CoordinatePointEditorDialog::on_layerCombo_currentIndexChanged(int index)
{
	if (index < 0)
		return;
	const SCP_string newLayer = ui->layerCombo->itemData(index).toString().toUtf8().constData();

	// Layer moves push a MoveLayerCommand directly (its redo() applies the change via the viewport),
	// so we do not also call _model->setLayer(). One command covers the whole selection.
	SCP_vector<ObjectLayerChange> changes;
	for (int objnum : _model->getSelectedObjnums()) {
		SCP_string oldLayer = _viewport->getObjectLayerName(objnum);
		if (oldLayer == newLayer)
			continue;
		changes.push_back({ Objects[objnum].signature, std::move(oldLayer), newLayer });
	}
	if (changes.empty())
		return;
	_fredView->mainUndoStack()->push(
		new MoveLayerCommand(std::move(changes), _viewport, _viewport->editor));
}

void CoordinatePointEditorDialog::on_flagsButton_clicked()
{
	// Built from the parse tables, so a new CoordinatePoint flag shows up here with no UI work.
	QVector<std::pair<QString, int>> options;
	for (size_t i = 0; i < Num_parse_coordinate_point_flags; ++i) {
		const auto& def = Parse_coordinate_point_flags[i];
		options.append({QString::fromUtf8(def.name), _model->getFlagState(def.def)});
	}
	QVector<std::pair<QString, QString>> descriptions;
	for (size_t i = 0; i < Num_parse_coordinate_point_flag_descriptions; ++i) {
		const auto& desc = Parse_coordinate_point_flag_descriptions[i];
		for (size_t j = 0; j < Num_parse_coordinate_point_flags; ++j) {
			if (Parse_coordinate_point_flags[j].def == desc.def) {
				descriptions.append({QString::fromUtf8(Parse_coordinate_point_flags[j].name), QString::fromUtf8(desc.flag_desc)});
				break;
			}
		}
	}

	CheckBoxListDialog dlg(this);
	dlg.setCaption(tr("Coordinate Point Flags"));
	dlg.setTristate(_model->hasMultipleSelection());
	dlg.setOptions(options);
	dlg.setOptionDescriptions(descriptions);
	if (dlg.exec() != QDialog::Accepted)
		return;

	SCP_vector<std::pair<CoordinatePoint::Flags, int>> states;
	const auto result = dlg.getFlags();
	for (int i = 0; i < result.size() && i < static_cast<int>(Num_parse_coordinate_point_flags); ++i) {
		states.emplace_back(Parse_coordinate_point_flags[i].def, result[i].second);
	}

	pushCoordinatePointField<flagset<CoordinatePoint::Flags>>(FieldId::CP_Flags, tr("Change Coordinate Point Flags"),
		[](mission_coordinate_point& cp) { return cp.flags; },
		[](mission_coordinate_point& cp, const flagset<CoordinatePoint::Flags>& v) { cp.flags = v; },
		[&] { _model->applyFlagStates(states); });
	updateUi();
}

void CoordinatePointEditorDialog::on_colorRSpinBox_valueChanged(int value)
{
	pushCoordinatePointField<int>(FieldId::CP_ColorR, tr("Change Coordinate Point Color"),
		[](mission_coordinate_point& cp) { return static_cast<int>(cp.display_color.red); },
		[](mission_coordinate_point& cp, const int& v) {
			gr_init_alphacolor(&cp.display_color, v, cp.display_color.green, cp.display_color.blue, cp.display_color.alpha);
		},
		[&] { _model->setColorR(value); });
	updateUi();
}

void CoordinatePointEditorDialog::on_colorGSpinBox_valueChanged(int value)
{
	pushCoordinatePointField<int>(FieldId::CP_ColorG, tr("Change Coordinate Point Color"),
		[](mission_coordinate_point& cp) { return static_cast<int>(cp.display_color.green); },
		[](mission_coordinate_point& cp, const int& v) {
			gr_init_alphacolor(&cp.display_color, cp.display_color.red, v, cp.display_color.blue, cp.display_color.alpha);
		},
		[&] { _model->setColorG(value); });
	updateUi();
}

void CoordinatePointEditorDialog::on_colorBSpinBox_valueChanged(int value)
{
	pushCoordinatePointField<int>(FieldId::CP_ColorB, tr("Change Coordinate Point Color"),
		[](mission_coordinate_point& cp) { return static_cast<int>(cp.display_color.blue); },
		[](mission_coordinate_point& cp, const int& v) {
			gr_init_alphacolor(&cp.display_color, cp.display_color.red, cp.display_color.green, v, cp.display_color.alpha);
		},
		[&] { _model->setColorB(value); });
	updateUi();
}

void CoordinatePointEditorDialog::on_colorASpinBox_valueChanged(int value)
{
	pushCoordinatePointField<int>(FieldId::CP_ColorA, tr("Change Coordinate Point Color"),
		[](mission_coordinate_point& cp) { return static_cast<int>(cp.display_color.alpha); },
		[](mission_coordinate_point& cp, const int& v) {
			gr_init_alphacolor(&cp.display_color, cp.display_color.red, cp.display_color.green, cp.display_color.blue, v);
		},
		[&] { _model->setColorA(value); });
	updateUi();
}

SCP_vector<int> CoordinatePointEditorDialog::transformLockObjects() const
{
	return _model->getSelectedObjnums();
}

void CoordinatePointEditorDialog::on_groupsButton_clicked()
{
	_fredView->editSelectionGroups(transformLockObjects(), this);
	updateUi();
}

void CoordinatePointEditorDialog::on_transformLockCheck_clicked()
{
	const auto objs = transformLockObjects();
	// a partly locked selection locks the rest
	pushTransformLock(objs, transformLockState(objs) != Qt::Checked, _viewport->editor, _fredView->mainUndoStack());
	updateUi();
}

} // namespace fso::fred::dialogs
