#include "resources/EResourceGroup.h"
#include <QDebug>

EResourceGroup::EResourceGroup(osg::Group *old_group)
{
	for (int i = old_group->getNumChildren() - 1; i >= 0; i--) {
		osg::Node *node = old_group->getChild(i);
		EResource *old_EResource = dynamic_cast<EResource*>(node);
		if (!old_EResource) continue;
		qDebug() << "Found an old EResource" << QString::fromStdString(old_EResource->getName()) << "- adding to group";
		this->addChild(old_EResource);
		old_group->removeChild(old_EResource);
	}
}
