#include "FlightManipulator.h"
#include "Util.h"

FlightManipulator::FlightManipulator() {
	setSensitivity(45.0);
	m_acceleration = 4.0;
	m_strafe_sensitivity = .01;
	m_strafe_smoothing = .01;
}

void FlightManipulator::stop()
{
	BaseFirstPersonManipulator::stop();
	m_speed = 0;
	//m_mouse_x = 0;
	//m_mouse_y = 0;
}

void FlightManipulator::update(double dt_sec, KeyTracker *keys, osg::Node *world) {
	// update speed
	Qt::MouseButtons mouse_buttons = keys->mouseButtons();
	if (mouse_buttons & Qt::LeftButton) {
		m_speed += m_acceleration * dt_sec;
	}
	if (mouse_buttons & Qt::RightButton) {
		m_speed -= m_acceleration * dt_sec;
	}

	// always do strafing
	double dx = Util::exponentialSmooth2(&m_scurrent_x, m_starget_x, m_strafe_smoothing, dt_sec, .001);
	double dy = Util::exponentialSmooth2(&m_scurrent_y, m_starget_y, m_strafe_smoothing, dt_sec, .001);
	moveUp(-dy * m_strafe_sensitivity);
	moveRight(dx * m_strafe_sensitivity);

	// only rotate if not strafing
	if (mouse_buttons & Qt::MiddleButton || keys->keyPressed(Qt::Key_Alt)) {
	}
	else {
		rotateByPixels(m_mouse_x * dt_sec, m_mouse_y * dt_sec);
	}

	// update position
	if (abs(m_speed) >= .001) {
		moveForward(m_speed * dt_sec);
	}
	BaseFirstPersonManipulator::update(dt_sec, keys, world);
}

void FlightManipulator::setMousePosition(double posx, double posy)
{
	m_mouse_x = posx;
	m_mouse_y = posy;
}

void FlightManipulator::strafe(int dx, int dy)
{
	m_starget_x += dx;
	m_starget_y += dy;
}

