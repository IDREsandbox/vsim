#include "narrative/NarrativeControl.h"
#include "Util.h"

#include <QObject>
#include <QDebug>
#include <QAction>
#include <QList>
#include <osg/Matrix>
#include <osg/io_utils>
#include <QUndoStack>
#include <QTextFrame>

#include <QElapsedTimer>

#include "MainWindow.h"
#include "OSGViewerWidget.h"

#include "NarrativeInfoDialog.h"
#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative2.h"
#include "narrative/NarrativeScrollBox.h"
#include "narrative/SlideScrollBox.h"
#include "narrative/SlideScrollItem.h"
#include "narrative/NarrativeSlideLabel.h"

//style
#include "StyleSettingsDialog.h"
#include "LabelStyle.h"
#include "LabelStyleGroup.h"

//#include "dragLabel.h"
//#include "labelCanvas.h"
//#include "labelCanvasView.h"
#include "narrative/NarrativeCanvas.h"
#include "editButtons.h"

#include "Selection.h"
#include "MainWindowTopBar.h"
#include "VSimApp.h"

#include "mrichtextedit.h"

NarrativeControl::NarrativeControl(VSimApp *app, MainWindow *window, QObject *parent)
	: m_app(app),
	m_window(window), 
	m_current_narrative(-1),
	m_narrative_group(nullptr)
{
	m_narrative_box = window->topBar()->ui.narratives;
	m_slide_box = window->topBar()->ui.slides;
	m_canvas = window->canvas();
	m_undo_stack = m_app->getUndoStack();

	connect(m_window, &MainWindow::sEditStyleSettings, this, &NarrativeControl::editStyleSettings);

	// NARRATIVE CONTROL
	// new
	connect(m_narrative_box, &NarrativeScrollBox::sNew, this, &NarrativeControl::newNarrative);
	connect(m_window->topBar()->ui.plus, &QPushButton::clicked, this, &NarrativeControl::newNarrative);
	// delete
	connect(m_narrative_box, &NarrativeScrollBox::sDelete, this, &NarrativeControl::deleteNarratives);
	connect(m_window->topBar()->ui.minus, &QPushButton::clicked, this, &NarrativeControl::deleteNarratives);
	// info
	connect(m_narrative_box, &NarrativeScrollBox::sInfo, this, &NarrativeControl::editNarrativeInfo);
	connect(m_window->topBar()->ui.info, &QPushButton::clicked, this, &NarrativeControl::editNarrativeInfo);
	// open
	connect(m_narrative_box, &NarrativeScrollBox::sOpen, this, &NarrativeControl::openNarrative);
	connect(m_window->topBar()->ui.open, &QPushButton::clicked, this, &NarrativeControl::openNarrative);

	connect(m_narrative_box, &NarrativeScrollBox::sMove, this, &NarrativeControl::moveNarratives);

	// SLIDE CONTROL
	// new
	connect(m_slide_box, &SlideScrollBox::sNewSlide, this, &NarrativeControl::newSlide);
	connect(m_window->topBar()->ui.plus_2, &QPushButton::clicked, this, &NarrativeControl::newSlide);
	// delete
	connect(m_slide_box, &SlideScrollBox::sDeleteSlides, this, &NarrativeControl::deleteSlides);
	connect(m_window->topBar()->ui.minus_2, &QPushButton::clicked, this, &NarrativeControl::deleteSlides);
	// edit
	connect(m_slide_box, &SlideScrollBox::sEditSlide, this, &NarrativeControl::editSlide);
	connect(m_window->topBar()->ui.open_2, &QPushButton::clicked, this, &NarrativeControl::editSlide);
	// duration
	connect(m_slide_box, &SlideScrollBox::sSetDuration, this, &NarrativeControl::setSlideDuration);
	// transition
	connect(m_slide_box, &SlideScrollBox::sSetTransitionDuration, this, &NarrativeControl::setSlideTransition);
	// camera
	connect(m_slide_box, &SlideScrollBox::sSetCamera, this, &NarrativeControl::setSlideCamera);
	// move
	connect(m_slide_box, &SlideScrollBox::sMove, this, &NarrativeControl::moveSlides);

	// back
	connect(m_window->topBar()->ui.left_2, &QPushButton::clicked, this, [this]() {setNarrative(-1); });
	//change
	connect(m_slide_box, &HorizontalScrollBox::sSelectionChange, this, &NarrativeControl::onSlideSelection);
	
	//CANVAS CONTROL
	// new
	//connect(m_canvas, &labelCanvas::sNewLabel, this, &NarrativeControl::newLabel); // doesn't exist, see edit buttons
	// move
	connect(m_canvas, &NarrativeCanvas::sItemsTransformed, this, &NarrativeControl::transformLabels);

	// text change
	//connect(m_canvas, SIGNAL(sSuperTextSet(QString, int)), this, SLOT(textEditLabel(QString, int))); // these two are directly connected
	connect(m_canvas, &NarrativeCanvas::sLabelUndoCommandAdded, this, &NarrativeControl::labelEdited);

	// delete
	//connect(m_canvas, &NarrativeCanvas::sDeleteLabel, this, &NarrativeControl::deleteLabel);

	m_label_buttons = m_window->labelButtons();
	m_label_buttons->move(10, 200);
	m_label_buttons->hide();

	connect(m_label_buttons->ui.head1, &QPushButton::clicked, this, [this]() { newLabel(LabelStyle::HEADER1); });
	connect(m_label_buttons->ui.head2, &QPushButton::clicked, this, [this]() { newLabel(LabelStyle::HEADER2); });
	connect(m_label_buttons->ui.body, &QPushButton::clicked, this, [this]() { newLabel(LabelStyle::BODY); });
	connect(m_label_buttons->ui.label, &QPushButton::clicked, this, [this]() { newLabel(LabelStyle::LABEL); });

	// edit
	connect(m_label_buttons->ui.edit, &QAbstractButton::pressed, this, &NarrativeControl::execEditLabel);

	//connect(m_label_buttons->ui.head1, &QPushButton::clicked, this, &NarrativeControl::newH1);
	//connect(m_label_buttons->ui.head2, &QPushButton::clicked, this, &NarrativeControl::newH2);
	//connect(m_label_buttons->ui.body, &QPushButton::clicked, this, &NarrativeControl::newBod);
	//connect(m_label_buttons->ui.image, &QPushButton::clicked, this, &NarrativeControl::newImg);

	connect(m_label_buttons->ui.delete_2, &QPushButton::clicked, this, &NarrativeControl::deleteLabels);
	connect(m_label_buttons->ui.done, &QPushButton::clicked, this, &NarrativeControl::exitEdit);

	// dirty slide thumbnails
	connect(m_slide_box, &SlideScrollBox::sThumbnailsDirty, this, 
		[this]() {
		qDebug() << "received dirty thumbnails from box";
		redrawThumbnails(m_slide_box->getDirtySlides());
	});

	connect(window, &MainWindow::sDebugControl, this, &NarrativeControl::debug);
}

