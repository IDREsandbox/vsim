#include <string>

namespace Util
{
    std::string addExtensionIfNotExist(const std::string& filename, const std::string& ext);
    std::string getExtension(const std::string& filename);
}