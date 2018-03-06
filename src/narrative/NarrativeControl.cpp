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
#include <unordered_map>

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

#include "SelectionStack.h"
#include "Selection.h"
#include "MainWindowTopBar.h"
#include "VSimApp.h"

#include "mrichtextedit.h"

NarrativeControl::NarrativeControl(VSimApp *app, MainWindow *window, QObject *parent)
	: m_app(app),
	m_window(window), 
	m_current_narrative(-1),
	m_narrative_group(nullptr),
	m_canvas_enabled(true),
	m_editing_enabled(false)
{
	m_narrative_box = window->topBar()->ui.narratives;
	m_slide_box = window->topBar()->ui.slides;
	m_narrative_selection = m_narrative_box->selectionStack();
	m_slide_selection = m_slide_box->selectionStack();
	m_canvas = window->canvas();
	m_fade_canvas = window->fadeCanvas();
	m_undo_stack = m_app->getUndoStack();

	m_fade_in = new QGraphicsOpacityEffect(m_canvas);
	m_fade_out = new QGraphicsOpacityEffect(m_fade_canvas);
	m_canvas->setGraphicsEffect(m_fade_in);
	m_fade_canvas->setGraphicsEffect(m_fade_out);
	m_fade_in_anim = new QPropertyAnimation(m_fade_in, "opacity");
	m_fade_out_anim = new QPropertyAnimation(m_fade_out, "opacity");
	m_fade_in_anim->setDuration(250);
	m_fade_out_anim->setDuration(250);
	showCanvas(false);

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
	// goto
	connect(m_slide_box, &SlideScrollBox::sGoto, this, [this](int index) {
		qDebug() << "goto slide" << index;
		openSlide(index);
	});
	// clear
	//connect(m_slide_box, &HorizontalScrollBox::sSelectionCleared, this, [this]() {
	//	qDebug() << "selection cleared";
	//	openSlide(-1);
	//});
	// transition to
	//connect(m_slide_box, &SlideScrollBox::sTransitionTo, this, 
	//	[this](int index) {
	//	m_app->transitionTo(index);
	//});

	// back
	connect(m_window->topBar()->ui.left_2, &QPushButton::clicked, this, [this]() {setNarrative(-1); });
	//change
	connect(m_slide_selection, &SelectionStack::sChanged, this, &NarrativeControl::onSlideSelectionChange);
	
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
		redrawThumbnails(m_slide_box->getDirtySlides());
	});

	connect(window, &MainWindow::sDebugControl, this, &NarrativeControl::debug);
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
	m_undo_stack->push(new SelectNarrativesCommand(this, { }, ON_UNDO));
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
	int active_item = m_narrative_selection->last();
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
	if (m_narrative_selection->empty()) return;
	auto selection = m_narrative_selection->data();

	// get pointers to nodes to delete
	m_undo_stack->beginMacro("Delete Narratives");
	m_undo_stack->push(new SelectNarrativesCommand(this, selection, ON_UNDO));
	m_undo_stack->push(new Group::RemoveSetCommand(m_narrative_group, m_narrative_selection->toSet()));
	m_undo_stack->push(new SelectNarrativesCommand(this, {}, ON_REDO));
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
	SelectionData sfrom(from.begin(), from.end());
	SelectionData sto(to.begin(), to.end());

	m_undo_stack->beginMacro("Move Narratives");
	m_undo_stack->push(new SelectNarrativesCommand(this, sfrom, ON_UNDO));
	m_undo_stack->push(new Group::MoveNodesCommand(m_narrative_group, mapping));
	m_undo_stack->push(new SelectNarrativesCommand(this, sto, ON_REDO));
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
	SelectionData selection_data(selection.begin(), selection.end());

	std::map<int, osg::Node*> new_nodes;
	for (uint i = 0; i < n; i++) {
		new_nodes[n + i] = group->getChild(i);
	}

	m_undo_stack->beginMacro("Import Narratives");
	m_undo_stack->push(new Group::InsertSetCommand(m_narrative_group, new_nodes));
	m_undo_stack->push(new SelectNarrativesCommand(this, selection_data, ON_REDO));
	m_undo_stack->endMacro();
}

