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
	//QAction a_prev;

public:
	// advances the slide or transition, immediately pauses on failure or PauseOnNode
	void next();
	void stop();
	void play();

	// state transitions
	void toTransitioning();
	void toAtNode();
	void toStopped();

private:
	VSimApp *m_app;
	NarrativeControl *m_narratives;

	QTimer m_timer;
};

#endif /* NARRATIVEPLAYER_H */
