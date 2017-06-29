
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
	connect(m_action_set_duration, &QAction::triggered, this, [this]() {
		this->durationDialog(true, 10.0f);
	});
	connect(m_action_set_transition, &QAction::triggered, this, [this]() {
		this->transitionDialog(2.0f);
	});

	setSpacing(10);
}

void SlideScrollBox::addItem()
{
	SlideScrollItem *new_item = new SlideScrollItem();
	connect(new_item, &SlideScrollItem::sTransitionDoubleClick, this, 
		[this]() {
			transitionDialog(1.0f);
		});

	HorizontalScrollBox::addItem(new_item);
}

void SlideScrollBox::keyPressEvent(QKeyEvent *event) {
	qDebug() << "key yooo press" << event;
	HorizontalScrollBox::keyPressEvent(event);
}

void SlideScrollBox::openMenu(QPoint globalPos) {
	m_bar_menu->exec(globalPos);
}

void SlideScrollBox::openItemMenu(QPoint globalPos)
{
	m_slide_menu->exec(globalPos);
}

float SlideScrollBox::transitionDialog(float duration)
{
	return QInputDialog::getDouble(nullptr, "Transition Time", "Transition Time (seconds)", duration, 0.0, 3600.0, 1, nullptr, Qt::WindowSystemMenuHint);
}

float SlideScrollBox::durationDialog(bool stay, float duration)
{
	NarrativeSlideDurationDialog *dialog = new NarrativeSlideDurationDialog(nullptr);
	dialog->setWindowFlags(Qt::WindowSystemMenuHint);
	dialog->setDuration(stay, duration);
	int result = dialog->exec();
	float output;
	if (result == QDialog::Rejected) {
		output = -1.0f;
	}
	else {
		output = dialog->getDuration();
	}
	qDebug() << "set duration dialog result -" << output;
	delete dialog;
	return output;
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