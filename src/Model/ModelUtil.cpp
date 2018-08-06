#include "ModelUtil.h"

#include <QFileDialog>
#include <QDebug>
#include <QtConcurrent>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <QMessageBox>

#include "Gui/FutureDialog.h"
#include "Model/Model.h"
#include "Model/ModelGroup.h"
#include "Model/ModelEditDialog.h"
#include "Core/Util.h"

QString ModelUtil::execModelFileDialog(QWidget *parent, QString last_dir)
{
	QString filename = QFileDialog::getOpenFileName(parent, "Choose Model",
		last_dir,
		"Model files (*.flt;*.ive;*.osg;*.osgb;*.osgt;*.obj;*.3ds;*.dae);;"
		"All types (*.*)");
	return filename;
}

std::shared_ptr<Model> ModelUtil::execImportModel(ModelGroup * group,
	QString current_dir,
	QString last_dir,
	QWidget *parent)
{
	QString filename = execModelFileDialog(parent, last_dir);
	if (filename.isEmpty()) {
		return nullptr;
	}
	qInfo() << "importing - " << filename;

	// model options dialog
	std::shared_ptr<Model> model = std::make_shared<Model>();
	//model->setPath(filename.toStdString());
	//model->setEmbedded(false);
	//model->setRelativePath(true);

	ModelEditDialog mdlg(true, current_dir, last_dir, parent);

	// init file
	mdlg.setName(QUrl(filename).fileName());
	mdlg.setFullFileName(filename);
	mdlg.setRelative(false);
	mdlg.setEmbedded(true);

	int result = mdlg.exec();
	if (result == QDialog::Rejected) {
		return nullptr;
	}

	QString path = mdlg.fileName();
	QString full_path = Util::resolvePath(mdlg.fileName(), current_dir);

	auto loaded_model = execLoadModel(full_path);

	if (!loaded_model) {
		QMessageBox::warning(parent, "Import Error", "Failed to load model " + full_path);
		return nullptr;
	}

	// take the results
	model->setName(mdlg.name().toStdString());
	if (mdlg.embedded()) {
		model->embedModel(loaded_model);
	}
	else {
		model->setFile(path.toStdString(), loaded_model);
	}

	group->addModel(model);

	return model;

}

void ModelUtil::execEditModel(std::shared_ptr<Model> model,
	QString current_dir,
	QString last_dir,
	QWidget *parent)
{
	bool old_embedded = model->embedded();
	QString old_full_path = Util::resolvePath(model->path().c_str(), current_dir);

	ModelEditDialog mdlg(false, current_dir, last_dir, parent);

	if (!old_embedded) {
		mdlg.setFileName(QString::fromStdString(model->path()));
	}
	mdlg.setName(QString::fromStdString(model->name()));
	mdlg.setEmbedded(model->embedded());

	int result = mdlg.exec();

	if (result == QDialog::Rejected) {
		return;
	}
	QString full_path = Util::resolvePath(mdlg.fileName(), current_dir);

	// reload when
	// 1. it was embedded, but now there is a file
	// 2. was a file, file changed
	bool reload_model =
		(old_embedded && !mdlg.fileName().isEmpty())
		|| (!old_embedded && old_full_path != full_path);

	osg::ref_ptr<osg::Node> node;

	if (reload_model) {
		node = execLoadModel(full_path);
		if (!node) {
			QMessageBox::warning(parent, "Load Error", "Failed to load model " + full_path);
			node = nullptr;
		}
	}
	else {
		node = model->nodeRef();
	}

	// assign everything to model
	model->setName(mdlg.name().toStdString());
	if (mdlg.embedded()) {
		model->embedModel(node);
	}
	else {
		model->setFile(mdlg.fileName().toStdString(), node);
	}
}

void ModelUtil::execReloadModel(Model *model, QString current_dir, QWidget * parent)
{
	if (!model) return;
	if (model->embedded()) return;

	QString full_path = Util::resolvePath(QString::fromStdString(model->path()), current_dir);
	auto node = execLoadModel(full_path);

	model->setFile(model->path(), node);
}

osg::ref_ptr<osg::Node> ModelUtil::execLoadModel(const QString & filename)
{
	QFuture<osg::Node*> future;
	FutureDialog dlg;
	dlg.setText("Loading " + filename);
	dlg.setWindowTitle("Loading");

	osg::ref_ptr<osg::Node> loaded_model;

	future = QtConcurrent::run(&osgDB::readNodeFile, filename.toStdString());
	dlg.spin(future); // spin until done loading
	loaded_model = future.result();

	return loaded_model;
}

void ModelUtil::fixRelativePaths(ModelGroup * models, const QString & old_base, const QString & new_base)
{
	for (auto model : *models->group()) {
		QString path = QString::fromStdString(model->path());
		// only fix if non-embedded relative
		if (model->embedded()
			|| !QUrl(path).isRelative()) {
			continue;
		}
		std::string new_path = Util::fixRelativePath(path, old_base, new_base).toStdString();
		model->setFile(new_path, model->node());
	}
}
