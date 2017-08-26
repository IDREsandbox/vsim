//#include "deprecated/resources/EResourcesNode.h"
#include "resources/EResource.h"


EResource::EResource()
	: Group(),
	m_name("Untitled"),
	m_filetype("null"),
	m_filepath("null"),
	m_description(" "),
	m_authors(" "),
	m_global(1),
	m_reposition(0),
	m_launch(0),
	m_copyright(0),
	m_min_year(-99999),
	m_max_year(99999),
	m_local_range(0),
	m_ertype(0),
	m_filter(0),
	m_camera_position(osg::Vec3f(0, 0, 0)),
	m_view_matrix(osg::Matrixd()),
	m_inview(0),
	m_cat_name("null"),
	m_blue(0),
	m_green(0),
	m_red(0),
	m_index(-1)
{
}

EResource::EResource(const EResource& er, const osg::CopyOp& copyop)
	: Group(er, copyop),
	m_name(er.m_name),
	m_filetype(er.m_filetype),
	m_filepath(er.m_filepath),
	m_description(er.m_description),
	m_authors(er.m_authors),
	m_global(er.m_global),
	m_reposition(er.m_reposition),
	m_launch(er.m_launch),
	m_copyright(er.m_copyright),
	m_min_year(er.m_min_year),
	m_max_year(er.m_max_year),
	m_local_range(er.m_local_range),
	m_ertype(er.m_ertype),
	m_filter(er.m_filter),
	m_camera_position(er.m_camera_position),
	m_view_matrix(er.m_view_matrix),
	m_inview(er.m_inview),
	m_cat_name(er.m_cat_name),
	m_blue(er.m_blue),
	m_green(er.m_green),
	m_red(er.m_red),
	m_index(er.m_index)
{
}

//EResource::EResource(const EResourcesNode *er) 
//	: Group(),
//	m_name(er->getResourceName()),
//	m_filetype(er->getResourceType()),
//	m_filepath(er->getResourcePath()),
//	m_description(er->getResourceDiscription()),
//	m_authors(er->getAuthor()),
//	m_global(er->getGlobal()),
//	m_reposition(er->getReposition()),
//	m_launch(er->getAutoLaunch()),
//	m_copyright(er->getCopyRight()),
//	m_min_year(er->getMinYear()),
//	m_max_year(er->getMaxYear()),
//	m_local_range(er->getLocalRange()),
//	m_ertype(er->getErType()),
//	m_filter(0),
//	m_view_matrix(er->getViewMatrix()),
//	m_camera_position(m_view_matrix.getTrans()),
//	m_inview(0),
//	m_cat_name(er->getCategoryName()),
//	m_blue(er->getBlue()),
//	m_green(er->getGreen()),
//	m_red(er->getRed())
//{
//}

EResource::~EResource()
{
}

void EResource::setIndex(int idx)
{
	m_index = idx;
}

const std::string& EResource::getResourceName() const 
{ 
	return m_name; 
}
void EResource::setResourceName(const std::string& name) 
{ 
	m_name = name; 
	emit sResourceNameChanged(name, m_index);
}
const std::string& EResource::getResourceType() const 
{ 
	return m_filetype; 
}
void EResource::setResourceType(const std::string& type) 
{ 
	m_filetype = type; 
	emit sResourceTypeChanged(type, m_index);
}
const std::string& EResource::getResourcePath() const 
{ 
	return m_filepath; 
}
void EResource::setResourcePath(const std::string& path) 
{
	m_filepath = path; 
	emit sResourcePathChanged(path, m_index);
}
const std::string& EResource::getResourceDescription() const 
{ 
	return m_description; 
}
void EResource::setResourceDescription(const std::string& description) 
{ 
	m_description = description; 
	emit sResourceDescriptionChanged(description, m_index);
}
const std::string& EResource::getAuthor() const 
{ 
	return m_authors; 
}
void EResource::setAuthor(const std::string& authors) 
{ 
	m_authors = authors; 
	emit sResourceAuthorChanged(authors, m_index);
}
int EResource::getGlobal() const 
{ 
	return m_global; 
}
void EResource::setGlobal(int gorl) 
{ 
	m_global = gorl; 
	emit sGlobalChanged(gorl, m_index);
}
int EResource::getCopyRight() const 
{ 
	return m_copyright; 
}
void EResource::setCopyRight(int cr)
{ 
	m_copyright = cr; 
	emit sCopyRightChanged(cr, m_index);
}
int EResource::getMinYear() const 
{ 
	return m_min_year; 
}
void EResource::setMinYear(int my) 
{ 
	m_min_year = my; 
	emit sMinYearChanged(my, m_index);
}
int EResource::getMaxYear() const 
{ 
	return m_max_year; 
}
void EResource::setMaxYear(int my) 
{ 
	m_max_year = my; 
	emit sMaxYearChanged(my, m_index);
}
int EResource::getReposition() const
{ 
	return m_reposition; 
}
void EResource::setReposition(int reposition) 
{ 
	m_reposition = reposition;
	emit sRepositionChanged(reposition, m_index);
}
int EResource::getAutoLaunch() const 
{ 
	return m_launch; 
}
void EResource::setAutoLaunch(int launch) 
{ 
	m_launch = launch; 
	emit sAutoLaunchChanged(launch, m_index);
}
float EResource::getLocalRange() const 
{ 
	return m_local_range; 
}
void EResource::setLocalRange(float lrange) 
{ 
	m_local_range = lrange; 
	emit sLocalRangeChanged(lrange, m_index);
}
int EResource::getErType() const 
{ 
	return m_ertype; 
}
void EResource::setErType(int ertype)
{ 
	m_ertype = ertype; 
	emit sErTypeChanged(ertype, m_index);
}

const osg::Matrixd& EResource::getViewMatrix() const 
{ 
	return m_view_matrix; 
}
osg::Matrixd& EResource::getViewMatrix() 
{ 
	return m_view_matrix; 
}
void EResource::setViewMatrix(const osg::Matrixd& matrix)
{
    m_view_matrix = matrix;
    m_camera_position = m_view_matrix.getTrans();
	emit sViewMatrixChanged(matrix, m_index);
}

const std::string& EResource::getCategoryName() const 
{ 
	return m_cat_name; 
}
void EResource::setCategoryName(const std::string& name) 
{
	m_cat_name = name; 
	emit sCategoryNameChanged(name, m_index);
}

int EResource::getRed() const 
{ 
	return m_red; 
}
void EResource::setRed(int red) 
{ 
	m_red = red; 
	emit sRedChanged(red, m_index);
}
int EResource::getGreen() const 
{ 
	return m_green; 
}
void EResource::setGreen(int green) 
{ 
	m_green = green; 
	emit sGreenChanged(green, m_index);
}
int EResource::getBlue()const 
{ 
	return m_blue; 
}
void EResource::setBlue(int blue) 
{ 
	m_blue = blue; 
	emit sBlueChanged(blue, m_index);
}

