#ifndef BASEFIRSTPERSONMANIPULATOR_H
#define BASEFIRSTPERSONMANIPULATOR_H

#include "SimpleCameraManipulator.h"
#include "KeyTracker.h"

class BaseFirstPersonManipulator : public SimpleCameraManipulator {
public:
	BaseFirstPersonManipulator();

	virtual void stop();

	// performs physics checks and movement, make sure to call this from 
	//  from derived classes
	virtual void update(double dt_sec, KeyTracker *keys, osg::Node *world);

	void rotateByPixels(double dx, double dy);

	void moveForward(double dist);
	void moveRight(double dist);
	void moveUp(double dist);

	double getSensitivity() const;
	void setSensitivity(double);

	void enableGravity(bool enable);
	void enableCollisions(bool enable);
private:
	double m_sensitivity;

	// stored relative to current position, 
	// [right up -fwd]
	osg::Vec3d m_position_delta;

	double m_gravity_acceleration; // m/s^2
	bool m_gravity_on;
	double m_gravity_velocity;

	bool m_collision_on;
	double m_collision_radius;
};

#endif