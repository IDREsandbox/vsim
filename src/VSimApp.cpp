#include "VSimApp.h"

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

#include "VSimApp.h"
#include "Util.h"

#define OPTIMIZE 0

VSimApp* g_vsimapp;

VSimApp::VSimApp(MainWindow* window)
	: m_window(window)
{
	m_viewer = window->getViewer();
	m_model = m_viewer->getSceneData();
}

void VSimApp::reset()
{
	m_model = new osg::Group();
	m_viewer->setSceneData(m_model.get());
}

bool VSimApp::importModel(const std::string& filename)
{
	osg::ref_ptr<osg::Node> loadedModel(NULL);

	// otherwise
	loadedModel = osgDB::readNodeFile(filename);
	if (!loadedModel.get()) {
		printf("error loading file %s\n", filename.c_str());
		QMessageBox::warning(m_window, "Import Error", "Error loading file " + QString::fromStdString(filename));
		return false;
	}

	m_model->asGroup()->addChild(loadedModel);
	return true;
}

bool VSimApp::openVSim(const std::string & filename)
{
	reset();
	osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename);

	// if .vsim, use osgb, TODO: our own readerwriter?
	std::string ext = Util::getExtension(filename);
	if (ext == ".vsim") {
		std::ifstream ifs;
		ifs.open(filename.c_str(), std::ios::binary);
		if (!ifs.good()) {
			QMessageBox::warning(m_window, "Save Error", "Error saving to file " + QString::fromStdString(filename));
			return false;
		}

		osgDB::ReaderWriter *rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
		if (!rw) {
			QMessageBox::warning(m_window, "Save Error", "Error creating osgb writer " + QString::fromStdString(filename));
			return false;
		}
		osgDB::ReaderWriter::ReadResult result = rw->readNode(ifs);
		if (result.success()) {
			loadedModel = result.takeNode();
		}
	}
	else {
		loadedModel = osgDB::readNodeFile(filename);
	}
	m_viewer->setSceneData(loadedModel.get());
}

bool VSimApp::saveVSim(const std::string& filename)
{

	bool success = osgDB::writeNodeFile(*m_model, filename);
	if (!success) {
		QMessageBox::warning(m_window, "Save Error", "Error saving to file " + QString::fromStdString(filename));
	}

	return success;
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

	//osgDB::ReaderWriter* rw = osgDB::Registry::instance()->getReaderWriterForExtension("osg");
	//if (!rw) {
	//	QMessageBox::warning(m_window, "Save Error", "Error creating osgb writer " + QString::fromStdString(filename));
	//	return false;
	//}

	//// TODO: check wresult
	//osgDB::ReaderWriter::WriteResult wresult = rw->writeNode(*m_model, ofs, nullptr);
	//ofs.close();

	//return true;
}

//		m_eresources_manager->addEResourcesToNode(m_model);
//		m_ccreator_manager->SaveModelInformation(m_model);//add if no modelinformation node
