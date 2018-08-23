#include "narrative/NarrativeControl.h"
#include "Core/Util.h"

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
#include "Gui/PasswordDialog.h"

#include "narrative/NarrativeInfoDialog.h"
#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "narrative/NarrativeScrollBox.h"
#include "narrative/SlideScrollBox.h"
#include "narrative/SlideScrollItem.h"
#include "VSimRoot.h"
#include "Core/GroupCommands.h"
#include "Core/LockTable.h"

#include "Canvas/LabelType.h"
#include "Canvas/LabelStyle.h"
#include "Canvas/LabelStyleGroup.h"
#include "Canvas/CanvasContainer.h"
#include "Canvas/CanvasEditor.h"
#include "Canvas/CanvasControl.h"

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
	m_canvas_stack_wrapper(this),
	m_current_lock(nullptr)
{
	m_bar = window->topBar();
	m_narrative_box = window->topBar()->ui.narratives;
	m_slide_box = window->topBar()->ui.slides;
	m_narrative_selection = m_narrative_box->selectionStack();
	m_slide_selection = m_slide_box->selectionStack();
	m_canvas = window->canvas();
	m_fade_canvas = window->fadeCanvas();
	m_undo_stack = m_app->getUndoStack();
	m_viewer = m_window->getViewerWidget();

	m_canvas->setStack(&m_canvas_stack_wrapper);

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

	m_thumbnail_size = QSize(288, 162);
	m_thumbnail_canvas = std::make_unique<CanvasContainer>();
	m_thumbnail_canvas->resize(m_thumbnail_size);
	m_thumbnail_canvas->move(0, 0);
	m_thumbnail_canvas->setScene(nullptr);
	m_thumbnail_canvas->setEditable(false);
	m_viewer->setThumbnailSize(m_thumbnail_size);

	showCanvas(false);

	//m_label_buttons = m_window->labelButtons();
	//m_label_buttons->move(10, 200);
	//m_label_buttons->hide();
	load(app->getRoot()->narratives());

	connect(m_window, &MainWindow::sEditStyleSettings, this, [this]() {
		if (getCurrentNarrativeIndex() < 0) {
			qWarning() << "Can't edit styles: no active narrative";
			return;
		}
		m_canvas->editStyles();
	});

	// app state
	connect(m_app, &VSimApp::sReset, this, &NarrativeControl::onReset);
	connect(m_app, &VSimApp::sTick, this, &NarrativeControl::update);
	connect(m_app, &VSimApp::sStateChanged, this, [this](auto old, auto current) {
		VSimApp::State state = m_app->state();
		if (m_app->isFlying()
			|| state == VSimApp::EDIT_ERS) {
			// fade out
			showCanvas(false, true);
			openSlide(-1);
		}

		if (state != VSimApp::EDIT_CANVAS
			&& state != VSimApp::EDIT_SLIDES) {
			enableEditing(false);
		}

		if (state == VSimApp::EDIT_CANVAS) {
			// open if possible
			int last = m_slide_selection->last();
			if (last >= 0) {
				openSlide(last, true);
				singleSelectOpenSlide();
			}
			m_canvas->setEditable(true);
		}
		else {
			// deselect canvas
			//m_canvas->setSelection({});
		}
	});

	// NARRATIVE CONTROL
	a_new_narrative = new QAction("New Narrative", this);
	a_new_narrative->setIconText("+");
	connect(a_new_narrative, &QAction::triggered, this, &NarrativeControl::newNarrative);

	a_delete_narratives = new QAction("Delete Narrative", this);
	a_delete_narratives->setIconText("-");
	connect(a_delete_narratives, &QAction::triggered, this, &NarrativeControl::deleteNarratives);

	a_narrative_info = new QAction("Narrative Info", this);
	a_narrative_info->setIconText("Info");
	connect(a_narrative_info, &QAction::triggered, this, &NarrativeControl::editNarrativeInfo);

	a_open_narrative = new QAction("Open Narrative", this);
	a_open_narrative->setIconText("Open");
	connect(a_open_narrative, &QAction::triggered, this, &NarrativeControl::showSlideBox);

	a_lock_narratives = new QAction("Lock Narrative", this);
	a_lock_narratives->setIconText("Lock");
	connect(a_lock_narratives, &QAction::triggered, this, &NarrativeControl::lockNarratives);

	a_unlock_narratives = new QAction("Unlock Narrative", this);
	a_unlock_narratives->setIconText("Unlock");
	connect(a_unlock_narratives, &QAction::triggered, this, &NarrativeControl::unlockNarratives);

	// menus
	QMenu *nar_menu = new QMenu("Narrative menu", m_narrative_box);
	QMenu *nar_item_menu = new QMenu("Narrative item menu", m_narrative_box);

	nar_menu->addAction(a_new_narrative);
	nar_item_menu->addAction(a_new_narrative);
	nar_item_menu->addAction(a_delete_narratives);
	nar_item_menu->addAction(a_narrative_info);
	nar_item_menu->addAction(a_open_narrative);
	nar_item_menu->addAction(a_lock_narratives);
	nar_item_menu->addAction(a_unlock_narratives);

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
	connect(m_narrative_selection, &SelectionStack::sChanged,
		this, &NarrativeControl::onNarrativeSelectionChanged);

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
	m_slide_menu = new QMenu("Narrative menu", m_narrative_box);
	m_slide_menu->addAction(a_new_slide);

	m_slide_item_menu = new QMenu("Narrative item menu", m_narrative_box);
	m_slide_item_menu->addAction(a_new_slide);
	m_slide_item_menu->addAction(a_delete_slides);
	m_slide_item_menu->addAction(a_slide_duration);
	m_slide_item_menu->addAction(a_slide_transition);
	m_slide_item_menu->addAction(a_slide_camera);

	m_slide_box->setMenu(m_slide_menu);
	m_slide_box->setItemMenu(m_slide_item_menu);

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
	connect(m_bar->ui.back, &QAbstractButton::clicked, this, [this]() {
		showNarrativeBox();
		m_app->setState(VSimApp::State::EDIT_NARS);
	});
	//change
	connect(m_slide_box, &HorizontalScrollBox::sTouch, this, [this]() {
		int last = m_slide_selection->last();
		if (last < 0) {
			m_app->setState(VSimApp::EDIT_FLYING);
		}
		else if (m_app->isPlaying()) {
			m_app->setState(VSimApp::PLAY_WAIT_CLICK);
			openSlide(m_slide_selection->last());
			showCanvas(true);
		}
		else {
			m_app->setState(VSimApp::EDIT_SLIDES);
			openSlide(m_slide_selection->last());
			showCanvas(true);
		}
	});

	// Canvas control
	connect(m_canvas, &CanvasEditor::sDone, this, &NarrativeControl::exitEdit);
	connect(m_canvas->control(), &CanvasControl::sAnyChange, this, [this]() {
		NarrativeSlide *slide = getCurrentSlide();
		if (!slide) return;
		slide->setForegroundDirty();
	});


	connect(m_viewer, &OSGViewerWidget::sThumbnailFinished, this, &NarrativeControl::onThumbnailReady);

	connect(window, &MainWindow::sDebugControl, this, &NarrativeControl::debug);
}

