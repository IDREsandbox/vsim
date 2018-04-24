#ifndef ERDISPLAY_H
#define ERDISPLAY_H
#include <QWidget>
#include "ui_ERDisplay.h"
#include "resources/EResource.h"

class ERDisplay : public QFrame {
	Q_OBJECT

public:
	ERDisplay(QWidget *parent = nullptr);
	void setInfo(EResource* er);
	void setCount(int n);

	//public slots:
	//void showER();
	//void hideER();

	void reload();

signals:
	void sClose();
	void sCloseAll();
	void sOpen();
	void sGoto();

private:
	Ui::ERDisplay ui;
	EResource* m_er;
};

#endif // ERDISPLAY_H