#include "NarrativeScrollBox.h"

NarrativeScrollBox::NarrativeScrollBox(QWidget * parent) : HorizontalScrollBox(parent) {
	
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

	//connect(m_action_new, &QAction::triggered, this, [this]() {
	//	addItem("weow", "cool");
	//});
	//connect(m_action_delete, &QAction::triggered, this, [this]() {
	//	this->deleteSelection();
	//});

}

NarrativeScrollBox::~NarrativeScrollBox() {
	
}

void NarrativeScrollBox::addItem(const std::string & title, const std::string & description)
{
	NarrativeScrollItem *item = new NarrativeScrollItem();
	item->setInfo(title, description);
	HorizontalScrollBox::addItem(item);
}

void NarrativeScrollBox::openMenu(QPoint globalPos)
{
	m_slide_menu->exec(globalPos);
}
void NarrativeScrollBox::openItemMenu(QPoint globalPos)
{
	m_slide_menu->exec(globalPos);
}

