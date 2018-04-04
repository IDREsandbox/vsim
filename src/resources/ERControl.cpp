#include "resources/ERControl.h"
#include "resources/EResource.h"
#include "resources/EResourceGroup.h"
#include "resources/ECategory.h"
#include "resources/ECategoryGroup.h"
#include "resources/ECategoryControl.h"
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

#include <QDesktopServices>

ERControl::ERControl(VSimApp *app, MainWindow *window, EResourceGroup *ers, QObject *parent)
	: QObject(parent),
	m_app(app),
	m_window(window),
	m_ers(nullptr),
	m_categories(nullptr),
	m_filter_proxy(nullptr),
	m_global_proxy(nullptr),
	m_local_proxy(nullptr)
{
	m_undo_stack = m_app->getUndoStack();
	m_global_box = m_window->erBar()->ui.global;
	m_local_box = m_window->erBar()->ui.local;
	m_global_selection = m_global_box->selectionStack();
	m_local_selection = m_local_box->selectionStack();
	m_display = m_window->erDisplay();
	m_filter_area = m_window->erFilterArea();

	m_filter_proxy = new ERFilterSortProxy(nullptr);
	m_filter_proxy->sortBy(ERFilterSortProxy::ALPHABETICAL);
	m_global_proxy = new ERFilterSortProxy(m_filter_proxy);
	m_global_proxy->showGlobal(true);
	m_global_proxy->showLocal(false);
	m_local_proxy = new ERFilterSortProxy(m_filter_proxy);
	m_local_proxy->showGlobal(false);
	m_local_proxy->showLocal(true);

	m_category_control = new ECategoryControl(app, nullptr);

	m_category_checkbox_model = new CheckableListProxy(this);
	m_category_checkbox_model->setSourceModel(m_category_control->categoryModel());
	m_filter_area->setCategoryModel(m_category_checkbox_model);
	m_filter_proxy->setCategories(m_category_checkbox_model);

	QAction *a_new_er = new QAction("New Resource", this);
	a_new_er->setIconText("+");
	connect(a_new_er, &QAction::triggered, this, &ERControl::newER);

	QAction *a_delete_er = new QAction("Delete Resources", this);
	a_delete_er->setIconText("-");
	connect(a_delete_er, &QAction::triggered, this, &ERControl::deleteER);

	QAction *a_edit_er = new QAction("Edit Resource", this);
	a_edit_er->setIconText("Edit");
	connect(a_edit_er, &QAction::triggered, this, &ERControl::editERInfo);

	QAction *a_open_er = new QAction("Open Resource", this);
	a_open_er->setIconText("Open");
	connect(a_open_er, &QAction::triggered, this, &ERControl::openResource);

	QAction *a_position_er = new QAction("Set Resource Position", this);
	a_position_er->setIconText("Set Position");
	connect(a_position_er, &QAction::triggered, this, &ERControl::setPosition);

	QAction *a_goto_er = new QAction("Goto Resource", this);
	connect(a_goto_er, &QAction::triggered, this, &ERControl::gotoPosition);

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

	connect(m_local_box, &HorizontalScrollBox::sTouch, this, &ERControl::onSelectionChange);
	connect(m_global_box, &HorizontalScrollBox::sTouch, this, &ERControl::onSelectionChange);

	// mash the two selections together
	// if one clears everything, then clear the other one too
	connect(m_local_box, &HorizontalScrollBox::sSelectionCleared, this, [this]() {
		m_global_selection->clear();
	});
	connect(m_global_box, &HorizontalScrollBox::sSelectionCleared, this, [this]() {
		m_local_selection->clear();
	});

	// hide/show filter area
	connect(m_window->erBar()->ui.filter, &QPushButton::pressed, this,
		[this]() {
		ERFilterArea *area = m_window->erFilterArea();
		area->setVisible(!area->isVisible());
	});

	connect(m_app, &VSimApp::sStateChanged, this, [this]() {
		VSimApp::State state = m_app->state();
		if (!m_app->isFlying() && state != VSimApp::EDIT_ERS) {
			setDisplay(-1);
		}
	});

	connect(m_app, &VSimApp::sAboutToReset, this, [this]() {
		setDisplay(-1);
	});

	load(ers);
}

