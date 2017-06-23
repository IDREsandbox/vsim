#include "NarrativeScrollBox.h"

NarrativeScrollBox::NarrativeScrollBox(QWidget * parent) : HorizontalScrollBox(parent) {
	
	// initialize menus
	m_slide_menu = new QMenu(tr("Slide context menu"), this);
	m_action_new = new QAction("New Narrative", m_slide_menu);
	m_action_delete = new QAction("Delete Narrative", m_slide_menu);
	m_action_edit = new QAction("Edit Narrative", m_slide_menu);

	m_slide_menu->addAction(m_action_new);
	m_slide_menu->addAction(m_action_delete);
	m_slide_menu->addAction(m_action_edit);

	connect(m_action_new, &QAction::triggered, this, [this]() {
		addItem("weow", "cool");
	});
	connect(m_action_delete, &QAction::triggered, this, [this]() {
		this->deleteSelection();
	});

}

NarrativeScrollBox::~NarrativeScrollBox() {
	
}

void NarrativeScrollBox::contextMenuEvent(QContextMenuEvent * event)
{
	qDebug() << "custom context menu shenanigans";
	m_slide_menu->exec(event->globalPos());
}

void NarrativeScrollBox::addItem(const std::string & title, const std::string & description)
{
	NarrativeScrollItem *new_item = new NarrativeScrollItem(title, description);
	HorizontalScrollBox::addItem(new_item);
}
