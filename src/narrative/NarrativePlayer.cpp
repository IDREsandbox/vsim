#include <QDebug>
#include <Util.h>
#include "NarrativePlayer.h"

NarrativePlayer::NarrativePlayer(QObject *parent, MainWindow *window, NarrativeControl *narratives)
	: QObject(parent),
	m_window(window),
	m_narratives(narratives),
	m_current_narrative(-1),
	m_current_slide(-1),
	m_playing(false),
	m_transitioning(false),
	m_slide_time_sec(0)
{
	m_narrative_box = m_window->ui.topBar->ui.narratives;
	m_slide_box = m_window->ui.topBar->ui.slides;

	// play pause
	connect(m_window->ui.topBar->ui.play_2, &QPushButton::clicked, this, &NarrativePlayer::play);
	connect(m_window->ui.topBar->ui.pause_2, &QPushButton::clicked, this, &NarrativePlayer::pause);

	// open
	connect(m_narrative_box, &NarrativeScrollBox::sOpen, this,
	[this]() {
		qDebug() << "Narrative player - open";
	});

	connect(m_window->ui.topBar->ui.open, &QPushButton::clicked, this, 
		[this]() {
		qDebug() << "Narrative player - open";
	});

	connect(m_window->ui.topBar->ui.left_2, &QPushButton::clicked, this, 
		[this]() {
		qDebug() << "Narrative player - close";
	});

	// narrative selection change
	connect(m_narrative_box, &NarrativeScrollBox::sSelectionChange, this, &NarrativePlayer::selectionChange);
	connect(m_slide_box, &SlideScrollBox::sSelectionChange, this, &NarrativePlayer::selectionChange);

	m_previous_time = m_window->getViewer()->elapsedTime();
	m_timer = new QTimer(this);
	m_timer->setInterval(0);
	m_timer->setSingleShot(false);
	m_timer->start();

	connect(m_timer, &QTimer::timeout, this,
		[this]() {
			m_window->m_osg_widget->update();
			
			// timestep
			double current_time = m_window->getViewer()->elapsedTime();
			update(current_time - m_previous_time);
			m_previous_time = current_time;
		});
}
void NarrativePlayer::update(double dt_sec)
{
	if (!isPlaying()) {
		return;
	}
	if (m_current_narrative == -1 || m_current_slide == -1) {
		qDebug() << "update error narrative";
		pause();
		return;
	}

	if (!m_transitioning) { // not transitioning, waiting on slide
		const NarrativeSlide *current_node = m_narratives->getNarrativeNode(m_current_narrative, m_current_slide);
		if (current_node == nullptr) {
			qWarning() << "Error: narrative player current slide is null";
			pause();
			return;
		}

		m_slide_time_sec += dt_sec;
		double wait_time = current_node->getDuration();
		if (m_slide_time_sec >= wait_time) {
			next();
		}
	}
	else { // transitioning
		if (m_current_slide <= 0) {
			qWarning() << "Error: player is transitioning in slide 0" << m_current_slide;
			pause();
			return;
		}
		int num_nodes = m_narratives->getNarrative(m_current_narrative)->getNumChildren();

		if (m_current_slide >= num_nodes) {
			qWarning() << "Error: narrative player's current slide # is greater than the number of slides";
			pause();
			return;
		}

		// get source slide
		const NarrativeSlide *source_node = m_narratives->getNarrativeNode(m_current_narrative, m_current_slide - 1);
		const NarrativeSlide *dest_node = m_narratives->getNarrativeNode(m_current_narrative, m_current_slide);
		double transition_duration = dest_node->getTransitionDuration();

		// advance time
		m_slide_time_sec += dt_sec;

		// time progress from [0,1], so we can do simple interpolation
		double t = m_slide_time_sec / transition_duration;

		// have we finished?
		if (t >= 1.0) {
			next();
			return;
		}
		//qDebug() << "transitioning:" << t;
		osg::Matrixd new_matrix = Util::viewMatrixLerp(t, source_node->getCameraMatrix(), dest_node->getCameraMatrix());
		setCameraMatrix(new_matrix);
	}
}

