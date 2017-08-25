#include "resources/NewCatDialog.h"
#include <qcolordialog.h>

NewCatDialog::NewCatDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	connect(ui.catColor, &QPushButton::clicked, this, &NewCatDialog::pickColor);
}
NewCatDialog::~NewCatDialog()
{
}

std::string NewCatDialog::getCatTitle() const
{
	return ui.catName->text().toStdString();
}
int NewCatDialog::getRed() const
{
	return color.red();
}
int NewCatDialog::getGreen()const
{
	return color.green();
}
int NewCatDialog::getBlue()const
{
	return color.blue();
}

void NewCatDialog::pickColor()
{
	QColorDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	color = dlg.selectedColor();
	ui.catColor->setStyleSheet(QString::fromStdString("background:rgb(" + std::to_string(color.red()) + "," + std::to_string(color.green()) + "," + std::to_string(color.blue()) + ");"));
}
