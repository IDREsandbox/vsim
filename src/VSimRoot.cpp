#include "VSimRoot.h"

#include <iostream>
#include <QDate>
#include <QThread>
#include <QEvent>
#include <QCryptographicHash>

#include "narrative/NarrativeSlide.h"
#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "Model/ModelGroup.h"
#include "Model/Model.h"
#include "resources/EResourceGroup.h"
#include "resources/EResource.h"
#include "resources/ECategoryGroup.h"
#include "resources/ECategory.h"
#include "deprecated/resources/EResourcesList.h"
#include "deprecated/ModelInformationOld.h"
#include "deprecated/narrative/NarrativeOld.h"
#include "Core/Util.h"
#include "Canvas/CanvasScene.h"
#include "Core/HashLock.h"
#include "Core/LockTable.h"

// debug
#include "Canvas/LabelStyleGroup.h"
#include "Canvas/LabelStyle.h"

#include "settings_generated.h" // SettingsT
#include "types_generated.h"

VSimRoot::VSimRoot(QObject *parent)
	: QObject(parent)
{
	m_narratives = std::make_unique<NarrativeGroup>();
	m_models = std::make_unique<ModelGroup>();
	m_resources = std::make_unique<EResourceGroup>();

	//m_settings = std::make_unique<VSim::FlatBuffers::SettingsT>();
	m_model_information = std::make_unique<VSim::FlatBuffers::ModelInformationT>();
	m_navigation_settings = std::make_unique<VSim::FlatBuffers::NavigationSettingsT>();
	m_graphics_settings = std::make_unique<VSim::FlatBuffers::GraphicsSettingsT>();
	Util::getOrCreate(m_graphics_settings->camera_settings);
	m_window_settings = std::make_unique<VSim::FlatBuffers::WindowSettingsT>();
	m_other_settings = std::make_unique<VSim::FlatBuffers::OtherSettingsT>();
	m_er_settings = std::make_unique<VSim::FlatBuffers::ERSettingsT>();
	m_branding_canvas = std::make_unique<CanvasScene>();

	m_lock = new LockTable(this);
	m_lock_add_remove = false;
	m_lock_settings = false;
	m_lock_navigation = false;
}

VSimRoot::~VSimRoot()
{
}

//void VSimRoot::take(VSimRoot * other)
//{
//	m_narratives->clear();
//	m_resources->clear();
//	m_models->clear();
//	*m_narratives = *other->m_narratives;
//	*m_resources = *other->m_resources;
//	m_models->copyReference(*other->m_models);
//
//	m_settings.swap(other->m_settings);
//	m_branding_canvas->copy(*other->m_branding_canvas);
//	*m_lock = *other->m_lock;
//}

void VSimRoot::copy(VSimRoot * other)
{
	m_narratives->clear();
	m_resources->clear();
	m_models->clear();
	*m_narratives = *other->m_narratives;
	*m_resources = *other->m_resources;
	m_models->copyReference(*other->m_models);

	copyModelInformation(*m_model_information, *other->m_model_information);
	copyNavigationSettings(*m_navigation_settings, *other->m_navigation_settings);
	copyGraphicsSettings(*m_graphics_settings, *other->m_graphics_settings);
	copyWindowSettings(*m_window_settings, *other->m_window_settings);
	copyOtherSettings(*m_other_settings, *other->m_other_settings);
	copyERSettings(*m_er_settings, *other->m_er_settings);

	m_branding_canvas->copy(*other->m_branding_canvas);
	*m_lock = *other->m_lock;
	m_lock_add_remove = other->m_lock_add_remove;
	m_lock_navigation = other->m_lock_navigation;
	m_lock_settings = other->m_lock_settings;
}

NarrativeGroup * VSimRoot::narratives() const
{
	return m_narratives.get();
}

ModelGroup * VSimRoot::models() const
{
	return m_models.get();
}

CanvasScene * VSimRoot::branding() const
{
	return m_branding_canvas.get();
}

EResourceGroup * VSimRoot::resources() const
{
	return m_resources.get();
}

//VSim::FlatBuffers::SettingsT &VSimRoot::settings() const
//{
//	return *m_settings.get();
//}

VSim::FlatBuffers::ModelInformationT & VSimRoot::modelInformation() const
{
	return *m_model_information.get();
}

