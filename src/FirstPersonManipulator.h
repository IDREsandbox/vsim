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

	// How this smoothing works:
	// There is a target (x,y) which is modified by the dx and dy of mouse movement
	// There is a lagging (x,y) which follows behind the target a little bit using rotateByPixels(dx,dy).
	// Actual motion uses the lagging motion and maps it to ypr
	// Approach is exponential (1/2)^n
	// +x is to the right
	// +y is down
	void rotateByPixels(int dx, int dy);

private:
	double m_prev_time_sec;

	bool m_key_w;
	bool m_key_a;
	bool m_key_s;
	bool m_key_d;
	bool m_key_shift;
	bool m_key_ctrl;

	double m_x_current;
	double m_y_current;
	double m_x_target;
	double m_y_target;

	double m_sensitivity;
	double m_smoothing;
};

#endif /* FIRSTPERSONMANIPULATOR_H */
