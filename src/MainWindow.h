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
#include "labelCanvas.h"
#include "labelCanvasView.h"
#include "resources/ERDisplay.h"

//#include "VSimApp.h"

//extern osgViewer::Viewer* g_viewer;

class VSimApp;
class labelCanvas;
class labelCanvasView;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

	// UI stuff (these are just tests, TODO: remove, just use the built in qt dialogs)
	void ErrorDialog(const std::string& msg);
	void MessageDialog(const std::string& msg);
	void LoadingDialog(const std::string& msg);

	// linking
	//OSGViewerWidget *getViewerWidget() const { return m_osg_widget;	}
	osgViewer::Viewer* getViewer() const { return m_osg_widget->getViewer(); };

	// event stuff
	void paintEvent(QPaintEvent* event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

	EResource* getResource(int idx);
	void selectResources(std::set<int> res);
	int nextSelectionAfterDelete(int total, std::set<int> selection);

public slots:
	void actionNew();
	void actionOpen();
	void actionSave();
	void actionSaveAs();
	void actionImportModel();

	void newER();
	void deleteER();
	void editERInfo();
	void openResource();

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
	void sSaveCurrent();
	void sNew();
	void sImportModel(const std::string&);

	// ui, signal emitters
public:
	Ui::MainWindow ui;

public:
	OSGViewerWidget *m_osg_widget;
	labelCanvas *m_drag_area;
	QUndoStack *m_undo_stack;
	labelCanvasView *m_view;
	ERDisplay *m_display;
	osg::ref_ptr<EResourceGroup> m_resource_group;
};

enum SelectionCommandWhen {
	ON_UNDO,
	ON_REDO,
	ON_BOTH
};

class SelectResourcesCommand : public QUndoCommand {
public:
	SelectResourcesCommand(MainWindow *control, std::set<int> resources, SelectionCommandWhen when = ON_BOTH, QUndoCommand *parent = nullptr);
	void undo();
	void redo();
private:
	MainWindow *m_control;
	SelectionCommandWhen m_when;
	std::set<int> m_resources;
};

#endif // MAINWINDOW_H
