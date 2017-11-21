#pragma once
#ifndef ECATEGORY_H
#define ECATEGORY_H

#include <string>
#include <osg/Node>
#include <QObject>
#include "Group.h"
#include "Command.h"

class ECategory : public Group {
	Q_OBJECT

public:
	ECategory();
	ECategory(const ECategory& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

	virtual ~ECategory();

	META_Node(, ECategory)

	const std::string& getCategoryName() const;
	void setCategoryName(const std::string& name);

	int getRed() const;
	void setRed(int red);
	int getGreen()const;
	void setGreen(int green);
	int getBlue()const;
	void setBlue(int blue);

signals:
	void sCNameChanged(std::string old_name, std::string new_name);
	void sCRedChanged(int);
	void sCBlueChanged(int);
	void sCGreenChanged(int);

private:
	std::string m_cat_name;
	int m_blue;
	int m_red;
	int m_green;

};
#endif // ECATEGORY_HPP