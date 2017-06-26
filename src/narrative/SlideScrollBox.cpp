
#include "SlideScrollBox.h"

SlideScrollBox::SlideScrollBox(QWidget * parent) 
	: HorizontalScrollBox(parent) 
{
	
	// initialize menus
	m_slide_menu = new QMenu(tr("Slide context menu"), this);
	m_action_new = new QAction("New Slide", m_slide_menu);
	m_action_delete = new QAction("Delete Slide", m_slide_menu);
	m_action_edit = new QAction("Edit Slide", m_slide_menu);

	m_slide_menu->addAction(m_action_new);
	m_slide_menu->addAction(m_action_delete);
	m_slide_menu->addAction(m_action_edit);

	connect(m_action_new, &QAction::triggered, this, [this]() {
		this->addItem();
	});
	connect(m_action_delete, &QAction::triggered, this, [this]() {
		this->deleteSelection();
	});

}

void SlideScrollBox::contextMenuEvent(QContextMenuEvent * event)
{
	qDebug() << "custom context menu shenanigans";
	m_slide_menu->exec(event->globalPos());
}

void SlideScrollBox::addItem()
{
	SlideScrollItem *new_item = new SlideScrollItem();
	SlideTransitionScrollItem *new_transition = new SlideTransitionScrollItem();

	HorizontalScrollBox::addItem(new_item);
	HorizontalScrollBox::addItem(new_transition);
}
