/*
 * EResourcesNode.h
 *
 *  Created on: December 15, 2011
 *      Author: Franklin Fang
 */
#ifndef ERESOURCES_NODE_H_
#define ERESOURCES_NODE_H_

#include <osg/Group>
#include "deprecated/resources/EResourcesCategory.h"

class EResourcesNode:public osg::Group {
	
public:
	EResourcesNode();
	virtual ~EResourcesNode();
	bool operator== (const EResourcesNode &other);

    EResourcesNode(const EResourcesNode& e, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
        : osg::Group(e, copyop),
        m_name(e.m_name),
        m_filetype(e.m_filetype),
        m_filepath(e.m_filepath),
        m_description(e.m_description),
		m_authors(e.m_authors),
        m_global(e.m_global),
		m_min_year(e.m_min_year),
		m_max_year(e.m_max_year),
        m_reposition(e.m_reposition),
        m_launch(e.m_launch),
		m_copyright(e.m_copyright),
        m_local_range(e.m_local_range),
        m_ertype(e.m_ertype),
		m_filter(false),
        m_camera_position(e.m_camera_position),
        m_category(e.m_category),
    	m_view_matrix(e.m_view_matrix)
        {}

    META_Node(, EResourcesNode)

	EResourcesNode & operator= (const EResourcesNode & copy);

	const std::string& getResourceName() const {return m_name;}
	void setResourceName(const std::string& name){m_name = name;}
	const std::string& getResourceType() const {return m_filetype;}
	void setResourceType(const std::string& type){m_filetype = type;}
	const std::string& getResourcePath() const {return m_filepath;}
	void setResourcePath(const std::string& path) {m_filepath = path;}
	const std::string& getResourceDiscription() const {return m_description;}
	void setResourceDiscription(const std::string& discription) {m_description = discription;}
	const std::string& getAuthor() const {return m_authors;}
	void setAuthor(const std::string& authors) {m_authors = authors;}
    int getGlobal() const {return m_global;}
	void setGlobal(int gorl){m_global = gorl;}
	int getCopyRight() const {return m_copyright;}
	void setCopyRight(int cr){m_copyright = cr;}
	int getMinYear() const {return m_min_year;}
	void setMinYear(int my){m_min_year = my;}
	int getMaxYear() const {return m_max_year;}
	void setMaxYear(int my){m_max_year = my;}
	int getReposition() const {return m_reposition;}
	void setReposition(int reposition){m_reposition = reposition;}
	int getAutoLaunch() const {return m_launch;}
	void setAutoLaunch(int launch){m_launch = launch;}
	float getLocalRange() const {return m_local_range;}
	void setLocalRange(float lrange){m_local_range = lrange;}
	int getErType() const {return m_ertype;}
	void setErType(int ertype){m_ertype = ertype;}
	const osg::Matrixd& getViewMatrix() const {return m_view_matrix;}

	const std::string& getCategoryName() const {return m_category.Name;}
	void setCategoryName(const std::string& name){m_category.Name = name;}
	unsigned int getRed() const {return m_category.Red;}
	void setRed(unsigned int red){m_category.Red = red;}
	unsigned int getGreen()const {return m_category.Green;}
	void setGreen(unsigned int green){m_category.Green = green;}
	unsigned int getBlue()const {return m_category.Blue;}
	void setBlue(unsigned int blue){m_category.Blue = blue;}

	void setViewMatrix(const osg::Matrixd& matrix) 
	{
		m_view_matrix = matrix; 
		m_camera_position = m_view_matrix.getTrans(); 		
	}
	osg::Matrixd& getViewMatrix() { return m_view_matrix; }

	std::string m_name;
	std::string m_filetype;
	std::string m_filepath;
	std::string m_description;
	std::string m_authors;
	int m_global;
	int m_reposition;
	int m_launch;
	int m_copyright;
	int m_min_year;//default -99999
	int m_max_year;//default 99999
	float m_local_range;//global should be false to make this valid
	int m_ertype;//0 file, 1 link, 2 annotation
	bool m_filter;

	osg::Vec3f m_camera_position;

	erCategory m_category;
	osg::Matrixd m_view_matrix;

	bool m_inview;

};

#endif 
