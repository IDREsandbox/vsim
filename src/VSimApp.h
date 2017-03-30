/*
 *  VSimApp.h
 *	This class has everything important.
 *
 */

#ifndef _VSIMAPP_H_
#define _VSIMAPP_H_

#include "MainWindow.h"

class MainWindow;

class VSimApp
{
public:
    VSimApp(MainWindow*);
    //~VSimApp();

    osgViewer::Viewer* getViewer() { return m_viewer;}

	void reset();
	bool importModel(const std::string& filename);
	bool openVSim(const std::string& filename);
	bool saveVSim(const std::string& filename);
    
    osg::Node* getModel() const { return m_model.get(); }

protected:
	MainWindow* m_window;
	osgViewer::Viewer* m_viewer;

    std::string m_modelFilename;
    osg::ref_ptr<osg::Node> m_model;
	bool m_model_loaded;
};

extern VSimApp* g_vsimapp;

#endif