NarrativeControl::~NarrativeControl()
{
}

void NarrativeControl::editStyleSettings()
{
	Narrative2 *narrative = getCurrentNarrative();
	if (!narrative) return;

	StyleSettingsDialog dlg;
	dlg.setStyles(narrative->getLabelStyles());
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}
}

void NarrativeControl::newNarrative()
{
	// open up the dialog
	NarrativeInfoDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	Narrative2 *nar = new Narrative2;

	m_undo_stack->beginMacro("New Narrative");
	int num_children = m_narrative_group->getNumChildren();
	m_undo_stack->push(new SelectNarrativesCommand(this, { num_children - 1 }, ON_UNDO));
	m_undo_stack->push(new Group::AddNodeCommand(m_narrative_group, nar));
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
	int last = m_narrative_box->getLastSelected();

	std::set<int> next_selection = Selection::nextSelectionAfterDelete(
		m_narrative_group->getNumChildren(), selection);

	// get pointers to nodes to delete
	m_undo_stack->beginMacro("Delete Narratives");
	m_undo_stack->push(new SelectNarrativesCommand(this, selection, ON_UNDO));
	m_undo_stack->push(new Group::RemoveSetCommand(m_narrative_group, selection));
	m_undo_stack->push(new SelectNarrativesCommand(this, next_selection, ON_REDO));
	m_undo_stack->endMacro();
}

