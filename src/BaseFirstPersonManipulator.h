#ifndef BASEFIRSTPERSONMANIPULATOR_H
#define BASEFIRSTPERSONMANIPULATOR_H

#include "SimpleCameraManipulator.h"
#include "KeyTracker.h"

class BaseFirstPersonManipulator : public SimpleCameraManipulator {
public:
	BaseFirstPersonManipulator();

	virtual void update(double dt_sec, KeyTracker *keys);

	void rotateByPixels(double dx, double dy);

	void moveForward(double dist);
	void moveRight(double dist);
	void moveUp(double dist);

	double getSensitivity() const;
	void setSensitivity(double);
private:
	double m_sensitivity;
};

#endif