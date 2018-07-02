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

	// +4 ticks is 2x speed, -4 ticks is 1/2 speed
	void changeSpeed(int ticks);
	void setSpeedTick(int ticks);
	int getSpeedTick(int ticks) const;
	double getSpeedMultiplier() const;

	void enableGravity(bool enable);
	void enableCollisions(bool enable);
	void clearGravity(); // resets gravity velocity to 0

private:
	double m_sensitivity;

	int m_speed_tick;

	// delta position to be applied on next frame, from move up/right/forward
	// stored relative to current position, 
	// [right up -fwd]
	osg::Vec3d m_position_delta;

	double m_gravity_acceleration; // m/s^2, negative
	double m_gravity_max_speed;
	bool m_gravity_on;
	double m_gravity_velocity;

	bool m_collision_on;
	double m_collision_radius;

	double m_collision_height;
};

#endif