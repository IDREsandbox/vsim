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
class SwitchManager;

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

	void prepareSave();

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

	// undo/redo
	QUndoStack *getUndoStack() const;

	// last directory used by a file dialog
	std::string getLastDirectory() const;
	void setLastDirectory(const std::string &dir, bool isFile = false);

	// current vsim working directory, based on the .vsim file location
	// always absolute
	// when current file is none, it uses exe dir
	QString getCurrentDirectory() const;
	
	// absolute path to current file, or none
	QString getCurrentFile() const;
	// fix_relative changes relative paths so that they keep pointing to the same files
	void setCurrentFile(const QString &, bool fix_relative = false);

	VSimRoot *getRoot() const;
	//OSGYearModel *modelTable() const;
	NarrativeControl *narrativeControl() const;
	NarrativePlayer *narrativePlayer() const;
	ERControl *erControl() const;
	NavigationControl *navigationControl() const;
	TimeManager *timeManager() const;
	BrandingControl *brandingControl() const;
	OSGViewerWidget *viewer() const;
	SwitchManager *switchManager() const;

	void debugCamera();

signals:
	void sTick(double sec);
	void sAboutToSave();
	void sAboutToReset();
	void sReset();
	void sStateChanged(VSimApp::State old, VSimApp::State current);
	void sPositionChanged(const osg::Vec3 &pos);

	void sGoingSomewhere();
	void sArrived();
	void sInterrupted();

	void sCurrentDirChanged(const QString &old_dir, const QString &new_dir);

private:
	void connectSwitchManager();

private:
	MainWindow *m_window;

	QUndoStack *m_undo_stack;

	std::string m_last_directory;
	QString m_current_file;
	bool m_model_loaded;

	QTimer *m_timer;
	QElapsedTimer *m_dt_timer;

	std::unique_ptr<VSimRoot> m_root;
	//ModelGroupModel *m_model_table_model;

	OSGViewerWidget *m_viewer;
	NarrativeControl *m_narrative_control;
	ERControl *m_er_control;
	NarrativePlayer *m_narrative_player;
	NavigationControl *m_navigation_control;
	TimeManager *m_time_manager;
	BrandingControl *m_branding_control;
	SwitchManager *m_switch_manager;

	osg::Matrixd m_camera_start;
	osg::Matrixd m_camera_target;
	QTimer *m_camera_timer;

	osg::Vec3d m_position;

	QTimer *m_slide_timer;

	State m_state;
};

//extern VSimApp* g_vsimapp;

#endif
