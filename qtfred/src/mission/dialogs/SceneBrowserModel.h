#pragma once

#include "AbstractDialogModel.h"

#include <QSet>
#include <QString>
#include <QTimer>
#include <QVector>

namespace fso::fred::dialogs {

/**
 * @brief A single selectable object in the scene browser tree
 *
 * Leaf nodes (ships, props, jump nodes, individual waypoints) have objNum >= 0.
 * Wing headers have wingIndex >= 0 and objNum == -1.
 * Waypoint path headers have waypointListIndex >= 0 and objNum == -1.
 */
struct BrowserObject {
	QString displayName;
	int objNum = -1;
	int wingIndex = -1;
	int waypointListIndex = -1;
	bool isPlayerStart = false;
	QVector<BrowserObject> children;
};

struct BrowserCategory {
	QString name;
	QVector<BrowserObject> items;
};

struct BrowserLayer {
	QString name;
	bool visible = true;
	QVector<BrowserCategory> categories;
};

class SceneBrowserModel : public AbstractDialogModel {
	Q_OBJECT

public:
	SceneBrowserModel(QObject* parent, EditorViewport* viewport);

	bool apply() override { return true; }
	void reject() override {}

	const QVector<BrowserLayer>& getTree() const { return _tree; }
	static QSet<int> getMarkedSet();
	QVector<QString> getLayerNames() const;

	void toggleLayerVisibility(const QString& layerName);
	bool isLayerVisible(const QString& layerName) const;
	// The objects on a layer, in object-list order; empty for a hidden layer
	QVector<int> getLayerObjects(const QString& layerName) const;
	bool renameLayer(const QString& oldName, const QString& newName, SCP_string* error);
	static bool isDefaultLayer(const QString& name);
	void moveObjectToLayer(int objNum, const QString& layerName);
	void moveWingToLayer(int wingIndex, const QString& layerName);
	void moveWaypointPathToLayer(int waypointListIndex, const QString& layerName);

	void selectObjectFromBrowser(int objNum);
	void multiSelectFromBrowser(const QVector<int>& objNums);
	void selectWingFromBrowser(int wingIndex);
	static QVector<int> getWingMemberObjects(int wingIndex);
	// The ship or prop class of an object, for searching by class; empty for objects without one
	static QString getObjectClassName(int objNum);

	// Environment entities (volumetric nebula, asteroid field later). These
	// live outside the object tree; the panel renders a top-level "Environment"
	// node for them.
	static bool hasVolumetricNebula();
	static bool hasAsteroidField();
	EnvironmentObject currentEnvironment() const;
	void selectEnvironmentFromBrowser(EnvironmentObject env);

	// Viewport visibility of the "Environment" node (like a layer checkbox).
	bool environmentVisible() const;
	void setEnvironmentVisible(bool visible);

	void setNameFilter(const QString& filter);
	const QString& getNameFilter() const { return _nameFilter; }

	// IFF filtering
	void setFilterIff(int team, bool visible);
	bool getFilterIff(int team) const;
	static int iffCount();
	static QString getIffName(int team);

	// Bulk selection
	void selectAll();
	void clearSelection();
	void invertSelection();

	bool isUpdatingFromBrowser() const { return _updatingFromBrowser; }

signals:
	/**
	 * @brief Fired when the full tree structure needs to be rebuilt (objects added/removed/renamed).
	 * The panel should call rebuildTree() in response.
	 */
	void treeStructureChanged();

private:
	void buildTree();
	void scheduleSelectionSync();

	QVector<BrowserLayer> _tree;
	QString _nameFilter;
	QVector<bool> _filterIff;
	bool _updatingFromBrowser = false;
	bool _syncPending = false;
	QTimer* _rebuildTimer = nullptr;

	Q_SLOT void onCurrentObjectChanged(int newObj);
	Q_SLOT void onCurrentEnvironmentChanged();
	Q_SLOT void onObjectMarkingChanged(int obj, bool marked);
	Q_SLOT void onLayerVisibilityChanged();
	Q_SLOT void onLayerStructureChanged();
	Q_SLOT void onMissionLoaded();
	Q_SLOT void onMissionChanged();
	Q_SLOT void onRebuildTimer();
};

} // namespace fso::fred::dialogs
