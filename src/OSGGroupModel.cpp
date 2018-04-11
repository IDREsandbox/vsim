#include "OSGGroupModel.h"

#include <osg/Group>
#include <vector>
#include <QDebug>
#include "Util.h"

OSGGroupModel::OSGGroupModel(QObject *parent)
	: QAbstractItemModel(parent),
	m_node(nullptr)
{
}

QModelIndex OSGGroupModel::index(int row, int column, const QModelIndex & parent) const
{
	if (!m_node) return QModelIndex();

	osg::Group *pgroup = nullptr;
	if (!parent.isValid()) {
		// root is parent
		pgroup = m_node->asGroup();
	}
	else {
		osg::Node *pnode = getNode(parent);
		pgroup = pnode->asGroup();
	}

	// parent is not a group?
	if (!pgroup) {
		return QModelIndex();
	}

	// sometimes it would crash trying to make out of range index
	if (row >= (int)pgroup->getNumChildren()) {
		return QModelIndex();
	}

	// get the nth child
	osg::Node *child = pgroup->getChild(row);

	return createIndex(row, column, child);
}

QModelIndex OSGGroupModel::parent(const QModelIndex & index) const
{
	// root index
	if (!index.isValid()) return QModelIndex();

	osg::Node *node = getNode(index);

	// no parents?
	int npar = node->getNumParents();
	if (npar == 0) return QModelIndex();
	osg::Node *parent = node->getParent(0);

	return indexOf(parent);
}

int OSGGroupModel::rowCount(const QModelIndex & parent) const
{
	if (!m_node) return 0;

	osg::Group *group;
	if (!parent.isValid()) {
		group = m_node->asGroup();
	}
	else {
		osg::Node *node = getNode(parent);
		group = node->asGroup();
	}
	if (!group) return 0;

	return group->getNumChildren();
}

int OSGGroupModel::columnCount(const QModelIndex & parent) const
{
	return 0;
}

Qt::ItemFlags OSGGroupModel::flags(const QModelIndex & index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant OSGGroupModel::data(const QModelIndex & index, int role) const
{
	if (!m_node) return QVariant();

	if (role == PointerRole) {
		// return the root pointer
		if (!index.isValid()) {
			return QVariant::fromValue((void*)(osg::Node*)m_node);
		}
		osg::Group *g = m_node->asGroup();
		if (g) {
			return QVariant::fromValue((void*)g->getChild(index.row()));
		}
	}
	return QVariant();
}

bool OSGGroupModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	return false;
}

void OSGGroupModel::setNode(osg::Node * group)
{
	beginResetModel();

	m_node = group;

	endResetModel();
}

osg::Node *OSGGroupModel::getNode(const QModelIndex & index) const
{
	if (!index.isValid()) return m_node;
	osg::Node *node = static_cast<osg::Node*>(index.internalPointer());
	return node;
}

QModelIndex OSGGroupModel::indexOf(osg::Node *node) const
{
	if (node == nullptr) {
		return QModelIndex();
	}

	// Is this the root?
	if (node == m_node) return QModelIndex();

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