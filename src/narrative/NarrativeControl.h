#ifndef NARRATIVECONTROL_H
#define NARRATIVECONTROL_H

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
#include "editButtons.h"

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

	// selection and focus
	void openNarrative(); // if index <0 then it uses the the narrative box selection
	void setNarrative(int index);
	void closeNarrative();
	void openSlide();

	void selectNarratives(std::set<int> narratives);
	void selectSlides(int narrative, std::set<int> slides);
	void selectLabels(int narrative, int slide, std::set<int> labels);

	// this should be const or it is dangerous to have public
	// it means that ppl can change narrative data w/o it being updated by the gui
	Narrative2 *getNarrative(int index);
	NarrativeSlide *getNarrativeSlide(int narrative, int slide);

	void redrawThumbnails(const std::vector<SlideScrollItem*> slides);
	QImage generateThumbnail(int option = 1);

	// Narratives
	void newNarrative();
	void editNarrativeInfo();
	void deleteNarratives();

	// Slides
	void newSlide();
	void deleteSlides();
	void editSlide();
	void setSlideDuration();
	void setSlideTransition();
	void setSlideCamera();
	
public slots:

	//editDlg buttons
	void exitEdit();
	void deleteLabelButton();
	void newLabelButton(QString style);

	//Canvas
	void newLabel(std::string, int idx);
	void moveLabel(QPoint pos, int idx);
	void resizeLabel(QSize size, int idx);
	void textEditLabel(QString str, int idx);
	void deleteLabel(int idx);

private:
	int nextSelectionAfterDelete(int total, std::set<int> selection);

	int m_current_narrative;
	int m_current_slide;

	// std::vector<Narrative*> m_narratives;
	osg::ref_ptr<NarrativeGroup> m_narrative_group; // the osg side data structure, instead of using a vector
	osg::Group *m_model;

	MainWindow *m_window; // TODO: remove this after redesign, this should be completely gui independent
	NarrativeScrollBox *m_narrative_box;
	SlideScrollBox *m_slide_box;
	labelCanvas *m_canvas;
	
	editButtons* editDlg;
	
	QUndoStack *m_undo_stack;
};

enum SelectionCommandWhen {
	ON_UNDO,
	ON_REDO,
	ON_BOTH
};

class SelectNarrativesCommand : public QUndoCommand {
public:
	SelectNarrativesCommand(NarrativeControl *control, std::set<int> narratives, SelectionCommandWhen when = ON_BOTH, QUndoCommand *parent = nullptr);
	void undo();
	void redo();
private:
	NarrativeControl *m_control;
	SelectionCommandWhen m_when;
	std::set<int> m_narratives;
};

class SelectSlidesCommand : public QUndoCommand {
public:
	SelectSlidesCommand(NarrativeControl *control, int narrative, std::set<int> slides, SelectionCommandWhen when = ON_BOTH, QUndoCommand *parent = nullptr);
	void undo();
	void redo();
private:
	NarrativeControl *m_control;
	SelectionCommandWhen m_when;
	int m_narrative;
	std::set<int> m_slides;
};

//class SelectLabelsCommand : public QUndoCommand {
//public:
//	SelectLabelsCommand(NarrativeControl *control, int narrative, int slide, std::set<int> labels, SelectionCommandWhen when = ON_BOTH, QUndoCommand *parent = nullptr);
//	void undo();
//	void redo();
//private:
//	NarrativeControl *m_control;
//	SelectionCommandWhen m_when;
//	int m_narrative;
//	int m_slides;
//	std::set<int> m_labels;
//};

#endif
