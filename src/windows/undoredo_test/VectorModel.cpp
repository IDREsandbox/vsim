#include "VectorModel.hpp"


VectorModel::VectorModel(std::vector<Thing*> &vector, QUndoStack *stack)
	: m_data_ptr(&vector), m_stack(stack) {}

QVariant VectorModel::data(const QModelIndex & index, int role) const
{
	//if (role == Qt::text)
	//return QString::fromStdString(m_data_ptr->at(index.row())->filename);
	if (role == Qt::DisplayRole) {
		return QString::fromStdString(m_data_ptr->at(index.row())->filename);
	}
	return QVariant();
	//return QVariant();
}

Qt::ItemFlags VectorModel::flags(const QModelIndex & index) const
{
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant VectorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QString("header data?");
}

QModelIndex VectorModel::index(int row, int column, const QModelIndex & parent) const
{

	return createIndex(row, column, m_data_ptr->at(row));
}

QModelIndex VectorModel::parent(const QModelIndex & index) const
{
	return QModelIndex();
}

int VectorModel::rowCount(const QModelIndex & parent) const
{
	qDebug() << "row count?" << m_data_ptr->size();
	return m_data_ptr->size();
}

int VectorModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

void VectorModel::poke()
{
	emit dataChanged(index(0,0), index(0,0));
}
