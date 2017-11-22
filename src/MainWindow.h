#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QErrorMessage>
#include <QtWidgets/QProgressDialog>
#include <QDropEvent>
#include <QUndoStack>
#include <osgViewer/Viewer>

namespace Ui { class MainWindow; }

class VSimApp;
class OSGViewerWidget;
class labelCanvas;
class labelCanvasView;
class ModelOutliner;
class TimeSlider;
class MainWindowTopBar;
class ERDisplay;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

	// linking
	OSGViewerWidget *getViewerWidget() const;
	osgViewer::Viewer* getViewer() const;

	MainWindowTopBar *topBar() const;
	labelCanvasView *canvasView() const;
	labelCanvas *canvas() const;
	ModelOutliner *outliner() const;
	TimeSlider *timeSlider() const;
	ERDisplay *erDisplay() const;

	// event stuff
	void paintEvent(QPaintEvent* event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

	void selectResources(std::set<int> res);
	void selectCategories(std::set<int> res);
	int nextSelectionAfterDelete(int total, std::set<int> selection);

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
	void sSaveCurrent();
	void sNew();
	void sImportModel(const std::string&);
	void sImportNarratives();
	void sExportNarratives();

	void sDebugOSG();
	void sDebugCamera();
	void sDebugControl();

	void sEditStyleSettings();

	// ui, signal emitters
public:
	Ui::MainWindow *ui;

public:
	QUndoStack *m_undo_stack;

	OSGViewerWidget *m_osg_widget;
	labelCanvas *m_drag_area;
	labelCanvasView *m_view;
	ERDisplay *m_er_display;
	ModelOutliner *m_outliner;
	TimeSlider *m_time_slider;
};

#endif // MAINWINDOW_H