void NarrativeControl::newNarrative()
{
	if (m_narrative_group->restrictedToCurrent()) return;

	// open up the dialog
	NarrativeInfoDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	auto nar = std::make_shared<Narrative>();

	m_undo_stack->beginMacro("New Narrative");
	int num_children = m_narrative_group->size();
	m_undo_stack->push(new SelectNarrativesCommand(this, { }, Command::ON_UNDO));
	m_undo_stack->push(new AddNodeCommand<Narrative>(m_narrative_group, nar));
	m_undo_stack->push(new SelectNarrativesCommand(this, { num_children }, Command::ON_REDO));
	m_undo_stack->endMacro();

	Narrative *narrative = getNarrative(m_narrative_group->size() - 1);
	narrative->setTitle(dlg.getTitle());
	narrative->setAuthor(dlg.getAuthor());
	narrative->setDescription(dlg.getDescription());
}

void NarrativeControl::editNarrativeInfo()
{
	int active_item = m_narrative_selection->last();
	qInfo() << "narrative list - begin edit on" << active_item;
	if (active_item < 0) {
		qWarning() << "narrative list - can't edit with no selection";
		return;
	}

	Narrative *narrative = getNarrative(active_item);

	NarrativeInfoDialog dlg(narrative);

	// read only mode
	if (narrative->lockTable()->isLocked()) {
		dlg.setReadOnly(true);
		dlg.exec();
		return;
	}

	// edit mode
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		qInfo() << "narrative list - cancelled edit on" << active_item;
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
	if (m_narrative_group->restrictedToCurrent()) return;
	if (m_narrative_selection->empty()) return;
	auto selection = m_narrative_selection->data();
	std::set<size_t> set(selection.begin(), selection.end());

	// get pointers to nodes to delete
	m_undo_stack->beginMacro("Delete Narratives");
	m_undo_stack->push(new SelectNarrativesCommand(this, selection, Command::ON_UNDO));
	m_undo_stack->push(new RemoveMultiCommand<Narrative>(m_narrative_group, set));
	m_undo_stack->push(new SelectNarrativesCommand(this, {}, Command::ON_REDO));
	m_undo_stack->endMacro();
}

