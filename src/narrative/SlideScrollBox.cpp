
#include "SlideScrollBox.h"

SlideScrollBox::SlideScrollBox(QWidget * parent) 
	: HorizontalScrollBox(parent) 
{
	
	// slide menu
	m_bar_menu = new QMenu("Slide context menu", this);
	m_slide_menu = new QMenu("Slide context menu 2", this);

	m_action_new = new QAction("New Slide", m_slide_menu);
	m_action_delete = new QAction("Delete Slide", m_slide_menu);
	m_action_edit = new QAction("Edit Slide", m_slide_menu);
	m_action_set_duration = new QAction("Set Duration", m_slide_menu);
	m_action_set_camera = new QAction("Set Camera", m_slide_menu);
	m_action_set_transition = new QAction("Set Transition", m_slide_menu);

	m_bar_menu->addAction(m_action_new);

	m_slide_menu->addAction(m_action_new);
	m_slide_menu->addAction(m_action_delete);
	m_slide_menu->addAction(m_action_edit);
	m_slide_menu->addAction(m_action_set_duration);
	m_slide_menu->addAction(m_action_set_camera);
	m_slide_menu->addAction(m_action_set_transition);

	connect(m_action_new, &QAction::triggered, this, [this]() {
		this->addItem();
	});
	connect(m_action_delete, &QAction::triggered, this, [this]() {
		this->deleteSelection();
	});

	setSpacing(10);
}

void SlideScrollBox::addItem()
{
	SlideScrollItem *new_item = new SlideScrollItem();

	HorizontalScrollBox::addItem(new_item);
}

void SlideScrollBox::keyPressEvent(QKeyEvent *event) {
	qDebug() << "key yooo press" << event;
	HorizontalScrollBox::keyPressEvent(event);
}
void SlideScrollBox::handleSlideMouseEvent(QMouseEvent * event)
{
	qDebug() << "slide mouse press event";

	//if (event->button() == Qt::RightButton) {
	//	m_slide_menu->exec(event->globalPos());
	//}
}
void SlideScrollBox::handleTransitionMouseEvent(QMouseEvent * event)
{
	if (event->type() == QEvent::MouseButtonDblClick) {
		qDebug() << "double shenanigan";
	}
}
void SlideScrollBox::openMenu(QPoint globalPos) {
	m_bar_menu->exec(globalPos);
}

void SlideScrollBox::openItemMenu(QPoint globalPos)
{
	m_slide_menu->exec(globalPos);
}

//connect(new_item->getSlideWidget(), &SlideScrollWidget::sMousePressEvent, this,
//	[this](QMouseEvent* e) {
//	qDebug() << "slide mouse press event";
//
//	if (e->button() == Qt::RightButton) {
//		m_slide_menu->exec(e->globalPos());
//	}
//
//});