void NarrativeControl::moveNarratives(const std::vector<std::pair<size_t, size_t>> &mapping)
{
	std::set<int> from;
	std::set<int> to;
	for (auto &pair : mapping) {
		from.insert(pair.first);
		to.insert(pair.second);
	}

	m_undo_stack->beginMacro("Move Narratives");
	m_undo_stack->push(new SelectNarrativesCommand(this, from, ON_UNDO));
	m_undo_stack->push(new Group::MoveNodesCommand(m_narrative_group, mapping));
	m_undo_stack->push(new SelectNarrativesCommand(this, to, ON_REDO));
	m_undo_stack->endMacro();
}

void NarrativeControl::loadNarratives(NarrativeGroup * group)
{
	// figure out selection
	uint n = group->getNumChildren();
	int selection_begin = n;
	std::set<int> selection;
	for (uint i = 0; i < n; i++) {
		selection.insert(selection_begin + i);
	}

	std::map<int, osg::Node*> new_nodes;
	for (uint i = 0; i < n; i++) {
		new_nodes[n + i] = group->getChild(i);
	}

	m_undo_stack->beginMacro("Import Narratives");
	m_undo_stack->push(new Group::InsertSetCommand(m_narrative_group, new_nodes));
	m_undo_stack->push(new SelectNarrativesCommand(this, selection, ON_REDO));
	m_undo_stack->endMacro();
}

void NarrativeControl::debug()
{
	qDebug() << "Narrative Control Debug";
	qDebug() << "current narrative" << m_current_narrative;
	qDebug() << "current slide" << m_current_slide;
	qDebug() << "nar box" << Util::setToString(m_narrative_box->getSelection());
	qDebug() << "slide box" << Util::setToString(m_slide_box->getSelection());
}

void NarrativeControl::load(NarrativeGroup *narratives)
{
	qDebug() << "Clearing narrative control";
	m_narrative_box->clear();
	m_slide_box->clear(); 
	m_narrative_group = narratives;
	setNarrative(-1);

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
	if (index == m_current_narrative) return;
	if (index < 0) {
		m_current_narrative = -1;
		m_current_slide = -1;
		this->m_window->topBar()->showNarratives();
		m_canvas->setSlide(nullptr);
		m_canvas->hide();
		emit sEditEvent();
		return;
	}

	qDebug() << "open narrative at" << index;
	this->m_window->topBar()->showSlides();
	m_current_narrative = index;

	Narrative2 *nar = getNarrative(index);
	this->m_window->topBar()->setSlidesHeader(nar->getTitle());
	m_slide_box->setGroup(nar);
	this->exitEdit();

	emit sEditEvent();
}

bool NarrativeControl::setSlide(int index, bool instant)
{
	if (m_current_slide == index) return true;
	qDebug() << "Narrative Control - set slide" << index;
	if (m_current_narrative < 0) {
		m_current_slide = -1;
		return false;
	}
	NarrativeSlide *slide = getSlide(m_current_narrative, index);
	if (!slide) {
		m_canvas->hide();
		m_canvas->setSlide(nullptr);
		m_current_slide = -1;
		return false;
	}

	// is this index selected? force selection
	std::set<int> sel = m_slide_box->getSelection();
	if (sel.find(index) == sel.end()) {
		m_slide_box->setSelection({index}, index);
	}

	m_current_slide = index;
	m_canvas->setSlide(slide);

	//emit sEditEvent();
	return true;
}

