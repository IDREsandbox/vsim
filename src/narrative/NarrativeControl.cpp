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

#include <QElapsedTimer>

#include "MainWindow.h"
#include "OSGViewerWidget.h"
#include "NarrativeInfoDialog.h"
#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative2.h"
#include "narrative/NarrativeScrollBox.h"
#include "narrative/SlideScrollBox.h"
#include "narrative/SlideScrollItem.h"
#include "narrative/NarrativeSlideLabels.h"
#include "dragLabel.h"
#include "labelCanvas.h"
#include "labelCanvasView.h"
#include "editButtons.h"
#include "StyleSettings.h"
#include "LabelStyle.h"

NarrativeControl::NarrativeControl(QObject *parent, MainWindow *window)
	: QObject(parent), 
	m_window(window), 
	m_current_narrative(-1),
	m_model(nullptr)
{
	m_narrative_box = window->topBar()->ui.narratives;
	m_slide_box = window->topBar()->ui.slides;
	m_canvas = window->m_drag_area;
	m_undo_stack = window->m_undo_stack;

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
	connect(m_slide_box, &SlideScrollBox::sSelectionChange, this, &NarrativeControl::onSlideSelection);
	
	//CANVAS CONTROL
	// new
	//connect(m_canvas, &labelCanvas::sNewLabel, this, &NarrativeControl::newLabel); // doesn't exist, see edit buttons
	// move
	connect(m_canvas, &labelCanvas::sSetSize, this, &NarrativeControl::resizeLabel);
	connect(m_canvas, &labelCanvas::sSetPos, this, &NarrativeControl::moveLabel);

	// text change
	//connect(m_canvas, SIGNAL(sSuperTextSet(QString, int)), this, SLOT(textEditLabel(QString, int))); // these two are directly connected
	connect(m_canvas, &labelCanvas::sEdited, this, &NarrativeControl::editLabel);

	// delete
	connect(m_canvas, SIGNAL(sDeleteLabel(int)), this, SLOT(deleteLabel(int)));

	editDlg = new editButtons(m_window);
	editDlg->move(10, 200);
	editDlg->hide();

	QSignalMapper* signalMapper = new QSignalMapper(this);

	connect(editDlg->ui.done, &QPushButton::clicked, this, &NarrativeControl::exitEdit);
	connect(editDlg->ui.delete_2, &QPushButton::clicked, this, &NarrativeControl::deleteLabelButton);
	connect(editDlg->ui.label, &QPushButton::clicked, this, &NarrativeControl::newLab);
	connect(editDlg->ui.head1, &QPushButton::clicked, this, &NarrativeControl::newH1);
	connect(editDlg->ui.head2, &QPushButton::clicked, this, &NarrativeControl::newH2);
	connect(editDlg->ui.body, &QPushButton::clicked, this, &NarrativeControl::newBod);	//connect(editDlg->ui.body, SIGNAL(clicked()), signalMapper, SLOT(map()));
	connect(editDlg->ui.image, &QPushButton::clicked, this, &NarrativeControl::newImg);



	/*connect(editDlg->ui.label, &QPushButton::clicked, this, [this]() {newLabel(
		"<p>New label</p>",
		"p{color:rgb(0, 0, 0); font-size: 12pt; font-family: \"Arial\"; font-weight: bold; text-align:center;}",
		"background-color: rgba(255, 255, 255, 178);"); });
	connect(editDlg->ui.head1, &QPushButton::clicked, this, [this]() {newLabel(
		"<p>Heading 1</p>",
		"p{color:rgb(240, 240, 240); font-size: 36pt; font-family: \"Arial\"; font-weight: bold; text-align:center;}",
		"background-color: rgba(0, 0, 0, 178);"); });
	connect(editDlg->ui.head2, &QPushButton::clicked, this, [this]() {newLabel(
		"<p>Heading 2</p>",
		"p{color:rgb(224, 147, 31); font-size: 20pt; font-family: \"Arial\"; font-weight: bold; text-align:center;}",
		"background-color: rgba(0, 0, 0, 178);"); });
	connect(editDlg->ui.body, &QPushButton::clicked, this, [this]() {newLabel(
		"<p>New body</p>",
		"p{color:rgb(240, 240, 240); font-size: 12pt; font-family: \"Arial\"; font-weight: regular; text-align:left;}",
		"background-color: rgba(0, 0, 0, 178);"); });*/

	// dirty slide thumbnails
	connect(m_slide_box, &SlideScrollBox::sThumbnailsDirty, this, 
		[this]() {redrawThumbnails(m_slide_box->getDirtySlides()); }
		);

	connect(window, &MainWindow::sDebugControl, this, &NarrativeControl::debug);
}

