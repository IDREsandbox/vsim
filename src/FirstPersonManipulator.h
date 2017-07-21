#ifndef FIRSTPERSONMANIPULATOR_H
#define FIRSTPERSONMANIPULATOR_H

#include <osgGA/StandardManipulator>
#include <osgGA/FirstPersonManipulator>

class FirstPersonManipulator : public osgGA::FirstPersonManipulator
{
   public:
	FirstPersonManipulator();

	//META_Node(osgGA, FirstPersonManipulator)

// virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

	// override all events
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &us);

protected:
	// override osgGA::FirstPersonManipulator controls...
	// we want our own controls
	virtual bool handleMouseWheel(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
	virtual bool performMovementLeftMouseButton(const double, double, double);
	virtual bool startAnimationByMousePointerIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

	osg::Matrixd m_inverse_matrix;

	// Qt's setCursor() creates an event and takes an entire frame, so nonzero
	// nonzero mouse events are at half the FPS as they should be. Sooo making
	// a smooth and responsive mouse is a bit complicated.
	// How smoothing works:
	// There is a target (x,y) which is modified by the dx and dy of mouse 
	// movement. There is a lagging (x,y) which follows behind the target 
	// exponentially based on smoothness (.5 = step halfway every frame).
	// The actual rotation is done by rotateByPixels which converts dx and dy
	// to yaw and pitch based on sensitivity (units of degrees/pixel).
	// +x is to the right
	// +y is down (Qt style)
	void rotateByPixels(double dx, double dy);

private:
	// position control
	bool m_key_w;
	bool m_key_a;
	bool m_key_s;
	bool m_key_d;
	bool m_key_shift;
	bool m_key_ctrl;

	// x fwd (w/s), y left (a/d), z up (shift/ctrl)
	osg::Vec3d m_pos_target;
	osg::Vec3d m_pos_current;
	double m_movement_smoothing;
	double m_speed; // meters per second (TODO)

	// rotation control
	// x fwd, y left, z up
	double m_x_current;
	double m_y_current;
	double m_x_target;
	double m_y_target;

	double m_sensitivity;
	double m_smoothing;
};

#endif /* FIRSTPERSONMANIPULATOR_H */
