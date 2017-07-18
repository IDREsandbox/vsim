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

VSimApp::VSimApp(MainWindow* window)
	: m_window(window)
{
	m_viewer = window->getViewer();
	m_narrative_list = new NarrativeControl(this, m_window);
	m_narrative_player = new NarrativePlayer(this, m_window, m_narrative_list);

	connect(window, &MainWindow::sOpenFile, this, &VSimApp::openVSim);
	connect(window, &MainWindow::sSaveFile, this, &VSimApp::saveVSim);
	connect(window, &MainWindow::sImportModel, this, &VSimApp::importModel);
	connect(window, &MainWindow::sNew, this, &VSimApp::reset);

	connect(m_window->ui.actionOSG_Debug, &QAction::triggered, this, &VSimApp::OSGDebug);
	connect(m_window->ui.actionCamera_Debug, &QAction::triggered, this,
		[this]() {
		osg::Matrixd matrix = m_window->getViewer()->getCameraManipulator()->getMatrix();
		osg::Vec3 trans, scale;
		osg::Quat rot, so;
		matrix.decompose(trans, rot, scale, so);

		double y, p, r;
		Util::quatToYPR(rot, &y, &p, &r);
		qInfo() << "ypr" << y * 180 / M_PI << p * 180 / M_PI << r * 180 / M_PI;
	});

	reset();
}

bool VSimApp::init()
{
	initWithVSim(new osg::Group);
	return true;
}
bool VSimApp::initWithModel(osg::Node *model)
{
	init();
	m_model_group->addChild(model);
	return true;
}
bool VSimApp::initWithVSim(osg::Node *new_node)
{
	qDebug() << "init with VSim";
	// try and convert to new format
	osg::Group *root = new_node->asGroup();
	if (!root) return false;
	if (!convertToNewVSim(root)) {
		return false;
	}

	m_narrative_group = findOrCreateChildGroup(root, "Narratives");
	m_model_group = findOrCreateChildGroup(root, "Models");

	m_root = root;
	m_viewer->setSceneData(root); // ideall this would be only models, but its easy to mess things up
	m_narrative_list->load(m_narrative_group);
	
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

	m_model_group->addChild(loadedModel);
	return true;
}

bool VSimApp::openVSim(const std::string & filename)
{
	std::cout << "open vsim: " << filename.c_str() << "\n";
	osg::ref_ptr<osg::Node> loadedModel;
	//osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(filename);
	//osgDB::Options* options = new osgDB::Options;
	//options->setPluginStringData("fileType", "Ascii");

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
	}
	else { // osgb or osgt
		qDebug() << "loading osgt/osgb";
		loadedModel = osgDB::readNodeFile(filename);
		if (!loadedModel) {
			QMessageBox::warning(m_window, "Load Error", "Error opening file " + QString::fromStdString(filename));
			return false;
		}
	}

	bool init_success = initWithVSim(loadedModel);
	if (!init_success) {
		QMessageBox::warning(m_window, "Load Error", "Model init failed " + QString::fromStdString(filename));
		return false;
	}
	setFileName(filename);
	return true;
}

