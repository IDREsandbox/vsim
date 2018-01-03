#include "resources/ERControl.h"

#include "resources/EResource.h"
#include "resources/EResourceGroup.h"
#include "resources/ECategory.h"
#include "resources/ECategoryGroup.h"
#include "resources/ERDialog.h"
#include "resources/ERScrollBox.h"
#include "resources/NewCatDialog.h"
#include "resources/ERDisplay.h"
#include "MainWindow.h"
#include "../ui_MainWindow.h"
#include "OSGViewerWidget.h"
#include "ERFilterSortProxy.h"

#include <QDesktopServices>

ERControl::ERControl(QObject *parent, MainWindow *window, EResourceGroup *ers)
	: QObject(parent),
	m_window(window),
	m_ers(nullptr),
	m_categories(nullptr),
	m_filter_proxy(nullptr),
	m_global_proxy(nullptr),
	m_local_proxy(nullptr)
{
	m_undo_stack = m_window->m_undo_stack;
	m_global_box = m_window->ui->global;
	m_local_box = m_window->ui->local;
	m_display = m_window->erDisplay();
	m_dialog = new ERDialog(m_window);

	auto &ui = m_window->ui;
	// new
	connect(m_local_box, &ERScrollBox::sNew, this, &ERControl::newER);
	connect(m_global_box, &ERScrollBox::sNew, this, &ERControl::newER);
	connect(ui->plus_2, &QPushButton::clicked, this, &ERControl::newER);
	// delete
	connect(m_local_box, &ERScrollBox::sDelete, this, &ERControl::deleteER);
	connect(m_global_box, &ERScrollBox::sDelete, this, &ERControl::deleteER);
	connect(ui->minus_2, &QPushButton::clicked, this, &ERControl::deleteER);
	// edit
	connect(m_local_box, &ERScrollBox::sEdit, this, &ERControl::editERInfo);
	connect(m_global_box, &ERScrollBox::sEdit, this, &ERControl::editERInfo);
	connect(ui->edit, &QPushButton::clicked, this, &ERControl::editERInfo);
	// open
	connect(ui->global, &ERScrollBox::sOpen, this, &ERControl::openResource);

	connect(m_local_box, &ERScrollBox::sSetPosition, this, &ERControl::setPosition);
	connect(m_global_box, &ERScrollBox::sSetPosition, this, &ERControl::setPosition);
	connect(m_local_box, &ERScrollBox::sGotoPosition, this, &ERControl::gotoPosition);
	connect(m_global_box, &ERScrollBox::sGotoPosition, this, &ERControl::gotoPosition);

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
		return;
	}
	m_ers = ers;
	m_categories = ers->categories();

	m_dialog->setCategoryGroup(m_categories);

	// set up proxies
	if (m_filter_proxy == nullptr) {
		m_filter_proxy = new ERFilterSortProxy(ers);
		m_filter_proxy->sortBy(ERFilterSortProxy::ALPHABETICAL);
		m_global_proxy = new ERFilterSortProxy(m_filter_proxy);
		m_global_proxy->filterGlobal(ERFilterSortProxy::SHOW_GLOBAL);
		m_local_proxy = new ERFilterSortProxy(m_filter_proxy);
		m_local_proxy->filterGlobal(ERFilterSortProxy::SHOW_LOCAL);
	}
	else {
		// we can just re-assign the root one
		m_filter_proxy->setBase(ers);
	}

	m_global_box->setGroup(m_global_proxy);
	m_local_box->setGroup(m_local_proxy);
}

void ERControl::newER()
{
	m_dialog->init(nullptr);
	int result = m_dialog->exec();
	if (result == QDialog::Rejected) {
		return;
	}
	qDebug() << "Command - New Embedded Resource";
	m_undo_stack->beginMacro("New Resource");

	EResource *resource = new EResource;
	resource->setResourceName(m_dialog->getTitle());
	resource->setAuthor(m_dialog->getAuthor());
	resource->setResourceDescription(m_dialog->getDescription());
	resource->setResourcePath(m_dialog->getPath());
	//resource->setResourceType(
	resource->setGlobal(m_dialog->getGlobal());
	resource->setCopyright(m_dialog->getCopyright());
	resource->setMinYear(m_dialog->getMinYear());
	resource->setMaxYear(m_dialog->getMaxYear());
	resource->setReposition(m_dialog->getReposition());
	resource->setAutoLaunch(m_dialog->getAutoLaunch());
	resource->setLocalRange(m_dialog->getLocalRange());
	resource->setERType(m_dialog->getERType());
	resource->setCameraMatrix(m_window->m_osg_widget->getCameraMatrix());

	m_undo_stack->push(new Group::AddNodeCommand(m_ers, resource));

	m_undo_stack->endMacro();
}

void ERControl::deleteER()
{
	std::set<int> selection = getCombinedSelection();
	if (selection.empty()) return;

	for (auto i : selection) qDebug() << "delete ER" << i;

	uint size = m_ers->getNumChildren();

	m_undo_stack->beginMacro("Delete Resources");
	for (auto i = selection.rbegin(); i != selection.rend(); ++i) {
		if (*i >= (int)size) {
			qWarning() << "Out of range selection when deleting ERs" << *i << "/" << size;
			continue;
		}
		m_undo_stack->push(new Group::DeleteNodeCommand(m_ers, *i));
	}
	m_undo_stack->endMacro();
}

