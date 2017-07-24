#include "FlightManipulator.h"

FlightManipulator::FlightManipulator() {
	setSensitivity(45.0);
	m_acceleration = 4.0;
}

void FlightManipulator::stop()
{
	m_speed = 0;
	//mouse_x = 0;
	//mouse_y = 0;
}

void FlightManipulator::update(double dt_sec, KeyTracker *keys) {
	// update speed
	Qt::MouseButtons mouse_buttons = keys->mouseButtons();
	if (mouse_buttons & Qt::LeftButton) {
		m_speed += m_acceleration * dt_sec;
	}
	if (mouse_buttons & Qt::RightButton) {
		m_speed -= m_acceleration * dt_sec;
	}

	// update rotation
	rotateByPixels(mouse_x * dt_sec, mouse_y * dt_sec);

	// update position
	if (abs(m_speed) >= .001) {
		moveForward(m_speed * dt_sec);
	}
}

void FlightManipulator::setMousePosition(double posx, double posy)
{
	mouse_x = posx;
	mouse_y = posy;
}

