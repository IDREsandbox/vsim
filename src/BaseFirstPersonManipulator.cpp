#include "BaseFirstPersonManipulator.h"
#include "Util.h"


BaseFirstPersonManipulator::BaseFirstPersonManipulator()
	: m_sensitivity(.25)
{
}

void BaseFirstPersonManipulator::update(double dt_sec, KeyTracker * keys)
{
}

void BaseFirstPersonManipulator::rotateByPixels(double dx, double dy)
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

void BaseFirstPersonManipulator::moveForward(double dist)
{
	osg::Matrixd mat = getMatrix();
	// remember osg does post multiplication (different from math class)
	//   so basis vectors are stored in rows
	// camera matrix is x right, y up, z toward face
	osg::Vec3d fwd = osg::Vec3d(-mat(2, 0), -mat(2, 1), -mat(2, 2));
	osg::Vec3d pos = mat.getTrans();
	mat.setTrans(pos + fwd * dist);
	setByMatrix(mat);
}

void BaseFirstPersonManipulator::moveRight(double dist)
{
	osg::Matrixd mat = getMatrix();
	osg::Vec3d fwd = osg::Vec3d(mat(0, 0), mat(0, 1), mat(0, 2));
	osg::Vec3d pos = mat.getTrans();
	mat.setTrans(pos + fwd * dist);
	setByMatrix(mat);
}

void BaseFirstPersonManipulator::moveUp(double dist)
{
	osg::Matrixd mat = getMatrix();
	osg::Vec3d fwd = osg::Vec3d(mat(1, 0), mat(1, 1), mat(1, 2));
	osg::Vec3d pos = mat.getTrans();
	mat.setTrans(pos + fwd * dist);
	setByMatrix(mat);
}

double BaseFirstPersonManipulator::getSensitivity() const
{
	return m_sensitivity;
}

void BaseFirstPersonManipulator::setSensitivity(double sensitivity)
{
	m_sensitivity = sensitivity;
}
