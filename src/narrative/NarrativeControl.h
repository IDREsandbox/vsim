#ifndef NARRATIVECONTROL_H
#define NARRATIVECONTROL_H

#include <set>
#include <osg/Node>
#include <QWidget>
#include <QUndoStack>
#include "MainWindow.h"

class MainWindow;
class NarrativeGroup;
class Narrative2;
class NarrativeSlide;
class NarrativeSlideLabels;
class NarrativeScrollBox;
class SlideScrollBox;
class labelCanvas;
class editButtons;
class SlideScrollItem;

// Bridges osg and gui for narratives
class NarrativeControl : public QObject
{
	Q_OBJECT
public:
	NarrativeControl(QObject *parent, MainWindow *window);
	~NarrativeControl();

	// initializes gui from osg data
	void load(NarrativeGroup *narratives);

	// Selection
	void openNarrative(); // if index <0 then it uses the the narrative box selection
	void setNarrative(int index);

	bool setSlide(int index);

	enum SelectionLevel {
		NARRATIVES,
		SLIDES,
		LABELS
	};
	SelectionLevel getSelectionLevel();
	void selectNarratives(std::set<int> narratives);
	void selectSlides(int narrative, std::set<int> slides);
	void selectLabel(int narrative, int slide, int label);

	int getCurrentNarrativeIndex();
	int getCurrentSlideIndex();
	Narrative2 *getCurrentNarrative();
	NarrativeSlide *getCurrentSlide();
	Narrative2 *getNarrative(int index);
	NarrativeSlide *getSlide(int narrative, int slide);
	NarrativeSlideLabels *getLabel(int narrative, int slide, int label);

	void onSlideSelection();

signals:
	void selectionChanged(); // this should happen after any edit event, 

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

public slots:

	//editDlg buttons
	void exitEdit();
	void deleteLabelButton();

	void editStyleSettings();

	//Canvas
	void newH1();
	void newH2();
	void newBod();
	void newLab();
	void newImg();

	void newLabel(const std::string &text, const std::string &style, const std::string &widget_style, float height, float width, int margin);
	void deleteLabel(int idx);
	void moveLabel(float rx, float ry, int idx);
	void resizeLabel(float rw, float rh, int idx);
	void editLabel(int idx);
	
	void debug();

private:

	int m_current_narrative;
	int m_current_slide;

	// std::vector<Narrative*> m_narratives;
	osg::ref_ptr<NarrativeGroup> m_narrative_group; // the osg side data structure, instead of using a vector
	//osg::ref_ptr<LabelStyleGroup> m_style_group;

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

class SelectLabelCommand : public QUndoCommand {
public:
	SelectLabelCommand(NarrativeControl *control, int narrative, int slide, int label, SelectionCommandWhen when = ON_BOTH, QUndoCommand *parent = nullptr);
	void undo();
	void redo();
private:
	NarrativeControl *m_control;
	SelectionCommandWhen m_when;
	int m_narrative;
	int m_slide;
	int m_label;
};

#endif
