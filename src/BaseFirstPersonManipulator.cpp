#include "BaseFirstPersonManipulator.h"
#include "Core/Util.h"


BaseFirstPersonManipulator::BaseFirstPersonManipulator()
	: m_sensitivity(.25),
	m_position_delta(),
	m_gravity_acceleration(-10.0),
	m_gravity_max_speed(40.0),
	m_gravity_on(false),
	m_collision_on(false),
	m_collision_radius(.5),
	m_collision_height(2.0)
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
		m_gravity_velocity += m_gravity_acceleration * dt_sec;
		// since negative it's like min(abs, abs)
		m_gravity_velocity = std::max(m_gravity_velocity, -m_gravity_max_speed);

		// gravity speed limit
		// not moving up/down relative to person, going relative to world
		//moveUp(-m_gravity_velocity * dt_sec);
		new_pos[2] += m_gravity_velocity * dt_sec;
	}

	// physics
	if (m_collision_on && new_pos != pos) {

		if (m_height_enabled) {
			// ground collision test
			osg::Vec3d up = osg::Vec3d(0, 0, m_collision_height);
			osg::Vec3d new_pos_down = new_pos - up;

			osg::ref_ptr<osgUtil::LineSegmentIntersector> gline =
				new osgUtil::LineSegmentIntersector(new_pos, new_pos_down);
			gline->setIntersectionLimit(osgUtil::Intersector::IntersectionLimit::LIMIT_NEAREST);

			osgUtil::IntersectionVisitor giv(gline.get());
			world->accept(giv);

			// if the height is exactly equal to the step height, then it gets confused
			if (gline->containsIntersections()) {
				auto intersections = gline->getIntersections();

				osg::Vec3d point = gline->getIntersections().begin()->getWorldIntersectPoint();
				// perform a step upward from the ground

				new_pos = point + up;

				m_gravity_velocity = 0;
			}
		}


		// forward collision test
		// extend the movement vector by 1m or so forward to make up for the near clip
		osg::Vec3d dir = new_pos - pos;
		dir.normalize();
		osg::Vec3d new_pos_plus = new_pos + dir * m_collision_radius;

		osg::ref_ptr<osgUtil::LineSegmentIntersector> vline = new osgUtil::LineSegmentIntersector(pos, new_pos_plus);
		vline->setIntersectionLimit(osgUtil::Intersector::IntersectionLimit::LIMIT_NEAREST);
		osgUtil::IntersectionVisitor iv(vline.get());
		world->accept(iv);

		if (vline->containsIntersections()) {
			osg::Vec3d point = vline->getIntersections().begin()->getWorldIntersectPoint();
			osg::Vec3d normal = vline->getIntersections().begin()->getWorldIntersectNormal();
			// if intersection then don't move
			new_pos = pos;
		}
	}

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
	return speedMultiplierForTick(m_speed_tick);
}

double BaseFirstPersonManipulator::speedMultiplierForTick(int tick)
{
	return std::pow(std::pow(2, .25), tick);
}

float BaseFirstPersonManipulator::eyeHeight() const
{
	return m_collision_height;
}

void BaseFirstPersonManipulator::setEyeHeight(float height)
{
	m_collision_height = height;
}

bool BaseFirstPersonManipulator::gravityEnabled() const
{
	return m_gravity_on;
}

void BaseFirstPersonManipulator::enableGravity(bool enable)
{
	m_gravity_velocity = 0;
	m_gravity_on = enable;
}

float BaseFirstPersonManipulator::gravityAcceleration() const
{
	return m_gravity_acceleration;
}

void BaseFirstPersonManipulator::setGravityAcceleration(float accel)
{
	m_gravity_acceleration = accel;
}

float BaseFirstPersonManipulator::gravitySpeed() const
{
	return m_gravity_max_speed;
}

void BaseFirstPersonManipulator::setGravitySpeed(float speed)
{
	m_gravity_max_speed = speed;
}

void BaseFirstPersonManipulator::enableHeight(bool enable)
{
	m_height_enabled = enable;
}

bool BaseFirstPersonManipulator::collisionsEnabled() const
{
	return m_collision_on;
}

void BaseFirstPersonManipulator::enableCollisions(bool enable)
{
	m_collision_on = enable;
}

void BaseFirstPersonManipulator::clearGravity()
{
	m_gravity_velocity = 0;
}

float BaseFirstPersonManipulator::collisionRadius() const
{
	return m_collision_radius;
}

void BaseFirstPersonManipulator::setCollisionRadius(float radius)
{
	m_collision_radius = radius;
}
