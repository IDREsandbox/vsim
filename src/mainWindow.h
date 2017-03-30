#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QtWidgets/QMainWindow>

#include "ui_MainWindow.h"
#include "OSGViewerWidget.h"
#include "dragLabel.h"

#include "VSimApp.h"

extern osgViewer::Viewer* g_viewer;

class VSimApp;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

	void paintEvent(QPaintEvent* event);

	OSGViewerWidget* getOSGWidget() const { return m_osg_widget; };


public slots:
	void actionNew();
	void actionOpen();
	void actionSave();
	void actionSaveAs();
	void actionImportModel();

private:
	Ui::MainWindow ui;
	OSGViewerWidget* m_osg_widget;

	QWidget* m_drag_area;
	
	// main window owns the vsim app
	// circular reference -> use a pointer
	std::unique_ptr<VSimApp> m_vsimapp;
};


#endif // MAINWINDOW_H