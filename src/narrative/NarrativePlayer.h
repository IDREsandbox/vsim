#ifndef NARRATIVEPLAYER_H
#define NARRATIVEPLAYER_H

#include "narrative/NarrativeControl.h"
#include "narrative/NarrativeSlide.h"
#include "MainWindow.h"
#include <QObject>
#include <osg/Matrix>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/TerrainManipulator>
#include "SimpleCameraManipulator.h"

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

	enum NavigationMode {
		NAVIGATION_FIRST_PERSON,
		NAVIGATION_FLIGHT,
		NAVIGATION_OBJECT
	};

	void setCameraMatrix(osg::Matrixd camera_matrix);
	void setNavigationMode(NavigationMode mode);
	void freezeCamera(bool freeze);

	// goto and pause at a slide
	// set the slide selection
	bool setSlide(int narrative, int slide); // TODO

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

	// camera manipulators
	NavigationMode m_navigation_mode;
	osg::ref_ptr<SimpleCameraManipulator> m_simple_manipulator; // this one has no controls	
	osg::ref_ptr<osgGA::FirstPersonManipulator> m_first_person_manipulator;
	osg::ref_ptr<osgGA::FirstPersonManipulator> m_flight_manipulator;
	osg::ref_ptr<osgGA::TerrainManipulator> m_terrain_manipulator;

	bool m_frozen;

	void figureOutFrozenCamera(); // locks the camera if playing or frozen

};

#endif /* NARRATIVEPLAYER_H */
