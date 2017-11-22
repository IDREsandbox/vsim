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
#include "ui_MainWindow.h"
#include "OSGViewerWidget.h"

#include <QDesktopServices>

ERControl::ERControl(QObject *parent, MainWindow *window, EResourceGroup *ers)
	: QObject(parent), m_window(window), m_ers(nullptr), m_categories(nullptr)
{
	m_undo_stack = m_window->m_undo_stack;
	m_box = m_window->ui->global;
	m_display = m_window->erDisplay();

	auto &ui = m_window->ui;
	// new
	connect(m_box, &ERScrollBox::sNew, this, &ERControl::newER);
	connect(ui->plus_2, &QPushButton::clicked, this, &ERControl::newER);
	// delete
	connect(m_box, &ERScrollBox::sDelete, this, &ERControl::deleteER);
	connect(ui->minus_2, &QPushButton::clicked, this, &ERControl::deleteER);
	// edit
	connect(m_box, &ERScrollBox::sEdit, this, &ERControl::editERInfo);
	connect(ui->edit, &QPushButton::clicked, this, &ERControl::editERInfo);
	// open
	connect(ui->global, &ERScrollBox::sOpen, this, &ERControl::openResource);

	connect(m_box, &ERScrollBox::sSetPosition, this, &ERControl::setPosition);
	connect(m_box, &ERScrollBox::sGotoPosition, this, &ERControl::gotoPosition);

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

	m_box->setGroup(ers);

}

void ERControl::newER()
{
	ERDialog dlg(nullptr, m_categories.get());
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}
	qDebug() << "Command - New Embedded Resource";
	m_undo_stack->beginMacro("New Resource");
	int num_children;

	EResource *resource = new EResource;
	resource->setResourceName(dlg.getTitle());
	resource->setAuthor(dlg.getAuthor());
	resource->setResourceDescription(dlg.getDescription());
	resource->setResourcePath(dlg.getPath());
	//resource->setResourceType(
	resource->setGlobal(dlg.getGlobal());
	resource->setCopyright(dlg.getCopyright());
	resource->setMinYear(dlg.getMinYear());
	resource->setMaxYear(dlg.getMaxYear());
	resource->setReposition(dlg.getReposition());
	resource->setAutoLaunch(dlg.getAutoLaunch());
	resource->setLocalRange(dlg.getLocalRange());
	resource->setERType(dlg.getERType());
	resource->setCameraMatrix(m_window->m_osg_widget->getCameraMatrix());

	m_undo_stack->push(new Group::AddNodeCommand<EResource>(m_ers, resource));

	m_undo_stack->endMacro();
}

void ERControl::deleteER()
{
	std::set<int> selection = m_box->getSelection();
	if (selection.empty()) return;

	uint size = m_ers->getNumChildren();

	m_undo_stack->beginMacro("Delete Resources");
	for (auto i = selection.rbegin(); i != selection.rend(); ++i) {
		if (*i >= size) {
			qWarning() << "Out of range selection when deleting ERs" << *i << "/" << size;
			continue;
		}
		m_undo_stack->push(new Group::DeleteNodeCommand<EResource>(m_ers, *i));
	}
	m_undo_stack->endMacro();
}

void ERControl::editERInfo()
{
	qInfo() << "Edit ER Info";

	int active_item = m_box->getLastSelected();
	qDebug() << "resource list - begin edit on" << active_item;
	if (active_item < 0) {
		qWarning() << "resource list - can't edit with no selection";
		return;
	}

	EResource *resource = m_ers->getResource(active_item);

	ERDialog dlg(resource, m_ers->categories());
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

	//m_undo_stack->push(new EResource::SetCameraMatrixCommand(resource, m_window->getViewerWidget()->getCameraMatrix()));

	m_undo_stack->endMacro();
}

void ERControl::openResource()
{
	int index = m_box->getLastSelected();
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
	int active_item = m_box->getLastSelected();
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
	int active_item = m_box->getLastSelected();
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

//void ERControl::openResource()
//{
//	//int index = ui->local->getLastSelected();
//	//if (index < 0) return;
//	int index = 0;
//
//	EResource *res = dynamic_cast<EResource*>(m_categories->getChild(index));
//	
//	m_display->show();
//	m_display->setInfo(res);
//
//	//this->getViewer()->getCameraManipulator()->setByMatrix(res->getViewMatrix());
//}