#include "ERSerializer.h"

#include <unordered_map>

#include "Core/TypesSerializer.h"
#include "Core/Util.h"
#include "Core/LockTable.h"
#include "resources/EResource.h"
#include "resources/EResourceGroup.h"
#include "resources/ECategory.h"
#include "resources/ECategoryGroup.h"

namespace fb = VSim::FlatBuffers;
void ERSerializer::readERTable(const fb::ERTable *buffer, EResourceGroup *group,
	const TypesSerializer::Params &p)
{
	group->clear();
	ECategoryGroup * cats = group->categories();
	cats->clear();

	auto categories = buffer->categories();
	for (auto o_cat : *categories) {
		ECategory * cat = new ECategory();
		if (o_cat->name()) cat->setCategoryName(o_cat->name()->str());
		if (o_cat->color()) cat->setColor(TypesSerializer::fb2qtColor(*o_cat->color()));
		cats->append(std::shared_ptr<ECategory>(cat));
	}

	auto resources = buffer->resources();

	for (auto o_res : *resources) {
		EResource *res = new EResource();

		// convert the path

		res->setERType(static_cast<EResource::ERType>(o_res->type()));
		if (o_res->name()) res->setResourceName(o_res->name()->str());
		if (o_res->author()) res->setAuthor(o_res->author()->str());
		if (o_res->description()) res->setResourceDescription(o_res->description()->str());
		if (o_res->path()) {
			if (res->getERType() == EResource::FILE) {
				// only do relative conversion if file
				// websites like www.yahoo.com get messed up otherwise
				res->setResourcePath(TypesSerializer::readRelativePath(o_res->path(), p).toStdString());
			}
			else {
				res->setResourcePath(o_res->path()->str());
			}
		}

		res->setGlobal(o_res->global());
		res->setCopyright(static_cast<EResource::Copyright>(o_res->copyright()));
		res->setMinYear(o_res->year_min());
		res->setMaxYear(o_res->year_max());
		res->setReposition(o_res->reposition());
		res->setAutoLaunch(static_cast<EResource::AutoLaunch>(o_res->auto_launch()));
		res->setLocalRange(o_res->range());
		if (o_res->camera()) res->setCameraMatrix(TypesSerializer::fb2osgCameraMatrix(*o_res->camera()));
		res->setCategoryIndex(o_res->category_index());
		res->lockTable()->readLockTable(o_res->lock_table());
		group->append(std::shared_ptr<EResource>(res));
	}

	group->restoreCategories();
}

flatbuffers::Offset<fb::ERTable> ERSerializer::createERTable(
	flatbuffers::FlatBufferBuilder *builder,
	const EResourceGroup *group,
	const TypesSerializer::Params &p)
{
	const ECategoryGroup *cats = group->categories();

	group->saveCategoryIndices();

	// build categories
	std::vector<flatbuffers::Offset<fb::ECategory>> categories;

	for (size_t i = 0; i < cats->size(); i++) {
		ECategory *cat = dynamic_cast<ECategory*>(cats->child(i));

		// strings
		auto o_name = builder->CreateString(cat->getCategoryName());

		// color
		fb::Color s_color = TypesSerializer::qt2fbColor(cat->getColor());

		auto o_cat = fb::CreateECategory(*builder, o_name, &s_color);
		categories.push_back(o_cat);
	}

	auto o_categories = builder->CreateVector(categories);

	// build resources
	std::vector<flatbuffers::Offset<fb::EResource>> resources;

	for (size_t i = 0; i < group->size(); i++) {
		EResource *res = dynamic_cast<EResource*>(group->child(i));
		if (!res) continue;

		// strings
		auto o_name = builder->CreateString(res->getResourceName());
		auto o_auth = builder->CreateString(res->getAuthor());
		auto o_desc = builder->CreateString(res->getResourceDescription());
		flatbuffers::Offset<flatbuffers::String> o_path;
		if (res->getERType() == EResource::FILE) {
			o_path = TypesSerializer::createRelativePath(builder,
				QString::fromStdString(res->getResourcePath()), p);
		}
		else {
			o_path = builder->CreateString(res->getResourcePath());
		}
		auto o_lock = res->lockTableConst()->createLockTable(builder);

		fb::EResourceBuilder b_res(*builder);
		b_res.add_type(static_cast<fb::ERType>(res->getERType()));
		b_res.add_name(o_name);
		b_res.add_author(o_auth);
		b_res.add_description(o_desc);
		b_res.add_path(o_path);
		b_res.add_global(res->getGlobal());
		b_res.add_copyright(static_cast<fb::Copyright>(res->getCopyright()));
		b_res.add_year_min(res->getMinYear());
		b_res.add_year_max(res->getMaxYear());
		b_res.add_reposition(res->getReposition());
		b_res.add_auto_launch(static_cast<fb::AutoLaunch>(res->getAutoLaunch()));
		b_res.add_range(res->getLocalRange());
		fb::CameraPosDirUp s_camera = TypesSerializer::osg2fbCameraMatrix(res->getCameraMatrix());
		b_res.add_camera(&s_camera);
		b_res.add_category_index(res->getCategoryIndex());
		b_res.add_lock_table(o_lock);
		auto o_res = b_res.Finish();
		resources.push_back(o_res);
	}
	auto o_resources = builder->CreateVector(resources);

	// build table
	fb::ERTableBuilder table_builder(*builder);
	table_builder.add_categories(o_categories);
	table_builder.add_resources(o_resources);
	return table_builder.Finish();
}