bool VSimApp::saveVSim(const std::string& filename)
{
	bool success = osgDB::writeNodeFile(*m_root, filename, new osgDB::Options("WriteImageHint=IncludeData"));
	if (!success) {
		QMessageBox::warning(m_window, "Save Error", "Error saving to file " + QString::fromStdString(filename));
		return false;
	}
	setFileName(filename);
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

std::string VSimApp::getFileName()
{
	return m_filename;
}

void VSimApp::setFileName(const std::string &str)
{
	m_filename = str;
	m_window->setWindowTitle("VSim - " + QString::fromStdString(str));
}

void VSimApp::OSGDebug()
{
	qInfo() << "root";
	for (uint i = 0; i < m_model_group->getNumChildren(); i++) {
		osg::Node *node = m_model_group->getChild(i);
		qInfo() << "-" << QString::fromStdString(node->getName()) << node->className();
	}

	qInfo() << "Narratives:";
	for (uint i = 0; i < m_narrative_group->getNumChildren(); i++) {
		Narrative2 *nar = dynamic_cast<Narrative2*>(m_narrative_group->getChild(i));
		if (!nar) continue;
		qInfo() << "Narrative" << i << QString::fromStdString(nar->getTitle());
		for (uint j = 0; j < nar->getNumChildren(); j++) {
			NarrativeSlide *slide = dynamic_cast<NarrativeSlide*>(nar->getChild(i));
			qInfo() << "\tSlide" << j << slide->getTransitionDuration();
		}
	}

	qInfo() << "Models:";
	for (uint i = 0; i < m_model_group->getNumChildren(); i++) {
		qInfo() << "Model" << QString::fromStdString(m_model_group->getChild(i)->getName());
	}
}

osg::Group *VSimApp::findOrCreateChildGroup(osg::Group *root, const std::string & name)
{
	uint num_children = root->getNumChildren();
	for (uint i = 0; i < num_children; i++) {
		osg::Group *child = root->getChild(i)->asGroup();

		if (!child) {
			// not a group, just skip it
			continue;
		}

		if (child->getName() == name) {
			//qInfo() << "Group" << QString::fromStdString(name) << "found";
			return child;
		}
	}
	//qInfo() << "Node" << QString::fromStdString(name) << "not found. Creating it.";
	osg::Group *new_group = new osg::Group();
	new_group->setName(name);
	root->addChild(new_group);

	return new_group;
}

bool VSimApp::convertToNewVSim(osg::Group *root)
{

	// find Narratives
	osg::Group *narrative_group = findOrCreateChildGroup(root, "Narratives");

	// find Models
	osg::Group *model_group = findOrCreateChildGroup(root, "Models");

	qDebug() << "converting to vsim" << narrative_group->getNumChildren();
	// find ModelInformation

	// find EResources
	// findOrCreateChildGroup(root, "EmbeddedResources");

	// Scan the root for different things, convert them, put them into the Narratives group
	for (uint i = 0; i < root->getNumChildren(); i++) {
		osg::Node *node = root->getChild(i);
		if (!node) {
			qInfo() << "child" << i << "is null ptr?";
		}
		std::string class_name = node->className();
		
		qDebug() << "scanning - found classname" << QString::fromStdString(class_name);
		// if we find a narrative in the root, then move it to the Narratives group
		if (class_name == "Narrative") {
			qDebug() << "Found a narrative";
			Narrative2 *nar = dynamic_cast<Narrative2*>(node);
			if (!nar) return false; // this probably makes no sense

			narrative_group->addChild(nar);
			root->removeChild(nar);
		}
		// otherwise just leave things where they are
		else {
			//root->removeChild(node);
			//model_group->addChild(node);
		}
		
	}

	return true;
}

bool VSimApp::mergeAnotherVSim(osg::Group *other)
{
	osg::Group *other_narrative_group = findOrCreateChildGroup(other, "Narratives");
	// basically just copy over the Narratives and Models contents
	qDebug() << "merging";
	qDebug() << "current narratives:" << m_narrative_group->getNumChildren();
	qDebug() << "other narratives:" << other_narrative_group->getNumChildren();

	for (uint i = 0; i < other_narrative_group->getNumChildren(); i++) {
		m_narrative_group->addChild(other_narrative_group->getChild(i));
	}
	osg::Group *other_model_group = findOrCreateChildGroup(other, "Models");
	for (uint i = 0; i < other_model_group->getNumChildren(); i++) {
		m_model_group->addChild(other_model_group->getChild(i));
	}

	// what are we supposed to do with all of the other junk?
	return true;
}

//		m_eresources_manager->addEResourcesToNode(m_model);
//		m_ccreator_manager->SaveModelInformation(m_model);//add if no modelinformation node
