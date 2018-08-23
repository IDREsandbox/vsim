#include "ExportDialog.h"

#include <QFileDialog>

ExportDialog::ExportDialog(QWidget * parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.export_all->setChecked(true);
	ui.lock->setChecked(false);
	connect(ui.file_button, &QAbstractButton::pressed,
		this, &ExportDialog::onChooseFile);
}

int ExportDialog::exec()
{
	// how do i inject an action on the first show? override exec()?
	// i feel like i've had this problem before (how to focus x on show?)
	bool ok = onChooseFile();
	if (!ok) {
		reject();
		return QDialog::Rejected;
	}

	return QDialog::exec();
}

QString ExportDialog::path() const
{
	return ui.file_edit->text();
}

bool ExportDialog::exportAll() const
{
	return ui.export_all->isChecked();
}

bool ExportDialog::lock() const
{
	return ui.lock->isChecked();
}

bool ExportDialog::usePassword() const
{
	return ui.use_password->isChecked();
}

QString ExportDialog::password() const
{
	return ui.password->text();
}

void ExportDialog::setTitle(QString title)
{
	m_title = title;
	setWindowTitle(title);
}

void ExportDialog::setTypes(QString types)
{
	m_types = types;
}

bool ExportDialog::onChooseFile()
{
	QString start;
	start = ui.file_edit->text();
	QString filename = QFileDialog::getSaveFileName(this, m_title,
		start, m_types);

	if (filename == "") {
		return false;
	}
	ui.file_edit->setText(filename);
	return true;
}
