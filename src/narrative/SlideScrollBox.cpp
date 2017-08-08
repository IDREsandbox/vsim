
#include "SlideScrollBox.h"

SlideScrollBox::SlideScrollBox(QWidget * parent) 
	: HorizontalScrollBox(parent),
	m_narrative(nullptr)
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

	connect(m_action_new, &QAction::triggered, this, &SlideScrollBox::sNewSlide);
	connect(m_action_delete, &QAction::triggered, this, &SlideScrollBox::sDeleteSlides);
	connect(m_action_edit, &QAction::triggered, this, &SlideScrollBox::sEditSlide);
	connect(m_action_set_duration, &QAction::triggered, this, &SlideScrollBox::sSetDuration);
	connect(m_action_set_transition, &QAction::triggered, this, &SlideScrollBox::sSetTransitionDuration);
	connect(m_action_set_camera, &QAction::triggered, this, &SlideScrollBox::sSetCamera);

	setSpacing(10);
}

void SlideScrollBox::setNarrative(Narrative2 *narrative)
{
	// disconnect incoming signals if already connected to a narrative
	if (m_narrative != nullptr) disconnect(m_narrative, 0, this, 0);

	clear();
	m_narrative = narrative;
	if (narrative == nullptr) return;

	// listen to the insert and remove signals from m_narratives
	// so that we can add/remove from the slide box accordingly
	connect(m_narrative, &Narrative2::sNewSlide, this, &SlideScrollBox::newItem);
	connect(m_narrative, &Narrative2::sDeleteSlide, this, &HorizontalScrollBox::deleteItem);

	for (uint i = 0; i < narrative->getNumChildren(); i++) {
		newItem(i);
	}
}

//SlideScrollItem *SlideScrollBox::addItem()
//{
//	SlideScrollItem *new_item = new SlideScrollItem();
//	connect(new_item, &SlideScrollItem::sDurationDoubleClick, this, &SlideScrollBox::sSetDuration);
//	connect(new_item, &SlideScrollItem::sTransitionDoubleClick, this, &SlideScrollBox::sSetTransitionDuration);
//	HorizontalScrollBox::addItem(new_item);
//	return new_item;
//}

SlideScrollItem *SlideScrollBox::getItem(int index)
{
	return dynamic_cast<SlideScrollItem*>(HorizontalScrollBox::getItem(index));
}

void SlideScrollBox::newItem(int index)
{
	NarrativeSlide *slide = dynamic_cast<NarrativeSlide*>(m_narrative->getChild(index));
	if (slide == nullptr) {
		qWarning() << "insert new slide" << index << "is not a NarrativeSlide";
		return;
	}
	insertNewSlide(index, slide);
}

void SlideScrollBox::insertNewSlide(int index, NarrativeSlide *slide)
{
	qDebug() << "insert new slide";
	SlideScrollItem *new_item = new SlideScrollItem(slide);
	connect(new_item, &SlideScrollItem::sDurationDoubleClick, this, &SlideScrollBox::sSetDuration);
	connect(new_item, &SlideScrollItem::sTransitionDoubleClick, this, &SlideScrollBox::sSetTransitionDuration);
	connect(new_item, &SlideScrollItem::sThumbnailDirty, this, &SlideScrollBox::sThumbnailsDirty);
	HorizontalScrollBox::insertItem(index, new_item);

	// this thing emits signals, so we need it at the end
	new_item->setThumbnailDirty(true);
}

void SlideScrollBox::openMenu(QPoint globalPos) {
	m_bar_menu->exec(globalPos);
}

void SlideScrollBox::openItemMenu(QPoint globalPos)
{
	m_slide_menu->exec(globalPos);
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