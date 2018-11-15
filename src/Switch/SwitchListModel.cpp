#include "SwitchListModel.h"

#include <QDebug>
#include <QColor>

SwitchListModel::SwitchListModel(QObject * parent)
	: QAbstractItemModel(parent)
{
}

void SwitchListModel::clear()
{
	beginResetModel();
	m_nodes.clear();
	endResetModel();
}

void SwitchListModel::addSwitch(osg::Switch *sw)
{
	addGeneric(sw, nullptr);
}

void SwitchListModel::addMultiSwitch(osgSim::MultiSwitch * msw)
{
	addGeneric(nullptr, msw);
}

void SwitchListModel::addGeneric(osg::Switch * sw, osgSim::MultiSwitch * msw)
{
	// both null?
	if (!sw && !msw) {
		return;
	}

	int index = m_nodes.size();

	beginInsertRows(QModelIndex(), index, index);

	auto node = std::make_unique<SwitchNode>();
	if (sw) {
		node->m_switch = sw;
		node->m_type = BASIC;
	}
	else {
		node->m_multi_switch = msw;
		node->m_type = MULTISWITCH;
	}

	auto node_head = std::make_unique<ParentPtrNode>();
	node_head->m_parent = node.get();
	node_head->m_section = NODES;
	node->m_node_head = std::move(node_head);

	auto set_head = std::make_unique<ParentPtrNode>();
	set_head->m_parent = node.get();
	set_head->m_section = SETS;
	node->m_set_head = std::move(set_head);

	m_nodes.push_back(std::move(node));

	endInsertRows();
}

void SwitchListModel::removeSwitch(osg::Switch *sw)
{
	// FIXME: breaks if there are duplicates

	// find switch
	removeNode(sw);
}

void SwitchListModel::removeNode(osg::Node *node)
{
	// FIXME:
	// beginRemoveRows() requires valid node state
	// but oberserver erases too quickly

	auto it = std::find_if(m_nodes.begin(), m_nodes.end(),
		[node](const std::unique_ptr<SwitchNode> &item) -> bool {
		return item->group() == node;
	});
	if (it == m_nodes.end()) return;
	int index = std::distance(m_nodes.begin(), it);

	beginRemoveRows(QModelIndex(), index, index);
	m_nodes.erase(it);
	endRemoveRows();
}

SwitchListModel::SwitchNode *SwitchListModel::switchNode(const QModelIndex & index) const
{
	ParentPtrNode *node = static_cast<ParentPtrNode*>(index.internalPointer());
	if (node == nullptr) {
		if (index.row() >= m_nodes.size()) {
			qWarning() << "bad switch node" << index << m_nodes.size();
			return nullptr;
		}
		return m_nodes[index.row()].get();
	}
	return node->m_parent;
}

SwitchListModel::Section SwitchListModel::sectionForIndex(const QModelIndex & index) const
{
	ParentPtrNode *node = static_cast<ParentPtrNode*>(index.internalPointer());
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
		if (row >= m_nodes.size()) return QModelIndex();

		return createIndex(row, column);
	}

	// switch 2, node 6
	// row 6, internal pointer is Node(switch 2, nodes)
	Section ps = sectionForColumn(parent.column());
	SwitchNode *pnode = m_nodes[parent.row()].get();

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
	auto pit = std::find_if(m_nodes.begin(), m_nodes.end(),
		[parent](const std::unique_ptr<SwitchNode> &x) -> bool {
		return x.get() == parent;
	});
	if (pit == m_nodes.end()) return QModelIndex();
	int row = std::distance(m_nodes.begin(), pit);

	return createIndex(row, columnForSection(node->m_section));
}

