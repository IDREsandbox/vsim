#include "VSimApp.h"

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers> // stats handler
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/StateSetManipulator>

#include <osgDB/ReadFile>

#include "VSimApp.h"

#define OPTIMIZE 0

VSimApp* g_vsimapp;

VSimApp::VSimApp(MainWindow* window)
	: m_window(window)
{
	m_viewer = window->getOSGWidget()->getViewer();
	m_model = m_viewer->getSceneData();
}

//int VSimApp::initViewer()
//{
//
//
//    // set up the camera manipulators.
//    {
//        //osg::setNotifyLevel(osg::INFO);
//        //osg::ref_ptr<NRTSKeySwitchMatrixManipulator> keyswitchManipulator = new NRTSKeySwitchMatrixManipulator(m_viewer);
//		osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;
//
//        keyswitchManipulator->setAutoComputeHomePosition(false);
//
//		// addMatrixManipulator takes ownership
//		keyswitchManipulator->addMatrixManipulator('1', "Trackball", new osgGA::TrackballManipulator());
//		keyswitchManipulator->addMatrixManipulator('2', "FirstPerson", new osgGA::FirstPersonManipulator());
//
//		// TODO: integration - add these later
//		//keyswitchManipulator->addMatrixManipulator( '1', "NRTS", new NRTSManipulator(), true);
//		//keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
//		//keyswitchManipulator->addMatrixManipulator( '2', "USIM", new USIMManipulator(), true);
//
//        keyswitchManipulator->selectMatrixManipulator(0);
//
//		// takes ownership of KeySwitchMatrixManipulator
//        m_viewer->setCameraManipulator( keyswitchManipulator.get() );//set the manipulator for the viewer
//    }
//
//    // add the state manipulator
//    osgGA::StateSetManipulator* ssm = new osgGA::StateSetManipulator(m_viewer->getCamera()->getOrCreateStateSet());
//	//ssm->getStateSet()->setGlobalDefaults();
//	//ssm->getStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//	//m_viewer->getCamera()->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//
//    ssm->setKeyEventToggleBackfaceCulling('B');
//
//    ssm->setKeyEventToggleLighting('L');
//    ssm->setKeyEventToggleTexturing('X');
//    ssm->setKeyEventCyclePolygonMode('W');
//    ssm->setLightingEnabled(false);
//	m_viewer->addEventHandler(ssm);
//	
//    // add the thread model handler
//    //m_viewer->addEventHandler(new osgViewer::ThreadingHandler);
//
//    // add the window size toggle handler
//    osgViewer::WindowSizeHandler* wshandler = new osgViewer::WindowSizeHandler;
//    wshandler->setKeyEventToggleFullscreen('F');
//    m_viewer->addEventHandler(wshandler);
//
//    // add the stats handler
//    //osgViewer::StatsHandler* shandler = new osgViewer::StatsHandler;
//    //shandler->setKeyEventTogglesOnScreenStats('S');
//    //shandler->setKeyEventPrintsOutStats('P');
//    //m_viewer->addEventHandler(shandler);
//
//    // add the help handler
//    //m_viewer->addEventHandler(new osgViewer::HelpHandler(arguments.getApplicationUsage()));
//
//    // add the record camera path handler
//    //m_viewer->addEventHandler(new osgViewer::RecordCameraPathHandler);
//
//    // add the LOD Scale handler
//	osgViewer::LODScaleHandler *lodscalehandler = new osgViewer::LODScaleHandler();
//	lodscalehandler->setKeyEventDecreaseLODScale('-');
//	lodscalehandler->setKeyEventIncreaseLODScale('+');
//	m_viewer->addEventHandler(lodscalehandler);
//
//    // add the screen capture handler
//    //m_viewer->addEventHandler(new osgViewer::ScreenCaptureHandler);
//	//m_viewer->addEventHandler(m_movie_manager);
//    //m_viewer->realize();
//
//	// NOTE: integration - just use the defaults
//    
//#if defined(__APPLE__)
//    // Preload imageio (png, jpg, etc)
//    std::string libname = osgDB::Registry::instance()->createLibraryNameForExtension("imageio");
//    osgDB::Registry::instance()->loadLibrary(libname);
//#endif
//    
//    return 0;
//}

bool VSimApp::importModel(const std::string& filename)
{
	osg::ref_ptr<osg::Node> loadedModel(NULL);
	loadedModel = osgDB::readNodeFile(filename);
	if (!loadedModel.get()) {
		printf("error loading file %s\n", filename.c_str());
		return false;
	}

	m_model->asGroup()->addChild(loadedModel);
	return true;
}

