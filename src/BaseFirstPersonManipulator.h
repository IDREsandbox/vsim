#ifndef BASEFIRSTPERSONMANIPULATOR_H
#define BASEFIRSTPERSONMANIPULATOR_H

#include "SimpleCameraManipulator.h"
#include "KeyTracker.h"

class BaseFirstPersonManipulator : public SimpleCameraManipulator {
public:
	BaseFirstPersonManipulator();

	// performs physics checks and movement, make sure to call this from 
	//  from derived classes
	virtual void update(double dt_sec, KeyTracker *keys);

	void rotateByPixels(double dx, double dy);

	void moveForward(double dist);
	void moveRight(double dist);
	void moveUp(double dist);

	double getSensitivity() const;
	void setSensitivity(double);
private:
	double m_sensitivity;

	// stored relative to current position, 
	// [right up -fwd]
	osg::Vec3d m_position_delta;
};

#endif