NarrativeControl::~NarrativeControl()
{
}

void NarrativeControl::editStyleSettings()
{
	int active_item = m_narrative_box->getLastSelected();
	Narrative2 *narrative = getNarrative(active_item);

	StyleSettings dlg(narrative);
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}
}

void NarrativeControl::newH1()
{
	int active_item = m_narrative_box->getLastSelected();
	Narrative2 *narrative = getNarrative(active_item);

	LabelStyle* lb = narrative->getH1();
	newLabel(
		"<p>New Header 1</p>",
		"p{color:rgba(" + std::to_string(lb->getRed()) + "," + std::to_string(lb->getGreen()) + "," + std::to_string(lb->getBlue()) + "," + std::to_string(lb->getOpacity()) + "); font-size: " + std::to_string(lb->getSize()) + "pt; font-family: \"" + lb->getFont() +"\"; font-weight: " + lb->getWeight() + "; text-align:" + lb->getAlign() + ";}",
		"background-color: rgba(" + std::to_string(lb->getRed_BG()) + "," + std::to_string(lb->getGreen_BG()) + "," + std::to_string(lb->getBlue_BG()) + "," + std::to_string(lb->getOpacity_BG()) + ");");
}

void NarrativeControl::newH2()
{
	int active_item = m_narrative_box->getLastSelected();
	Narrative2 *narrative = getNarrative(active_item);

	LabelStyle* lb = narrative->getH2();
	newLabel(
		"<p>New Header 2</p>",
		"p{color:rgba(" + std::to_string(lb->getRed()) + "," + std::to_string(lb->getGreen()) + "," + std::to_string(lb->getBlue()) + "," + std::to_string(lb->getOpacity()) + "); font-size: " + std::to_string(lb->getSize()) + "pt; font-family: \"" + lb->getFont() + "\"; font-weight: " + lb->getWeight() + "; text-align:" + lb->getAlign() + ";}",
		"background-color: rgba(" + std::to_string(lb->getRed_BG()) + "," + std::to_string(lb->getGreen_BG()) + "," + std::to_string(lb->getBlue_BG()) + "," + std::to_string(lb->getOpacity_BG()) + ");");
}

void NarrativeControl::newBod()
{
	int active_item = m_narrative_box->getLastSelected();
	Narrative2 *narrative = getNarrative(active_item);

	LabelStyle* lb = narrative->getBod();
	newLabel(
		"<p>New Body Text</p>",
		"p{color:rgba(" + std::to_string(lb->getRed()) + "," + std::to_string(lb->getGreen()) + "," + std::to_string(lb->getBlue()) + "," + std::to_string(lb->getOpacity()) + "); font-size: " + std::to_string(lb->getSize()) + "pt; font-family: \"" + lb->getFont() + "\"; font-weight: " + lb->getWeight() + "; text-align:" + lb->getAlign() + ";}",
		"background-color: rgba(" + std::to_string(lb->getRed_BG()) + "," + std::to_string(lb->getGreen_BG()) + "," + std::to_string(lb->getBlue_BG()) + "," + std::to_string(lb->getOpacity_BG()) + ");");

}

void NarrativeControl::newLab()
{
	int active_item = m_narrative_box->getLastSelected();
	Narrative2 *narrative = getNarrative(active_item);

	LabelStyle* lb = narrative->getLab();
	newLabel(
		"<p>New Label</p>",
		"p{color:rgba(" + std::to_string(lb->getRed()) + "," + std::to_string(lb->getGreen()) + "," + std::to_string(lb->getBlue()) + "," + std::to_string(lb->getOpacity()) + "); font-size: " + std::to_string(lb->getSize()) + "pt; font-family: \"" + lb->getFont() + "\"; font-weight: " + lb->getWeight() + "; text-align:" + lb->getAlign() + ";}",
		"background-color: rgba(" + std::to_string(lb->getRed_BG()) + "," + std::to_string(lb->getGreen_BG()) + "," + std::to_string(lb->getBlue_BG()) + "," + std::to_string(lb->getOpacity_BG()) + ");");

}

