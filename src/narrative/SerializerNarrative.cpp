/*
 * SerializerNarrative.cpp
 *
 *  Created on: Mar 26, 2011
 *      Author: eduardo
 */

#include <osgDB/ObjectWrapper>
#include <osgDB/InputStream>
#include <osgDB/OutputStream>

#include "narrative/Narrative.h"
#include "narrative/NarrativeNode.h"

// DEPRECATED - These functions only exist for backwards compatibility
static bool checkNarrativeNodes( const Narrative& node )
{
	//return node.getNumNodes() > 0;
    return false;
}
static bool readNarrativeNodes( osgDB::InputStream& is, Narrative& node )
{
	uint size = 0;
	is >> size >> is.BEGIN_BRACKET;
	qDebug() << "read narrative nodes, size" << size;
	// The first node has no transition
	if (size >= 1) {
		NarrativeNode* childNode = dynamic_cast<NarrativeNode*>(is.readObject());
		if (!childNode) {
			qWarning() << "Error: reading old narrative format and read a non-NarrativeNode";
			//return false;
		}
		node.addChild(childNode);

		for (uint i = 1; i < size; i++) {
			// read in a transition
			NarrativeTransition* childTrans = dynamic_cast<NarrativeTransition*>(is.readObject());
			// read in a node
			NarrativeNode* childNode = dynamic_cast<NarrativeNode*>(is.readObject());
			if (childNode == nullptr || childTrans == nullptr) {
				qWarning() << "Error: reading old narrative format and read null transition/node pair" << i << "/" << size;
				//return false;
			}
			if (childNode && childTrans) {
				childNode->setTransitionDuration(childTrans->getDuration());
				node.addChild(childNode);
			}
		}
	}
	is >> is.END_BRACKET;
	return true;
}
static bool writeNarrativeNodes( osgDB::OutputStream& os, const Narrative& node )
{
	// DEPRECATED - just write nothing
	return true;
	//unsigned int size = node.getNumNodes();
	//os << size << os.BEGIN_BRACKET << std::endl;
	//for (unsigned int i = 0; i < size; i++)
	//{
	//	os << node.getNode(i);
	//	if (i < size - 1)
	//	{
	//		os << node.getTransition(i);
	//	}
	//}
	//os << os.END_BRACKET << std::endl;
	//return true;
}
static bool checkSelection( const Narrative& node )
{
	//return node.getSelection().node >= 0;
	return false;
}
static bool readSelection( osgDB::InputStream& is, Narrative& node )
{
	//int nodeIndex;
	//bool isTransitionSelected;
	//is >> nodeIndex >> isTransitionSelected;
	//node.setSelection(nodeIndex, isTransitionSelected);
	return true;
}

static bool writeSelection( osgDB::OutputStream& os, const Narrative& node )
{
	//NarrativeSelectionInfo nsi = node.getSelection();
	//os << nsi.node << nsi.isTransitionSelected;
	return true;
}

REGISTER_OBJECT_WRAPPER( Narrative,
                         new ::Narrative,
                         ::Narrative,
                         "osg::Object osg::Node osg::Group ::Narrative" )
{
	//ADD_LIST_SERIALIZER(Strings, std::vector<std::string>);
    ADD_USER_SERIALIZER(NarrativeNodes); // DEPRECATED, kept for backwards compatibility
    ADD_USER_SERIALIZER(Selection); // DEPRECATED, is unused
	ADD_STRING_SERIALIZER(Name, "");
	ADD_STRING_SERIALIZER(Author, "");
	ADD_STRING_SERIALIZER(Description, "");
	ADD_BOOL_SERIALIZER(Lock, false);
	//ADD_FLOAT_SERIALIZER(Foo, 4.0);
	//ADD_LIST_SERIALIZER(Strings, std::vector<std::string>);
}
