#include <string>

class VSimRoot;
class NarrativeGroup;

namespace FileUtil
{
	// read vsim file
	bool readVSimFile(const std::string &path, VSimRoot *root);

	// write vsim file
	bool writeVSimFile(const std::string &path, VSimRoot *root);

	// read narratives from file
	
	// write narratives to file

	// read ers from file

	// write ers to file
}