void NarrativeControl::lockNarratives()
{
	// which can be locked?
	std::vector<LockTable*> lock_me;
	for (int i : m_narrative_selection->data()) {
		auto *table = getNarrative(i)->lockTable();
		if (!table->isLocked()) {
			lock_me.push_back(table);
		}
	}

	if (lock_me.size() == 0) return;
	
	// warning dialog
	auto btn = QMessageBox::warning(m_window, "Lock Narratives",
		"Are you sure you want to lock these narratives? This is not undoable.",
		QMessageBox::StandardButton::Ok, QMessageBox::StandardButton::Cancel);
	if (btn == QMessageBox::Cancel) return;

	CreatePasswordDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) return;
	bool use_pw = dlg.usePassword();
	QString pw = dlg.password();

	HashLock hash;
	if (use_pw) hash = HashLock::generateHash(pw.toStdString());

	int bad_count = 0;
	int good_count = 0;
	for (int i : m_narrative_selection->data()) {
		Narrative *nar = getNarrative(i);
		if (!nar) return;
		auto *lock = nar->lockTable();
		if (lock->isLocked()) {
			bad_count++;
			continue;
		}
		good_count++;
		if (use_pw) {
			lock->lockWithPasswordHash(hash);
		}
		else {
			lock->lock();
		}
	}

	if (bad_count > 0) {
		QString msg = QString().sprintf(
			"Failed to lock %d selected narratives - were already locked.",
			bad_count);
		QMessageBox::warning(m_window, "Lock Narratives", msg);
	}

	if (good_count > 0) {
		m_undo_stack->clear();
	}

	// update actions
	onNarrativeSelectionChanged();
}

