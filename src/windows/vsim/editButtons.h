#ifndef EDITBUTTONS_H
#define EDITBUTTONS_H

#include "ui_editButtons.h"

class editButtons : public QWidget
{
	Q_OBJECT

public:
	editButtons(QWidget* parent = nullptr);

signals:
	void sNewLabel(std::string);
	void sDeleteLabel(int);
	void sEditExit();

private:
	Ui::editButtons ui;
};

#endif // EDITBUTTONS_H
