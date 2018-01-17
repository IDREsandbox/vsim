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

#include "VSimRoot.h"

class NarrativeControl;
class NarrativePlayer;
class ERControl;
class MainWindow;
class VSimRoot;
class ModelTableModel;

class VSimApp : public QObject
{
	Q_OBJECT
public:
	VSimApp(MainWindow*);
	void setWindow(MainWindow*);

	osgViewer::Viewer* getViewer();

	// this is called on every new, reset, etc
	bool initWithVSim(osg::Node *root);
	bool init();

	void update(float dt_sec);

	void addModel(osg::Node *node, const std::string &name);
	bool importModel(const std::string& filename); // TODO: more complicated logic with vsim
	bool openVSim(const std::string& filename);
	bool saveVSim(const std::string& filename);
	bool saveCurrentVSim();
	bool exportNarratives();
	bool importNarratives();

	VSimRoot *getRoot() const;

	QString getCurrentDirectory() const;
	std::string getFileName() const;
	void setFileName(const std::string &);

	void setStatusMessage(const QString &message, int timeout = 0);
	osg::Matrixd getCameraMatrix() const;
	void setCameraMatrix(const osg::Matrixd &matrix);
	void setCameraMatrixSmooth(const osg::Matrixd &matrix, float time);

	QUndoStack *getUndoStack() const;

	void debugCamera();

	ModelTableModel *modelTable() const;
	NarrativeControl *narrativeControl() const;
	ERControl *erControl() const;

signals:
	void tick(double sec);
	void foo();
	void sReset();

private:
	MainWindow *m_window;
	osgViewer::Viewer *m_viewer;

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

	osg::Matrixd m_camera_start;
	osg::Matrixd m_camera_target;
	QTimer *m_camera_timer;
};

//extern VSimApp* g_vsimapp;

#endif
