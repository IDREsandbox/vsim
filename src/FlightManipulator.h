#ifndef FLIGHTMANIPULATOR_H
#define FLIGHTMANIPULATOR_H

#include "BaseFirstPersonManipulator.h"

class FlightManipulator : public BaseFirstPersonManipulator {
public:
	FlightManipulator();

	// stop all motion
	void stop();

	virtual void update(double dt_sec, KeyTracker *keys) override;

	// Normalized mouse position (pixelx/(height/2) and pixely/(height/2)). The reason
	//  is that tiny screens will rotate less than larger screens, which is no
	//  good.
	// Suggested units of sensitivity: degrees per percent y.
	//  ex. 90 -> 60 degrees per second if the mouse is 2/3 up
	// +x causes rotation right, -x causes rotation left
	// +y causes rotation down, -y causes rotation up
	void setMousePosition(double posx, double posy);

private:
	// current speed moving fwd, can be negative
	double m_speed;
	double m_acceleration;

	double mouse_x;
	double mouse_y;
};
#endif
