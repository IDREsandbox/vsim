
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

	m_action_new->setShortcut(Qt::CTRL + Qt::Key_M);

	m_bar_menu->addAction(m_action_new);

	m_slide_menu->addAction(m_action_new);
	m_slide_menu->addAction(m_action_delete);
	m_slide_menu->addAction(m_action_edit);
	m_slide_menu->addAction(m_action_set_duration);
	m_slide_menu->addAction(m_action_set_camera);
	m_slide_menu->addAction(m_action_set_transition);

	connect(m_action_new, &QAction::triggered, this, &SlideScrollBox::sNewSlide);
	connect(m_action_delete, &QAction::triggered, this, &SlideScrollBox::sDeleteSlides);
	connect(m_action_edit, &QAction::triggered, this, &SlideScrollBox::sEditSlide);
	connect(m_action_set_duration, &QAction::triggered, this, &SlideScrollBox::sSetDuration);
	connect(m_action_set_transition, &QAction::triggered, this, &SlideScrollBox::sSetTransitionDuration);
	connect(m_action_set_camera, &QAction::triggered, this, &SlideScrollBox::sSetCamera);

	setSpacing(10);

	setMenu(m_bar_menu);
	setItemMenu(m_slide_menu);
}

SlideScrollItem *SlideScrollBox::getItem(int index)
{
	return dynamic_cast<SlideScrollItem*>(HorizontalScrollBox::getItem(index));
}

ScrollBoxItem * SlideScrollBox::createItem(osg::Node * node)
{
	NarrativeSlide *slide = dynamic_cast<NarrativeSlide*>(node);
	if (slide == nullptr) {
		qWarning() << "insert new slide" << node << "is not a NarrativeSlide";
		return nullptr;
	}
	SlideScrollItem *item = new SlideScrollItem(slide);
	connect(item, &SlideScrollItem::sDurationDoubleClick, this, &SlideScrollBox::sSetDuration);
	connect(item, &SlideScrollItem::sTransitionDoubleClick, this, &SlideScrollBox::sSetTransitionDuration);
	connect(item, &SlideScrollItem::sThumbnailDirty, this, &SlideScrollBox::sThumbnailsDirty);

	item->setThumbnailDirty(true); // hmm

	return item;
}

std::vector<SlideScrollItem*> SlideScrollBox::getDirtySlides()
{
	std::vector<SlideScrollItem*> items;
	for (auto item : m_items) {
		SlideScrollItem *slide_item = dynamic_cast<SlideScrollItem*>(item);
		if (slide_item->thumbnailDirty()) {
			items.push_back(slide_item);
		}
	}
	return items;
}

void SlideScrollBox::paintEvent(QPaintEvent * event)
{
	auto dirty = getDirtySlides();
	if (!dirty.empty()) {
		emit sThumbnailsDirty();
	}
	HorizontalScrollBox::paintEvent(event);
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