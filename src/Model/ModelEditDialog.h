#ifndef MODELEDITDIALOG_H
#define MODELEDITDIALOG_H

#include <QDialog>
#include <memory>

class Model;
namespace Ui { class ModelEditDialog; };

// Edits a Model's metadata - including name, file path, embedded
class ModelEditDialog : public QDialog {
	Q_OBJECT;
public:
	ModelEditDialog(bool importing,
		QString base_dir,
		QString last_dir,
		QWidget *parent = nullptr);
	~ModelEditDialog() override;

	void setName(const QString &name);
	void setFileName(const QString &filename); // also sets relative
	void setFullFileName(const QString &filename);
	void setRelative(bool relative);
	void setEmbedded(bool embed);

	QString name();
	QString fileName() const;
	bool embedded() const;

private:
	void check();

	void updateFileName();
	void updateRelative();

private:
	std::unique_ptr<Ui::ModelEditDialog> ui;
	bool m_importing;

	QString m_base_dir;
	QString m_last_dir;

	QString m_full_file_name;
};

#endif