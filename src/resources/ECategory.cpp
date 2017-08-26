#include "resources/ECategory.h"

ECategory::ECategory()
	: Group(),
	m_cat_name("Untitled"),
	m_red(0),
	m_blue(0),
	m_green(0),
	m_index(-1)
{

}
ECategory::ECategory(const ECategory& e, const osg::CopyOp& copyop)
	: Group(e, copyop),
	m_cat_name(e.m_cat_name),
	m_red(e.m_red),
	m_blue(e.m_blue),
	m_green(e.m_green),
	m_index(e.m_index)
{

}

ECategory::~ECategory()
{
}

void ECategory::setIndex(int idx)
{
	m_index = idx;
}

const std::string& ECategory::getCategoryName() const
{
	return m_cat_name;
}
void ECategory::setCategoryName(const std::string& name)
{
	m_cat_name = name;
	emit sCNameChanged(m_cat_name, m_index);
}

int ECategory::getRed() const
{
	return m_red;
}
void ECategory::setRed(int red)
{
	m_red = red;
	emit sCRedChanged(m_red, m_index);
}
int ECategory::getGreen() const
{
	return m_green;
}
void ECategory::setGreen(int green)
{
	m_green = green;
	emit sCGreenChanged(m_green, m_index);
}
int ECategory::getBlue() const
{
	return m_blue;
}
void ECategory::setBlue(int blue)
{
	m_blue = blue;
	emit sCBlueChanged(m_blue, m_index);
}
