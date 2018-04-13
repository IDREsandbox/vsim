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
	//void merge(VSimRoot *other);

	NarrativeGroup *narratives() const;
	EResourceGroup *resources() const;
	ModelGroup *models() const;

	VSim::FlatBuffers::SettingsT *settings() const; // not optional

	void debug();

private:
	std::unique_ptr<NarrativeGroup> m_narratives; // qt ownership
	std::unique_ptr<ModelGroup> m_models;
	std::unique_ptr<EResourceGroup> m_resources;
	std::unique_ptr<VSim::FlatBuffers::SettingsT> m_settings;
};

#endif