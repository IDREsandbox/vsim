#include "NarrativeScrollBox.h"

NarrativeScrollBox::NarrativeScrollBox(QWidget * parent)
	: HorizontalScrollBox(parent)
{
	// initialize menus
	QMenu *m_menu = new QMenu(tr("Narrative menu"), this);
	QMenu *m_item_menu = new QMenu(tr("Narrative item menu"), this);

	QAction *m_action_new = new QAction("New Narrative", this);
	QAction *m_action_delete = new QAction("Delete Narrative", this);
	QAction *m_action_info = new QAction("Edit Narrative Info", this);
	QAction *m_action_open = new QAction("Open Narrative", this);

	m_menu->addAction(m_action_new);
	m_item_menu->addAction(m_action_new);
	m_item_menu->addAction(m_action_delete);
	m_item_menu->addAction(m_action_info);
	m_item_menu->addAction(m_action_open);

	// forward signals
	connect(m_action_new, &QAction::triggered, this, &NarrativeScrollBox::sNew);
	connect(m_action_delete, &QAction::triggered, this, &NarrativeScrollBox::sDelete);
	connect(m_action_info, &QAction::triggered, this, &NarrativeScrollBox::sInfo);
	connect(m_action_open, &QAction::triggered, this, &NarrativeScrollBox::sOpen);

	setMenu(m_menu);
	setItemMenu(m_item_menu);

	setMIMEType("application/x-narrative");
}

NarrativeScrollBox::~NarrativeScrollBox() {
	
}

ScrollBoxItem * NarrativeScrollBox::createItem(osg::Node * node)
{
	qDebug() << "text";
	Narrative2 *narrative = dynamic_cast<Narrative2*>(node);
	if (narrative == nullptr) {
		qWarning() << "Narrative scroll box insert new narrative. Node" << node << "is not a NarrativeSlide, creating a null item anyway";
		//return nullptr;
	}
	NarrativeScrollItem *item = new NarrativeScrollItem(this);
	item->setNarrative(narrative);
	connect(item, &NarrativeScrollItem::sDoubleClick, this, &NarrativeScrollBox::sOpen);
	return item;
}
