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
	// delete
	connect(m_canvas, SIGNAL(sDeleteLabel(int)), this, SLOT(deleteLabel(int)));

	editDlg = new editButtons(m_window);
	editDlg->move(10, 200);
	editDlg->hide();

	QSignalMapper* signalMapper = new QSignalMapper(this);

	connect(editDlg->ui.done, &QPushButton::clicked, this, &NarrativeControl::exitEdit);
	connect(editDlg->ui.delete_2, &QPushButton::clicked, this, &NarrativeControl::deleteLabelButton);
	connect(editDlg->ui.label, SIGNAL(clicked()), signalMapper, SLOT(map()));
	connect(editDlg->ui.head1, SIGNAL(clicked()), signalMapper, SLOT(map()));
	connect(editDlg->ui.head2, SIGNAL(clicked()), signalMapper, SLOT(map()));
	connect(editDlg->ui.body, SIGNAL(clicked()), signalMapper, SLOT(map()));

	signalMapper->setMapping(editDlg->ui.label, QString("color:rgb(0, 0, 0); background: rgba(255, 255, 255, 70); font-size: 12pt; font-family: \"Arial\"; font-weight: bold; text-align:center;"));
	signalMapper->setMapping(editDlg->ui.head1, QString("color:rgb(240, 240, 240); background-color:rgba(0, 0, 0, 70); font-size: 36pt; font-family: \"Arial\"; font-weight: bold; text-align:center;"));
	signalMapper->setMapping(editDlg->ui.head2, QString("color:rgb(224, 147, 31); background-color:rgba(0, 0, 0, 70); font-size: 20pt; font-family: \"Arial\"; font-weight: bold; text-align:center;"));
	signalMapper->setMapping(editDlg->ui.body, QString("color:rgb(240, 240, 240); background-color:rgba(0, 0, 0, 70); font-size: 12pt; font-family: \"Arial\"; font-weight: regular; text-align:left;"));
	connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(newLabelButton(QString)));


	// dirty slide thumbnails
	connect(m_slide_box, &SlideScrollBox::sThumbnailsDirty, this, 
		[this]() {redrawThumbnails(m_slide_box->getDirtySlides()); }
		);

	connect(window->ui.actionControl_Debug, &QAction::triggered, this, &NarrativeControl::debug);
}

NarrativeControl::~NarrativeControl()
{
}

void NarrativeControl::newNarrative()
{
	// open up the dialog
	NarrativeInfoDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	// for now just add to the end
	m_undo_stack->beginMacro("New Narrative");
	int num_children = m_narrative_group->getNumChildren();
	m_undo_stack->push(new SelectNarrativesCommand(this, { num_children - 1 }, ON_UNDO));
	m_undo_stack->push(new NarrativeGroup::NewNarrativeCommand(m_narrative_group, m_narrative_group->getNumChildren()));
	m_undo_stack->push(new SelectNarrativesCommand(this, { num_children }, ON_REDO));
	m_undo_stack->endMacro();

	Narrative2 *narrative = getNarrative(m_narrative_group->getNumChildren() - 1);
	narrative->setTitle(dlg.getTitle());
	narrative->setAuthor(dlg.getAuthor());
	narrative->setDescription(dlg.getDescription());
}

void NarrativeControl::editNarrativeInfo()
{
	int active_item = m_narrative_box->getLastSelected();
	qDebug() << "narrative list - begin edit on" << active_item;
	if (active_item < 0) {
		qWarning() << "narrative list - can't edit with no selection";
		return;
	}

	Narrative2 *narrative = getNarrative(active_item);

	NarrativeInfoDialog dlg(narrative);
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		qDebug() << "narrative list - cancelled edit on" << active_item;
		return;
	}

	m_undo_stack->beginMacro("Set Narrative Info");
	m_undo_stack->push(new SelectNarrativesCommand(this, { active_item }));
	m_undo_stack->push(new Narrative2::SetTitleCommand(narrative, dlg.getTitle()));
	m_undo_stack->push(new Narrative2::SetAuthorCommand(narrative, dlg.getAuthor()));
	m_undo_stack->push(new Narrative2::SetDescriptionCommand(narrative, dlg.getDescription()));
	m_undo_stack->endMacro();
}

