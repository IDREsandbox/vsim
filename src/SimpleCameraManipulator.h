#ifndef SIMPLECAMERAMANIPULATOR_H
#define SIMPLECAMERAMANIPULATOR_H

#include <osgGA/StandardManipulator>

class SimpleCameraManipulator : public osgGA::CameraManipulator
{
public:
	SimpleCameraManipulator() {};
	virtual const char* className() const { return "SimpleCameraManipulator"; }
	virtual void setByMatrix(const osg::Matrixd& matrix);
	virtual void setByInverseMatrix(const osg::Matrixd& matrix);
	virtual osg::Matrixd getMatrix() const;
	virtual osg::Matrixd getInverseMatrix() const;

protected:
	osg::Matrixd m_inverse_matrix;
};

#endif /* SIMPLECAMERAMANIPULATOR_H */
