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