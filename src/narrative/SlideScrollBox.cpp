﻿
#include "SlideScrollBox.h"

SlideScrollBox::SlideScrollBox(QWidget * parent) 
	: HorizontalScrollBox(parent) 
{
	
	// slide menu
	m_bar_menu = new QMenu("Slide context menu", this);
	m_slide_menu = new QMenu("Slide context menu 2", this);

	//m_slide_menu->setStyleSheet("background-color: rgb(255,255,255);");
	m_action_new = new QAction("New Slide", m_slide_menu);
	m_action_delete = new QAction("Delete Slide", m_slide_menu);
	m_action_set_duration = new QAction("Set Duration", m_slide_menu);
	m_action_set_camera = new QAction("Set Camera", m_slide_menu);
	m_action_set_transition = new QAction("Set Transition", m_slide_menu);

	m_bar_menu->addAction(m_action_new);

	m_slide_menu->addAction(m_action_new);
	m_slide_menu->addAction(m_action_delete);
	m_slide_menu->addAction(m_action_set_duration);
	m_slide_menu->addAction(m_action_set_camera);
	m_slide_menu->addAction(m_action_set_transition);

	connect(m_action_new, &QAction::triggered, this, &SlideScrollBox::sNewSlide);
	connect(m_action_delete, &QAction::triggered, this, &SlideScrollBox::sDeleteSlides);
	connect(m_action_set_duration, &QAction::triggered, this, &SlideScrollBox::durationDialog);
	connect(m_action_set_transition, &QAction::triggered, this, &SlideScrollBox::transitionDialog);
	connect(m_action_set_camera, &QAction::triggered, this, &SlideScrollBox::sSetCamera);

	setSpacing(10);
}

SlideScrollItem *SlideScrollBox::addItem()
{
	SlideScrollItem *new_item = new SlideScrollItem();
	connect(new_item, &SlideScrollItem::sTransitionDoubleClick, this, &SlideScrollBox::transitionDialog);
	connect(new_item, &SlideScrollItem::sDurationDoubleClick, this, &SlideScrollBox::durationDialog);
	HorizontalScrollBox::addItem(new_item);
	return new_item;
}
SlideScrollItem *SlideScrollBox::getItem(int index)
{
	return dynamic_cast<SlideScrollItem*>(HorizontalScrollBox::getItem(index));
}
void SlideScrollBox::transitionDialog()
{
	int last = getLastSelected();
	if (last == -1) return;
	SlideScrollItem *item = dynamic_cast<SlideScrollItem*>(getItem(last));
	float duration = execTransitionDialog(item->getTransition());
	emit sSetTransitionDuration(duration);
	//setTranstionDuration(duration); // uncomment to wire to self, otherwise the owner has to do this
}
void SlideScrollBox::durationDialog()
{
	int last = getLastSelected();
	if (last == -1) return;
	SlideScrollItem *item = dynamic_cast<SlideScrollItem*>(getItem(last));
	float duration = execDurationDialog(item->getDuration());
	if (duration < 0) return;
	emit sSetDuration(duration);
	//setDuration(duration);
}
float SlideScrollBox::execTransitionDialog(float duration)
{
	double d = QInputDialog::getDouble(nullptr, "Transition Time", "Transition Time (seconds)", duration, 0.0, 3600.0, 1, nullptr, Qt::WindowSystemMenuHint);
	qDebug() << "value of d" << d;
	return d;
}
float SlideScrollBox::execDurationDialog(float init_duration)
{
	NarrativeSlideDurationDialog *dialog = new NarrativeSlideDurationDialog(nullptr);
	dialog->setWindowFlags(Qt::WindowSystemMenuHint);
	
	if (init_duration == 0) {
		dialog->setDuration(true, 30);
	}
	else {
		dialog->setDuration(false, init_duration);
	}
	int result = dialog->exec();
	float duration;
	if (result == QDialog::Rejected) {
		duration = -1.0f;
	}
	else {
		duration = dialog->getDuration();
	}
	qDebug() << "set duration dialog result -" << duration;
	delete dialog;
	return duration;
}


void SlideScrollBox::openMenu(QPoint globalPos) {
	m_bar_menu->exec(globalPos);
}

void SlideScrollBox::openItemMenu(QPoint globalPos)
{
	m_slide_menu->exec(globalPos);
}

void SlideScrollBox::keyPressEvent(QKeyEvent *event) {
	qDebug() << "slide key press" << event;
	HorizontalScrollBox::keyPressEvent(event);
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