void NarrativeControl::deleteNarratives()
{
	std::set<int> selection = m_narrative_box->getSelection();
	if (selection.empty()) return;
	int next_selection = nextSelectionAfterDelete(m_narrative_group->getNumChildren(), selection);

	// get pointers to nodes to delete
	m_undo_stack->beginMacro("Delete Narratives");
	m_undo_stack->push(new SelectNarrativesCommand(this, selection, ON_UNDO));
	// delete in reverse order
	for (auto i = selection.rbegin(); i != selection.rend(); ++i) {
		qDebug() << "pushing delete narrative" << *i;
		m_undo_stack->push(new NarrativeGroup::DeleteNarrativeCommand(m_narrative_group, *i));
	}
	m_undo_stack->push(new SelectNarrativesCommand(this, {next_selection}, ON_REDO));
	m_undo_stack->endMacro();
}

void NarrativeControl::deleteLabel(int idx)
{
	m_current_slide = m_slide_box->getLastSelected();
	if (m_current_slide < 0) return;

	NarrativeSlide* curSl = getNarrativeSlide(m_current_narrative, m_current_slide);
	NarrativeSlideLabels* data = dynamic_cast<NarrativeSlideLabels*>(curSl->getChild(idx));

	curSl->removeChild(data);

	SlideScrollItem *item = m_slide_box->getItem(m_current_slide);
	item->setThumbnailDirty(true);
}

void NarrativeControl::debug()
{
	qDebug() << "Narrative Control Debug";
	qDebug() << "current narrative" << m_current_narrative;
	qDebug() << "current slide" << m_current_slide;
}

void NarrativeControl::load(NarrativeGroup *narratives)
{
	m_narrative_box->clear();
	m_slide_box->clear(); 
	m_current_narrative = -1;
	m_current_slide = -1;
	m_narrative_group = narratives;
	closeNarrative();

	m_narrative_box->setGroup(narratives);
}

void NarrativeControl::openNarrative()
{
	int index = m_narrative_box->getLastSelected();
	if (index < 0) return;

	setNarrative(index);
}

void NarrativeControl::setNarrative(int index)
{
	qDebug() << "open narrative at" << index;
	this->m_window->ui.topBar->showSlides();
	m_current_narrative = index;

	Narrative2 *nar = getNarrative(index);
	this->m_window->ui.topBar->setSlidesHeader(nar->getTitle());
	m_slide_box->setGroup(nar);
	this->exitEdit();

	if (nar->getNumChildren() > 0) {
		m_slide_box->setLastSelected(0);
		openSlide();
	}
}

void NarrativeControl::closeNarrative()
{
	qDebug() << "close narrative";
	m_current_narrative = -1;
	m_current_slide = -1;
	this->m_window->ui.topBar->showNarratives();
	m_canvas->clearCanvas();
}

void NarrativeControl::openSlide() 
{
	m_current_slide = m_slide_box->getLastSelected();
	if (m_current_slide < 0) return;

	NarrativeSlide* curSl = getNarrativeSlide(m_current_narrative, m_current_slide);
	m_canvas->clearCanvas();
	NarrativeSlideLabels* data;

	for (uint i = 0; i < curSl->getNumChildren(); i++) {
		data = dynamic_cast<NarrativeSlideLabels*>(curSl->getChild(i));
		m_canvas->newLabel(data->getStyle(), data->getText(), data->getrX(), data->getrY(), data->getrW(), 
			data->getrH());
	}
}

void NarrativeControl::deleteLabelButton() {
	m_canvas->deleteLabel();
}

