#ifndef SWITCHLISTMODEL_H
#define SWITCHLISTMODEL_H

#include <QAbstractItemModel>
#include <osg/Switch>
#include <osgSim/MultiSwitch>

// Multiple osg::Switch-related QAbstractItemModels taped together

// SwitchListModel
// 0, display: name
// 0, checkbox: all checked
// 1: NodeListModel, parent of basic switches
//   0, display: name
//   0, checkbox: active
// 2: SwitchSetModel, parent of switch set
//   0, display: name
//   0, checkbox: active

class SwitchListModel : public QAbstractItemModel {
	Q_OBJECT;
public:
	SwitchListModel(QObject *parent = nullptr);

	enum Section {
		NONE,
		SWITCHES,
		NODES,
		SETS
	};
	enum SwitchType {
		BASIC,
		MULTISWITCH
	};

	void clear();
	void addSwitch(osg::Switch *sw);
	void addMultiSwitch(osgSim::MultiSwitch *msw);
	void addGeneric(osg::Switch *sw, osgSim::MultiSwitch *msw);
	void removeSwitch(osg::Switch *sw);
	void removeNode(osg::Node *node);

	// what section is this index in?
	Section sectionForIndex(const QModelIndex &index) const;

	// what section is an index in given this parent?
	Section sectionFromParent(const QModelIndex &parent) const;

	// column where each section is kept, only applies on the root table
	static Section sectionForColumn(int column);
	static int columnForSection(Section section);

	// Qt model interface
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

private:
	struct SwitchNode;

	// given an index in NODES or SETS get the parent switch node
	SwitchNode *switchNode(const QModelIndex &index) const;

private:
	// TODO: for multiple types, metadata, check all?


	// QModelIndex metainformation
	// used for internal pointers, so we that can distinguish indices
	struct ParentPtrNode {
		Section m_section = NONE;
		SwitchNode *m_parent = nullptr;
	};

	// root nodes of model
	// - has stubs for QModelIndex metainformation
	// - wraps Switch and MultiSwitch into one interface
	struct SwitchNode {
	public:
		osg::Group *group() const;
		QString name() const;
		int getNumChildren() const;
		QString childName(int index) const;

		bool isChecked(int index) const;
		void setChecked(int index, bool value);

		void setAll(bool value);

	public:
		SwitchType m_type;

		// root nodes only
		osg::Switch *m_switch = nullptr;
		osgSim::MultiSwitch *m_multi_switch = nullptr;
		std::unique_ptr<ParentPtrNode> m_node_head;
		std::unique_ptr<ParentPtrNode> m_set_head;
	};

private:
	//std::vector<osg::Switch*> m_list;

	std::vector<std::unique_ptr<SwitchNode>> m_nodes;
};


#endif