void NarrativeControl::debug()
{
	qDebug() << "Narrative Control Debug";
	qDebug() << "current narrative" << m_current_narrative;
	qDebug() << "current slide" << m_current_slide;
	auto ns = m_slide_selection->data();
	qDebug() << "nar box" << Util::iterToString(ns.begin(), ns.end());
	auto ss = m_slide_selection->data();
	qDebug() << "slide box" << Util::iterToString(ss.begin(), ss.end());
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
	int index = m_narrative_selection->last();
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

bool NarrativeControl::openSlide(int index, bool go, bool fade)
{
	NarrativeSlide *slide = getSlide(m_current_narrative, index);

	if (m_current_slide == index) {
		return true;
	}

	// failed to set slide
	if (!slide) {
		m_current_slide = -1;
		showCanvas(false, fade);
		m_canvas->setSlide(nullptr);
		exitEdit();
		return false;
	}

	m_current_slide = index;
	m_canvas->setSlide(slide);
	showCanvas(true, fade);

	// is this index selected? force selection
	if (!m_slide_selection->has(index)) {
		m_slide_selection->select(index);
	}

	if (go) {
		m_app->setCameraMatrix(slide->getCameraMatrix());
	}

	if (m_editing_slide) {
		showCanvasEditor(true);
	}

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
	return openSlide(next, instant);
}

void NarrativeControl::showCanvas(bool show, bool fade)
{
	m_canvas->setVisible(show);
	m_canvas->setAttribute(Qt::WA_TransparentForMouseEvents, !show);
	if (show) {
		if (fade) {
			m_fade_in_anim->start();
		}
		else {
			m_fade_in_anim->setCurrentTime(m_fade_in_anim->duration());
		}
	}
	else {
		if (fade) {
			m_fade_canvas->show();
			m_fade_canvas->setSlide(m_canvas->getSlide());
			m_fade_out_anim->start();
		}
	}
}

void NarrativeControl::showCanvasEditor(bool show)
{
	m_label_buttons->setVisible(show);
}

//void NarrativeControl::hideCanvas(bool instant)
//{
//	m_canvas->hide();
//	exitEdit();
//	qDebug() << "hide canvas";
//}

void NarrativeControl::editSlide() {
	//qDebug() << "EDIT SLIDE CURRENT SLDIE?" << m_current_narrative << m_current_slide << getCurrentSlide();
	//m_window->canvasView()->setAttribute(Qt::WA_TransparentForMouseEvents, false);

	Narrative2 *nar = getCurrentNarrative();
	if (!nar) {
		qWarning() << "failed to edit slide - no narrative";
		return;
	}

	// is a slide open?
	if (m_current_slide < 0) {
		int last = m_slide_selection->last();
		auto sel = m_slide_selection->data();
		Narrative2 *nar = getCurrentNarrative();
		// open last selected
		if (last >= 0) {
			openSlide(last);
		}
		// open last slide in selection
		else if (sel.size() > 0) {
			openSlide(*sel.rbegin());
		}
		// open last slide
		else if (nar->getNumChildren() > 0) {
			openSlide(nar->getNumChildren() - 1);
		}
	}
	if (m_current_slide < 0) {
		qWarning() << "failed to open slide for editing";
		return;
	}

	showCanvasEditor(true);
	m_canvas->setEditable(true);
	m_editing_slide = true;
}

void NarrativeControl::exitEdit() {
	m_label_buttons->hide();
	m_canvas->setEditable(false);
	m_editing_slide = false;
	//m_window->canvasView()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

NarrativeControl::SelectionLevel NarrativeControl::getSelectionLevel()
{
	if (m_current_narrative == -1) return NARRATIVES;
	if (m_current_slide == -1) return SLIDES;
	return LABELS;
}

void NarrativeControl::selectNarratives(const SelectionData &narratives)
{
	setNarrative(-1);
	m_narrative_selection->set(narratives);
	emit sEditEvent();
}

void NarrativeControl::selectSlides(int narrative, const SelectionData &slides)
{
	qDebug() << "select slides";
	setNarrative(narrative);
	m_narrative_selection->set({narrative});
	m_slide_selection->set(slides);
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
	m_narrative_selection->set({ narrative });
	//m_slide_selection->set({ slide });
	openSlide(slide);
	m_canvas->setSelection(labels);
}

int NarrativeControl::getCurrentNarrativeIndex()
{
	return m_current_narrative;
}

std::vector<Narrative2*> NarrativeControl::getSelectedNarratives() const
{
	std::vector<Narrative2*> nars;
	auto sel = m_narrative_selection->toSet();
	for (auto i : sel) {
		Narrative2 *nar = dynamic_cast<Narrative2*>(m_narrative_group->child(i));
		nars.push_back(nar);
	}
	return nars;
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

void NarrativeControl::onSlideSelectionChange()
{
	// if (edit mode)
	openSlide(m_slide_selection->last());
}

void NarrativeControl::newSlide()
{
	Narrative2 *nar = getNarrative(m_current_narrative);
	auto matrix = m_window->m_osg_widget->getCameraMatrix();

	// figure out where to insert
	int index;
	if (m_slide_selection->empty()) {
		index = nar->getNumChildren();
	}
	else {
		index = m_slide_selection->last() + 1;
	}

	// make new slide, initialize matrix and stuff
	NarrativeSlide *slide = new NarrativeSlide;
	slide->setCameraMatrix(matrix);

	// perform command
	m_undo_stack->beginMacro("New Slide");
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, {}, ON_UNDO));
	m_undo_stack->push(new Group::AddNodeCommand(nar, slide, index));
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, { index }, ON_REDO));
	m_undo_stack->endMacro();

	//std::cout << m_window->getViewer()->getCameraManipulator()->getMatrix();

	openSlide(index);
}

