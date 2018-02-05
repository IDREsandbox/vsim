#include <string>
#include <QRect>
#include <QImage>
#include <osg/Image>
#include <math.h>
#include <QtMath> // PI macros
#include <osg/Vec4>
#include <osg/Matrix>
#include <algorithm>

namespace Util
{
	std::string addExtensionIfNotExist(const std::string& filename, const std::string& ext);
	std::string getExtension(const std::string& filename);
	std::string getFilename(const std::string &path);
	
	// Fits the largest possible rectangle with given aspect ratio into another rectangle
	QRect rectFit(QRect container, float whratio);

	// QImage to osg::Image, allocates memory
	osg::Image *imageQtToOsg(const QImage& qimg);

	// osg::Image to QImage
	QImage imageOsgToQt(const osg::Image *oimg);

	QColor vecToColor(const osg::Vec4 &vec);
	osg::Vec4 colorToVec(const QColor &color);

	// convert QColor to "background:rgba(r, g, b, a);"
	QString colorToStylesheet(QColor);

	QString setToString(std::set<int> set);

	template <typename T>
	T clamp(T value, T min, T max) {
		return std::min(std::max(value, min), max);
	}

	// forces an angle between 0 and 180
	double angleWrap(double x);

	// gives the closer version of angle y with respect to angle x
	// plz give me wrapped angles
	// x: 10, y: 350 -> returns y: -10 (but all in radians)
	double closestAngle(double x, double y);

	// Converts the quaternion quat to yaw, pitch, and roll
	// assumes z up, x right, y fwd
	// yaw is rotation ccw about world z, so facing left (w/ respect to x fwd) is +90
	// pitch is rotation ccw about world x, so tilted down is -45 pitch
	// roll is ccw about world y, so facing +y and rolling right is positive
	// yaw and roll are [0,2PI]
	// pitch is [-PI/2, PI/2]
	void quatToYPR(const osg::Quat& quat, double *yaw, double *pitch, double *roll);

	// Converts yaw, pitch, and roll to a quaternion
	// assumes world z up, x right, y fwd
	osg::Quat YPRToQuat(double yaw, double pitch, double roll);

	// linear interpolation, t:[0,1]
	template <typename T>
	T lerp(T x0, T x1, double t) {
		return x0 * (1.0 - t) + x1 * t;
	}

	// linear interpolation between two camera matrices
	osg::Matrixd cameraMatrixInterp(osg::Matrixd m0, osg::Matrixd m1, double t);

	// Exponential interpolation? Returns the new value of start
	// - factor is the portion remaining after the stepf
	// - if (end-start) is smaller than clip, then we jump to the end
	// ex. exponentialSmooth(start=1, end=2, factor=.25, dt=1, clip=0) is 1.25
	// ex. exponentialSmooth(start=1, end=1.1, factor=.5, dt=1, clip=.2) is 1.1
	double exponentialSmooth(double start, double end, double factor, double dt = 1.0, double clip = 0.0);

	// Same as above, but updates current, and returns the change in current
	double exponentialSmooth2(double *current, double end, double factor, double dt = 1.0, double clip = 0.0);

	// 
	QString osgMatrixToQString(osg::Matrix);
	osg::Vec4d mult_vec(osg::Matrixd M, osg::Vec4d v);

	//4x4 transpose
	osg::Matrixd transpose(osg::Matrixd m);

	struct endPt {
		osg::Vec3d pos;
		osg::Vec3d tan;
	};
	
	osg::Matrixd camMatHerm(double t, osg::Matrixd m0, osg::Matrixd m1);

	// a dumb version of hermite curve
	double simpleCubic(double x0, double x1, double t);

	Util::endPt hermiteCurve(osg::Vec4d a, osg::Vec4d b, osg::Vec4d da, osg::Vec4d db, double t, double epsl = .0001);
}