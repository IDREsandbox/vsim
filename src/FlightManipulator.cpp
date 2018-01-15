#include "FlightManipulator.h"
#include "Util.h"

FlightManipulator::FlightManipulator()
	: m_mouse_x(0),
	m_mouse_y(0),
	m_speed(0),
	m_scurrent_x(0),
	m_scurrent_y(0),
	m_starget_x(0),
	m_starget_y(0)
{
	// parameters
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

	// strafing
	if (mouse_buttons & Qt::MiddleButton || keys->keyPressed(Qt::Key_Alt)) {
		double multiplier = m_strafe_sensitivity * dt_sec * getSpeedMultiplier();
		moveUp(m_mouse_x * multiplier);
		moveRight(-m_mouse_y * multiplier);

	}
	// moving and rotating
	else {
		if (mouse_buttons & Qt::LeftButton) {
			m_speed += m_acceleration * dt_sec * getSpeedMultiplier();
		}
		if (mouse_buttons & Qt::RightButton) {
			m_speed -= m_acceleration * dt_sec * getSpeedMultiplier();
		}

		rotateByPixels(m_mouse_x * dt_sec, m_mouse_y * dt_sec);
		if (std::abs(m_speed) >= .001) {
			moveForward(m_speed * dt_sec);
		}
	}

	BaseFirstPersonManipulator::update(dt_sec, keys, world);
}

void FlightManipulator::setMousePosition(double posx, double posy)
{
	m_mouse_x = posx;
	m_mouse_y = posy;
}
//
//void FlightManipulator::strafe(int dx, int dy)
//{
//	m_starget_x += dx;
//	m_starget_y += dy;
//}

