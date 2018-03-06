#ifndef MAINWINDOWTOPBAR_H
#define MAINWINDOWTOPBAR_H
#include <QFrame>
#include "ui_MainWindowTopBar.h"

class MainWindowTopBar : public QFrame {
	Q_OBJECT

public:
	MainWindowTopBar(QWidget * parent = nullptr);
	~MainWindowTopBar();

	void showNarratives();
	void showSlides();

	void setSlidesHeader(std::string);

	// TODO
	//void enableEdting(bool);
signals:
	void sNarrativesPoked();
	void sSlidesPoked();

public:
	Ui::MainWindowTopBar ui;
};

#endif // MAINWINDOWTOPBAR_H