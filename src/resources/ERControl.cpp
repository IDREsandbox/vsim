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
#include "OSGViewerWidget.h"
#include "resources/ERFilterSortProxy.h"
#include "CheckableListProxy.h"
#include "VSimApp.h"

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
	m_global_box = m_window->erGlobal();
	m_local_box = m_window->erLocal();
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

	// new
	connect(m_local_box, &ERScrollBox::sNew, this, &ERControl::newER);
	connect(m_global_box, &ERScrollBox::sNew, this, &ERControl::newER);
	connect(window->newERButton(), &QAbstractButton::clicked, this, &ERControl::newER);
	// delete
	connect(m_local_box, &ERScrollBox::sDelete, this, &ERControl::deleteER);
	connect(m_global_box, &ERScrollBox::sDelete, this, &ERControl::deleteER);
	connect(window->deleteERButton(), &QAbstractButton::clicked, this, &ERControl::deleteER);
	// edit
	connect(m_local_box, &ERScrollBox::sEdit, this, &ERControl::editERInfo);
	connect(m_global_box, &ERScrollBox::sEdit, this, &ERControl::editERInfo);
	connect(window->editERButton(), &QAbstractButton::clicked, this, &ERControl::editERInfo);
	// open
	connect(m_local_box, &ERScrollBox::sOpen, this, &ERControl::openResource);
	connect(m_global_box, &ERScrollBox::sOpen, this, &ERControl::openResource);

	connect(m_local_box, &ERScrollBox::sSetPosition, this, &ERControl::setPosition);
	connect(m_global_box, &ERScrollBox::sSetPosition, this, &ERControl::setPosition);
	connect(m_local_box, &ERScrollBox::sGotoPosition, this, &ERControl::gotoPosition);
	connect(m_global_box, &ERScrollBox::sGotoPosition, this, &ERControl::gotoPosition);

	connect(m_local_box, &ERScrollBox::sSelectionChange, this, &ERControl::onSelectionChange);
	connect(m_global_box, &ERScrollBox::sSelectionChange, this, &ERControl::onSelectionChange);

	// mash the two selections together
	connect(m_local_box, &HorizontalScrollBox::sSelectionCleared, this, [this]() {
		m_global_box->setSelection({}, -1);
	});
	connect(m_global_box, &HorizontalScrollBox::sSelectionCleared, this, [this]() {
		m_local_box->setSelection({}, -1);
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
	qDebug() << "Command - New Embedded Resource";
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

	m_undo_stack->endMacro();
}

void ERControl::deleteER()
{
	std::set<int> selection = getCombinedSelection();
	if (selection.empty()) return;

	m_undo_stack->beginMacro("Delete Resources");
	// save old categories, so that we can restore them later
	auto cmd = new Group::EditCommand(m_ers, selection);
	for (int i : selection) {
		EResource *res = m_ers->getResource(i);
		if (!res) continue;
		qDebug() << "setting category to null command";
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
		qDebug() << "resource list - cancelled edit on" << active_item;
		return;
	}

	//ECategory* category = getCategory(dlg.getCategory());

	m_undo_stack->beginMacro("Set Resource Info");
	//m_undo_stack->push(new SelectResourcesCommand(this, { active_item }));

	if (resource->getResourceName() != dlg.getTitle())
		m_undo_stack->push(new EResource::SetResourceNameCommand(resource, dlg.getTitle()));
	if (resource->getAuthor() != dlg.getAuthor())
		m_undo_stack->push(new EResource::SetResourceAuthorCommand(resource, dlg.getAuthor()));
	if (resource->getResourceDescription() != dlg.getDescription())
		m_undo_stack->push(new EResource::SetResourceDescriptionCommand(resource, dlg.getDescription()));
	if (resource->getResourcePath() != dlg.getPath())
		m_undo_stack->push(new EResource::SetResourcePathCommand(resource, dlg.getPath()));
	if (resource->getGlobal() != dlg.getGlobal())
		m_undo_stack->push(new EResource::SetGlobalCommand(resource, dlg.getGlobal()));
	if (resource->getCopyright() != dlg.getCopyright())
		m_undo_stack->push(new EResource::SetCopyrightCommand(resource, dlg.getCopyright()));
	if (resource->getMinYear() != dlg.getMinYear())
		m_undo_stack->push(new EResource::SetMinYearCommand(resource, dlg.getMinYear()));
	if (resource->getMaxYear() != dlg.getMaxYear())
		m_undo_stack->push(new EResource::SetMaxYearCommand(resource, dlg.getMaxYear()));
	if (resource->getReposition() != dlg.getReposition())
		m_undo_stack->push(new EResource::SetRepositionCommand(resource, dlg.getReposition()));
	if (resource->getAutoLaunch() != dlg.getAutoLaunch())
		m_undo_stack->push(new EResource::SetAutoLaunchCommand(resource, dlg.getAutoLaunch()));
	if (resource->getLocalRange() != dlg.getLocalRange())
		m_undo_stack->push(new EResource::SetLocalRangeCommand(resource, dlg.getLocalRange()));
	if (resource->getERType() != dlg.getERType())
		m_undo_stack->push(new EResource::SetErTypeCommand(resource, dlg.getERType()));
	if (resource->getCategory() != dlg.getCategory())
		m_undo_stack->push(new EResource::SetCategoryCommand(resource, dlg.getCategory()));

	//m_undo_stack->push(new EResource::SetCameraMatrixCommand(resource, m_window->getViewerWidget()->getCameraMatrix()));

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

int i;
void ERControl::onSelectionChange()
{
	QString whodunnit;
	if (QObject::sender() == m_local_box) whodunnit = "local";
	else whodunnit = "global";
	i++;

	int new_active = getCombinedLastSelected();
	if (new_active != m_active_item) {
		// go to and set
		m_active_item = new_active;
	
		EResource *res = m_ers->getResource(m_active_item);
		if (!res) {
			qInfo() << "Hide ER display";
			m_display->setInfo(nullptr);
			m_display->hide();
			return;
		}

		qInfo() << "Set ER display to :" << m_active_item;
		m_display->setInfo(res);
		m_display->show();
		m_app->setCameraMatrixSmooth(res->getCameraMatrix(), .3);
	}
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
	std::set<int> local_selection = m_local_box->getSelection();
	for (auto i : local_selection) {
		nodes.insert(m_local_proxy->child(i));
	}
	std::set<int> global_selection = m_global_box->getSelection();
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

int ERControl::getCombinedLastSelected()
{
	QObject *sender = QObject::sender();
	ERFilterSortProxy *proxy;
	int last;
	// use the sender to determine the last selected
	if (sender == m_local_box && m_local_box->getLastSelected() != -1) {
		last = m_local_box->getLastSelected();
		proxy = m_local_proxy;
	}
	else if (sender == m_global_box && m_global_box->getLastSelected() != -1) {
		last = m_global_box->getLastSelected();
		proxy = m_global_proxy;
	}
	else {
		// just take whatever is selected
		if (m_local_box->getLastSelected() != -1) {
			last = m_local_box->getLastSelected();
			proxy = m_local_proxy;
		}
		else {
			last = m_global_box->getLastSelected();
			proxy = m_global_proxy;
		}
	}

	// convert into original index
	if (last < 0) return -1;
	osg::Node *node = proxy->child(last);
	int last_base = m_ers->indexOf(node);

	if (last_base >= m_ers->getNumChildren()) return -1;
	return last_base;
}
