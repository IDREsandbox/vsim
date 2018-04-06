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
#include "narrative/Narrative.h"
#include "narrative/NarrativeScrollBox.h"
#include "narrative/SlideScrollBox.h"
#include "narrative/SlideScrollItem.h"
#include "narrative/NarrativeSlideLabel.h"
#include "ModelGroup.h"

//style
#include "StyleSettingsDialog.h"
#include "LabelType.h"
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
	m_canvas_enabled(true)
{
	m_bar = window->topBar();
	m_narrative_box = window->topBar()->ui.narratives;
	m_slide_box = window->topBar()->ui.slides;
	m_narrative_selection = m_narrative_box->selectionStack();
	m_slide_selection = m_slide_box->selectionStack();
	m_canvas = window->canvas();
	m_fade_canvas = window->fadeCanvas();
	m_undo_stack = m_app->getUndoStack();

	m_fade_in = new QGraphicsOpacityEffect(m_canvas);
	m_fade_in->setOpacity(1.0);
	m_canvas->setGraphicsEffect(m_fade_in);
	m_fade_in_anim = new QPropertyAnimation(m_fade_in, "opacity");
	m_fade_in_anim->setDuration(250);
	m_fade_in_anim->setStartValue(0.0);
	m_fade_in_anim->setEndValue(1.0);

	m_fade_out = new QGraphicsOpacityEffect(m_fade_canvas);
	m_fade_out->setOpacity(1.0);
	m_fade_canvas->setGraphicsEffect(m_fade_out);
	m_fade_out_anim = new QPropertyAnimation(m_fade_out, "opacity");
	m_fade_out_anim->setDuration(250);
	m_fade_out_anim->setStartValue(1.0);
	m_fade_out_anim->setEndValue(0.0);

	showCanvas(false);

	m_label_buttons = m_window->labelButtons();
	m_label_buttons->move(10, 200);
	m_label_buttons->hide();

	connect(m_window, &MainWindow::sEditStyleSettings, this, &NarrativeControl::editStyleSettings);

	// app state
	connect(m_app, &VSimApp::sStateChanged, this, [this](auto old, auto current) {
		VSimApp::State state = m_app->state();
		if (m_app->isFlying()
			|| state == VSimApp::EDIT_ERS) {
			// fade out
			showCanvas(false, true);
			//openSlide(-1);
		}

		if (state != VSimApp::EDIT_CANVAS
			&& state != VSimApp::EDIT_SLIDES) {
			enableEditing(false);
		}

		if (state == VSimApp::EDIT_CANVAS) {
			// open if possible
			int last = m_slide_selection->last();
			if (last >= 0) {
				openSlide(last);
				// single select slides
				m_slide_selection->set({ last });
			}
		}
		else {
			// deselect canvas
			m_canvas->setSelection({});
		}
	});

	// NARRATIVE CONTROL
	a_new_narrative = new QAction("New Narrative", this);
	a_new_narrative->setIconText("+");
	connect(a_new_narrative, &QAction::triggered, this, &NarrativeControl::newNarrative);

	a_delete_narratives = new QAction("Delete Narrative", this);
	a_delete_narratives->setIconText("-");
	connect(a_delete_narratives, &QAction::triggered, this, &NarrativeControl::deleteNarratives);

	a_narrative_info = new QAction("Edit Narrative Info", this);
	a_narrative_info->setIconText("Info");
	connect(a_narrative_info, &QAction::triggered, this, &NarrativeControl::editNarrativeInfo);

	a_open_narrative = new QAction("Open Narrative", this);
	a_open_narrative->setIconText("Open");
	connect(a_open_narrative, &QAction::triggered, this, &NarrativeControl::showSlideBox);

	// menus
	QMenu *nar_menu = new QMenu("Narrative menu", m_narrative_box);
	QMenu *nar_item_menu = new QMenu("Narrative item menu", m_narrative_box);

	nar_menu->addAction(a_new_narrative);
	nar_item_menu->addAction(a_new_narrative);
	nar_item_menu->addAction(a_delete_narratives);
	nar_item_menu->addAction(a_narrative_info);
	nar_item_menu->addAction(a_open_narrative);

	m_narrative_box->setMenu(nar_menu);
	m_narrative_box->setItemMenu(nar_item_menu);

	// new
	m_bar->ui.add->setDefaultAction(a_new_narrative);
	// delete
	m_bar->ui.remove->setDefaultAction(a_delete_narratives);
	// info
	m_bar->ui.info->setDefaultAction(a_narrative_info);
	// open
	m_bar->ui.open->setDefaultAction(a_open_narrative);
	connect(m_narrative_box, &NarrativeScrollBox::sOpen, a_open_narrative, &QAction::trigger);
	// move
	connect(m_narrative_box, &NarrativeScrollBox::sMove, this, &NarrativeControl::moveNarratives);
	// touch
	connect(m_narrative_box, &HorizontalScrollBox::sTouch, this, [this]() {
		int last = m_narrative_selection->last();
		if (last < 0) {
			m_app->setState(VSimApp::EDIT_FLYING);
			openNarrative(-1);
		}
		else {
			m_app->setState(VSimApp::EDIT_NARS);
			openNarrative(last);
			openSlide(0);
		}
	});

	// SLIDE CONTROL
	a_new_slide = new QAction("New Slide", this);
	a_new_slide->setIconText("+");
	a_delete_slides = new QAction("Delete Slide", this);
	a_delete_slides->setIconText("-");
	a_slide_duration = new QAction("Set Duration", this);
	a_slide_transition = new QAction("Set Transition", this);
	a_slide_camera = new QAction("Set Camera", this);
	a_edit_slide = new QAction("Edit Slide", this);
	a_edit_slide->setIconText("Edit");

	// menus
	QMenu *slide_menu = new QMenu("Narrative menu", m_narrative_box);
	slide_menu->addAction(a_new_slide);

	QMenu *slide_item_menu = new QMenu("Narrative item menu", m_narrative_box);
	slide_item_menu->addAction(a_new_slide);
	slide_item_menu->addAction(a_delete_slides);
	slide_item_menu->addAction(a_slide_duration);
	slide_item_menu->addAction(a_slide_transition);
	slide_item_menu->addAction(a_slide_camera);

	m_slide_box->setMenu(slide_menu);
	m_slide_box->setItemMenu(slide_item_menu);

	// new
	connect(a_new_slide, &QAction::triggered, this, &NarrativeControl::newSlide);
	m_bar->ui.add_2->setDefaultAction(a_new_slide);
	// delete
	connect(a_delete_slides, &QAction::triggered, this, &NarrativeControl::deleteSlides);
	m_bar->ui.remove_2->setDefaultAction(a_delete_slides);
	// edit
	connect(a_edit_slide, &QAction::triggered, this, &NarrativeControl::editSlide);
	m_bar->ui.edit->setDefaultAction(a_edit_slide);
	// move
	connect(m_slide_box, &SlideScrollBox::sMove, this, &NarrativeControl::moveSlides);
	// duration
	connect(a_slide_duration, &QAction::triggered, this, &NarrativeControl::setSlideDuration);
	connect(m_slide_box, &SlideScrollBox::sSetDuration, a_slide_duration, &QAction::trigger);
	// transition
	connect(a_slide_transition, &QAction::triggered, this, &NarrativeControl::setSlideTransition);
	connect(m_slide_box, &SlideScrollBox::sSetTransitionDuration, a_slide_transition, &QAction::trigger);
	// camera
	connect(a_slide_camera, &QAction::triggered, this, &NarrativeControl::setSlideCamera);
	// goto
	//connect(m_slide_box, &SlideScrollBox::sGoto, this, [this](int index) {
	//	openSlide(index);
	//});
	// back
	connect(m_bar->ui.back, &QPushButton::clicked, this, &NarrativeControl::showNarrativeBox);
	//change
	connect(m_slide_box, &HorizontalScrollBox::sTouch, this, [this]() {
		int last = m_slide_selection->last();
		if (last < 0) {
			m_app->setState(VSimApp::EDIT_FLYING);
		}
		else {
			m_app->setState(VSimApp::EDIT_SLIDES);
			openSlide(m_slide_selection->last());
			showCanvas(true);
		}
	});

	//CANVAS CONTROL
	// new
	connect(m_label_buttons->ui.head1, &QPushButton::clicked, this, [this]() { newLabel(LabelType::HEADER1); });
	connect(m_label_buttons->ui.head2, &QPushButton::clicked, this, [this]() { newLabel(LabelType::HEADER2); });
	connect(m_label_buttons->ui.body, &QPushButton::clicked, this, [this]() { newLabel(LabelType::BODY); });
	connect(m_label_buttons->ui.label, &QPushButton::clicked, this, [this]() { newLabel(LabelType::LABEL); });
	// move
	connect(m_canvas, &NarrativeCanvas::sItemsTransformed, this, &NarrativeControl::transformLabels);
	// text change
	connect(m_canvas, &NarrativeCanvas::sLabelUndoCommandAdded, this, &NarrativeControl::labelEdited);
	// delete
	connect(m_label_buttons->ui.delete_2, &QPushButton::clicked, this, &NarrativeControl::deleteLabels);
	// edit
	connect(m_label_buttons->ui.edit, &QAbstractButton::pressed, this, &NarrativeControl::execEditLabel);
	connect(m_label_buttons->ui.done, &QPushButton::clicked, this, &NarrativeControl::exitEdit);
	// touch
	connect(m_canvas, &NarrativeCanvas::sPoked, this, [this]() {
		m_app->setState(VSimApp::EDIT_CANVAS);
	});

	// dirty slide thumbnails
	connect(m_slide_box, &SlideScrollBox::sThumbnailsDirty, this, 
		[this]() {
		redrawThumbnails(m_slide_box->getDirtySlides());
	});

	connect(window, &MainWindow::sDebugControl, this, &NarrativeControl::debug);
}

