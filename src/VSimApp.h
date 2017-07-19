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
#include "narrative/NarrativeControl.h"
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
	NarrativeControl* getNarList() { return m_narrative_list;}

	// file stuff
	bool init();
	bool initWithModel(osg::Node *model);
	bool initWithVSim(osg::Node *root);

	void reset();
	bool importModel(const std::string& filename); // TODO: more complicated logic with vsim
	bool openVSim(const std::string& filename);
	bool saveVSim(const std::string& filename);
	bool saveCurrentVSim();

	osg::Group* getRoot() const { return m_root.get(); }

	std::string getFileName();
	void setFileName(const std::string &);

	void OSGDebug();

private:
	static osg::Group *findOrCreateChildGroup(osg::Group *root, const std::string &name);
	// creates Narratives, Models, etc groups
	static bool convertToNewVSim(osg::Group *root);
	// merges an existing vsim root with this one
	// expects new format
	bool mergeAnotherVSim(osg::Group *); 

	MainWindow* m_window;
	osgViewer::Viewer* m_viewer;

	std::string m_filename;
	osg::ref_ptr<osg::Group> m_root;
	bool m_model_loaded;

	osg::Group *m_narrative_group;
	osg::Group *m_model_group;

	NarrativePlayer *m_narrative_player;
	NarrativeControl *m_narrative_list;
};

extern VSimApp* g_vsimapp;

#endif
