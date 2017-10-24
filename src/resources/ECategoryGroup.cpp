#include "resources/ECategoryGroup.h"
#include <QDebug>

ECategoryGroup::ECategoryGroup(osg::Group *old_group)
{
	// conversion constructor
	// scans a list of EResources or whatever and builds a list of categories
	for (int i = old_group->getNumChildren() - 1; i >= 0; i--) {
		osg::Node *node = old_group->getChild(i);
		EResource *old_EResource = dynamic_cast<EResource*>(node);
		if (old_EResource) {
			qDebug() << "old EResource" << QString::fromStdString(old_EResource->getName())
				<< " category:" << QString::fromStdString(old_EResource->getCategoryName());

			// TODO: build group
		}
	}
}

ECategoryGroup::AddECategoryCommand::AddECategoryCommand(ECategoryGroup * group, ECategory * ECat, QUndoCommand * parent)
	: QUndoCommand(parent),
	m_group(group),
	m_category(ECat)
{
}

void ECategoryGroup::AddECategoryCommand::undo()
{
	uint index = m_group->getNumChildren() - 1;
	m_group->removeChild(index);
	m_group->sDelete(index);
}

void ECategoryGroup::AddECategoryCommand::redo()
{
	uint index = m_group->getNumChildren();
	m_group->addChild(m_category);
	m_group->sNew(index);
}
