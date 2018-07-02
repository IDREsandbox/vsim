/*
 *  VSimApp.h
 *	Owns most of the big classes
 *  Handles file operations
 *
 */

#ifndef VSIMAPP_H
#define VSIMAPP_H

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <QTimer>
#include <QElapsedTimer>
#include <QUndoStack>
#include <vector>
#include <QSize>
#include <memory>

class NarrativeControl;
class NarrativePlayer;
class ERControl;
class MainWindow;
class VSimRoot;
class OSGYearModel;
class Narrative;
class NavigationControl;
class NarrativeSlide;
class CanvasContainer;
class OSGViewerWidget;
class TimeManager;
class BrandingControl;

class VSimApp : public QObject
{
	Q_OBJECT
public:
	VSimApp(MainWindow*);
	~VSimApp();
	void setWindow(MainWindow*);

	// takes() from root
	// doesn't reassign our current m_root
	// this is called on every new, reset, etc
	bool initWithVSim(VSimRoot *root = nullptr);

	// state
	enum State {
		EDIT_NARS,
		EDIT_SLIDES,
		EDIT_CANVAS,
		EDIT_ERS,
		EDIT_FLYING,
		EDIT_BRANDING,
		PLAY_WAIT_CLICK,
		PLAY_WAIT_TIME,
		PLAY_TRANSITION,
		PLAY_END,
		PLAY_FLYING
	};
	static constexpr const char *StateStrings[] = {
		"Edit Narratives",
		"Edit Slides",
		"Edit Canvas",
		"Edit ERs",
		"Edit Flying",
		"Edit Branding",
		"Play Wait Click",
		"Play Wait Time",
		"Play Transition",
		"Play End",
		"Play Flying",
	};
	State state() const;
	void setState(State state);

	bool isPlaying() const;
	static bool isPlaying(VSimApp::State state);
	bool isFlying() const;

	void update(float dt_sec);

	// status
	void setStatusMessage(const QString &message, int ms = 2000);

	// camera
	osg::Vec3d getPosition() const;
	osg::Matrixd getCameraMatrix() const;
	void setCameraMatrix(const osg::Matrixd &matrix);
	void setCameraMatrixSmooth(const osg::Matrixd &matrix, float time);
	bool goingSomewhere() const;
	void stopGoingSomewhere();
	QImage generateThumbnail(NarrativeSlide *slide);

	// undo/redo
	QUndoStack *getUndoStack() const;

	// last directory used by a file dialog
	std::string getLastDiretory() const;
	void setLastDirectory(const std::string &dir, bool isFile = false);

	// current vsim working directory, based on the .vsim file location
	// always absolute
	std::string getCurrentDirectory() const;
	
	// absolute path to current file, or none
	std::string getFileName() const;
	void setFileName(const std::string &);

	VSimRoot *getRoot() const;
	//OSGYearModel *modelTable() const;
	NarrativeControl *narrativeControl() const;
	ERControl *erControl() const;
	NavigationControl *navigationControl() const;
	TimeManager *timeManager() const;
	BrandingControl *brandingControl() const;

	void debugCamera();

signals:
	void sTick(double sec);
	void sAboutToReset();
	void sReset();
	void sStateChanged(VSimApp::State old, VSimApp::State current);
	void sPositionChanged(const osg::Vec3 &pos);

	void sGoingSomewhere();
	void sArrived();
	void sInterrupted();

private:
	MainWindow *m_window;

	QUndoStack *m_undo_stack;

	std::string m_last_directory;
	std::string m_filename;
	bool m_model_loaded;

	QTimer *m_timer;
	QElapsedTimer *m_dt_timer;

	std::unique_ptr<VSimRoot> m_root;
	//ModelGroupModel *m_model_table_model;

	NarrativeControl *m_narrative_control;
	ERControl *m_er_control;
	NarrativePlayer *m_narrative_player;
	NavigationControl *m_navigation_control;
	TimeManager *m_time_manager;
	BrandingControl *m_branding_control;

	osg::Matrixd m_camera_start;
	osg::Matrixd m_camera_target;
	QTimer *m_camera_timer;

	osg::Vec3d m_position;

	QTimer *m_slide_timer;

	State m_state;

	// thumbnails
	osg::ref_ptr<osgViewer::CompositeViewer> m_viewer;
	CanvasContainer *m_render_canvas;
	OSGViewerWidget *m_render_view;
	OSGViewerWidget *m_main_view;
	QSize m_thumbnail_size;
};

//extern VSimApp* g_vsimapp;

#endif