void NarrativeControl::exitEdit() {
	editDlg->hide();
	m_window->m_view->setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

void NarrativeControl::newLabelButton(QString style) {
	m_canvas->newLabel(style);
}

void NarrativeControl::selectNarratives(std::set<int> narratives)
{
	closeNarrative();
	m_narrative_box->setSelection(narratives);
}

void NarrativeControl::selectSlides(int narrative, std::set<int> slides)
{
	if (m_current_narrative != narrative) {
		setNarrative(narrative);
	}
	m_narrative_box->setSelection({narrative});
	m_slide_box->setSelection({slides});
}

void NarrativeControl::newLabel(std::string str, int idx) {
	NarrativeSlideLabels* lab = new NarrativeSlideLabels();
	dragLabel* temp = m_canvas->m_items.at(idx);
	lab->setrX(temp->ratioX);
	lab->setrY(temp->ratioY);
	lab->setrW(temp->ratioWidth);
	lab->setrH(temp->ratioHeight);
	lab->setText(temp->toHtml().toStdString());
	lab->setStyle(str);

	m_current_slide = m_slide_box->getLastSelected();
	if (m_current_slide < 0) return;

	NarrativeSlide* curSl = getNarrativeSlide(m_current_narrative, m_current_slide);
	curSl->addChild(lab);

	//m_canvas->exitEdit();
	QImage new_thumbnail = generateThumbnail();
	//m_canvas->editCanvas();

	SlideScrollItem *item = m_slide_box->getItem(m_current_slide);
	item->setThumbnailDirty(true);
}

void NarrativeControl::moveLabel(QPoint pos, int idx) {
	NarrativeSlide* curSl = getNarrativeSlide(m_current_narrative, m_current_slide);
	NarrativeSlideLabels* lab = dynamic_cast<NarrativeSlideLabels*>(curSl->getChild(idx));
	dragLabel* temp = m_canvas->m_items.at(idx);
	lab->setrX(temp->ratioX);
	lab->setrY(temp->ratioY);

	SlideScrollItem *item = m_slide_box->getItem(m_current_slide);
	item->setThumbnailDirty(true);
}

void NarrativeControl::resizeLabel(QSize size, int idx) {
	NarrativeSlide* curSl = getNarrativeSlide(m_current_narrative, m_current_slide);
	NarrativeSlideLabels* lab = dynamic_cast<NarrativeSlideLabels*>(curSl->getChild(idx));
	dragLabel* temp = m_canvas->m_items.at(idx);
	lab->setrH(temp->ratioHeight);
	lab->setrW(temp->ratioWidth);

	SlideScrollItem *item = m_slide_box->getItem(m_current_slide);
	item->setThumbnailDirty(true);
}

void NarrativeControl::textEditLabel(QString str, int idx) {
	NarrativeSlide* curSl = getNarrativeSlide(m_current_narrative, m_current_slide);
	NarrativeSlideLabels* lab = dynamic_cast<NarrativeSlideLabels*>(curSl->getChild(idx));
	lab->setText(str.toStdString());
}

int NarrativeControl::nextSelectionAfterDelete(int total, std::set<int> selection)
{
	// figure out the selection after deleting
	int first_index = *selection.begin();
	int remaining = total - selection.size();
	int next_selection;
	if (remaining == 0) { // everyone's gone
		next_selection = -1;
	}
	else if (remaining >= first_index + 1) {
		next_selection = first_index; // select next non-deleted item
	}
	else {
		next_selection = first_index - 1; // select the previous item
	}
	return next_selection;
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
	Narrative2 *nar = getNarrative(m_current_narrative);
	auto matrix = m_window->m_osg_widget->getCameraMatrix();

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
	qDebug() << "before push";

	// figure out what to select if we were to undo
	int undo_selection;
	if (nar->getNumChildren() == 0) undo_selection = -1;
	else if (index == 0) undo_selection = index;
	else undo_selection = index - 1;

	// perform command
	m_undo_stack->beginMacro("New Slide");
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, { undo_selection }, ON_UNDO));
	m_undo_stack->push(new Narrative2::NewSlideCommand(nar, index));
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, { index }, ON_REDO));
	m_undo_stack->endMacro();

	qDebug() << "after push";

	// initialization
	NarrativeSlide *slide = getNarrativeSlide(m_current_narrative, index);
	slide->setCameraMatrix(matrix);

	openSlide();
}

