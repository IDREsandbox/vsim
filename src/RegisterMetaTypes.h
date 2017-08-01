#ifndef REGISTERMETATYPES_H
#define REGISTERMETATYPES_H

#include <QMetaType>

#include <osg/Matrix>

// The purpose of this file is to allow openscenegraph types to 
//  be stored in QVariants for our Models

Q_DECLARE_METATYPE(osg::Matrixd);

extern void registerMetaTypes() {
	qRegisterMetaType<osg::Matrixd>();
}

#endif