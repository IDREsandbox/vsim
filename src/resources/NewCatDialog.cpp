#include "resources/NewCatDialog.h"
#include <qcolordialog.h>
#include <QDebug>

NewCatDialog::NewCatDialog(QString window_title, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	setWindowTitle(window_title);

	setColor(QColor(0, 0, 150));

	connect(ui.catColor, &QPushButton::clicked, this, &NewCatDialog::pickColor);
}
NewCatDialog::~NewCatDialog()
{
}

void NewCatDialog::setTitle(QString title)
{
	ui.catName->setText(title);
}

QString NewCatDialog::getCatTitle() const
{
	return ui.catName->text();
}

void NewCatDialog::setColor(QColor color)
{
	m_color = color;
	QString style = "background:rgb("
		+ QString::number(color.red()) + ","
		+ QString::number(color.green()) + ","
		+ QString::number(color.blue()) + ");";
	ui.catColor->setStyleSheet(style);
}

QColor NewCatDialog::getColor() const
{
	return m_color;
}

void NewCatDialog::pickColor()
{
	QColorDialog dlg;
	dlg.setCurrentColor(m_color);
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	setColor(dlg.selectedColor());
}