void ERControl::load(EResourceGroup *ers)
{
	if (ers == nullptr) {
		m_ers = nullptr;
		m_categories = nullptr;
		m_category_control->load(nullptr);
		return;
	}

	m_ers = ers;
	m_categories = ers->categories();

	m_filter_proxy->setBase(ers);

	m_global_box->setGroup(m_global_proxy);
	m_local_box->setGroup(m_local_proxy);

	m_category_control->load(m_categories);

	m_filter_area->reset();
}

void ERControl::newER()
{
	ERDialog dlg(m_category_control, m_app->getCurrentDirectory());

	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}
	qInfo() << "Command - New Embedded Resource";
	m_undo_stack->beginMacro("New Resource");

	EResource *resource = new EResource;
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
	resource->setCategory(dlg.getCategory());

	m_undo_stack->push(new Group::AddNodeCommand(m_ers, resource));
	m_undo_stack->push(new SelectERCommand(this, { resource }, Command::ON_REDO));
	m_undo_stack->endMacro();
}

void ERControl::deleteER()
{
	std::set<int> selection = getCombinedSelection();
	if (selection.empty()) return;

	m_undo_stack->beginMacro("Delete Resources");
	m_undo_stack->push(new SelectERCommand(this, getCombinedSelectionP(), Command::ON_UNDO));
	// save old categories, so that we can restore them later
	auto cmd = new Group::EditCommand(m_ers, selection);
	for (int i : selection) {
		EResource *res = m_ers->getResource(i);
		if (!res) continue;
		new EResource::SetCategoryCommand(res, nullptr, cmd);
	}
	m_undo_stack->push(cmd);
	// remove resources
	m_undo_stack->push(new Group::RemoveSetCommand(m_ers, selection));
	m_undo_stack->endMacro();
}

void ERControl::editERInfo()
{
	int active_item = getCombinedLastSelected();
	if (active_item < 0) {
		qWarning() << "resource list - can't edit with no selection";
		return;
	}
	EResource *resource = m_ers->getResource(active_item);

	ERDialog dlg(m_category_control, m_app->getCurrentDirectory());

	dlg.init(resource);
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		qInfo() << "resource list - cancelled edit on" << active_item;
		return;
	}

	//ECategory* category = getCategory(dlg.getCategory());

	m_undo_stack->beginMacro("Set Resource Info");
	//m_undo_stack->push(new SelectResourcesCommand(this, { active_item }));

	auto cmd = new Group::EditCommand(m_ers, { active_item });

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
	if (resource->getCategory() != dlg.getCategory())
		new EResource::SetCategoryCommand(resource, dlg.getCategory(), cmd);

	//m_undo_stack->push(new EResource::SetCameraMatrixCommand(resource, m_window->getViewerWidget()->getCameraMatrix()));
	m_undo_stack->push(new SelectERCommand(this, { resource }));
	m_undo_stack->push(cmd);

	m_undo_stack->endMacro();
}

