/*
 *  VSimApp.h
 *	This class has everything important.
 *
 *  Created by Eduardo Poyart on 11/5/10.
 */

#ifndef _VSIMAPP_H_
#define _VSIMAPP_H_

#include "MainWindow.h"

//class SwitchManager;
//class TimeSwitch;
//class NarrativeManager;
//class NavigationManager;
//class LightingManager;
//class ContentCreatorManager;
//class AboutVSimManager;
//class CCoordinateManager;
//class ModelInfoManager;
//class DisplayManager;
//class RestrictionsManager;
//class OverlayManager;
//class BrandingManager;
//class EResourcesManager;
//class ImageCaptureHandler;
//class SaveImageManager;
//class SaveMovieManager;

class MainWindow;

class VSimApp
{
public:
    VSimApp(MainWindow*);
    //~VSimApp();

    osgViewer::Viewer* getViewer() { return m_viewer;}
    //int initViewer();
    int init();

	bool importModel(const std::string& filename);
    
    osg::Node* getModel() const { return m_model.get(); }
 //   OverlayManager* getOverlayManager() { return m_overlay_manager; }
 //   NarrativeManager* getNarrativeManager() { return m_narrative_manager; }
 //   EResourcesManager* getEResourcesManager() { return m_eresources_manager; }
	//ContentCreatorManager* getCCreaterManager(){ return m_ccreator_manager; }
	//ModelInfoManager* getModelInfoManager(){ return m_model_info_manager;}
	//BrandingManager* getBrandingManager(){return m_branding_manager;}
	//TimeSwitch* getTimeSwitch(){return m_time_switch;}
    //void CloseAllDialogs();

protected:
    void insertExternalReferences();

	MainWindow* m_window;
	osgViewer::Viewer* m_viewer;
    //NRTSViewer* m_viewer;
    //SwitchManager* m_switch_manager;
    //TimeSwitch* m_time_switch;
    //NarrativeManager* m_narrative_manager;
    //OverlayManager* m_overlay_manager;
	//NavigationManager* m_navigation_manager;
	//ContentCreatorManager* m_ccreator_manager;
	//AboutVSimManager* m_about_vsim_manager;
	//CCoordinateManager* m_current_coordinate_manager;
	//ModelInfoManager* m_model_info_manager;
	//DisplayManager* m_display_manager;
	//EResourcesManager* m_eresources_manager;
	//BrandingManager* m_branding_manager;
	//SaveImageManager* m_image_manager;
	//SaveMovieManager* m_movie_manager;

    std::string m_modelFilename;
    osg::ref_ptr<osg::Node> m_model;
	bool m_model_loaded;
};

extern VSimApp* g_vsimapp;

#endif
