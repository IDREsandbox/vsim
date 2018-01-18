#include "ECategoryModel.h"
#include <QDebug>

ECategoryModel::ECategoryModel(QObject *parent)
	: GroupModel(parent)
{
}

int ECategoryModel::columnCount(const QModelIndex & parent) const
{
	if (!m_group) return 0;
	return 1;
}

QVariant ECategoryModel::data(const QModelIndex & index, int role) const
{
	if (!m_group) {
		//qDebug() << "group is null";
		return QVariant();
	}
	if (index.row() < 0 || index.row() >= (int)m_group->getNumChildren()) return QVariant();
	if (index.column() > 0) return QVariant();

	ECategory *cat = dynamic_cast<ECategory*>(m_group->child(index.row()));
	if (cat == nullptr) {
		//qDebug() << "non-category in CategoryModel";
		return QVariant();
	}

	if (role == Qt::ItemDataRole::DecorationRole) {
		return cat->getColor();
	}
	else if (role == Qt::ItemDataRole::DisplayRole) {
		//qDebug() << "display role" << index.row() << index.column() << QString::fromStdString(cat->getCategoryName());
		return QString::fromStdString(cat->getCategoryName());
	}

	return GroupModel::data(index, role);
}
