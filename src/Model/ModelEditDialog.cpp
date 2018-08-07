#include "ModelEditDialog.h"

#include <QUrl>
#include <QDir>
#include <QDebug>

#include "ui_ModelEditDialog.h"
#include "Model.h"
#include "ModelUtil.h"
#include "Core/Util.h"

ModelEditDialog::ModelEditDialog(bool importing, QString base_dir, QString last_dir, QWidget *parent)
	: QDialog(parent),
	m_importing(importing),
	m_base_dir(base_dir),
	m_last_dir(last_dir)
{
	ui = std::make_unique<Ui::ModelEditDialog>();
	ui->setupUi(this);

	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui->embed->setToolTip("Check to embed the model into this vsim file. An unembedded model will load based on the path.");
	ui->relative->setToolTip("Check to load the model from a path relative to this vsim file");

	ui->file_edit->setReadOnly(true);

	if (importing) {
		setWindowTitle("Import Model");
	}
	else {
		setWindowTitle("Edit Model");
	}

	// apply
	//connect(this, &QDialog::accepted, this, [this]() {
	//	apply(m_model);
	//});

	connect(ui->file_button, &QAbstractButton::pressed, this, [this]() {
		QString result = ModelUtil::execModelFileDialog(this, m_last_dir);
		if (result.isEmpty()) return;
		setFullFileName(result);

		ui->name_edit->setText(QUrl(m_full_file_name).fileName());
	});

	connect(ui->relative, &QCheckBox::clicked, this, [this](bool checked) {
		updateFileName();
	});
}

ModelEditDialog::~ModelEditDialog()
{
}

void ModelEditDialog::setName(const QString & name)
{
	ui->name_edit->setText(name);
}

void ModelEditDialog::setFileName(const QString & filename)
{
	setFullFileName(Util::resolvePath(filename, m_base_dir));
	ui->file_edit->setText(filename);

	updateRelative();
}

void ModelEditDialog::setFullFileName(const QString & filename)
{
	m_full_file_name = filename;
	updateFileName();
}

void ModelEditDialog::setRelative(bool relative)
{
	ui->relative->setChecked(true);
	updateFileName();
}

void ModelEditDialog::setEmbedded(bool embed)
{
	ui->embed->setChecked(embed);
}

QString ModelEditDialog::name()
{
	return ui->name_edit->text();
}

bool ModelEditDialog::embedded() const
{
	return ui->embed->isChecked();
}

QString ModelEditDialog::fileName() const
{
	if (m_base_dir.isEmpty()) {
		return m_full_file_name;
	}
	else {
		if (ui->relative->isChecked()) {
			// figure out relative path
			QDir current(m_base_dir);
			QString rel = current.relativeFilePath(m_full_file_name);
			return rel;
		}
		else {
			// set whole
			return m_full_file_name;
		}
	}
}

void ModelEditDialog::check()
{
}

void ModelEditDialog::updateFileName()
{
	ui->file_edit->setText(fileName());

}

void ModelEditDialog::updateRelative()
{
	bool relative = QUrl(ui->file_edit->text()).isRelative();
	ui->relative->setChecked(relative);
}
