#include "ObjectManipulator.h"
#include <QString>
#include <QDebug>

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
	// TODO: set _distance to be on the z=0 plane maybe? the .length() is usually going to be 1
	_distance = lookVector.length(); 
	_rotation = matrix.getRotate();
	
	// then run the osg version
	osgGA::TerrainManipulator::setByMatrix(matrix);
}

void ObjectManipulator::stop()
{
	finishAnimation();
}
