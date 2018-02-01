#ifndef NARRATIVECONTROL_H
#define NARRATIVECONTROL_H

#include <set>
#include <osg/Node>
#include <QWidget>
#include <QUndoStack>
#include "MainWindow.h"

class VSimApp;
class MainWindow;
class NarrativeGroup;
class Narrative2;
class NarrativeSlide;
class NarrativeSlideItem;
class NarrativeSlideLabel;
class NarrativeScrollBox;
class SlideScrollBox;
//class labelCanvas;
class NarrativeCanvas;
class editButtons;
class SlideScrollItem;

// Bridges osg and gui for narratives
class NarrativeControl : public QObject
{
	Q_OBJECT
public:
	NarrativeControl(VSimApp *app, MainWindow *window, QObject *parent = nullptr);
	~NarrativeControl();

	// initializes gui from osg data
	void load(NarrativeGroup *narratives);

	// Selection
	void openNarrative(); // if index <0 then it uses the the narrative box selection
	void setNarrative(int index);

	bool setSlide(int index, bool instant = true);

	//bool advance(bool forward);
	bool advanceSlide(bool forward, bool instant = true);
	void showCanvas(bool instant);
	void hideCanvas(bool instant);

	enum SelectionLevel {
		NARRATIVES,
		SLIDES,
		LABELS
	};
	SelectionLevel getSelectionLevel();
	void selectNarratives(std::set<int> narratives);
	void selectSlides(int narrative, std::set<int> slides);
	//void selectLabel(int narrative, int slide, NarrativeSlideItem *label);
	void selectLabels(int narrative, int slide, const std::set<NarrativeSlideItem*> &labels);

	int getCurrentNarrativeIndex();
	int getCurrentSlideIndex();
	Narrative2 *getCurrentNarrative();
	NarrativeSlide *getCurrentSlide();
	Narrative2 *getNarrative(int index);
	NarrativeSlide *getSlide(int narrative, int slide);
	NarrativeSlideLabel *getLabel(int narrative, int slide, int label);

	void onSlideSelection();

signals:
	//void selectionChanged(); // this should happen after any edit event, 
	void sEditEvent();

public:

	void redrawThumbnails(const std::vector<SlideScrollItem*> slides);
	//QImage generateThumbnail(int option = 1);
	QImage generateThumbnail(NarrativeSlide *slide);

	// Narratives
	void newNarrative();
	void editNarrativeInfo();
	void deleteNarratives();
	void moveNarratives(std::set<int> from, int to);
	// for importing narratives
	void loadNarratives(NarrativeGroup *group);

	// Slides
	void newSlide();
	void deleteSlides();
	void editSlide();
	void setSlideDuration();
	void setSlideTransition();
	void setSlideCamera();
	void moveSlides(std::set<int> from, int to);

	// Labels
	//editDlg buttons
	void newLabel();
	void exitEdit();
	void deleteLabels();
	void transformLabels(const std::map<NarrativeSlideItem *, QRectF> &rects);
	void labelEdited(NarrativeSlideLabel *label);

	void editStyleSettings();

	void debug();

private:
	VSimApp * m_app;
	int m_current_narrative; // opened narrative
	int m_current_slide; // active canvas slide
	bool m_editing_slide;

	// std::vector<Narrative*> m_narratives;
	osg::ref_ptr<NarrativeGroup> m_narrative_group;
	//osg::ref_ptr<LabelStyleGroup> m_style_group;

	MainWindow *m_window;
	NarrativeScrollBox *m_narrative_box;
	SlideScrollBox *m_slide_box;
	NarrativeCanvas *m_canvas;
	
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

//class SelectLabelCommand : public QUndoCommand {
//public:
//	SelectLabelCommand(NarrativeControl *control, int narrative, int slide, NarrativeSlideItem *label, SelectionCommandWhen when = ON_BOTH, QUndoCommand *parent = nullptr);
//	void undo();
//	void redo();
//private:
//	NarrativeControl *m_control;
//	SelectionCommandWhen m_when;
//	int m_narrative;
//	int m_slide;
//	int m_label;
//};

class SelectLabelsCommand : public QUndoCommand {
public:
	SelectLabelsCommand(NarrativeControl *control, int narrative, int slide, std::set<NarrativeSlideItem *> labels, SelectionCommandWhen when = ON_BOTH, QUndoCommand *parent = nullptr);
	void undo();
	void redo();
private:
	NarrativeControl * m_control;
	SelectionCommandWhen m_when;
	int m_narrative;
	int m_slide;
	std::set<NarrativeSlideItem*> m_labels;
};

#endif
