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
	//init();

}

static int frame = 0;

bool FirstPersonManipulator::handle(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & us)
{
	osgGA::GUIEventAdapter::EventType type = ea.getEventType();

	if (type == osgGA::GUIEventAdapter::FRAME) {
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
			//qDebug() << "strafe";
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

		//qDebug() << "mouse struggle";
		//qDebug()
		//	<< "x" << ea.getX()
		//	<< "y" << ea.getY()
		//	<< "yt" << yfromtop
		//	<< "xmax" << ea.getXmax()
		//	<< "ymax" << ea.getYmax()
		//	<< "xmin" << ea.getXmin()
		//	<< "xnorm" << ea.getXnormalized()
		//	<< "ynorm" << ea.getYnormalized()
		//	<< "scrolling" << ea.getScrollingDeltaX();
		//qDebug() << "osg" << dx << dy;

		// if dx or dy is too small make 0
		//dx = (abs(dx) <= .5) ? 0 : dx;
		//dy = (abs(dy) <= .5) ? 0 : dy;

		//qDebug() << "round" << dx << dy;

		osg::Matrixd m = getMatrix();
		double y, p, r;
		Util::quatToYPR(m.getRotate(), &y, &p, &r);
		//qDebug() << "ypr1" << y*180/M_PI << p*180/M_PI << r*180/M_PI;

		// dx+ yaw decreases
		// dx- yaw increases
		// dy+ pitch increases
		// dy- pitch decreases
		// conversion: pixels -> radians
		y -= dx * .005;
		p += dy * .005;
		//qDebug() << dx << dy;
		//qDebug() << "ypr2" << y * 180 / M_PI << p * 180 / M_PI << r * 180 / M_PI;

		p = Util::clamp(p, -M_PI_2+.01, M_PI_2-.01);
		// this doesn't work?
		//rotateYawPitch(osg::Quat(), y*180/M_PI, p*180/M_PI, osg::Vec3d(0, 0, 1));

		osg::Quat rot = Util::YPRToQuat(y, p, 0);
		osg::Vec3d pos = m.getTrans();

		//std::cout << "yaw " << yaw*180/M_PI << " pitch " << pitch * 180 / M_PI << " roll " << roll * 180 / M_PI << " xyz " << pos << "\n";

		setByMatrix(osg::Matrix::rotate(rot) * osg::Matrix::translate(pos));
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
