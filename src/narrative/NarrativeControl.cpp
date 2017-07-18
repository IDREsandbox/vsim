#include "narrative/NarrativeControl.h"
#include "narrative/Narrative2.h"
#include "Util.h"

#include <QObject>
#include <QDebug>
#include <QAction>
#include <QList>
#include <osg/Matrix>
#include <osg/io_utils>

NarrativeControl::NarrativeControl(QObject *parent, MainWindow *window)
	: QObject(parent), 
	m_window(window), 
	m_current_narrative(-1),
	m_model(nullptr)
{
	m_narrative_box = window->ui.topBar->ui.narratives;
	m_slide_box = window->ui.topBar->ui.slides;
	m_canvas = window->m_drag_area;

	// NARRATIVE CONTROL
	// new
	connect(m_narrative_box, &NarrativeScrollBox::sNew, this, &NarrativeControl::newNarrative);
	connect(m_window->ui.topBar->ui.plus, &QPushButton::clicked, this, &NarrativeControl::newNarrative);
	// delete
	connect(m_narrative_box, &NarrativeScrollBox::sDelete, this, &NarrativeControl::deleteNarratives);
	connect(m_window->ui.topBar->ui.minus, &QPushButton::clicked, this, &NarrativeControl::deleteNarratives);
	// info
	connect(m_narrative_box, &NarrativeScrollBox::sInfo, this, &NarrativeControl::editNarrativeInfo);
	connect(m_window->ui.topBar->ui.info, &QPushButton::clicked, this, &NarrativeControl::editNarrativeInfo);
	// open
	connect(m_narrative_box, &NarrativeScrollBox::sOpen, this, &NarrativeControl::openNarrative);
	connect(m_window->ui.topBar->ui.open, &QPushButton::clicked, this, &NarrativeControl::openNarrative);


	// SLIDE CONTROL
	// new
	connect(m_slide_box, &SlideScrollBox::sNewSlide, this, &NarrativeControl::newSlide);
	connect(m_window->ui.topBar->ui.plus_2, &QPushButton::clicked, this, &NarrativeControl::newSlide);
	// delete
	connect(m_slide_box, &SlideScrollBox::sDeleteSlides, this, &NarrativeControl::deleteSlides);
	connect(m_window->ui.topBar->ui.minus_2, &QPushButton::clicked, this, &NarrativeControl::deleteSlides);
	// edit
	connect(m_slide_box, &SlideScrollBox::sEditSlide, this, &NarrativeControl::editSlide);
	connect(m_window->ui.topBar->ui.open_2, &QPushButton::clicked, this, &NarrativeControl::editSlide);
	// duration
	connect(m_slide_box, &SlideScrollBox::sSetDuration, this, &NarrativeControl::setSlideDuration);
	// transition
	connect(m_slide_box, &SlideScrollBox::sSetTransitionDuration, this, &NarrativeControl::setSlideTransition);
	// camera
	connect(m_slide_box, &SlideScrollBox::sSetCamera, this, &NarrativeControl::setSlideCamera);
	// back
	connect(m_window->ui.topBar->ui.left_2, &QPushButton::clicked, this, &NarrativeControl::closeNarrative);
	
}

NarrativeControl::~NarrativeControl()
{
}

void NarrativeControl::newNarrative()
{
	// open up the dialog
	NarrativeInfoDialog *dlg = m_window->m_narrative_info_dialog;
	dlg->clear();
	int result = dlg->exec();
	if (result == QDialog::Rejected) {
		return;
	}
	NarrativeInfo info = dlg->getInfo();

	// add item to osg and to here
	if (m_narrative_group == nullptr) {
		qWarning() << "Error: failed to create new narrative - model is not initialized";
		return;
	}
	Narrative2 *narrative = new Narrative2();
	narrative->setTitle(info.m_title);
	narrative->setAuthor(info.m_contact);
	narrative->setDescription(info.m_description);
	m_narrative_group->addChild(narrative);

	// add to gui
	addToGui(narrative);
}