void ERControl::openResource()
{
	int index = getCombinedLastSelected();
	if (index < 0) return;
	EResource *res = m_ers->getResource(index);
	if (!res) return;

	if (res->getERType() == EResource::FILE) {
		QString path = m_app->getCurrentDirectory() + "/" + res->getResourcePath().c_str();
		qInfo() << "Attempting to open file:" << path;
		QDesktopServices::openUrl(QUrl(path));
	}
	else if (res->getERType() == EResource::URL) {
		qInfo() << "Attempting to open url:" << res->getResourcePath().c_str();
		QDesktopServices::openUrl(QUrl(res->getResourcePath().c_str()));
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
	int active_item = getCombinedLastSelected();
	qInfo() << "Set ER position" << active_item;
	if (active_item < 0) {
		qWarning() << "Can't set ER position - no selection";
		return;
	}

	EResource *resource = m_ers->getResource(active_item);
	m_undo_stack->beginMacro("Set Resource Info");
	m_undo_stack->push(new EResource::SetCameraMatrixCommand(resource, m_app->getCameraMatrix()));
	m_undo_stack->push(new SelectERCommand(this, { resource }));
	m_undo_stack->endMacro();
}

void ERControl::gotoPosition()
{
	int active_item = getCombinedLastSelected();
	qInfo() << "Goto ER position" << active_item;
	if (active_item < 0) {
		qWarning() << "Can't goto ER position - no selection";
		return;
	}
	EResource *resource = m_ers->getResource(active_item);
	m_app->setCameraMatrix(resource->getCameraMatrix());
}

void ERControl::setDisplay(int index, bool go)
{
	m_active_item = index;

	EResource *res = m_ers->getResource(m_active_item);
	if (!res) {
		m_display->setInfo(nullptr);
		m_display->hide();
		return;
	}

	m_display->setInfo(res);
	m_display->show();

	if (go) {
		m_app->setCameraMatrixSmooth(res->getCameraMatrix(), .3);
	}
}

void ERControl::onSelectionChange()
{
	int new_active = getCombinedLastSelected();

	if (new_active >= 0) {
		// set state
		m_app->setState(VSimApp::EDIT_ERS);
		setDisplay(new_active);
	}
	else {
		setDisplay(-1);
		m_app->setState(VSimApp::EDIT_FLYING);
	}
}

void ERControl::selectERs(const std::vector<EResource*> &res)
{
	std::vector<int> globals = m_global_proxy->indicesOf(res, false);
	m_global_box->selectionStack()->set(globals);

	std::vector<int> locals = m_local_proxy->indicesOf(res, false);
	m_local_box->selectionStack()->set(locals);

	m_app->setState(VSimApp::EDIT_ERS);
	setDisplay(getCombinedLastSelected());
}

void ERControl::debug()
{
	qInfo() << "Debugging ERControl";
	qInfo() << "local proxy - ";
	m_local_proxy->debug();
	qInfo() << "global proxy - ";
	m_global_proxy->debug();

	qInfo() << "Categories";
	for (unsigned int i = 0; i < m_categories->getNumChildren(); i++) {
		ECategory *cat = m_categories->category(i);
		qInfo() << i << ":" << "node" << (void*)m_categories->child(i) <<
			"cat" << (void*)cat << QString::fromStdString(cat->getCategoryName()) << cat->getColor();
	}
}

std::set<int> ERControl::getCombinedSelection()
{
	// remap selection to nodes
	std::set<osg::Node*> nodes;
	std::set<int> local_selection = m_local_selection->toSet();
	for (auto i : local_selection) {
		nodes.insert(m_local_proxy->child(i));
	}
	std::set<int> global_selection = m_global_selection->toSet();
	for (auto i : global_selection) {
		nodes.insert(m_global_proxy->child(i));
	}

	// map nodes to indices
	std::set<int> selection;
	for (auto node : nodes) {
		selection.insert(m_ers->indexOf(node));
	}
	return selection;
}

std::vector<EResource*> ERControl::getCombinedSelectionP()
{
	std::set<EResource*> has;
	std::vector<EResource*> out;

	// tape global on top of local
	SelectionData local = m_local_selection->data();
	for (int index : local) {
		EResource *res = m_local_proxy->getResource(index);
		if (has.find(res) == has.end() && res) {
			has.insert(res);
			out.push_back(res);
		}
	}

	SelectionData global = m_global_selection->data();
	for (int index : global) {
		EResource *res = m_global_proxy->getResource(index);
		if (has.find(res) == has.end() && res) {
			has.insert(res);
			out.push_back(res);
		}
	}

	return out;
}

int ERControl::getCombinedLastSelected()
{
	QObject *sender = QObject::sender();
	ERFilterSortProxy *proxy;
	int last;
	int local_last = m_local_selection->last();
	int global_last = m_global_selection->last();
	// use the sender to determine the last selected
	if ((sender == m_local_box 
		|| sender == m_local_box->selectionStack())
		&& local_last != -1) {
		last = local_last;
		proxy = m_local_proxy;
	}
	else if ((sender == m_global_box
		|| sender == m_global_box->selectionStack())
		&& global_last != -1) {
		last = global_last;
		proxy = m_global_proxy;
	}
	else {
		// just take whatever is selected
		if (local_last != -1) {
			last = local_last;
			proxy = m_local_proxy;
		}
		else {
			last = global_last;
			proxy = m_global_proxy;
		}
	}

	// convert into original index
	if (last < 0) return -1;
	osg::Node *node = proxy->child(last);
	int last_base = m_ers->indexOf(node);

	if (last_base >= (int)m_ers->getNumChildren()) return -1;
	return last_base;
}

void ERControl::clearSelection()
{
	m_global_box->selectionStack()->clear();
	m_local_box->selectionStack()->clear();
}

SelectERCommand::SelectERCommand(ERControl *control,
	const std::vector<EResource*> &resources,
	Command::When when,
	QUndoCommand *parent)
	: m_control(control),
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