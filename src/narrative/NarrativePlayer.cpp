#include <QDebug>
#include <Util.h>
#include "NarrativePlayer.h"

#include "narrative/NarrativeControl.h"
#include "narrative/NarrativeSlide.h"
#include "VSimApp.h"
#include "MainWindowTopBar.h"

#include <osg/io_utils>

NarrativePlayer::NarrativePlayer(VSimApp *app,
	NarrativeControl *narratives,
	MainWindowTopBar *top_bar,
	QObject *parent)
	: QObject(parent),
	m_app(app),
	m_narratives(narratives)
{
	// actions
	a_play = new QAction(this);
	a_play->setText("Play");
	a_play->setShortcut(Qt::Key_P);
	connect(a_play, &QAction::triggered, this, &NarrativePlayer::play);

	a_stop = new QAction(this);
	a_stop->setText("Stop");
	a_stop->setShortcut(Qt::Key_Escape);
	connect(a_stop, &QAction::triggered, this, &NarrativePlayer::stop);

	a_next = new QAction(this);
	a_next->setText("Next");
	a_next->setShortcut(Qt::Key_Right);
	connect(a_next, &QAction::triggered, this, &NarrativePlayer::next);

	// ui connections

	connect(m_app, &VSimApp::sStateChanged, this, [this]() {
		// clean up timer
		VSimApp::State state = m_app->state();
		if (state != VSimApp::PLAY_TRANSITION
			&& state != VSimApp::PLAY_WAIT_TIME) {
			m_timer.stop();
		}

		// stop events
		if (!m_app->isPlaying()) {
			stop();
		}
	});
	connect(&m_timer, &QTimer::timeout, this, &NarrativePlayer::next);
}

void NarrativePlayer::play()
{
	if (m_app->isPlaying()) {
		next();
		return;
	}
	else {
		//try to select something
		NarrativeSlide *slide = m_narratives->getCurrentSlide();
		if (!slide) {
			m_narratives->openSlide(0);
		}
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

	VSimApp::State state = m_app->state();

	if (state == VSimApp::PLAY_WAIT_CLICK
		|| state == VSimApp::PLAY_WAIT_TIME) {
		toTransitioning();
	}
	else if (state == VSimApp::PLAY_TRANSITION) {
		toAtNode();
	}
}

void NarrativePlayer::toTransitioning()
{
	// get the next slide matrix & transition
	int next_index = m_narratives->getCurrentSlideIndex() + 1;
	NarrativeSlide *next = m_narratives->getSlide(m_narratives->getCurrentNarrativeIndex(),
		next_index + 1);

	// end if fail
	if (!next) {
		m_app->setState(VSimApp::PLAY_END);
		return;
	}
	m_app->setState(VSimApp::PLAY_TRANSITION);

	m_narratives->showCanvas(false, true); // fade out this slide
	m_narratives->openSlide(next_index, false); // open next slide
	m_narratives->showCanvas(false, false); // hide it
	m_app->setCameraMatrixSmooth(next->getCameraMatrix(), next->getTransitionDuration());

	m_timer.setInterval(next->getTransitionDuration() * 1000);
	m_timer.start();
}

void NarrativePlayer::toAtNode()
{
	// fade in
	m_narratives->showCanvas(true, true);

	NarrativeSlide *slide = m_narratives->getCurrentSlide();
	if (!slide) {
		m_app->setState(VSimApp::PLAY_END);
		return;
	}
	if (slide->getStayOnNode()) {
		m_app->setState(VSimApp::PLAY_WAIT_CLICK);
	}
	else {
		m_app->setState(VSimApp::PLAY_WAIT_TIME);
		m_timer.setInterval(slide->getDuration() * 1000);
		m_timer.start();
	}
}

void NarrativePlayer::toStopped()
{
	if (m_app->isPlaying()) m_app->setState(VSimApp::EDIT_FLYING);
}