void NarrativeControl::unlockNarratives()
{
	// clump narratives by same hash
	// unlock them in groups

	// just do the n^2 solution
	std::vector<std::vector<Narrative*>> clumps;

	std::set<size_t> nar_set = getSelectedNarratives();
	std::vector<size_t> indices(nar_set.begin(), nar_set.end());
	int nperm = 0;
	int nskipped = 0;
	int nunlocked = 0;

	std::vector<bool> checked(indices.size(), false);

	for (size_t i = 0; i < indices.size(); i++) {
		if (checked[i]) continue;
		checked[i] = true;

		std::vector<Narrative*> clump;
		Narrative *nar = m_narrative_group->child(indices[i]);
		const LockTable *lt = nar->lockTable();

		if (lt->isPermanent()) {
			nperm++;
			continue;
		}

		if (!nar->lockTable()->isLocked()) {
			continue;
		}

		HashLock hash = lt->hash();
		clump.push_back(nar);

		for (size_t j = i; j < indices.size(); j++) {
			if (checked[j]) continue;
			Narrative *nar2 = m_narrative_group->child(indices[j]);

			if (!nar2->lockTable()->hasPassword()) {
				continue;
			}

			if (hash == nar2->lockTable()->hash()) {
				checked[j] = true;
				clump.push_back(nar2);
			}
		}
		clumps.push_back(clump);
	}

	for (int i = 0; i < clumps.size(); i++) {
		auto &clump = clumps[i];

		// get the first narrative title
		if (clump.size() < 0) return;
		Narrative *first = clump[0];
		int count = (int)clump.size();
		QString title = QString::fromStdString(first->getTitle());

		QString msg;

		if (clump.size() == 1) {
			msg = QString("Enter password for %1.")
				.arg(title);
		}
		else {
			msg = QString("Enter password for %1 narratives. (%2, ...)")
				.arg(clump.size())
				.arg(title);
		}

		std::vector<LockTable*> locks;
		for (Narrative *nar : clump) {
			locks.push_back(nar->lockTable());
		}

		TestPasswordDialog dlg;
		dlg.setWindowTitle("Unlock Narratives");
		dlg.setLabel(msg);
		dlg.showSkipButton(clumps.size() > 1);
		dlg.setLocks(locks);
		int result = dlg.exec();
		if (result == QDialog::Rejected) {
			break;
		}
		if (dlg.skipped()) {
			nskipped += locks.size();
		}
		if (dlg.unlocked()) {
			nunlocked += locks.size();
		}
	}

	QStringList msgs;
	if (nunlocked > 0) {
		msgs.push_back(QString("Unlocked %1 resources")
			.arg(nunlocked));
	}
	if (nskipped > 0) {
		msgs.push_back(QString("Skipped %1 resources")
			.arg(nskipped));
	}
	if (nperm > 0) {
		msgs.push_back(QString("Failed to unlock %1 permanently locked narratives.")
			.arg(nperm));
	}
	if (!msgs.isEmpty()) {
		QMessageBox::information(nullptr, "Unlock Resources", msgs.join("\n"));
	}
	onNarrativeSelectionChanged();
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
	m_undo_stack->push(new SelectNarrativesCommand(this, sfrom, Command::ON_UNDO));
	m_undo_stack->push(new MoveNodesCommand<Narrative>(m_narrative_group, mapping));
	m_undo_stack->push(new SelectNarrativesCommand(this, sto, Command::ON_REDO));
	m_undo_stack->endMacro();
}

