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
	//qDebug() << "--- create index" << (void*)m_group->child(row);
	return createIndex(row, column, m_group->child(row));
}

QModelIndex GroupModel::parent(const QModelIndex & index) const
{
	return QModelIndex();
}

int GroupModel::rowCount(const QModelIndex & parent) const
{
	if (!m_group) return 0;
	return m_group->getNumChildren();
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
		return QVariant::fromValue((void*)m_group->child(index.row()));
	}
	return QVariant();
}

bool GroupModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	return false;
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
	emit endResetModel();
}

int GroupModel::itemType(int index) const
{
	return 0;
}

//void GroupModel::addColumn(GetterFunc getter)
//{
//	int end = m_num_columns;
//	beginInsertColumns(QModelIndex(), end, end);
//	m_getters.push_back(getter);
//	m_num_columns++;
//	endInsertColumns();
//}
