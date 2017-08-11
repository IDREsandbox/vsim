#include "NarrativeScrollBox.h"

NarrativeScrollBox::NarrativeScrollBox(QWidget * parent)
	: HorizontalScrollBox(parent)
{
	
	// initialize menus
	m_slide_menu = new QMenu(tr("Slide context menu"), this);
	m_action_new = new QAction("New Narrative", m_slide_menu);
	m_action_delete = new QAction("Delete Narrative", m_slide_menu);
	m_action_info = new QAction("Edit Narrative Info", m_slide_menu);
	m_action_open = new QAction("Open Narrative", m_slide_menu);

	m_slide_menu->addAction(m_action_new);
	m_slide_menu->addAction(m_action_delete);
	m_slide_menu->addAction(m_action_info);
	m_slide_menu->addAction(m_action_open);

	// forward signals
	connect(m_action_new, &QAction::triggered, this, &NarrativeScrollBox::sNew);
	connect(m_action_delete, &QAction::triggered, this, &NarrativeScrollBox::sDelete);
	connect(m_action_info, &QAction::triggered, this, &NarrativeScrollBox::sInfo);
	connect(m_action_open, &QAction::triggered, this, &NarrativeScrollBox::sOpen);

	setMenu(m_slide_menu);
	setItemMenu(m_slide_menu);
}

NarrativeScrollBox::~NarrativeScrollBox() {
	
}

ScrollBoxItem * NarrativeScrollBox::createItem(osg::Node * node)
{
	Narrative2 *narrative = dynamic_cast<Narrative2*>(node);
	if (narrative == nullptr) {
		qWarning() << "Narrative scroll box insert new narrative. Node" << node << "is not a NarrativeSlide, creating a null item anyway";
		//return nullptr;
	}
	NarrativeScrollItem *item = new NarrativeScrollItem(narrative);
	connect(item, &NarrativeScrollItem::sDoubleClick, this, &NarrativeScrollBox::sOpen);
	return item;
}
