/*
 * SerializerNarrative.cpp
 *
 *  Created on: Mar 26, 2011
 *      Author: eduardo
 */

#include <osgDB/ObjectWrapper>
#include <osgDB/InputStream>
#include <osgDB/OutputStream>
#include "deprecated/narrative/Narrative.h"
#include "deprecated/narrative/NarrativeNode.h"

static bool checkNarrativeNodes( const Narrative& node )
{
    return node.getNumNodes() > 0;
}

static bool readNarrativeNodes( osgDB::InputStream& is, Narrative& node )
{
    unsigned int size = 0;
	is >> size >> is.BEGIN_BRACKET;
    for (unsigned int i = 0; i < size; i++)
    {
        NarrativeNode* childNode = dynamic_cast<NarrativeNode*>(is.readObject());
        NarrativeTransition* childTrans;
        if (i < size - 1)
        {
            childTrans = dynamic_cast<NarrativeTransition*>(is.readObject());
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

static bool writeNarrativeNodes( osgDB::OutputStream& os, const Narrative& node )
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

static bool checkSelection( const Narrative& node )
{
    return node.getSelection().node >= 0;
}

static bool readSelection( osgDB::InputStream& is, Narrative& node )
{
    int nodeIndex;
    bool isTransitionSelected;
    is >> nodeIndex >> isTransitionSelected;
    node.setSelection(nodeIndex, isTransitionSelected);
    return true;
}

static bool writeSelection( osgDB::OutputStream& os, const Narrative& node )
{
    NarrativeSelectionInfo nsi = node.getSelection();
    os << nsi.node << nsi.isTransitionSelected;
    return true;
}

REGISTER_OBJECT_WRAPPER( Narrative,
                         new ::Narrative,
                         ::Narrative,
                         "osg::Object osg::Node ::Narrative" )
{
    ADD_USER_SERIALIZER(NarrativeNodes);
    ADD_USER_SERIALIZER(Selection);
	ADD_STRING_SERIALIZER(Name, "");
	ADD_STRING_SERIALIZER(Author, "");
	ADD_STRING_SERIALIZER(Description, "");
	ADD_BOOL_SERIALIZER(Lock, false);
}
