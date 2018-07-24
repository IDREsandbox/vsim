#include "SwitchModel.h"

#include <QDebug>

SwitchListModel::SwitchListModel(QObject * parent)
	: QAbstractItemModel(parent)
{
}

void SwitchListModel::clear()
{
	beginResetModel();
	m_list2.clear();
	endResetModel();
}

void SwitchListModel::addSwitch(osg::Switch * sw)
{
	int index = m_list2.size();

	beginInsertRows(QModelIndex(), index, index);

	auto node = std::make_unique<Node>();
	node->m_parent = nullptr;
	node->m_section = SWITCHES;
	node->m_switch = sw;

	auto node_head = std::make_unique<Node>();
	node_head->m_parent = node.get();
	node_head->m_section = NODES;
	node->m_node_head = std::move(node_head);

	auto set_head = std::make_unique<Node>();
	set_head->m_parent = node.get();
	set_head->m_section = NODES;
	node->m_set_head = std::move(set_head);

	m_list2.push_back(std::move(node));

	endInsertRows();
}

void SwitchListModel::addMultiSwitch(osgSim::MultiSwitch * sw)
{
}

void SwitchListModel::removeSwitch(osg::Switch *sw)
{
	// FIXME: breaks if there are duplicates

	// find switch

	auto it2 = std::find_if(m_list2.begin(), m_list2.end(),
		[this, sw](const std::unique_ptr<Node> &item) -> bool {
		return item->m_switch == sw;
	});
	if (it2 == m_list2.end()) return;
	int index = std::distance(m_list2.begin(), it2);


	beginRemoveRows(QModelIndex(), index, index);
	m_list2.erase(it2);
	endRemoveRows();
}

bool SwitchListModel::isValid(const QModelIndex & index) const
{
	// TODO
	switch (sectionForIndex(index)) {

	case SWITCHES:
		if (!index.isValid()
			|| index.column() > 2
			|| index.row() >= m_list2.size()) {
			return false;
		}
		return true;
		break;
	case NODES:
		if (!index.isValid()) return false;


		return true;
		break;
	case SETS:
		return false;
		break;
	}

	return false;
}

osg::Switch *SwitchListModel::switchForIndex(const QModelIndex & index) const
{
	Node *node = static_cast<Node*>(index.internalPointer());
	if (node == nullptr) {
		// stuff

		return m_list2[index.row()].get()->m_switch;
	}
	return node->m_parent->m_switch;
}

SwitchListModel::Section SwitchListModel::sectionForIndex(const QModelIndex & index) const
{
	Node *node = static_cast<Node*>(index.internalPointer());
	if (node == nullptr) return SWITCHES;

	return node->m_section;
}

SwitchListModel::Section SwitchListModel::sectionFromParent(const QModelIndex & parent) const
{
	if (!parent.isValid()) {
		return SWITCHES;
	}
	return sectionForColumn(parent.column());
}

SwitchListModel::Section SwitchListModel::sectionForColumn(int column)
{
	if (column == 1) return NODES;
	else if (column == 2) return SETS;
	return NONE;
}

int SwitchListModel::columnForSection(Section section)
{
	if (section == NODES) {
		return 1;
	}
	else if (section == SETS) {
		return 2;
	}
	return 0;
}

QModelIndex SwitchListModel::index(int row, int column, const QModelIndex &parent) const
{
	// no parent -> switches
	// return (row, column, nullptr)
	if (!parent.isValid()) {
		return createIndex(row, column);
	}

	// switch 2, node 6
	// row 6, internal pointer is Node(switch 2, nodes)
	Section ps = sectionForColumn(parent.column());
	Node *pnode = m_list2[parent.row()].get();

	if (ps == NODES) {
		return createIndex(row, column, pnode->m_node_head.get());
	}
	else if (ps == SETS) {
		return createIndex(row, column, pnode->m_set_head.get());
	}

	return QModelIndex();
}

QModelIndex SwitchListModel::parent(const QModelIndex &index) const
{
	// cast internal to node

	ParentPtrNode *node = static_cast<ParentPtrNode*>(index.internalPointer());

	if (node == nullptr) return QModelIndex();

	SwitchNode *parent = node->m_parent;

	// get index of parent
	auto pit = std::find_if(m_list2.begin(), m_list2.end(),
		[parent](const std::unique_ptr<SwitchNode> &x) -> bool {
		return x.get() == parent;
	});
	if (pit == m_list2.end()) return QModelIndex();
	int row = std::distance(m_list2.begin(), pit);

	return createIndex(row, columnForSection(node->m_section));
}

