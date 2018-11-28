#include "NarrativePlayer.h"

#include <QDebug>
#include <osg/io_utils>

#include "narrative/NarrativeControl.h"
#include "narrative/NarrativeSlide.h"
#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "VSimApp.h"
#include "MainWindowTopBar.h"
#include "Core/Util.h"

NarrativePlayer::NarrativePlayer(VSimApp *app,
	NarrativeControl *narratives,
	MainWindowTopBar *top_bar,
	QObject *parent)
	: QObject(parent),
	m_app(app),
	m_narratives(narratives),
	m_paused(false)
{
	// actions

	// also acts as pause button
	a_play = new QAction(this);
	a_play->setText("Play");
	a_play->setShortcut(Qt::Key_P);
	connect(a_play, &QAction::triggered, this, &NarrativePlayer::play);

	a_pause = new QAction(this);
	a_pause->setText("Pause");
	a_pause->setShortcut(Qt::Key_P);
	connect(a_pause, &QAction::triggered, this, &NarrativePlayer::pause);

	a_stop = new QAction(this);
	a_stop->setText("Stop");
	a_stop->setShortcut(Qt::Key_Escape);
	connect(a_stop, &QAction::triggered, this, &NarrativePlayer::stop);

	a_next = new QAction(this);
	a_next->setText("Next");
	a_next->setShortcut(Qt::Key_Right);
	connect(a_next, &QAction::triggered, this, &NarrativePlayer::next);

	a_prev = new QAction(this);
	a_prev->setText("Prev");
	a_prev->setShortcut(Qt::Key_Left);
	connect(a_prev, &QAction::triggered, this, &NarrativePlayer::prev);

	m_center_msg_timer = new QTimer(this);
	m_center_msg_timer->setSingleShot(true);

	// ui connections
	top_bar->ui.play->setDefaultAction(a_play);
	top_bar->ui.play_2->setDefaultAction(a_play);
	top_bar->ui.stop->setDefaultAction(a_stop);
	top_bar->ui.stop_2->setDefaultAction(a_stop);
	top_bar->addAction(a_next);
	top_bar->addAction(a_prev);
	m_bar = top_bar;

	// state
	connect(m_app, &VSimApp::sStateChanged, this, [this](VSimApp::State old, VSimApp::State state) {
		// clean up timer
		if (state != VSimApp::PLAY_TRANSITION
			&& state != VSimApp::PLAY_WAIT_TIME) {
		}
		if (state == VSimApp::PLAY_WAIT_CLICK) {
			setCenterMessage(CenterMessage::CLICK);
		}
		else if (state == VSimApp::State::PLAY_FLYING) {
			setCenterMessage(CenterMessage::PRESSP);
		}
		else if (state == VSimApp::PLAY_END) {
			setCenterMessage(CenterMessage::FINISHED);
		}
		else {
			if (state == VSimApp::PLAY_TRANSITION
				|| state == VSimApp::PLAY_WAIT_TIME) {
				// clear the message if it isn't playing
				if (m_center_msg != CenterMessage::PLAYING) {
					setCenterMessage(CenterMessage::NONE);
				}
			}
			else {
				setCenterMessage(CenterMessage::NONE);
			}
		}

		// fly around interruption
		if (state == VSimApp::State::PLAY_FLYING
			&& VSimApp::isPlaying(old)
			&& old != VSimApp::State::PLAY_FLYING) {
			m_rewind_on_resume = (old == VSimApp::State::PLAY_TRANSITION);
		}

		//qDebug() << "playing old?"
		//	<< VSimApp::StateStrings[old]
		//	<< VSimApp::isPlaying(old)
		//	<< "playing new?"
		//	<< VSimApp::StateStrings[state]
		//	<< VSimApp::isPlaying(state);

		// stop playing interruption
		if (VSimApp::isPlaying(old) && !VSimApp::isPlaying(state)) {
			stop();
			setCenterMessage(CenterMessage::STOPPED);
		}

		a_stop->setEnabled(m_app->isPlaying());
		recheckPlayPause();
		updateStatusMessage();
	});
	connect(m_app, &VSimApp::sTick, this, &NarrativePlayer::update);
}

void NarrativePlayer::play()
{
	// unpause
	if ((m_app->state() == VSimApp::PLAY_WAIT_TIME
		|| m_app->state() == VSimApp::PLAY_TRANSITION)
		&& m_paused
		) {
		m_paused = false;
		recheckPlayPause();
		setCenterMessage(CenterMessage::NONE);
		return;
	}

	if (m_app->state() == VSimApp::PLAY_FLYING) {
		// if we were transitioning, then replay the transition
		if (m_rewind_on_resume) {
			toTransitioning(m_narratives->getTopSlide());
		}
		// go to the currently selected node
		else {
			toAtNode();
		}
		return;
	}

	if (m_app->state() == VSimApp::PLAY_WAIT_CLICK) {
		// advance
		next();
		return;
	}

	// not playing -> playing

	// try to start playing something

	size_t n = m_narratives->narratives()->size();
	if (n == 0) return;

	Narrative *nar = m_narratives->getCurrentNarrative();
	if (!nar) {
		qInfo() << "there is no current narative";
		m_narratives->selectNarratives({ 0 });
		// don't necessarily have to ->openNarrative()
	}

	NarrativeSlide *slide = m_narratives->getCurrentSlide();
	if (!slide) {
		int top = m_narratives->getTopSlide();
		if (top < 0) {
			m_narratives->openSlide(0);
		}
		else {
			m_narratives->openSlide(top);
		}
	}

	m_narratives->singleSelectOpenSlide();
	toAtNode();
	setCenterMessage(CenterMessage::PLAYING);
}

