#ifndef EDITBUTTONS_H
#define EDITBUTTONS_H

#include "ui_editButtons.h"
#include "LabelType.h"

class editButtons : public QFrame
{
	Q_OBJECT

public:
	editButtons(QWidget* parent = nullptr);
	QPushButton *button(LabelType type);

signals:
	//void sNewLabel(std::string);
	//void sDeleteLabel(int);
	//void sEditExit();
	void sPoked();

public:
	Ui::editButtons ui;
};

#endif // EDITBUTTONS_H