VSim::FlatBuffers::NavigationSettingsT & VSimRoot::navigationSettings() const
{
	return *m_navigation_settings.get();
}

VSim::FlatBuffers::GraphicsSettingsT & VSimRoot::graphicsSettings() const
{
	return *m_graphics_settings.get();
}

VSim::FlatBuffers::WindowSettingsT & VSimRoot::windowSettings() const
{
	return *m_window_settings.get();
}

VSim::FlatBuffers::OtherSettingsT & VSimRoot::otherSettings() const
{
	return *m_other_settings;
}

VSim::FlatBuffers::ERSettingsT & VSimRoot::erSettings() const
{
	return *m_er_settings;
}

void VSimRoot::prepareSave()
{
	// make all the unique ptr business
	//auto &fb_lsettings = Util::getOrCreate(m_settings->lock_settings);
	//auto &fb_lock = Util::getOrCreate(fb_lsettings->lock);

	//fb_lsettings->lock_settings = m_lock_settings;
	//fb_lsettings->lock_add_remove = m_lock_add_remove;
	//fb_lsettings->lock_navigation = m_lock_navigation;

	//m_lock->createLockTableT(fb_lock.get());
}

void VSimRoot::postLoad()
{
	//auto &fb_lsettings = Util::getOrCreate(m_settings->lock_settings);
	//auto &fb_lock = Util::getOrCreate(fb_lsettings->lock);

	//setSettingsLocked(fb_lsettings->lock_settings);
	//setRestrictToCurrent(fb_lsettings->lock_add_remove);
	//setNavigationLocked(fb_lsettings->lock_navigation);

	//m_lock->readLockTableT(fb_lock.get());
}

bool VSimRoot::settingsLocked() const
{
	return m_lock->isLocked() && m_lock_settings;
}

bool VSimRoot::restrictedToCurrent() const
{
	return m_lock->isLocked() && m_lock_add_remove;
}

bool VSimRoot::navigationLocked() const
{
	return m_lock->isLocked() && m_lock_navigation;
}

//bool VSimRoot::unlock(QString pw)
//{
//	bool success = m_lock.unlock(pw);
//	if (!success) return false;
//	setSettingsLocked(false);
//	setRestrictToCurrent(false);
//	setNavigationLocked(false);
//	return true;
//}

void VSimRoot::setLockTable(const LockTable &lock)
{
	*m_lock = lock;
}

LockTable * VSimRoot::lockTable()
{
	return m_lock;
}

const LockTable * VSimRoot::lockTableConst() const
{
	return m_lock;
}

void VSimRoot::setNoExpiration()
{
	m_expiration_date = QDate();
}

bool VSimRoot::expires() const
{
	return m_expiration_date.isValid();
}

void VSimRoot::setExpirationDate(QDate date)
{
	m_expiration_date = date;
}

QDate VSimRoot::expirationDate() const
{
	return m_expiration_date;
}

void VSimRoot::setSettingsLocked(bool locked)
{
	m_lock_settings = locked;
	emit sSettingsLockedChanged(locked);
}

void VSimRoot::setRestrictToCurrent(bool restrict)
{
	m_lock_add_remove = restrict;
	emit sRestrictToCurrentChanged(restrict);

	m_narratives->setRestrictToCurrent(restrict);
	m_resources->setRestrictToCurrent(restrict);
	m_resources->categories()->setRestrictToCurrent(restrict);
}

void VSimRoot::setNavigationLocked(bool disable)
{
	m_lock_navigation = disable;
	emit sDisableNavigationChanged(disable);
}