void NarrativePlayer::pause()
{
	if (m_app->state() == VSimApp::PLAY_WAIT_TIME
		|| m_app->state() == VSimApp::PLAY_TRANSITION) {
		m_paused = true;

		setCenterMessage(CenterMessage::PAUSED);
		updateStatusMessage();
	}

	recheckPlayPause();
}

void NarrativePlayer::stop()
{
	toStopped();
}

void NarrativePlayer::next()
{
	VSimApp::State state = m_app->state();

	if (state == VSimApp::PLAY_WAIT_CLICK
		|| state == VSimApp::PLAY_WAIT_TIME
		|| state == VSimApp::PLAY_FLYING) {

		int next_index = m_narratives->getTopSlide() + 1;
		NarrativeSlide *next = m_narratives->getSlide(m_narratives->getCurrentNarrativeIndex(),
			next_index);

		if (!next) {
			m_app->setState(VSimApp::PLAY_END);
			return;
		}

		toTransitioning(next_index);
	}
	else if (state == VSimApp::PLAY_TRANSITION) {
		toAtNode();
	}
}

void NarrativePlayer::prev()
{
	VSimApp::State state = m_app->state();
	if (!m_app->isPlaying()) return;

	// try to set the slie back one
	int prev_index = m_narratives->getCurrentSlideIndex() - 1;
	NarrativeSlide *next = m_narratives->getSlide(m_narratives->getCurrentNarrativeIndex(),
		prev_index);

	if (!next) return;

	m_narratives->openSlide(prev_index, true);
	toAtNode();
}

void NarrativePlayer::toTransitioning(int index)
{
	auto *prev = m_narratives->slideAtIndex(index - 1);
	auto *slide = m_narratives->slideAtIndex(index);

	if (!slide) {
		m_app->setState(VSimApp::PLAY_END);
		return;
	}
	if (!prev) {
		toAtNode();
		return;
	}

	m_app->setState(VSimApp::PLAY_TRANSITION);

	m_narratives->fadeOutSlide(prev); // fade out the previous slide
	//m_narratives->showCanvas(false, true); // fade out this slide
	m_narratives->openSlide(index, false); // open next slide
	m_narratives->showCanvas(false, false); // hide it

	m_camera_from = prev->getCameraMatrix();
	m_camera_to = slide->getCameraMatrix();

	m_slope_from = prev->doesSlowdown() ? 0.0 : 1.0;
	m_slope_to = slide->doesSlowdown() ? 0.0 : 1.0;

	m_total_time = slide->getTransitionDuration();
	m_time_remaining_sec = slide->getTransitionDuration();
	m_paused = false;

	recheckPlayPause();
}

void NarrativePlayer::toAtNode()
{
	NarrativeSlide *top_slide = m_narratives->slideAtIndex(m_narratives->getTopSlide());
	if (!top_slide) {
		m_app->setState(VSimApp::PLAY_END);
		return;
	}

	NarrativeSlide *slide = m_narratives->getCurrentSlide();
	if (!slide) {
		m_narratives->openSlide(m_narratives->getTopSlide(), true);
		slide = top_slide;
	}

	// fade in
	if (!m_narratives->canvasVisible()) m_narratives->showCanvas(true, true);

	// force camera
	m_app->stopGoingSomewhere();
	m_app->setCameraMatrix(slide->getCameraMatrix());

	if (slide->getStayOnNode()) {
		m_app->setState(VSimApp::PLAY_WAIT_CLICK);
	}
	else {
		m_app->setState(VSimApp::PLAY_WAIT_TIME);
		m_time_remaining_sec = slide->getDuration();
		m_paused = false;
	}
	recheckPlayPause();
}

void NarrativePlayer::toStopped()
{
	if (m_app->isPlaying()) m_app->setState(VSimApp::EDIT_FLYING);
	m_paused = false;
}

