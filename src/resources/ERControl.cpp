#include "resources/ERControl.h"
#include "resources/EResource.h"
#include "resources/EResourceGroup.h"
#include "resources/ECategory.h"
#include "resources/ECategoryGroup.h"
#include "resources/ECategoryControl.h"
#include "resources/ECategoryModel.h"
#include "resources/ERDialog.h"
#include "resources/ERScrollBox.h"
#include "resources/NewCatDialog.h"
#include "resources/ERDisplay.h"
#include "resources/ERFilterArea.h"
#include "MainWindow.h"
#include "ERBar.h"
#include "resources/ERFilterSortProxy.h"
#include "CheckableListProxy.h"
#include "VSimApp.h"
#include "SelectionStack.h"
#include "Core/GroupCommands.h"
#include "Core/Util.h"
#include "TimeManager.h"
#include "VSimRoot.h"
#include "Core/LockTable.h"
#include "Gui/PasswordDialog.h"
#include "settings_generated.h"
#include "ECategoryLegend.h"

#include <QDesktopServices>
#include <QPushButton>
#include <QDebug>
#include <QDir>

ERControl::ERControl(VSimApp *app, MainWindow *window, QObject *parent)
	: QObject(parent),
	m_app(app),
	m_window(window),
	m_ers(nullptr),
	m_categories(nullptr),
	m_filter_proxy(nullptr),
	m_global_proxy(nullptr),
	m_local_proxy(nullptr),
	m_last_touched(nullptr),
	m_radius(1.0f),
	m_enabled(true),
	m_auto_launch(true),
	m_showing_all(false)
{
	m_undo_stack = m_app->getUndoStack();
	m_bar = m_window->erBar();
	m_global_box = m_bar->ui.global;
	m_local_box = m_bar->ui.local;
	m_all_box = m_bar->ui.all;
	m_last_touched = m_local_box;
	m_display = m_window->erDisplay();
	m_filter_area = m_window->erFilterArea();
	m_legend = m_window->categoryLegend();

	m_legend_opacity = new QGraphicsOpacityEffect(this);
	m_legend_opacity->setOpacity(1.0);
	m_legend->setGraphicsEffect(m_legend_opacity);
	m_legend_in_anim = new QPropertyAnimation(m_legend_opacity, "opacity");
	m_legend_in_anim->setDuration(250);
	m_legend_in_anim->setStartValue(0.0);
	m_legend_in_anim->setEndValue(1.0);
	m_legend_out_anim = new QPropertyAnimation(m_legend_opacity, "opacity");
	m_legend_out_anim->setDuration(250);
	m_legend_out_anim->setStartValue(1.0);
	m_legend_out_anim->setEndValue(0.0);
	connect(m_legend_out_anim, &QPropertyAnimation::finished, m_legend, &QWidget::hide);

	m_filter_proxy = std::unique_ptr<ERFilterSortProxy>(new ERFilterSortProxy(nullptr));
	m_filter_proxy->sortBy(ER::SortBy::NONE);
	m_filter_proxy->enableRange(false);
	m_all_proxy = std::unique_ptr<ERFilterSortProxy>(new ERFilterSortProxy(m_filter_proxy.get()));
	m_all_proxy->showGlobal(true);
	m_all_proxy->showLocal(true);
	m_all_proxy->enableRange(false);
	m_global_proxy = std::unique_ptr<ERFilterSortProxy>(new ERFilterSortProxy(m_filter_proxy.get()));
	m_global_proxy->showGlobal(true);
	m_global_proxy->showLocal(false);
	m_global_proxy->enableRange(false);
	m_local_proxy = std::unique_ptr<ERFilterSortProxy>(new ERFilterSortProxy(m_filter_proxy.get()));
	m_local_proxy->showGlobal(false);
	m_local_proxy->showLocal(true);
	m_local_proxy->enableRange(true);

	m_category_control = new ECategoryControl(app, this);

	m_category_checkbox_model = new CheckableListProxy(this);
	m_category_checkbox_model->setSourceModel(m_category_control->categoryModel());
	m_filter_area->setCategoryModel(m_category_checkbox_model);
	m_filter_proxy->setCategories(m_category_checkbox_model);

	a_new_er = new QAction("New Resource", this);
	a_new_er->setIconText("+");
	connect(a_new_er, &QAction::triggered, this, &ERControl::newER);

	a_delete_er = new QAction("Delete Resources", this);
	a_delete_er->setIconText("-");
	connect(a_delete_er, &QAction::triggered, this, &ERControl::deleteER);

	a_edit_er = new QAction("Edit Resource", this);
	a_edit_er->setIconText("Edit");
	connect(a_edit_er, &QAction::triggered, this, &ERControl::editERInfo);

	a_open_er = new QAction("Launch Resource", this);
	a_open_er->setIconText("Launch");
	connect(a_open_er, &QAction::triggered, this, &ERControl::openTopResource);

	a_position_er = new QAction("Set Resource Positions", this);
	a_position_er->setIconText("Set Position");
	connect(a_position_er, &QAction::triggered, this, &ERControl::setPosition);

	a_goto_er = new QAction("Goto Resource", this);
	connect(a_goto_er, &QAction::triggered, this, &ERControl::gotoPosition);

	a_lock_ers = new QAction("Lock Resources", this);
	connect(a_lock_ers, &QAction::triggered, this, &ERControl::lockResources);

	a_unlock_ers = new QAction("Unlock Resources", this);
	connect(a_unlock_ers, &QAction::triggered, this, &ERControl::unlockResources);

	a_close_er = new QAction("Close Resource", this);
	connect(a_close_er, &QAction::triggered, this, &ERControl::closeER);

	a_close_all = new QAction("Close All", this);
	connect(a_close_all, &QAction::triggered, this, &ERControl::closeAll);

	load(m_app->getRoot()->resources());

	// box menus
	QList<QAction*> actions = {
		a_new_er,
		a_delete_er,
		a_edit_er,
		a_open_er,
		a_position_er,
		a_goto_er,
		a_lock_ers,
		a_unlock_ers
	};
	QMenu *box_menu = new QMenu(window->erBar());
	box_menu->addActions(actions);
	m_local_box->setMenu(box_menu);
	m_local_box->setItemMenu(box_menu);
	m_global_box->setMenu(box_menu);
	m_global_box->setItemMenu(box_menu);
	m_all_box->setMenu(box_menu);
	m_all_box->setItemMenu(box_menu);

	// ui connect
	ERBar *bar = m_window->erBar();
	// new
	bar->ui.newERButton->setDefaultAction(a_new_er);
	// delete
	bar->ui.deleteERButton->setDefaultAction(a_delete_er);
	// edit
	bar->ui.editERButton->setDefaultAction(a_edit_er);
	// show all
	connect(bar->ui.showAll, &QAbstractButton::pressed, this, [this]() {
		showAll(!m_showing_all);
	});

	// goto and open? or just open?
	auto goto_open = [this]() {
		//gotoPosition();
		EResource *res = getCombinedLastSelectedP();
		if (!res) return;
		if (res->getReposition()) gotoPosition();
		openTopResource();
	};
	connect(m_local_box, &ERScrollBox::sDoubleClick, this, goto_open);
	connect(m_global_box, &ERScrollBox::sDoubleClick, this, goto_open);
	connect(m_all_box, &ERScrollBox::sDoubleClick, this, goto_open);

	connect(m_local_box, &FastScrollBox::sTouch, this, &ERControl::onTouch);
	connect(m_global_box, &FastScrollBox::sTouch, this, &ERControl::onTouch);
	connect(m_all_box, &FastScrollBox::sTouch, this, &ERControl::onTouch);

	connect(m_local_box, &ERScrollBox::sTopChanged, this, &ERControl::onTopChange);
	connect(m_global_box, &ERScrollBox::sTopChanged, this, &ERControl::onTopChange);
	connect(m_all_box, &ERScrollBox::sTopChanged, this, &ERControl::onTopChange);

	connect(m_local_box, &ERScrollBox::sSelectionChanged, this, &ERControl::onSelectionChange);
	connect(m_global_box, &ERScrollBox::sSelectionChanged, this, &ERControl::onSelectionChange);
	connect(m_all_box, &ERScrollBox::sSelectionChanged, this, &ERControl::onSelectionChange);


	// mash the two selections together
	// if one clears everything, then clear the other one too
	connect(m_local_box, &FastScrollBox::sSelectionCleared, this, [this]() {
		m_global_box->selection()->clear();
	});
	connect(m_global_box, &FastScrollBox::sSelectionCleared, this, [this]() {
		m_local_box->selection()->clear();
	});

	connect(m_app, &VSimApp::sStateChanged, this, [this]() {
		VSimApp::State state = m_app->state();
		if (isERState()) {
			m_enabled = true;
			setDisplay(m_displayed, false); // try to redisplay
		}
		else {
			m_enabled = false;
			m_display->hide();
		}
		checkShowLegend();
	});
	connect(m_app, &VSimApp::sTick, this, &ERControl::onUpdate);
	connect(m_app, &VSimApp::sArrived, this, [this]() {
		m_going_to = false;
	});
	connect(m_app, &VSimApp::sInterrupted, this, [this]() {
		m_going_to = false;
	});
	connect(m_app, &VSimApp::sAboutToSave, this, &ERControl::gatherSettings);
	connect(m_app, &VSimApp::sReset, this, &ERControl::onReset);
	connect(m_app->timeManager(), &TimeManager::sYearChanged,
		m_filter_proxy.get(), &ERFilterSortProxy::setYear);
	connect(m_app->timeManager(), &TimeManager::sTimeEnableChanged,
		this, &ERControl::onYearsEnabledChanged);

	connect(m_display, &ERDisplay::sClose, this, &ERControl::closeER);
	connect(m_display, &ERDisplay::sCloseAll, this, &ERControl::closeAll);
	connect(m_display, &ERDisplay::sOpen, this, &ERControl::openTopResource);
	connect(m_display, &ERDisplay::sGoto, this, &ERControl::gotoPosition);

	// filter area -> this
	connect(m_filter_area, &ERFilterArea::sClear,
		this, &ERControl::resetFilters);
	connect(m_filter_area, &ERFilterArea::sSetRadius,
		this, &ERControl::setRadius);
	connect(m_filter_area, &ERFilterArea::sEnableRange,
		m_local_proxy.get(), &ERFilterSortProxy::enableRange);
	connect(m_filter_area, &ERFilterArea::sSortAll,
		m_all_proxy.get(), &ERFilterSortProxy::sortBy);
	connect(m_filter_area, &ERFilterArea::sSortLocal,
		m_local_proxy.get(), &ERFilterSortProxy::sortBy);
	connect(m_filter_area, &ERFilterArea::sSortGlobal,
		m_global_proxy.get(), &ERFilterSortProxy::sortBy);
	connect(m_filter_area, &ERFilterArea::sEnableAutoLaunch,
		this, &ERControl::enableAutoLaunch);
	connect(m_filter_area, &ERFilterArea::sSearch,
		m_filter_proxy.get(), &ERFilterSortProxy::setSearch);
	connect(m_filter_area, &ERFilterArea::sEnableYears,
		this, &ERControl::enableYears);
	connect(m_filter_area, &ERFilterArea::sShowLegend,
		this, &ERControl::showLegend);

	connect(m_window->erBar()->ui.filter, &QPushButton::pressed, this,
		[this]() {
		m_filter_area->setVisible(!m_filter_area->isVisible());
	});

	// this -> filter area
	connect(m_all_proxy.get(), &ERFilterSortProxy::sSortByChanged,
		m_filter_area, &ERFilterArea::setSortAll);
	connect(m_local_proxy.get(), &ERFilterSortProxy::sSortByChanged,
		m_filter_area, &ERFilterArea::setSortLocal);
	connect(m_global_proxy.get(), &ERFilterSortProxy::sSortByChanged,
		m_filter_area, &ERFilterArea::setSortGlobal);
	connect(m_local_proxy.get(), &ERFilterSortProxy::sUseRangeChanged,
		m_filter_area, &ERFilterArea::enableRange);
	connect(m_filter_proxy.get(), &ERFilterSortProxy::sSearchChanged,
		m_filter_area, &ERFilterArea::setSearch);

	resetFilters();
	showAll(false);
}

