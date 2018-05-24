#include "VSimRoot.h"

#include <iostream>

#include "narrative/NarrativeSlide.h"
#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "ModelGroup.h"
#include "Model.h"
#include "resources/EResourceGroup.h"
#include "resources/EResource.h"
#include "resources/ECategoryGroup.h"
#include "resources/ECategory.h"
#include "deprecated/resources/EResourcesList.h"
#include "deprecated/ModelInformationOld.h"
#include "deprecated/narrative/NarrativeOld.h"
#include <QDate>
#include <QThread>
#include <QEvent>
#include "Core/Util.h"

// debug
#include "Canvas/LabelStyleGroup.h"
#include "Canvas/LabelStyle.h"

#include "settings_generated.h" // SettingsT

VSimRoot::VSimRoot(QObject *parent)
	: QObject(parent)
{
	m_narratives = std::make_unique<NarrativeGroup>();
	m_models = std::make_unique<ModelGroup>();
	m_resources = std::make_unique<EResourceGroup>();

	m_settings = std::make_unique<VSim::FlatBuffers::SettingsT>();
}

VSimRoot::~VSimRoot()
{
}

void VSimRoot::take(VSimRoot * other)
{
	m_narratives->clear();
	m_resources->clear();
	m_models->clear();
	*m_narratives = *other->m_narratives;
	*m_resources = *other->m_resources;
	m_models->copyReference(*other->m_models);

	m_settings.swap(other->m_settings);
}

NarrativeGroup * VSimRoot::narratives() const
{
	return m_narratives.get();
}
ModelGroup * VSimRoot::models() const
{
	return m_models.get();
}
EResourceGroup * VSimRoot::resources() const
{
	return m_resources.get();
}

void VSimRoot::copySettings(const VSimRoot * other)
{
	m_settings->graphics_settings;

}

VSim::FlatBuffers::SettingsT * VSimRoot::settings() const
{
	return m_settings.get();
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

	qInfo() << "Models:" << m_models->size();
	for (uint i = 0; i < m_models->size(); i++) {
		qInfo() << "Model" << QString::fromStdString(m_models->child(i)->name());
	}
}

void VSimRoot::loadOld(osg::Group * old_group)
{
	qDebug() << "loading old vsim" << old_group->getNumChildren();
	// find ModelInformation

	// scan for model information
	for (uint i = 0; i < old_group->getNumChildren(); i++) {
		osg::Node *node = old_group->getChild(i);
		ModelInformationOld *info = dynamic_cast<ModelInformationOld*>(node);
		if (info) {
			qInfo() << "Found old model information";
			m_settings->model_information =
				std::make_unique<VSim::FlatBuffers::ModelInformationT>();
			auto *new_info = m_settings->model_information.get();

			new_info->authors = info->getPrimaryAuthors();
			new_info->contributors = info->getContributors();
			new_info->model_name = info->getModelName();
			new_info->place_of_publication = info->getPlaceofPublication();
			QDate date;
			bool hit = Util::mxdxyyToQDate(info->getProjectDate(), &date);
			if (!hit) date = QDate();
			new_info->project_date_julian_day = date.toJulianDay();
			new_info->version = info->getReleaseDateVersion();
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
		new_model->setNode(node);
		new_model->setName(node->getName());
		m_models->append(new_model);
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

//void VSimRoot::merge(VSimRoot *other)
//{
//	// basically just copy over the Narratives and Models contents
//	qDebug() << "merging";
//
//	NarrativeGroup *other_narratives = other->narratives();
//	qDebug() << "current narratives:" << m_narratives->getNumChildren();
//	qDebug() << "other narratives:" << other_narratives->getNumChildren();
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
