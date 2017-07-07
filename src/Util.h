#include <string>
#include <QRect>
#include <QImage>
#include <osg/Image>

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
}