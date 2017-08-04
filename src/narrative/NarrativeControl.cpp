#include "narrative/NarrativeControl.h"
#include "narrative/Narrative2.h"
#include "Util.h"

#include <QObject>
#include <QDebug>
#include <QAction>
#include <QList>
#include <osg/Matrix>
#include <osg/io_utils>
#include <QUndoStack>

NarrativeControl::NarrativeControl(QObject *parent, MainWindow *window)
	: QObject(parent), 
	m_window(window), 
	m_current_narrative(-1),
	m_model(nullptr)
{
	m_narrative_box = window->ui.topBar->ui.narratives;
	m_slide_box = window->ui.topBar->ui.slides;
	m_canvas = window->m_drag_area;
	m_undo_stack = window->m_undo_stack;

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
	//change
	connect(m_slide_box, SIGNAL(sSelectionChange()), this, SLOT(openSlide()));
	
	//CANVAS CONTROL
	// new
	connect(m_canvas, SIGNAL(sNewLabel(std::string, int)), this, SLOT(newLabel(std::string, int)));
	// move
	connect(m_canvas, SIGNAL(sSuperSizeSet(QSize, int)), this, SLOT(resizeLabel(QSize, int)));
	// resize
	connect(m_canvas, SIGNAL(sSuperPosSet(QPoint, int)), this, SLOT(moveLabel(QPoint, int)));
	// text change
	connect(m_canvas, SIGNAL(sSuperTextSet(QString, int)), this, SLOT(textEditLabel(QString, int)));


	// dirty slide thumbnails
	connect(m_slide_box, &SlideScrollBox::sThumbnailsDirty, this, 
		[this]() {redrawThumbnails(m_slide_box->getDirtySlides()); }
		);
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

//void NarrativeControl::loadSlides(Narrative2 *narrative)
//{
//	m_slide_box->clear();
//	for (unsigned int i = 0; i < narrative->getNumChildren(); i++) {
//		NarrativeSlide *node = dynamic_cast<NarrativeSlide*>(narrative->getChild(i));
//		if (!node) {
//			qWarning() << "Load error: non-narrative detected in narrative children";
//		}
//		//qDebug() << "loading slide" << node->getDuration() << node->getStayOnNode() << node->getTransitionDuration();
//
//		addNodeToGui(node);
//	}
//}


void NarrativeControl::openNarrative()
{
	int index = m_narrative_box->getLastSelected();
	if (index < 0) return;

	qDebug() << "open narrative at" << index;
	this->m_window->ui.topBar->showSlides();
	m_current_narrative = index;

	Narrative2 *nar = getNarrative(index);
	this->m_window->ui.topBar->setSlidesHeader(nar->getTitle());
	m_slide_box->setNarrative(nar);

	//m_canvas->exitEdit();
}

void NarrativeControl::closeNarrative()
{
	m_current_narrative = -1;
	this->m_window->ui.topBar->showNarratives();
	m_canvas->clearCanvas();
}

void NarrativeControl::openSlide() //should handle loading widgets onto canvas
{
	//m_current_slide = m_slide_box->getLastSelected();
	//if (m_current_slide < 0) return;

	//NarrativeSlide* curSl = getNarrativeSlide(m_current_narrative, m_current_slide);
	//m_canvas->clearCanvas();
	//NarrativeSlideLabels* data;

	//for (uint i = 0; i < curSl->getNumChildren(); i++) {
	//	data = dynamic_cast<NarrativeSlideLabels*>(curSl->getChild(i));
	//	m_canvas->newLabel(data->getStyle(), data->getText(), data->getrX(), data->getrY(), data->getrW(), 
	//		data->getrH());
	//}

	//curSl->setThumbnail(Util::imageQtToOsg(generateThumbnail()));
}

void NarrativeControl::newLabel(std::string str, int idx) {
	NarrativeSlideLabels* lab = new NarrativeSlideLabels();
	dragLabel* temp = m_canvas->m_items.at(idx);
	lab->setrX(temp->ratioX);
	lab->setrY(temp->ratioY);
	lab->setrW(temp->ratioWidth);
	lab->setrH(temp->ratioHeight);
	lab->setText(temp->toHtml().toStdString());//ext().toStdString());
	lab->setStyle(str);

	m_current_slide = m_slide_box->getLastSelected();
	if (m_current_slide < 0) return;

	NarrativeSlide* curSl = getNarrativeSlide(m_current_narrative, m_current_slide);
	curSl->addChild(lab);

	m_canvas->exitEdit();
	QImage new_thumbnail = generateThumbnail();
	m_canvas->editCanvas();
	//curSl->setThumbnail(Util::imageQtToOsg(new_thumbnail));

	SlideScrollItem *item = m_slide_box->getItem(m_current_slide);
	item->setImage(new_thumbnail);
}

void NarrativeControl::moveLabel(QPoint pos, int idx) {
	NarrativeSlide* curSl = getNarrativeSlide(m_current_narrative, m_current_slide);
	NarrativeSlideLabels* lab = dynamic_cast<NarrativeSlideLabels*>(curSl->getChild(idx));
	dragLabel* temp = m_canvas->m_items.at(idx);
	lab->setrX(temp->ratioX);
	lab->setrY(temp->ratioY);

	int flag = 0;
	if (m_canvas->editDlg->isVisible()) {
		m_canvas->exitEdit();
		flag = 1;
	}

	QImage new_thumbnail = generateThumbnail();
	if (flag == 1)
		m_canvas->editCanvas();
	//curSl->setThumbnail(Util::imageQtToOsg(new_thumbnail));

	SlideScrollItem *item = m_slide_box->getItem(m_current_slide);
	item->setImage(new_thumbnail);
}

void NarrativeControl::resizeLabel(QSize size, int idx) {
	NarrativeSlide* curSl = getNarrativeSlide(m_current_narrative, m_current_slide);
	NarrativeSlideLabels* lab = dynamic_cast<NarrativeSlideLabels*>(curSl->getChild(idx));
	dragLabel* temp = m_canvas->m_items.at(idx);
	lab->setrH(temp->ratioHeight);
	lab->setrW(temp->ratioWidth);

	int flag = 0;
	if (m_canvas->editDlg->isVisible()) {
		m_canvas->exitEdit();
		flag = 1;
	}
	QImage new_thumbnail = generateThumbnail();
	if (flag == 1)
		m_canvas->editCanvas();
	//curSl->setThumbnail(Util::imageQtToOsg(new_thumbnail));

	SlideScrollItem *item = m_slide_box->getItem(m_current_slide);
	item->setImage(new_thumbnail);
}

void NarrativeControl::textEditLabel(QString str, int idx) {
	NarrativeSlide* curSl = getNarrativeSlide(m_current_narrative, m_current_slide);
	NarrativeSlideLabels* lab = dynamic_cast<NarrativeSlideLabels*>(curSl->getChild(idx));
	lab->setText(str.toStdString());
}

Narrative2 *NarrativeControl::getNarrative(int index)
{
	if (index < 0 || (uint)index >= m_narrative_group->getNumChildren()) {
		return nullptr;
	}
	osg::Node *c = m_narrative_group->getChild(index);
	return dynamic_cast<Narrative2*>(c);
}

NarrativeSlide * NarrativeControl::getNarrativeSlide(int narrative, int slide)
{
	Narrative2 *nar = getNarrative(narrative);
	if (!nar) return nullptr;
	if (slide < 0 || (uint)slide >= nar->getNumChildren()) return nullptr;
	return dynamic_cast<NarrativeSlide*>(nar->getChild(slide));
}

void NarrativeControl::newSlide()
{
	//m_canvas->clearCanvas();
	Narrative2 *nar = getNarrative(m_current_narrative);
	auto matrix = m_window->m_osg_widget->getCameraMatrix();
	auto thumbnail = generateThumbnail();

	// figure out where to insert
	int index;
	if (m_slide_box->getSelection().empty() || m_slide_box->getLastSelected() < 0) {
		// no selection? insert at the end
		index = nar->getNumChildren();
	}
	else {
		// insert after the last selected
		index = m_slide_box->getLastSelected() + 1;
	}

	// perform command
	m_undo_stack->push(new Narrative2::NewSlideCommand(nar, index));
	
	// initialization
	NarrativeSlide *slide = getNarrativeSlide(m_current_narrative, index);
	slide->setCameraMatrix(matrix);

	// initializing the new slide

	// canvas stuff
	int flag = 0;
	if (m_canvas->editDlg->isVisible()) {
		m_canvas->exitEdit();
		flag = 1;
	}

	if (flag == 1)
		m_canvas->editCanvas();
	
	
}

void NarrativeControl::deleteSlides()
{
	qDebug() << "delete slides";
	std::set<int> selection = m_slide_box->getSelection();
	Narrative2 *nar = getNarrative(m_current_narrative);
	
	// perform command
	m_undo_stack->beginMacro("Delete Narratives");
	// we need to delete in reverse order to get the indices right
	for (auto i = selection.rbegin(); i != selection.rend(); ++i) {
		qDebug() << "pushing delete" << *i;
		m_undo_stack->push(new Narrative2::DeleteSlideCommand(nar, *i));
	}
	m_undo_stack->endMacro();
}

void NarrativeControl::editSlide() {
	m_canvas->editCanvas();
}

void NarrativeControl::setSlideDuration()
{
	qDebug() << "set slide duration";
	std::set<int> selection = m_slide_box->getSelection();
	if (selection.empty()) return;
	
	// Create the duration dialog, initialize values to the last item selected
	int slide_index = m_slide_box->getLastSelected();
	NarrativeSlide *first_slide = getNarrativeSlide(m_current_narrative, slide_index);
	float duration = NarrativeSlideDurationDialog::create(first_slide->getStayOnNode(), first_slide->getDuration());
	if (duration < 0) return;

	// perform the command
	m_undo_stack->beginMacro("Set Duration");
	for (auto index : selection) {
		NarrativeSlide *slide = getNarrativeSlide(m_current_narrative, index);
		if (duration == 0) {
			m_window->m_undo_stack->push(new NarrativeSlide::SetStayOnNodeCommand(slide, true));
		}
		else {
			m_window->m_undo_stack->push(new NarrativeSlide::SetDurationCommand(slide, duration));
		}
	}
	m_undo_stack->endMacro();
}

void NarrativeControl::setSlideTransition()
{
	std::set<int> selection = m_slide_box->getSelection();
	if (selection.empty()) return;

	// intial values for the dialog by looking up the first selection
	int slide_index = *selection.begin();
	NarrativeSlide *first_slide = getNarrativeSlide(m_current_narrative, slide_index);
	float duration = QInputDialog::getDouble(nullptr, "Transition Time", "Transition Time (seconds)", first_slide->getDuration(), 0.0, 3600.0, 1, nullptr, Qt::WindowSystemMenuHint);

	QUndoStack *stack = m_window->m_undo_stack;
	stack->beginMacro("Set Transition Duration");
	for (auto index : selection) {
		NarrativeSlide *slide = getNarrativeSlide(m_current_narrative, index);
		stack->push(new NarrativeSlide::SetTransitionDurationCommand(slide, duration));
	}
	stack->endMacro();
}

void NarrativeControl::setSlideCamera()
{
	std::set<int> selection = m_slide_box->getSelection();
	QImage new_thumbnail = generateThumbnail();
	osg::Matrixd matrix = m_window->m_osg_widget->getCameraMatrix();

	QUndoStack *stack = m_window->m_undo_stack;
	stack->beginMacro("Set Transition Duration");
	for (auto index : selection) {
		NarrativeSlide *slide = getNarrativeSlide(m_current_narrative, index);
		stack->push(new NarrativeSlide::SetCameraMatrixCommand(slide, matrix));
	}
	stack->endMacro();
}

void NarrativeControl::addToGui(Narrative2 *nar)
{
	m_narrative_box->addItem(nar->getTitle(), nar->getDescription());
}

//void NarrativeControl::addNodeToGui(NarrativeSlide *node)
//{
//	SlideScrollItem *newitem = m_slide_box->addItem();
//	newitem->setSlide(node);
//}

void NarrativeControl::redrawThumbnails(const std::vector<SlideScrollItem*>& slides)
{
	osg::Matrixd old_matrix = m_window->m_osg_widget->getCameraMatrix();
	osg::Matrixd current_matrix;
	current_matrix(0, 0) = INFINITY; // make the matrix nonsense so that it draws at least once
	QImage thumbnail = generateThumbnail();

	for (auto slide : slides) {
		qDebug() << "redrawing thumbnail" << slide->getIndex();
		// if the matrix is the same, then don't bother re-rendering
		osg::Matrixd potential_matrix = slide->getSlide()->getCameraMatrix();
		if (potential_matrix != current_matrix) {
			current_matrix = potential_matrix;
			m_window->m_osg_widget->setCameraMatrix(current_matrix);
			thumbnail = generateThumbnail();
		}

		slide->setImage(thumbnail);
		slide->setThumbnailDirty(false);
	}
}

QImage NarrativeControl::generateThumbnail()
{
	// widget dimensions
	QRect dims = m_window->centralWidget()->geometry(); 

	// screenshot dimensions
	QRect ssdims = Util::rectFit(dims, 16.0 / 9.0);
	ssdims.setY(ssdims.y() + 50);


	QImage img(ssdims.width(), ssdims.height(), QImage::Format_ARGB32);
	QPainter painter(&img);

	m_window->m_osg_widget->render(&painter, QPoint(0, 0), QRegion(ssdims), QWidget::DrawWindowBackground);
	m_window->m_drag_area->render(&painter, QPoint(0, 0), QRegion(ssdims), QWidget::DrawChildren | QWidget::IgnoreMask);

	// optional, fewer big screenshots
	QImage smallimg;
	smallimg = img.scaled(288, 162, Qt::IgnoreAspectRatio);
	return smallimg;
}
