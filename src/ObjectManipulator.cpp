#include "ObjectManipulator.h"

ObjectManipulator::ObjectManipulator()
{
}
// _center, _distance, _rotation
void ObjectManipulator::setByMatrix(const osg::Matrixd &matrix)
{
	osg::Vec3d lookVector(-matrix(2, 0), -matrix(2, 1), -matrix(2, 2));
	osg::Vec3d eye(matrix(3, 0), matrix(3, 1), matrix(3, 2));

	// set default
	_center = eye + lookVector;
	_distance = lookVector.length();
	_rotation = matrix.getRotate();
	
	// then run the osg version
	osgGA::TerrainManipulator::setByMatrix(matrix);
}

void ObjectManipulator::stop()
{
	finishAnimation();
}