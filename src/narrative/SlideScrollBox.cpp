
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

	setSpacing(0);
}

void SlideScrollBox::contextMenuEvent(QContextMenuEvent * event)
{
	qDebug() << "custom context menu shenanigans";
	//m_slide_menu->exec(event->globalPos());
}

void SlideScrollBox::addItem()
{
	SlideScrollItem *new_item = new SlideScrollItem();

	
	connect(new_item->getSlideWidget(), &SlideScrollWidget::sMousePressEvent, this,
		[this](QMouseEvent* e) {
		qDebug() << "slide mouse press event";
	});

	connect(new_item->getTransitionWidget(), &SlideTransitionScrollWidget::sMousePressEvent, this,
		[this](QMouseEvent* e) {
		qDebug() << "transition mouse press event";
	});

	HorizontalScrollBox::addItem(new_item);
}
