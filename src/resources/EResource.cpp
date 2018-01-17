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

EResource::EResource(const EResource& er, const osg::CopyOp& copyop)
	: m_name(er.m_name),
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
	m_camera_position(er.m_camera_position),
	m_camera_matrix(er.m_camera_matrix),
	m_category(er.m_category),
	m_index(er.m_index)
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
	if (it != cats.end()) cat = it->second;

	if (er->getMinYear() != 0 || er->getMaxYear() != 0) {
		qDebug() << "- nonzero resource year:" << QString::fromStdString(er->getResourceName()) << m_min_year << m_max_year;
	}

	m_category = cat;
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
	if (old_cat) disconnect(old_cat, 0, this, 0);

	m_category = category;

	if (m_category.get()) {
		m_category->addResource(this);
		// deletion consistency
		connect(this, &QObject::destroyed, m_category.get(),
			[this]() {
			m_category->removeResource(this);
		});
	}

	emit sCategoryChanged(old_cat, category);
}