int VSimApp::init()
{
    //m_model = new osg::Group();
	// integration - what is this?
    //m_model->setNodeMask(osgNewWidget::NODEMASK_3D);

	//osg::Camera* vcamera = m_viewer->getCamera();
	//vcamera->getOrCreateStateSet()->setGlobalDefaults();

    //osg::Group* sceneData = new osg::Group();
    //m_viewer->setSceneData(sceneData);
    //sceneData->addChild(m_model);

	// TODO: integration 
    //m_overlay_manager->init();
    //m_narrative_manager->init();
	//m_eresources_manager->init();
	//m_branding_manager->init();

    return 0;
}
//
//void VSimApp::TestInit(){
//	std::string modelpath = "C:\\Users\\ffang\\VSIM\\data\\Compostela\\Compostela_060112.vsim";
//	LoadFile(modelpath);
//	
//	//std::string narrativepath = "C:\\Users\\ffang\\VSIM\\data\\Compostela\\CompostelaTour_Oct8release.nar";
//	//m_narrative_manager->importNarrative(narrativepath);
//	//std::string erepath = "C:\\Users\\ffang\\VSIM\\data\\Compostela\\Compostela_ER_Oct8release.ere";
//	//m_eresources_manager->importEResources(erepath);
//}
//
//void VSimApp::OnFileOpen()
//{
//	OutputDebugStringA("line 564\n\n");
//    GenericFileDialog dlg;
//	dlg.init("(*.vsim; *.flt;*.ive;*.osg;*.osgb;*.osgt;*.obj;*.3ds; *.dae)|*.vsim;*.flt;*.ive;*.osg;*.osgb;*.osgt;*.obj;*.3ds; *.dae|All files (*.*)|*.*", VSIM_DIALOG_LOAD); //;*.dae
//    bool result = dlg.show();
//	OutputDebugStringA("line 567\n\n");
//    if (!result)
//        return;
//    string filename = dlg.getPathName();
//	OutputDebugStringA("line 571\n\n");
//    LoadFile(filename);
//}
//
//void VSimApp::OnFileAdd()
//{
//	if(m_model_loaded == false){
//	    MessageDialog md;
//        std::stringstream ss;
//		ss << "Please Load a file first "<<std::endl;
//        md.show(ss.str());
//        return;
//	}
//
//	GenericFileDialog dlg;
//	dlg.init("(*.vsim; *.flt;*.ive;*.osg;*.osgb;*.osgt;*.obj;*.3ds)|*.vsim;*.flt;*.ive;*.osg;*.osgb;*.osgt;*.obj;*.3ds|All files (*.*)|*.*", VSIM_DIALOG_LOAD); //;*.dae
//    bool result = dlg.show();
//    if (!result)
//        return;
//    string filename = dlg.getPathName();
//
//   osg::ref_ptr<osg::Node> loadedModel(NULL);
//    if (Util::getExtension(filename) == "vsim" )
//    {
//        std::ifstream ifs;
//        ifs.open(filename.c_str(), std::ios::binary);
//        if (!ifs.good())
//        {
//            MessageDialog md;
//            std::stringstream ss;
//            ss << "Error opening file " << filename << " for read.";
//            md.show(ss.str());
//            return;
//        }
//        osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
//        assert(rw);
//        ProgressDialog pd;
//        pd.show("Loading file. Please wait.");
//        osgDB::ReaderWriter::ReadResult rresult = rw->readNode(ifs);
//        pd.hide();
//        ifs.close();
//        if (rresult.success())
//        {
//            loadedModel = rresult.takeNode();
//        }
//    }
//    else
//    {
//        ProgressDialog pd;
//        pd.show("Loading file. Please wait.");
//        loadedModel = osgDB::readNodeFile(filename);
//        pd.hide();
//
//    }
//
//    if (!loadedModel.get())
//    {
//        MessageDialog md;
//        std::stringstream ss;
//        ss << "Error loading file: " << filename;
//        md.show(ss.str());
//        return;
//	}
//
//    //osgDB::writeNodeFile(*loadedModel.get(), "saved.osg");
//
//    // optimize the scene graph, remove redundant nodes and state etc.
//#if OPTIMIZE
//    osgUtil::Optimizer optimizer;
//    optimizer.optimize(loadedModel.get(), osgUtil::Optimizer::DEFAULT_OPTIMIZATIONS & ~osgUtil::Optimizer::REMOVE_REDUNDANT_NODES);
//#endif
//
//
//    m_model->asGroup()->addChild(loadedModel);
//
//    m_narrative_manager->addNarrativesFromNode(loadedModel);
//	m_eresources_manager->addEResourcesFromNode(loadedModel);
//
//}
//
//void VSimApp::OnFileSave()
//{
//    GenericFileDialog dlg;
//    dlg.init("VSim files (*.vsim)|*.vsim|IVE files (*.ive)|*.ive|OSG files (*.osg)|*.osg|OSGB files (*.osgb)|*.osgb|OSGT files (*.osgt)|*.osgt|All files (*.*)|*.*", VSIM_DIALOG_SAVE);
//    bool result = dlg.show();
//    if (!result)
//        return;
//    string filename = dlg.getPathName();
//    filename = Util::addExtensionIfNotExist(filename, "vsim");
//    SaveFile(filename);
//}
//
//void VSimApp::LoadFile(const string& filename)
//{
//    osg::ref_ptr<osg::Node> loadedModel(NULL);
//    if (Util::getExtension(filename) == "vsim")
//    {
//        std::ifstream ifs;
//        ifs.open(filename.c_str(), std::ios::binary);
//        if (!ifs.good())
//        {
//            MessageDialog md;
//            std::stringstream ss;
//            ss << "Error opening file " << filename << " for read.";
//            md.show(ss.str());
//            return;
//        }
//        osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
//        assert(rw);
//        ProgressDialog pd;
//        pd.show("Loading file. Please wait.");
//        osgDB::ReaderWriter::ReadResult rresult = rw->readNode(ifs);
//        pd.hide();
//        ifs.close();
//        if (rresult.success())
//        {
//            loadedModel = rresult.takeNode();
//        }
//    }
//    else
//    {
//        ProgressDialog pd;
//        pd.show("Loading file. Please wait.");
//		OutputDebugStringA("line 695\n\n");
//		OutputDebugStringA(filename.c_str());
//		OutputDebugStringA("\n\n");
//        loadedModel = osgDB::readNodeFile(filename);
//		OutputDebugStringA("line 697\n\n");
//        pd.hide();
//
//    }
//
//    if (!loadedModel.get())
//    {
//        MessageDialog md;
//        std::stringstream ss;
//        ss << "Error loading file: " << filename;
//        md.show(ss.str());
//        return;
//    }
//
//    //osgDB::writeNodeFile(*loadedModel.get(), "saved.osg");
//
//    // optimize the scene graph, remove redundant nodes and state etc.
//#if OPTIMIZE
//    osgUtil::Optimizer optimizer;
//    optimizer.optimize(loadedModel.get(), osgUtil::Optimizer::DEFAULT_OPTIMIZATIONS & ~osgUtil::Optimizer::REMOVE_REDUNDANT_NODES);
//#endif
//
//	// TODO: integration
//	//m_branding_manager->extractBrandingOverlayFromNode(loadedModel);
//	//m_ccreator_manager->extractModelInformationFromNode(loadedModel);
//	//if(m_ccreator_manager->ModelExpired()){
//	//	MessageDialog md;
// //       std::stringstream ss;
// //       ss << "File Expired! " << filename;
// //       md.show(ss.str());
//	//	return;//the model is expired
//	//}
// //   m_switch_manager->readFromModel(loadedModel.get());
// //   m_time_switch->readFromModel(loadedModel.get());
//    //osgDB::writeNodeFile(*loadedModel.get(), "saved-opt.osg");
//
//    if (m_viewer->getSceneData())
//    {
//        osg::Group* group = static_cast<osg::Group*>(m_viewer->getSceneData());
//        assert(group);
//        group->replaceChild(m_model, loadedModel);
//    }
//    else
//    {
//        m_viewer->setSceneData(loadedModel);
//    }
//
//    //m_narrative_manager->extractNarrativesFromNode(loadedModel);
//	//m_eresources_manager->extractEResourcesFromNode(loadedModel);
//
//	//m_viewer->getCamera()->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//
//	m_model_loaded = true;
//    m_model = loadedModel;
//    m_modelFilename = filename;
//
//	m_viewer->getCamera()->getView()->setLightingMode(osg::View::SKY_LIGHT);
//	osg::Light* h_light = m_viewer->getCamera()->getView()->getLight();
//	if (h_light != NULL)
//	{
//		h_light->setAmbient(osg::Vec4(0, 0, 0, 1));
//		h_light->setDiffuse(osg::Vec4(0, 0, 0, 1));
//		h_light->setSpecular(osg::Vec4(0, 0, 0, 1));
//	}
//
//	osg::ref_ptr<osg::ComputeBoundsVisitor> cbv = new osg::ComputeBoundsVisitor();
//	m_model->accept(*cbv);
//
//	osg::BoundingBox bb(cbv->getBoundingBox());
//
//	PositionAttitudeTransform *lightTransform;
//	StateSet *lightStateSet;
//
//	osg::Group* group = static_cast<osg::Group*>(m_viewer->getSceneData());
//	lightStateSet = group->getOrCreateStateSet();
//
//	Light *light = new Light();
//	light->setLightNum(1);
//	light->setPosition(Vec4(0, 0, 0, 1.0));  
//	light->setDiffuse(Vec4(1.0, 1.0, 1.0, 1.0));
//	light->setSpecular(Vec4(0.4, 0.4, 0.4, 1.0));
//	light->setAmbient(Vec4(0.5, 0.5, 0.5, 1.0));
//
//	/*Material *material = new Material();
//	material->setDiffuse(Material::FRONT, Vec4(1.0, 1.0, 1.0, 1.0));
//	material->setSpecular(Material::FRONT, Vec4(0.0, 0.0, 0.0, 1.0));
//	material->setAmbient(Material::FRONT, Vec4(0.1, 0.1, 0.1, 1.0));
//	material->setEmission(Material::FRONT, Vec4(0.2, 0.2, 0.2, 1.0));
//	material->setShininess(Material::FRONT, 25.0);*/
//
//	/*Geode *lightMarker = new Geode();
//	lightMarker->addDrawable(new ShapeDrawable(new Sphere(Vec3(), 1)));
//	lightMarker->getOrCreateStateSet()->setAttribute(material);*/
//
//	LightSource *lightSource = new LightSource();
//	lightSource->setLight(light);
//	lightSource->setLocalStateSetModes(StateAttribute::ON);
//	lightSource->setStateSetModes(*lightStateSet, StateAttribute::ON);
//
//	lightTransform = new PositionAttitudeTransform();
//	lightTransform->addChild(lightSource);
//	//lightTransform->addChild(lightMarker);
//	lightTransform->setPosition(Vec3(((bb.xMax() - bb.xMin()) / 2) + bb.xMin() + ((bb.xMax() - bb.xMin()) * .2), ((bb.yMax() - bb.yMin()) / 2) + bb.yMin() + ((bb.yMax() - bb.yMin()) * .2), bb.zMax() - ((bb.zMax() - bb.zMin()) * .3))); //((bb.zMax() - bb.zMin()) / 2) + bb.zMin()));
//	lightTransform->setScale(Vec3(1, 1, 1));
//
//	group->addChild(lightTransform);
//
//
//	//QLabel* qText = new QLabel("");
//
//	//qText->setStyleSheet("QLabel { background-color : blue; color : yellow; border : 1px solid  #ff0000; }");
//	//qText->setFont(QFont("times", 30));
//	//qText->resize(450, 450);
//
//
//	//m_viewer->getCamera()->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
//
//	// Force a home()
//    m_viewer->setCameraManipulator( m_viewer->getCameraManipulator() );
//
//	//m_ccreator_manager->applyInitSettings();
//
//	//group->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//	//group->getOrCreateStateSet()->setMode(GL_LIGHT0, osg::StateAttribute::OFF);
//	//group->getOrCreateStateSet()->setMode(GL_LIGHT1, osg::StateAttribute::OFF);
//}
//
//
//void VSimApp::SaveFile(const string& filename)
//{
//    if (Util::getExtension(filename) == "vsim")
//    {
//        std::ofstream ofs;
//        ofs.open(filename.c_str(), std::ios::binary);
//        if (!ofs.good())
//        {
//            MessageDialog md;
//            std::stringstream ss;
//            ss << "Error opening file " << filename << " for write.";
//            md.show(ss.str());
//            return;
//        }
//        insertExternalReferences();
//        osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
//        assert(rw);
//        m_narrative_manager->addNarrativesToNode(m_model);
//		m_eresources_manager->addEResourcesToNode(m_model);
//		m_ccreator_manager->SaveModelInformation(m_model);//add if no modelinformation node
//        ProgressDialog pd;
//        pd.show("Saving file. Please wait.");
//        osgDB::Options* options = new osgDB::Options(
//                //"WriteImageHint=WriteOut "
//                //"Compressor=zlib "
//                //"Ascii "
//                );
//        osgDB::ReaderWriter::WriteResult wresult = rw->writeNode(*m_model, ofs, options);
//        ofs.close();
//        pd.hide();
//		
//        if (!wresult.success())
//        {
//            MessageDialog md;
//            std::stringstream ss;
//            ss << "Error saving file: " << filename;
//            md.show(ss.str());
//        }
//        return;
//    }
//    else
//    {
//        insertExternalReferences();
//        ProgressDialog pd;
//        pd.show("Saving file. Please wait.");
//        bool result = osgDB::writeNodeFile(*m_model, filename);
//        pd.hide();
//        if (!result)
//        {
//            MessageDialog md;
//            std::stringstream ss;
//            ss << "Error saving file: " << filename;
//            md.show(ss.str());
//        }
//        return;
//    }
//}
//
