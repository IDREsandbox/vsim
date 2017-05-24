#ifndef NARRATIVEOVERLAY_TEST_H
#define NARRATIVEOVERLAY_TEST_H

#include <QtWidgets/QMainWindow>
#include "ui_narrativeoverlay_test.h"

class NarrativeOverlay_test : public QMainWindow
{
	Q_OBJECT

public:
	NarrativeOverlay_test(QWidget *parent = 0);
	~NarrativeOverlay_test();

private:
	Ui::MainWindow ui;
};

#endif // NARRATIVEOVERLAY_TEST_H