int SwitchListModel::rowCount(const QModelIndex &parent) const
{
	auto s = sectionFromParent(parent);

	if (s == SWITCHES) {
		return m_nodes.size();
	}
	else if (s == NODES) {
		if (parent.row() >= m_nodes.size()) {
			return 0;
		}

		int count = m_nodes[parent.row()]->getNumChildren();
		return count;
	}
	else if (s == SETS) {
		if (parent.row() >= m_nodes.size()) {
			return 0;
		}

		SwitchNode *sn = m_nodes[parent.row()].get();

		if (sn->m_type != MULTISWITCH) return 0;

		int count = sn->m_multi_switch->getSwitchSetList().size();
		return count;
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
	bool multi = (switchNode(index)->m_type == MULTISWITCH);

	Qt::ItemFlags flags;

	if (sec == SWITCHES) {
		if (index.column() != 0) return flags;
		flags |= Qt::ItemFlag::ItemIsSelectable
			| Qt::ItemFlag::ItemIsEnabled
			| Qt::ItemFlag::ItemIsAutoTristate;

		// not checkable if multiswitch
		if (!multi) {
			flags |= Qt::ItemIsUserCheckable;
		}

		return flags;
	}
	else if (sec == NODES) {
		if (index.column() != 0) return flags;
		flags |= Qt::ItemFlag::ItemIsSelectable
			| Qt::ItemFlag::ItemIsEnabled;

		// not checkable if multiswitch
		if (!multi) {
			flags |= Qt::ItemIsUserCheckable;
		}

		return flags;
	}
	else if (sec == SETS) {
		if (index.column() != 0) return flags;

		return Qt::ItemFlag::ItemIsUserCheckable
			| Qt::ItemFlag::ItemIsEnabled;
	}

	return Qt::NoItemFlags;
}

QVariant SwitchListModel::data(const QModelIndex &index, int role) const
{
	//if (!isValid(index)) return QVariant();

	Section sec = sectionForIndex(index);

	SwitchNode *sn = switchNode(index);

	if (sec == SWITCHES) {
		if (index.column() == 0) {

			if (role == Qt::DisplayRole) {
				return sn->name();
			}
			else if (role == Qt::CheckStateRole) {
				if (sn->m_type == MULTISWITCH) return QVariant();
				osg::Switch *sw = sn->m_switch;

				// scan all children of switch, check if on/off
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

				if (sn->getNumChildren() == 0
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

		sn->isChecked(index.row());

		int row = index.row();

		if (role == Qt::ItemDataRole::DisplayRole) {
			return sn->childName(row);
		}
		else if (role == Qt::ItemDataRole::CheckStateRole) {
			return sn->isChecked(row) ? Qt::Checked : Qt::Unchecked;
		}
	}
	else if (sec == SETS) {
		if (index.column() != 0) return QVariant();

		osgSim::MultiSwitch *ms = sn->m_multi_switch;
		int row = index.row();
		bool checked = ms->getActiveSwitchSet() == row;

		if (role == Qt::ItemDataRole::DisplayRole) {
			return QString::fromStdString(ms->getValueName(row));
		}
		else if (role == Qt::ItemDataRole::CheckStateRole) {
			return checked ? Qt::Checked : Qt::Unchecked;
		}
		//else if (role == Qt::ItemDataRole::BackgroundRole) {
		//	return checked ? QColor(150, 150, 255) : QColor();
		//}

	}

	return QVariant();
}

bool SwitchListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	//if (!isValid(index)) return false;

	Section sec = sectionForIndex(index);

	SwitchNode *sn = switchNode(index);

	// switches
	if (sec == SWITCHES) {
		if (index.column() == 0
			&& role == Qt::CheckStateRole
			&& sn->m_type == BASIC) {

			bool check = ((Qt::CheckState)value.toInt()) == Qt::CheckState::Checked;

			sn->setAll(check);

			// signal all children
			QModelIndex node_parent = this->index(index.row(), columnForSection(NODES));
			QModelIndex set_parent = this->index(index.row(), columnForSection(SETS));
			emit dataChanged(node_parent, set_parent);
			return true;
		}
		return false;
	}

	else if (sec == NODES) {

		if (index.column() == 0 && role == Qt::CheckStateRole) {
			bool check = ((Qt::CheckState)value.toInt()) == Qt::CheckState::Checked;
			sn->setChecked(index.row(), check);
			emit dataChanged(index, index);

			// prod the parent's check state
			int switch_row = index.parent().row();
			QModelIndex switch_parent = this->index(switch_row, 0);
			emit dataChanged(switch_parent, switch_parent, {Qt::CheckStateRole});
			return true;
		}
	}
	else if (sec == SETS) {

		if (index.column() == 0
			&& role == Qt::CheckStateRole
			&& ((Qt::CheckState)value.toInt()) == Qt::CheckState::Checked)
		{
			// only allow check ons, no check offs

			auto *ms = sn->m_multi_switch;
			ms->setActiveSwitchSet(index.row());

			// poke the nodes
			QModelIndex node_parent = this->index(index.row(), columnForSection(NODES));
			emit dataChanged(node_parent, node_parent);

			return true;
		}
	}

	return false;
}

void SwitchListModel::preSave()
{
	for (auto &x : m_nodes) {
		if (x->m_multi_switch == nullptr) continue;
		saveCustomSwitchNames(x->m_multi_switch);
	}
}

void SwitchListModel::postLoad()
{
	for (auto &x : m_nodes) {
		if (x->m_multi_switch == nullptr) continue;
		loadCustomSwitchNames(x->m_multi_switch);
	}
}

void SwitchListModel::saveCustomSwitchNames(osgSim::MultiSwitch * ms)
{
	std::vector<std::string> names = getSwitchNames(ms);

	// get container
	auto *cont = ms->getOrCreateUserDataContainer();

	// remove any existing switch_names
	osg::Object *search = cont->getUserObject("switch_names");
	unsigned int existing = cont->getUserObjectIndex("switch_names");
	if (existing < cont->getNumUserObjects()) {
		cont->removeUserObject(existing);
	}

	// create our target
	osg::DefaultUserDataContainer *name_cont = new osg::DefaultUserDataContainer;
	name_cont->setName("switch_names");
	name_cont->setDescriptions(names);
	cont->addUserObject(name_cont);
}

void SwitchListModel::loadCustomSwitchNames(osgSim::MultiSwitch * ms)
{
	auto *cont = ms->getOrCreateUserDataContainer();
	osg::Object *search = cont->getUserObject("switch_names");

	auto *name_cont2 = dynamic_cast<osg::UserDataContainer*>(search);

	if (name_cont2) {
		std::vector<std::string> names = name_cont2->getDescriptions();
		setSwitchNames(ms, names);
	}
}

std::vector<std::string> SwitchListModel::getSwitchNames(osgSim::MultiSwitch * ms)
{
	std::vector<std::string> out;
	for (unsigned int i = 0; i < ms->getSwitchSetList().size(); i++) {
		out.push_back(ms->getValueName(i));
	}
	return out;
}

void SwitchListModel::setSwitchNames(osgSim::MultiSwitch * ms, const std::vector<std::string>& names)
{
	for (unsigned int i = 0;
		i < names.size() && i < ms->getSwitchSetList().size();
		i++) {
		ms->setValueName(i, names[i]);
	}
}

osg::Group * SwitchListModel::SwitchNode::group() const
{
	if (m_type == BASIC) {
		return m_switch;
	}
	else {
		return m_multi_switch;
	}
}

QString SwitchListModel::SwitchNode::name() const
{
	switch (m_type) {
	case BASIC:
		return QString::fromStdString(m_switch->getName());
		break;
	case MULTISWITCH:
		return QString::fromStdString(m_multi_switch->getName());
		break;
	}
	return QString();
}

int SwitchListModel::SwitchNode::getNumChildren() const
{
	return group()->getNumChildren();
}

QString SwitchListModel::SwitchNode::childName(int index) const
{
	return QString::fromStdString(group()->getChild(index)->getName());
}

bool SwitchListModel::SwitchNode::isChecked(int index) const
{
  	if (m_type == BASIC) {
		return m_switch->getValue(index);
	}
	else {
		return m_multi_switch->getValue(m_multi_switch->getActiveSwitchSet(), index);
	}
	return false;
}

void SwitchListModel::SwitchNode::setChecked(int index, bool value)
{
	if (m_type == BASIC) {
		return m_switch->setValue(index, value);
	}
	else {

	}
}

void SwitchListModel::SwitchNode::setAll(bool value)
{
	if (value) {
		if (m_switch) {
			m_switch->setAllChildrenOn();
		}
		if (m_multi_switch) {
			//m_multi_switch->setAllChildrenOn();
		}
	}
	else {
		if (m_switch) {
			m_switch->setAllChildrenOff();
		}
		if (m_multi_switch) {
			//m_multi_switch->setAllChildrenOff();
		}
	}
}