//bool NarrativeControl::advance(bool forward)
//{
//	SelectionLevel level = getSelectionLevel();
//	int i;
//	int next;
//	if (level == NARRATIVES) {
//		i = m_narrative_box->getLastSelected();
//		if (i < 0) next = 0;
//		else {
//			if (forward) next = i + 1;
//			else next = i - 1;
//		}
//
//		if (next < 0 || next >= m_narrative_group->getNumChildren()) return false;
//		selectNarratives({next});
//	}
//	else if (level == SLIDES) {
//		i = m_current_slide;
//		if (i < 0) next = 0;
//		else {
//			if (forward) next = i + 1;
//			else next = i - 1;
//		}
//
//		if (next < 0 || next >= getCurrentNarrative()->getNumChildren()) return false;
//		selectSlides(getCurrentNarrativeIndex(), {next});
//	}
//	else {
//		qDebug() << "can't advance when editing slide";
//	}
//}

bool NarrativeControl::advanceSlide(bool forward, bool instant)
{
	int ni = getCurrentNarrativeIndex();
	if (ni < 0) return false;

	int i = m_current_slide;
	int next;
	if (i < 0) next = 0;
	else {
		if (forward) next = i + 1;
		else next = i - 1;
	}

	if (next < 0 || next >= (int)getCurrentNarrative()->getNumChildren()) return false;
	return setSlide(next, instant);
}

void NarrativeControl::showCanvas(bool instant)
{
	m_canvas->show();
	qDebug() << "show canvas";
}

void NarrativeControl::hideCanvas(bool instant)
{
	m_canvas->hide();
	exitEdit();
	qDebug() << "hide canvas";
}

void NarrativeControl::editSlide() {
	//qDebug() << "EDIT SLIDE CURRENT SLDIE?" << m_current_narrative << m_current_slide << getCurrentSlide();
	//m_window->canvasView()->setAttribute(Qt::WA_TransparentForMouseEvents, false);
	m_label_buttons->show();
	m_canvas->setEditable(true);

}

