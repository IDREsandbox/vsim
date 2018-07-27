#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <QRect>
#include <QImage>
#include <osg/Image>
#include <math.h>
#include <QtMath> // PI macros
#include <osg/Vec4>
#include <osg/Matrix>
#include <algorithm>
#include <QDate>
#include <QWidget>

namespace Util
{
	std::string addExtensionIfNotExist(const std::string& filename, const std::string& ext);
	std::string getExtension(const std::string& filename);
	std::string getFilename(const std::string &path);

	// converts m*/d*/yy to QDate, * means 1 or 2 chars
	// year conversion: <80 is 20xx, >=80 is 19xx
	bool mxdxyyToQDate(const std::string &str, QDate *out);

	// converts a name "T: pillar 123 456" into 123, 456
	// also works for pillar T: 123 456
	bool nodeTimeInName(const std::string &name, int * begin, int * end);

	// Fits the largest possible rectangle with given aspect ratio into another rectangle
	QRect rectFit(QRect container, float whratio);

	// checks if child is parent or descendant of parent
	bool isDescendant(QWidget *parent, QWidget *child);

	// QImage to osg::Image, allocates memory
	osg::Image *imageQtToOsg(const QImage& qimg);

	// osg::Image to QImage
	QImage imageOsgToQt(const osg::Image *oimg);

	QColor vecToColor(const osg::Vec4 &vec);
	osg::Vec4 colorToVec(const QColor &color);

	// convert QColor to "background:rgba(r, g, b, a);"
	QString colorToStylesheet(QColor);
	// convert QColor to "background:rgba(r, g, b, a); color:rgb(r, g, b);"
	// where the text color is chosen to contrast the background
	QString colorToContrastStyleSheet(QColor);

	QString setToString(std::set<int> set);

	template<class T>
	QString iterToString(T start, T end) {
		QString str = "";
		for (T it = start; it != end; ++it) {
			str += QString::number(*it);
			str += " ";
		}
		return str;
	}

	// timing functions
	// call tic to start the clock, toc to finish
	// toc returns msec
	void tic();
	double toc();

	template <typename T>
	T clamp(T value, T min, T max) {
		return std::min(std::max(value, min), max);
	}

	// forces an angle between [0, 2PI)
	double angleWrap(double x);

	// gives the closest version of angle y with respect to angle x
	// x: 10, y: 350 -> returns y': -10 (but in radians)
	// x: 370, y: 5 -> returns y': 365
	double closestAngle(double x, double y);

	// shortest difference between two angles
	// returns [-PI, PI]
	double angleDiff(double x, double y);

	double rad(double degrees);
	double deg(double radians);

	// returns a north facing camera matrix
	osg::Matrix baseCamera();

	// Converts the quaternion quat to yaw, pitch, and roll
	// based on a north facing camera:
	//  z up, x east, y north
	// yaw is rotation ccw about world z, so west is +90
	// pitch is rotation ccw about world x, so tilted down is -45 pitch
	// roll is ccw about world y, so facing +y and rolling right is positive
	// yaw and roll are [0,2PI)
	// pitch is [-PI/2, PI/2]
	void quatToYPR(const osg::Quat& quat, double *yaw, double *pitch, double *roll);
	void matToYPR(const osg::Matrix &mat, double *yaw, double *pitch, double *roll);

	// Converts yaw, pitch, and roll to a quaternion
	// assumes world z up, x right, y fwd
	osg::Quat yprToQuat(double yaw, double pitch, double roll);
	osg::Matrix yprToMat(double yaw, double pitch, double roll);

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
	//osg::Vec4d multVec(osg::Matrixd M, osg::Vec4d v);
	bool osgMatrixEq(const osg::Matrix m1, const osg::Matrix m2, double epsilon = .001);

	bool doubleEq(double x, double y, double epsilon = .00001);

	//4x4 transpose
	osg::Matrixd transpose(osg::Matrixd m);

	struct endPt {
		osg::Vec3d pos;
		osg::Vec3d tan;
	};
	
	osg::Matrixd camMatHerm(double t, osg::Matrixd m0, osg::Matrixd m1);

	// a dumb version of hermite curve
	double simpleCubic(double x0, double x1, double t);

	//Util::endPt hermiteCurve(osg::Vec4d a, osg::Vec4d b, osg::Vec4d da, osg::Vec4d db, double t, double epsl = .0001);

	bool spheresOverlap(const osg::Vec3f &p1, float r1, const osg::Vec3f &p2, float r2);

	// disconnects current connection old_watched, reconnects with new_watched
	// add destroyed safety
	// returns the new category
	template <class T>
	T *reconnect(QObject *listener, T **old_watched, T *new_watched) {
		if (*old_watched) QObject::disconnect(*old_watched, 0, listener, 0);

		*old_watched = new_watched;
		if (new_watched == nullptr) return new_watched;

		QObject::connect(new_watched, &QObject::destroyed, listener, [listener, old_watched]() {
			*old_watched = nullptr;
		});
		return new_watched;
	}
}

#endif