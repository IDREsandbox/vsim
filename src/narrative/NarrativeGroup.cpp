#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "deprecated/narrative/NarrativeOld.h"
#include <QDebug>

NarrativeGroup::NarrativeGroup(const osg::Group *old_group) 
{
	for (int i = old_group->getNumChildren() - 1; i >= 0; i--) {
		const osg::Node *node = old_group->getChild(i);
		const NarrativeOld *old_narrative = dynamic_cast<const NarrativeOld*>(node);
		if (old_narrative) {
			qDebug() << "Found an old narrative" << QString::fromStdString(old_narrative->getName()) << "- converting";
			append(std::make_shared<Narrative>(old_narrative));
		}
	}
}

Narrative * NarrativeGroup::narrative(int index) const
{
	return child(index);
}
