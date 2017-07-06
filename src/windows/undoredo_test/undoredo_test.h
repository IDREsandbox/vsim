#ifndef UNDOREDO_TEST_H
#define UNDOREDO_TEST_H

#include <QtWidgets/QMainWindow>
#include "ui_undoredo_test.h"

class undoredo_test : public QMainWindow
{
	Q_OBJECT

public:
	undoredo_test(QWidget *parent = 0);
	~undoredo_test();

private:
	Ui::undoredo_testClass ui;
};

#endif // UNDOREDO_TEST_H
