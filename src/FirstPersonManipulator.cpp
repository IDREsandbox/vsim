#include "FirstPersonManipulator.h"
#include <QDebug>
#include <algorithm>
#include "Util.h"

FirstPersonManipulator::FirstPersonManipulator()
	: m_key_w(false),
	m_key_a(false),
	m_key_s(false),
	m_key_d(false),
	m_pos_current(),
	m_pos_target()
{
	setMaxVelocity(10);
	m_x_current = 0;
	m_y_current = 0;
	m_x_target = 0;
	m_y_target = 0;
	//init();

	m_sensitivity = .25;
	m_smoothing = .2;

	m_movement_smoothing = .1;
	m_speed = 10.0 / 60;

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
			m_pos_target[0] += m_speed;
			qDebug() << "key w" << m_pos_current[0] << m_pos_target[0];
		}
		if (m_key_s) {
			m_pos_target[0] += -m_speed;
			qDebug() << "key s";
		}
		if (m_key_d) {
			m_pos_target[1] += -m_speed;
			qDebug() << "key d";
		}
		if (m_key_a) {
			m_pos_target[1] += m_speed;
			qDebug() << "key a";
		}
		if (m_key_shift) {
			m_pos_target[2] += m_speed;
			qDebug() << "key r";
		}
		if (m_key_ctrl) {
			m_pos_target[2] += -m_speed;
			qDebug() << "key f";
		}

		// Position
		double px, py, pz, tx, ty, tz;
		px = m_pos_current.x();
		py = m_pos_current.y();
		pz = m_pos_current.z();
		tx = m_pos_target.x();
		ty = m_pos_target.y();
		tz = m_pos_target.z();
		double cx = Util::exponentialSmooth2(&px, tx, m_movement_smoothing, .01);
		double cy = Util::exponentialSmooth2(&py, ty, m_movement_smoothing, .01);
		double cz = Util::exponentialSmooth2(&pz, tz, m_movement_smoothing, .01);
		//qDebug() << "deltas" << cx << cy << cz;

		if (abs(cx) >= .001) moveForward(cx);
		if (abs(cy) >= .001) moveRight(-cy);
		if (abs(cz) >= .001) moveUp(cz);
		// update the vector
		m_pos_current = osg::Vec3d(px, py, pz);


		// Rotation
		double dx = Util::exponentialSmooth2(&m_x_current, m_x_target, m_smoothing, .01);
		double dy = Util::exponentialSmooth2(&m_y_current, m_y_target, m_smoothing, .01);

		if (abs(dx) != 0 || abs(dy) != 0) {
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

void FirstPersonManipulator::rotateByPixels(double dx, double dy)
{
	osg::Matrixd m = getMatrix();
	double yaw, pitch, roll;
	Util::quatToYPR(m.getRotate(), &yaw, &pitch, &roll);
	//qDebug() << "ypr1" << y*180/M_PI << p*180/M_PI << r*180/M_PI;

	// dx+ yaw decreases
	// dx- yaw increases
	// dy+ pitch increases
	// dy- pitch decreases
	// sensitivity is degrees/pixel, to convert to radians/pixel use M_PI/180
	yaw -= dx * m_sensitivity * M_PI / 180;
	pitch += dy * m_sensitivity * M_PI / 180;

	pitch = Util::clamp(pitch, -M_PI_2 + .01, M_PI_2 - .01);

	// why doesnt this work...? how is this even supposed to work?
	//rotateYawPitch(osg::Quat(), y*180/M_PI, p*180/M_PI, osg::Vec3d(0, 0, 1));
	// nvm let's just do it by hand
	osg::Quat rot = Util::YPRToQuat(yaw, pitch, 0);
	osg::Vec3d pos = m.getTrans();
	setByMatrix(osg::Matrix::rotate(rot) * osg::Matrix::translate(pos));
}
