#ifndef NARRATIVEPLAYER_H
#define NARRATIVEPLAYER_H

#include <QObject>
#include <osg/Matrix>
#include <QAction>
#include <QTimer>

class NarrativeControl;
class NarrativeSlide;
class MainWindowTopBar;
class VSimApp;

// State machine ish
class NarrativePlayer : public QObject
{
	Q_OBJECT
public:
	NarrativePlayer(VSimApp *app,
		NarrativeControl *narratives,
		MainWindowTopBar *top_bar,
		QObject *parent = nullptr);

public: // actions
	QAction *a_play;
	QAction *a_stop;
	QAction *a_next;
	QAction *a_prev;
	QAction *a_pause;

public:
	// advances the slide or transition, immediately pauses on failure or PauseOnNode
	void next();
	void prev();
	void stop();
	void play();
	void pause();

	// state transitions
	void toTransitioning(int index);
	void toAtNode();
	void toStopped();

	void update(double dt_sec);

private:
	void recheckPlayPause(); // enables/disables accordingly

private:
	VSimApp *m_app;
	NarrativeControl *m_narratives;
	MainWindowTopBar *m_bar;

	//QTimer m_timer;
	bool m_paused;
	double m_time_remaining_sec;
	double m_total_time;

	osg::Matrix m_camera_from;
	osg::Matrix m_camera_to;
};

#endif /* NARRATIVEPLAYER_H */