void ERControl::load(EResourceGroup *ers)
{
	setDisplay(nullptr);
	if (ers == nullptr) {
		m_ers = nullptr;
		m_categories = nullptr;
		m_category_control->load(nullptr);
		return;
	}
	m_ers = ers;
	m_categories = ers->categories();

	m_filter_proxy->setBase(ers);

	m_all_box->setGroup(m_all_proxy.get());
	m_local_box->setGroup(m_local_proxy.get());
	m_global_box->setGroup(m_global_proxy.get());

	m_category_control->load(m_categories);
	m_legend->setCategoryGroup(m_categories);

	connect(ers, &EResourceGroup::sRestrictToCurrentChanged, this, &ERControl::onRestrictToCurrent);
	connect(m_categories, &ECategoryGroup::sAnyChange, this, &ERControl::checkShowLegend);
}

void ERControl::newER()
{
	if (m_ers->restrictedToCurrent()) return;
	// start with global selected if we picked global
	QWidget *focus = QApplication::focusWidget();
	bool start_global = Util::isDescendant(m_global_box, focus);

	ERDialog dlg(m_category_control, m_app->getCurrentDirectory());
	dlg.setGlobal(start_global);

	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}
	qInfo() << "Command - New Embedded Resource";
	m_undo_stack->beginMacro("New Resource");

	auto resource = std::shared_ptr<EResource>(new EResource);
	resource->setResourceName(dlg.getTitle());
	resource->setAuthor(dlg.getAuthor());
	resource->setResourceDescription(dlg.getDescription());
	resource->setResourcePath(dlg.getPath());
	resource->setGlobal(dlg.getGlobal());
	resource->setCopyright(dlg.getCopyright());
	resource->setMinYear(dlg.getMinYear());
	resource->setMaxYear(dlg.getMaxYear());
	resource->setReposition(dlg.getReposition());
	resource->setAutoLaunch(dlg.getAutoLaunch());
	resource->setLocalRange(dlg.getLocalRange());
	resource->setERType(dlg.getERType());
	resource->setCameraMatrix(m_app->getCameraMatrix());
	resource->setCategory(dlg.categoryShared());

	m_undo_stack->push(new SelectERCommand(this, { resource.get() }, Command::ON_UNDO));
	m_undo_stack->push(new AddNodeCommand<EResource>(m_ers, resource));
	m_undo_stack->push(new SelectERCommand(this, { resource.get() }, Command::ON_REDO));
	m_undo_stack->endMacro();
}

