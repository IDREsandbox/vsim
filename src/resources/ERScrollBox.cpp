#include "resources/ERScrollBox.h"
#include "ERFilterSortProxy.h"

ERScrollBox::ERScrollBox(QWidget * parent)
	: HorizontalScrollBox(parent)
{
	// initialize menus
	QMenu *m_menu = new QMenu(tr("ER menu"), this);
	QMenu *m_item_menu = new QMenu(tr("ER item menu"), this);

	QAction *m_action_new = new QAction("New Embedded Resource", this);
	QAction *m_action_delete = new QAction("Delete Embedded Resource", this);
	QAction *m_action_edit = new QAction("Edit Embedded Resource", this);
	QAction *m_action_open = new QAction("Open Embedded Resource", this);
	QAction *m_action_set = new QAction("Set Position", this);
	QAction *m_action_goto = new QAction("Go to Position", this);

	m_menu->addAction(m_action_new);
	m_item_menu->addAction(m_action_new);
	m_item_menu->addAction(m_action_delete);
	m_item_menu->addAction(m_action_edit);
	m_item_menu->addAction(m_action_open);
	m_item_menu->addAction(m_action_set);
	m_item_menu->addAction(m_action_goto);

	// forward signals
	connect(m_action_new, &QAction::triggered, this, &ERScrollBox::sNew);
	connect(m_action_delete, &QAction::triggered, this, &ERScrollBox::sDelete);
	connect(m_action_edit, &QAction::triggered, this, &ERScrollBox::sEdit);
	connect(m_action_open, &QAction::triggered, this, &ERScrollBox::sOpen);
	connect(m_action_set, &QAction::triggered, this, &ERScrollBox::sSetPosition);
	connect(m_action_goto, &QAction::triggered, this, &ERScrollBox::sGotoPosition);

	setMenu(m_menu);
	setItemMenu(m_item_menu);
}

ERScrollBox::~ERScrollBox() {
	
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