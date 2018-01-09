/*
 * Narrative.h
 *
 *  Created on: Feb 4, 2011
 *	Author: eduardo
 */
#ifndef NARRATIVE_H_
#define NARRATIVE_H_

#include <osg/Node>
#include <assert.h>
#include <vector>
#include "deprecated/narrative/NarrativeNode.h"

typedef std::vector<osg::ref_ptr<NarrativeNode> > NarrativeNodeVector;
typedef std::vector<osg::ref_ptr<NarrativeTransition> > NarrativeTransitionVector;

struct NarrativeSelectionInfo
{
	int node;
	bool isTransitionSelected;
	NarrativeSelectionInfo(): node(-1), isTransitionSelected(false) {}
	NarrativeSelectionInfo(int in_node, bool in_trans): node(in_node), isTransitionSelected(in_trans) {}
};

class Narrative: public osg::Node
{
public:
	Narrative();
	Narrative(const Narrative& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY):
			osg::Node(n, copyop),
			m_selection(n.m_selection),
			m_nodes(n.m_nodes),
			m_transitions(n.m_transitions),
			m_locked(false) {}
	virtual ~Narrative();

	META_Node(, Narrative)

	std::string& getName() {	return m_name;}
	const std::string& getName() const {	return m_name;}
	void setName(const std::string& name){m_name = name;}
	const std::string& getAuthor() const {return m_author;}
	void setAuthor(const std::string& author){m_author = author;}
	const std::string& getDescription() const {return m_description;}
	void setDescription(const std::string& description){m_description = description;}
	bool getLock() const{return m_locked;}
	void setLock(bool lock){ m_locked = lock;}	
	NarrativeSelectionInfo getSelection() const { return m_selection; }
	void setSelection(int nodeIndex, bool isTransitionSelected);
	void addNode(int nodeIndex, NarrativeNode* node, NarrativeTransition* tran);
	void removeNode(int nodeIndex);
	unsigned int getNumNodes() const { return m_nodes.size(); }
	NarrativeNode* getNode(unsigned int i) const { return m_nodes[i].get(); }
	NarrativeTransition* getTransition(unsigned int i) const { assert(i < m_transitions.size() - 1); return m_transitions[i].get(); }
	void clear() { m_nodes.clear(); m_transitions.clear(); setName(""); setSelection(-1, false); }
	

protected:
	std::string m_name;
	std::string m_description;
	std::string m_author;
	NarrativeSelectionInfo m_selection;
	NarrativeNodeVector m_nodes;
	NarrativeTransitionVector m_transitions;
	bool m_locked;
};

#endif /* NARRATIVE_H_ */
