#ifndef NARRATIVEPLAYER_H
#define NARRATIVEPLAYER_H

#include "narrative/NarrativeControl.h"
#include "narrative/NarrativeNode.h"
#include "MainWindow.h"
#include <QObject>
#include <osg/Matrix>
#include <osgGA/FirstPersonManipulator>

class NarrativePlayer : public QObject
{
	Q_OBJECT
public:
	NarrativePlayer(QObject *parent, MainWindow *window, NarrativeControl *narratives);

	void update(double dt_sec);
	void play();
	void pause();

	bool isPlaying();

	void setViewMatrix(osg::Matrixd view_matrix);

public: //slots
	void selectionChange();

private:
	// advances the slide or transition, immediately pauses on failure or PauseOnNode
	void next();
	
	MainWindow *m_window;
	NarrativeControl *m_narratives;

	// important stuff
	int m_current_narrative;
	int m_current_slide;
	bool m_playing;
	bool m_transitioning; // transitions are BEFORE current narrative and slide
	double m_slide_time_sec;

	double m_previous_time;
	QTimer *m_timer; // frame timer, used for updates

	//osg::ref_ptr<osgGA::StandardManipulator> m_manipulator;

	// convenience pointers
	NarrativeScrollBox *m_narrative_box;
	SlideScrollBox *m_slide_box;
};

#endif /* NARRATIVEPLAYER_H */
