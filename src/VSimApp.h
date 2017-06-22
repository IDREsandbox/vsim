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
#include "narrative/NarrativeList.h"

class NarrativeList;
class MainWindow;

class VSimApp : public QObject
{
	Q_OBJECT
public:
    VSimApp(MainWindow*);
    //~VSimApp();

    osgViewer::Viewer* getViewer() { return m_viewer;}
    NarrativeList* getNarList() { return m_narrative_list;}

	// file stuff
	void reset();
	bool importModel(const std::string& filename);
	bool openVSim(const std::string& filename);
	bool saveVSim(const std::string& filename);

    osg::Node* getModel() const { return m_model.get(); }

protected:
	MainWindow* m_window;
	osgViewer::Viewer* m_viewer;

    std::string m_model_filename;
    osg::ref_ptr<osg::Node> m_model;
	bool m_model_loaded;

	// narrative stuff, TODO: move to a narrative manager/player class?
	void extractNarrativesFromNode(osg::Node* node);

	//osg::ref_ptr<NarrativeList> m_narrative_list;
	NarrativeList *m_narrative_list;
};

extern VSimApp* g_vsimapp;

#endif
