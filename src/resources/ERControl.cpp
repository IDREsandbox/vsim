#include "resources/ERControl.h"

#include "resources/EResource.h"
#include "resources/EResourceGroup.h"
#include "resources/ECategory.h"
#include "resources/ECategoryGroup.h"
#include "resources/ERDialog.h"
#include "resources/ERScrollBox.h"
#include "resources/NewCatDialog.h"
#include "MainWindow.h"

ERControl::ERControl(QObject *parent, MainWindow *window, EResourceGroup *ers, ECategoryGroup *categories)
	: QObject(parent), m_window(window), m_ers(nullptr), m_categories(nullptr)
{
	m_undo_stack = m_window->m_undo_stack;
	m_box = m_window->ui->global;

	auto &ui = m_window->ui;
	// new
	connect(ui->local, &ERScrollBox::sNew, this, &ERControl::newER);
	connect(ui->plus_2, &QPushButton::clicked, this, &ERControl::newER);
	// delete
	connect(ui->local, &ERScrollBox::sDelete, this, &ERControl::deleteER);
	connect(ui->minus_2, &QPushButton::clicked, this, &ERControl::deleteER);
	// edit
	connect(ui->local, &ERScrollBox::sEdit, this, &ERControl::editERInfo);
	connect(ui->edit, &QPushButton::clicked, this, &ERControl::editERInfo);
	// open
	connect(ui->local, &ERScrollBox::sOpen, this, &ERControl::openResource);

	load(ers, categories);
}

void ERControl::load(EResourceGroup *ers, ECategoryGroup *categories)
{
	m_ers = ers;
	m_categories = categories;
	
	if (ers == nullptr || categories == nullptr) {

		return;
	}

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
	resource->setCopyRight(dlg.getCopyRight());
	resource->setMinYear(dlg.getMinYear());
	resource->setMaxYear(dlg.getMaxYear());
	resource->setReposition(dlg.getReposition());
	resource->setAutoLaunch(dlg.getAutoLaunch());
	resource->setLocalRange(dlg.getLocalRange());
	resource->setErType(dlg.getErType());
	resource->setViewMatrix(m_window->m_osg_widget->getCameraMatrix());

	m_undo_stack->push(new Group::AddNodeCommand<EResource>(m_ers, resource));

	m_undo_stack->endMacro();
}

void ERControl::deleteER()
{
	std::set<int> selection = m_box->getSelection();
	if (selection.empty()) return;

	uint size = m_categories->getNumChildren();

	m_undo_stack->beginMacro("Delete Resources");
	for (auto i = selection.rbegin(); i != selection.rend(); ++i) {
		if (*i >= size) {
			qWarning() << "Out of range selection when deleting ERs";
			continue;
		}
		m_undo_stack->push(new Group::DeleteNodeCommand<EResource>(m_ers, *i));
	}
	m_undo_stack->endMacro();
}

void ERControl::editERInfo()
{
	//int active_item = ui->local->getLastSelected();
	//qDebug() << "resource list - begin edit on" << active_item;
	//if (active_item < 0) {
	//	qWarning() << "resource list - can't edit with no selection";
	//	return;
	//}

	//EResource *resource = getResource(active_item);

	//ERDialog dlg(resource, this);
	//int result = dlg.exec();
	//if (result == QDialog::Rejected) {
	//	qDebug() << "resource list - cancelled edit on" << active_item;
	//	return;
	//}

	//ECategory* category = getCategory(dlg.getCategory());

	//m_undo_stack->beginMacro("Set Resource Info");
	//m_undo_stack->push(new SelectResourcesCommand(this, { active_item }));
	//if (resource->getResourceName() != dlg.getTitle())
	//	m_undo_stack->push(new EResource::SetResourceNameCommand(resource, dlg.getTitle()));
	//if (resource->getAuthor() != dlg.getAuthor())
	//	m_undo_stack->push(new EResource::SetResourceAuthorCommand(resource, dlg.getAuthor()));
	//if (resource->getResourceDescription() != dlg.getDescription())
	//	m_undo_stack->push(new EResource::SetResourceDescriptionCommand(resource, dlg.getDescription()));
	//if (resource->getResourcePath() != dlg.getPath())
	//	m_undo_stack->push(new EResource::SetResourcePathCommand(resource, dlg.getPath()));
	//if (resource->getGlobal() != dlg.getGlobal())
	//	m_undo_stack->push(new EResource::SetGlobalCommand(resource, dlg.getGlobal()));
	//if (resource->getCopyRight() != dlg.getCopyRight())
	//	m_undo_stack->push(new EResource::SetCopyRightCommand(resource, dlg.getCopyRight()));
	//if (resource->getMinYear() != dlg.getMinYear())
	//	m_undo_stack->push(new EResource::SetMinYearCommand(resource, dlg.getMinYear()));
	//if (resource->getMaxYear() != dlg.getMaxYear())
	//	m_undo_stack->push(new EResource::SetMaxYearCommand(resource, dlg.getMaxYear()));
	//if (resource->getReposition() != dlg.getReposition())
	//	m_undo_stack->push(new EResource::SetRepositionCommand(resource, dlg.getReposition()));
	//if (resource->getAutoLaunch() != dlg.getAutoLaunch())
	//	m_undo_stack->push(new EResource::SetAutoLaunchCommand(resource, dlg.getAutoLaunch()));
	//if (resource->getLocalRange() != dlg.getLocalRange())
	//	m_undo_stack->push(new EResource::SetLocalRangeCommand(resource, dlg.getLocalRange()));
	//if (resource->getErType() != dlg.getErType())
	//	m_undo_stack->push(new EResource::SetErTypeCommand(resource, dlg.getErType()));

	//m_undo_stack->push(new EResource::SetViewMatrixCommand(resource, this->getViewer()->getCameraManipulator()->getMatrix()));

	//if (resource->getCategoryName() != category->getCategoryName())
	//	m_undo_stack->push(new EResource::SetCategoryNameCommand(resource, category->getCategoryName()));
	//if (resource->getRed() != category->getRed())
	//	m_undo_stack->push(new EResource::SetRedCommand(resource, category->getRed()));
	//if (resource->getGreen() != category->getGreen())
	//	m_undo_stack->push(new EResource::SetGreenCommand(resource, category->getGreen()));
	//if (resource->getBlue() != category->getBlue())
	//	m_undo_stack->push(new EResource::SetBlueCommand(resource, category->getBlue()));
	//m_undo_stack->endMacro();
}

void ERControl::openResource()
{
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