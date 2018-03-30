#include <string>

class VSimRoot;
class NarrativeGroup;

namespace FileUtil
{
	// read vsim file
	void readVSimFile(const std::string &path, VSimRoot *root);

	// write vsim file
	void writeVSimFile(const std::string &path, VSimRoot *root);

	// read narratives from file
	
	// write narratives to file

	// read ers from file

	// write ers to file
}