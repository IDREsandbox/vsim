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
#include "GroupCommands.h"
#include "Util.h"

#include <QDesktopServices>
#include <QPushButton>
#include <QDebug>

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
	m_enabled(true)
{
	m_undo_stack = m_app->getUndoStack();
	m_global_box = m_window->erBar()->ui.global;
	m_local_box = m_window->erBar()->ui.local;
	m_last_touched = m_local_box;
	m_display = m_window->erDisplay();
	m_filter_area = m_window->erFilterArea();

	m_filter_proxy = std::unique_ptr<ERFilterSortProxy>(new ERFilterSortProxy(nullptr));
	m_filter_proxy->sortBy(ER::SortBy::TITLE);
	m_filter_proxy->enableRange(false);
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

	a_open_er = new QAction("Open Resource", this);
	a_open_er->setIconText("Open");
	connect(a_open_er, &QAction::triggered, this, &ERControl::openResource);

	a_position_er = new QAction("Set Resource Position", this);
	a_position_er->setIconText("Set Position");
	connect(a_position_er, &QAction::triggered, this, &ERControl::setPosition);

	a_goto_er = new QAction("Goto Resource", this);
	connect(a_goto_er, &QAction::triggered, this, &ERControl::gotoPosition);

	a_close_er = new QAction("Close Resource", this);
	connect(a_close_er, &QAction::triggered, this, &ERControl::closeER);

	a_close_all = new QAction("Close All", this);
	connect(a_close_all, &QAction::triggered, this, &ERControl::closeAll);

	// box menus
	QList<QAction*> actions = {
		a_new_er,
		a_delete_er,
		a_edit_er,
		a_open_er,
		a_position_er,
		a_goto_er
	};
	QMenu *local_menu = new QMenu(m_local_box);
	local_menu->addActions(actions);
	m_local_box->setMenu(local_menu);
	m_local_box->setItemMenu(local_menu);
	QMenu *global_menu = new QMenu(m_global_box);
	global_menu->addActions(actions);
	m_global_box->setMenu(global_menu);
	m_global_box->setItemMenu(global_menu);

	// ui connect
	ERBar *bar = m_window->erBar();
	// new
	bar->ui.newERButton->setDefaultAction(a_new_er);
	// delete
	bar->ui.deleteERButton->setDefaultAction(a_delete_er);
	// edit
	bar->ui.editERButton->setDefaultAction(a_edit_er);
	// open
	connect(m_local_box, &ERScrollBox::sOpen, a_open_er, &QAction::trigger);
	connect(m_global_box, &ERScrollBox::sOpen, a_open_er, &QAction::trigger);

	connect(m_local_box, &FastScrollBox::sTouch, this, &ERControl::onTouch);
	connect(m_global_box, &FastScrollBox::sTouch, this, &ERControl::onTouch);

	connect(m_local_box, &ERScrollBox::sTopChanged, this, &ERControl::onTopChange);
	connect(m_global_box, &ERScrollBox::sTopChanged, this, &ERControl::onTopChange);

	connect(m_local_box, &ERScrollBox::sSelectionChanged, this, &ERControl::onSelectionChange);
	connect(m_global_box, &ERScrollBox::sSelectionChanged, this, &ERControl::onSelectionChange);


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
		bool ok = m_app->isFlying()
			|| state == VSimApp::EDIT_ERS;
		if (ok) {
			m_enabled = true;
			setDisplay(m_displayed, false); // try to redisplay
		}
		else {
			m_enabled = false;
			m_display->hide();
		}
	});

	connect(m_app, &VSimApp::sArrived, this, [this]() {
		m_going_to = false;
	});
	connect(m_app, &VSimApp::sInterrupted, this, [this]() {
		m_going_to = false;
	});

	connect(m_display, &ERDisplay::sClose, this, &ERControl::closeER);
	connect(m_display, &ERDisplay::sCloseAll, this, &ERControl::closeAll);
	connect(m_display, &ERDisplay::sOpen, this, &ERControl::openResource);
	connect(m_display, &ERDisplay::sGoto, this, &ERControl::gotoPosition);

	// filter area -> this
	connect(m_filter_area, &ERFilterArea::sClear,
		this, &ERControl::resetFilters);
	connect(m_filter_area, &ERFilterArea::sSetRadius,
		this, &ERControl::setRadius);
	connect(m_filter_area, &ERFilterArea::sEnableRange,
		m_local_proxy.get(), &ERFilterSortProxy::enableRange);
	connect(m_filter_area, &ERFilterArea::sEnableYears,
		m_filter_proxy.get(), &ERFilterSortProxy::enableYears);
	connect(m_filter_area, &ERFilterArea::sSortLocal,
		m_local_proxy.get(), &ERFilterSortProxy::sortBy);
	connect(m_filter_area, &ERFilterArea::sSortGlobal,
		m_global_proxy.get(), &ERFilterSortProxy::sortBy);

	connect(m_window->erBar()->ui.filter, &QPushButton::pressed, this,
		[this]() {
		m_filter_area->setVisible(!m_filter_area->isVisible());
	});

	// this -> filter area
	connect(m_local_proxy.get(), &ERFilterSortProxy::sSortByChanged,
		m_filter_area, &ERFilterArea::setSortLocal);
	connect(m_global_proxy.get(), &ERFilterSortProxy::sSortByChanged,
		m_filter_area, &ERFilterArea::setSortGlobal);
	connect(this, &ERControl::sRadiusChanged,
		m_filter_area, &ERFilterArea::setRadius);
	connect(m_filter_proxy.get(), &ERFilterSortProxy::sUseYearsChanged,
		m_filter_area, &ERFilterArea::enableYears);
	connect(m_local_proxy.get(), &ERFilterSortProxy::sUseRangeChanged,
		m_filter_area, &ERFilterArea::enableRange);

	resetFilters();

	load(nullptr);
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

	m_global_box->setGroup(m_global_proxy.get());
	m_local_box->setGroup(m_local_proxy.get());

	m_category_control->load(m_categories);

	m_filter_area->reset();
}