void NarrativeControl::exitEdit() {
	m_label_buttons->hide();
	m_canvas->setEditable(false);
	//m_window->canvasView()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

NarrativeControl::SelectionLevel NarrativeControl::getSelectionLevel()
{
	if (m_current_narrative == -1) return NARRATIVES;
	if (m_current_slide == -1) return SLIDES;
	return LABELS;
}

void NarrativeControl::selectNarratives(std::set<int> narratives)
{
	setNarrative(-1);
	m_narrative_box->setSelection(narratives, *narratives.begin());
	emit sEditEvent();
}

void NarrativeControl::selectSlides(int narrative, std::set<int> slides)
{
	setNarrative(narrative);
	m_narrative_box->setSelection({narrative}, narrative);
	m_slide_box->setSelection({slides}, *slides.begin());
	emit sEditEvent();
}

//void NarrativeControl::selectLabel(int narrative, int slide, int label)
//{
//	setNarrative(narrative);
//	m_narrative_box->setSelection({ narrative }, narrative);
//	setSlide(slide);
//	m_narrative_box->setSelection({ slide }, slide);
//
//	m_canvas->setItemFocus();
//}

void NarrativeControl::selectLabels(int narrative, int slide, const std::set<NarrativeSlideItem *> &labels)
{
	setNarrative(narrative);
	m_narrative_box->setSelection({ narrative }, narrative);
	setSlide(slide);
	m_narrative_box->setSelection({ slide }, slide);
	
	m_canvas->setSelection(labels);
}

//const std::string &text, const std::string &style, const std::string &widget_style, float height, float width, int margin

//void NarrativeControl::deleteLabel(int idx)
//{
//	NarrativeSlide *slide = getCurrentSlide();
//	if (slide == nullptr) return;
//
//	// consistency check
//	if (idx < 0 || (uint) idx >= slide->getNumChildren()) {
//		qWarning() << "Delete label out of range" << idx << "/" << slide->getNumChildren();
//	}
//	
//	// items to delete
//	m_
//
//	// push delete command
//	m_undo_stack->beginMacro("Import Narratives");
//	m_undo_stack->push(new SelectLabelsCommand(this, m_current_narrative, m_current_slide, idx, ON_UNDO));
//	m_undo_stack->push(new Group::DeleteNodeCommand(slide, idx));
//	m_undo_stack->endMacro();
//
//	SlideScrollItem *item = m_slide_box->getItem(m_current_slide);
//	item->setThumbnailDirty(true);
//}

//void NarrativeControl::moveLabel(float rx, float ry, int idx) {
//	NarrativeSlideLabel *label = getLabel(m_current_narrative, m_current_slide, idx);
//	if (!label) {
//		qWarning() << "Narrative Control - move label with invalid index" 
//			<< m_current_narrative << m_current_slide << idx;
//		return;
//	}
//
//	m_undo_stack->beginMacro("Move Label");
//	m_undo_stack->push(new NarrativeSlideLabel::MoveCommand(label, rx, ry));
//	m_undo_stack->push(new SelectLabelCommand(this, m_current_narrative, m_current_slide, idx));
//	m_undo_stack->endMacro();
//
//	SlideScrollItem *item = m_slide_box->getItem(m_current_slide);
//	item->setThumbnailDirty(true);
//}
//
//void NarrativeControl::resizeLabel(float rw, float rh, int idx) {
//	NarrativeSlideLabel *label = getLabel(m_current_narrative, m_current_slide, idx);
//	if (!label) {
//		qWarning() << "Narrative Control - resize label with invalid index"
//			<< m_current_narrative << m_current_slide << idx;
//		return;
//	}
//
//	m_undo_stack->beginMacro("Resize Label");
//	m_undo_stack->push(new NarrativeSlideLabel::ResizeCommand(label, rw, rh));
//	m_undo_stack->push(new SelectLabelCommand(this, m_current_narrative, m_current_slide, idx));
//	m_undo_stack->endMacro();
//
//	SlideScrollItem *item = m_slide_box->getItem(m_current_slide);
//	item->setThumbnailDirty(true);
//}

//void NarrativeControl::textEditLabel(QString str, int idx) {
//	qDebug() << "edit text";
//	NarrativeSlide* curSl = getNarrativeSlide(m_current_narrative, m_current_slide);
//	NarrativeSlideLabel* lab = dynamic_cast<NarrativeSlideLabel*>(curSl->getChild(idx));
//	lab->setText(str.toStdString());
//}

int NarrativeControl::getCurrentNarrativeIndex()
{
	return m_current_narrative;
}

int NarrativeControl::getCurrentSlideIndex()
{
	return m_current_slide;
}

Narrative2 * NarrativeControl::getCurrentNarrative()
{
	if (m_current_narrative < 0) return nullptr;
	return getNarrative(m_current_narrative);
}

NarrativeSlide * NarrativeControl::getCurrentSlide()
{
	if (m_current_narrative < 0) return nullptr;
	return getSlide(m_current_narrative, m_current_slide);
}

Narrative2 *NarrativeControl::getNarrative(int index)
{
	if (index < 0 || (uint)index >= m_narrative_group->getNumChildren()) {
		return nullptr;
	}
	osg::Node *c = m_narrative_group->getChild(index);
	return dynamic_cast<Narrative2*>(c);
}

NarrativeSlide * NarrativeControl::getSlide(int narrative, int slide)
{
	Narrative2 *nar = getNarrative(narrative);
	if (!nar) return nullptr;
	if (slide < 0 || (uint)slide >= nar->getNumChildren()) return nullptr;
	return dynamic_cast<NarrativeSlide*>(nar->getChild(slide));
}

NarrativeSlideLabel * NarrativeControl::getLabel(int narrative, int slide, int label)
{
	NarrativeSlide *s = getSlide(narrative, slide);
	if (!s) return nullptr;
	if (label < 0 || (uint)label >= s->getNumChildren()) return nullptr;
	return dynamic_cast<NarrativeSlideLabel*>(s->getChild(label));
}

void NarrativeControl::onSlideSelection()
{
	if (m_current_narrative < 0) {
		qWarning() << "Narrative Control - slide selection while current narrative null";
		return;
	}
	int s = m_slide_box->getLastSelected();

	if (s == m_current_slide) return;

	setSlide(m_slide_box->getLastSelected());

	emit sEditEvent();
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
	
	// figure out what to select if we were to undo
	std::set<int> undo_selection = Selection::nextSelectionAfterDelete(
		nar->getNumChildren() + 1, { index });

	// make new slide, initialize matrix and stuff
	NarrativeSlide *slide = new NarrativeSlide;
	slide->setCameraMatrix(matrix);

	// perform command
	m_undo_stack->beginMacro("New Slide");
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, undo_selection, ON_UNDO));
	m_undo_stack->push(new Group::AddNodeCommand(nar, slide, index));
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, { index }, ON_REDO));
	m_undo_stack->endMacro();

	//std::cout << m_window->getViewer()->getCameraManipulator()->getMatrix();

	setSlide(index);
}