void VSimRoot::debug()
{
	qInfo() << "root";

	qInfo() << "Narratives:" << m_narratives->size();
	for (uint i = 0; i < m_narratives->size(); i++) {
		Narrative *nar = m_narratives->child(i);
		if (!nar) continue;
		qInfo() << "Narrative" << i << QString::fromStdString(nar->getTitle());
		for (uint j = 0; j < nar->size(); j++) {
			NarrativeSlide *slide = nar->child(j);
			qInfo() << "\tSlide" << j << "trans" << slide->getTransitionDuration() << "dur" << slide->getDuration();

			//for (uint k = 0; k < slide->size(); k++) {
			//	NarrativeSlideLabel *label = dynamic_cast<NarrativeSlideLabel*>(slide->child(k));
			//	qInfo() << "\t\tLabel" << k; //<< QString::fromStdString(label->getText());
			//}
		}
		LabelStyleGroup *labs = nar->labelStyles();
		for (uint j = 0; j < std::size(LabelTypeNames); j++) {
			LabelStyle *style = labs->getStyle((LabelType)j);
			if (style) qInfo()
				<< "\tStyle" << j
				<< style->m_font_family.c_str()
				<< style->m_point_size;
		}
	}

	qInfo() << "Embedded Resources:" << m_resources->size();
	for (uint i = 0; i < m_resources->size(); i++) {
		EResource *er = m_resources->child(i);
		ECategory *cat = er->category();
		QString cat_name;
		if (cat) cat_name = cat->getCategoryName().c_str();
		qInfo() << "Resource" << i
			<< QString::fromStdString(er->getResourceName())
			<< "global:" << er->getGlobal()
			<< "cat:" << cat_name;

	}
	const ECategoryGroup *cats = m_resources->categories();
	qInfo() << "ER Categories:" << cats->size();
	for (uint i = 0; i < cats->size(); i++) {
		const ECategory *cat = cats->child(i);
		if (!cat) continue;
		qInfo() << "Category" << i << QString::fromStdString(cat->getCategoryName());
	}

	qInfo() << "Models:" << m_models->group()->size();
	for (uint i = 0; i < m_models->group()->size(); i++) {
		qInfo() << "Model" << QString::fromStdString(m_models->group()->child(i)->name());
	}
}

void VSimRoot::loadOld(osg::Group * old_group)
{
	qInfo() << "loading old vsim" << old_group->getNumChildren();
	// find ModelInformation

	// scan for model information
	for (uint i = 0; i < old_group->getNumChildren(); i++) {
		osg::Node *node = old_group->getChild(i);
		ModelInformationOld *info = dynamic_cast<ModelInformationOld*>(node);
		if (info) {
			qInfo() << "Found old model information";
			auto &new_info = m_model_information;

			new_info->authors = info->getPrimaryAuthors();
			new_info->contributors = info->getContributors();
			new_info->model_name = info->getModelName();
			new_info->place_of_publication = info->getPlaceofPublication();
			QDate date;
			bool hit = Util::mxdxyyToQDate(info->getProjectDate(), &date);
			if (!hit) date = QDate();
			new_info->project_date_julian_day = date.toJulianDay();
			//new_info->version = info->getReleaseDateVersion();
			new_info->url = info->getAccessInformationURL();
			continue;
		}

		EResourcesList *old_ers = dynamic_cast<EResourcesList*>(node);
		if (old_ers) {
			qInfo() << "Found an old resource list";
			m_resources->loadOld(old_ers);
			continue;
		}

		const NarrativeOld *old_narrative = dynamic_cast<const NarrativeOld*>(node);
		if (old_narrative) {
			qInfo() << "Found an old narrative" << QString::fromStdString(old_narrative->getName()) << "- converting";
			auto nar = std::make_shared<Narrative>();
			nar->loadOld(old_narrative);
			m_narratives->append(nar);
			continue;
		}

		const VSCanvas *old_canvas = dynamic_cast<const VSCanvas*>(node);
		if (old_canvas) {
			continue;
		}

		// whatever's left just add to model
		auto new_model = std::make_shared<Model>();
		new_model->setName(node->getName());
		new_model->embedModel(node);
		m_models->addModel(new_model);
	}
}

void VSimRoot::moveAllToThread(QThread *t)
{
	// move all smart ptr managed qobjects to thread
	// er group
	// nar group
	// model group
	// er
	// er cat
	// nar
	// slide
	// label
	m_resources->moveToThread(t);
	m_narratives->moveToThread(t);
	m_models->moveToThread(t);

	for (auto er : *m_resources.get()) {
		er->moveToThread(t);
	}
	for (auto cat : *m_resources->categories()) {
		cat->moveToThread(t);
	}
	for (auto nar : *m_narratives) {
		nar->moveToThread(t);
		for (auto slide : *nar) {
			slide->moveToThread(t);
		}
	}
}