void NarrativeControl::newImg()
{
	int active_item = m_narrative_box->getLastSelected();
	Narrative2 *narrative = getNarrative(active_item);

	LabelStyle* lb = narrative->getImg();
	newLabel(
		"<p>New Image Label</p>",
		"p{color:rgba(" + std::to_string(lb->getRed()) + "," + std::to_string(lb->getGreen()) + "," + std::to_string(lb->getBlue()) + "," + std::to_string(lb->getOpacity()) + "); font-size: " + std::to_string(lb->getSize()) + "pt; font-family: \"" + lb->getFont() + "\"; font-weight: " + lb->getWeight() + "; text-align:" + lb->getAlign() + ";}",
		"background-color: rgba(" + std::to_string(lb->getRed_BG()) + "," + std::to_string(lb->getGreen_BG()) + "," + std::to_string(lb->getBlue_BG()) + "," + std::to_string(lb->getOpacity_BG()) + ");");

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

void NarrativeControl::moveNarratives(std::set<int> from, int to)
{
	std::vector<std::pair<int, int>> mapping;
	std::set<int> to_set;
	auto it = from.begin();
	for (int i = 0; i < from.size(); i++) {
		mapping.push_back(std::make_pair(*it, to + i));
		to_set.insert(to + i);
		++it;
	}

	m_undo_stack->beginMacro("Move Narratives");
	m_undo_stack->push(new SelectNarrativesCommand(this, from, ON_UNDO));
	m_undo_stack->push(new Group::MoveNodesCommand(m_narrative_group, mapping));
	m_undo_stack->push(new SelectNarrativesCommand(this, to_set, ON_REDO));
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
	int next_selection = nextSelectionAfterDelete(n, selection);

	m_undo_stack->beginMacro("Import Narratives");
	m_undo_stack->push(new SelectNarrativesCommand(this, { next_selection }, ON_UNDO));
	for (uint i = 0; i < n; i++) {
		Narrative2 *narrative = dynamic_cast<Narrative2*>(group->getChild(i));
		if (narrative == nullptr) {
			qWarning() << "Non-narrative detected when loading narrative group";
		}
		m_undo_stack->push(new NarrativeGroup::AddNarrativeCommand(m_narrative_group, narrative));
	}
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
		m_canvas->clearCanvas();
		emit selectionChanged();
		return;
	}

	qDebug() << "open narrative at" << index;
	this->m_window->topBar()->showSlides();
	m_current_narrative = index;

	Narrative2 *nar = getNarrative(index);
	this->m_window->topBar()->setSlidesHeader(nar->getTitle());
	m_slide_box->setGroup(nar);
	this->exitEdit();

	if (nar->getNumChildren() > 0) {
		m_slide_box->setLastSelected(0);
		setSlide(0);
	}
	emit selectionChanged();
}

bool NarrativeControl::setSlide(int index)
{
	if (m_current_slide == index) return true;
	qDebug() << "Narrative Control - set slide" << index;
	if (m_current_narrative < 0) {
		m_current_slide = -1;
		return false;
	}
	NarrativeSlide *slide = getSlide(m_current_narrative, index);
	if (!slide) {
		m_current_slide = -1;
		m_canvas->clearCanvas();
		return false;
	}

	m_current_slide = index;
	// if not selected then force selection
	if (!m_slide_box->isSelected(index)) m_slide_box->select(index);

	m_canvas->setSlide(slide);
	emit selectionChanged();
	return true;
}

void NarrativeControl::deleteLabelButton() {
	int selection = m_canvas->getSelection();
	deleteLabel(selection);
}

void NarrativeControl::exitEdit() {
	editDlg->hide();
	m_window->canvasView()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
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
	emit selectionChanged();
}

void NarrativeControl::selectSlides(int narrative, std::set<int> slides)
{
	if (m_current_narrative != narrative) {
		setNarrative(narrative);
	}
	m_narrative_box->setSelection({narrative}, narrative);
	m_slide_box->setSelection({slides}, *slides.begin());
	emit selectionChanged();
}

void NarrativeControl::selectLabel(int narrative, int slide, int label)
{
	setNarrative(narrative);
	setSlide(slide);
	m_canvas->setSelection(label);
}

