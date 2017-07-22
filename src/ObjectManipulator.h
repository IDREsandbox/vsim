#ifndef OBJECTMANIPULATOR_H
#define OBJECTMANIPULATOR_H

#include <osgGA/TerrainManipulator>

class ObjectManipulator : public osgGA::TerrainManipulator {
public:
	ObjectManipulator::ObjectManipulator();

	// We have to override the default setByMatrix because its behavior isn't 
	// great when there is no ground. Basically we have to fill in those 
	// protected values in osg. _center, _distance, _rotation
	void setByMatrix(const osg::Matrixd& matrix);
	void stop();
};


#endif