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
class ModelOutliner;
class TimeSlider;
class MainWindowTopBar;
class ERDisplay;
class ERFilterArea;
class NarrativeCanvas;
class editButtons;
class ERBar;
class StatsWindow;
class HistoryWindow;

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

	MainWindowTopBar *topBar() const;
	NarrativeCanvas *canvas() const;
	NarrativeCanvas *fadeCanvas() const;
	ModelOutliner *outliner() const;
	TimeSlider *timeSlider() const;
	editButtons *labelButtons() const;
	// TextToolbar *textToolbar() const;

	QMenu *navigationMenu() const;

	HistoryWindow *historyWindow() const;

	// er
	ERBar *erBar() const;
	ERDisplay *erDisplay() const;
	ERFilterArea *erFilterArea() const;

	// event stuff
	virtual void dragEnterEvent(QDragEnterEvent *event) override;
	virtual void dropEvent(QDropEvent *event) override;
	virtual void resizeEvent(QResizeEvent *event) override;

	void updatePositions();

public slots:
	void actionNew();
	void actionOpen();
	void actionSave();
	void actionSaveAs();
	void actionImportModel();

	void actionImportNarratives();
	void actionExportNarratives();

	void actionImportERs();
	void actionExportERs();

	void execModelInformation();

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
	NarrativeCanvas *m_canvas;
	NarrativeCanvas *m_fade_canvas;
	ERDisplay *m_er_display;
	ERFilterArea *m_er_filter_area;
	ModelOutliner *m_outliner;
	TimeSlider *m_time_slider;
	editButtons *m_label_buttons;
	StatsWindow *m_stats_window;
	HistoryWindow *m_history_window;

	QActionGroup *m_navigation_action_group;
	QAction *m_action_first_person;
	QAction *m_action_flight;
	QAction *m_action_object;
};

#endif // MAINWINDOW_H
