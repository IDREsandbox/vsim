/*
 * EResourcesCategory.h
 *
 *  Created on: December 30, 2011
 *      Author: Franklin Fang
 */

#ifndef ERCATEGORY_H_
#define ERCATEGORY_H_

class erCategory:public osg::Group{

public:
	std::string Name;
	//color
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;


	erCategory();
	erCategory(std::string name, unsigned char red, unsigned char green, unsigned char blue);
	~erCategory();
	erCategory & operator= (const erCategory & copy);
	bool operator== (const erCategory &other);
	bool operator!= (const erCategory &other);
};

typedef std::vector<erCategory> CategoryVector; 

#endif