void NarrativePlayer::play()
{
	qDebug() << "play";
	if (m_current_narrative == -1 || m_current_slide == -1) {
		return;
	}
	const NarrativeSlide *next_node = m_narratives->getNarrativeNode(m_current_narrative, m_current_slide + 1);
	if (next_node == nullptr) { // check if at the end
		qInfo() << "Can't play - at the end";
		//pause();
		return;
	}

	m_playing = true;
	m_transitioning = false;
	m_slide_time_sec = 0;
	
	m_old_navigation_mode = m_window->m_osg_widget->getNavigationMode();
	m_window->m_osg_widget->setNavigationMode(OSGViewerWidget::NAVIGATION_SIMPLE);
}

void NarrativePlayer::next()
{
	qInfo() << "Narrative Player - next";
	// if not playing, we still want to do the transition on next()
	if (!m_transitioning) {
		const NarrativeSlide *next_node = m_narratives->getNarrativeNode(m_current_narrative, m_current_slide + 1);
		printf("next node %p\n", next_node);
		if (next_node == nullptr) { // check if at the end
			pause();
			return;
		}
		// if the next slide has a transition, then transitioning, otherwise skip
		if (next_node->getTransitionDuration() <= .00001) {
			m_transitioning = false;
		}
		else {
			m_transitioning = true;
		}

		m_current_slide++;
		m_slide_time_sec = 0;
		m_slide_box->forceSelect(m_current_slide);
		return;
	}

	// if transitioning, jump to the finish
	else {
		m_transitioning = false;
		m_slide_time_sec = 0;

		const NarrativeSlide *current_node = m_narratives->getNarrativeNode(m_current_narrative, m_current_slide);
		setCameraMatrix(current_node->getCameraMatrix());

		// if there is no next node, we're done
		const NarrativeSlide *next_node = m_narratives->getNarrativeNode(m_current_narrative, m_current_slide + 1);
		if (next_node == nullptr) {
			qInfo() << "Narrative Player - last slide reached";
			pause();
			return;
		}
	}

	// after the switch and slide update, check if there is a StayOnNode
	if (m_transitioning == false && isPlaying()) {
		const NarrativeSlide *current_node = m_narratives->getNarrativeNode(m_current_narrative, m_current_slide);
		if (current_node->getStayOnNode()) {
			pause();
		}
	}
}

void NarrativePlayer::pause()
{
	if (!m_playing) return;

	qInfo() << "Narrative Player - pause";
	if (m_transitioning) {
		m_current_slide = m_current_slide - 1;
		return;
	}
	
	NarrativeSlide *new_slide = m_narratives->getNarrativeNode(m_current_narrative, m_current_slide);
	if (new_slide == nullptr) {
		qWarning() << "Pause error, current slide is null";
		return;
	}

	m_slide_box->forceSelect(m_current_slide);
	m_playing = false;
	m_transitioning = false;
	setCameraMatrix(new_slide->getCameraMatrix());

	m_window->m_osg_widget->setNavigationMode(m_old_navigation_mode);
}
bool NarrativePlayer::isPlaying()
{
	return m_playing;
}

void NarrativePlayer::setCameraMatrix(osg::Matrixd camera_matrix)
{
	m_window->m_osg_widget->setCameraMatrix(camera_matrix);
}

//bool NarrativePlayer::setSlide(int narrative, int slide)
//{
//	NarrativeSlide *new_slide = m_narratives->getNarrativeNode(narrative, slide);
//	if (new_slide == nullptr) {
//		return false;
//	}
//	m_current_narrative = narrative;
//	m_current_slide = slide;
//	m_slide_box->forceSelect(m_current_slide);
//	m_playing = false;
//	m_transitioning = false;
//	setCameraMatrix(new_slide->getCameraMatrix());
//	
//	return true;
//}

void NarrativePlayer::selectionChange()
{
	std::set<int> nar_sel = m_narrative_box->getSelection();
	int slide;
	if (nar_sel.empty()) {
		m_current_narrative = -1;
	}
	else {
		m_current_narrative = *nar_sel.rbegin();
	}

	std::set<int> slide_sel = m_slide_box->getSelection();
	if (slide_sel.empty()) {
		m_current_slide = -1;
	}
	else {
		m_current_slide = *slide_sel.rbegin();

		qDebug() << "Narrative Player - slide selection" << m_current_narrative << m_current_slide;
		const NarrativeSlide *current_node = m_narratives->getNarrativeNode(m_current_narrative, m_current_slide);
		if (current_node == nullptr) {
			qWarning() << "Error: can't set narrative player view to selection, indices: ", m_current_narrative, m_current_slide;
		}
		else {
			setCameraMatrix(current_node->getCameraMatrix());
		}
	}
	pause();
}
