#include "resources/ECategory.h"

ECategory::ECategory(QObject *parent)
	: QObject(parent),
	m_cat_name("Untitled"),
	m_color(0, 0, 0)
{

}

void ECategory::operator=(const ECategory & other)
{
	m_cat_name = other.m_cat_name;
	m_color = other.m_color;
	m_resources.clear();
}

std::string ECategory::getCategoryName() const
{
	return m_cat_name;
}
void ECategory::setCategoryName(const std::string& name)
{
	std::string old_name = m_cat_name;
	m_cat_name = name;
	emit sCNameChanged(old_name, name);
}

QColor ECategory::getColor() const
{
	return m_color;
}

void ECategory::setColor(QColor color)
{
	m_color = color;
	emit sColorChanged(color);
}

void ECategory::addResource(EResource * res)
{
	m_resources.insert(res);
}

void ECategory::removeResource(EResource * res)
{
	m_resources.erase(res);
}

const std::set<EResource*>& ECategory::resources() const
{
	return m_resources;
}
