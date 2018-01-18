/*
 * SeriallizerEResourcesList.cpp
 *
 *  Created on: January 14, 2011
 *      Author: Franklin Fang
 */

#include <osgDB/ObjectWrapper>
#include <osgDB/InputStream>
#include <osgDB/OutputStream>

#include "deprecated/resources/EResourcesNode.h"
#include "deprecated/resources/EResourcesList.h"

static bool checkEResources( const EResourcesList& node )
{
	if(node.m_list.size() == 0)
	{
		return false;
	}
	return true;
}

static bool readEResources( osgDB::InputStream& is, EResourcesList& node )
{
	//check whether save current list or not, or auto save the list
	node.m_list.clear();
	node.m_widgets.clear();
	unsigned int size = 0;
    is >> size >> is.BEGIN_BRACKET;
    for (unsigned int i = 0; i < size; i++)
    {
		osg::ref_ptr<EResourcesNode> childNode = is.readObjectOfType<EResourcesNode>();
		osg::ref_ptr<VSLabel> childWidget = is.readObjectOfType<VSLabel>();
		node.m_list.push_back(childNode.get());
		node.m_widgets.push_back(childWidget.get());
    }
    is >> is.END_BRACKET;
    return true;
}

static bool writeEResources( osgDB::OutputStream& os, const EResourcesList& node )
{
	unsigned int size = node.m_list.size();
    os << size << os.BEGIN_BRACKET << std::endl;
    for (unsigned int i = 0; i < size; i++)
    {
        os << node.m_list[i];
		os << node.m_widgets[i];
    }
    os << os.END_BRACKET << std::endl;
    return true;
}

REGISTER_OBJECT_WRAPPER( EResourcesList,
                         new ::EResourcesList,
                         ::EResourcesList,
                         "osg::Object osg::Node ::EResourcesList" )
{
    ADD_USER_SERIALIZER(EResources);
	ADD_BOOL_SERIALIZER(Lock, false);
}
