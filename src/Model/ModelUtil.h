#ifndef MODELUTIL_H
#define MODELUTIL_H

#include <memory>
#include <QString>
#include <QWidget>
#include <osg/Node>

class Model;
class ModelGroup;

// TODO: pick a better name for this class
namespace ModelUtil {
	// opens a file dialog
	QString execModelFileDialog(QWidget *parent, QString last_dir);

	// includes gui stuff, opens gui window, spins, etc
	// returns nullptr on failure/cancel
	// current_dir is for relative model paths
	// last_dir is where the file dialog starts
	std::shared_ptr<Model> execImportModel(ModelGroup *group,
		QString current_dir,
		QString last_dir,
		QWidget *parent);

	void execEditModel(std::shared_ptr<Model> model,
		QString current_dir,
		QString last_dir,
		QWidget *parent);

	void execReloadModel(Model *model,
		QString current_dir,
		QWidget *parent);

	osg::ref_ptr<osg::Node> execLoadModel(const QString &filename);

	void fixRelativePaths(ModelGroup *model,
		const QString &old_base,
		const QString &new_base);
};

#endif