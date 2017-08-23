#include <QDebug>
#include <Util.h>
#include "NarrativePlayer.h"
#include "labelCanvasView.h"

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
	connect(m_narratives, &NarrativeControl::selectionChanged, this, &NarrativePlayer::editEvent);
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
	next(FORWARD);
}

void NarrativePlayer::leftArrow()
{
	next(BACKWARD);
}

//void NarrativePlayer::leftClick()
//{
//	next(FORWARD);
//}

void NarrativePlayer::timerExpire()
{
	next(FORWARD);
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
	if (slide) {
		qDebug() << "update camera";
		updateCamera(slide->getCameraMatrix());
	}

	toStopped();
}

void NarrativePlayer::play()
{
	if (m_state == ATNODE && m_narratives->getCurrentSlide()->getStayOnNode()) {
		qDebug() << "play - continue";
		next(FORWARD);
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

void NarrativePlayer::next(Advance dir)
{
	qInfo() << "Narrative Player - next";
	int si = m_narratives->getCurrentSlideIndex(); // current slide index
	int si2; // next slide index

	if (m_state == ATNODE) {
		if (dir == BACKWARD) si2 = si; // don't change slide
		else si2 = si + 1; // do change slide

	}
	else if (m_state == TRANSITIONING) {
		if (dir == BACKWARD) si2 = si - 1; // don't change slide
		else si2 = si; // do change slide

	}

	// change the slide if necessary
	if (si != si2) {
		qDebug() << "have to change the slide";
		bool ok = setSlide(si2);
		if (!ok) {
			// this happens if we're at the beginning or end
			qDebug() << "failed to change slide, stopping";
			toStopped();
			return;
		}
	}

	if (m_state == ATNODE) {
		toTransitioning();
	}
	else if (m_state == TRANSITIONING) {
		toAtNode();
	}
}

void NarrativePlayer::toTransitioning()
{
	m_slide_time_sec = 0;
	m_state = TRANSITIONING;
}

void NarrativePlayer::toAtNode()
{
	emit updateCamera(m_narratives->getCurrentSlide()->getCameraMatrix());
	m_slide_time_sec = 0;
	m_state = ATNODE;
}

void NarrativePlayer::toStopped()
{
	if (m_state == STOPPED) return;
	qDebug() << "to stopped";
	m_state = STOPPED;
	enableNavigation(true);
}

bool NarrativePlayer::setSlide(int index)
{
	// set the slide while edit event
	int ni = m_narratives->getCurrentNarrativeIndex();
	NarrativeSlide *slide = m_narratives->getNarrativeSlide(ni, index);
	if (slide == nullptr) return false;

	m_expect_selection_change = true;
	m_narratives->selectSlides(ni, {index});
	m_expect_selection_change = false;

	return true;
	//return m_narratives->openSlide(m_ni, index);
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
	NarrativeSlide *from = m_narratives->getNarrativeSlide(nar_index, prev_index);

	osg::Matrixd new_matrix;
	if (t >= 1) new_matrix = to->getCameraMatrix();
	else new_matrix = Util::cameraMatrixInterp(from->getCameraMatrix(), to->getCameraMatrix(), t);
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


