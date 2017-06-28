
#include "SlideScrollBox.h"

SlideScrollBox::SlideScrollBox(QWidget * parent) 
	: HorizontalScrollBox(parent) 
{
	
	// slide menu
	m_slide_menu = new QMenu(tr("Slide context menu"), this);
	m_action_new = new QAction("New Slide", m_slide_menu);
	m_action_delete = new QAction("Delete Slide", m_slide_menu);
	m_action_edit = new QAction("Edit Slide", m_slide_menu);
	m_action_set_duration = new QAction("Set Duration", m_slide_menu);
	m_action_set_camera = new QAction("Set Camera", m_slide_menu);

	m_slide_menu->addAction(m_action_new);
	m_slide_menu->addAction(m_action_delete);
	m_slide_menu->addAction(m_action_edit);
	m_slide_menu->addAction(m_action_set_duration);
	m_slide_menu->addAction(m_action_set_camera);

	connect(m_action_new, &QAction::triggered, this, [this]() {
		this->addItem();
	});
	connect(m_action_delete, &QAction::triggered, this, [this]() {
		this->deleteSelection();
	});

	setSpacing(10);
}

void SlideScrollBox::contextMenuEvent(QContextMenuEvent * event)
{
	qDebug() << "custom context menu shenanigans";
	//m_slide_menu->exec(event->globalPos());
}

void SlideScrollBox::addItem()
{
	SlideScrollItem *new_item = new SlideScrollItem();
	//new_item->setSpacing(5);
	
	//connect(new_item->getSlideWidget(), &SlideScrollWidget::sMousePressEvent, this,
	//	[this](QMouseEvent* e) {
	//	qDebug() << "slide mouse press event";

	//	if (e->button() == Qt::RightButton) {
	//		m_slide_menu->exec(e->globalPos());
	//	}
	//	
	//});

	HorizontalScrollBox::addItem(new_item);
}