void NarrativeControl::editStyleSettings()
{
	Narrative *narrative = getCurrentNarrative();
	if (!narrative) return;

	StyleSettingsDialog dlg;
	dlg.setStyles(narrative->labelStyles());
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

	Narrative *nar = new Narrative;

	m_undo_stack->beginMacro("New Narrative");
	int num_children = m_narrative_group->getNumChildren();
	m_undo_stack->push(new SelectNarrativesCommand(this, { }, ON_UNDO));
	m_undo_stack->push(new Group::AddNodeCommand(m_narrative_group, nar));
	m_undo_stack->push(new SelectNarrativesCommand(this, { num_children }, ON_REDO));
	m_undo_stack->endMacro();

	Narrative *narrative = getNarrative(m_narrative_group->getNumChildren() - 1);
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

	Narrative *narrative = getNarrative(active_item);

	NarrativeInfoDialog dlg(narrative);
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		qDebug() << "narrative list - cancelled edit on" << active_item;
		return;
	}

	m_undo_stack->beginMacro("Set Narrative Info");
	m_undo_stack->push(new SelectNarrativesCommand(this, { active_item }));
	m_undo_stack->push(new Narrative::SetTitleCommand(narrative, dlg.getTitle()));
	m_undo_stack->push(new Narrative::SetAuthorCommand(narrative, dlg.getAuthor()));
	m_undo_stack->push(new Narrative::SetDescriptionCommand(narrative, dlg.getDescription()));
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