void NarrativeControl::mergeNarratives(const NarrativeGroup * g)
{
	std::set<int> indices;
	m_undo_stack->beginMacro("Import Narratives");
	m_undo_stack->push(new SelectNarrativesCommand(this, {}, Command::ON_UNDO));
	for (size_t i = 0; i < g->size(); i++) {
		std::shared_ptr<Narrative> nar = g->childShared(i);
		if (!nar) continue;
		indices.insert(m_narrative_group->size());
		m_undo_stack->push(new AddNodeCommand<Narrative>(m_narrative_group, nar));
	}
	SelectionData sel(indices.begin(), indices.end());
	m_undo_stack->push(new SelectNarrativesCommand(this, sel, Command::ON_REDO));
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

void NarrativeControl::onNarrativeSelectionChanged()
{
	// whats the selection
	// disable/enable actions accordingly

	// locked/unlocked checking
	// any unlocked? -> enable lock
	bool any_unlocked = false;
	bool any_locked = false;
	for (size_t i : getSelectedNarratives()) {
		Narrative *nar = m_narrative_group->child(i);
		if (!nar->lockTable()->isLocked()) {
			any_unlocked = true;
		}
		else {
			any_locked = true;
		}
	}

	a_lock_narratives->setEnabled(any_unlocked);
	a_unlock_narratives->setEnabled(any_locked);

}

void NarrativeControl::onReset()
{
	showNarrativeBox();
	openNarrative(-1);
	onRestrictToCurrent();
	onCurrentLockChange();
}

void NarrativeControl::onRestrictToCurrent()
{
	bool restrict = m_narrative_group->restrictedToCurrent();
	bool enable = !restrict;

	a_new_narrative->setEnabled(enable);
	a_delete_narratives->setEnabled(enable);

	m_bar->ui.add->setVisible(enable);
	m_bar->ui.remove->setVisible(enable);

	m_narrative_box->enableDragging(enable);
}

void NarrativeControl::onLeaveEditSlide(NarrativeSlide *slide)
{
	if (slide == nullptr) return;
	slide->setBackgroundDirty();
}

void NarrativeControl::onCurrentLockChange()
{
	// get current nar, get current lock
	if (m_current_lock == nullptr) return;

	// locking stuff
	bool locked = m_current_lock->isLocked();
	bool enable = !locked;
	a_delete_slides->setEnabled(enable);
	a_new_slide->setEnabled(enable);
	a_edit_slide->setEnabled(enable);
	a_slide_camera->setEnabled(enable);
	a_slide_duration->setEnabled(enable);
	a_slide_transition->setEnabled(enable);

	// hide the menus
	if (enable) {
		m_slide_box->setMenu(m_slide_menu);
		m_slide_box->setItemMenu(m_slide_item_menu);
	}
	else {
		m_slide_box->setMenu(nullptr);
		m_slide_box->setItemMenu(nullptr);
	}

	m_slide_box->enableDragging(enable);

	m_bar->ui.add_2->setVisible(enable);
	m_bar->ui.remove_2->setVisible(enable);
	m_bar->ui.edit->setVisible(enable);
	m_bar->showLockIcon(locked);
}

void NarrativeControl::enableEditing(bool enable)
{
	showCanvasEditor(enable);
	m_canvas->setEditable(enable);
	m_editing_slide = enable;

	// onLeaveEditSlide event (for demanding repaints)
	NarrativeSlide *old_slide = getCurrentSlide();
	if (!enable && old_slide) {
		onLeaveEditSlide(old_slide);
	}
}



void NarrativeControl::load(NarrativeGroup *narratives)
{
	Util::reconnect(this, &m_narrative_group, narratives);
	m_narrative_box->clear();
	openNarrative(-1);
	openSlide(-1);
	//m_fade_canvas->setSlide(nullptr);
	m_fade_canvas->setScene(nullptr);

	m_narrative_group = narratives;
	showNarrativeBox();
	m_narrative_box->setGroup(narratives);

	connect(narratives, &NarrativeGroup::sRestrictToCurrentChanged, this,
		&NarrativeControl::onRestrictToCurrent);
}

void NarrativeControl::update(double dt)
{
	//// 800x600, 4x3

	// paint thumbnails?
	if (getCurrentNarrative() != nullptr && !showingNarrativeBox()) {

		std::vector<size_t> indices = m_slide_box->visibleItems();

		// search for background to paint if not already busy
		if (m_thumbnail_slide.expired()) {
			for (size_t i : indices) {
				// get appropriate slide
				Narrative *nar = getCurrentNarrative();
				auto slide = nar->childShared(i);

				if (!slide) continue;

				if (slide->thumbnailBackgroundDirty()) {
					// queue up for painting
					bool ok = m_viewer->paintThumbnail(slide->getCameraMatrix());
					if (ok) {
						m_thumbnail_slide = slide;
						break;
					}
				}
			}
		}

		// scan for a dirty foregrounds
		// paint them all on invisible canvas
		for (size_t i : indices) {
			// get appropriate slide

			NarrativeSlide *slide = getSlide(m_current_narrative, i);
			if (!slide) continue;

			if (slide->thumbnailForegroundDirty()) {
				// render and assign scene

				m_thumbnail_canvas->setScene(slide->scene());

				QPixmap pixmap(m_thumbnail_size);
				pixmap.fill(Qt::transparent);
				QPainter painter(&pixmap);

				m_thumbnail_canvas->render(&painter, QPoint(0, 0), QRect(QPoint(0, 0), m_thumbnail_size),
					QWidget::DrawChildren); // | ignore mask
				slide->setThumbnailForeground(pixmap);
			}
		}
	}
}

void NarrativeControl::openNarrative(int index)
{
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
	m_canvas->setStyles(nar->labelStyles());

	// listen to current narrative lock table
	Util::reconnect(this, &m_current_lock, nar->lockTable());
	if (m_current_lock == nullptr) return;

	onCurrentLockChange();
	connect(m_current_lock, &LockTable::sLockChanged, this,
		&NarrativeControl::onCurrentLockChange);
}

bool NarrativeControl::openSlide(int index, bool go)
{
	NarrativeSlide *slide = getSlide(m_current_narrative, index);

	// onLeaveEditSlide event (for demanding repaints)
	NarrativeSlide *old_slide = getCurrentSlide();
	if (old_slide != slide && m_editing_slide) {
		onLeaveEditSlide(old_slide);
	}

	// failed to set slide
	if (!slide) {
		m_current_slide = -1;
		showCanvas(false);
		m_canvas->setScene(nullptr);
		enableEditing(false);
		return false;
	}

	if (m_current_slide == index) {
	}
	else {
		// setting slide
		m_current_slide = index;
		m_canvas->setScene(slide->scene());
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
//		if (next < 0 || next >= m_narrative_group->size()) return false;
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
//		if (next < 0 || next >= getCurrentNarrative()->size()) return false;
//		selectSlides(getCurrentNarrativeIndex(), {next});
//	}
//	else {
//		qInfo() << "can't advance when editing slide";
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

	if (next < 0 || next >= (int)getCurrentNarrative()->size()) return false;
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
			m_fade_canvas->setScene(m_canvas->scene());
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
	//m_label_buttons->setVisible(show);
	m_canvas->setEditable(show);
}

void NarrativeControl::fadeOutSlide(NarrativeSlide *slide)
{
	if (!slide) {
		m_fade_out_anim->stop();
		m_fade_canvas->hide();
		m_fade_canvas->setScene(nullptr);
	}
	else {
		m_fade_canvas->show();
		m_fade_canvas->setScene(slide->scene());
		m_fade_out_anim->start();
	}
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

bool NarrativeControl::showingNarrativeBox() const
{
	return m_bar->showingNarratives();
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
	else if (nar->size() > 0) {
		slide = nar->size() - 1;
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

void NarrativeControl::selectSlides(int narrative, const SelectionData &slides, bool edit)
{
	openNarrative(narrative);
	m_narrative_selection->select(narrative);
	m_slide_selection->set(slides);
	openSlide(m_slide_selection->last());
	m_bar->showSlides();
	if (edit) {
		m_app->setState(VSimApp::EDIT_CANVAS);
	}
	else {
		m_app->setState(VSimApp::EDIT_SLIDES);
	}
	emit sEditEvent();
}

void NarrativeControl::singleSelectOpenSlide()
{
	int index = getCurrentSlideIndex();
	if (index < 0) return;
	m_slide_selection->set({index});
}

NarrativeGroup * NarrativeControl::narratives() const
{
	return m_narrative_group;
}

int NarrativeControl::getCurrentNarrativeIndex() const
{
	return m_current_narrative;
}

std::set<size_t> NarrativeControl::getSelectedNarratives() const
{
	return m_narrative_selection->toUSet();
}

int NarrativeControl::getCurrentSlideIndex() const
{
	return m_current_slide;
}

SelectionData NarrativeControl::getSelectedSlides() const
{
	return m_slide_selection->data();
}

int NarrativeControl::getTopSlide() const
{
	if (m_current_narrative < 0) return -1;
	return m_slide_selection->last();
}

Narrative * NarrativeControl::getCurrentNarrative() const
{
	if (m_current_narrative < 0) return nullptr;
	return getNarrative(m_current_narrative);
}

NarrativeSlide * NarrativeControl::getCurrentSlide() const
{
	if (m_current_narrative < 0) return nullptr;
	return getSlide(m_current_narrative, m_current_slide);
}

NarrativeSlide * NarrativeControl::slideAtIndex(int index) const
{
	Narrative *nar = getCurrentNarrative();
	if (!nar) return nullptr;
	if (index < 0 || index >= nar->size()) return nullptr;
	return nar->child(index);
}

Narrative *NarrativeControl::getNarrative(int index) const
{
	if (index < 0) return nullptr;
	return m_narrative_group->child(index);
}

NarrativeSlide * NarrativeControl::getSlide(int narrative, int slide) const
{
	Narrative *nar = getNarrative(narrative);
	if (!nar) return nullptr;
	return nar->child(slide);
}

void NarrativeControl::newSlide()
{
	if (m_current_lock && m_current_lock->isLocked()) return;
	Narrative *nar = getNarrative(m_current_narrative);
	auto matrix = m_app->getCameraMatrix();

	// figure out where to insert
	int index;
	if (m_slide_selection->empty()) {
		index = nar->size();
	}
	else {
		index = m_slide_selection->last() + 1;
	}

	// make new slide, initialize matrix and stuff
	auto slide = std::shared_ptr<NarrativeSlide>(new NarrativeSlide);
	slide->setCameraMatrix(matrix);

	// perform command
	m_undo_stack->beginMacro("New Slide");
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, {}, Command::ON_UNDO));
	m_undo_stack->push(new AddNodeCommand<NarrativeSlide>(nar, slide, index));
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, { index }, Command::ON_REDO));
	m_undo_stack->endMacro();

	//std::cout << m_window->getViewer()->getCameraManipulator()->getMatrix();

	openSlide(index);
}

void NarrativeControl::deleteSlides()
{
	if (m_current_lock && m_current_lock->isLocked()) return;
	if (m_narrative_selection->empty()) {
		return;
	}
	Narrative *nar = getNarrative(m_current_narrative);

	m_undo_stack->beginMacro("Delete Slides");
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, m_slide_selection->data(), Command::ON_UNDO));
	m_undo_stack->push(new RemoveMultiCommand<NarrativeSlide>(nar, m_slide_selection->toUSet()));
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, {}, Command::ON_REDO));
	m_undo_stack->endMacro();
}

