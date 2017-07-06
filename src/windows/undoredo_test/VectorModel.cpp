#include "VectorModel.hpp"


VectorModel::VectorModel(std::vector<Thing*> &vector, QUndoStack *stack)
	: m_data_ptr(&vector), m_stack(stack) {}

QVariant VectorModel::data(const QModelIndex & index, int role) const
{
	//return QString::fromStdString(m_data_ptr->at(index.row())->filename);
	if (role == Qt::DisplayRole) {
		QModelIndex p = index.parent();

		// OtherThing data
		if (p != QModelIndex()) {
			////qDebug() << "this is a child thing";
			std::string text = m_data_ptr->at(p.row())->children.at(index.row())->pajamas;
			return QString::fromStdString(text);
		}

		// Thing data
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
	if (parent != QModelIndex()) {
		Thing *thing = static_cast<Thing*>(parent.internalPointer());
		return createIndex(row, 0, thing->children.at(row));
	}

	return createIndex(row, 0, m_data_ptr->at(row));
}

QModelIndex VectorModel::parent(const QModelIndex & index) const
{
	if (!index.isValid()) {
		//qDebug() << "invalid index?";
		return QModelIndex();
	}
	if (index == QModelIndex()) {
		//qDebug() << "blank index";
		return QModelIndex();
	}
	Node *me = static_cast<Node*>(index.internalPointer());
	
	Other *meother = dynamic_cast<Other*>(me);
	if (meother) {
		//qDebug() << "meother";
		ptrdiff_t parentindex = std::find(m_data_ptr->begin(), m_data_ptr->end(), me->parent) - m_data_ptr->begin();
		if (parentindex >= m_data_ptr->size()) {
			//qDebug() << "parent does not exist";
			return QModelIndex();
		}

		return createIndex(parentindex, 0, me->parent);
	}
	Thing *mething = dynamic_cast<Thing*>(me);
	if (mething) {
		//qDebug() << "mething";
		return QModelIndex();
	}

	return QModelIndex();
}

int VectorModel::rowCount(const QModelIndex & parent) const
{
	if (!parent.isValid()) {
		//qDebug() << "rowcount invalid?" << parent.internalPointer();
		return m_data_ptr->size();
	}

	Node *node = static_cast<Node*>(parent.internalPointer());
	if (node->parent == nullptr) {
		Thing *thing = static_cast<Thing*>(parent.internalPointer());
		//qDebug() << "rowcount null parent" << QString::fromStdString(thing->filename);
		return thing->children.size();
	}
	// otherwise no children
	//qDebug() << "no children" << static_cast<Other*>(parent.internalPointer())->hairstyle;
	return 0;
}

int VectorModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

void VectorModel::poke()
{
	emit dataChanged(index(0,0), index(0,0));
}
