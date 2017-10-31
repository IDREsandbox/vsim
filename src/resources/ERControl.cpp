#include "resources/ERControl.h"

#include "resources/EResource.h"
#include "resources/EResourceGroup.h"
#include "resources/ECategory.h"
#include "resources/ECategoryGroup.h"

#include "ERDialog.h"

ERControl::ERControl(QObject *parent, MainWindow *window, EResourceGroup *ers, ECategoryGroup *categories)
	: m_window(window)
{
	load(ers, categories);
}

void ERControl::load(EResourceGroup *ers, ECategoryGroup *categories)
{
	m_ers = ers;
	m_categories = categories;
	
	if (ers == nullptr || categories == nullptr) {

		return;
	}


}

void ERControl::newER()
{
	//ERDialog dlg(this);
	//int result = dlg.exec();
	//if (result == QDialog::Rejected) {
	//	return;
	//}

	//m_undo_stack->beginMacro("New Resource");
	//int num_children;
	//if (dlg.getGlobal())
	//	num_children = m_global_resource_group->getNumChildren();
	//else
	//	num_children = m_local_resource_group->getNumChildren();
	//m_undo_stack->push(new SelectResourcesCommand(this, { num_children - 1 }, ON_UNDO));
	//if (dlg.getGlobal()) {
	//	m_undo_stack->push(new EResourceGroup::NewEResourceCommand(m_global_resource_group, m_global_resource_group->getNumChildren()));
	//	m_undo_stack->push(new EResourceGroup::NewEResourceCommand(m_resource_group, m_resource_group->getNumChildren()));
	//}
	//else {
	//	m_undo_stack->push(new EResourceGroup::NewEResourceCommand(m_local_resource_group, m_local_resource_group->getNumChildren()));
	//	m_undo_stack->push(new EResourceGroup::NewEResourceCommand(m_resource_group, m_resource_group->getNumChildren()));
	//}
	//m_undo_stack->push(new SelectResourcesCommand(this, { num_children }, ON_REDO));
	//m_undo_stack->endMacro();

	//EResource* resource;
	//if (dlg.getGlobal()) {
	//	osg::Node *c = m_global_resource_group->getChild(m_global_resource_group->getNumChildren() - 1);
	//	resource = dynamic_cast<EResource*>(c); // getResource(m_resource_group->getNumChildren() - 1);
	//	resource->setIndex(m_global_resource_group->getNumChildren() - 1);
	//}
	//else {
	//	osg::Node *c = m_local_resource_group->getChild(m_local_resource_group->getNumChildren() - 1);
	//	resource = dynamic_cast<EResource*>(c); // getResource(m_resource_group->getNumChildren() - 1);
	//	resource->setIndex(m_local_resource_group->getNumChildren() - 1);
	//}
	//resource->setResourceName(dlg.getTitle());
	//resource->setAuthor(dlg.getAuthor());
	//resource->setResourceDescription(dlg.getDescription());
	//resource->setResourcePath(dlg.getPath());
	////resource->setResourceType(
	//resource->setGlobal(dlg.getGlobal());
	//resource->setCopyRight(dlg.getCopyRight());
	//resource->setMinYear(dlg.getMinYear());
	//resource->setMaxYear(dlg.getMaxYear());
	//resource->setReposition(dlg.getReposition());
	//resource->setAutoLaunch(dlg.getAutoLaunch());
	//resource->setLocalRange(dlg.getLocalRange());
	//resource->setErType(dlg.getErType());

	//resource->setViewMatrix(this->getViewer()->getCameraManipulator()->getMatrix());

	////set category details
	//ECategory *category = getCategory(dlg.getCategory());
	//resource->setCategoryName(category->getCategoryName());
	//resource->setRed(category->getRed());
	//resource->setGreen(category->getGreen());
	//resource->setBlue(category->getBlue());
}

void ERControl::deleteER()
{
	//std::set<int> selection = ui->local->getSelection();
	//if (selection.empty()) return;
	//int next_selection = nextSelectionAfterDelete(m_resource_group->getNumChildren(), selection);

	//// get pointers to nodes to delete
	//m_undo_stack->beginMacro("Delete Resources");
	//m_undo_stack->push(new SelectResourcesCommand(this, selection, ON_UNDO));
	//// delete in reverse order
	//for (auto i = selection.rbegin(); i != selection.rend(); ++i) {
	//	qDebug() << "pushing delete resource" << *i;
	//	m_undo_stack->push(new EResourceGroup::DeleteEResourceCommand(m_resource_group, *i));
	//}
	//m_undo_stack->push(new SelectResourcesCommand(this, { next_selection }, ON_REDO));
	//m_undo_stack->endMacro();
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

void ERControl::newERCat(const std::string &name, QColor color)
{
	ECategory *category = new ECategory();
	category->setCategoryName(name);
	category->setRed(color.red());
	category->setBlue(color.blue());
	category->setGreen(color.green());
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