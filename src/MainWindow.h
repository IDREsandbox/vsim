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
#include <QActionGroup>

namespace Ui { class MainWindow; }

class VSimApp;
class OSGViewerWidget;
class labelCanvas;
class labelCanvasView;
class ModelOutliner;
class TimeSlider;
class MainWindowTopBar;
class ERDisplay;
class ERFilterArea;
class ERScrollBox;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

	// app connection
	void setApp(VSimApp *vsim);
	void onReset();

	// linking
	OSGViewerWidget *getViewerWidget() const;
	osgViewer::Viewer* getViewer() const;

	MainWindowTopBar *topBar() const;
	labelCanvasView *canvasView() const;
	labelCanvas *canvas() const;
	ModelOutliner *outliner() const;
	TimeSlider *timeSlider() const;
	
	// er
	ERScrollBox *erLocal() const;
	ERScrollBox *erGlobal() const;
	ERDisplay *erDisplay() const;
	ERFilterArea *erFilterArea() const;
	QAbstractButton *newERButton() const;
	QAbstractButton *deleteERButton() const;
	QAbstractButton *editERButton() const;
	QAbstractButton *filterERButton() const;

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
	VSimApp *m_app;

	OSGViewerWidget *m_osg_widget;
	labelCanvas *m_drag_area;
	labelCanvasView *m_view;
	ERDisplay *m_er_display;
	ERFilterArea *m_er_filter_area;
	ModelOutliner *m_outliner;
	TimeSlider *m_time_slider;

	QActionGroup *m_navigation_action_group;
	QAction *m_action_first_person;
	QAction *m_action_flight;
	QAction *m_action_object;
};

#endif // MAINWINDOW_H
