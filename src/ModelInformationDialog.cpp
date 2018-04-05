#include "ModelInformationDialog.h"

#include "ui_ModelInformationDialog.h"

#include "types_generated.h"
#include "settings_generated.h"

ModelInformationDialog::ModelInformationDialog(
	const VSim::FlatBuffers::ModelInformationT *data,
	QWidget *parent)
	: QDialog(parent)
{
	ui = std::make_unique<Ui::modelInformationDialog>();
	ui->setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	// set ui stuff
	if (!data) return;
	// TODO: try to grab defaults?

	// data -> ui
	ui->authors->setPlainText(QString::fromStdString(data->authors));
	ui->contributors->setPlainText(QString::fromStdString(data->contributors));
	ui->place->setPlainText(QString::fromStdString(data->place_of_publication));
	ui->modelName->setText(QString::fromStdString(data->model_name));
	ui->website->setText(QString::fromStdString(data->url));
	ui->projectDate->setDate(QDate::fromJulianDay(data->project_date_julian_day));
	ui->releaseDate->setDate(QDate::fromJulianDay(data->release_date_julian_day));
}

ModelInformationDialog::~ModelInformationDialog()
{
}


VSim::FlatBuffers::ModelInformationT ModelInformationDialog::getData()
{
	VSim::FlatBuffers::ModelInformationT data;
	// ui -> data
	data.authors = ui->authors->toPlainText().toStdString();
	data.contributors = ui->contributors->toPlainText().toStdString();
	data.place_of_publication = ui->place->toPlainText().toStdString();
	data.model_name = ui->modelName->text().toStdString();
	data.url = ui->website->text().toStdString();
	data.project_date_julian_day = ui->projectDate->date().toJulianDay();
	data.release_date_julian_day = ui->releaseDate->date().toJulianDay();

	return data;
}

