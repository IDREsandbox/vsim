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

private:

	bool m_key_w;
	bool m_key_a;
	bool m_key_s;
	bool m_key_d;
	bool m_key_shift;
	bool m_key_ctrl;

};

#endif /* FIRSTPERSONMANIPULATOR_H */
