#include "OSGYearModel.h"

#include <QDebug>
#include <osg/ValueObject>

#include "Model.h"
#include "ModelGroup.h"

OSGYearModel::OSGYearModel(QObject *parent)
	: OSGGroupModel(parent),
	m_model(nullptr)
{
}

void OSGYearModel::setModel(Model *model)
{
	setNode(model->node());
	m_model = model;
	connect(model, &Model::sNodeYearChanged, this,
		[this](osg::Node *node, int year, bool begin) {
		QModelIndex index = indexOf(node);
		int column = begin ? 2 : 3;
		QModelIndex outIndex = this->index(index.row(), column, index.parent());
		emit dataChanged(outIndex, outIndex, { Qt::DisplayRole, Qt::EditRole });
	});
}

//{
//	m_model_group = dynamic_cast<ModelGroup*>(g);
//	if (!m_model_group) return;
//
//	connect(m_model_group, &ModelGroup::sNodeYearChanged, this,
//		[this](osg::Node *node, int year, bool begin) {
//		QModelIndex index = indexOf(node);
//		int column = begin ? 2 : 3;
//		QModelIndex outIndex = this->index(index.row(), column, index.parent());
//		emit dataChanged(outIndex, outIndex, { Qt::DisplayRole, Qt::EditRole });
//	});

//void OSGYearModel::setGroup(Group * group)
//{
//	// check type
//	ModelGroup *model_group = dynamic_cast<ModelGroup*>(group);
//	if (!model_group) {
//		m_model_group = nullptr;
//		GroupModel::setGroup(nullptr);
//		return;
//	}
//	GroupModel::setGroup(group);
//}

int OSGYearModel::columnCount(const QModelIndex &parent) const
{
	return 4;
}

QVariant OSGYearModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return OSGGroupModel::data(index, role);

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
		if (role == Qt::EditRole) return 0; // this makes the editor a spinbox
		return QVariant();
		break;
	case 3:
		ok = node->getUserValue("yearEnd", value);
		if (ok) return value;
		if (role == Qt::EditRole) return 0;
		return QVariant();
		break;
	}
	return QVariant();
}

Qt::ItemFlags OSGYearModel::flags(const QModelIndex &index) const
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

bool OSGYearModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (!index.isValid()) {
		return false;
	}

	int col = index.column();
	osg::Node *node = static_cast<osg::Node*>(index.internalPointer());
	std::string prop;
	switch (col) {
	case 0:
		// TODO make command?
		node->setName(value.toString().toStdString());
		// If the name has time values, then apply those
		TimeInitVisitor::touch(m_model, node);
		break;
	case 1:
		return false;
		break;
	case 2:
		m_model->setNodeYear(node, value.toInt(), true);
		break;
	case 3:
		m_model->setNodeYear(node, value.toInt(), false);
		break;
	}
	return false;
}

QVariant OSGYearModel::headerData(int section, Qt::Orientation orientation, int role) const
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

//void OSGYearModel::connectGroup(Group * g)
//{
//	m_model_group = dynamic_cast<ModelGroup*>(g);
//	if (!m_model_group) return;
//
//	connect(m_model_group, &ModelGroup::sNodeYearChanged, this,
//		[this](osg::Node *node, int year, bool begin) {
//		QModelIndex index = indexOf(node);
//		int column = begin ? 2 : 3;
//		QModelIndex outIndex = this->index(index.row(), column, index.parent());
//		emit dataChanged(outIndex, outIndex, { Qt::DisplayRole, Qt::EditRole });
//	});
//}



