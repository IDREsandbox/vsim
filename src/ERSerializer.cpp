#include "ERSerializer.h"

#include "types_generated.h"
#include "eresources_generated.h"
#include "resources/EResource.h"
#include "resources/EResourceGroup.h"
#include "resources/ECategoryGroup.h"
#include <QDebug>
#include <unordered_map>

namespace fb = VSim::FlatBuffers;
void readERTable(const fb::ERTable *buffer, EResourceGroup *group)
{
	qDebug() << "DEBUGGIN! read ER buffer";
	group->clear();
	ECategoryGroup * cats = group->categories();
	cats->clear();

	auto categories = buffer->categories();
	for (auto o_cat : *categories) {
		ECategory * cat = new ECategory();
		cat->setName(o_cat->name()->str());
		
		const fb::Color *c = o_cat->color();
		cat->setColor(QColor(c->r(), c->g(), c->b(), c->a()));
	}

	auto resources = buffer->resources();

	for (auto o_res : *resources) {
		EResource *res = new EResource();

		res->setResourceName(o_res->name()->str());
		res->setERType(static_cast<EResource::ERType>(res->getERType()));
		res->setResourceDescription(o_res->description()->str());
		res->setResourcePath(o_res->path()->str());
		res->setGlobal(o_res->global());

		res->setCopyright(static_cast<EResource::Copyright>(res->getCopyright()));
		res->setMinYear(o_res->year_min());
		res->setMaxYear(o_res->year_max());
		res->setReposition(o_res->reposition());
		res->setAutoLaunch(o_res->autolaunch());
		res->setLocalRange(o_res->range());

		// TODO
		const fb::CameraPosLook *s_camera = o_res->camera();
		osg::Matrix camera_matrix;
		res->setCameraMatrix(camera_matrix);

		// set category pointer
		int cat_index = o_res->category_index();
		if (cat_index >= 0) {
			ECategory *cat = cats->category(cat_index);
			res->setCategory(cat);
		}
	}
}

flatbuffers::Offset<fb::ERTable> createERTable(const EResourceGroup *group, flatbuffers::FlatBufferBuilder *builder)
{
	qDebug() << "WRITE ER BUFFer! write ER buffer";

	const ECategoryGroup *cats = group->getCategories();

	std::unordered_map<ECategory*, size_t> cat_to_index;
	for (size_t i = 0; i < cats->getNumChildren(); i++) {
		ECategory *cat = cats->category(i);
		if (!cat) continue;
		cat_to_index[cat] = i;
	}

	// build categories
	std::vector<flatbuffers::Offset<fb::ECategory>> categories;
	
	for (size_t i = 0; i < cats->getNumChildren(); i++) {
		ECategory *cat = dynamic_cast<ECategory*>(cats->child(i));

		// strings
		auto o_name = builder->CreateString(cat->getCategoryName());

		// color
		QColor qcolor = cat->getColor();
		fb::Color s_color = fb::Color(qcolor.red(), qcolor.green(), qcolor.blue(), qcolor.alpha());

		auto o_cat = fb::CreateECategory(*builder, o_name, &s_color);
		categories.push_back(o_cat);
	}

	auto o_categories = builder->CreateVector(categories);

	// build resources
	std::vector<flatbuffers::Offset<fb::EResource>> resources;

	for (size_t i = 0; i < group->getNumChildren(); i++) {
		EResource *res = dynamic_cast<EResource*>(group->child(i));
		if (!res) continue;

		// strings
		auto o_name = builder->CreateString(res->getResourceName());
		auto o_auth = builder->CreateString(res->getAuthor());
		auto o_desc = builder->CreateString(res->getResourceDescription());
		auto o_path = builder->CreateString(res->getResourcePath());

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
		b_res.add_autolaunch(res->getAutoLaunch());
		b_res.add_range(res->getLocalRange());

		// TODO
		fb::CameraPosLook s_camera;
		b_res.add_camera(&s_camera);

		// lookup category in index table
		ECategory *cat = res->category();
		auto it = cat_to_index.find(cat);
		if (it != cat_to_index.end()) {
			b_res.add_category_index(-1);
		}
		else {
			b_res.add_category_index(it->second);
		}

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
