#ifndef NARRATIVELIST_H_
#define NARRATIVELIST_H_

#include <set>
#include <osg/Node>
#include "narrative/Narrative.h"
#include "narrative/NarrativeScrollBox.h"
#include "narrative/SlideScrollBox.h"
#include "HorizontalScrollBox.h"
#include "MainWindow.h"

class NarrativeControl : public QObject
{
	Q_OBJECT
public:
    NarrativeControl(QObject *parent, MainWindow *window);
    virtual ~NarrativeControl();

	// initializes gui from osg data, also converts old narratives to new
	void load(osg::Group *model);
	void loadSlides(Narrative *narrative);

	//void showSlides(int index);
	void openNarrative();
	void closeNarrative();

	Narrative *getNarrative(int index);
	NarrativeNode *getNarrativeNode(int narrative, int slide);

	// All Commands -
	// Narratives
	//	New Narrative
	//	Delete Narrative
	//	Set Narrative Info
	//	(Move Narrative)
	//	Duplicate Narrative
	//	Import Narrative
	void newNarrative();
	void editNarrativeInfo();
	void deleteNarratives();

	//Slides
	//	New Slide
	//	Delete Slides
	//	Set Slide Duration
	//	Set Slide Transition
	//  Set Slide Camera
	//	Move Slides
	//	Duplicate Slides
	// these act on currently selected items
	// TODO: versions using sets, ex. deleteSlides(std::set<int>)
	void newSlide();
	void deleteSlides();
	void editSlide();
	void setSlideDuration(float);
	void setSlideTransition(float);
	void setSlideCamera();
	
	//Canvas
	//	New Box
	//	Delete Box
	//	Move Box
	//	Edit Box Content

private:
	void addToGui(Narrative *);
	void addNodeToGui(NarrativeNode *);
	QImage generateThumbnail();

	int m_current_narrative;
	int m_current_slide;

	// std::vector<Narrative*> m_narratives;
	osg::Group *m_narrative_group; // the osg side data structure, instead of using a vector
	osg::Group *m_model;

	MainWindow *m_window;
	NarrativeScrollBox *m_narrative_box;
	SlideScrollBox *m_slide_box;
	labelCanvas *m_canvas;
};

#endif /* NARRATIVELIST_H_ */
