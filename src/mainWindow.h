#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include "OSGViewerWidget.h"
#include "dragLabel.h"
#include "ui_MainWindow.h"

extern osgViewer::Viewer* g_viewer;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);

	void paintEvent(QPaintEvent* event);
	void resizeEvent(QResizeEvent* event);

private:
	Ui::MainWindow ui;
	OSGViewerWidget* m_osg_widget;

	QWidget* m_drag_area;

};


#endif // MAINWINDOW_H