/*
 * NarrativeList.cpp
 *
 *  Created on: Mar 26, 2011
 *      Author: eduardo
 */

#include "narrative/NarrativeList.h"
#include "narrative/Narrative.h"

NarrativeList::NarrativeList(): m_selection(-1)
{

}

NarrativeList::~NarrativeList()
{

}

void NarrativeList::addNarrative(NarrativeReference* narrative)
{
    m_narrativeReferences.push_back(osg::ref_ptr<NarrativeReference>(narrative));
    setSelection(m_narrativeReferences.size() - 1);
}

void NarrativeList::removeNarrative(int index)
{
    assert(index >= 0 && index < (int)m_narrativeReferences.size());
    NarrativeReferenceVector::iterator it = m_narrativeReferences.begin();
    it += index;
    it = m_narrativeReferences.erase(it);
    int new_selected = index;
    if (it == m_narrativeReferences.end())
        new_selected = m_narrativeReferences.size() - 1;
    setSelection(new_selected);
}