void NarrativeControl::deleteSlides()
{
	std::set<int> selection = m_slide_box->getSelection();
	Narrative2 *nar = getNarrative(m_current_narrative);

	std::set<int> next_selection = Selection::nextSelectionAfterDelete(
		nar->getNumChildren(), selection);

	m_undo_stack->beginMacro("Delete Slides");
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, selection, ON_UNDO));
	m_undo_stack->push(new Group::RemoveSetCommand(nar, selection));
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, next_selection, ON_REDO));
	m_undo_stack->endMacro();
}

void NarrativeControl::setSlideDuration()
{
	qDebug() << "set slide duration";
	std::set<int> selection = m_slide_box->getSelection();
	if (selection.empty()) return;
	
	// Create the duration dialog, initialize values to the last item selected
	int slide_index = m_slide_box->getLastSelected();
	NarrativeSlide *first_slide = getSlide(m_current_narrative, slide_index);
	float duration = NarrativeSlideDurationDialog::create(first_slide->getStayOnNode(), first_slide->getDuration());
	if (duration < 0) return;

	// perform the command
	m_undo_stack->beginMacro("Set Duration");
	for (auto index : selection) {
		NarrativeSlide *slide = getSlide(m_current_narrative, index);
		if (duration == 0) {
			m_undo_stack->push(new NarrativeSlide::SetStayOnNodeCommand(slide, true));
		}
		else {
			m_undo_stack->push(new NarrativeSlide::SetDurationCommand(slide, duration));
			m_undo_stack->push(new NarrativeSlide::SetStayOnNodeCommand(slide, false));
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
	NarrativeSlide *first_slide = getSlide(m_current_narrative, slide_index);
	float duration = QInputDialog::getDouble(nullptr, "Transition Time", "Transition Time (seconds)", first_slide->getTransitionDuration(), 0.0, 3600.0, 1, nullptr, Qt::WindowSystemMenuHint);

	m_undo_stack->beginMacro("Set Transition Duration");
	for (auto index : selection) {
		NarrativeSlide *slide = getSlide(m_current_narrative, index);
		m_undo_stack->push(new NarrativeSlide::SetTransitionDurationCommand(slide, duration));
	}
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, selection));
	m_undo_stack->endMacro();
}

void NarrativeControl::setSlideCamera()
{
	std::set<int> selection = m_slide_box->getSelection();
	osg::Matrixd matrix = m_window->m_osg_widget->getCameraMatrix();

	m_undo_stack->beginMacro("Set Camera");
	for (auto index : selection) {
		NarrativeSlide *slide = getSlide(m_current_narrative, index);
		m_undo_stack->push(new NarrativeSlide::SetCameraMatrixCommand(slide, matrix));
	}
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, selection));
	m_undo_stack->endMacro();
}

void NarrativeControl::moveSlides(const std::vector<std::pair<size_t, size_t>> &mapping)
{
	std::set<int> from;
	std::set<int> to;
	for (auto &pair : mapping) {
		from.insert(pair.first);
		to.insert(pair.second);
	}

	Narrative2 *narrative = getCurrentNarrative();
	m_undo_stack->beginMacro("Move Slides");
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, from, ON_UNDO));
	m_undo_stack->push(new Group::MoveNodesCommand(narrative, mapping));
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, to, ON_REDO));
	m_undo_stack->endMacro();
}


