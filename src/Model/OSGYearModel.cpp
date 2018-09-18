#include "OSGYearModel.h"

#include <QDebug>
#include <osg/ValueObject>

#include "Core/Util.h"
#include "Model.h"
#include "ModelGroup.h"
#include "OSGNodeWrapper.h"

OSGYearModel::OSGYearModel(QObject *parent)
	: OSGGroupModel(parent),
	m_base(nullptr)
{
}

void OSGYearModel::setBase(OSGNodeWrapper *base)
{
	beginResetModel();
	Util::reconnect(this, &m_base, base);
	setNodeInternal(m_base->getRoot());
	endResetModel();

	if (!m_base) return;

	// TODO: connect insertions
	// TODO: connect removals
	// TODO: move this to OSGGroupModel?
	connect(m_base, &OSGNodeWrapper::sAboutToReset, this,
		[this]() {
		beginResetModel();
	});
	connect(m_base, &OSGNodeWrapper::sReset, this,
		[this]() {
		setNodeInternal(m_base->getRoot());
		endResetModel();
	});
	connect(m_base, &OSGNodeWrapper::sNodeYearChanged, this,
		[this](osg::Node *node, int year, bool begin) {
		Column c = begin ? BEGIN : END;
		QModelIndex index = indexOf(node, c);
		if (!index.isValid()) return;
		emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole });
	});
	connect(m_base, &OSGNodeWrapper::sChanged, this,
		[this](osg::Node *node) {
		QModelIndex index = indexOf(node);
		if (!index.isValid()) return;
		emit dataChanged(index, index);
	});
	connect(m_base, &OSGNodeWrapper::sAboutToInsertChild, this,
		[this](osg::Group *group, size_t i) {
		beginInsertRows(indexOf(group), i, i);
	});
	connect(m_base, &OSGNodeWrapper::sInsertedChild, this,
		[this]() {
		endInsertRows();
	});
	connect(m_base, &OSGNodeWrapper::sAboutToRemoveChild, this,
		[this](osg::Group *group, size_t i) {
		QModelIndex idx = indexOf(group);
		beginRemoveRows(indexOf(group), i, i);
	});
	connect(m_base, &OSGNodeWrapper::sRemovedChild, this,
		[this]() {
		endRemoveRows();
	});
}

//void OSGYearModel::setModel(Model *model)
//{
//	setNode(model->node());
//	m_model = model;
//
//	// TODO
//	//connect(model, &Model::sNodeYearChanged, this,
//	//	[this](osg::Node *node, int year, bool begin) {
//	//	QModelIndex index = indexOf(node);
//	//	int column = begin ? 2 : 3;
//	//	QModelIndex outIndex = this->index(index.row(), column, index.parent());
//	//	emit dataChanged(outIndex, outIndex, { Qt::DisplayRole, Qt::EditRole });
//	//});
//}

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
	return Util::size(ColumnStrings);
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
	case NAME:
		return QString::fromStdString(node->getName());
		break;
	case TYPE:
		return node->className();
		break;
	case BEGIN:
		ok = node->getUserValue("yearBegin", value);
		if (ok) return value;
		if (role == Qt::EditRole) return 0; // this makes the editor a spinbox
		return QVariant();
		break;
	case END:
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
	bool editable = false;
	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	if (col == NAME) {
		editable = true;
	}
	if (col == BEGIN || col == END) {
		// if the name is T: begin end, then times are fixed
		// TODO: other stuck time conditions
		int begin, end;
		if (!Util::nodeTimeInName(getNode(index)->getName(), &begin, &end)) {
			editable = true;
		}
	}
	// check if actually editable
	flags.setFlag(Qt::ItemIsEditable, editable);

	return flags;
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
	case NAME:
		// TODO make command?
		node->setName(value.toString().toStdString());
		// If the name has time values, then apply those
		TimeInitVisitor::touch(m_base, node);
		break;
	case TYPE:
		return false;
		break;
	case BEGIN:
		m_base->setNodeYear(node, value.toInt(), true);
		break;
	case END:
		m_base->setNodeYear(node, value.toInt(), false);
		break;
	}
	return false;
}

QVariant OSGYearModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		
		if (section >= columnCount()) return QVariant();
		return ColumnStrings[section];
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



