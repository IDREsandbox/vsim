/*
 * SerializerNarrative.cpp
 *
 *  Created on: Mar 26, 2011
 *      Author: eduardo
 */

#include <osgDB/ObjectWrapper>
#include <osgDB/InputStream>
#include <osgDB/OutputStream>
#include "deprecated/narrative/NarrativeOld.h"
#include "deprecated/narrative/NarrativeNode.h"

static bool checkNarrativeNodes( const NarrativeOld& node )
{
    return node.getNumNodes() > 0;
}

static bool readNarrativeNodes( osgDB::InputStream& is, NarrativeOld& node )
{
    unsigned int size = 0;
	is >> size >> is.BEGIN_BRACKET;
    for (unsigned int i = 0; i < size; i++)
    {
		osg::Object *read_object = is.readObject(); // needed for mac
        NarrativeNode* childNode = dynamic_cast<NarrativeNode*>(read_object);
        NarrativeTransition* childTrans;
        if (i < size - 1)
        {
			read_object = is.readObject();
            childTrans = dynamic_cast<NarrativeTransition*>(read_object);
        }
        else
        {
            childTrans = new NarrativeTransition();
        }
        if (childNode && childTrans)
            node.addNode(node.getNumNodes() - 1, childNode, childTrans);
    }
	is >> is.END_BRACKET;
    return true;
}

static bool writeNarrativeNodes( osgDB::OutputStream& os, const NarrativeOld& node )
{
    unsigned int size = node.getNumNodes();
    os << size << os.BEGIN_BRACKET << std::endl;
    for (unsigned int i = 0; i < size; i++)
    {
        os << node.getNode(i);
        if (i < size - 1)
        {
            os << node.getTransition(i);
        }
    }
	os << os.END_BRACKET << std::endl;
    return true;
}

static bool checkSelection( const NarrativeOld& node )
{
    return node.getSelection().node >= 0;
}

static bool readSelection( osgDB::InputStream& is, NarrativeOld& node )
{
    int nodeIndex;
    bool isTransitionSelected;
    is >> nodeIndex >> isTransitionSelected;
    node.setSelection(nodeIndex, isTransitionSelected);
    return true;
}

static bool writeSelection( osgDB::OutputStream& os, const NarrativeOld& node )
{
    NarrativeSelectionInfo nsi = node.getSelection();
    os << nsi.node << nsi.isTransitionSelected;
    return true;
}

REGISTER_OBJECT_WRAPPER2(Narrative,
                         new ::NarrativeOld,
                         ::NarrativeOld,
	                     "::Narrative",
                         "osg::Object osg::Node ::Narrative" )
{
    ADD_USER_SERIALIZER(NarrativeNodes);
    ADD_USER_SERIALIZER(Selection);
	ADD_STRING_SERIALIZER(Name, "");
	ADD_STRING_SERIALIZER(Author, "");
	ADD_STRING_SERIALIZER(Description, "");
	ADD_BOOL_SERIALIZER(Lock, false);
}