void NarrativeControl::newLabel(int style) {
	NarrativeSlide *slide = getCurrentSlide();
	if (!slide) {
		qWarning() << "Narrative Control - creating label without slide open";
		return;
	}

	NarrativeSlideLabel *label = new NarrativeSlideLabel;

	// initialization
	//label->setStyle(0);
	label->setHtml("New Label");
	label->setRect(QRectF(-.2, -.2, .4, .2));
	label->setBackground(QColor(0, 0, 0, 100));

	qInfo() << "Creating new canvas label";

	Narrative2 *nar = getCurrentNarrative();
	LabelStyle *label_style = nar->getLabelStyles()->getStyle((LabelStyle::Style)style);
	if (label_style) label->applyStyle(label_style);

	// push command
	m_undo_stack->beginMacro("New Label");
	m_undo_stack->push(new Group::AddNodesPCommand(slide, { label }));
	m_undo_stack->push(new SelectLabelsCommand(this, m_current_narrative, m_current_slide, { label }, ON_REDO));
	m_undo_stack->endMacro();


	dirtyCurrentSlide();
}

void NarrativeControl::deleteLabels() {
	auto items = m_canvas->getSelection();
	// convert slide items to Node*
	std::set<osg::Node*> nodes;
	for (auto item : items) nodes.insert(item);

	NarrativeSlide *slide = getCurrentSlide();
	if (!slide) return;

	qInfo() << "Deleting" << items.size() << "canvas items";

	m_undo_stack->beginMacro("Delete Canvas Items");
	m_undo_stack->push(new SelectLabelsCommand(this, m_current_narrative, m_current_slide, items, ON_UNDO));
	m_undo_stack->push(new Group::RemoveNodesPCommand(slide, nodes));
	m_undo_stack->endMacro();

	dirtyCurrentSlide();
}

void NarrativeControl::transformLabels(const std::map<NarrativeSlideItem *, QRectF>& rects)
{
	// build set of items, for selection
	std::set<NarrativeSlideItem*> items;
	for (auto item_rect : rects) {
		items.insert(item_rect.first);
	}
	qDebug() << "Transforming" << items.size() << "canvas items";

	m_undo_stack->beginMacro("Transform Canvas Items");
	for (auto item_rect : rects) {
		NarrativeSlideItem *item = item_rect.first;
		QRectF rect = item_rect.second;
		m_undo_stack->push(new NarrativeSlideItem::TransformCommand(item, rect));
	}
	m_undo_stack->push(new SelectLabelsCommand(this, m_current_narrative, m_current_slide, items, ON_BOTH));
	m_undo_stack->endMacro();

	dirtyCurrentSlide();
}

void NarrativeControl::labelEdited(NarrativeSlideLabel *label)
{
	// This one is special
	// Since the label and document are directly connected via QTextDocument
	// all we need to know about is when changes happen so that we can undo them

	if (!label) {
		qWarning() << "Narrative Control - edit label with invalid index"
			<< m_current_narrative << m_current_slide << label;
		return;
	}
	QTextDocument *doc = label->getDocument();

	qDebug() << "Label edited command";

	m_undo_stack->beginMacro("Edit Label");
	m_undo_stack->push(new NarrativeSlideLabel::DocumentEditWrapperCommand(doc));
	m_undo_stack->push(new SelectLabelsCommand(this, m_current_narrative, m_current_slide, { label }));
	m_undo_stack->endMacro();

	dirtyCurrentSlide();
}

void NarrativeControl::execEditLabel()
{
	// this button steals away focus, just use the first selected item instead
	// NarrativeSlideItem *item = m_canvas->getItemFocus();
	const auto & sel = m_canvas->getSelection();
	if (sel.empty()) return;

	NarrativeSlideLabel *label = dynamic_cast<NarrativeSlideLabel*>(*sel.begin());
	if (!label) return;

	MRichTextEdit dlg;
	dlg.document()->setHtml(label->getDocument()->toHtml());
	// explicitly set background color
	QTextFrameFormat ff = dlg.document()->rootFrame()->frameFormat();
	ff.setBackground(label->getBackground());
	dlg.document()->rootFrame()->setFrameFormat(ff);

	int result = dlg.exec();
	if (result != QDialog::Accepted) {
		return;
	}

	m_undo_stack->beginMacro("Edit Label");
	// delete everything and insert new html
	QTextCursor cursor(label->getDocument());
	cursor.select(QTextCursor::SelectionType::Document);
	cursor.deleteChar();
	cursor.insertHtml(dlg.toHtml());
	m_undo_stack->push(new SelectLabelsCommand(this, m_current_narrative, m_current_slide, { label }));
	m_undo_stack->endMacro();

	dirtyCurrentSlide();
}