void NarrativeControl::setSlideDuration()
{
	if (m_current_lock && m_current_lock->isLocked()) return;

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
	if (m_current_lock && m_current_lock->isLocked()) return;
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
	if (m_current_lock && m_current_lock->isLocked()) return;
	auto selection = m_slide_selection->data();
	osg::Matrixd matrix = m_app->getCameraMatrix();

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
	if (m_current_lock && m_current_lock->isLocked()) return;
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
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, sfrom, Command::ON_UNDO));
	m_undo_stack->push(new MoveNodesCommand<NarrativeSlide>(narrative, mapping));
	m_undo_stack->push(new SelectSlidesCommand(this, m_current_narrative, sto, Command::ON_REDO));
	m_undo_stack->endMacro();
}

void NarrativeControl::clearAllThumbnails()
{
	for (std::shared_ptr<Narrative> nar : *m_narrative_group) {
		for (std::shared_ptr<NarrativeSlide> slide : *nar) {
			slide->setBackgroundDirty();
			slide->setForegroundDirty();
		}
	}
}

void NarrativeControl::onThumbnailReady(QImage img)
{
	auto slide = m_thumbnail_slide.lock();
	if (!slide) return;

	//QLabel *label = new QLabel(m_window);
	//label->setWindowFlags(Qt::Window);
	//label->setPixmap(QPixmap::fromImage(img));
	//label->show();

	slide->setThumbnailBackground(QPixmap::fromImage(img));
	m_thumbnail_slide.reset();
}

