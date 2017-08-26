#ifndef NARRATIVEPLAYER_H
#define NARRATIVEPLAYER_H

#include <QObject>
#include <osg/Matrix>

class NarrativeControl;
class NarrativeSlide;

// State machine ish
class NarrativePlayer : public QObject
{
	Q_OBJECT
public:
	NarrativePlayer(QObject *parent, NarrativeControl *narratives);
	enum State {
		STOPPED,
		ATNODE,
		TRANSITIONING
	};
	enum Advance {
		FORWARD,
		BACKWARD
	};

	// slots
	void play();
	void stop();
	void update(double dt_sec);
	void rightArrow();
	void leftArrow();
	//void leftClick();
	void timerExpire(); // our own timers
	void editEvent(); // selection changes

	// advances the slide or transition, immediately pauses on failure or PauseOnNode
	void next(Advance fwdback);

	// state transitions
	void toTransitioning();
	void toAtNode();
	void toStopped();

	// signals out, if you want to remove dependency then replace with signals
	bool setSlide(int index);
	void setCameraInTransition(double t);

signals: // more signals out, removes the viewer widget dependency
	void updateCamera(osg::Matrixd camera_matrix);
	void enableNavigation(bool enable); // TODO
	void hideCanvas();
	void showCanvas();


private:
	// pointers
	NarrativeControl *m_narratives;

	State m_state;
	double m_slide_time_sec;

	// This is so that calling setSelection doesn't cause us to stop
	bool m_expect_selection_change;

	//QGraphicsOpacityEffect* effect;

	//void figureOutFrozenCamera(); // locks the camera if playing or frozen
	// remembers the previous navigation mode to switch back to after finishing playing
	//OSGViewerWidget::NavigationMode m_old_navigation_mode;
	//labelCanvas* m_canvas;
};

#endif /* NARRATIVEPLAYER_H */