void NarrativeControl::editNarrativeInfo()
{
	int active_item = m_narrative_box->getLastSelected();
	qDebug() << "narrative list - begin edit on" << active_item;
	if (active_item < 0) {
		qWarning() << "narrative list - can't edit with no selection";
		return;
	}

	NarrativeScrollItem *item = dynamic_cast<NarrativeScrollItem*>(m_narrative_box->getItem(active_item));
	Narrative2 *narrative = dynamic_cast<Narrative2*>(m_narrative_group->getChild(active_item));

	NarrativeInfoDialog *dlg = m_window->m_narrative_info_dialog;
	dlg->setInfo(*narrative);
	int result = dlg->exec();
	if (result == QDialog::Rejected) {
		qDebug() << "narrative list - cancelled edit on" << active_item;
		return;
	}
	
	// get the new info
	NarrativeInfo info = dlg->getInfo();
	narrative->setTitle(info.m_title);
	narrative->setDescription(info.m_description);
	narrative->setAuthor(info.m_contact);

	item->setInfo(info.m_title, info.m_description);
}

void NarrativeControl::deleteNarratives()
{
	std::set<int> selection = m_narrative_box->getSelection();
	std::vector<Narrative2*> deletionList;

	// get pointers to nodes to delete
	for (auto i : selection) {
		Narrative2 *nar = dynamic_cast<Narrative2*>(m_narrative_group->getChild(i));
		if (nar == NULL) {
			qWarning() << "detected a non-narrative in the osg narrative group";
		}
		deletionList.push_back(nar);
	}
	// delete them
	for (auto delete_me : deletionList) {
		m_narrative_group->removeChild(delete_me);
	}

	m_narrative_box->deleteSelection();
}

void NarrativeControl::load(osg::Group *narratives)
{
	m_narrative_box->clear();
	m_slide_box->clear(); 
	m_current_narrative = -1;
	m_current_slide = -1;
	m_narrative_group = narratives;
	closeNarrative();

	// load narratives into the gui
	for (unsigned int i = 0; i < m_narrative_group->getNumChildren(); i++) {
		osg::Node* c = m_narrative_group->getChild(i);
		Narrative2* nar = dynamic_cast<Narrative2*>(c);
		if (nar) {
			// add item to gui
			addToGui(nar);
		}
		qDebug() << "loading narrative to gui -" << QString::fromStdString(nar->getTitle());
	}
}

void NarrativeControl::loadSlides(Narrative2 *narrative)
{
	m_slide_box->clear();
	for (unsigned int i = 0; i < narrative->getNumChildren(); i++) {
		NarrativeSlide *node = dynamic_cast<NarrativeSlide*>(narrative->getChild(i));
		if (!node) {
			qWarning() << "Load error: non-narrative detected in narrative children";
		}
		qDebug() << "loading slide" << node->getDuration() << node->getStayOnNode() << node->getTransitionDuration();

		addNodeToGui(node);
	}
}


void NarrativeControl::openNarrative()
{
	int index = m_narrative_box->getLastSelected();
	if (index < 0) return;

	this->m_window->ui.topBar->showSlides();
	m_current_narrative = index;

	Narrative2 *nar = getNarrative(index);
	this->m_window->ui.topBar->setSlidesHeader(nar->getTitle());

	loadSlides(getNarrative(index));
}

void NarrativeControl::closeNarrative()
{
	m_current_narrative = -1;
	this->m_window->ui.topBar->showNarratives();
}

void NarrativeControl::openSlide()
{
	int index = m_slide_box->getLastSelected();
}

Narrative2 *NarrativeControl::getNarrative(int index)
{
	if (index >= m_narrative_group->getNumChildren() || index < 0) {
		return nullptr;
	}
	osg::Node *c = m_narrative_group->getChild(index);
	return dynamic_cast<Narrative2*>(c);
}

NarrativeSlide * NarrativeControl::getNarrativeNode(int narrative, int slide)
{
	Narrative2 *nar = getNarrative(narrative);
	if (!nar) return nullptr;
	if (slide >= nar->getNumChildren() || slide < 0) return nullptr;
	return dynamic_cast<NarrativeSlide*>(nar->getChild(slide));
}