NarrativeControl::SelectNarrativesCommand::SelectNarrativesCommand(
	NarrativeControl *control, const SelectionData &narratives, Command::When when, QUndoCommand *parent)
	: QUndoCommand(parent),
	m_control(control),
	m_narratives(narratives),
	m_when(when)
{
}
void NarrativeControl::SelectNarrativesCommand::undo() {
	if (m_when != Command::ON_REDO) {
		m_control->selectNarratives(m_narratives);
	}
}
void NarrativeControl::SelectNarrativesCommand::redo() {
	if (m_when != Command::ON_UNDO) {
		m_control->selectNarratives(m_narratives);
	}
}

NarrativeControl::SelectSlidesCommand::SelectSlidesCommand(
	NarrativeControl *control,
	int narrative,
	const SelectionData &slides,
	Command::When when,
	bool edit,
	QUndoCommand *parent)
	: QUndoCommand(parent),
	m_control(control),
	m_narrative(narrative),
	m_slides(slides),
	m_when(when),
	m_edit(edit)
{
}

void NarrativeControl::SelectSlidesCommand::undo() {
	if (m_when != Command::ON_REDO) {
		m_control->selectSlides(m_narrative, m_slides, m_edit);
	}
	QUndoCommand::undo();
}
void NarrativeControl::SelectSlidesCommand::redo() {
	if (m_when != Command::ON_UNDO) {
		m_control->selectSlides(m_narrative, m_slides, m_edit);
	}
	QUndoCommand::redo();
}

NarrativeControl::CanvasStackWrapper::CanvasStackWrapper(
	NarrativeControl * control)
	: m_control(control)
{
}

QUndoCommand *NarrativeControl::CanvasStackWrapper::begin()
{
	m_cmd = new SelectSlidesCommand(m_control,
		m_control->getCurrentNarrativeIndex(),
		m_control->getSelectedSlides(),
		Command::ON_BOTH,
		true);

	return m_cmd;
}

void NarrativeControl::CanvasStackWrapper::end()
{
	m_control->m_undo_stack->push(m_cmd);
}
