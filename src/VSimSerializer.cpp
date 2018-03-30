#include "VSimSerializer.h"
#include "NarrativeSerializer.h"
#include "ERSerializer.h"
#include "VSimRoot.h"

namespace fb = VSim::FlatBuffers;

void VSimSerializer::readModel(const VSim::FlatBuffers::Model *buffer,
	ModelGroup *models)
{

}

flatbuffers::Offset<VSim::FlatBuffers::Model>
	VSimSerializer::createModel(flatbuffers::FlatBufferBuilder *builder,
		const ModelGroup *models)
{
	//
	std::vector<int8_t> x;

	auto o_data = builder->CreateVector(x);
	fb::ModelBuilder b_model(*builder);
	b_model.add_data(o_data);
	b_model.add_ascii(false);
	b_model.add_format("osgb");
	auto o_model = b_model.Finish();

	flatbuffers::FlatBufferBuilder bbb;
	bbb.FinishSizePrefixed(o_model);
	bbb.GetBufferPointer();

	bbb.Finish(o_model);

	// read in bytes
	char *data;

	//flatbuffers::FlatBufferBuilder

	//flatbuffers::buffer

	return o_model;
}

void VSimSerializer::readRoot(const VSim::FlatBuffers::Root *buffer, VSimRoot *root)
{

	NarrativeGroup *nars = root->narratives();
	EResourceGroup *ers = root->resources();

	root->models();
}

flatbuffers::Offset<VSim::FlatBuffers::Root> VSimSerializer::createRoot(flatbuffers::FlatBufferBuilder * builder, const VSimRoot * root)
{
	auto o_version = builder->CreateString("2.0.0");
	auto o_nars = NarrativeSerializer::createNarrativeTable(builder, root->narratives());
	auto o_ers = ERSerializer::createERTable(builder, root->resources());
	auto o_model = createModel(builder, root->models());

	fb::RootBuilder b_root(*builder);
	b_root.add_version(o_version);
	b_root.add_narratives(o_nars);
	b_root.add_eresources(o_ers);
	b_root.add_model(o_model);
	auto o_root = b_root.Finish();

	return o_root;
}
