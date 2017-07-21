#include "FirstPersonManipulator.h"
#include <QDebug>
#include <algorithm>
#include "Util.h"

FirstPersonManipulator::FirstPersonManipulator()
	: m_key_w(false),
	m_key_a(false),
	m_key_s(false),
	m_key_d(false)
{
	setMaxVelocity(10);
	m_x_current = 0;
	m_y_current = 0;
	m_x_target = 0;
	m_y_target = 0;
	//init();

	m_sensitivity = .005;
	m_smoothing = .2;

}

static int frame = 0;

bool FirstPersonManipulator::handle(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & us)
{
	osgGA::GUIEventAdapter::EventType type = ea.getEventType();

	if (type == osgGA::GUIEventAdapter::FRAME) {
		//m_prev_time_sec = viewer
		//qDebug() << "frame" << frame;

		//qDebug() << "FRAME " << frame
		//	<< "x" << ea.getX()
		//	<< "y" << ea.getY()
		//	<< "xmax" << ea.getXmax()
		//	<< "xmin" << ea.getXmin()
		//	<< "xnorm" << ea.getXnormalized();
		frame++;

		if (m_key_w) {
			moveForward(.2);
		}
		if (m_key_s) {
			moveForward(-.2);
		}
		if (m_key_d) {
			qDebug() << "strafe" << frame;
			moveRight(.2);
		}
		if (m_key_a) {
			moveRight(-.2);
		}
		if (m_key_shift) {
			moveUp(.2);
		}
		if (m_key_ctrl) {
			moveUp(-.2);
		}

		// go halfway to the target
		double difference_x = m_x_target - m_x_current;
		double difference_y = m_y_target - m_y_current;
		
		float dx = difference_x * m_smoothing;
		float dy = difference_y * m_smoothing;

		if (abs(difference_x) <= 1) {
			m_x_current = m_x_target;
		}
		else {
			m_x_current += dx;
		}

		if (abs(difference_y) <= 1) {
			m_y_current = m_y_target;
		}
		else {
			m_y_current += dy;
		}

		if (abs(difference_x) >= .01 || abs(difference_y) >= .001) {
			rotateByPixels(dx, dy);
		}

	}
	else if (type == osgGA::GUIEventAdapter::PUSH) {

		qDebug() << "push";

	}
	else if (type == osgGA::GUIEventAdapter::RELEASE) {

		qDebug() << "release";

	}
	else if (type == osgGA::GUIEventAdapter::TOUCH_MOVED) {
		qDebug() << "touch move";
	}
	else if (type == osgGA::GUIEventAdapter::MOVE) {

		float dx = ea.getX() - ea.getXmax() / 2;

		// sooo, qt uses ints and +y is down, osg uses float and +y is up
		// to get the right center... convert to ints and do the halving
		// dy (down is +)
		int yfromtop = (int)ea.getYmax() - (int)ea.getY();
		float dy = yfromtop - ((int)ea.getYmax()) / 2;

		m_x_target += dx;
		m_y_target += dy;

		return true;

	}
	else if (type == osgGA::GUIEventAdapter::KEYUP) {
		int key = ea.getKey();
		switch (key) {
		case osgGA::GUIEventAdapter::KEY_W:
			m_key_w = false;
			break;
		case osgGA::GUIEventAdapter::KEY_A:
			m_key_a = false;
			break;
		case osgGA::GUIEventAdapter::KEY_S:
			m_key_s = false;
			break;
		case osgGA::GUIEventAdapter::KEY_D:
			m_key_d = false;
			break;
		case osgGA::GUIEventAdapter::KEY_R:
			m_key_shift = false;
			break;
		case osgGA::GUIEventAdapter::KEY_F:
			m_key_ctrl = false;
			break;
		}

		qDebug() << "keyup" << frame;
		return true;

	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN) {
		int key = ea.getKey();
		int mask = ea.getModKeyMask();
		switch (key) {
		case osgGA::GUIEventAdapter::KEY_W:
			m_key_w = true;
			break;
		case osgGA::GUIEventAdapter::KEY_A:
			m_key_a = true;
			break;
		case osgGA::GUIEventAdapter::KEY_S:
			m_key_s = true;
			break;
		case osgGA::GUIEventAdapter::KEY_D:
			m_key_d = true;
			break;
		case osgGA::GUIEventAdapter::KEY_R:
			m_key_shift = true;
			break;
		case osgGA::GUIEventAdapter::KEY_F:
			m_key_ctrl = true;
			break;
		}

		qDebug() << "keydown" << key << frame << mask;
		return true;
	}
	//StandardManipulator::handle(ea, us);
	return false;
}

bool FirstPersonManipulator::handleMouseWheel(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & us)
{
	us.requestWarpPointer(100, 100);
	qDebug() << "handle mouse wheel";
	return false;
}

bool FirstPersonManipulator::performMovementLeftMouseButton(const double, double, double)
{
	qDebug() << "osg left mouse";
	return false;
}

bool FirstPersonManipulator::startAnimationByMousePointerIntersection(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & us)
{
	qDebug() << "animation start";
	return false;
}

void FirstPersonManipulator::rotateByPixels(int dx, int dy)
{
	osg::Matrixd m = getMatrix();
	double yaw, pitch, roll;
	Util::quatToYPR(m.getRotate(), &yaw, &pitch, &roll);
	//qDebug() << "ypr1" << y*180/M_PI << p*180/M_PI << r*180/M_PI;

	// dx+ yaw decreases
	// dx- yaw increases
	// dy+ pitch increases
	// dy- pitch decreases
	// conversion: pixels -> radians
	yaw -= dx * m_sensitivity;
	pitch += dy * m_sensitivity;

	pitch = Util::clamp(pitch, -M_PI_2 + .01, M_PI_2 - .01);

	// why doesnt this work...? how is this even supposed to work?
	//rotateYawPitch(osg::Quat(), y*180/M_PI, p*180/M_PI, osg::Vec3d(0, 0, 1));

	// just do it by hand
	osg::Quat rot = Util::YPRToQuat(yaw, pitch, 0);
	osg::Vec3d pos = m.getTrans();
	setByMatrix(osg::Matrix::rotate(rot) * osg::Matrix::translate(pos));

	//qDebug() << "mouse event" << frame;
}
