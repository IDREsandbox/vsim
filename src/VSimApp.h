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
	bool init(osg::Node *model); // loads node, does error checking
	void reset();
	bool importModel(const std::string& filename);
	bool openVSim(const std::string& filename);
	bool saveVSim(const std::string& filename);

    osg::Node* getModel() const { return m_model.get(); }

protected:
	MainWindow* m_window;
	osgViewer::Viewer* m_viewer;

    std::string m_model_filename;
    osg::ref_ptr<osg::Group> m_model;
	bool m_model_loaded;

	NarrativeControl *m_narrative_list;
};

extern VSimApp* g_vsimapp;

#endif
