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
#include <memory>

#include "MainWindow.h"
#include "VSimRoot.h"
#include "ModelTableModel.h"

class NarrativeControl;
class NarrativePlayer;
class ERControl;
class MainWindow;

class VSimApp : public QObject
{
	Q_OBJECT
public:
	VSimApp(MainWindow*);
	//~VSimApp();

	osgViewer::Viewer* getViewer();

	// this is called on every new, reset, etc
	bool initWithVSim(osg::Node *root);
	bool init();

	void addModel(osg::Node *node, const std::string &name);
	bool importModel(const std::string& filename); // TODO: more complicated logic with vsim
	bool openVSim(const std::string& filename);
	bool saveVSim(const std::string& filename);
	bool saveCurrentVSim();
	bool exportNarratives();
	bool importNarratives();

	VSimRoot* getRoot() const { return m_root.get(); }

	std::string getFileName();
	void setFileName(const std::string &);

	void debugCamera();
	void updateTime();
signals:
	void tick(double sec);
	void foo();

private:
	QTimer *m_timer;
	QElapsedTimer *m_dt_timer;

	MainWindow* m_window;
	osgViewer::Viewer* m_viewer;

	std::string m_filename;
	bool m_model_loaded;

	osg::ref_ptr<VSimRoot> m_root;
	ModelTableModel m_model_table_model;

	NarrativeControl *m_narrative_control;
	ERControl *m_er_control;
	NarrativePlayer *m_narrative_player;
};

extern VSimApp* g_vsimapp;

#endif
