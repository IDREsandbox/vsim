#include <algorithm>
#include <QDebug>
#include "util.h"

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

void Util::quatToYPR(const osg::Quat &quat, double *yaw, double *pitch, double *roll)
{
	osg::Vec3 dir;
	quat.getRotate(*roll, dir);
	double theta = atan2(dir.y(), dir.x());
	double r = 1; // it should be normalized already... I'm not certain though
	double phi = acos(dir.z() / r);

	*yaw = theta;
	*pitch = phi - M_PI_2;
}

osg::Quat Util::YPRToQuat(double yaw, double pitch, double roll)
{
	// x = rcos(theta)sin(phi)
	// y = rsin(theta)sin(phi)
	// z = rcos(phi)
	// pitch = pi/2 - phi
	double x = cos(yaw)*cos(pitch);
	double y = sin(yaw)*cos(pitch);
	double z = sin(pitch);
	osg::Vec3 dir(x, y, z);
	return osg::Quat(roll, dir);
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
	double pitch = Util::lerp(t, pitch0, pitch1);
	double yaw = Util::lerp(t, yaw0, yaw1);
	osg::Quat rot = Util::YPRToQuat(yaw, pitch, 0);

	return osg::Matrix::rotate(rot) * osg::Matrix::translate(pos);
}
