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
#include <QTimer>
#include <QElapsedTimer>
#include <QUndoStack>
#include <vector>
#include <QSize>

#include "VSimRoot.h"

class NarrativeControl;
class NarrativePlayer;
class ERControl;
class MainWindow;
class VSimRoot;
class ModelTableModel;
class Narrative2;
class NavigationControl;
class NarrativeSlide;
class NarrativeCanvas;
class OSGViewerWidget;

class VSimApp : public QObject
{
	Q_OBJECT
public:
	VSimApp(MainWindow*);
	void setWindow(MainWindow*);

	// this is called on every new, reset, etc
	bool initWithVSim(osg::Node *root = nullptr);

	// state
	enum State {
		EDIT_NARS,
		EDIT_SLIDES,
		EDIT_CANVAS,
		EDIT_ERS,
		EDIT_FLYING,
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
		"Play Wait Click",
		"Play Wait Time",
		"Play Transition",
		"Play End",
		"Play Flying",
	};
	State state() const;
	void setState(State state);

	bool isPlaying() const;
	bool isFlying() const;

	void play();
	void stop();
	void timerExpire();
	void moveTimerExpire();
	void transitionTo(int index);
	void startFlying();
	void editNarratives();
	void next();
	void prev();

	void update(float dt_sec);

	// status
	void setStatusMessage(const QString &message, int timeout = 0);

	// camera
	osg::Matrixd getCameraMatrix() const;
	void setCameraMatrix(const osg::Matrixd &matrix);
	void setCameraMatrixSmooth(const osg::Matrixd &matrix, float time);
	QImage generateThumbnail(NarrativeSlide *slide);

	// undo/redo
	QUndoStack *getUndoStack() const;

	// file stuff
	QString getCurrentDirectory() const;
	std::string getFileName() const;
	void setFileName(const std::string &);

	void addModel(osg::Node *node, const std::string &name);
	bool importModel(const std::string& filename); // TODO: more complicated logic with vsim
	bool openVSim(const std::string& filename);
	bool saveVSim(const std::string& filename);
	bool saveCurrentVSim();
	bool exportNarratives();
	bool importNarratives();
	bool exportResources();
	bool importResources();

	// components
	VSimRoot *getRoot() const;
	ModelTableModel *modelTable() const;
	NarrativeControl *narrativeControl() const;
	ERControl *erControl() const;

	void debugCamera();

signals:
	void sTick(double sec);
	void sAboutToReset();
	void sReset();
	void sStateChanged(VSimApp::State old, VSimApp::State current);

private:
	MainWindow *m_window;

	QUndoStack *m_undo_stack;

	std::string m_filename;
	bool m_model_loaded;

	QTimer *m_timer;
	QElapsedTimer *m_dt_timer;

	osg::ref_ptr<VSimRoot> m_root;
	ModelTableModel *m_model_table_model;

	NarrativeControl *m_narrative_control;
	ERControl *m_er_control;
	NarrativePlayer *m_narrative_player;
	NavigationControl *m_navigation_control;

	osg::Matrixd m_camera_start;
	osg::Matrixd m_camera_target;
	QTimer *m_camera_timer;

	QTimer *m_slide_timer;

	State m_state;
	bool m_play_state;
	bool m_playing;
	bool m_editing_narrative;
	bool m_flying;
	bool m_transitioning;
	int m_transition_to;

	// thumbnails
	NarrativeCanvas *m_render_canvas;
	OSGViewerWidget *m_render_view;
	QSize m_thumbnail_size;
};

//extern VSimApp* g_vsimapp;

#endif
