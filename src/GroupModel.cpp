#include "GroupModel.h"

#include "Group.h"
#include <vector>
#include <QDebug>

GroupModel::GroupModel(QObject *parent)
	: QAbstractItemModel(parent),
	m_group(nullptr)
{
}

QModelIndex GroupModel::index(int row, int column, const QModelIndex & parent) const
{
	if (!m_group) return QModelIndex();

	if (!m_hierarchal) {
		if (row >= m_group->getNumChildren()) return QModelIndex();
		return createIndex(row, column, m_group->child(row));
	}

	osg::Node *pnode = getNode(parent);
	osg::Group *pgroup = pnode->asGroup();
	// parent is not a group
	if (!pgroup) {
		return QModelIndex();
	}

	// get the nth child
	osg::Node *child = pgroup->getChild(row);

	//qDebug() << "CREATE INDEX" << row << column << child;
	return createIndex(row, column, child);
}

QModelIndex GroupModel::parent(const QModelIndex & index) const
{
	// root index
	if (!index.isValid()) return QModelIndex();

	if (!m_hierarchal) return QModelIndex();

	osg::Node *node = getNode(index);

	// no parents?
	int npar = node->getNumParents();
	if (npar < 0) return QModelIndex();
	osg::Node *parent = node->getParent(0);

	return indexOf(parent);
}

int GroupModel::rowCount(const QModelIndex & parent) const
{
	if (!m_group) return 0;
	if (!m_hierarchal) {
		if (!parent.isValid()) return m_group->getNumChildren();
		return 0;
	}

	osg::Group *group;
	if (!parent.isValid()) {
		group = m_group;
	}
	else {
		osg::Node *node = getNode(parent);
		group = node->asGroup();
	}
	if (!group) return 0;

	return group->getNumChildren();
}

int GroupModel::columnCount(const QModelIndex & parent) const
{
	return 0;
}

Qt::ItemFlags GroupModel::flags(const QModelIndex & index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant GroupModel::data(const QModelIndex & index, int role) const
{
	if (!m_group) return QVariant();

	if (role == PointerRole) {
		// return the root pointer
		if (!index.isValid()) {
			return QVariant::fromValue((void*)(osg::Node*)m_group);
		}
		return QVariant::fromValue((void*)m_group->child(index.row()));
	}
	return QVariant();
}

bool GroupModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	return false;
}

void GroupModel::connectGroup(Group * g)
{
}

void GroupModel::setGroup(Group * group)
{
	if (m_group != nullptr) disconnect(this, 0, m_group, 0);

	emit beginResetModel();

	m_group = group;

	if (group != nullptr) {
		connect(group, &Group::sNew,
			[this](int index) {
			beginInsertRows(QModelIndex(), index, index);
			endInsertRows();
		});
		connect(group, &Group::sDelete,
			[this](int index) {
			beginRemoveRows(QModelIndex(), index, index);
			endRemoveRows();
		});
		connect(group, &Group::sSet,
			[this](int i) {
			if (columnCount() <= 0) return;
			emit dataChanged(index(i, 0), index(i, columnCount() - 1));
		});
		//connect(group, &Group::sMove,
		//	[this](std::vector<std::pair<int, int> mapping) {
		//	qDebug() << "GroupModel - move signal not supported yet";
		//});
	}

	connectGroup(group);

	emit endResetModel();
}

void GroupModel::setHierarchal(bool hierarchal)
{
	m_hierarchal = true;
}

osg::Node *GroupModel::getNode(const QModelIndex & index) const
{
	if (!index.isValid()) return m_group;
	osg::Node *node = static_cast<osg::Node*>(index.internalPointer());
	return node;
}

QModelIndex GroupModel::indexOf(osg::Node *node) const
{
	if (node == nullptr) {
		return QModelIndex();
	}

	// Is this the root?
	if (node == m_group) return QModelIndex();

	// no parents?
	size_t npar = node->getNumParents();
	if (npar == 0) return QModelIndex();
	osg::Node *parent = node->getParent(0);

	// cast parent to group
	osg::Group *g = dynamic_cast<osg::Group*>(parent);
	if (!g) return QModelIndex(); // parent is not a group?

	// node not a child
	size_t row = g->getChildIndex(node);
	if (row >= g->getNumChildren()) return QModelIndex(); // child not found

	return createIndex(row, 0, node);
}

//void GroupModel::addColumn(GetterFunc getter)
//{
//	int end = m_num_columns;
//	beginInsertColumns(QModelIndex(), end, end);
//	m_getters.push_back(getter);
//	m_num_columns++;
//	endInsertColumns();
//}