void NarrativeControl::mergeNarratives(const NarrativeGroup * g)
{
	std::set<int> indices;
	m_undo_stack->beginMacro("Import Narratives");
	m_undo_stack->push(new SelectNarrativesCommand(this, {}, ON_UNDO));
	for (size_t i = 0; i < g->getNumChildren(); i++) {
		Narrative *nar = g->narrative(i);
		if (!nar) continue;
		indices.insert(m_narrative_group->getNumChildren());
		m_undo_stack->push(new Group::AddNodeCommand(m_narrative_group, nar));
	}
	SelectionData sel(indices.begin(), indices.end());
	m_undo_stack->push(new SelectNarrativesCommand(this, sel, ON_REDO));
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
	qInfo() << "Narrative Control Debug";
	qInfo() << "current narrative" << m_current_narrative;
	qInfo() << "current slide" << m_current_slide;
	auto ns = m_slide_selection->data();
	qInfo() << "nar box" << Util::iterToString(ns.begin(), ns.end());
	auto ss = m_slide_selection->data();
	qInfo() << "slide box" << Util::iterToString(ss.begin(), ss.end());
}

void NarrativeControl::enableEditing(bool enable)
{
	showCanvasEditor(enable);
	m_canvas->setEditable(enable);
	m_editing_slide = enable;
}

