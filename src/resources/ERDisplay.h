#ifndef ERDISPLAY_H
#define ERDISPLAY_H
#include <QWidget>
#include "ui_ERDisplay.h"
#include "resources/EResource.h"

class ERDisplay : public QWidget {
	Q_OBJECT

public:
	ERDisplay(QWidget *parent = nullptr);
	void setInfo(EResource* er);

	//public slots:
	//void showER();
	//void hideER();

private:
	Ui::ERDisplay ui;
	EResource* m_er;
};

#endif // ERDISPLAY_H