void NarrativePlayer::update(double dt_sec)
{
	if (m_app->state() == VSimApp::PLAY_TRANSITION
		&& !m_paused) {

		// advance the time
		m_time_remaining_sec -= dt_sec;

		// update the camera
		// t: [0, 1]
		// max to avoid dividing by 0
		double t_linear = 1 - m_time_remaining_sec / std::max(m_total_time, .001);

		// if an endpoint is 0.0s then generate a curve w/ slope 1
		double t_smooth = Util::cubicInterp(m_slope_from, m_slope_to, t_linear);

		m_app->setCameraMatrix(Util::cameraMatrixInterp(m_camera_from, m_camera_to, t_smooth));

		// check timer finished
		if (m_time_remaining_sec < 0) {
			next();
		}
	}

	if (m_app->state() == VSimApp::PLAY_WAIT_TIME
		&& !m_paused
		) {

		m_time_remaining_sec -= dt_sec;

		if (m_time_remaining_sec < 0) {
			next();
		}
	}
}

bool NarrativePlayer::isPaused() const
{
	// can only be paused during
	// transitioning, waiting
	auto state = m_app->state();
	return (state == VSimApp::PLAY_TRANSITION
		|| state == VSimApp::PLAY_WAIT_TIME)
		&& m_paused;
}

void NarrativePlayer::recheckPlayPause()
{
	// play and pause are mutually exclusive
	bool enable_play =
		!m_app->isPlaying()
		|| m_app->state() == VSimApp::PLAY_WAIT_CLICK
		|| m_app->state() == VSimApp::PLAY_FLYING
		|| (m_app->state() == VSimApp::PLAY_TRANSITION && m_paused)
		|| (m_app->state() == VSimApp::PLAY_WAIT_TIME && m_paused)
		;

	a_play->setEnabled(enable_play);
	a_pause->setEnabled(!enable_play);

	// they aren't completely mutually exclusive right?

	// play
	//  not playing
	//  flying
	//  transition & paused
	//  waiting & paused
	//  wait for click
	//  end?

	// pause
	//  transitioning & !paused
	//  waiting & !paused

	// neither?

	bool can_play =
		!m_app->isPlaying()
		|| m_app->state() == VSimApp::PLAY_WAIT_CLICK
		|| m_app->state() == VSimApp::PLAY_FLYING
		|| (m_app->state() == VSimApp::PLAY_TRANSITION && m_paused)
		|| (m_app->state() == VSimApp::PLAY_WAIT_TIME && m_paused)
		|| m_app->state() == VSimApp::PLAY_END;

	bool can_pause =
		(m_app->state() == VSimApp::PLAY_TRANSITION && !m_paused)
		|| (m_app->state() == VSimApp::PLAY_WAIT_TIME && !m_paused);

	a_play->setEnabled(can_play);
	a_pause->setEnabled(can_pause);

	//emit sCanPause(enable_play);

	// swap ui play/pause
	QAction *use_me = can_pause ? a_pause : a_play;
	QAction *dont_use = can_pause ? a_play : a_pause;
	for (QToolButton *button : { m_bar->ui.play, m_bar->ui.play_2 }) {
		button->removeAction(dont_use);
		button->setDefaultAction(use_me);
	}

}

QString NarrativePlayer::getNarrativeName() const
{
	Narrative *nar = m_narratives->getCurrentNarrative();
	if (!nar) return QString();
	QString title = QString::fromStdString(nar->getTitle());
	return title;
}

void NarrativePlayer::updateStatusMessage()
{
	auto state = m_app->state();

	QString status;
	QString base = "Playing narrative: " + getNarrativeName();
	if (state == VSimApp::PLAY_WAIT_CLICK) {
		status = base + " (wait click)";
	}
	else if (state == VSimApp::State::PLAY_FLYING) {
		status = base + " (flying)";
	}
	else if (state == VSimApp::PLAY_END) {
		status = "End of narrative: " + getNarrativeName();
	}
	else if (isPaused()) {
		status = base + " (paused)";
	}
	else if (m_app->isPlaying()) {
		status = base;
	}
	else {
	}

	m_status_message = status;
	if (!status.isEmpty()) {
		m_app->addPermanentStatusMessage(&m_status_message, m_status_message, false);
	}
	else {
		m_app->removePermanentStatusMessage(&m_status_message);
	}
}

void NarrativePlayer::setCenterMessage(CenterMessage msg)
{
	QString str;
	int duration = 0;
	m_center_msg_timer->stop();
	if (msg == CenterMessage::NONE) {
	}
	else if (msg == CenterMessage::CLICK) {
		str = "(Click or press P to continue)";
	}
	else if (msg == CenterMessage::PRESSP) {
		str = "(Press P to continue)";
	}
	else if (msg == CenterMessage::FINISHED) {
		str = "(Narrative finished)";
		duration = 2000;
	}
	else if (msg == CenterMessage::PLAYING) {
		str = "(Playing narrative: " + getNarrativeName() + ")";
		duration = 2000;
	}
	else if (msg == CenterMessage::PAUSED) {
		str = "(Paused, press P to continue)";
	}
	else if (msg == CenterMessage::STOPPED) {
		str = "(Narrative stopped)";
		duration = 2000;
	}
	m_center_msg = msg;
	m_app->setCenterMessage(str, duration);
}
