//#include "deprecated/resources/EResourcesNode.h"
#include "resources/EResource.h"
#include "deprecated/resources/EResourcesNode.h"
#include "resources/ECategory.h"
#include "ECategoryGroup.h"

EResource::EResource(QObject *parent)
	: QObject(parent),
	m_name("Untitled"),
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
	m_category(),
	m_index(-1),
	m_distance_to(0.0f),
	m_in_range(false)
{
}

void EResource::loadOld(const EResourcesNode * old)
{
	m_name = old->getResourceName();
	m_filepath = old->getResourcePath();
	m_description = old->getResourceDiscription();
	m_authors = old->getAuthor();
	m_global = !old->getGlobal();
	m_reposition = old->getReposition();
	m_launch = old->getAutoLaunch();
	m_copyright = (Copyright)old->getCopyRight();
	m_min_year = old->getMinYear();
	m_max_year = old->getMaxYear();
	m_local_range = old->getLocalRange();
	m_ertype = (ERType)old->getErType();
	m_camera_matrix = old->getViewMatrix();
	m_camera_position = m_camera_matrix.getTrans();
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
void EResource::setCameraMatrix(const osg::Matrixd& matrix)
{
    m_camera_matrix = matrix;
    m_camera_position = m_camera_matrix.getTrans();
	emit sViewMatrixChanged(matrix);
}

const osg::Vec3f &EResource::getPosition() const
{
	return m_camera_position;
}

double EResource::getDistanceTo() const
{
	return m_distance_to;
}

void EResource::setDistanceTo(double dist)
{
	m_distance_to = dist;
	emit sDistanceToChanged(dist);
}

bool EResource::inRange() const
{
	return m_in_range;
}

bool EResource::setInRange(bool in)
{
	if (m_in_range == in) return false;
	m_in_range = in;
	emit sInRangeChanged(in);
	return true;
}

ECategory * EResource::category() const
{
	return m_category.lock().get(); // this is probably super illegal
}

std::shared_ptr<ECategory> EResource::categoryShared() const
{
	return m_category.lock();
}

void EResource::setCategory(std::shared_ptr<ECategory> category)
{
	std::shared_ptr<ECategory> old_cat = m_category.lock();

	if (old_cat) {
		old_cat->removeResource(this);
	}

	m_category = category;

	if (category) {
		category->addResource(this);
	}
	emit sCategoryChanged(old_cat.get(), category.get());
}

//void EResource::setCategory(ECategory *category) 
//{
//	ECategory *old_cat = m_category.get();
//	if (old_cat) {
//		old_cat->removeResource(this);
//	}
//
//	m_category = category;
//
//	if (m_category.get()) {
//		m_category->addResource(this);
//	}
//	emit sCategoryChanged(old_cat, category);
//}

int EResource::getCategoryIndex() const
{
	return m_category_index;
}

void EResource::setCategoryIndex(int index)
{
	m_category_index = index;
}

