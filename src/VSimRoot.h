#ifndef NARRATIVEROOT_H
#define NARRATIVEROOT_H

#include <osg/Group>
#include <osg/NodeVisitor>
#include "narrative/NarrativeGroup.h"
#include "ModelGroup.h"
#include "resources/EResourceGroup.h"
#include "resources/ECategoryGroup.h"

class VSimRoot : public QObject, public osg::Group {
	Q_OBJECT
public:
	VSimRoot();
	VSimRoot(const VSimRoot& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	VSimRoot(osg::Group *old_group); // Convert old VSim format to new VSim format

	META_Node(, VSimRoot)

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

	ECategoryGroup *categories() const;
	const ECategoryGroup *getCategories() const;
	void setCategories(ECategoryGroup *categories);

	int getFoo() const;
	void setFoo(int x);
	int getFoob() const;
	void setFoob(int x);
	int getFoox() const;
	void setFoox(int x);

	void debug();

private:
	osg::ref_ptr<NarrativeGroup> m_narratives;
	osg::ref_ptr<ModelGroup> m_models;
	osg::ref_ptr<EResourceGroup> m_resources;
	osg::ref_ptr<ECategoryGroup> m_categories;

	int foox;
};

class DebugVisitor : public osg::NodeVisitor {
public:
	DebugVisitor() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN), m_tabs(0) {}
	virtual void apply(osg::Group &group) override;
	virtual void apply(osg::Node &node) override;

private:
	int m_tabs;
};

#endif