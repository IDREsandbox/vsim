#include "ModelTableModel.h"

#include <QDebug>
#include <osg/ValueObject>

ModelTableModel::ModelTableModel(ModelGroup *group, QObject *parent)
	: QAbstractItemModel(parent),
	m_group(group)
{
}

ModelTableModel::~ModelTableModel()
{
}

int ModelTableModel::columnCount(const QModelIndex &parent) const
{
	return 4;
}

QVariant ModelTableModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();

	// 0: Name, 1: ClassName
	osg::Node *node = getNode(index);

	int value;
	bool ok;

	int col = index.column();
	switch (col) {
	case 0:
		return QString::fromStdString(node->getName());
		break;
	case 1:
		return node->className();
		break;
	case 2:
		ok = node->getUserValue("yearBegin", value);
		if (ok) return value;
		else return QVariant();
		break;
	case 3:
		ok = node->getUserValue("yearEnd", value);
		if (ok) return value;
		else return QVariant();
		break;
	}
	return QVariant();
}

Qt::ItemFlags ModelTableModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) return 0;
	int col = index.column();
	if (col == 1) return Qt::ItemIsEnabled;

	if (col == 2 || col == 3) {
		// if the filename is T: begin end, then not editable
		int begin, end;
		if (ModelGroup::nodeTimeInName(getNode(index)->getName(), &begin, &end)) 
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	// check if actually editable
	
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
	//return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

bool ModelTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (!index.isValid()) {
		return false;
	}

	int col = index.column();
	osg::Node *node = static_cast<osg::Node*>(index.internalPointer());

	switch (col) {
	case 0:
		// TODO make command?
		node->setName(value.toString().toStdString());
		break;
	case 1:
		return false;
		break;
	case 2:
		if (!value.isValid() || value == 0) {
			removeUserValue(node, "yearBegin");
			m_group->sUserValueChanged(node, "yearBegin");
			return true;
		}
		node->setUserValue("yearBegin", value.toInt());
		m_group->sUserValueChanged(node, "yearBegin");
		return true;
		break;
	case 3:
		if (!value.isValid() || value == 0) {
			removeUserValue(node, "yearEnd");
			m_group->sUserValueChanged(node, "yearEnd");
			return true;
		}
		node->setUserValue("yearEnd", value.toInt());
		m_group->sUserValueChanged(node, "yearEnd");
		return true;
		break;
	}
	return false;
}

QVariant ModelTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		switch (section) {
		case 0:
			return "Name";
		case 1:
			return "Type";
		case 2:
			return "Begin Year";
		case 3:
			return "End Year";
		default:
			return QVariant();
		}
	}
	
	return QVariant();
}

QModelIndex ModelTableModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))	return QModelIndex();

	osg::Node *pnode = getNode(parent);
	osg::Group *pgroup = pnode->asGroup();
	
	// osg::Nodes have no child tables because the data of this node 
	// is stored in their parent table
	if (!pgroup) {
		return QModelIndex();
	}

	// get the nth child
	osg::Node *child = pgroup->getChild(row);

	if (column >= 4) return QModelIndex();

	//qDebug() << "CREATE INDEX" << row << column << child;
	return createIndex(row, column, child);
}

QModelIndex ModelTableModel::parent(const QModelIndex &index) const
{
	if (!index.isValid()) return QModelIndex();

	osg::Node *node = getNode(index);
	if (node == m_group) {
		qDebug() << "does this happen?";
		return QModelIndex();
	}

	osg::Group *parent = node->getParent(0);

	return indexOf(parent);
}

int ModelTableModel::rowCount(const QModelIndex &parent) const
{
	osg::Group *group;
	if (!parent.isValid()) {
		// do fun stuff
		group = m_group;
		//qDebug() << "root children count";
	}
	else {
		osg::Node *node = getNode(parent);
		group = node->asGroup();
	}
	if (!group) return 0;

	return group->getNumChildren();
}

QModelIndex ModelTableModel::indexOf(osg::Node *node) const
{
	if (node == nullptr) {
		qDebug() << "null indexof";
		return QModelIndex();
	}

	// Is this the root?
	ModelGroup *mg = dynamic_cast<ModelGroup*>(node);
	if (mg) {
		return QModelIndex();
	}

	size_t row = 0;
	node->getParent(0)->getChildIndex(node);

	return createIndex(row, 0, node);
}

osg::Node * ModelTableModel::getNode(QModelIndex index) const
{
	if (!index.isValid()) return m_group;
	osg::Node *node = static_cast<osg::Node*>(index.internalPointer());
	return node;
}

void ModelTableModel::setGroup(ModelGroup * group)
{
	beginResetModel();
	m_group = group;
	endResetModel();
}

void ModelTableModel::removeUserValue(osg::Node *node, const std::string & name)
{
	osg::UserDataContainer *cont = node->getUserDataContainer();
	cont->removeUserObject(cont->getUserObjectIndex(name));
}


