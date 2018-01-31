#include <QDebug>
#include <Util.h>
#include "NarrativePlayer.h"

#include "narrative/NarrativeControl.h"
#include "narrative/NarrativeSlide.h"

#include <osg/io_utils>

NarrativePlayer::NarrativePlayer(QObject *parent, NarrativeControl *narratives)
	: QObject(parent),
	m_narratives(narratives),
	m_state(STOPPED),
	m_slide_time_sec(0),
	m_expect_selection_change(false)
{
	connect(m_narratives, &NarrativeControl::sEditEvent, this, &NarrativePlayer::editEvent);
}
void NarrativePlayer::update(double dt_sec)
{
	if (m_state == STOPPED) return;
	if (m_state == ATNODE && m_narratives->getCurrentSlide()->getStayOnNode()) return;

	NarrativeSlide *slide = m_narratives->getCurrentSlide();
	if (slide == nullptr) {
		qDebug() << "Narrative Player - update, current slide is null";
		toStopped();
		return;
	}

	if (m_state == ATNODE) {
		//qDebug() << "atnode" << m_slide_time_sec / slide->getDuration();
		m_slide_time_sec += dt_sec;
		if (m_slide_time_sec >= slide->getDuration()) {
			timerExpire();
		}
	}
	else { // TRANSITIONING
		m_slide_time_sec += dt_sec;
		double t = m_slide_time_sec / slide->getTransitionDuration();
		//qDebug() << "transition" << t;
		//effect->setOpacity(1 - t - .5);
		//m_window->canvasView()->setGraphicsEffect(effect);
		setCameraInTransition(t);

		if (t >= 1.0) {
			timerExpire();
		}
	}
}

void NarrativePlayer::rightArrow()
{
	next();
}

void NarrativePlayer::leftArrow()
{
	//next(false);
}

void NarrativePlayer::leftClick()
{
	next();
}

void NarrativePlayer::timerExpire()
{
	next();
}

void NarrativePlayer::editEvent()
{
	// if we caused the selection change then don't do anything
	if (m_expect_selection_change) {
		return;
	}
	// update camera and everything
	NarrativeSlide *slide = m_narratives->getCurrentSlide();
	qDebug() << "edit event - current slide" << m_narratives->getCurrentSlideIndex();
	if (slide) {;
		updateCamera(slide->getCameraMatrix());
	}

	toStopped();
}

void NarrativePlayer::play()
{
	if (m_state == ATNODE && m_narratives->getCurrentSlide()->getStayOnNode()) {
		qDebug() << "play - continue";
		next();
		return;
	}
	if (m_state == STOPPED) {
		qDebug() << "play";
		emit enableNavigation(false);
		toAtNode();
	}
}

void NarrativePlayer::stop()
{
	qDebug() << "stop";
	toStopped();
}

void NarrativePlayer::next()
{
	qInfo() << "Narrative Player - next";

	if (m_state == ATNODE) {
		toTransitioning();
	}
	else if (m_state == TRANSITIONING) {
		toAtNode();
	}
}

void NarrativePlayer::toTransitioning()
{
	bool ok = advanceSlide(true);

	if (!ok) {
		// couldn't advance
		qDebug() << "failed to change slide, stopping";
		toStopped();
		return;
	}

	hideCanvas();
	m_slide_time_sec = 0;
	m_state = TRANSITIONING;
}

void NarrativePlayer::toAtNode()
{
	showCanvas();
	emit updateCamera(m_narratives->getCurrentSlide()->getCameraMatrix());
	m_slide_time_sec = 0;
	m_state = ATNODE;
}

void NarrativePlayer::toStopped()
{
	if (m_state == STOPPED) return;
	qDebug() << "to stopped";
	m_state = STOPPED;
	showCanvas();
	emit enableNavigation(true);
}

bool NarrativePlayer::advanceSlide(bool forward)
{
	// try to advance the narrative control
	m_expect_selection_change = true;
	bool ok = m_narratives->advanceSlide(forward);
	m_expect_selection_change = false;
	if (!ok) return false;
	return true;
}

void NarrativePlayer::hideCanvas()
{
	m_narratives->hideCanvas(true);
}

void NarrativePlayer::showCanvas()
{
	m_narratives->showCanvas(false);
}
void NarrativePlayer::setCameraInTransition(double t)
{
	int m_current_slide = m_narratives->getCurrentSlideIndex();
	if (m_current_slide == 0) {
		updateCamera(m_narratives->getCurrentSlide()->getCameraMatrix());
		return;
	}

	NarrativeSlide *to = m_narratives->getCurrentSlide();
	int nar_index = m_narratives->getCurrentNarrativeIndex();
	int prev_index = m_narratives->getCurrentSlideIndex() - 1;
	NarrativeSlide *from = m_narratives->getSlide(nar_index, prev_index);

	osg::Matrixd new_matrix;
	if (t >= 1) new_matrix = to->getCameraMatrix();

	else new_matrix = Util::cameraMatrixInterp(from->getCameraMatrix(), to->getCameraMatrix(), Util::simpleCubic(0, 1, t));
	//std::cout << Util::camMatHerm(t, source_node->getCameraMatrix(), dest_node->getCameraMatrix()) << endl;

	emit updateCamera(new_matrix);
}
//
//void NarrativePlayer::enableNavigation(bool enable)
//{
//	//if (!enable) {
//	//	// remember the navigation mode
//	//	m_old_navigation_mode = m_window->m_osg_widget->getNavigationMode();
//	//	m_window->m_osg_widget->setNavigationMode(OSGViewerWidget::NAVIGATION_SIMPLE);
//	//}
//}


