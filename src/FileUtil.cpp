#include "FileUtil.h"
#include "VSimSerializer.h"
#include "NarrativeSerializer.h"
#include "ERSerializer.h"
#include <fstream>
#include <sstream>
#include <QDebug>

#include "narrative/NarrativeGroup.h"
#include "narrative/Narrative.h"
#include "deprecated/narrative/NarrativeOld.h"

namespace fb = VSim::FlatBuffers;
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

bool FileUtil::importModel(const std::string & path, VSimRoot * root)
{
	return false;
}

bool FileUtil::importNarrativesStream(std::istream &in, NarrativeGroup *group)
{
	// read into buffer
	std::stringstream ss;
	ss << in.rdbuf();
	std::string s = ss.str();
	const uint8_t *buf = reinterpret_cast<const uint8_t*>(s.c_str());

	bool fb_match = fb::VerifyNarrativeTableBuffer(flatbuffers::Verifier(buf, s.length()));
	if (fb_match) {
		// reading flatbuffers
		const fb::NarrativeTable *fb_root = fb::GetNarrativeTable(s.c_str());
		NarrativeSerializer::readNarrativeTable(fb_root, group);
		return true;
	}

	// trying to read old stuff
	osg::ref_ptr<osg::Node> node = VSimSerializer::readOSGB(ss, false, false);
	if (!node) {
		qWarning() << "failed to load narrative file, not flatbuffer or osg";
		return false;
	}
	// try group, try narrative
	NarrativeOld *old = dynamic_cast<NarrativeOld*>(node.get());
	if (old) {
		group->addChild(new Narrative(old));
	}
}

bool FileUtil::exportNarrativesStream(std::ostream &out, const NarrativeGroup *group,
	const std::set<int> &selection)
{
	if (!out.good()) {
		return false;
	}

	// make a new group w/ copy of narratives
	NarrativeGroup g;
	for (auto i : selection) {
		Narrative *nar = dynamic_cast<Narrative*>(group->child(i));
		if (nar) {
			g.addChild(nar);
		}
	}

	flatbuffers::FlatBufferBuilder builder;
	auto o_table = NarrativeSerializer::createNarrativeTable(&builder, &g);
	fb::FinishNarrativeTableBuffer(builder, o_table);

	const char *buf = reinterpret_cast<const char*>(builder.GetBufferPointer());
	out.write(buf, builder.GetSize());

	return out.good();
}

//bool FileUtil::importNarratives(const std::string & path, NarrativeGroup *group)
//{
//	//// Open dialog
//	//qInfo("Importing narratives");
//	//QString filename = QFileDialog::getOpenFileName(m_window, "Import Narratives",
//	//	getCurrentDirectory(), "Narrative files (*.nar);;All types (*.*)");
//	//if (filename == "") {
//	//	qInfo() << "import cancel";
//	//	return false;
//	//}
//
//	//osg::ref_ptr<osg::Node> loadedModel;
//
//	//// open file
//	std::ifstream ifs;
//	ifs.open(path, std::ios::binary);
//	if (!ifs.good()) {
//		return false;
//	}
//
//	return importNarrativesStream(ifs, group);
//}
//
//bool FileUtil::exportNarratives(const std::string & path, const NarrativeGroup * group,
//	const std::set<int>& selection)
//{
//	std::ofstream ofs;
//	ofs.open(path, std::ios::binary);
//	if (!ofs.good()) {
//		return false;
//	}
//
//	return exportNarrativesStream(ofs, group, selection);
//}

bool FileUtil::importEResources(const std::string & path, EResourceGroup * group)
{
	return false;
}

bool FileUtil::exportEResources(const std::string & path, const EResourceGroup * group,
	const std::set<int>& selection)
{
	return false;
}