void NarrativeControl::newSlide()
{
	Narrative2 *nar = getNarrative(m_current_narrative);

	NarrativeSlide *node = new NarrativeSlide();
	node->setTransitionDuration(2.0f);
	node->setStayOnNode(false);
	node->setDuration(15.0f);
	
	node->setThumbnail(Util::imageQtToOsg(generateThumbnail()));
	
	node->setCameraMatrix(m_window->getViewer()->getCameraManipulator()->getMatrix());
	
	// add to osg
	nar->addChild(node);
	// add to gui
	addNodeToGui(node);
}

void NarrativeControl::deleteSlides()
{
	std::set<int> selection = m_slide_box->getSelection();
	std::vector<NarrativeSlide*> nodes;
	
	// data
	for (int slide : selection) {
		nodes.push_back(getNarrativeNode(m_current_narrative, slide));
	}
	for (NarrativeSlide *node : nodes) {
		Narrative2 *nar = getNarrative(m_current_narrative);
		nar->removeChild(node);
	}

	// gui
	m_slide_box->deleteSelection();
}

void NarrativeControl::editSlide() {
	m_canvas->editCanvas();
}

void NarrativeControl::setSlideDuration(float duration)
{
	std::set<int> selection = m_slide_box->getSelection();
	for (auto slide : selection) {
		// data
		NarrativeSlide *node = getNarrativeNode(m_current_narrative, slide);
		if (duration == 0) {
			node->setStayOnNode(true);
		}
		else {
			node->setDuration(duration);
		}

		// gui
		SlideScrollItem *item = m_slide_box->getItem(slide);
		item->setDuration(duration);
	}
}

void NarrativeControl::setSlideTransition(float transition)
{
	std::set<int> selection = m_slide_box->getSelection();
	for (auto slide : selection) {
		NarrativeSlide *node = getNarrativeNode(m_current_narrative, slide);
		// data
		node->setTransitionDuration(transition);

		// gui
		SlideScrollItem *item = m_slide_box->getItem(slide);
		item->setTransition(transition);
	}
}

void NarrativeControl::setSlideCamera()
{
	std::set<int> selection = m_slide_box->getSelection();
	QImage new_thumbnail = generateThumbnail();
	// widget dimensions
	for (auto slide : selection) {
		NarrativeSlide *node = getNarrativeNode(m_current_narrative, slide);
		node->setThumbnail(Util::imageQtToOsg(new_thumbnail));
		node->setCameraMatrix(m_window->getViewer()->getCameraManipulator()->getMatrix());

		SlideScrollItem *item = m_slide_box->getItem(slide);
		item->setImage(new_thumbnail);
	}
}

void NarrativeControl::addToGui(Narrative2 *nar)
{
	m_narrative_box->addItem(nar->getTitle(), nar->getDescription());
}

void NarrativeControl::addNodeToGui(NarrativeSlide *node)
{
	SlideScrollItem *newitem = m_slide_box->addItem();
	newitem->setTransition(node->getTransitionDuration());
	if (node->getStayOnNode()) {
		newitem->setDuration(0.0f);
	}
	else {
		newitem->setDuration(node->getDuration());
	}
	newitem->setImage(Util::imageOsgToQt(node->getThumbnail()));
}

QImage NarrativeControl::generateThumbnail()
{
	// widget dimensions
	QRect dims = m_window->m_osg_widget->geometry();
	// screenshot dimensions
	QRect ssdims = Util::rectFit(dims, 16.0 / 9.0);

	QImage img(ssdims.width(), ssdims.height(), QImage::Format_RGB444);
	QPainter painter(&img);
	m_window->m_osg_widget->render(&painter, QPoint(0, 0), QRegion(ssdims));

	// optional, fewer big screenshots
	QImage smallimg;
	smallimg = img.scaled(288, 162, Qt::IgnoreAspectRatio);
	return smallimg;
}
