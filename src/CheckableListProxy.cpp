#include "CheckableListProxy.h"
#include <QDebug>
#include <QItemSelection>

CheckableListProxy::CheckableListProxy(QObject * parent)
	: QAbstractProxyModel(parent),
	m_checked_count(0),
	m_unchecked_count(0),
	m_check_all(Qt::Checked)
{
}

void CheckableListProxy::setSourceModel(QAbstractItemModel *newModel)
{
	QAbstractProxyModel::setSourceModel(newModel);
	emit beginResetModel();

	resetChecks();

	QAbstractItemModel *source = sourceModel();
	if (source != nullptr) disconnect(source, 0, this, 0);

	if (newModel) {

		//QAbstractProxyModel::setSourceModel(newModel);

		//// listen up!
		// insertion
		connect(newModel, &QAbstractItemModel::rowsAboutToBeInserted,
			[this](const QModelIndex &, int start, int end) {
			beginInsertRows(QModelIndex(), start + 1, end + 1);

			// fix up the map
			int count = end - start + 1;
			m_checked.insert(m_checked.begin() + start, true, count);
			m_checked_count += count;
			updateTristate();
		});
		connect(newModel, &QAbstractItemModel::rowsInserted, this,
			[this](const QModelIndex &, int start, int end) {
			endInsertRows();
		});

		// removal
		connect(newModel, &QAbstractItemModel::rowsAboutToBeRemoved,
			[this](const QModelIndex &, int start, int end) {
			beginRemoveRows(QModelIndex(), start + 1, end + 1);
			for (int i = start; i <= end; i++) {
				if (m_checked[i]) m_checked_count--;
				else m_unchecked_count--;
			}
			m_checked.erase(m_checked.begin() + start, m_checked.begin() + end + 1);
			updateTristate();
		});
		connect(newModel, &QAbstractItemModel::rowsRemoved, this,
			[this](const QModelIndex &, int start, int end) {
			endRemoveRows();
		});

		// reset
		connect (newModel, &QAbstractItemModel::modelAboutToBeReset,
			[this]() {
			beginResetModel();
		});
		connect (newModel, &QAbstractItemModel::modelReset, this,
			[this]() {
			resetChecks();
			endResetModel();
		});
	}

	emit endResetModel();
}
QModelIndex CheckableListProxy::mapFromSource(const QModelIndex & sourceIndex) const
{
	QAbstractItemModel *source = sourceModel();
	if (!source) return QModelIndex();

	return createIndex(sourceIndex.row() + 1, sourceIndex.column(), sourceIndex.internalPointer());
}

QModelIndex CheckableListProxy::mapToSource(const QModelIndex & proxyIndex) const
{
	QAbstractItemModel *source = sourceModel();
	if (!source) return QModelIndex();

	return source->index(proxyIndex.row() - 1, proxyIndex.column());
}

QModelIndex CheckableListProxy::index(int row, int column, const QModelIndex &parent) const
{
	QAbstractItemModel *source = sourceModel();
	if (!source) return QModelIndex();
	// attempt to get that internal pointer
	const QModelIndex src = mapToSource(createIndex(row, column));
	if (src.isValid()) {
		return createIndex(row, column, src.internalPointer());
	}
	else {
		return createIndex(row, column);
	}
}

QModelIndex CheckableListProxy::parent(const QModelIndex & index) const
{
	return QModelIndex();
}

bool CheckableListProxy::insertRows(int row, int count, const QModelIndex & parent)
{
	QAbstractItemModel *source = sourceModel();
	if (!source) return false;
	if (row == 0) return false;

	m_checked.insert(m_checked.begin() + row, true, count);
	m_checked_count += count;
	updateTristate();

	source->insertRows(row - 1, count);
	return false;
}

bool CheckableListProxy::removeRows(int row, int count, const QModelIndex & parent)
{
	QAbstractItemModel *source = sourceModel();
	if (!source) return false;
	int top = std::min(row + count, (int)m_checked.size());
	for (int i = row; i < top; i++) {
		if (m_checked[i]) m_checked_count--;
		if (m_checked[i]) m_unchecked_count--;
	}
	m_checked.erase(m_checked.begin() + row, m_checked.begin() + top);
	updateTristate();

	source->removeRows(row - 1, count);
	return false;
}

int CheckableListProxy::rowCount(const QModelIndex & parent) const
{
	QAbstractItemModel *source = sourceModel();
	if (!source) return 1;

	return sourceModel()->rowCount() + 1;
}

int CheckableListProxy::columnCount(const QModelIndex & parent) const
{
	return 1;
}

