#include <algorithm>
#include <QDebug>
#include "util.h"

#include <osg/Matrix>
#include <osg/io_utils>
#include <iostream>

// ext must be of the form "txt".
std::string Util::addExtensionIfNotExist(const std::string& filename, const std::string& ext)
{
    size_t fs = filename.rfind("/");
    size_t bs = filename.rfind("\\");
    size_t s;
    if (fs == std::string::npos)
        s = bs;
    else
        s = fs;
    int n;
    if (s == std::string::npos)
        n = -1;
    else
        n = s;
    size_t d = filename.rfind('.');
    if (d == std::string::npos || (int)d < n)
        return filename + "." + ext;
    else
        return filename;
}

std::string Util::getExtension(const std::string& filename)
{
    size_t fs = filename.rfind("/");
    size_t bs = filename.rfind("\\");
    size_t s;
    if (fs == std::string::npos)
        s = bs;
    else
        s = fs;
    int n;
    if (s == std::string::npos)
        n = -1;
    else
        n = s;
    size_t d = filename.rfind('.');
    if (d == std::string::npos || (int)d < n)
        return "";
    else
    {
        std::string ext = filename.substr(d + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }
}

QRect Util::rectFit(QRect container, float whratio)
{
	float container_ratio = container.width() / (float) container.height();
	
	int width, height;
	int top, left;

	// container is wider, so match the vertical
	if (container_ratio >= whratio) {
		height = container.height();
		width = whratio*height;
	
		int leftover = container.width() - width;
		top = 0;
		left = leftover / 2;
	}
	// container is taller, so match the horizontal
	else {
		width = container.width();
		height = (1 / whratio)*width;

		int leftover = container.height() - height;
		left = 0;
		top = leftover / 2;
	}

	return QRect(left, top, width, height);
}

osg::Image *Util::imageQtToOsg(const QImage & qimg)
{
	QImage img_rgba8 = qimg.convertToFormat(QImage::Format_RGBA8888);
	osg::Image *oimg = new osg::Image;

	unsigned char *data = new unsigned char[img_rgba8.byteCount()];
	memcpy(data, img_rgba8.bits(), img_rgba8.byteCount());

	oimg->setImage(qimg.width(), qimg.height(), 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data, osg::Image::USE_NEW_DELETE, 1);
	return oimg;
}

QImage Util::imageOsgToQt(const osg::Image *oimg)
{
	// TODO: lots of possible bugs here
	// - wrong format
	// - non-contiguous rows
	// - WARNING: the QImage references the osg byte array instead of copying, this could be a problem
	return QImage(oimg->data(), oimg->s(), oimg->t(), QImage::Format_RGBA8888);

}

// credits to stackoverflow
double Util::angleWrap(double x)
{
	x = fmod(x, M_PI*2);
	if (x < 0)
		x += M_PI*2;
	return x;
}

double Util::closestAngle(double x, double y)
{
	// already the closest?
	if (abs(x - y) <= M_PI) {
		return y;
	}
	else {
		if (x > y) {
			// going down is too far, go up instead
			return y + 2 * M_PI;
		}
		else {
			// going up is too far, go down instead
			return y - 2 * M_PI;
		}
	}
}

void Util::quatToYPR(const osg::Quat &quat, double *yaw, double *pitch, double *roll)
{
	osg::Matrix m;
	quat.get(m);

	osg::Vec3 fwd, right;
	right[0] = m(0, 0);
	right[1] = m(0, 1);
	right[2] = m(0, 2);
	fwd[0] = -m(2, 0);
	fwd[1] = -m(2, 1);
	fwd[2] = -m(2, 2);
	
	double theta = atan2(fwd.y(), fwd.x());
	double r = 1; // it should be normalized already... I'm not certain though
	double phi = acos(fwd.z() / r);

	*yaw = angleWrap(theta);
	*pitch = angleWrap(phi - M_PI_2);

	// calculating roll
	// roll is the angle difference between flatright and right
	osg::Vec3 vertical(0, 0, 1);
	osg::Vec3 flatright = fwd ^ vertical;
	flatright.normalize();
	
	double roll2 = acos(right * flatright);
	// if roll goes below horizontal flip around 180
	if (right[2] < 0) {
		roll2 = 2*M_PI - roll2;
	}
	// its backwards... since x is world forward
	*roll = angleWrap(-roll2);
}

osg::Quat Util::YPRToQuat(double yaw, double pitch, double roll)
{
	// start with the camera facing forward in the world
	osg::Matrix base(
		0, -1, 0, 0,
		0, 0, 1, 0,
		-1, 0, 0, 0,
		0, 0, 0, 1);
	return (base * osg::Matrix::rotate(osg::Quat(roll, osg::Vec3(1, 0, 0), pitch, osg::Vec3(0, 1, 0), yaw, osg::Vec3(0, 0, 1)))).getRotate();
}

osg::Matrixd Util::viewMatrixLerp(double t, osg::Matrixd m0, osg::Matrixd m1)
{
	// linear interpolation of position
	osg::Vec3d pos0 = m0.getTrans();
	osg::Vec3d pos1 = m1.getTrans();
	osg::Vec3d pos = Util::lerp(t, pos0, pos1);

	// linear interpolation of yaw and pitch
	double yaw0, pitch0, roll0;
	double yaw1, pitch1, roll1;
	Util::quatToYPR(m0.getRotate(), &yaw0, &pitch0, &roll0);
	Util::quatToYPR(m1.getRotate(), &yaw1, &pitch1, &roll1);

	// take the shortest path... if the difference is greater that 180 degrees, then shift to the closer 180
	yaw1 = closestAngle(yaw0, yaw1);
	pitch1 = closestAngle(pitch0, pitch1);
	roll1 = closestAngle(roll0, roll1);

	//qDebug() << "rollin" << roll0 << roll1;

	double pitch = Util::lerp(t, pitch0, pitch1);
	double yaw = Util::lerp(t, yaw0, yaw1);
	double roll = Util::lerp(t, roll0, roll1);
	osg::Quat rot = Util::YPRToQuat(yaw, pitch, roll);

	//std::cout << "yaw " << yaw*180/M_PI << " pitch " << pitch * 180 / M_PI << " roll " << roll * 180 / M_PI << " xyz " << pos << "\n";

	return osg::Matrix::rotate(rot) * osg::Matrix::translate(pos);
}
