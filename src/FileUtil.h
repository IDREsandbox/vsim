#include <string>
#include <set>
#include <iostream>

class VSimRoot;
class NarrativeGroup;
class EResourceGroup;

namespace FileUtil
{
	bool readVSimFile(const std::string &path, VSimRoot *root);
	bool writeVSimFile(const std::string &path, VSimRoot *root);

	bool importModel(const std::string &path, VSimRoot *root);

	bool importNarrativesStream(std::istream &in, NarrativeGroup *group);
	bool exportNarrativesStream(std::ostream &out, const NarrativeGroup *group,
		const std::set<int> &selection);
	//bool importNarratives(const std::string &path, NarrativeGroup *group);
	//bool exportNarratives(const std::string &path, const NarrativeGroup *group,
	//	const std::set<int> &selection);

	bool importEResources(std::istream &in, EResourceGroup *group);
	bool exportEResources(std::ostream &out, const EResourceGroup *group,
		const std::set<int> &selection);
}