void ERControl::deleteER()
{
	if (m_ers->restrictedToCurrent()) return;
	std::set<size_t> selection = getCombinedSelection();
	if (selection.empty()) return;

	std::vector<EResource*> resources;
	for (size_t i : selection) resources.push_back(m_ers->getResource(i));

	m_undo_stack->beginMacro("Delete Resources");
	m_undo_stack->push(new SelectERCommand(this, getCombinedSelectionP(), Command::ON_UNDO));
	// remove resources
	m_undo_stack->push(new RemoveMultiCommand<EResource>(m_ers, selection));
	for (auto *res : resources) { // null old categories so they get restored later
		m_undo_stack->push(new EResource::SetCategoryCommand(res, nullptr));
	}
	m_undo_stack->push(new SelectERCommand(this, {}, Command::ON_REDO));
	m_undo_stack->endMacro();
}

void ERControl::editERInfo()
{
	EResource *resource = getCombinedLastSelectedP();
	if (resource == nullptr) {
		qWarning() << "resource list - can't edit with no selection";
		return;
	}

	// locked? do read only version
	bool locked = resource->lockTableConst()->isLocked();

	ERDialog dlg(m_category_control, m_app->getCurrentDirectory());
	dlg.init(resource);
	if (!locked) {
		dlg.showPositionButton();
		connect(&dlg, &ERDialog::sSetPosition, this, &ERControl::setPosition);
	}
	dlg.enableEditingCategories(!m_ers->restrictedToCurrent());

	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		qInfo() << "resource list - cancelled edit on" << resource;
		return;
	}
	if (locked) {
		return;
	}

	m_undo_stack->beginMacro("Set Resource Info");

	int index = m_ers->indexOf(resource);

	auto cmd = new EditCommand(m_ers, { (size_t)index });

	if (resource->getResourceName() != dlg.getTitle())
		new EResource::SetResourceNameCommand(resource, dlg.getTitle(), cmd);
	if (resource->getAuthor() != dlg.getAuthor())
		new EResource::SetResourceAuthorCommand(resource, dlg.getAuthor(), cmd);
	if (resource->getResourceDescription() != dlg.getDescription())
		new EResource::SetResourceDescriptionCommand(resource, dlg.getDescription(), cmd);
	if (resource->getResourcePath() != dlg.getPath())
		new EResource::SetResourcePathCommand(resource, dlg.getPath(), cmd);
	if (resource->getGlobal() != dlg.getGlobal())
		new EResource::SetGlobalCommand(resource, dlg.getGlobal(), cmd);
	if (resource->getCopyright() != dlg.getCopyright())
		new EResource::SetCopyrightCommand(resource, dlg.getCopyright(), cmd);
	if (resource->getMinYear() != dlg.getMinYear())
		new EResource::SetMinYearCommand(resource, dlg.getMinYear(), cmd);
	if (resource->getMaxYear() != dlg.getMaxYear())
		new EResource::SetMaxYearCommand(resource, dlg.getMaxYear(), cmd);
	if (resource->getReposition() != dlg.getReposition())
		new EResource::SetRepositionCommand(resource, dlg.getReposition(), cmd);
	if (resource->getAutoLaunch() != dlg.getAutoLaunch())
		new EResource::SetAutoLaunchCommand(resource, dlg.getAutoLaunch(), cmd);
	if (resource->getLocalRange() != dlg.getLocalRange())
		new EResource::SetLocalRangeCommand(resource, dlg.getLocalRange(), cmd);
	if (resource->getERType() != dlg.getERType())
		new EResource::SetErTypeCommand(resource, dlg.getERType(), cmd);
	if (resource->category() != dlg.getCategory())
		new EResource::SetCategoryCommand(resource, dlg.categoryShared(), cmd);

	m_undo_stack->push(new SelectERCommand(this, { resource }));
	m_undo_stack->push(cmd);

	m_undo_stack->endMacro();
}

