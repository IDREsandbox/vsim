#include "ModelInformationDialog.h"

#include "ui_ModelInformationDialog.h"

#include "types_generated.h"
#include "settings_generated.h"

ModelInformationDialog::ModelInformationDialog(
	const VSim::FlatBuffers::ModelInformationT *data,
	QWidget *parent)
	: QDialog(parent),
	m_read_only(false)
{
	ui = std::make_unique<Ui::modelInformationDialog>();
	ui->setupUi(this);
	setWindowFlag(Qt::WindowContextHelpButtonHint, false);
	setWindowTitle("Model Information");

	// set ui stuff
	if (!data) return;
	// TODO: try to grab defaults?

	// data -> ui
	ui->model_name->setText(QString::fromStdString(data->model_name));
	ui->release_date->setDate(QDate::fromJulianDay(data->release_date_julian));
	ui->release_notes->setPlainText(QString::fromStdString(data->release_notes));
	ui->project_date->setDate(QDate::fromJulianDay(data->project_date_julian_day));
	ui->project_date_end->setDate(QDate::fromJulianDay(data->project_date_end_julian));
	ui->authors->setPlainText(QString::fromStdString(data->authors));
	ui->contributors->setPlainText(QString::fromStdString(data->contributors));
	ui->place->setPlainText(QString::fromStdString(data->place_of_publication));
	ui->funders->setPlainText(QString::fromStdString(data->funders));
	ui->objective->setPlainText(QString::fromStdString(data->research_objective));
	ui->technology->setPlainText(QString::fromStdString(data->technology));
	ui->website->setText(QString::fromStdString(data->url));
}

ModelInformationDialog::~ModelInformationDialog()
{
}


VSim::FlatBuffers::ModelInformationT ModelInformationDialog::getData()
{
	VSim::FlatBuffers::ModelInformationT data;

	// ui -> data

	data.model_name = ui->model_name->text().toStdString();
	data.release_notes = ui->release_notes->toPlainText().toStdString();
	data.release_date_julian = ui->release_date->date().toJulianDay();
	data.project_date_julian_day = ui->project_date->date().toJulianDay();
	data.project_date_end_julian = ui->project_date_end->date().toJulianDay();
	data.authors = ui->authors->toPlainText().toStdString();
	data.contributors = ui->contributors->toPlainText().toStdString();
	data.place_of_publication = ui->place->toPlainText().toStdString();
	data.funders = ui->funders->toPlainText().toStdString();
	data.research_objective = ui->objective->toPlainText().toStdString();
	data.technology = ui->technology->toPlainText().toStdString();
	data.url = ui->website->text().toStdString();

	return data;
}

bool ModelInformationDialog::readOnly() const
{
	return m_read_only;
}

void ModelInformationDialog::setReadOnly(bool ro)
{
	ui->model_name->setReadOnly(ro);
	ui->release_date->setReadOnly(ro);
	ui->release_notes->setReadOnly(ro);
	ui->project_date->setReadOnly(ro);
	ui->project_date_end->setReadOnly(ro);
	ui->authors->setReadOnly(ro);
	ui->contributors->setReadOnly(ro);
	ui->place->setReadOnly(ro);
	ui->funders->setReadOnly(ro);
	ui->objective->setReadOnly(ro);
	ui->technology->setReadOnly(ro);
	ui->website->setReadOnly(ro);
}

