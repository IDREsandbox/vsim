#ifndef NARRATIVELIST_H_
#define NARRATIVELIST_H_

#include <set>
#include <osg/Node>
#include "narrative/Narrative.h"
#include "HorizontalScrollBox.h"
#include "MainWindow.h"

class MainWindow;
class NarrativeList : public QObject // why osg? : public osg::Referenced
{
	Q_OBJECT
public:
    NarrativeList(QObject* parent, MainWindow* window);
    virtual ~NarrativeList();

    //NarrativeReference* getSelectedNarrativeReference() { return (m_selection >= 0 ? m_narrativeReferences[m_selection] : NULL); }
 //   int getSelection() { return m_selection; }
 //   void setSelection(int index) { 
	//	assert(index >= -1 && index < (int)m_narrativeReferences.size()); 
	//	m_selection = index;
	//}
	// add an existing narrative to the gui
	void addNarrative(Narrative* narrative);
	//void insertNarrative(Narrative* narrative, int index);
	void addBlankNarrative();

	// opens up the dialog, makes a new narrative
	void newNarrative();

	// 
	void deleteSelection();

	//void deleteSelection(const std::set<int>& selection);
	//void insertNarrative();
 //   void removeNarrative(int index);
	unsigned int getNumNarratives();
	Narrative* getNarrativeReference(unsigned int i);
	void clear();

	void loadFromNode(osg::Node* model);

protected:
	int m_focus;
	//int m_selection;
	std::vector<Narrative*> m_narratives;
	
	osg::Node *m_model;
	MainWindow *m_window;
	HorizontalScrollBox *m_list_gui;
};

#endif /* NARRATIVELIST_H_ */
