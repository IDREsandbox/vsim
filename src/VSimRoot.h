#ifndef NARRATIVEROOT_H
#define NARRATIVEROOT_H

#include <osg/Group>
#include <memory>

class NarrativeGroup;
class ModelGroup;
class EResourceGroup;
class ECategoryGroup;
namespace VSim { namespace FlatBuffers {
	struct SettingsT;
}}

class VSimRoot {
public:
	VSimRoot();
	~VSimRoot();

	void loadOld(osg::Group *old_group);
	// Merges two VSimRoots together
	void merge(VSimRoot *other);

	NarrativeGroup *narratives() const;
	const NarrativeGroup *getNarratives() const; // need the const version for the serializer
	void setNarratives(NarrativeGroup *narratives);

	ModelGroup *models() const;
	const ModelGroup *getModels() const;
	void setModels(ModelGroup *models);

	EResourceGroup *resources() const;
	const EResourceGroup *getResources() const;
	void setResources(EResourceGroup *resources);

	VSim::FlatBuffers::SettingsT *settings() const; // not optional

	void debug();

private:
	osg::ref_ptr<NarrativeGroup> m_narratives; // qt ownership
	osg::ref_ptr<ModelGroup> m_models;
	osg::ref_ptr<EResourceGroup> m_resources;
	std::unique_ptr<VSim::FlatBuffers::SettingsT> m_settings;
};

//class DebugVisitor : public osg::NodeVisitor {
//public:
//	DebugVisitor() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN), m_tabs(0) {}
//	virtual void apply(osg::Group &group) override;
//	virtual void apply(osg::Node &node) override;
//
//private:
//	int m_tabs;
//};

#endif