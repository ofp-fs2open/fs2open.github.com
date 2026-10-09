#include "ui/dialogs/PropEditorDialog.h"
#include "ui/dialogs/PropTextureReplacementDialog.h"

#include <ui/util/DialogUndo.h>

#include "ui_PropEditorDialog.h"

#include <globalincs/globals.h>
#include <globalincs/linklist.h>
#include <mission/commands/FredCommands.h>
#include <mission/missionparse.h>
#include <mission/object.h>
#include <object/object.h>
#include <prop/prop.h>
#include <ui/util/SignalBlockers.h>
#include <ui/util/menu.h>
#include <ui/widgets/FlagList.h>
#include <ui/widgets/sexp_tree_view.h>

#include <QMetaObject>
#include <QShortcut>

namespace fso::fred::dialogs {

PropEditorDialog::PropEditorDialog(FredView* parent, EditorViewport* viewport)
	: QDialog(parent), SexpTreeEditorInterface(flagset<TreeFlags>()), _fredView(parent), _viewport(viewport),
	  ui(new ::Ui::PropEditorDialog()), _model(new PropEditorDialogModel(this, viewport)) {

	ui->setupUi(this);
	util::installMainStackUndoShortcuts(this, _fredView->mainUndoStack());

	// F6 / Shift+F6 cycle to the next / previous prop, mirroring the Next/Prev buttons.
	auto* nextShortcut = new QShortcut(QKeySequence(Qt::Key_F6), this);
	connect(nextShortcut, &QShortcut::activated, this, [this] { ui->nextButton->click(); });
	auto* prevShortcut = new QShortcut(QKeySequence(QStringLiteral("Shift+F6")), this);
	connect(prevShortcut, &QShortcut::activated, this, [this] { ui->prevButton->click(); });

	ui->propNameLineEdit->setMaxLength(NAME_LENGTH - 1);

	// SEXP operator help panel, toggled via Preferences (same as the Ship/Wing editors)
	ui->cueHelpTitle->setVisible(viewport->Show_sexp_help_prop_editor);
	ui->cueHelpText->setVisible(viewport->Show_sexp_help_prop_editor);

	initializeUi();
	updateUi();
	updateCues();

	// Everything refreshes off the model's own signal, which initializeData() emits once
	// it has re-read mission data. Driving updateCues() straight off the editor's
	// selection/missionChanged signals instead would race the model: since #7644 the
	// model's refresh is deferred through a timer, so the cue trees would be reloaded
	// from the stale cached formula indices. For a cue undo that is not merely stale --
	// SexpCueEditCommand frees the old sexp chain before emitting missionChanged, so
	// load_tree() would walk freed nodes.
	connect(_model.get(), &PropEditorDialogModel::modelDataChanged, this, [this]() {
		initializeUi();
		updateUi();
		updateCues();
	});

	connect(ui->spawnCueTree, &sexp_tree_view::modified, this, &PropEditorDialog::on_spawnCueTree_modified);
	connect(ui->spawnCueTree, &sexp_tree_view::helpChanged, this, &PropEditorDialog::on_spawnCueTree_helpChanged);
	connect(ui->spawnCueTree, &sexp_tree_view::miniHelpChanged, this, &PropEditorDialog::on_spawnCueTree_miniHelpChanged);
	connect(ui->despawnCueTree, &sexp_tree_view::modified, this, &PropEditorDialog::on_despawnCueTree_modified);
	connect(ui->despawnCueTree, &sexp_tree_view::helpChanged, this, &PropEditorDialog::on_despawnCueTree_helpChanged);
	connect(ui->despawnCueTree, &sexp_tree_view::miniHelpChanged, this, &PropEditorDialog::on_despawnCueTree_miniHelpChanged);

	connect(ui->propFlagsListWidget, &fso::fred::FlagListWidget::flagsChanged, this,
		[this](const QVector<std::pair<QString, int>>& snapshot) {
			const auto& labels = _model->getFlagLabels();
			const auto& selectedObjs = _model->getSelectedPropObjects();

			auto* cmd = new FieldEditCommand<bool>(
				FieldId::Prop_Flags, _viewport->editor, tr("Change Prop Flag"), true);
			// Merge-identity key: the selected prop signatures, so flag edits
			// merge only while the selection is unchanged.
			SCP_string targetKey;
			for (int obj_idx : selectedObjs) {
				if (query_valid_object(obj_idx)) {
					targetKey += std::to_string(Objects[obj_idx].signature);
					targetKey += ',';
				}
			}
			cmd->setTargetKey(std::move(targetKey));

			for (const auto& [name, newState] : snapshot) {
				if (newState == Qt::PartiallyChecked) continue;
				for (size_t i = 0; i < labels.size(); ++i) {
					if (name != QString::fromStdString(labels[i].first)) continue;
					const size_t flag_index = labels[i].second;
					for (int obj_idx : selectedObjs) {
						if (!query_valid_object(obj_idx) || Objects[obj_idx].type != OBJ_PROP) continue;
						const bool before = PropEditorDialogModel::getFlagValueForObject(Objects[obj_idx], flag_index);
						const bool after = (newState == Qt::Checked);
						if (before == after) continue;
						const int sig = Objects[obj_idx].signature;
						cmd->addEntry(before, after, [sig, flag_index](const bool& v) {
							const int cur = obj_get_by_signature(sig);
							if (cur < 0 || Objects[cur].type != OBJ_PROP) return;
							const auto& def = Parse_prop_flags[flag_index];
							if (!stricmp(def.name, "no_collide")) {
								Objects[cur].flags.set(Object::Object_Flags::Collides, !v);
							}
						});
					}
					_model->setFlagState(i, newState);
					break;
				}
			}

			if (!cmd->isEmpty()) {
				_fredView->mainUndoStack()->push(cmd);
			} else {
				delete cmd;
			}

			// Defer missionChanged to avoid re-entering FlagListWidget while it processes itemChanged.
			QMetaObject::invokeMethod(this, [this]() { _viewport->editor->missionChanged(); }, Qt::QueuedConnection);
		});

	// "Select Prop" menu: jump the editor to any prop in the mission.
	Editor* editor = viewport->editor;
	util::installSelectMenu(
		this,
		viewport,
		[]() {
			std::vector<util::SelectMenuEntry> entries;
			for (auto* ptr = GET_FIRST(&obj_used_list); ptr != END_OF_LIST(&obj_used_list); ptr = GET_NEXT(ptr)) {
				if (ptr->type == OBJ_PROP && Props[ptr->instance].has_value()) {
					entries.push_back({QString::fromUtf8(Props[ptr->instance]->prop_name), OBJ_INDEX(ptr)});
				}
			}
			return entries;
		},
		[this, editor]() { return _model->hasMultipleSelection() ? -1 : editor->currentObject; },
		[editor](int objnum) {
			editor->unmark_all();
			editor->selectObject(objnum);
		},
		tr("&Select Prop"));

	resize(QDialog::sizeHint());
}

PropEditorDialog::~PropEditorDialog() = default;

void PropEditorDialog::changeEvent(QEvent* e)
{
	if (e->type() == QEvent::ActivationChange && isActiveWindow())
		_fredView->undoGroup()->setActiveStack(_fredView->mainUndoStack());
	QDialog::changeEvent(e);
}

void PropEditorDialog::initializeUi() {
	util::SignalBlockers blockers(this);

	const auto& labels = _model->getFlagLabels();
	QVector<std::pair<QString, int>> toWidget;
	toWidget.reserve(static_cast<int>(labels.size()));

	for (size_t i = 0; i < labels.size(); ++i) {
		toWidget.append({QString::fromStdString(labels[i].first), _model->getFlagState()[i]});
	}
	ui->propFlagsListWidget->setFlags(toWidget);

	const auto& descs = _model->getPropFlagDescriptions();
	QVector<std::pair<QString, QString>> qtDescs;
	qtDescs.reserve(static_cast<int>(descs.size()));
	for (const auto& d : descs)
		qtDescs.append({QString::fromUtf8(d.first.c_str()), QString::fromUtf8(d.second.c_str())});
	ui->propFlagsListWidget->setFlagDescriptions(qtDescs);

	ui->propFlagsListWidget->setFilterVisible(true);
	ui->propFlagsListWidget->setToolbarVisible(true);

	ui->layerCombo->clear();
	for (const auto& name : _viewport->getLayerNames()) {
		ui->layerCombo->addItem(QString::fromStdString(name), QString::fromStdString(name));
	}

	// prop class list (Prop_info is sorted by category, so this groups naturally); skip classes
	// flagged no_fred, matching how props are offered when adding them
	ui->propClassCombo->clear();
	for (int i = 0; i < prop_info_size(); ++i) {
		if (Prop_info[i].flags[Prop::Info_Flags::No_fred])
			continue;
		ui->propClassCombo->addItem(QString::fromStdString(Prop_info[i].name), QVariant(i));
	}

	const auto enable = _model->hasValidSelection();
	const auto has_props = _model->hasAnyPropsInMission();
	ui->propNameLineEdit->setEnabled(enable && !_model->hasMultipleSelection());
	ui->propFlagsListWidget->setEnabled(enable);
	ui->nextButton->setEnabled(has_props);
	ui->prevButton->setEnabled(has_props);
	ui->layerCombo->setEnabled(enable);
	ui->transformLockCheck->setEnabled(enable);
	ui->groupsButton->setEnabled(enable);
	ui->propClassCombo->setEnabled(enable);
	// texture replacement operates on a single prop
	ui->textureReplacementButton->setEnabled(_model->getSelectedPropObject() >= 0);
}

void PropEditorDialog::updateUi() {
	util::SignalBlockers blockers(this);

	ui->propNameLineEdit->setText(QString::fromStdString(_model->getPropName()));
	ui->layerCombo->setCurrentIndex(ui->layerCombo->findData(QString::fromStdString(_model->getLayer())));
	{
		const auto lockState = transformLockState(transformLockObjects());
		ui->transformLockCheck->setTristate(lockState == Qt::PartiallyChecked);
		ui->transformLockCheck->setCheckState(lockState);
	}
	ui->groupsButton->setText(tr("Groups: %1").arg(selectionGroupsText(transformLockObjects())));
	ui->propClassCombo->setCurrentIndex(ui->propClassCombo->findData(_model->getPropClass()));
}

void PropEditorDialog::on_propNameLineEdit_editingFinished() {
	const SCP_string newName = ui->propNameLineEdit->text().toUtf8().constData();
	const SCP_string oldName = _model->getPropName();
	if (newName == oldName) return;
	if (!_model->setPropName(newName)) {
		updateUi();
		return;
	}
	const auto& selected = _model->getSelectedPropObjects();
	if (selected.empty()) return;
	_fredView->mainUndoStack()->push(
		new RenameObjectCommand(selected.front(), oldName, newName, _viewport->editor, true));

	// Edit committed; clear the field's text-undo history so Ctrl+Z hits the mission stack.
	util::SignalBlockers b(this);
	ui->propNameLineEdit->setText(QString::fromStdString(_model->getPropName()));
}

void PropEditorDialog::on_nextButton_clicked() {
	_model->selectNextProp();
}

void PropEditorDialog::on_prevButton_clicked() {
	_model->selectPreviousProp();
}

void PropEditorDialog::on_layerCombo_currentIndexChanged(int index) {
	if (index < 0) return;
	const SCP_string newLayer = ui->layerCombo->itemData(index).toString().toStdString();

	SCP_vector<ObjectLayerChange> changes;
	for (int obj_idx : _model->getSelectedPropObjects()) {
		if (!query_valid_object(obj_idx) || Objects[obj_idx].type != OBJ_PROP) continue;
		SCP_string oldLayer = _viewport->getObjectLayerName(obj_idx);
		if (oldLayer == newLayer) continue;
		changes.push_back({ Objects[obj_idx].signature, std::move(oldLayer), newLayer });
	}
	if (changes.empty()) return;
	// MoveLayerCommand::redo() applies the change, so do not also call _model->setLayer().
	_fredView->mainUndoStack()->push(
		new MoveLayerCommand(std::move(changes), _viewport, _viewport->editor));
}

void PropEditorDialog::on_propClassCombo_currentIndexChanged(int index) {
	if (index < 0)
		return;
	const int newClass = ui->propClassCombo->itemData(index).toInt();

	// Capture the pre-change state of every selected prop that actually changes class.
	// change_prop_type() (run inside the command) reseeds textures and toggles collision,
	// so we snapshot both here to restore them faithfully on undo.
	SCP_vector<PropClassChange> changes;
	for (int obj_idx : _model->getSelectedPropObjects()) {
		if (!query_valid_object(obj_idx) || Objects[obj_idx].type != OBJ_PROP)
			continue;
		prop* prp = prop_id_lookup(Objects[obj_idx].instance);
		if (prp == nullptr || prp->prop_info_index == newClass)
			continue;
		PropClassChange c;
		c.signature      = Objects[obj_idx].signature;
		c.beforeClass    = prp->prop_info_index;
		c.afterClass     = newClass;
		c.beforeTextures = prp->replacement_textures;
		c.beforeCollides = Objects[obj_idx].flags[Object::Object_Flags::Collides];
		changes.push_back(std::move(c));
	}
	if (changes.empty())
		return;

	// The command's redo() applies the change, so don't also call _model->setPropClass().
	_fredView->mainUndoStack()->push(new ChangePropClassCommand(std::move(changes), _viewport->editor));
}

void PropEditorDialog::on_textureReplacementButton_clicked() {
	const int propObjNum = _model->getSelectedPropObject();
	if (propObjNum < 0)
		return;
	prop* prp = prop_id_lookup(Objects[propObjNum].instance);
	if (prp == nullptr)
		return;

	// Capture the prop's texture state before the (modal, working-copy) dialog applies.
	// Identity is the signature so the undo command survives later delete/undelete cycles.
	const int sig = Objects[propObjNum].signature;
	auto before = std::make_shared<SCP_vector<texture_replace>>(prp->replacement_textures);

	auto dialog = new dialogs::PropTextureReplacementDialog(this, _viewport, propObjNum);
	dialog->setAttribute(Qt::WA_DeleteOnClose);

	// The dialog's apply() has already written mission data by the time accepted() fires;
	// diff against the captured before-state and push one undo step if anything changed.
	connect(dialog, &QDialog::accepted, this, [this, sig, before]() {
		const int n = obj_get_by_signature(sig);
		if (n < 0 || Objects[n].type != OBJ_PROP)
			return;
		prop* p = prop_id_lookup(Objects[n].instance);
		if (p == nullptr)
			return;

		SCP_vector<texture_replace> after = p->replacement_textures;
		auto sameEntries = [](const SCP_vector<texture_replace>& a, const SCP_vector<texture_replace>& b) {
			if (a.size() != b.size()) return false;
			for (size_t i = 0; i < a.size(); i++) {
				if (stricmp(a[i].old_texture, b[i].old_texture) != 0 ||
				    stricmp(a[i].new_texture, b[i].new_texture) != 0)
					return false;
			}
			return true;
		};
		if (sameEntries(*before, after))
			return;

		auto* cmd = new fso::fred::PropTextureReplacementCommand(sig, *before, _viewport->editor);
		cmd->setAfter(std::move(after));
		_fredView->mainUndoStack()->push(cmd);
	});

	dialog->show();
}

void PropEditorDialog::updateCues() {
	util::SignalBlockers blockers(this);

	// cues are edited for a single selected prop only
	const bool single = _model->getSelectedPropObject() >= 0;

	ui->spawnCueTree->initializeEditor(_viewport->editor, this, _viewport);
	ui->despawnCueTree->initializeEditor(_viewport->editor, this, _viewport);

	if (single) {
		ui->spawnCueTree->load_tree(_model->getSpawnFormula(), "true");
		ui->spawnCueTree->expandAll();
		ui->despawnCueTree->load_tree(_model->getDespawnFormula(), "false");
		ui->despawnCueTree->expandAll();
		ui->spawnDelaySpinBox->setValue(_model->getSpawnDelay());
		ui->despawnDelaySpinBox->setValue(_model->getDespawnDelay());
	} else {
		ui->spawnCueTree->clear_tree("");
		ui->despawnCueTree->clear_tree("");
		ui->spawnDelaySpinBox->setValue(0);
		ui->despawnDelaySpinBox->setValue(0);
	}

	ui->spawnCueTree->setEnabled(single);
	ui->despawnCueTree->setEnabled(single);
	ui->spawnDelaySpinBox->setEnabled(single);
	ui->despawnDelaySpinBox->setEnabled(single);
}

void PropEditorDialog::on_spawnCueTree_modified() {
	const int obj_idx = _model->getSelectedPropObject();
	if (obj_idx < 0)
		return; // cues are edited for a single selected prop only

	const int sig = Objects[obj_idx].signature;
	prop* prp = prop_id_lookup(Objects[obj_idx].instance);

	auto* cmd = new SexpCueEditCommand(_viewport->editor, tr("Edit Prop Spawn Cue"), true);
	cmd->addOwner(prp ? prp->spawn_cue : -1,
		[sig]() -> int {
			const int n = obj_get_by_signature(sig);
			if (n < 0) return -1;
			prop* p = prop_id_lookup(Objects[n].instance);
			return p ? p->spawn_cue : -1;
		},
		[sig](int formula) {
			const int n = obj_get_by_signature(sig);
			if (n < 0) return;
			prop* p = prop_id_lookup(Objects[n].instance);
			if (p) p->spawn_cue = formula;
		});

	const int newFormula = ui->spawnCueTree->_model.save_tree();
	_model->setSpawnTreeDirty(newFormula);

	if (cmd->isEmpty()) {
		delete cmd;
		return;
	}
	cmd->captureAfter(newFormula);
	_fredView->mainUndoStack()->push(cmd);
}

void PropEditorDialog::on_spawnCueTree_helpChanged(const QString& help) {
	ui->cueHelpText->setPlainText(help);
}

void PropEditorDialog::on_spawnCueTree_miniHelpChanged(const QString& help) {
	ui->cueHelpTitle->setText(help);
}

void PropEditorDialog::on_spawnDelaySpinBox_valueChanged(int value) {
	const int obj_idx = _model->getSelectedPropObject();
	if (obj_idx < 0)
		return;
	prop* prp = prop_id_lookup(Objects[obj_idx].instance);
	if (prp == nullptr)
		return;

	const int before = prp->spawn_delay;
	_model->setSpawnDelay(value);
	const int after = _model->getSpawnDelay(); // read back: the model clamps negatives
	if (before == after)
		return;

	const int sig = Objects[obj_idx].signature;
	auto* cmd = new FieldEditCommand<int>(FieldId::Prop_SpawnDelay, _viewport->editor, tr("Change Prop Spawn Delay"), true);
	cmd->setTargetKey(std::to_string(sig));
	cmd->addEntry(before, after, [sig](const int& v) {
		const int n = obj_get_by_signature(sig);
		if (n < 0) return;
		prop* p = prop_id_lookup(Objects[n].instance);
		if (p) p->spawn_delay = v;
	});
	_fredView->mainUndoStack()->push(cmd);
}

void PropEditorDialog::on_despawnCueTree_modified() {
	const int obj_idx = _model->getSelectedPropObject();
	if (obj_idx < 0)
		return; // cues are edited for a single selected prop only

	const int sig = Objects[obj_idx].signature;
	prop* prp = prop_id_lookup(Objects[obj_idx].instance);

	auto* cmd = new SexpCueEditCommand(_viewport->editor, tr("Edit Prop Despawn Cue"), true);
	cmd->addOwner(prp ? prp->despawn_cue : -1,
		[sig]() -> int {
			const int n = obj_get_by_signature(sig);
			if (n < 0) return -1;
			prop* p = prop_id_lookup(Objects[n].instance);
			return p ? p->despawn_cue : -1;
		},
		[sig](int formula) {
			const int n = obj_get_by_signature(sig);
			if (n < 0) return;
			prop* p = prop_id_lookup(Objects[n].instance);
			if (p) p->despawn_cue = formula;
		});

	const int newFormula = ui->despawnCueTree->_model.save_tree();
	_model->setDespawnTreeDirty(newFormula);

	if (cmd->isEmpty()) {
		delete cmd;
		return;
	}
	cmd->captureAfter(newFormula);
	_fredView->mainUndoStack()->push(cmd);
}

void PropEditorDialog::on_despawnCueTree_helpChanged(const QString& help) {
	ui->cueHelpText->setPlainText(help);
}

void PropEditorDialog::on_despawnCueTree_miniHelpChanged(const QString& help) {
	ui->cueHelpTitle->setText(help);
}

void PropEditorDialog::on_despawnDelaySpinBox_valueChanged(int value) {
	const int obj_idx = _model->getSelectedPropObject();
	if (obj_idx < 0)
		return;
	prop* prp = prop_id_lookup(Objects[obj_idx].instance);
	if (prp == nullptr)
		return;

	const int before = prp->despawn_delay;
	_model->setDespawnDelay(value);
	const int after = _model->getDespawnDelay(); // read back: the model clamps negatives
	if (before == after)
		return;

	const int sig = Objects[obj_idx].signature;
	auto* cmd = new FieldEditCommand<int>(FieldId::Prop_DespawnDelay, _viewport->editor, tr("Change Prop Despawn Delay"), true);
	cmd->setTargetKey(std::to_string(sig));
	cmd->addEntry(before, after, [sig](const int& v) {
		const int n = obj_get_by_signature(sig);
		if (n < 0) return;
		prop* p = prop_id_lookup(Objects[n].instance);
		if (p) p->despawn_delay = v;
	});
	_fredView->mainUndoStack()->push(cmd);
}

SCP_vector<int> PropEditorDialog::transformLockObjects() const
{
	SCP_vector<int> objs;
	for (int obj_idx : _model->getSelectedPropObjects()) {
		if (query_valid_object(obj_idx) && Objects[obj_idx].type == OBJ_PROP)
			objs.push_back(obj_idx);
	}
	return objs;
}

void PropEditorDialog::on_groupsButton_clicked()
{
	_fredView->editSelectionGroups(transformLockObjects(), this);
	updateUi();
}

void PropEditorDialog::on_transformLockCheck_clicked()
{
	const auto objs = transformLockObjects();
	// a partly locked selection locks the rest
	pushTransformLock(objs, transformLockState(objs) != Qt::Checked, _viewport->editor, _fredView->mainUndoStack());
	updateUi();
}

} // namespace fso::fred::dialogs
