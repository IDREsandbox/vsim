#include "BaseFirstPersonManipulator.h"
#include "Core/Util.h"


BaseFirstPersonManipulator::BaseFirstPersonManipulator()
	: m_sensitivity(.25),
	m_position_delta(),
	m_gravity_acceleration(-4.0),
	m_gravity_on(false),
	m_collision_on(false),
	m_collision_radius(1.0)
{
	stop();
}

void BaseFirstPersonManipulator::stop()
{
	m_gravity_velocity = 0;
}

void BaseFirstPersonManipulator::update(double dt_sec, KeyTracker *keys, osg::Node *world)
{

	osg::Matrixd mat = getMatrix();
	osg::Vec3d right = osg::Vec3d(mat(0, 0), mat(0, 1), mat(0, 2));
	osg::Vec3d up = osg::Vec3d(mat(1, 0), mat(1, 1), mat(1, 2));
	osg::Vec3d back = osg::Vec3d(mat(2, 0), mat(2, 1), mat(2, 2));
	osg::Vec3d pos = mat.getTrans();
	osg::Vec3d new_pos = pos + right*m_position_delta[0] + up*m_position_delta[1] + back*m_position_delta[2];

	if (m_gravity_on) {
		// update velocity
		m_gravity_velocity -= m_gravity_acceleration;
		m_gravity_velocity = std::max(m_gravity_velocity, -20.0);

		// gravity speed limit
		// not moving up/down relative to person, going relative to world
		//moveUp(-m_gravity_velocity * dt_sec);
		new_pos[2] -= m_gravity_velocity * dt_sec;
	}

	// physics
	if (m_collision_on && new_pos != pos) {

		// extend the movement vector by 1m or so forward to make up for the near clip
		osg::Vec3d dir = new_pos - pos;
		dir.normalize();
		osg::Vec3d new_pos_plus = new_pos + dir * m_collision_radius;

		osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(pos, new_pos_plus);

		osgUtil::IntersectionVisitor iv(lsi.get());
		iv.setTraversalMask(0xffffffff);
		world->accept(iv);

		if (lsi->containsIntersections()) {
			osg::Vec3d point = lsi->getIntersections().begin()->getWorldIntersectPoint();
			osg::Vec3d normal = lsi->getIntersections().begin()->getWorldIntersectNormal();
			// if intersection then don't move
			new_pos = pos;
		}
	}
	
	// delta z becomes the new gravity velocity
	// limit it to less than 0
	osg::Vec3d delta = new_pos - pos;
	m_gravity_velocity = std::min(delta[2] / dt_sec, 0.0);

	mat.setTrans(new_pos);
	setByMatrix(mat);

	m_position_delta = osg::Vec3d();
}

void BaseFirstPersonManipulator::rotateByPixels(double dx, double dy)
{
	osg::Matrixd m = getMatrix();
	double yaw, pitch, roll;
	Util::quatToYPR(m.getRotate(), &yaw, &pitch, &roll);

	// dx+ yaw decreases
	// dx- yaw increases
	// dy+ pitch increases
	// dy- pitch decreases
	// sensitivity is degrees/pixel, to convert to radians/pixel use M_PI/180
	yaw -= dx * m_sensitivity * M_PI / 180;
	pitch -= dy * m_sensitivity * M_PI / 180;

	pitch = Util::clamp(pitch, -M_PI_2 + .01, M_PI_2 - .01);

	setByMatrix(Util::yprToMat(yaw, pitch, 0) * osg::Matrix::translate(m.getTrans()));
}

void BaseFirstPersonManipulator::moveForward(double dist)
{
	m_position_delta[2] += -dist;
}

void BaseFirstPersonManipulator::moveRight(double dist)
{
	m_position_delta[0] += dist;
}

void BaseFirstPersonManipulator::moveUp(double dist)
{
	m_position_delta[1] += dist;
}

double BaseFirstPersonManipulator::getSensitivity() const
{
	return m_sensitivity;
}

void BaseFirstPersonManipulator::setSensitivity(double sensitivity)
{
	m_sensitivity = sensitivity;
}

void BaseFirstPersonManipulator::setSpeedTick(int ticks)
{
	m_speed_tick = ticks;
}

int BaseFirstPersonManipulator::getSpeedTick(int ticks) const
{
	return m_speed_tick;
}

void BaseFirstPersonManipulator::changeSpeed(int ticks)
{
	m_speed_tick += ticks;
}

double BaseFirstPersonManipulator::getSpeedMultiplier() const
{
	return std::pow(std::pow(2, .25), m_speed_tick);
}

void BaseFirstPersonManipulator::enableGravity(bool enable)
{
	m_gravity_velocity = 0;
	m_gravity_on = enable;
}

void BaseFirstPersonManipulator::enableCollisions(bool enable)
{
	m_collision_on = enable;
}
