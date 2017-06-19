/*
 * NarrativeList.h
 *
 *  Created on: Mar 26, 2011
 *      Author: eduardo
 */

#ifndef NARRATIVELIST_H_
#define NARRATIVELIST_H_

#include <assert.h>
#include "narrative/NarrativeReference.h"
#include "HorizontalScrollBox.h"
#include "MainWindow.h"

class MainWindow;
class NarrativeList // why osg? : public osg::Referenced
{
public:
	NarrativeList();
    NarrativeList(MainWindow*);
    virtual ~NarrativeList();

    NarrativeReference* getSelectedNarrativeReference() { return (m_selection >= 0 ? m_narrativeReferences[m_selection] : NULL); }
    int getSelection() { return m_selection; }
    void setSelection(int index) { 
		assert(index >= -1 && index < (int)m_narrativeReferences.size()); 
		m_selection = index;
	}
    void addNarrative(NarrativeReference* narrative);
    void removeNarrative(int index);
    unsigned int getNumNarratives() { return m_narrativeReferences.size(); }
    NarrativeReference* getNarrativeReference(unsigned int i) { assert(i < m_narrativeReferences.size()); return m_narrativeReferences[i]; }
    void clear() { m_narrativeReferences.clear(); setSelection(-1); }

protected:
    int m_selection;
    typedef std::vector<osg::ref_ptr<NarrativeReference>> NarrativeReferenceVector;
    NarrativeReferenceVector m_narrativeReferences;

	MainWindow *m_window;
	HorizontalScrollBox *m_list_gui;
};

#endif /* NARRATIVELIST_H_ */
