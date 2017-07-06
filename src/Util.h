#include <string>
#include <QRect>
namespace Util
{
    std::string addExtensionIfNotExist(const std::string& filename, const std::string& ext);
    std::string getExtension(const std::string& filename);
	
	// Fits the largest possible rectangle with given aspect ratio into another rectangle
	QRect rectFit(QRect container, float whratio);
}