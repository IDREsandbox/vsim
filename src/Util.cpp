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