void ERControl::openTopResource()
{
	EResource *res = getCombinedLastSelectedP();
	if (!res) return;
	//gotoPosition();
	launchResource(res);
}

void ERControl::launchResource(const EResource *res) {
	if (!res) return;
	auto type = res->getERType();
	if (type == EResource::FILE) {
		QString path = res->getResourcePath().c_str();
		QString abs = Util::resolvePath(path, m_app->getCurrentDirectory());

		qInfo() << "Attempting to open file:" << abs;
		QDesktopServices::openUrl(QUrl::fromLocalFile(abs));
	}
	else if (type == EResource::URL) {
		qInfo() << "Attempting to open url:" << res->getResourcePath().c_str();
		QDesktopServices::openUrl(QUrl(res->getResourcePath().c_str()));
	}
	else if (type == EResource::ANNOTATION) {
	}
}

//void ERControl::showResource()
//{
//	//m_display->setInfo(res);
//	//m_display->show();
//	//m_app->setCameraMatrixSmooth(res->getCameraMatrix(), .3);
//	//m_app->setCameraMatrix(res->getCameraMatrix());
//}

void ERControl::setPosition()
{
	EResource *resource = getCombinedLastSelectedP();
	if (resource == nullptr) {
		qWarning() << "Can't set ER position - no selection";
		return;
	}

	auto indices = getCombinedSelection();
	auto resources = getCombinedSelectionP();
	auto cam = m_app->getCameraMatrix();

	int count = 0;
	auto cmd = new EditCommand(m_ers, indices);
	for (auto *res : resources) {
		if (res->lockTableConst()->isLocked()) continue;
		count++;
		new EResource::SetCameraMatrixCommand(res, cam, cmd);
	}
	if (count == 0) return;

	m_undo_stack->beginMacro("Set Resource Position(s)");
	m_undo_stack->push(cmd);
	m_undo_stack->push(new SelectERCommand(this, resources));
	m_undo_stack->endMacro();
}