void NarrativeControl::load(NarrativeGroup *narratives)
{
	m_narrative_box->clear();
	openNarrative(-1);
	openSlide(-1);
	m_fade_canvas->setSlide(nullptr);

	m_narrative_group = narratives;
	showNarrativeBox();
	m_narrative_box->setGroup(narratives);
}

void NarrativeControl::openNarrative(int index)
{
	qInfo() << "open narrative at" << index;
	if (index == m_current_narrative) return;
	Narrative *nar = getNarrative(index);
	if (!nar || index < 0) {
		m_current_narrative = -1;
		m_slide_box->setGroup(nullptr);
		openSlide(-1);
		return;
	}

	// force selection
	if (!m_narrative_selection->has(index)) {
		m_narrative_selection->select(index);
	}

	openSlide(-1);
	m_current_narrative = index;
	m_bar->setSlidesHeader(nar->getTitle());
	m_slide_box->setGroup(nar);
}

bool NarrativeControl::openSlide(int index, bool go)
{
	NarrativeSlide *slide = getSlide(m_current_narrative, index);

	// failed to set slide
	if (!slide) {
		m_current_slide = -1;
		showCanvas(false);
		m_canvas->setSlide(nullptr);
		enableEditing(false);
		return false;
	}

	if (m_current_slide == index) {
		qDebug() << "slide already open";
	}
	else {
		// setting slide
		m_current_slide = index;
		m_canvas->setSlide(slide);
	}

	showCanvas(true, false);

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
	bool old_visible = m_canvas->isVisible();
	m_canvas->setVisible(show);
	m_canvas->setAttribute(Qt::WA_TransparentForMouseEvents, !show);
	if (show) {
		if (fade) {
			m_fade_in_anim->start();
		}
		else {
			m_fade_in_anim->stop();
			m_fade_in->setOpacity(1.0);
		}
	}
	else {
		if (fade && old_visible) {
			m_fade_canvas->show();
			m_fade_canvas->setSlide(m_canvas->getSlide());
			m_fade_out_anim->start();
		}
	}
}

bool NarrativeControl::canvasVisible() const
{
	return m_canvas->isVisible();
}

void NarrativeControl::showCanvasEditor(bool show)
{
	m_label_buttons->setVisible(show);
}

void NarrativeControl::showNarrativeBox()
{
	m_bar->showNarratives();
}

void NarrativeControl::showSlideBox()
{
	if (m_current_narrative < 0) return;
	m_bar->showSlides();
	Narrative *nar = getCurrentNarrative();
	if (nar) m_bar->setSlidesHeader(nar->getTitle());
}

void NarrativeControl::editSlide()
{
	if (m_editing_slide) {
		exitEdit();
		return;
	}

	Narrative *nar = getCurrentNarrative();
	if (!nar) {
		qWarning() << "failed to edit slide - no narrative";
		return;
	}

	// is a slide open?
	int slide = -1;
	int last = m_slide_selection->last();
	// open last selected
	if (last >= 0) {
		slide = last;
	}
	// open last slide
	else if (nar->getNumChildren() > 0) {
		slide = nar->getNumChildren() - 1;
	}

	if (slide < 0) {
		qWarning() << "failed to open slide for editing" << slide;
		return;
	}

	openSlide(slide);
	enableEditing(true);
	m_app->setState(VSimApp::EDIT_CANVAS);
}

void NarrativeControl::exitEdit() {
	enableEditing(false);
	m_app->setState(VSimApp::EDIT_SLIDES);
}

NarrativeControl::SelectionLevel NarrativeControl::getSelectionLevel()
{
	if (m_current_narrative == -1) return NARRATIVES;
	if (m_current_slide == -1) return SLIDES;
	return LABELS;
}