int SwitchListModel::rowCount(const QModelIndex &parent) const
{
	auto s = sectionFromParent(parent);

	if (s == SWITCHES) {
		return m_list2.size();
	}
	else if (s == NODES) {
		int count = m_list2[parent.row()]->m_switch->getNumChildren();
		qDebug() << "nodes row count" << count;
		return count;
	}
	else if (s == SETS) {
		return 0;
	}

	return 0;
}

int SwitchListModel::columnCount(const QModelIndex &parent) const
{
	auto s = sectionFromParent(parent);

	if (s == SWITCHES) {
		return 3;
	}
	else if (s == NODES) {
		return 1;
	}
	else if (s == SETS) {
		return 1;
	}
	return 0;
}

Qt::ItemFlags SwitchListModel::flags(const QModelIndex &index) const
{
	Section sec = sectionForIndex(index);

	if (sec == SWITCHES) {
		if (index.column() == 0) {
			return Qt::ItemFlag::ItemIsSelectable
				| Qt::ItemFlag::ItemIsAutoTristate
				| Qt::ItemFlag::ItemIsUserCheckable
				| Qt::ItemFlag::ItemIsEnabled;
		}
	}
	else if (sec == NODES) {
		if (index.column() == 0) {
			return Qt::ItemFlag::ItemIsSelectable
				| Qt::ItemFlag::ItemIsUserCheckable
				| Qt::ItemFlag::ItemIsEnabled;
		}
	}

	return Qt::NoItemFlags;
}

QVariant SwitchListModel::data(const QModelIndex &index, int role) const
{
	//if (!isValid(index)) return QVariant();

	Section sec = sectionForIndex(index);

	if (sec == SWITCHES) {
		if (index.column() == 0) {
			osg::Switch *sw = switchForIndex(index);

			if (role == Qt::DisplayRole) {
				return QVariant(QString::fromStdString(sw->getName()));
			}
			else if (role == Qt::CheckStateRole) {
				// scan all children of switch, check on/off
				bool all_on = true;
				bool all_off = true;
				auto list = sw->getValueList();
				for (bool hit : list) {
					if (hit) {
						all_off = false;
					}
					else {
						all_on = false;
					}
				}

				if (sw->getNumChildren() == 0
					|| all_on) {
					return QVariant(Qt::CheckState::Checked);
				}
				else if (all_off) {
					return QVariant(Qt::CheckState::Unchecked);
				}
				else {
					return QVariant(Qt::CheckState::PartiallyChecked);
				}
			}
		}
	}
	else if (sec == NODES) {
		if (index.column() != 0) return QVariant();

		osg::Switch *sw = switchForIndex(index);

		if (role == Qt::ItemDataRole::DisplayRole) {
			return QString::fromStdString(sw->getChild(index.row())->getName());
		}
		else if (role == Qt::ItemDataRole::CheckStateRole) {
			Qt::CheckState state;
			if (sw->getValue(index.row())) {
				state = Qt::CheckState::Checked;
			}
			else {
				state = Qt::CheckState::Unchecked;
			}
			return state;
		}
	}

	return QVariant();
}

bool SwitchListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	//if (!isValid(index)) return false;

	Section sec = sectionForIndex(index);

	// switches
	if (sec == SWITCHES) {
		if (index.column() == 0 && role == Qt::CheckStateRole) {
			osg::Switch *sw = switchForIndex(index);

			Qt::CheckState state = (Qt::CheckState)value.toInt();

			if (state == Qt::CheckState::Checked) {
				sw->setAllChildrenOn();
			}
			else {
				sw->setAllChildrenOff();
			}
			// signal all children
			QModelIndex node_parent = this->index(index.row(), columnForSection(NODES));
			emit dataChanged(node_parent, node_parent);
			return true;
		}
		return false;
	}

	else if (sec == NODES) {
		//
		auto *sw = switchForIndex(index);
		sw->getValue(index.row());

		if (index.column() == 0 && role == Qt::CheckStateRole) {
			bool check = ((Qt::CheckState)value.toInt()) == Qt::CheckState::Checked;
			sw->setValue(index.row(), check);
			return true;
		}
	}
	else if (sec == SWITCHES) {
	}

	return false;
}
