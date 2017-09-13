#include "resources/ERScrollBox.h"

ERScrollBox::ERScrollBox(QWidget * parent)
	: HorizontalScrollBox(parent)
{
	
	// initialize menus
	m_slide_menu = new QMenu(tr("ER context menu"), this);
	m_action_new = new QAction("New Embedded Resource", m_slide_menu);
	m_action_delete = new QAction("Delete Embedded Resource", m_slide_menu);
	m_action_edit = new QAction("Edit Embedded Resource", m_slide_menu);
	m_action_open = new QAction("Open Embedded Resource", m_slide_menu);

	m_slide_menu->addAction(m_action_new);
	m_slide_menu->addAction(m_action_delete);
	m_slide_menu->addAction(m_action_edit);
	m_slide_menu->addAction(m_action_open);

	// forward signals
	connect(m_action_new, &QAction::triggered, this, &ERScrollBox::sNew);
	connect(m_action_delete, &QAction::triggered, this, &ERScrollBox::sDelete);
	connect(m_action_edit, &QAction::triggered, this, &ERScrollBox::sEdit);
	connect(m_action_open, &QAction::triggered, this, &ERScrollBox::sOpen);

	setMenu(m_slide_menu);
	setItemMenu(m_slide_menu);
}

ERScrollBox::~ERScrollBox() {
	
}

void ERScrollBox::filter(int type)
{
	

}

ScrollBoxItem * ERScrollBox::createItem(osg::Node * node)
{
	EResource *er = dynamic_cast<EResource*>(node);
	if (er == nullptr) {
		qWarning() << "ER scroll box insert new ER. Node" << node << "is not a ER, creating a null item anyway";
		//return nullptr;
	}
	ERScrollItem *item = new ERScrollItem(er);
	connect(item, &ERScrollItem::sDoubleClick, this, &ERScrollBox::sOpen);
	return item;
}

void ERScrollBox::setSelection(std::set<int> set, int last)
{
	ERScrollItem* curr;
	auto it = set.begin();
	for (size_t i = 0; i < m_items.size(); i++) {
		// move the iterator forward
		while (it != set.end() && i > *it) ++it;
		if (it == set.end() || *it > i) {
			curr = dynamic_cast<ERScrollItem*>(m_items[i]);
			curr->setStyleSheet("background:rgb(" + QString::number(curr->m_er->getRed()-(curr->m_er->getRed()/2)) + "," + QString::number(curr->m_er->getGreen()-(curr->m_er->getGreen()/2)) + "," + QString::number(curr->m_er->getBlue()-(curr->m_er->getBlue()/2)) + ");");
		}
		else if (*it == i) {
			curr = dynamic_cast<ERScrollItem*>(m_items[i]);
			curr->setStyleSheet("background:rgb(" + QString::number(curr->m_er->getRed()) + "," + QString::number(curr->m_er->getGreen()) + "," + QString::number(curr->m_er->getBlue()) + ");");
		}
	}
	m_selection = set;
	m_last_selected = last;

	// have the viewport focus the first selected item
	if (!set.empty()) {
		int first = *set.begin();
		if (first < 0) return;
		if (first >= m_items.size()) return;
		ensureWidgetVisible(m_items[first]);
	}
	emit sSelectionChange();

}