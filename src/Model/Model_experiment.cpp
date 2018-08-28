#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QFileDialog>
#include <osgDB/ReadFile>
#include <QDebug>
#include <flatbuffers/flatbuffers.h>

#include "Model.h"
#include "ModelGroup.h"
#include "ModelOutliner.h"
#include "ModelUtil.h"
#include "Core/Util.h"
#include "ModelSerializer.h"

namespace fb = VSim::FlatBuffers;

void loadModels(QString path, ModelGroup *models) {
	std::ifstream in(path.toStdString(), std::ios::in | std::ios::binary);

	if (in.fail()) {
		qDebug() << "init fail";
		return;
	}

	// flatbuffer buffer
	std::string buffer;

	// read in flatbuffer size
	flatbuffers::uoffset_t fb_size;
	Util::appendn(in, buffer, sizeof(fb_size));
	if (in.fail()) {
		qDebug() << "appendn fail";
	}

	fb_size = flatbuffers::ReadScalar<flatbuffers::uoffset_t>(buffer.c_str());

	qDebug() << "fb size" << fb_size;

	// read the rest
	Util::appendn(in, buffer, fb_size);

	// construct & validate flatbuffer
	// TODO: flatbuffers should eventually have VerifySizePrefixedRoot
	auto *fb_root = flatbuffers::GetSizePrefixedRoot<fb::ModelTable>(buffer.c_str());
	const uint8_t *buf = reinterpret_cast<const uint8_t*>(buffer.c_str());

	auto verifier = flatbuffers::Verifier(buf, buffer.size());
	bool fb_ok = verifier.VerifySizePrefixedBuffer<fb::ModelTable>(fb::ModelTableIdentifier());
	if (!fb_ok) return;

	// now do business
	ModelSerializer::readModels(fb_root, models, in, Util::absoluteDirOf(path));

}

void saveModels(QString path, const ModelGroup *models, QString old_path) {
	std::ofstream out(path.toStdString(), std::ios::out | std::ios::binary);

	// place to dump model bytes
	std::stringstream model_buffer;

	// build flatbuffer
	flatbuffers::FlatBufferBuilder builder;
	TypesSerializer::Params p;
	p.old_base = old_path;
	p.new_base = Util::absoluteDirOf(path);
	auto o_root = ModelSerializer::createModels(&builder, models, model_buffer, p);
	builder.FinishSizePrefixed(o_root, fb::ModelTableIdentifier());
	// TODO: when flatbuffer updates - fb::FinishRootSizePrefixed();

	auto verifier = flatbuffers::Verifier(builder.GetBufferPointer(), builder.GetSize());
	bool ok = verifier.VerifySizePrefixedBuffer<fb::ModelTable>(fb::ModelTableIdentifier());

	//const fb::Root *fb_root = flatbuffers::GetRoot<fb::Root>(builder.GetBufferPointer());

	// write flatbuffer
	const char *buf = reinterpret_cast<const char*>(builder.GetBufferPointer());
	size_t size = builder.GetSize(); // TODO: check this? does it include the prefix?
	out.write(buf, size);

	// write models
	out << model_buffer.rdbuf();
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QMainWindow window;

	auto *mb = new QMenuBar(&window);
	window.setMenuBar(mb);
	auto *menu = mb->addMenu("stuff");

	ModelGroup *models = new ModelGroup(&window);

	auto *a_open = menu->addAction("open");
	auto *a_save = menu->addAction("save");
	auto *a_import = menu->addAction("import");
	auto *a_set_current = menu->addAction("set current");

	ModelOutliner *outliner = new ModelOutliner(&window);
	outliner->setModelGroup(models);
	outliner->show();

	QString current_dir = QDir::currentPath();

	auto setCurrentFile = [&](QString path) {
		// convert path to dir
		QString dir = Util::absoluteDirOf(path);
		current_dir = dir;
		outliner->setCurrentDir(dir);
	};


	QObject::connect(a_open, &QAction::triggered, [&]() {
		QString path = QFileDialog::getOpenFileName();
		models->clear();
		loadModels(path, models);
		setCurrentFile(path);
	});
	QObject::connect(a_save, &QAction::triggered, [&]() {
		QString path = QFileDialog::getSaveFileName();
		QString old_base = current_dir;
		saveModels(path, models, old_base);
		setCurrentFile(path);
		ModelUtil::fixRelativePaths(models, old_base, current_dir);
	});
	QObject::connect(a_import, &QAction::triggered, [&]() {
		ModelUtil::execImportModel(models, QDir::currentPath(), QString(), &window);
	});
	QObject::connect(a_set_current, &QAction::triggered, [&]() {
		QString path = QFileDialog::getOpenFileName();
		qDebug() << "path" << path << "dirname" << QDir(path).dirName();
		if (path.isEmpty()) return;
		setCurrentFile(path);
	});

	window.show();

	return a.exec();
}