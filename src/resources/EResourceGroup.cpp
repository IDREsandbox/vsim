#include "resources/EResourceGroup.h"
#include <QDebug>

EResourceGroup::EResourceGroup(osg::Group *old_group)
{
	for (int i = old_group->getNumChildren() - 1; i >= 0; i--) {
		osg::Node *node = old_group->getChild(i);
		EResource *old_EResource = dynamic_cast<EResource*>(node);
		if (!old_EResource) continue;
		qDebug() << "Found an old EResource" << QString::fromStdString(old_EResource->getName()) << "- converting";
		this->addChild(old_EResource);
		old_group->removeChild(old_EResource);
	}
}

EResourceGroup::AddEResourceCommand::AddEResourceCommand(EResourceGroup * group, EResource * EResource, QUndoCommand * parent)
	: QUndoCommand(parent),
	m_group(group),
	m_resource(EResource)
{
}

void EResourceGroup::AddEResourceCommand::undo()
{
	uint index = m_group->getNumChildren() - 1;
	m_group->removeChild(index);
	m_group->sDelete(index);
}

void EResourceGroup::AddEResourceCommand::redo()
{
	uint index = m_group->getNumChildren();
	m_group->addChild(m_resource);
	m_group->sNew(index);
}
