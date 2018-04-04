#include "FileUtil.h"
#include "VSimSerializer.h"
#include <fstream>

bool FileUtil::readVSimFile(const std::string & path, VSimRoot * root)
{
	std::ifstream in(path, std::ios::binary);
	if (!in.good()) {
		return false;
	}
	return VSimSerializer::readStreamRobust(in, root);
}

bool FileUtil::writeVSimFile(const std::string & path, VSimRoot * root)
{
	std::ofstream out(path, std::ios::binary);
	if (!out.good()) {
		return false;
	}
	return VSimSerializer::writeStream(out, root);
}
