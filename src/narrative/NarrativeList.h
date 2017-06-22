#ifndef NARRATIVELIST_H_
#define NARRATIVELIST_H_

#include <set>
#include <osg/Node>
#include "narrative/Narrative.h"
#include "HorizontalScrollBox.h"
#include "MainWindow.h"

class NarrativeList : public QObject
{
	Q_OBJECT
public:
    NarrativeList(QObject* parent, MainWindow* window);
    virtual ~NarrativeList();

	// opens up the dialog, makes a new narrative
	void newNarrative();
	
	// deletes the gui selection
	void deleteSelection();

	//void deleteSelection(const std::set<int>& selection);
	//void insertNarrative();
	//void removeNarrative(int index);

	// initializes gui from osg data, also converts old narratives to new
	void load(osg::Group* model);

protected:
	int m_focus;

	// std::vector<Narrative*> m_narratives;
	osg::Group *m_narrative_group; // the osg side data structure, instead of using a vector
	osg::Group *m_model;

	MainWindow *m_window;
	HorizontalScrollBox *m_list_gui;
};

#endif /* NARRATIVELIST_H_ */