void NarrativeControl::selectNarratives(const SelectionData &narratives)
{
	// if single selection and already open then don't do anything
	//if (narratives.size() == 1 && m_current_narrative == *narratives.rbegin()) {
	//	m_app->setState(VSimApp::EDIT_NARS);
	//	return;
	//}

	openNarrative(-1); // force re-open on things like new/remove narrative
	m_narrative_selection->set(narratives);
	openNarrative(m_narrative_selection->last());
	openSlide(0);

	m_app->setState(VSimApp::EDIT_NARS);
	m_bar->showNarratives();
	emit sEditEvent();
}

void NarrativeControl::selectSlides(int narrative, const SelectionData &slides)
{
	openNarrative(narrative);
	m_narrative_selection->select(narrative);
	m_slide_selection->set(slides);
	openSlide(m_slide_selection->last());
	m_app->setState(VSimApp::EDIT_SLIDES);
	m_bar->showSlides();
	emit sEditEvent();
}

void NarrativeControl::selectLabels(int narrative, int slide, const std::set<NarrativeSlideItem *> &labels)
{
	openNarrative(narrative);
	m_narrative_selection->select(narrative);
	m_slide_selection->select(slide);
	enableEditing(true);
	openSlide(slide);
	m_canvas->setSelection(labels);
	m_app->setState(VSimApp::EDIT_CANVAS);
}

int NarrativeControl::getCurrentNarrativeIndex()
{
	return m_current_narrative;
}

std::set<int> NarrativeControl::getSelectedNarratives() const
{
	return m_narrative_selection->toSet();
}

int NarrativeControl::getCurrentSlideIndex()
{
	return m_current_slide;
}

Narrative * NarrativeControl::getCurrentNarrative()
{
	if (m_current_narrative < 0) return nullptr;
	return getNarrative(m_current_narrative);
}

NarrativeSlide * NarrativeControl::getCurrentSlide()
{
	if (m_current_narrative < 0) return nullptr;
	return getSlide(m_current_narrative, m_current_slide);
}

Narrative *NarrativeControl::getNarrative(int index)
{
	if (index < 0) return nullptr;
	return dynamic_cast<Narrative*>(m_narrative_group->child(index));
}

NarrativeSlide * NarrativeControl::getSlide(int narrative, int slide)
{
	Narrative *nar = getNarrative(narrative);
	if (!nar) return nullptr;
	return dynamic_cast<NarrativeSlide*>(nar->child(slide));
}

NarrativeSlideLabel * NarrativeControl::getLabel(int narrative, int slide, int label)
{
	NarrativeSlide *s = getSlide(narrative, slide);
	if (!s) return nullptr;
	if (label < 0 || (uint)label >= s->getNumChildren()) return nullptr;
	return dynamic_cast<NarrativeSlideLabel*>(s->getChild(label));
}

void NarrativeControl::newSlide()
{
	Narrative *nar = getNarrative(m_current_narrative);
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
	Narrative *nar = getNarrative(m_current_narrative);

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

	Narrative *narrative = getCurrentNarrative();
	m_undo_stack->beginMacro("Move Slides");
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, sfrom, ON_UNDO));
	m_undo_stack->push(new Group::MoveNodesCommand(narrative, mapping));
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, sto, ON_REDO));
	m_undo_stack->endMacro();
}

void NarrativeControl::newLabel(LabelType style) {
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
	label->setBackground(QColor(0, 0, 0, 255));

	qInfo() << "Creating new canvas label";

	Narrative *nar = getCurrentNarrative();
	LabelStyle *label_style = nar->getLabelStyles()->getStyle((LabelType)style);
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
	qInfo() << "Transforming" << items.size() << "canvas items";

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
	qDebug() << "redraw thumbnails";
	for (auto slide : slides) {
		qDebug() << "redrawing thumbnail" << slide;
		QImage thumbnail;

		thumbnail = m_app->generateThumbnail(slide);
		
		slide->setThumbnail(thumbnail);
		//item->setImage(thumbnail);
		//item->setThumbnailDirty(false);
	}
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