void ERControl::mergeERs(const EResourceGroup *ers)
{
	qInfo() << "Merging ER Groups";
	std::vector<EResource*> res;
	for (size_t i = 0; i < ers->size(); i++) {
		res.push_back(ers->getResource(i));
	}

	auto *cmd = new QUndoCommand("Import Resources");
	new SelectERCommand(this, {}, Command::ON_UNDO, cmd);
	EResourceGroup::mergeCommand(m_ers, ers, cmd);
	new SelectERCommand(this, res, Command::ON_REDO, cmd);

	m_undo_stack->push(cmd);
}

void ERControl::gotoPosition()
{
	EResource *resource = getCombinedLastSelectedP();
	if (resource == nullptr) {
		qWarning() << "Can't goto ER position - no selection";
		return;
	}
	setDisplay(resource, true);
}

void ERControl::lockResources()
{
	auto selected = getCombinedSelectionP();

	// which can be locked?
	std::vector<LockTable*> lock_me;
	for (auto *res : selected) {
		auto *table = res->lockTable();
		if (!table->isLocked()) {
			lock_me.push_back(table);
		}
	}

	if (lock_me.size() == 0) return;

	// warning dialog
	auto btn = QMessageBox::warning(m_window, "Lock Resources",
		"Are you sure you want to lock these resources? A resource locked without a password cannot be unlocked.",
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
	for (auto *res : selected) {
		auto *lock = res->lockTable();
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
			"Failed to lock %d selected resources - were already locked.",
			bad_count);
		QMessageBox::warning(m_window, "Lock Resources", msg);
	}

	if (good_count > 0) {
		m_undo_stack->clear();
	}
	onSelectionChange();
}

