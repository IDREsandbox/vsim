#ifndef SWITCHMODEL_H
#define SWITCHMODEL_H

#include <QAbstractItemModel>
#include <osg/Switch>
#include <osgSim/MultiSwitch>

class SwitchModel;
class SwitchSetModel;

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
	void addMultiSwitch(osgSim::MultiSwitch *sw);
	void removeSwitch(osg::Switch *sw);

	bool isValid(const QModelIndex &index) const;

	osg::Switch *switchForIndex(const QModelIndex &index) const;

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
	// TODO: for multiple types, metadata, check all?

	struct SwitchNode;

	// used for internal pointers, so we can distinguish indices
	struct ParentPtrNode {
		Section m_section = NONE;
		SwitchNode *m_parent = nullptr;
	};

	// root nodes
	struct SwitchNode {
	public:
		bool isChecked(int index) const;
		void setChecked(int index, bool value);

	public:
		SwitchType m_type;

		// root nodes only
		osg::Switch *m_switch = nullptr;
		osgSim::MultiSwitch *m_multi_switch = nullptr;
		std::unique_ptr<ParentPtrNode> m_node_head;
		std::unique_ptr<ParentPtrNode> m_set_head;
	};

	//struct SwitchListItem;
	//struct NodeListHead;
	//struct SetListHead;
	//struct NodeListHead {
	//	SwitchListItem *m_parent;
	//};
	//struct SetListHead {
	//	SwitchListItem *m_parent;
	//};

	//struct InternalId {
	//	InternalId(quint64 x);
	//	quint64 toInt() const;
	//	uint32_t m_parent_row; // 
	//	bool m_is_nodes;
	//};


private:
	//std::vector<osg::Switch*> m_list;

	std::vector<std::unique_ptr<SwitchNode>> m_list2;
};

//class SwitchModel : public QAbstractItemModel {
//	Q_OBJECT;
//public:
//};

//class SwitchSetModel : public QAbstractItemModel {
//	Q_OBJECT;
//public:
//};

#endif
