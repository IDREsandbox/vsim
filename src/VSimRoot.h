#ifndef NARRATIVEROOT_H
#define NARRATIVEROOT_H

#include <osg/Group>
#include <memory>
#include <QObject>

class NarrativeGroup;
class ModelGroup;
class EResourceGroup;
class ECategoryGroup;
class CanvasScene;
namespace VSim { namespace FlatBuffers {
	struct SettingsT;
}}

class VSimRoot : public QObject {
public:
	VSimRoot(QObject *parent = nullptr);
	~VSimRoot();

	void loadOld(osg::Group *old_group);
	// Merges two VSimRoots together
	//void merge(VSimRoot *other);

	// copies another VSimRoot
	// steals some unique_ptrs where QObject is no involved
	void take(VSimRoot *other);

	NarrativeGroup *narratives() const;
	EResourceGroup *resources() const;
	ModelGroup *models() const;
	CanvasScene *branding() const;

	// settings stuff, there is no settings class yet
	void copySettings(const VSimRoot *other);

	// never null
	VSim::FlatBuffers::SettingsT *settings() const;

	void debug();

	// only kind of works
	void moveAllToThread(QThread *thread);

private:
	std::unique_ptr<NarrativeGroup> m_narratives;
	std::unique_ptr<ModelGroup> m_models;
	std::unique_ptr<EResourceGroup> m_resources;
	std::unique_ptr<VSim::FlatBuffers::SettingsT> m_settings;
	std::unique_ptr<CanvasScene> m_branding_canvas;
};

#endif