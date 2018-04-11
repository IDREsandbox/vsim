#ifndef GROUPMODELTEMPLATE_INL
#define GROUPMODELTEMPLATE_INL

#include "GroupModelTemplate.h"

template<class T>
inline TGroupModel<T>::TGroupModel(QObject * parent)
	: QAbstractItemModel(parent),
	m_group(nullptr)
{
}

template<class T>
inline void TGroupModel<T>::setGroup(TGroup<T>* group)
{
	if (m_group != nullptr) disconnect(this, 0, m_group, 0);

	beginResetModel();

	m_group = group;

	if (group != nullptr) {
		connect(group, &GroupSignals::sAboutToInsert, this,
			[this](size_t index, size_t count) {
			beginInsertRows(QModelIndex(), index, index + count - 1);
		});
		connect(group, &GroupSignals::sInserted, this,
			[this](size_t index, size_t count) {
			endInsertRows();
		});
		connect(group, &GroupSignals::sAboutToRemove, this,
			[this](size_t index, size_t count) {
			beginRemoveRows(QModelIndex(), index, index + count - 1);
		});
		connect(group, &GroupSignals::sRemoved, this,
			[this](size_t index, size_t count) {
			endRemoveRows();
		});
		connect(group, &GroupSignals::sAboutToReset, this,
			[this]() {
			beginResetModel();
		});
		connect(group, &GroupSignals::sReset, this,
			[this]() {
			endResetModel();
		});
		//connect(group, &Group::sSet,
		//	[this](int i) {
		//	if (columnCount() <= 0) return;
		//	emit dataChanged(index(i, 0), index(i, columnCount() - 1));
		//});
		//connect(group, &Group::sMove,
		//	[this](std::vector<std::pair<int, int> mapping) {
		//	qDebug() << "GroupModel - move signal not supported yet";
		//});
	}

	endResetModel();
}

template<class T>
inline T * TGroupModel<T>::get(const QModelIndex & index) const
{
	return m_group->child(index.row());
}

template<class T>
inline QModelIndex TGroupModel<T>::indexOf(T * node) const
{
	if (!m_group) return QModelIndex();
	int row = m_group->indexOf(node);
	if (row < 0) return QModelIndex();
	return QModelIndex(row, 0);
}

template<class T>
inline QModelIndex TGroupModel<T>::index(int row, int column, const QModelIndex &parent) const
{
	if (!m_group
		|| parent.isValid() // only null parent is ok
		|| row >= m_group->size()
		// || column >= 0 // TODO column?
		) {
		return QModelIndex();
	}
	return createIndex(row, column);
}

template<class T>
inline QModelIndex TGroupModel<T>::parent(const QModelIndex & index) const
{
	return QModelIndex();
}

template<class T>
inline int TGroupModel<T>::rowCount(const QModelIndex &parent) const
{
	if (!m_group) return 0;
	if (parent != QModelIndex()) return 0; // only invalid has children
	return m_group->size();
}

template<class T>
inline int TGroupModel<T>::columnCount(const QModelIndex & parent) const
{
	return 0;
}

template<class T>
inline QVariant TGroupModel<T>::data(const QModelIndex & index, int role) const
{
	if (!m_group) return QVariant();
	if (!index.isValid()) return QVariant();
	if (role == PointerRole) {
		return QVariant::fromValue((void*)get(index));
	}
	return QVariant();
}


//void GroupModel::addColumn(GetterFunc getter)
//{
//	int end = m_num_columns;
//	beginInsertColumns(QModelIndex(), end, end);
//	m_getters.push_back(getter);
//	m_num_columns++;
//	endInsertColumns();
//}

#endif