/*
 *  VSimApp.h
 *	This class has everything important.
 *
 */

#ifndef _VSIMAPP_H_
#define _VSIMAPP_H_

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>

#include "MainWindow.h"
#include "VSimRoot.h"
#include "narrative/NarrativePlayer.h"

class NarrativeControl;
class MainWindow;

class VSimApp : public QObject
{
	Q_OBJECT
public:
	VSimApp(MainWindow*);
	//~VSimApp();

	osgViewer::Viewer* getViewer() { return m_viewer;}
	NarrativeControl* getNarList() { return m_narrative_control;}

	// this is called on every new, reset, etc
	bool initWithVSim(osg::Node *root); 
	bool initWithModel(osg::Node *model); 
	bool init();
	
	void reset();
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

private:

	MainWindow* m_window;
	osgViewer::Viewer* m_viewer;

	std::string m_filename;
	osg::ref_ptr<VSimRoot> m_root;
	bool m_model_loaded;

	NarrativePlayer *m_narrative_player;
	NarrativeControl *m_narrative_control;
};

extern VSimApp* g_vsimapp;

#endif
