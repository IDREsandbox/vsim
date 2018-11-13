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

	bool isPaused() const;

private:
	void recheckPlayPause(); // enables/disables accordingly
	QString getNarrativeName() const;
	void updateStatusMessage();

	enum CenterMessage {
		NONE,
		CLICK,
		PRESSP,
		PLAYING,
		FINISHED,
		PAUSED,
		STOPPED
	};
	void setCenterMessage(CenterMessage msg);

private:
	VSimApp *m_app;
	NarrativeControl *m_narratives;
	MainWindowTopBar *m_bar;

	//QTimer m_timer;
	bool m_paused;
	bool m_rewind_on_resume;
	double m_time_remaining_sec;
	double m_total_time;
	QString m_status_message;
	QTimer *m_center_msg_timer;
	CenterMessage m_center_msg;

	osg::Matrix m_camera_from;
	osg::Matrix m_camera_to;
};

#endif /* NARRATIVEPLAYER_H */
