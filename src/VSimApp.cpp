#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers> // stats handler
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/StateSetManipulator>

#include <osg/io_utils>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QAction>
#include "VSimApp.h"
#include "Util.h"
#include "deprecated/narrative/Narrative.h"
#include "narrative/NarrativeGroup.h"

#include <QObject>

#define OPTIMIZE 0


VSimApp::VSimApp(MainWindow* window)
	: m_window(window)
{
	m_viewer = window->getViewer();
	m_narrative_control = new NarrativeControl(this, m_window);
	m_narrative_player = new NarrativePlayer(this, m_window, m_narrative_control);

	connect(window, &MainWindow::sOpenFile, this, &VSimApp::openVSim);
	connect(window, &MainWindow::sSaveFile, this, &VSimApp::saveVSim);
	connect(window, &MainWindow::sImportModel, this, &VSimApp::importModel);
	connect(window, &MainWindow::sNew, this, &VSimApp::reset);
	connect(window, &MainWindow::sSaveCurrent, this, &VSimApp::saveCurrentVSim);

	connect(m_window->ui.actionOSG_Debug, &QAction::triggered, this, [this]() {m_root->debug(); });
	connect(m_window->ui.actionCamera_Debug, &QAction::triggered, this, &VSimApp::debugCamera);

	reset();
}

bool VSimApp::init()
{
	setFileName("");
	initWithVSim(new VSimRoot);
	return true;
}
bool VSimApp::initWithModel(osg::Node *model)
{
	init();
	m_root->models()->addChild(model);
	m_window->m_osg_widget->reset();
	return true;
}
bool VSimApp::initWithVSim(osg::Node *new_node)
{
	VSimRoot *root = dynamic_cast<VSimRoot*>(new_node);
	if (root == nullptr) {
		// create a new VSimRoot, convert from old format
		root = new VSimRoot(new_node->asGroup());
	}
	else {
		qDebug() << "is in fact a vsimroot";
	}
	m_root = root;
	m_viewer->setSceneData(m_root->models()); // ideally this would be only models, but its easy to mess things up

	m_narrative_control->load(m_root->narratives());

	m_window->m_undo_stack->clear();
	m_window->m_osg_widget->reset();
	//m_viewer->getCamera()->setProjectionMatrixAsPerspective(75.0f, )
	
	return true;
}
void VSimApp::reset()
{
	init();
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
	m_root->models()->addChild(loadedModel);
	m_window->m_osg_widget->reset();
	return true;
}

bool VSimApp::openVSim(const std::string & filename)
{
	std::cout << "open vsim: " << filename.c_str() << "\n";
	osg::ref_ptr<osg::Node> loadedModel;
	//osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename);
	//osgDB::Options* options = new osgDB::Options;
	//options->setPluginStringData("fileType", "Ascii");

	bool init_success = false;

	// if .vsim, use osgb, TODO: our own readerwriter?
	std::string ext = Util::getExtension(filename);
	if (ext == "vsim") {
		qDebug() << "loading vsim";
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
		init_success = initWithVSim(loadedModel);
	}
	else if (ext == "osgb" || ext == "osgt") { // osgb or osgt
		qDebug() << "loading osgt/osgb";
		loadedModel = osgDB::readNodeFile(filename);
		if (!loadedModel) {
			QMessageBox::warning(m_window, "Load Error", "Error opening file " + QString::fromStdString(filename));
			return false;
		}
		init_success = initWithVSim(loadedModel);
	}
	else {
		qDebug() << "loading a non osg model";
		loadedModel = osgDB::readNodeFile(filename);
		init_success = initWithModel(loadedModel);
	}

	if (!init_success) {
		QMessageBox::warning(m_window, "Load Error", "Model init failed " + QString::fromStdString(filename));
		return false;
	}
	setFileName(filename);
	return true;
}

bool VSimApp::saveVSim(const std::string& filename)
{
	// if vsim, then use osgb
	std::string ext = Util::getExtension(filename);
	if (ext == "vsim") {
		qDebug() << "saving vsim";
		std::ofstream ofs;
		ofs.open(filename.c_str(), std::ios::binary);
		if (!ofs.good()) {
			QMessageBox::warning(m_window, "Save Error", "Error opening file for writing " + QString::fromStdString(filename));
			return false;
		}
		osgDB::ReaderWriter *rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
		if (!rw) {
			QMessageBox::warning(m_window, "Save Error", "Error creating osgb writer " + QString::fromStdString(filename));
			return false;
		}
		//// this is how you do ascii (from robertosfield on github)
		//osgDB::Options* options = new osgDB::Options;
		//options->setPluginStringData("fileType", "Ascii");
		osgDB::ReaderWriter::WriteResult result = rw->writeNode(*m_root, ofs, new osgDB::Options("WriteImageHint=IncludeData"));

		if (result.success()) {
		}
		else {
			QMessageBox::warning(m_window, "Save Error", "Error writing osg nodes " + QString::fromStdString(filename));
			return false;
		}
	}
	// otherwise use osgb/osgt, let osg decide
	else {
		bool success = osgDB::writeNodeFile(*m_root, filename, new osgDB::Options("WriteImageHint=IncludeData"));
		if (!success) {
			QMessageBox::warning(m_window, "Save Error", "Error saving to file " + QString::fromStdString(filename));
			return false;
		}
	}
	setFileName(filename);
	return true;
}

bool VSimApp::saveCurrentVSim()
{
	if (m_filename == "") {
		m_window->actionSaveAs();
		return true;
	}
	std::string ext = Util::getExtension(m_filename);
	if (ext != ".vsim") {
		m_window->actionSaveAs();
		return true;
	}
	saveVSim(m_filename);
	return true;
}

std::string VSimApp::getFileName()
{
	return m_filename;
}

void VSimApp::setFileName(const std::string &str)
{
	m_filename = str;
	m_window->setWindowTitle("VSim - " + QString::fromStdString(str));
}

void VSimApp::debugCamera()
{
	osg::Matrixd matrix = m_window->getViewer()->getCameraManipulator()->getMatrix();
	osg::Vec3 trans, scale;
	osg::Quat rot, so;
	matrix.decompose(trans, rot, scale, so);

	double y, p, r;
	Util::quatToYPR(rot, &y, &p, &r);
	std::cout << "matrix " << matrix << "\ntranslation " << trans << "\nscale " << scale << "\nrotation " << rot << "\n";
	qInfo() << "ypr" << y * 180 / M_PI << p * 180 / M_PI << r * 180 / M_PI;
}
