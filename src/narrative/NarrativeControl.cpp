#include "narrative/NarrativeControl.h"
#include "narrative/Narrative.h"
#include "Util.h"

#include <QObject>
#include <QDebug>
#include <QAction>
#include <QList>

NarrativeControl::NarrativeControl(QObject *parent, MainWindow *window)
	: QObject(parent), 
	m_window(window), 
	m_current_narrative(-1),
	m_model(nullptr)
{
	m_narrative_box = window->ui.topBar->ui.narratives;
	m_slide_box = window->ui.topBar->ui.slides;

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
	if (m_model == nullptr) {
		qDebug() << "failed to create new narrative - model is not initialized";
		return;
	}
	Narrative *narrative = new Narrative();
	narrative->setName(info.m_title);
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
	Narrative *narrative = dynamic_cast<Narrative*>(m_narrative_group->getChild(active_item));

	NarrativeInfoDialog *dlg = m_window->m_narrative_info_dialog;
	dlg->setInfo(*narrative);
	int result = dlg->exec();
	if (result == QDialog::Rejected) {
		qDebug() << "narrative list - cancelled edit on" << active_item;
		return;
	}
	
	// get the new info
	NarrativeInfo info = dlg->getInfo();
	narrative->setName(info.m_title);
	narrative->setDescription(info.m_description);
	narrative->setAuthor(info.m_contact);

	item->setInfo(info.m_title, info.m_description);
}

void NarrativeControl::deleteNarratives()
{
	std::set<int> selection = m_narrative_box->getSelection();
	std::vector<Narrative*> deletionList;

	// get pointers to nodes to delete
	for (auto i : selection) {
		Narrative *nar = dynamic_cast<Narrative*>(m_narrative_group->getChild(i));
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

void NarrativeControl::load(osg::Group * model)
{
	m_narrative_box->clear();
	m_slide_box->clear(); 
	m_current_narrative = -1;
	m_current_slide = -1;
	m_narrative_group = nullptr;
	m_model = model;
	closeNarrative();

	// new: load narratives in a NarrativeList group
	// search for a narrative list node, if not found then create one
	for (unsigned int i = 0; i < model->getNumChildren(); i++) {
		osg::Group* group = model->getChild(i)->asGroup();
		if (group) {
			std::string name = group->getName();
			if (name == "NarrativeList") {
				m_narrative_group = group->asGroup();
				qDebug() << "found NarrativeList in file";
				break;
			}
		}
	}

	// if no NarrativeList was found then create one
	if (m_narrative_group == nullptr) {
		m_narrative_group = new osg::Group;
		m_narrative_group->setName("NarrativeList");
		model->addChild(m_narrative_group);
		qDebug() << "didnt find NarrativeList in file - creating a new one";
	}

	// load narratives into the gui
	for (unsigned int i = 0; i < m_narrative_group->getNumChildren(); i++) {
		osg::Node* c = m_narrative_group->getChild(i);
		Narrative* nar = dynamic_cast<Narrative*>(c);
		if (nar) {
			// add item to gui
			addToGui(nar);
		}
		qDebug() << "loading narrative" << QString::fromStdString(nar->getName());
	}

	// old code: load narratives stored directly, for backward compatibility
	for (unsigned int i = 0; i < m_model->getNumChildren(); i++) {
		osg::Node* c = m_model->getChild(i);
		Narrative* nar = dynamic_cast<Narrative*>(c);
		if (nar) {
			qDebug() << "found old narrative" << nar->getName().c_str();
			// remove from old
			m_model->removeChild(nar);
			// add item to osg data structure
			m_narrative_group->addChild(nar);
			// add item to gui
			addToGui(nar);
		}
	}
}

void NarrativeControl::loadSlides(Narrative * narrative)
{
	m_slide_box->clear();
	for (unsigned int i = 0; i < narrative->getNumChildren(); i++) {
		NarrativeNode *node = dynamic_cast<NarrativeNode*>(narrative->getChild(i));
		if (!node) {
			qWarning() << "Load error: non-narrative detected in narrative children";
		}
		qDebug() << "loading slide" << node->getPauseAtNode() << node->getStayOnNode() << node->getTransitionDuration();

		addNodeToGui(node);
	}
}


void NarrativeControl::openNarrative()
{
	int index = m_narrative_box->getLastSelected();
	if (index < 0) return;

	this->m_window->ui.topBar->showSlides();
	m_current_narrative = index;

	Narrative *nar = getNarrative(index);
	this->m_window->ui.topBar->setSlidesHeader(nar->getName());

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

Narrative *NarrativeControl::getNarrative(int index)
{
	if (index >= m_narrative_group->getNumChildren() || index < 0) {
		return nullptr;
	}
	osg::Node *c = m_narrative_group->getChild(index);
	return dynamic_cast<Narrative*>(c);
}

NarrativeNode * NarrativeControl::getNarrativeNode(int narrative, int slide)
{
	Narrative *nar = getNarrative(narrative);
	if (!nar) return nullptr;
	if (slide >= nar->getNumChildren() || slide < 0) return nullptr;
	return dynamic_cast<NarrativeNode*>(nar->getChild(slide));
}

void NarrativeControl::newSlide()
{
	Narrative *nar = getNarrative(m_current_narrative);

	NarrativeNode *node = new NarrativeNode();
	node->setTransitionDuration(2.0f);
	node->setStayOnNode(false);
	node->setPauseAtNode(15.0f);
	
	node->setImage(Util::imageQtToOsg(generateThumbnail()));
	
	node->setViewMatrix(m_window->getViewer()->getCameraManipulator()->getMatrix());
	
	// add to osg
	nar->addChild(node);
	// add to gui
	addNodeToGui(node);
}

void NarrativeControl::deleteSlides()
{
	m_slide_box->deleteSelection();
}

void NarrativeControl::setSlideDuration(float duration)
{
	// data
	std::set<int> selection = m_slide_box->getSelection();
	for (auto slide : selection) {
		
		NarrativeNode *node = getNarrativeNode(m_current_narrative, slide);
		if (duration == 0) {
			node->setStayOnNode(true);
		}
		else {
			node->setPauseAtNode(duration);
		}

		SlideScrollItem *item = m_slide_box->getItem(slide);
		item->setDuration(duration);
	}
}

void NarrativeControl::setSlideTransition(float transition)
{
	// data
	std::set<int> selection = m_slide_box->getSelection();
	for (auto slide : selection) {
		NarrativeNode *node = getNarrativeNode(m_current_narrative, slide);
		node->setTransitionDuration(transition);

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
		NarrativeNode *node = getNarrativeNode(m_current_narrative, slide);
		node->setImage(Util::imageQtToOsg(new_thumbnail));
		node->setViewMatrix(m_window->getViewer()->getCameraManipulator()->getMatrix());

		SlideScrollItem *item = m_slide_box->getItem(slide);
		item->setImage(new_thumbnail);
	}
}

void NarrativeControl::addToGui(Narrative *nar)
{
	m_narrative_box->addItem(nar->getName(), nar->getDescription());
}

void NarrativeControl::addNodeToGui(NarrativeNode *node)
{
	SlideScrollItem *newitem = m_slide_box->addItem();
	newitem->setTransition(node->getTransitionDuration());
	if (node->getStayOnNode()) {
		newitem->setDuration(0.0f);
	}
	else {
		newitem->setDuration(node->getPauseAtNode());
	}
	newitem->setImage(Util::imageOsgToQt(node->getImage()));
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