void ERControl::update(double dt_sec)
{
	// update all positions
	osg::Vec3 pos = m_app->getPosition();
	
	std::set<size_t> change_list;
	std::set<EResource*> trigger_list;
	for (size_t i = 0; i < m_ers->size(); i++) {
		EResource *res = m_ers->getResource(i);
		// update resource distance
		osg::Vec3 res_pos = res->getCameraMatrix().getTrans();
		res->setDistanceTo((pos - res_pos).length());

		bool overlap = Util::spheresOverlap(pos, m_radius, res_pos, res->getLocalRange());
		bool changed = res->setInRange(overlap);
		if (changed) {
			change_list.insert(i);
		}
		if (changed && overlap && res->getAutoLaunch()) {
			trigger_list.insert(res);
			//qDebug() << "in range" << res;
		}
		if (changed && !overlap) {
			//qDebug() << "out of range" << res;
		}
	}
	m_ers->sEdited(change_list);
	// ... proxy stuff propagates to scroll boxes

	// check the other end
	for (auto *res : trigger_list) {
		if (isSelectable(res)) {
			//qDebug() << "triggered resource" << res;
			// if we're going somewhere then queue
			// if just moving around then stack
			// confusing stuff
			addToSelection(res, !m_going_to);
		}
	}

	m_local_proxy->positionChangePoke();

	//m_local_proxy->setPosition(pos);

	//m_prev_position = pos;
}

void ERControl::newER()
{
	ERDialog dlg(m_category_control, m_app->getCurrentDirectory().c_str());

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

	ERDialog dlg(m_category_control, m_app->getCurrentDirectory().c_str());

	dlg.init(resource);
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		qInfo() << "resource list - cancelled edit on" << resource;
		return;
	}

	//ECategory* category = getCategory(dlg.getCategory());

	m_undo_stack->beginMacro("Set Resource Info");
	//m_undo_stack->push(new SelectResourcesCommand(this, { active_item }));

	int index = m_ers->indexOf(resource);

	auto cmd = new EditCommand<EResource>(m_ers, { (size_t)index });

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

	//m_undo_stack->push(new EResource::SetCameraMatrixCommand(resource, m_window->getViewerWidget()->getCameraMatrix()));
	m_undo_stack->push(new SelectERCommand(this, { resource }));
	m_undo_stack->push(cmd);

	m_undo_stack->endMacro();
}

