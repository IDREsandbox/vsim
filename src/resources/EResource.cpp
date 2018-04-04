//#include "deprecated/resources/EResourcesNode.h"
#include "resources/EResource.h"
#include "deprecated/resources/EResourcesNode.h"
#include "ECategoryGroup.h"
#include <QDebug>

EResource::EResource()
	: m_name("Untitled"),
	m_filepath(""),
	m_description(" "),
	m_authors(" "),
	m_global(true),
	m_reposition(false),
	m_launch(false),
	m_copyright(UNSPECIFIED),
	m_min_year(0),
	m_max_year(0),
	m_local_range(0),
	m_ertype(ANNOTATION),
	m_camera_position(osg::Vec3f(0, 0, 0)),
	m_camera_matrix(osg::Matrixd()),
	m_category(nullptr),
	m_index(-1)
{
}

EResource::EResource(const EResourcesNode *er, const std::map<std::string, ECategory*> &cats)
	: m_name(er->getResourceName()),
	m_filepath(er->getResourcePath()),
	m_description(er->getResourceDiscription()),
	m_authors(er->getAuthor()),
	m_global(er->getGlobal()),
	m_reposition(er->getReposition()),
	m_launch(er->getAutoLaunch()),
	m_copyright((Copyright)er->getCopyRight()),
	m_min_year(er->getMinYear()),
	m_max_year(er->getMaxYear()),
	m_local_range(er->getLocalRange()),
	m_ertype((ERType)er->getErType()),
	m_camera_matrix(er->getViewMatrix()),
	m_camera_position(m_camera_matrix.getTrans())
{
	ECategory *cat = nullptr;
	auto it = cats.find(er->getCategoryName());
	if (it != cats.end()) {
		qDebug() << "Assiging an old ER cat" << this << it->first.c_str();
		cat = it->second;
	}

	if (er->getMinYear() != 0 || er->getMaxYear() != 0) {
		qDebug() << "- nonzero resource year:" << QString::fromStdString(er->getResourceName()) << m_min_year << m_max_year;
	}

	setCategory(cat);
}

const std::string& EResource::getResourceName() const 
{ 
	return m_name; 
}
void EResource::setResourceName(const std::string& name) 
{ 
	m_name = name; 
	emit sResourceNameChanged(name);
}
//const std::string& EResource::getResourceType() const 
//{ 
//	return m_filetype; 
//}
//void EResource::setResourceType(const std::string& type) 
//{ 
//	m_filetype = type; 
//	emit sResourceTypeChanged(type, m_index);
//}
const std::string& EResource::getResourcePath() const 
{ 
	return m_filepath; 
}
void EResource::setResourcePath(const std::string& path) 
{
	m_filepath = path; 
	emit sResourcePathChanged(path);
}
const std::string& EResource::getResourceDescription() const 
{ 
	return m_description; 
}
void EResource::setResourceDescription(const std::string& description) 
{ 
	m_description = description; 
	emit sResourceDescriptionChanged(description);
}
const std::string& EResource::getAuthor() const 
{ 
	return m_authors; 
}
void EResource::setAuthor(const std::string& authors) 
{ 
	m_authors = authors; 
	emit sResourceAuthorChanged(authors);
}
bool EResource::getGlobal() const 
{ 
	return m_global; 
}
void EResource::setGlobal(bool global)
{ 
	m_global = global;
	emit sGlobalChanged(global);
}
EResource::Copyright EResource::getCopyright() const 
{ 
	return m_copyright; 
}
void EResource::setCopyright(EResource::Copyright cr)
{ 
	m_copyright = cr; 
	emit sCopyRightChanged(cr);
}
int EResource::getMinYear() const 
{ 
	return m_min_year; 
}
void EResource::setMinYear(int my) 
{ 
	m_min_year = my; 
	emit sMinYearChanged(my);
}
int EResource::getMaxYear() const 
{ 
	return m_max_year; 
}
void EResource::setMaxYear(int my) 
{ 
	m_max_year = my; 
	emit sMaxYearChanged(my);
}
bool EResource::getReposition() const
{ 
	return m_reposition; 
}
void EResource::setReposition(bool reposition)
{ 
	m_reposition = reposition;
	emit sRepositionChanged(reposition);
}
bool EResource::getAutoLaunch() const
{ 
	return m_launch; 
}
void EResource::setAutoLaunch(bool launch) 
{ 
	m_launch = launch; 
	emit sAutoLaunchChanged(launch);
}
float EResource::getLocalRange() const 
{ 
	return m_local_range; 
}
void EResource::setLocalRange(float lrange) 
{ 
	m_local_range = lrange; 
	emit sLocalRangeChanged(lrange);
}
EResource::ERType EResource::getERType() const
{ 
	return m_ertype; 
}
void EResource::setERType(ERType ertype)
{ 
	m_ertype = ertype; 
	emit sErTypeChanged(ertype);
}

const osg::Matrixd& EResource::getCameraMatrix() const 
{ 
	return m_camera_matrix; 
}
osg::Matrixd& EResource::getCameraMatrix()
{ 
	return m_camera_matrix; 
}
void EResource::setCameraMatrix(const osg::Matrixd& matrix)
{
    m_camera_matrix = matrix;
    m_camera_position = m_camera_matrix.getTrans();
	emit sViewMatrixChanged(matrix);
}

ECategory * EResource::category() const
{
	return m_category.get();
}

const ECategory *EResource::getCategory() const
{ 
	return m_category.get(); 
}

void EResource::setCategory(ECategory *category) 
{
	ECategory *old_cat = m_category.get();
	if (old_cat) {
		old_cat->removeResource(this);
	}

	m_category = category;

	if (m_category.get()) {
		m_category->addResource(this);
	}
	emit sCategoryChanged(old_cat, category);
}

int EResource::getCategoryIndex() const
{
	return m_category_index;
}

void EResource::setCategoryIndex(int index)
{
	m_category_index = index;
}