void NarrativeControl::newLabel(const std::string &text, const std::string &style, const std::string &widget_style) {
	NarrativeSlide *slide = getCurrentSlide();
	if (!slide) {
		qWarning() << "Narrative Control - creating label without slide open";
		return;
	}
	// new index at the end
	int idx = slide->getNumChildren();

	auto cmd = new Group::NewNodeCommand<NarrativeSlideLabels>(slide, idx);
	NarrativeSlideLabels *label = cmd->getNode();

	// initialization
	//lab->setrX(temp->ratioX);
	//lab->setrY(temp->ratioY);
	//lab->setrW(temp->ratioWidth);
	//lab->setrH(temp->ratioHeight);
	label->setStyle(style); // have to set style before text because that's how the document works
	label->setWidgetStyle(widget_style);
	label->setText(text);

	// push command
	m_undo_stack->beginMacro("New Label");
	m_undo_stack->push(cmd);
	m_undo_stack->push(new SelectLabelCommand(this, m_current_narrative, m_current_slide, idx, ON_REDO));
	m_undo_stack->endMacro();

	SlideScrollItem *item = m_slide_box->getItem(m_current_slide);
	item->setThumbnailDirty(true);
}

void NarrativeControl::deleteLabel(int idx)
{
	NarrativeSlide *slide = getCurrentSlide();
	if (slide == nullptr) return;

	// consistency check
	if (idx < 0 || idx >= slide->getNumChildren()) {
		qWarning() << "Delete label out of range" << idx << "/" << slide->getNumChildren();
	}

	// push delete command
	m_undo_stack->beginMacro("Import Narratives");
	m_undo_stack->push(new SelectLabelCommand(this, m_current_narrative, m_current_slide, idx, ON_UNDO));
	m_undo_stack->push(new Group::DeleteNodeCommand<NarrativeSlideLabels>(slide, idx));
	m_undo_stack->endMacro();

	SlideScrollItem *item = m_slide_box->getItem(m_current_slide);
	item->setThumbnailDirty(true);
}

void NarrativeControl::moveLabel(float rx, float ry, int idx) {
	NarrativeSlideLabels *label = getLabel(m_current_narrative, m_current_slide, idx);
	if (!label) {
		qWarning() << "Narrative Control - move label with invalid index" 
			<< m_current_narrative << m_current_slide << idx;
		return;
	}

	m_undo_stack->beginMacro("Move Label");
	m_undo_stack->push(new NarrativeSlideLabels::MoveCommand(label, rx, ry));
	m_undo_stack->push(new SelectLabelCommand(this, m_current_narrative, m_current_slide, idx));
	m_undo_stack->endMacro();

	SlideScrollItem *item = m_slide_box->getItem(m_current_slide);
	item->setThumbnailDirty(true);
}

void NarrativeControl::resizeLabel(float rw, float rh, int idx) {
	NarrativeSlideLabels *label = getLabel(m_current_narrative, m_current_slide, idx);
	if (!label) {
		qWarning() << "Narrative Control - resize label with invalid index"
			<< m_current_narrative << m_current_slide << idx;
		return;
	}

	m_undo_stack->beginMacro("Resize Label");
	m_undo_stack->push(new NarrativeSlideLabels::ResizeCommand(label, rw, rh));
	m_undo_stack->push(new SelectLabelCommand(this, m_current_narrative, m_current_slide, idx));
	m_undo_stack->endMacro();

	SlideScrollItem *item = m_slide_box->getItem(m_current_slide);
	item->setThumbnailDirty(true);
}

void NarrativeControl::editLabel(int idx)
{
	// This one is special
	// Since the label and document are directly connected via QTextDocument
	// all we need to know about is when changes happen so that we can undo them

	NarrativeSlideLabels *label = getLabel(m_current_narrative, m_current_slide, idx);
	if (!label) {
		qWarning() << "Narrative Control - edit label with invalid index"
			<< m_current_narrative << m_current_slide << idx;
		return;
	}
	QTextDocument *doc = label->getDocument();

	qDebug() << "on undo command added";
	m_undo_stack->beginMacro("Edit Label");
	m_undo_stack->push(new NarrativeSlideLabels::DocumentEditWrapperCommand(doc));
	m_undo_stack->push(new SelectLabelCommand(this, m_current_narrative, m_current_slide, idx));
	m_undo_stack->endMacro();
}

//void NarrativeControl::textEditLabel(QString str, int idx) {
//	qDebug() << "edit text";
//	NarrativeSlide* curSl = getNarrativeSlide(m_current_narrative, m_current_slide);
//	NarrativeSlideLabels* lab = dynamic_cast<NarrativeSlideLabels*>(curSl->getChild(idx));
//	lab->setText(str.toStdString());
//}

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

NarrativeSlideLabels * NarrativeControl::getLabel(int narrative, int slide, int label)
{
	NarrativeSlide *s = getSlide(narrative, slide);
	if (!s) return nullptr;
	if (label < 0 || (uint)label >= s->getNumChildren()) return nullptr;
	return dynamic_cast<NarrativeSlideLabels*>(s->getChild(label));
}