void VSimRoot::copyModelInformation(VSim::FlatBuffers::ModelInformationT & dest,
	const VSim::FlatBuffers::ModelInformationT & src)
{
	dest.model_name = src.model_name;
	dest.project_date_julian_day = src.project_date_julian_day;
	dest.authors = src.authors;
	dest.contributors = src.contributors;
	dest.place_of_publication = src.place_of_publication;
	dest.url = src.url;
	dest.project_date_end_julian = src.project_date_end_julian;
	dest.funders = src.funders;
	dest.research_objective = src.research_objective;
	dest.technology = src.technology;
	dest.release_date_julian = src.release_date_julian;
	dest.release_notes = src.release_notes;
}

void VSimRoot::copyNavigationSettings(VSim::FlatBuffers::NavigationSettingsT &dest,
	const VSim::FlatBuffers::NavigationSettingsT &src)
{
	dest.base_speed = src.base_speed;
	dest.collisions_on = src.collisions_on;
	dest.collision_radius = src.collision_radius;
	dest.default_start_camera = src.default_start_camera;
	dest.eye_height = src.eye_height;
	dest.flight_speed_tick = src.flight_speed_tick;
	dest.gravity_acceleration = src.gravity_acceleration;
	dest.gravity_fall_speed = src.gravity_fall_speed;
	dest.ground_mode_on = src.ground_mode_on;
	Util::getOrCreate(dest.start_camera);
	if (src.start_camera) *dest.start_camera = *src.start_camera;
}

void VSimRoot::copyGraphicsSettings(VSim::FlatBuffers::GraphicsSettingsT &dest,
	const VSim::FlatBuffers::GraphicsSettingsT &src)
{
	dest.lighting = src.lighting;
	auto &cs = Util::getOrCreate(dest.camera_settings);
	auto &cs2 = src.camera_settings;
	if (cs2) {
		cs->fovy = cs2->fovy;
		cs->near_clip = cs2->near_clip;
		cs->far_clip = cs2->far_clip;
		cs->auto_clip = cs2->auto_clip;
	}

	if (src.ambient) {
		dest.ambient = std::make_unique<VSim::FlatBuffers::Color>(*src.ambient);
	}
	else {
		dest.ambient = nullptr;
	}
}

void VSimRoot::copyWindowSettings(VSim::FlatBuffers::WindowSettingsT &dest,
	const VSim::FlatBuffers::WindowSettingsT & src)
{
	dest.window_width = src.window_width;
	dest.window_height = src.window_height;
	dest.nbar_size = src.nbar_size;
	dest.ebar_size = src.ebar_size;
	dest.has_window_size = src.has_window_size;
}

void VSimRoot::copyOtherSettings(VSim::FlatBuffers::OtherSettingsT & dest,
	const VSim::FlatBuffers::OtherSettingsT & src)
{
	dest.year = src.year;
	dest.years_enabled = src.years_enabled;
	dest.narrative_cycling = src.narrative_cycling;
}

void VSimRoot::copyERSettings(VSim::FlatBuffers::ERSettingsT & dest,
	const VSim::FlatBuffers::ERSettingsT & src)
{
	dest.show_all = src.show_all;
	dest.sort_all = src.sort_all;
	dest.sort_global = src.sort_global;
	dest.sort_local = src.sort_local;
	dest.range_enabled = src.range_enabled;
	dest.years_enabled = src.years_enabled;
	dest.auto_launch = src.auto_launch;
	dest.local_radius = src.local_radius;
	dest.categories = src.categories;
	dest.show_legend = src.show_legend;
}


//void VSimRoot::merge(VSimRoot *other)
//{
//	// basically just copy over the Narratives and Models contents
//
//	NarrativeGroup *other_narratives = other->narratives();
//	for (uint i = 0; i < other_narratives->getNumChildren(); i++) {
//		m_narratives->addChild(other_narratives->getChild(i));
//	}
//
//	m_models->merge(other->models());
//}

//void DebugVisitor::apply(osg::Group &group)
//{
//	std::cout << std::string(m_tabs, '\t');
//	std::cout << group.className() << " " << group.getName() << " " << group.getNumChildren() << '\n';
//
//	m_tabs++;
//	traverse(group);
//	m_tabs--;
//}
//
//void DebugVisitor::apply(osg::Node & node)
//{
//	std::cout << std::string(m_tabs, '\t') << node.className() << " " << node.getName() << "\n";
//}
