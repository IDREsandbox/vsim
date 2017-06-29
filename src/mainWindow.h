#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QErrorMessage>
#include <QtWidgets/QProgressDialog>
#include <QDropEvent>

#include "ui_MainWindow.h"
#include "OSGViewerWidget.h"
#include "dragLabel.h"
#include "narrative/NarrativeInfoDialog.h"
#include "dragLabelInput.h"

//#include "VSimApp.h"

extern osgViewer::Viewer* g_viewer;

class VSimApp;
class dragLabel;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

	void resizeEvent(QResizeEvent * event);

	// UI stuff (these are just tests, TODO: remove, just use the built in qt dialogs)
	void ErrorDialog(const std::string& msg);
	void MessageDialog(const std::string& msg);
	void LoadingDialog(const std::string& msg);

	// linking
	osgViewer::Viewer* getViewer() const { return m_osg_widget->getViewer(); };

	// event stuff
	void paintEvent(QPaintEvent* event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

public slots:
	void actionNew();
	void actionOpen();
	void actionSave();
	void actionSaveAs();
	void actionImportModel();

	// TODO
	// void narListForward();
	// void narListAdd();
	// void narListDelete();
	// void narListPause();
	// void narListOpen();
	// void narListInfo();

signals:
	void sOpenFile(const std::string&);
	void sSaveFile(const std::string&);
	void sNew();
	void sImportModel(const std::string&);

	// ui, signal emitters
public:
	Ui::MainWindow ui;
	dragLabelInput* dragLabelEdit;
	NarrativeInfoDialog *m_narrative_info_dialog;

private:
	OSGViewerWidget* m_osg_widget;
	dragLabel* test;
	QWidget* m_drag_area;
	//QPoint offset;
	int offset;
	
};


#endif // MAINWINDOW_H