void NarrativeControl::dirtyCurrentSlide()
{
	qDebug() << "dirtying current slide";
	NarrativeSlide *slide = getCurrentSlide();
	if (slide) slide->dirtyThumbnail();
}

void NarrativeControl::redrawThumbnails(const std::vector<NarrativeSlide*> slides)
{
	for (auto slide : slides) {
		qDebug() << "redrawing thumbnail" << slide;
		QImage thumbnail;

		thumbnail = generateThumbnail(slide);
		
		slide->setThumbnail(thumbnail);
		//item->setImage(thumbnail);
		//item->setThumbnailDirty(false);
	}
}

QImage NarrativeControl::generateThumbnail(NarrativeSlide *slide)
{
	QElapsedTimer timer;
	timer.start();

	// set the camera, create a dummy canvas
	osg::Matrixd old_matrix = m_window->getViewerWidget()->getCameraMatrix();
	m_window->getViewerWidget()->setCameraMatrix(slide->getCameraMatrix());

	NarrativeCanvas canvas(m_window->getViewerWidget());
	// canvas.setSlide(slide); TODO: FIXME

	// widget dimensions
	QRect dims = m_window->getViewerWidget()->geometry();

	// screenshot dimensions
	QRect ssdims = Util::rectFit(dims, 16.0 / 9.0);
	//QRect ssdims = Util::rectFit(QRect(0, 0, 300, 300), 16.0 / 9.0);
	//ssdims.setY(ssdims.y() + 50);

	QImage img(ssdims.width(), ssdims.height(), QImage::Format_ARGB32);
	QPainter painter(&img);

	// render
	m_window->m_osg_widget->render(&painter, QPoint(0, 0), QRegion(ssdims), QWidget::DrawWindowBackground);
	//qDebug() << "render canvas size" << canvas.size();
	canvas.QWidget::render(&painter, QPoint(0, 0), QRect(QPoint(0, 0), canvas.size()), QWidget::DrawChildren | QWidget::IgnoreMask);

	// scale down the image
	QImage smallimg;
	smallimg = img.scaled(288, 162, Qt::IgnoreAspectRatio);

	// revert the camera
	m_window->getViewerWidget()->setCameraMatrix(old_matrix);

	int ns = timer.nsecsElapsed();
	//qDebug() << "thumbnail time ms" << ns / 1.0e6;
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

//SelectLabelCommand::SelectLabelCommand(NarrativeControl * control, int narrative, int slide, int label, SelectionCommandWhen when, QUndoCommand * parent)
//	: QUndoCommand(parent),
//	m_control(control),
//	m_narrative(narrative),
//	m_slide(slide),
//	m_label(label),
//	m_when(when)
//{
//}
//void SelectLabelCommand::undo()
//{
//	if (m_when != ON_REDO) {
//		m_control->selectLabel(m_narrative, m_slide, m_label);
//	}
//}
//void SelectLabelCommand::redo()
//{
//	if (m_when != ON_UNDO) {
//		m_control->selectLabel(m_narrative, m_slide, m_label);
//	}
//}

SelectLabelsCommand::SelectLabelsCommand(NarrativeControl * control, int narrative, int slide, std::set<NarrativeSlideItem *> labels, SelectionCommandWhen when, QUndoCommand * parent)
	: QUndoCommand(parent),
	m_control(control),
	m_narrative(narrative),
	m_slide(slide),
	m_labels(labels),
	m_when(when)
{
}
void SelectLabelsCommand::undo()
{
	if (m_when != ON_REDO) {
		m_control->selectLabels(m_narrative, m_slide, m_labels);
	}
}
void SelectLabelsCommand::redo()
{
	if (m_when != ON_UNDO) {
		m_control->selectLabels(m_narrative, m_slide, m_labels);
	}
}