void NarrativeControl::deleteSlides()
{
	qDebug() << "delete slides";
	std::set<int> selection = m_slide_box->getSelection();
	Narrative2 *nar = getNarrative(m_current_narrative);

	int next_selection = nextSelectionAfterDelete(nar->getNumChildren(), selection);
	
	m_undo_stack->beginMacro("Delete Narratives");
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, selection, ON_UNDO));
	// we need to delete in reverse order to get the indices right
	for (auto i = selection.rbegin(); i != selection.rend(); ++i) {
		qDebug() << "pushing delete" << *i;
		m_undo_stack->push(new Narrative2::DeleteSlideCommand(nar, *i));
	}
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, {next_selection}, ON_REDO));
	m_undo_stack->endMacro();
}

void NarrativeControl::editSlide() {
	m_window->m_view->setAttribute(Qt::WA_TransparentForMouseEvents, false);
	editDlg->show();
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
			m_undo_stack->push(new NarrativeSlide::SetStayOnNodeCommand(slide, true));
		}
		else {
			m_undo_stack->push(new NarrativeSlide::SetDurationCommand(slide, duration));
		}
	}
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, selection));
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

	m_undo_stack->beginMacro("Set Transition Duration");
	for (auto index : selection) {
		NarrativeSlide *slide = getNarrativeSlide(m_current_narrative, index);
		m_undo_stack->push(new NarrativeSlide::SetTransitionDurationCommand(slide, duration));
	}
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, selection));
	m_undo_stack->endMacro();
}

void NarrativeControl::setSlideCamera()
{
	std::set<int> selection = m_slide_box->getSelection();
	osg::Matrixd matrix = m_window->m_osg_widget->getCameraMatrix();

	m_undo_stack->beginMacro("Set Transition Duration");
	for (auto index : selection) {
		NarrativeSlide *slide = getNarrativeSlide(m_current_narrative, index);
		m_undo_stack->push(new NarrativeSlide::SetCameraMatrixCommand(slide, matrix));
	}
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, selection));
	m_undo_stack->endMacro();
}

void NarrativeControl::redrawThumbnails(const std::vector<SlideScrollItem*> slides)
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

QImage NarrativeControl::generateThumbnail(int option)
{
	// widget dimensions
	QRect dims = m_window->centralWidget()->geometry(); 

	// screenshot dimensions
	QRect ssdims = Util::rectFit(dims, 16.0 / 9.0);
	ssdims.setY(ssdims.y() + 50);


	QImage img(ssdims.width(), ssdims.height(), QImage::Format_ARGB32);
	QPainter painter(&img);

	if (option == 1) {
		m_window->m_osg_widget->render(&painter, QPoint(0, 0), QRegion(ssdims), QWidget::DrawWindowBackground);
		m_window->m_drag_area->render(&painter, QPoint(0, 0), QRegion(ssdims), QWidget::DrawChildren | QWidget::IgnoreMask);
	}
	else if (option == 2) {
		m_window->m_osg_widget->render(&painter, QPoint(0, 0), QRegion(ssdims), QWidget::DrawWindowBackground);
	}

	// optional, fewer big screenshots
	QImage smallimg;
	smallimg = img.scaled(288, 162, Qt::IgnoreAspectRatio);
	return smallimg;
}


SelectNarrativesCommand::SelectNarrativesCommand(NarrativeControl *control, std::set<int> narratives, SelectionCommandWhen when, QUndoCommand *parent)
	: QUndoCommand(parent),
	m_control(control),
	m_narratives(narratives),
	m_when(when)
{
}
void SelectNarrativesCommand::undo() {
	if (m_when != ON_REDO) {
		m_control->selectNarratives(m_narratives);
	}
}
void SelectNarrativesCommand::redo() {
	if (m_when != ON_UNDO) {
		m_control->selectNarratives(m_narratives);
	}
}

SelectSlidesCommand::SelectSlidesCommand(NarrativeControl *control, int narrative, std::set<int> slides, SelectionCommandWhen when, QUndoCommand *parent)
	: QUndoCommand(parent),
	m_control(control),
	m_narrative(narrative),
	m_slides(slides),
	m_when(when)
{
}
void SelectSlidesCommand::undo() {
	if (m_when != ON_REDO) {
		m_control->selectSlides(m_narrative, m_slides);
	}
}
void SelectSlidesCommand::redo() {
	if (m_when != ON_UNDO) {
		m_control->selectSlides(m_narrative, m_slides);
	}
}
