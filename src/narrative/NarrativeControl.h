#ifndef NARRATIVECONTROL_H
#define NARRATIVECONTROL_H

#include <set>
#include <osg/Node>
#include <QWidget>
#include <QUndoStack>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include "SelectionStack.h"
#include "Core/Command.h"
#include "Core/ICommandStack.h"

class VSimApp;
class MainWindow;
class MainWindowTopBar;
class NarrativeGroup;
class Narrative;
class NarrativeSlide;
class NarrativeSlideItem;
class NarrativeSlideLabel;
class NarrativeScrollBox;
class SlideScrollBox;
class SlideScrollItem;
class SelectionStack;
class OSGViewerWidget;

class CanvasItem;
class CanvasEditor;
class CanvasContainer;

// Bridges osg and gui for narratives
class NarrativeControl : public QObject
{
	Q_OBJECT;
public:
	NarrativeControl(VSimApp *app, MainWindow *window, QObject *parent = nullptr);

	// initializes gui from osg data
	void load(NarrativeGroup *narratives);

	// poll thumbnails
	void update(double dt);

	// Opens narrative in slide box
	// - closes slide if not already open
	// - hide on -1
	void openNarrative(int index); // opens narrative in slide box

	// Opens slide in canvas
	// - requires a current/selected narrative
	// - forces selection, shows canvas
	// - hides on -1 or no narrative
	// - returns true if open, false if failed
	// - doesn't change state
	// go: move camera to position
	bool openSlide(int index, bool go = true);

	void showNarrativeBox();
	void showSlideBox();
	bool showingNarrativeBox() const;
	void showCanvas(bool show, bool fade = false);
	bool canvasVisible() const;
	void showCanvasEditor(bool show);
	void fadeOutSlide(NarrativeSlide *slide);

	void editSlide();
	void exitEdit();
	//void editStyleSettings();
	//void onStylesChanged();

	//bool advance(bool forward);
	bool advanceSlide(bool forward, bool instant = true);

	// change state
	void selectNarratives(const SelectionData &narratives);
	void selectSlides(int narrative, const SelectionData &slides, bool edit);
	//void selectLabel(int narrative, int slide, NarrativeSlideItem *label);
	//void selectLabels(int narrative, int slide, const std::set<CanvasItem*> &labels);

	void singleSelectOpenSlide();

	NarrativeGroup *narratives() const;
	Narrative *getCurrentNarrative() const;
	int getCurrentNarrativeIndex() const;
	std::set<size_t> getSelectedNarratives() const;

	// current slide is an opened slide
	// top slide is the last selected
	NarrativeSlide *getCurrentSlide() const;
	NarrativeSlide *slideAtIndex(int index) const;
	int getCurrentSlideIndex() const;
	SelectionData getSelectedSlides() const;
	int getTopSlide() const;
	
	Narrative *getNarrative(int index) const;
	NarrativeSlide *getSlide(int narrative, int slide) const;
	//NarrativeSlideLabel *getLabel(int narrative, int slide, int label) const;

signals:
	//void selectionChanged(); // this should happen after any edit event, 
	void sEditEvent();

public: // Actions
	// Narratives
	QAction *a_new_narrative;
	QAction *a_narrative_info;
	QAction *a_delete_narratives;
	QAction *a_open_narrative;
	void newNarrative();
	void editNarrativeInfo();
	void deleteNarratives();
	void moveNarratives(const std::vector<std::pair<size_t, size_t>> &mapping);
	void mergeNarratives(const NarrativeGroup *g);

	// Slides
	QAction *a_new_slide;
	QAction *a_delete_slides;
	QAction *a_slide_duration;
	QAction *a_slide_transition;
	QAction *a_slide_camera;
	QAction *a_edit_slide;
	void newSlide();
	void deleteSlides();
	void setSlideDuration();
	void setSlideTransition();
	void setSlideCamera();
	void moveSlides(const std::vector<std::pair<size_t, size_t>> &);

	// Canvas

	// Thumbnails
	void clearAllThumbnails();
	void onThumbnailReady(QImage img);

	void debug();

private:
	void enableEditing(bool enable);

private: // classes
	class SelectNarrativesCommand : public QUndoCommand {
	public:
		SelectNarrativesCommand(NarrativeControl *control, const SelectionData &narratives, Command::When when = Command::ON_BOTH, QUndoCommand *parent = nullptr);
		void undo() override;
		void redo() override;
	private:
		NarrativeControl * m_control;
		Command::When m_when;
		SelectionData m_narratives;
	};

	class SelectSlidesCommand : public QUndoCommand {
	public:
		SelectSlidesCommand(NarrativeControl *control,
			int narrative, const SelectionData &slides,
			Command::When when = Command::ON_BOTH,
			bool edit = false,
			QUndoCommand *parent = nullptr);
		void undo() override;
		void redo() override;
	private:
		NarrativeControl * m_control;
		Command::When m_when;
		int m_narrative;
		SelectionData m_slides;
		bool m_edit;
	};

	class CanvasStackWrapper : public ICommandStack {
	public:
		CanvasStackWrapper(NarrativeControl *control);
		// overrides
		QUndoCommand *begin() override;
		void end() override;
	private:
		NarrativeControl * m_control;
		QUndoStack *m_stack;
		SelectSlidesCommand *m_cmd;
	};

private:
	VSimApp *m_app;
	int m_current_narrative; // opened narrative
	int m_current_slide; // active canvas slide
	bool m_editing_slide;
	bool m_canvas_enabled;
	bool m_show_slides; // show narratives or slides

	QGraphicsOpacityEffect *m_fade_out;
	QGraphicsOpacityEffect *m_fade_in;
	QPropertyAnimation *m_fade_out_anim;
	QPropertyAnimation *m_fade_in_anim;

	NarrativeGroup *m_narrative_group;

	MainWindow *m_window;
	MainWindowTopBar *m_bar;
	NarrativeScrollBox *m_narrative_box;
	SelectionStack *m_narrative_selection;
	SlideScrollBox *m_slide_box;
	SelectionStack *m_slide_selection;

	CanvasEditor *m_canvas;
	CanvasContainer *m_fade_canvas;

	QUndoStack *m_undo_stack;
	CanvasStackWrapper m_canvas_stack_wrapper;

	// thumbnails
	std::unique_ptr<CanvasContainer> m_thumbnail_canvas;
	OSGViewerWidget *m_viewer;
	QSize m_thumbnail_size;

	// slide currently being painted
	std::weak_ptr<NarrativeSlide> m_thumbnail_slide;
};

#endif
