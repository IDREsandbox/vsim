#ifndef SIMPLECAMERAMANIPULATOR_H
#define SIMPLECAMERAMANIPULATOR_H

#include <osgGA/StandardManipulator>

class SimpleCameraManipulator : public osgGA::CameraManipulator
{
public:
	SimpleCameraManipulator() {};
	virtual const char* className() const { return "SimpleCameraManipulator"; }
	virtual void setByMatrix(const osg::Matrixd& matrix) { m_inverse_matrix = osg::Matrixd::inverse(matrix); }
	virtual void setByInverseMatrix(const osg::Matrixd& matrix) { m_inverse_matrix = matrix; }
	virtual osg::Matrixd getMatrix() const { return osg::Matrixd::inverse(m_inverse_matrix); }
	virtual osg::Matrixd getInverseMatrix() const { return m_inverse_matrix; }

	// virtual void init(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
	// virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

	protected:

	osg::Matrixd m_inverse_matrix;
};

#endif /* SIMPLECAMERAMANIPULATOR_H */
