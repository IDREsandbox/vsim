#ifndef NARRATIVELIST_H_
#define NARRATIVELIST_H_

#include <set>
#include <osg/Node>
#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative2.h"
#include "narrative/NarrativeScrollBox.h"
#include "narrative/SlideScrollBox.h"
#include "narrative/NarrativeSlideLabels.h"
#include "HorizontalScrollBox.h"
#include "MainWindow.h"
#include "dragLabel.h"

// Interface to the underlying osg data for narratives
// Exactly one should exist per VSimApp
// TODO: convert to QAbstractItemModel, remove focus state, remove thumbnail code
// TODO: undo/redo
class NarrativeControl : public QObject
{
	Q_OBJECT
public:
	NarrativeControl(QObject *parent, MainWindow *window);
	virtual ~NarrativeControl();

	// initializes gui from osg data, pass in a group of Narratives
	//void load(osg::Group *narratives);
	//void loadSlides(Narrative2 *narrative);
	void load(NarrativeGroup *narratives);

	//void showSlides(int index);
	void openNarrative();
	void closeNarrative();

	// this should be const or it is dangerous to have public
	// it means that ppl can change narrative data w/o it being updated by the gui
	Narrative2 *getNarrative(int index);
	NarrativeSlide *getNarrativeSlide(int narrative, int slide);

	void redrawThumbnails(const std::vector<SlideScrollItem*>& slides);
	QImage generateThumbnail();

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
	void setSlideDuration();
	void setSlideTransition();
	void setSlideCamera();
	
	//Canvas
	//void newLabel(int idx);
	//	Delete Box
	//	Move Box
	//	Edit Box Content

public slots:
	//Canvas
	void newLabel(std::string, int idx);
	void moveLabel(QPoint pos, int idx);
	void resizeLabel(QSize size, int idx);
	void textEditLabel(QString str, int idx);
	//	Delete Box
	//	Move Box
	//	Edit Box Content
	void openSlide();

private:
	int m_current_narrative;
	int m_current_slide;

	// std::vector<Narrative*> m_narratives;
	osg::ref_ptr<NarrativeGroup> m_narrative_group; // the osg side data structure, instead of using a vector
	osg::Group *m_model;

	MainWindow *m_window; // TODO: remove this after redesign, this should be completely gui independent
	NarrativeScrollBox *m_narrative_box;
	SlideScrollBox *m_slide_box;
	labelCanvas *m_canvas;

	QUndoStack *m_undo_stack;
};

#endif /* NARRATIVELIST_H_ */
