#ifndef FIRSTPERSONMANIPULATOR_H
#define FIRSTPERSONMANIPULATOR_H

#include <osgGA/StandardManipulator>
#include <osgGA/FirstPersonManipulator>
#include "KeyTracker.h"

class FirstPersonManipulator : public osgGA::FirstPersonManipulator
{
   public:
	FirstPersonManipulator();

	void stop();
	void update(double dt, KeyTracker *keys);
	void mouseMove(int dx, int dy);
	void accelerate(int ticks);

	// override all osg events
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us);

	double getMaxSpeed();

private:
	// x fwd (w/s), y left (a/d), z up (shift/ctrl)
	osg::Vec3d m_pos_target;
	osg::Vec3d m_pos_current;
	double m_movement_smoothing;
	double m_base_speed; // meters per second (TODO)
	int m_speed_click; // speed limiter

	// rotation control
	// x fwd, y left, z up
	double m_x_current;
	double m_y_current;
	double m_x_target;
	double m_y_target;

	double m_sensitivity;
	double m_smoothing;

	// Qt's setCursor() creates an event and takes an entire frame, so nonzero
	// nonzero mouse events are at half the FPS as they should be. Sooo making
	// a smooth and responsive mouse is a bit complicated.
	// How smoothing works:
	// There is a target (x,y) which is modified by the dx and dy of mouse 
	// movement. There is a lagging (x,y) which follows behind the target 
	// exponentially based on smoothness (.75 = step .25 way every second).
	// The actual rotation is done by rotateByPixels which converts dx and dy
	// to yaw and pitch based on sensitivity (units of degrees/pixel).
	// +x is to the right
	// +y is down (Qt style)
	void rotateByPixels(double dx, double dy);
};

#endif /* FIRSTPERSONMANIPULATOR_H */
