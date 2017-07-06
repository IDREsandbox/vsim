/*
 * Narrative.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: eduardo
 */
#include <osg/node>
#include <assert.h>
#include "narrative/Narrative.h"
#include "narrative/NarrativeNode.h"

Narrative::Narrative()
	: osg::Group(),
	m_name("Unknown"),
	m_description("Unknown"),
	m_author("Unknown"),
	m_selection(-1, false),
	m_locked(false)
{
}

Narrative::~Narrative()
{
}

void Narrative::setSelection(int nodeIndex, bool isTransitionSelected)
{
    assert(nodeIndex >= -1 && nodeIndex < (int)m_nodes.size());
    m_selection = NarrativeSelectionInfo(nodeIndex, isTransitionSelected);
}

void Narrative::addNode(int nodeIndex, NarrativeNode* node, NarrativeTransition* tran)
{
    assert(nodeIndex < (int)m_nodes.size());
    assert(m_nodes.size() == m_transitions.size());
    NarrativeNodeVector::iterator it = m_nodes.begin();
    int actualNodeIndex = nodeIndex + 1;
    if (actualNodeIndex < 0)
        actualNodeIndex = 0;
    it += actualNodeIndex;
    m_nodes.insert(it, node);
    NarrativeTransitionVector::iterator itt = m_transitions.begin();
    itt += actualNodeIndex;
    m_transitions.insert(itt, tran);
    setSelection(actualNodeIndex, false);
}

void Narrative::removeNode(int nodeIndex)
{
    assert(nodeIndex >= 0 && nodeIndex < (int)m_nodes.size());
    assert(m_nodes.size() == m_transitions.size());
    NarrativeNodeVector::iterator it = m_nodes.begin();
    it += nodeIndex;
    it = m_nodes.erase(it);
    NarrativeTransitionVector::iterator itt = m_transitions.begin();
    itt += nodeIndex;
    itt = m_transitions.erase(itt);
    int new_selected = nodeIndex;
    if (it == m_nodes.end())
        new_selected = m_nodes.size() - 1;
    setSelection(new_selected, false);
}
