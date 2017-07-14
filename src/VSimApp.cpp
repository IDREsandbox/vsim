#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers> // stats handler
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/StateSetManipulator>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QAction>
#include "VSimApp.h"
#include "Util.h"

#include <QObject>

#define OPTIMIZE 0

VSimApp* g_vsimapp;

VSimApp::VSimApp(MainWindow* window)
	: m_window(window)
{
	m_viewer = window->getViewer();
	m_narrative_list = new NarrativeControl(this, m_window);
	m_narrative_player = new NarrativePlayer(this, m_window, m_narrative_list);

	QObject::connect(window, &MainWindow::sOpenFile, this, &VSimApp::openVSim);
	QObject::connect(window, &MainWindow::sSaveFile, this, &VSimApp::saveVSim);
	QObject::connect(window, &MainWindow::sImportModel, this, &VSimApp::importModel);
	QObject::connect(window, &MainWindow::sNew, this, &VSimApp::reset);

	reset();
}

bool VSimApp::init(osg::Node *model)
{
	if (model == nullptr) {
		qWarning() << "attempting to init vsim with a null model";
		return false;
	}
	osg::Group *group = model->asGroup();
	if (group == nullptr) {
		qWarning() << "attempting to init vsim but model is not a group";
		return false;
	}

	m_model = group;
	m_viewer->setSceneData(m_model);
	m_narrative_list->load(m_model);

	return true;
}
void VSimApp::reset()
{
	init(new osg::Group);
}

bool VSimApp::importModel(const std::string& filename)
{
	osg::ref_ptr<osg::Node> loadedModel(NULL);

	// otherwise
	loadedModel = osgDB::readNodeFile(filename);
	if (!loadedModel.get()) {
		qWarning() << "error loading file %s\n" << filename.c_str();
		QMessageBox::warning(m_window, "Import Error", "Error loading file " + QString::fromStdString(filename));
		return false;
	}

	m_model->addChild(loadedModel);
	return true;
}

bool VSimApp::openVSim(const std::string & filename)
{
	std::cout << "open vsim: " << filename.c_str() << "\n";
	osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename);

	// if .vsim, use osgb, TODO: our own readerwriter?
	std::string ext = Util::getExtension(filename);
	if (ext == "vsim") {
		std::ifstream ifs;
		ifs.open(filename.c_str(), std::ios::binary);
		if (!ifs.good()) {
			QMessageBox::warning(m_window, "Load Error", "Error opening file " + QString::fromStdString(filename));
			return false;
		}

		osgDB::ReaderWriter *rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
		if (!rw) {
			QMessageBox::warning(m_window, "Load Error", "Error creating osgb reader " + QString::fromStdString(filename));
			return false;
		}
		osgDB::ReaderWriter::ReadResult result = rw->readNode(ifs);
		if (result.success()) {
			loadedModel = result.takeNode();
		}
		else {
			QMessageBox::warning(m_window, "Load Error", "Error converting file to osg nodes " + QString::fromStdString(filename));
			return false;
		}
	}
	else {
		loadedModel = osgDB::readNodeFile(filename);
		if (!loadedModel) {
			QMessageBox::warning(m_window, "Load Error", "Error opening file " + QString::fromStdString(filename));
			return false;
		}
	}

	if (!init(loadedModel)) {
		QMessageBox::warning(m_window, "Load Error", "Model init failed " + QString::fromStdString(filename));
		return false;
	}
	return true;
}

bool VSimApp::saveVSim(const std::string& filename)
{
	bool success = osgDB::writeNodeFile(*m_model, filename, new osgDB::Options("WriteImageHint=IncludeData"));
	if (!success) {
		QMessageBox::warning(m_window, "Save Error", "Error saving to file " + QString::fromStdString(filename));
		return false;
	}
	return success;

	//new osgDB::Options("WriteImageHint=IncludeData Compressor=zlib")

	//std::ofstream ofs;
	//ofs.open(filename.c_str(), std::ios::binary);
	//if (!ofs.good()) {
	//	QMessageBox::warning(m_window, "Save Error", "Error saving to file " + QString::fromStdString(filename));
	//	return false;
	//}

	//// TODO: binary, compression, etc... (this also leaks)
	////	osgDB::Options* options = new osgDB::Options(
	////		//"WriteImageHint=WriteOut "
	////		//"Compressor=zlib "
	////		//"Ascii "
	////		);

	//// this is how you do ascii (from robertosfield on github)
	//osgDB::Options* options = new osgDB::Options;
	//options->setPluginStringData("fileType", "Ascii");

	// for image data WriteImageHint=IncludeData or WriteOut

	//osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgt");
	//if (!rw) {
	//	QMessageBox::warning(m_window, "Save Error", "Error creating osgb writer " + QString::fromStdString(filename));
	//	return false;
	//}

	//// TODO: check wresult
	//osgDB::ReaderWriter::WriteResult wresult = rw->writeNode(*m_model, ofs, options);
	//ofs.close();

	//return true;
}



//		m_eresources_manager->addEResourcesToNode(m_model);
//		m_ccreator_manager->SaveModelInformation(m_model);//add if no modelinformation node