void ERControl::unlockResources()
{
	std::vector<std::vector<EResource*>> clumps;

	std::vector<EResource*> selected = getCombinedSelectionP();
	std::unordered_set<EResource*> checked;
	int nperm = 0;
	int nskipped = 0;
	int nunlocked = 0;

	for (size_t i = 0; i < selected.size(); i++) {
		EResource *res = selected[i];
		if (checked.find(res) != checked.end()) continue;
		checked.insert(res);

		std::vector<EResource*> clump;
		const LockTable *lt = res->lockTable();

		if (lt->isPermanent()) {
			nperm++;
			continue;
		}
		if (!lt->isLocked()) {
			continue;
		}

		HashLock hash = lt->hash();
		clump.push_back(res);

		for (size_t j = i + 1; j < selected.size(); j++) {
			EResource *res2 = selected[j];
			const LockTable *lt2 = res2->lockTable();

			if (lt2->hasPassword() && hash == lt2->hash()) {
				checked.insert(res2);
				clump.push_back(res2);
			}

			// don't check off if it has a different password
		}
		clumps.push_back(clump);
	}

	// unlock the clumps
	for (int i = 0; i < clumps.size(); i++) {
		auto &clump = clumps[i];

		// get the first narrative title
		if (clump.size() < 0) return;
		EResource *first = clump[0];
		int count = (int)clump.size();
		QString title = QString::fromStdString(first->getResourceName());

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
		for (EResource *res : clump) {
			locks.push_back(res->lockTable());
		}

		TestPasswordDialog dlg;
		dlg.setWindowTitle("Unlock Resources");
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

	onSelectionChange();
}

void ERControl::setDisplay(EResource *res, bool go, bool fade)
{
	bool was_hidden = !m_display->isVisible();
	m_displayed = res;

	// info
	m_display->setInfo(res);

	// display
	bool is_null = (res == nullptr);
	if (!m_enabled) {
		m_display->setHardVisible(false);
	}
	else if (fade) {
		if (is_null) {
			// extra check for random fade outs
			if (!was_hidden) m_display->fadeOut();
		}
		else {
			// an extra check to prevent double fadeins
			if (m_display->canFadeIn()) m_display->fadeIn();
		}
	}
	else {
		m_display->setHardVisible(!is_null);
	}

	// goto
	if (go && !is_null) {
		m_going_to = true;
		m_app->setCameraMatrixSmooth(res->getCameraMatrix(), .6);
	}
}

void ERControl::closeER()
{
	// closes the current er

	// -> deselection
	// -> next in line pops up

	m_local_box->deselect(m_displayed);
	m_global_box->deselect(m_displayed);
	m_all_box->deselect(m_displayed);
}

void ERControl::closeAll()
{
	m_local_box->setSelection({});
	m_global_box->setSelection({});
	m_all_box->setSelection({});
}

void ERControl::showAll(bool all)
{
	m_showing_all = all;
	m_bar->ui.showAll->setText(all ? "Show Local/Global" : "Show All");
	m_bar->ui.stackedWidget->setCurrentIndex(all ? 0 : 1);
	m_filter_area->setToAll(all);
}

bool ERControl::showingAll() const
{
	return m_bar->ui.stackedWidget->currentIndex() == 0;
}

bool ERControl::isERState() const
{
	return m_app->isFlying()
		|| m_app->state() == VSimApp::EDIT_ERS;
}

void ERControl::onTouch()
{
	QObject *sender = QObject::sender();
	if (sender == m_local_box) {
		m_last_touched = m_local_box;
	}
	else if (sender == m_global_box) {
		m_last_touched = m_global_box;
	}
	else if (sender == m_all_box) {
		m_last_touched = m_all_box;
	}

	EResource *resource = getCombinedLastSelectedP();
	if (resource) {
		m_app->setState(VSimApp::EDIT_ERS);
		setDisplay(resource, resource->getReposition(), true);
	}
	else {
		setDisplay(nullptr);
		m_app->setState(VSimApp::EDIT_FLYING);
	}
}

void ERControl::onTopChange()
{
	// this happens when moving around
	// ex. moving out of range

	// we want to change the display but not goto
	EResource *res = getCombinedLastSelectedP();
	if (res != m_displayed) {
		setDisplay(res, false, true);
	}

	// TODO: change "edit" to "info"
}

void ERControl::onSelectionChange()
{
	auto s = getCombinedSelectionP();
	m_display->setCount(s.size());

	// enable/disabled lock/unlock
	bool can_lock = false; // aka any_unlocked
	bool can_unlock = false;
	for (auto *res : s) {
		auto *lt = res->lockTableConst();
		if (lt->hasPassword()) {
			can_unlock = true;
		}
		if (!lt->isLocked()) {
			can_lock = true;
		}
	}
	a_lock_ers->setEnabled(can_lock);
	a_unlock_ers->setEnabled(can_unlock);

	// enable/disabled set res position
	a_position_er->setEnabled(can_lock);

}

void ERControl::addToSelection(EResource * res, bool top)
{
	m_global_box->addToSelection(res, top);
	m_local_box->addToSelection(res, top);
	m_all_box->addToSelection(res, top);
}

void ERControl::selectERs(const std::vector<EResource*> &res)
{
	m_global_box->setSelection(res);
	m_local_box->setSelection(res);
	m_all_box->setSelection(res);

	m_app->setState(VSimApp::EDIT_ERS);
	setDisplay(getCombinedLastSelectedP(), true, false);
}

void ERControl::resetFilters()
{
	// load based on the default settings
	VSim::FlatBuffers::ERSettingsT settings;
	loadFilterSettings(settings);

	m_category_checkbox_model->setCheckAll(true);
}

void ERControl::setRadius(float radius)
{
	m_radius = radius;
	m_filter_area->setRadius(radius);
}

void ERControl::enableAutoLaunch(bool enable)
{
	if (m_auto_launch == enable) return;
	m_auto_launch = enable;
	m_filter_area->enableAutoLaunch(enable);
}

void ERControl::enableYears(bool enable)
{
	m_filter_area->enableYears(enable);
	m_years_enabled = enable;
	onYearsEnabledChanged();
}

void ERControl::onUpdate()
{
	QElapsedTimer timer;
	timer.start();

	// update all positions
	const osg::Vec3 &pos = m_app->getPosition();

	std::set<size_t> change_list;
	std::set<EResource*> trigger_list;
	for (size_t i = 0; i < m_ers->size(); i++) {
		EResource *res = m_ers->getResource(i);
		// update resource distance
		osg::Vec3 res_pos = res->getCameraMatrix().getTrans();
		res->setDistanceTo((pos - res_pos).length());

		bool overlap = Util::spheresOverlap(pos, m_radius, res_pos, res->getLocalRange());
		bool changed = (res->inRange() != overlap);
		bool do_popup = !res->getGlobal()
			&& (res->getAutoLaunch() != EResource::AutoLaunch::OFF);
		res->setInRange(overlap);
		if (changed) {
			change_list.insert(i);
		}
		if (changed && overlap && do_popup) {
			trigger_list.insert(res);
		}
		if (changed && !overlap) {
		}
	}
	// sort by distance
	m_local_proxy->positionChangePoke();
	m_global_proxy->positionChangePoke();
	m_filter_proxy->positionChangePoke();

	// send updates to proxies
	m_ers->sEdited(change_list);

	// add to selection if possible
	for (auto *res : trigger_list) {
		if (!isSelectable(res)) continue;

		// if we're going somewhere then queue (want to keep target on top)
		// if just moving around then stack (want to change target)
		addToSelection(res, !m_going_to);

		if (res->getAutoLaunch() == EResource::ON && m_auto_launch) {
			// try to open this thing
			launchResource(res);
		}
	}

	m_update_time_sec = timer.nsecsElapsed() / 1.0e9;
}

QString ERControl::debugString()
{
	return QString().sprintf("frame ms: %.2f", m_update_time_sec * 1000);
}

void ERControl::debug()
{
	qInfo() << "Debugging ERControl";
	qInfo() << "local proxy - ";
	m_local_proxy->debug();
	qInfo() << "global proxy - ";
	m_global_proxy->debug();

	qInfo() << "Categories";
	for (unsigned int i = 0; i < m_categories->size(); i++) {
		ECategory *cat = m_categories->category(i);
		qInfo() << i << ":" << "node" << (void*)m_categories->child(i) <<
			"cat" << (void*)cat << QString::fromStdString(cat->getCategoryName()) << cat->getColor();
	}
}

void ERControl::gatherSettings()
{
	if (m_app->getRoot()->settingsLocked()) return;
	auto &es = m_app->getRoot()->erSettings();

	es.sort_all = static_cast<VSim::FlatBuffers::ERSortBy>(m_all_proxy->getSortBy());
	es.sort_global = static_cast<VSim::FlatBuffers::ERSortBy>(m_global_proxy->getSortBy());
	es.sort_local = static_cast<VSim::FlatBuffers::ERSortBy>(m_local_proxy->getSortBy());
	es.show_all = m_showing_all;

	es.range_enabled = m_local_proxy->rangeEnabled();
	es.years_enabled = m_years_enabled;
	es.auto_launch = m_auto_launch;
	es.local_radius = m_radius;

	es.categories = m_category_checkbox_model->getChecked();
	es.show_legend = m_show_legend;
}

void ERControl::extractSettings()
{
	auto &es = m_app->getRoot()->erSettings();
	loadFilterSettings(es);
	showAll(es.show_all);
	showLegend(es.show_legend);
}

void ERControl::loadFilterSettings(VSim::FlatBuffers::ERSettingsT &es)
{
	m_all_proxy->sortBy(static_cast<ER::SortBy>(es.sort_all));
	m_local_proxy->sortBy(static_cast<ER::SortBy>(es.sort_local));
	m_global_proxy->sortBy(static_cast<ER::SortBy>(es.sort_global));
	m_local_proxy->enableRange(es.range_enabled);

	enableYears(es.years_enabled);
	enableAutoLaunch(es.auto_launch);
	setRadius(es.local_radius);

	for (size_t i = 0; i < es.categories.size(); i++) {
		m_category_checkbox_model->setChecked(i, es.categories[i]);
	}
}

void ERControl::onReset()
{
	onRestrictToCurrent();
	extractSettings();
	checkShowLegend();
}

void ERControl::onYearsEnabledChanged()
{
	m_filter_proxy->setYearsEnabled(m_years_enabled
		&& m_app->timeManager()->timeEnabled());
}

void ERControl::showLegend(bool show)
{
	m_show_legend = show;
	m_filter_area->setShowLegend(show);
	checkShowLegend();
}

void ERControl::checkShowLegend()
{
	// er/flying state + show_legend + has categories
	bool show = isERState()
		&& m_show_legend
		&& m_categories->size() > 0;

	// show
	if (show) {
		m_legend_out_anim->stop();
		m_legend->show();

		if (m_legend_in_anim->state() == QPropertyAnimation::State::Stopped
			&& m_legend_opacity->opacity() < 1.0) {
			m_legend_in_anim->start();
		}
	}
	// hide
	else {
		m_legend_in_anim->stop();
		// only play if visible, not playing
		if (m_legend->isVisible() &&
			m_legend_out_anim->state() == QPropertyAnimation::State::Stopped) {
			m_legend->show();
			m_legend_out_anim->start();
		}
	}
}

void ERControl::onRestrictToCurrent()
{
	bool restrict = m_ers->restrictedToCurrent();
	bool enable = !restrict;
	a_new_er->setEnabled(enable);
	a_delete_er->setEnabled(enable);
	a_position_er->setEnabled(enable);

	m_bar->ui.newERButton->setVisible(enable);
	m_bar->ui.deleteERButton->setVisible(enable);
}

std::set<size_t> ERControl::getCombinedSelection() const
{
	auto stack = getCombinedSelectionP();

	std::set<size_t> indices;
	for (EResource *res : stack) {
		indices.insert(res->index());
	}
	return indices;
}

std::vector<EResource*> ERControl::getCombinedSelectionP() const
{
	std::vector<EResource*> out;

	if (showingAll()) {
		out = m_all_box->getSelection();
	}
	else {
		auto ls = m_local_box->getSelection();
		auto gs = m_global_box->getSelection();
		decltype(ls) *first, *second;

		// tape the two stacks together,
		// the last touched one on top
		if (m_last_touched == m_global_box) {
			first = &ls;
			second = &gs;
		}
		else {
			first = &gs;
			second = &ls;
		}

		out.insert(out.end(), first->begin(), first->end());
		out.insert(out.end(), second->begin(), second->end());
	}

	return out;
}

EResource *ERControl::getCombinedLastSelectedP() const
{
	auto sel = getCombinedSelectionP();

	if (sel.size() > 0) {
		return *sel.rbegin();
	}
	
	return nullptr;
}

bool ERControl::isSelectable(EResource *res) const
{
	if (showingAll()) {
		return m_all_box->has(res);
	}
	return m_global_box->has(res) || m_local_box->has(res);
}

void ERControl::clearSelection()
{
	m_global_box->setSelection({});
	m_local_box->setSelection({});
}

SelectERCommand::SelectERCommand(ERControl *control,
	const std::vector<EResource*> &resources,
	Command::When when,
	QUndoCommand *parent)
	: QUndoCommand(parent),
	m_control(control),
	m_resources(resources),
	m_when(when)
{
}
void SelectERCommand::undo()
{
	if (!(m_when & Command::ON_UNDO)) return;
	m_control->selectERs(m_resources);
}
void SelectERCommand::redo()
{
	if (!(m_when & Command::ON_REDO)) return;
	m_control->selectERs(m_resources);
}