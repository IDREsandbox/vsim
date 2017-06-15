#include "narrative/NarrativeList.h"
#include "narrative/Narrative.h"


NarrativeList::NarrativeList(MainWindow *window) : m_window(window), m_selection(-1)
{
	m_list_gui = window->ui.narratives;
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
