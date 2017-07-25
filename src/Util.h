#include <string>
#include <QRect>
#include <QImage>
#include <osg/Image>
#include <math.h>
#include <QtMath> // PI macros
#include <osg/Vec4>
#include <osg/Matrix>

namespace Util
{
	std::string addExtensionIfNotExist(const std::string& filename, const std::string& ext);
	std::string getExtension(const std::string& filename);
	
	// Fits the largest possible rectangle with given aspect ratio into another rectangle
	QRect rectFit(QRect container, float whratio);

	// QImage to osg::Image, allocates memory
	osg::Image *imageQtToOsg(const QImage& qimg);

	// osg::Image to QImage
	QImage imageOsgToQt(const osg::Image *oimg);

	double clamp(double value, double min, double max);

	// forces an angle between 0 and 180
	double angleWrap(double x);

	// gives the closer version of angle y with respect to angle x
	// plz give me wrapped angles
	// x: 10, y: 350 -> returns y: -10 (but all in radians)
	double closestAngle(double x, double y);

	// Converts the quaternion quat to yaw, pitch, and roll
	// assumes z up, x fwd, y left
	// yaw is rotation ccw about world z, so facing left (w/ respect to x fwd) is +90
	// pitch is rotation ccw about y, so tilted down is -45 pitch
	// yaw and roll are [0,2PI]
	// pitch is [-PI/2, PI/2]
	void quatToYPR(const osg::Quat& quat, double *yaw, double *pitch, double *roll);

	// Converts yaw, pitch, and roll to a quaternion
	// assumes z up, x right, y fwd
	osg::Quat YPRToQuat(double yaw, double pitch, double roll);

	// linear interpolation, t:[0,1]
	template <typename T>
	T lerp(double t, T x0, T x1) {
		return x0 * (1.0 - t) + x1 * t;
	}

	// linear interpolation between two rotation matrices
	osg::Matrixd viewMatrixLerp(double t, osg::Matrixd m0, osg::Matrixd m1);

	// Exponential interpolation? Returns the new value of start
	// - factor is the portion remaining after the step
	// - if (end-start) is smaller than clip, then we jump to the end
	// ex. exponentialSmooth(start=1, end=2, factor=.25, dt=1, clip=0) is 1.25
	// ex. exponentialSmooth(start=1, end=1.1, factor=.5, dt=1, clip=.2) is 1.1
	double exponentialSmooth(double start, double end, double factor, double dt = 1.0, double clip = 0.0);

	// Same as above, but updates current, and returns the change in current
	double exponentialSmooth2(double *current, double end, double factor, double dt = 1.0, double clip = 0.0);
}