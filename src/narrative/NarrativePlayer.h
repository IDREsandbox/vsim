#ifndef NARRATIVEPLAYER_H
#define NARRATIVEPLAYER_H

#include "narrative/NarrativeControl.h"
#include "narrative/NarrativeSlide.h"
#include "MainWindow.h"
#include <QObject>
#include <osg/Matrix>
#include "labelCanvas.h"

// NarrativePlayer owns the camera
class NarrativePlayer : public QObject
{
	Q_OBJECT
public:
	NarrativePlayer(QObject *parent, MainWindow *window, NarrativeControl *narratives);

	void update(double dt_sec);
	void play();
	void pause();
	// advances the slide or transition, immediately pauses on failure or PauseOnNode
	void next();

	bool isPlaying();

	void setCameraMatrix(osg::Matrixd camera_matrix);

	// goto and pause at a slide
	// set the slide selection
	// bool setSlide(int narrative, int slide); // TODO

public: //slots
	void selectionChange();

private:
	// pointers
	MainWindow *m_window;
	NarrativeControl *m_narratives;
	NarrativeScrollBox *m_narrative_box;
	SlideScrollBox *m_slide_box;

	// important stuff
	int m_current_narrative;
	int m_current_slide;
	bool m_playing;
	bool m_transitioning; // transitions are BEFORE current narrative and slide
	double m_slide_time_sec;

	double m_previous_time;
	QTimer *m_timer; // frame timer, used for updates

	//void figureOutFrozenCamera(); // locks the camera if playing or frozen
	// remembers the previous navigation mode to switch back to after finishing playing
	OSGViewerWidget::NavigationMode m_old_navigation_mode;
	labelCanvas* m_canvas;
};

#endif /* NARRATIVEPLAYER_H */