Qt::ItemFlags CheckableListProxy::flags(const QModelIndex & index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;

	//QAbstractItemModel *source = sourceModel();
	//if (!source) return 0;

	//if (index.row() == 0) {
	//	return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
	//}

	//Qt::ItemFlags flags = source->flags(source->index(index.row() - 1, index.column()));
	//if (index.column() == 0) {
	//	Qt::ItemFlags newflags = Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
	//	return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
	//	//return (flags | Qt::ItemIsUserCheckable) & ~Qt::ItemIsSelectable;
	//}
	//return 0;
}

QVariant CheckableListProxy::data(const QModelIndex & index, int role) const
{
	QAbstractItemModel *source = sourceModel();
	if (!source) return QVariant();

	if (index.row() == 0) {
		if (role == Qt::ItemDataRole::DisplayRole
			|| role == Qt::EditRole) {
			return QString("(Show All)");
		}
		if (role == Qt::ItemDataRole::CheckStateRole) {
			return m_check_all;
		}
		return QVariant();
	}

	if (role == Qt::ItemDataRole::CheckStateRole) {
		if (isChecked(index.row() - 1)) return Qt::Checked;
		else return Qt::Unchecked;
	}

	if (mapToSource(index) == QModelIndex()) {
		return source->data(source->index(0, 0), role);
	}
	return source->data(mapToSource(index), role);
}

bool CheckableListProxy::setData(const QModelIndex & index, const QVariant & value, int role)
{
	QAbstractItemModel *source = sourceModel();
	if (!source) return false;

	// (Check All)
	if (index.row() == 0) {
		if (role == Qt::ItemDataRole::CheckStateRole) {
			Qt::CheckState val = value.value<Qt::CheckState>();
			setCheckAll((val == Qt::Checked) ? true : false);
			return true;
		}
		else {
			return false;
		}
	}

	// [ ] [x]
	if (role == Qt::ItemDataRole::CheckStateRole) {
		Qt::CheckState val = value.value<Qt::CheckState>();
		bool setChecked = (val == Qt::Checked) ? true : false;

		if (isChecked(index.row() - 1) == setChecked) {
			// nothing changed
			return false;
		}
		m_checked[index.row() - 1] = setChecked;
		if (setChecked) {
			m_checked_count++;
			m_unchecked_count--;
		}
		else {
			m_checked_count--;
			m_unchecked_count++;
		}
		updateTristate();
		emit dataChanged(index, index, {Qt::ItemDataRole::CheckStateRole});
		return true;
	}

	// TODO: try to set data on original
	return false;
}

QModelIndex CheckableListProxy::buddy(const QModelIndex &index) const
{
	// for some reason we need this... the default uses mapFromSource which breaks things
	return CheckableListProxy::index(index.row(), index.column());
}

bool CheckableListProxy::isChecked(int row) const
{
	QAbstractItemModel *source = sourceModel();
	if (!source) return 0;

	if (row >= m_checked.size()) return false;
	return m_checked[row];
}

std::vector<bool> CheckableListProxy::getChecked() const
{
	return m_checked;
}

void CheckableListProxy::setCheckAll(bool value)
{
	for (int i = 0; i < m_checked.size(); i++) {
		m_checked[i] = value;
	}

	if (value) {
		m_checked_count = (int)m_checked.size();
		m_unchecked_count = 0;
	}
	else {
		m_checked_count = 0;
		m_unchecked_count = (int)m_checked.size();
	}
	updateTristate();
	// everything changed
	emit dataChanged(
		CheckableListProxy::index(0, 0),
		CheckableListProxy::index(rowCount() - 1, 0),
		{ Qt::CheckStateRole });
}

Qt::CheckState CheckableListProxy::getCheckAll() const
{
	return m_check_all;
}

void CheckableListProxy::resetChecks()
{
	if (!sourceModel()) {
		m_checked = {};
		m_checked_count = 0;
		m_unchecked_count = 0;
		m_check_all = Qt::Checked;
		return;
	}
	m_checked = std::vector<bool>(sourceModel()->rowCount(), true);
	m_checked_count = sourceModel()->rowCount();
	m_unchecked_count = 0;
	m_check_all = Qt::Checked;
}

void CheckableListProxy::updateTristate()
{
	Qt::CheckState new_state;

	if (m_unchecked_count == 0) new_state = Qt::Checked;
	else if (m_checked_count == 0) new_state = Qt::Unchecked;
	else new_state = Qt::PartiallyChecked;

	m_check_all = new_state;
	emit dataChanged(index(0, 0), index(0, 0), {Qt::CheckStateRole});
}
