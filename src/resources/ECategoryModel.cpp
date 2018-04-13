#include "ECategoryModel.h"
#include "resources/ECategory.h"
#include "resources/ECategoryGroup.h"
#include <QDebug>

ECategoryModel::ECategoryModel(QObject *parent)
	: TGroupModel<ECategory>(parent)
{
}

int ECategoryModel::columnCount(const QModelIndex & parent) const
{
	if (!m_group) return 0;
	return 1;
}

QVariant ECategoryModel::data(const QModelIndex & index, int role) const
{
	if (!m_group) return QVariant();
	if (index.row() < 0 || index.row() >= (int)m_group->size()) return QVariant();
	if (index.column() > 0) return QVariant();

	ECategory *cat = m_group->child(index.row());
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

	return TGroupModel<ECategory>::data(index, role);
}