void ERControl::openResource()
{
	EResource *res = getCombinedLastSelectedP();
	if (!res) return;
	auto type = res->getERType();
	gotoPosition();
	if (type == EResource::FILE) {
		QString path = QString::fromStdString(m_app->getCurrentDirectory() + "/" + res->getResourcePath());
		qInfo() << "Attempting to open file:" << path;
		QDesktopServices::openUrl(QUrl(path));
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
	m_undo_stack->beginMacro("Set Resource Info");
	m_undo_stack->push(new EResource::SetCameraMatrixCommand(resource, m_app->getCameraMatrix()));
	m_undo_stack->push(new SelectERCommand(this, { resource }));
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

void ERControl::setDisplay(EResource *res, bool go)
{
	m_displayed = res;
	if (res == nullptr) {
		m_display->setInfo(nullptr);
		m_display->hide();
		return;
	}

	m_display->setInfo(res);

	if (m_enabled) {
		m_display->show();
	}

	if (go) {
		m_going_to = true;
		m_app->setCameraMatrixSmooth(res->getCameraMatrix(), .3);
	}
}

void ERControl::closeER()
{
	// closes the current er

	// -> deselection
	// -> next in line pops up

	m_local_box->deselect(m_displayed);
	m_global_box->deselect(m_displayed);
}

void ERControl::closeAll()
{
	m_local_box->setSelection({});
	m_global_box->setSelection({});
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

	EResource *resource = getCombinedLastSelectedP();
	if (resource) {
		m_app->setState(VSimApp::EDIT_ERS);
		setDisplay(resource, resource->getReposition());
	}
	else {
		setDisplay(nullptr);
		m_app->setState(VSimApp::EDIT_FLYING);
	}
}

void ERControl::onTopChange()
{
	// this stuff happens when moving around
	// and there is no er touching

	// null out the current thing?
	//if (!isSelectable(m_displayed)) {
	//	setDisplay(nullptr);
	//}

	// only go here if
	EResource *res = getCombinedLastSelectedP();
	if (res != m_displayed) {
		setDisplay(res, false);
	}
	//EResource *res = getCombinedLastSelectedP();
	//bool go = (res && res->getReposition());
	//setDisplay(res, go);
}

void ERControl::onSelectionChange()
{
	auto s = getCombinedSelectionP();
	m_display->setCount(s.size());
}

void ERControl::addToSelection(EResource * res, bool top)
{
	m_global_box->addToSelection(res, top);
	m_local_box->addToSelection(res, top);
}

void ERControl::selectERs(const std::vector<EResource*> &res)
{
	m_global_box->setSelection(res);
	m_local_box->setSelection(res);

	m_app->setState(VSimApp::EDIT_ERS);
	setDisplay(getCombinedLastSelectedP());
}

void ERControl::resetFilters()
{
	m_local_proxy->enableRange(true);
	m_filter_proxy->sortBy(ER::SortBy::NONE);
	m_local_proxy->sortBy(ER::SortBy::DISTANCE);
	m_global_proxy->sortBy(ER::SortBy::TITLE);
	m_filter_proxy->setTitleSearch("");
	m_filter_proxy->showGlobal(true);
	m_filter_proxy->showLocal(true);
	setRadius(5.0f);
	if (m_category_checkbox_model) m_category_checkbox_model->setCheckAll(true);
}

void ERControl::setRadius(float radius)
{
	m_radius = radius;
	emit sRadiusChanged(radius);
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

std::set<size_t> ERControl::getCombinedSelection() const
{
	// remap selection to nodes
	std::set<EResource*> nodes;

	auto ls = m_local_box->getSelection();
	auto gs = m_global_box->getSelection();
	nodes.insert(ls.begin(), ls.end());
	nodes.insert(gs.begin(), gs.end());

	// map nodes to indices
	std::set<size_t> indices;
	for (size_t i = 0; i < m_ers->size(); i++) {
		if (nodes.count(m_ers->child(i)) > 0) {
			indices.insert(i);
		}
	}
	return indices;
}

std::vector<EResource*> ERControl::getCombinedSelectionP() const
{
	std::vector<EResource*> out;

	auto ls = m_local_box->getSelection();
	auto gs = m_global_box->getSelection();
	decltype(ls) *first, *second;

	if (m_last_touched == m_global_box) {
		// insert global on top
		first = &ls;
		second = &gs;
	}
	else {
		first = &gs;
		second = &ls;
	}

	out.insert(out.end(), first->begin(), first->end());
	out.insert(out.end(), second->begin(), second->end());

	return out;
}

int ERControl::getCombinedLastSelected() const
{
	//QObject *sender = QObject::sender();
	//ERFilterSortProxy *proxy;
	//int last;
	//int local_last = m_local_selection->last();
	//int global_last = m_global_selection->last();
	//// use the sender to determine the last selected
	//if ((sender == m_local_box 
	//	|| sender == m_local_box->selectionStack())
	//	&& local_last != -1) {
	//	last = local_last;
	//	proxy = m_local_proxy.get();
	//}
	//else if ((sender == m_global_box
	//	|| sender == m_global_box->selectionStack())
	//	&& global_last != -1) {
	//	last = global_last;
	//	proxy = m_global_proxy.get();
	//}
	//else {
	//	// just take whatever is selected
	//	if (local_last != -1) {
	//		last = local_last;
	//		proxy = m_local_proxy.get();
	//	}
	//	else {
	//		last = global_last;
	//		proxy = m_global_proxy.get();
	//	}
	//}

	//// convert into original index
	//if (last < 0) return -1;
	//EResource *node = proxy->child(last);
	//int last_base = m_ers->indexOf(node);

	return -1;
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