#include "ModelGroupModel.h"

#include <QDebug>

#include "Core/Util.h"
#include "Model.h"
#include "ModelGroup.h"

ModelGroupModel::ModelGroupModel(QObject *parent)
	: QAbstractItemModel(parent),
	m_base(nullptr)
{
}

void ModelGroupModel::setBase(ModelGroup *base)
{
	// TODO: disconnect old base
	beginResetModel();

	Util::reconnect(this, &m_base, base->group());

	endResetModel();

	if (!m_base) return;

	// TODO: connect insertions
	// TODO: connect removals
	// TODO: move this to OSGGroupModel?
	connect(m_base, &GroupSignals::sAboutToReset, this,
		[this]() {
		beginResetModel();
	});
	connect(m_base, &GroupSignals::sReset, this,
		[this]() {
		endResetModel();
	});
	connect(m_base, &GroupSignals::sAboutToInsert, this,
		[this](size_t index, size_t count) {
		beginInsertRows(QModelIndex(), index, index + count - 1);
	});
	connect(m_base, &GroupSignals::sInserted, this,
		[this]() {
		endInsertRows();
	});
	connect(m_base, &GroupSignals::sAboutToRemove, this,
		[this](size_t index, size_t count) {
		beginRemoveRows(QModelIndex(), index, index + count - 1);
	});
	connect(m_base, &GroupSignals::sRemoved, this,
		[this]() {
		endRemoveRows();
	});
}

QModelIndex ModelGroupModel::index(int row, int column, const QModelIndex & parent) const
{
	if (parent != QModelIndex()) return QModelIndex();
	return createIndex(row, column);
}

QModelIndex ModelGroupModel::parent(const QModelIndex & index) const
{
	return QModelIndex();
}

int ModelGroupModel::rowCount(const QModelIndex &parent) const
{
	if (!m_base) return 0;
	if (parent != QModelIndex()) {
		return 0;
	}

	return m_base->size();
}

int ModelGroupModel::columnCount(const QModelIndex &parent) const
{
	return std::size(ColumnStrings);
}

QVariant ModelGroupModel::data(const QModelIndex &index, int role) const
{
	if (!m_base) return QVariant();
	if (!index.isValid())
		return QVariant();

	Model *model = m_base->child(index.row());
	if (!model) return QVariant();

	int col = index.column();
	switch (col) {
	case NAME:
		if (role != Qt::DisplayRole) break;
		return QString::fromStdString(model->name());
		break;
	case PATH:
		if (role != Qt::DisplayRole) break;
		return QString::fromStdString(model->path());
		break;
	case EMBEDDED:
		if (role == Qt::CheckStateRole) {
			Qt::CheckState state = model->embedded() ? Qt::Checked : Qt::Unchecked;
			return state;
		}
		break;
	}
	return QVariant();
}

Qt::ItemFlags ModelGroupModel::flags(const QModelIndex &index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ModelGroupModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {

		if (section >= columnCount()) return QVariant();
		return ColumnStrings[section];
	}
	return QVariant();
}