void ERControl::editERInfo()
{
	qInfo() << "Edit ER Info";
	int active_item = getCombinedLastSelected();
	qDebug() << "resource list - begin edit on" << active_item;
	if (active_item < 0) {
		qWarning() << "resource list - can't edit with no selection";
		return;
	}

	EResource *resource = m_ers->getResource(active_item);

	m_dialog->init(resource);
	int result = m_dialog->exec();
	if (result == QDialog::Rejected) {
		qDebug() << "resource list - cancelled edit on" << active_item;
		return;
	}

	//ECategory* category = getCategory(dlg.getCategory());

	m_undo_stack->beginMacro("Set Resource Info");
	//m_undo_stack->push(new SelectResourcesCommand(this, { active_item }));

	if (resource->getResourceName() != m_dialog->getTitle())
		m_undo_stack->push(new EResource::SetResourceNameCommand(resource, m_dialog->getTitle()));
	if (resource->getAuthor() != m_dialog->getAuthor())
		m_undo_stack->push(new EResource::SetResourceAuthorCommand(resource, m_dialog->getAuthor()));
	if (resource->getResourceDescription() != m_dialog->getDescription())
		m_undo_stack->push(new EResource::SetResourceDescriptionCommand(resource, m_dialog->getDescription()));
	if (resource->getResourcePath() != m_dialog->getPath())
		m_undo_stack->push(new EResource::SetResourcePathCommand(resource, m_dialog->getPath()));
	if (resource->getGlobal() != m_dialog->getGlobal())
		m_undo_stack->push(new EResource::SetGlobalCommand(resource, m_dialog->getGlobal()));
	if (resource->getCopyright() != m_dialog->getCopyright())
		m_undo_stack->push(new EResource::SetCopyrightCommand(resource, m_dialog->getCopyright()));
	if (resource->getMinYear() != m_dialog->getMinYear())
		m_undo_stack->push(new EResource::SetMinYearCommand(resource, m_dialog->getMinYear()));
	if (resource->getMaxYear() != m_dialog->getMaxYear())
		m_undo_stack->push(new EResource::SetMaxYearCommand(resource, m_dialog->getMaxYear()));
	if (resource->getReposition() != m_dialog->getReposition())
		m_undo_stack->push(new EResource::SetRepositionCommand(resource, m_dialog->getReposition()));
	if (resource->getAutoLaunch() != m_dialog->getAutoLaunch())
		m_undo_stack->push(new EResource::SetAutoLaunchCommand(resource, m_dialog->getAutoLaunch()));
	if (resource->getLocalRange() != m_dialog->getLocalRange())
		m_undo_stack->push(new EResource::SetLocalRangeCommand(resource, m_dialog->getLocalRange()));
	if (resource->getERType() != m_dialog->getERType())
		m_undo_stack->push(new EResource::SetErTypeCommand(resource, m_dialog->getERType()));

	//m_undo_stack->push(new EResource::SetCameraMatrixCommand(resource, m_window->getViewerWidget()->getCameraMatrix()));

	m_undo_stack->endMacro();
}

void ERControl::openResource()
{
	int index = getCombinedLastSelected();
	if (index < 0) return;
	EResource *res = dynamic_cast<EResource*>(m_ers->getChild(index));

	m_display->setInfo(res);
	m_display->show();

	if (res->getERType() == EResource::FILE ||
		res->getERType() == EResource::URL) {
		qInfo() << "Attempting to open file:" << res->getResourcePath().c_str();
		QDesktopServices::openUrl(QUrl(res->getResourcePath().c_str()));
	}

	EResource *resource = m_ers->getResource(index);
	m_window->getViewerWidget()->setCameraMatrix(resource->getCameraMatrix());
}

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
	m_undo_stack->push(new EResource::SetCameraMatrixCommand(resource, m_window->getViewerWidget()->getCameraMatrix()));
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
	m_window->getViewerWidget()->setCameraMatrix(resource->getCameraMatrix());
}

void ERControl::newERCat()
{
	NewCatDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	ECategory *category = new ECategory();
	category->setCategoryName(dlg.getCatTitle());
	category->setRed(dlg.getRed());
	category->setBlue(dlg.getGreen());
	category->setGreen(dlg.getBlue());
	m_categories->addChild(category);
}

void ERControl::debug()
{
	qDebug() << "Debugging ERControl";
	qDebug() << "local proxy - ";
	m_local_proxy->debug();
	qDebug() << "global proxy - ";
	m_global_proxy->debug();
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
	if (sender == m_local_box) {
		qDebug() << "local box sent";
		last = m_local_box->getLastSelected();
		proxy = m_local_proxy;
	}
	else if (sender == m_global_box) {
		qDebug() << "global box sent";
		last = m_global_box->getLastSelected();
		proxy = m_global_proxy;
	}
	else {
		qDebug() << "who sent this?" << sender;
		return -1;
	}

	if (last < 0) return -1;
	osg::Node *node = proxy->child(last);
	int last_base = m_ers->indexOf(node);

	if (last_base >= m_ers->getNumChildren()) return -1;
	qDebug() << "last:" << last_base;
	return last_base;
}