void NarrativeControl::onSlideSelection()
{
	if (m_current_narrative < 0) {
		qWarning() << "Narrative Control - slide selection while current narrative null";
		return;
	}

	setSlide(m_slide_box->getLastSelected());

	emit selectionChanged();
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

	// make new slide, initialize matrix and stuff
	auto newcmd = new Narrative2::NewSlideCommand(nar, index);
	NarrativeSlide *slide = newcmd->getNode();
	slide->setCameraMatrix(matrix);

	// perform command
	m_undo_stack->beginMacro("New Slide");
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, { undo_selection }, ON_UNDO));
	m_undo_stack->push(newcmd);
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, { index }, ON_REDO));
	m_undo_stack->endMacro();

	qDebug() << "after push";

	qDebug() << "after set camera matrix";
	std::cout << m_window->getViewer()->getCameraManipulator()->getMatrix();

	setSlide(nar->getNumChildren() - 1);
}

void NarrativeControl::deleteSlides()
{
	qDebug() << "delete slides";
	std::set<int> selection = m_slide_box->getSelection();
	Narrative2 *nar = getNarrative(m_current_narrative);

	int next_selection = nextSelectionAfterDelete(nar->getNumChildren(), selection);
	
	m_undo_stack->beginMacro("Delete Slides");
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
	qDebug() << "EDIT SLIDE CURRENT SLDIE?" << m_current_narrative << m_current_slide << getCurrentSlide();
	m_window->canvasView()->setAttribute(Qt::WA_TransparentForMouseEvents, false);
	editDlg->show();
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

void NarrativeControl::moveSlides(std::set<int> from, int to)
{
	// map this to single move commands
	std::vector<std::pair<int, int>> mapping;
	std::set<int> to_set;
	auto it = from.begin();
	for (int i = 0; i < from.size(); i++) {
		mapping.push_back(std::make_pair(*it, to + i));
		to_set.insert(to + i);
		++it;
	}

	m_undo_stack->beginMacro("Move Slides");
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, from, ON_UNDO));
	m_undo_stack->push(new Group::MoveNodesCommand(getNarrative(m_current_narrative), mapping));
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, to_set, ON_REDO));
	m_undo_stack->endMacro();
}


void NarrativeControl::redrawThumbnails(const std::vector<SlideScrollItem*> items)
{

	for (auto item : items) {
		qDebug() << "redrawing thumbnail" << item->getIndex();
		QImage thumbnail;

		thumbnail = generateThumbnail(item->getSlide());

		item->setImage(thumbnail);
		item->setThumbnailDirty(false);
	}
}

QImage NarrativeControl::generateThumbnail(NarrativeSlide *slide)
{
	QElapsedTimer timer;
	timer.start();

	// set the camera, create a dummy canvas
	osg::Matrixd old_matrix = m_window->getViewerWidget()->getCameraMatrix();
	m_window->getViewerWidget()->setCameraMatrix(slide->getCameraMatrix());

	auto container = m_window->getViewerWidget();
	labelCanvas canvas;
	labelCanvasView view(m_window->getViewerWidget(), &canvas);
	canvas.setGeometry(0, 0, container->width(), container->height());
	canvas.setSlide(slide);

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
	qDebug() << "render canvas size" << canvas.size();
	canvas.render(&painter, QPoint(0, 0), QRect(QPoint(0, 0),canvas.size()), QWidget::DrawChildren | QWidget::IgnoreMask);

	// scale down the image
	QImage smallimg;
	smallimg = img.scaled(288, 162, Qt::IgnoreAspectRatio);

	// revert the camera
	m_window->getViewerWidget()->setCameraMatrix(old_matrix);

	int ns = timer.nsecsElapsed();
	qDebug() << "thumbnail time ms" << ns / 1.0e6;
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

SelectLabelCommand::SelectLabelCommand(NarrativeControl * control, int narrative, int slide, int label, SelectionCommandWhen when, QUndoCommand * parent)
	: QUndoCommand(parent),
	m_control(control),
	m_narrative(narrative),
	m_slide(slide),
	m_label(label),
	m_when(when)
{
}
void SelectLabelCommand::undo()
{
	if (m_when != ON_REDO) {
		m_control->selectLabel(m_narrative, m_slide, m_label);
	}
}
void SelectLabelCommand::redo()
{
	if (m_when != ON_UNDO) {
		m_control->selectLabel(m_narrative, m_slide, m_label);
	}
}
