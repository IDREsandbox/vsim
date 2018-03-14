#ifndef NARRATIVECONTROL_H
#define NARRATIVECONTROL_H

#include <set>
#include <osg/Node>
#include <QWidget>
#include <QUndoStack>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include "SelectionStack.h"

class VSimApp;
class MainWindow;
class NarrativeGroup;
class Narrative2;
class NarrativeSlide;
class NarrativeSlideItem;
class NarrativeSlideLabel;
class NarrativeScrollBox;
class SlideScrollBox;
class NarrativeCanvas;
class editButtons;
class SlideScrollItem;
class SelectionStack;

// Bridges osg and gui for narratives
class NarrativeControl : public QObject
{
	Q_OBJECT
public:
	NarrativeControl(VSimApp *app, MainWindow *window, QObject *parent = nullptr);

	// initializes gui from osg data
	void load(NarrativeGroup *narratives);
	void loadNarratives(NarrativeGroup *group);

	// Selection
	void openNarrative(); // if index <0 then it uses the the narrative box selection
	void setNarrative(int index);

	// select slide, show canvas
	// hides if index out of range or narrative not open
	// returns true if open, false if failed
	bool openSlide(int index, bool go = true, bool fade = false);

	void editSlide();
	void exitEdit();
	void editStyleSettings();

	//bool advance(bool forward);
	bool advanceSlide(bool forward, bool instant = true);
	void showCanvas(bool show, bool fade = false);
	void showCanvasEditor(bool show);

	enum SelectionLevel {
		NARRATIVES,
		SLIDES,
		LABELS
	};
	SelectionLevel getSelectionLevel();
	void selectNarratives(const SelectionData &narratives);
	void selectSlides(int narrative, const SelectionData &slides);
	//void selectLabel(int narrative, int slide, NarrativeSlideItem *label);
	void selectLabels(int narrative, int slide, const std::set<NarrativeSlideItem *> &labels);

	Narrative2 *getCurrentNarrative();
	int getCurrentNarrativeIndex();
	std::vector<Narrative2*> getSelectedNarratives() const;

	NarrativeSlide *getCurrentSlide();
	int getCurrentSlideIndex();
	
	Narrative2 *getNarrative(int index);
	NarrativeSlide *getSlide(int narrative, int slide);
	NarrativeSlideLabel *getLabel(int narrative, int slide, int label);

signals:
	//void selectionChanged(); // this should happen after any edit event, 
	void sEditEvent();

public: // Actions
	// Narratives
	void newNarrative();
	void editNarrativeInfo();
	void deleteNarratives();
	void moveNarratives(const std::vector<std::pair<size_t, size_t>> &mapping);

	// Slides
	void newSlide();
	void deleteSlides();
	void setSlideDuration();
	void setSlideTransition();
	void setSlideCamera();
	void moveSlides(const std::vector<std::pair<size_t, size_t>> &);

	// Labels
	//editDlg buttons
	void newLabel(int style); // LabelStyle::Style
	void deleteLabels();
	void transformLabels(const std::map<NarrativeSlideItem *, QRectF> &rects);
	void labelEdited(NarrativeSlideLabel *label);
	void execEditLabel();

	// Thumbnails
	void dirtyCurrentSlide();
	void redrawThumbnails(const std::vector<NarrativeSlide*> slides);
	//QImage generateThumbnail(int option = 1);
	QImage generateThumbnail(NarrativeSlide *slide);

	void debug();

private:
	void enableEditing(bool enable);

private:
	VSimApp * m_app;
	int m_current_narrative; // opened narrative
	int m_current_slide; // active canvas slide
	bool m_editing_slide;
	bool m_force_select;
	bool m_editing_enabled;
	bool m_canvas_enabled;

	QGraphicsOpacityEffect *m_fade_out;
	QGraphicsOpacityEffect *m_fade_in;
	QPropertyAnimation *m_fade_out_anim;
	QPropertyAnimation *m_fade_in_anim;

	osg::ref_ptr<NarrativeGroup> m_narrative_group;

	MainWindow *m_window;
	NarrativeScrollBox *m_narrative_box;
	SelectionStack *m_narrative_selection;
	SlideScrollBox *m_slide_box;
	SelectionStack *m_slide_selection;
	NarrativeCanvas *m_canvas;
	NarrativeCanvas *m_fade_canvas;

	editButtons* m_label_buttons;

	QUndoStack *m_undo_stack;
};


enum SelectionCommandWhen {
	ON_UNDO,
	ON_REDO,
	ON_BOTH
};

class SelectNarrativesCommand : public QUndoCommand {
public:
	SelectNarrativesCommand(NarrativeControl *control, const SelectionData &narratives, SelectionCommandWhen when = ON_BOTH, QUndoCommand *parent = nullptr);
	void undo();
	void redo();
private:
	NarrativeControl *m_control;
	SelectionCommandWhen m_when;
	SelectionData m_narratives;
};

class SelectSlidesCommand : public QUndoCommand {
public:
	SelectSlidesCommand(NarrativeControl *control, int narrative, const SelectionData &slides, SelectionCommandWhen when = ON_BOTH, QUndoCommand *parent = nullptr);
	void undo();
	void redo();
private:
	NarrativeControl *m_control;
	SelectionCommandWhen m_when;
	int m_narrative;
	SelectionData m_slides;
};

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