void NarrativeControl::deleteSlides()
{
	if (m_narrative_selection->empty()) {
		return;
	}
	Narrative2 *nar = getNarrative(m_current_narrative);

	m_undo_stack->beginMacro("Delete Slides");
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, m_slide_selection->data(), ON_UNDO));
	m_undo_stack->push(new Group::RemoveSetCommand(nar, m_slide_selection->toSet()));
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, {}, ON_REDO));
	m_undo_stack->endMacro();
}

void NarrativeControl::setSlideDuration()
{
	qInfo() << "set slide duration";
	auto selection = m_slide_selection->data();
	if (selection.empty()) return;

	NarrativeSlide *first_slide = getSlide(m_current_narrative, m_slide_selection->last());
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
	qInfo() << "set slide transition duration";
	auto selection = m_slide_selection->data();
	if (selection.empty()) return;

	// intial values for the dialog by looking up the first selection
	int slide_index = *selection.begin();
	NarrativeSlide *first_slide = getSlide(m_current_narrative, m_slide_selection->last());
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
	auto selection = m_slide_selection->data();
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
	qDebug() << "move slides";
	std::unordered_map<int, int> umap;

	std::set<int> from;
	std::set<int> to;
	for (auto &pair : mapping) {
		from.insert(pair.first);
		to.insert(pair.second);
		umap[pair.first] = pair.second;
	}

	SelectionData sfrom = m_slide_selection->data();
	SelectionData sto = sfrom;

	// remap the to
	for (size_t i = 0; i < sto.size(); i++) {
		sto[i] = umap[sto[i]];
	}

	// is from actually equivalent to selection?
	std::set<int> sel_set(sfrom.begin(), sfrom.end());
	if (sel_set != from) {
		qWarning() << "move slides selection != from";
		return;
	}

	Narrative2 *narrative = getCurrentNarrative();
	m_undo_stack->beginMacro("Move Slides");
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, sfrom, ON_UNDO));
	m_undo_stack->push(new Group::MoveNodesCommand(narrative, mapping));
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, sto, ON_REDO));
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
	//LabelStyle *label_style = nar->getLabelStyles()->getStyle((LabelStyle::Style)style);
	//if (label_style) label->applyStyle(label_style);

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

	qInfo() << "Label edited command";

	m_undo_stack->beginMacro("Edit Label");
	m_undo_stack->push(new NarrativeSlideLabel::DocumentEditWrapperCommand(doc));
	m_undo_stack->push(new SelectLabelsCommand(this, m_current_narrative, m_current_slide, { label }, ON_BOTH));
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
	NarrativeSlide *slide = getCurrentSlide();
	if (slide) slide->dirtyThumbnail();
}

void NarrativeControl::redrawThumbnails(const std::vector<NarrativeSlide*> slides)
{
	//qDebug() << "redraw thumbnails";
	//for (auto slide : slides) {
	//	qDebug() << "redrawing thumbnail" << slide;
	//	QImage thumbnail;

	//	thumbnail = generateThumbnail(slide);
	//	
	//	slide->setThumbnail(thumbnail);
	//	//item->setImage(thumbnail);
	//	//item->setThumbnailDirty(false);
	//}
}

QImage NarrativeControl::generateThumbnail(NarrativeSlide *slide)
{
	//qDebug() << "generate thumbnail" << slide;
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


SelectNarrativesCommand::SelectNarrativesCommand(NarrativeControl *control, const SelectionData &narratives, SelectionCommandWhen when, QUndoCommand *parent)
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

SelectSlidesCommand::SelectSlidesCommand(NarrativeControl *control, int narrative, const SelectionData &slides, SelectionCommandWhen when, QUndoCommand *parent)
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
