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
			beginInsertRows(QModelIndex(), (int)index, (int)(index + count - 1));
		});
		connect(group, &GroupSignals::sInserted, this,
			[this](size_t index, size_t count) {
			endInsertRows();
		});
		connect(group, &GroupSignals::sAboutToRemove, this,
			[this](size_t index, size_t count) {
			beginRemoveRows(QModelIndex(), (int)index, (int)(index + count - 1));
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
inline TGroup<T>* TGroupModel<T>::group() const
{
	return m_group;
}

template<class T>
inline T * TGroupModel<T>::get(const QAbstractItemModel * model, int row)
{
	return static_cast<T*>(
		model->data(model->index(row, 0), PointerRole).value<void*>());
}
template<class T>
inline std::shared_ptr<T> TGroupModel<T>::getShared(TGroup<T> *base,
	const QAbstractItemModel *model, int row)
{
	int base_index = model->data(model->index(row, 0), BaseIndexRole).toInt();
	return base->childShared(base_index);
}
template<class T>
inline int TGroupModel<T>::baseIndex(const QAbstractItemModel *model, int row)
{
	return = model->data(model->index(row, 0), BaseIndexRole).toInt();
}

template<class T>
inline T * TGroupModel<T>::get(const QModelIndex & index) const
{
	if (!m_group) return nullptr;
	if (index.model() == this) {
		return m_group->child(index.row());
	}
	return get(index.model(), index.row());
}

template<class T>
inline std::shared_ptr<T> TGroupModel<T>::getShared(const QModelIndex & index) const
{
	if (!m_group) return nullptr;
	if (index.model() == this) {
		return m_group->childShared(index.row());
	}
	// foreign model, assuming its a proxy
	return getShared(m_group, index.model(), index.row());
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
	return (int)m_group->size();
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
	if (role == BaseIndexRole) {
		if (index.row() >= m_group->size()) return -1;
		return index.row();
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