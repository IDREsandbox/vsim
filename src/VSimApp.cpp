#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers> // stats handler
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/StateSetManipulator>

#include <osg/io_utils>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <QObject>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QAction>
#include <QFileDialog>

#include "VSimApp.h"
#include "Util.h"
#include "deprecated/narrative/Narrative.h"
#include "narrative/NarrativeGroup.h"
#include "labelCanvasView.h"
#include "OSGViewerWidget.h"
#include "MainWindow.h"
#include "ModelOutliner.h"
#include "TimeSlider.h"
#include "narrative/NarrativeControl.h"
#include "narrative/NarrativePlayer.h"
#include "resources/ERControl.h"

VSimApp::VSimApp(MainWindow* window)
	: m_window(window),
	m_filename(""),
	m_root(new VSimRoot),
	m_model_table_model(m_root->models())
{
	m_viewer = window->getViewer();

	// timers
	m_timer = new QTimer;
	m_timer->setInterval(15);
	m_timer->setSingleShot(false);
	m_dt_timer = new QElapsedTimer;
	m_timer->start();
	m_dt_timer->start();
	connect(m_timer, &QTimer::timeout, this, &VSimApp::updateTime);

	m_narrative_control = new NarrativeControl(this, m_window);
	m_er_control = new ERControl(this, m_window, m_root->resources(), m_root->categories());

	// Narrative player
	m_narrative_player = new NarrativePlayer(this, m_narrative_control);

	connect(window->topBar()->ui.play_2, &QPushButton::pressed, m_narrative_player, &NarrativePlayer::play);
	connect(window->topBar()->ui.pause_2, &QPushButton::pressed, m_narrative_player, &NarrativePlayer::stop);
	connect(m_narrative_player, &NarrativePlayer::updateCamera, m_window->getViewerWidget(), &OSGViewerWidget::setCameraMatrix);
	connect(this, &VSimApp::tick, m_narrative_player, &NarrativePlayer::update);
	//connect(this, &VSimApp::foo, window->getViewerWidget(), static_cast<void(OSGViewerWidget::*)()>(&OSGViewerWidget::update));
	connect(this, &VSimApp::tick, window->getViewerWidget(), static_cast<void(OSGViewerWidget::*)()>(&OSGViewerWidget::update));
	connect(m_narrative_player, &NarrativePlayer::enableNavigation, window->getViewerWidget(), &OSGViewerWidget::enableNavigation);
	connect(m_narrative_player, &NarrativePlayer::hideCanvas, window->canvasView(), &labelCanvasView::hide);
	connect(m_narrative_player, &NarrativePlayer::showCanvas, window->canvasView(), &labelCanvasView::fadeIn);

	// This is a really awkward place... but this has to be done after setting model
	m_window->outliner()->setModel(&m_model_table_model);
	m_window->outliner()->header()->resizeSection(0, 200);
	m_window->outliner()->resize(505, 600);
	qDebug() << m_window->outliner()->windowFlags();

	connect(window, &MainWindow::sOpenFile, this, &VSimApp::openVSim);
	connect(window, &MainWindow::sSaveFile, this, &VSimApp::saveVSim);
	connect(window, &MainWindow::sImportModel, this, &VSimApp::importModel);
	connect(window, &MainWindow::sNew, this, &VSimApp::init);
	connect(window, &MainWindow::sSaveCurrent, this, &VSimApp::saveCurrentVSim);
	connect(window, &MainWindow::sImportNarratives, this, &VSimApp::importNarratives);
	connect(window, &MainWindow::sExportNarratives, this, &VSimApp::exportNarratives);

	connect(window, &MainWindow::sDebugOSG, this, [this]() {m_root->debug(); });
	connect(window, &MainWindow::sDebugCamera, this, &VSimApp::debugCamera);

	initWithVSim(m_root);
}

bool VSimApp::init()
{
	setFileName("");
	initWithVSim(new VSimRoot);
	return true;
}

osgViewer::Viewer * VSimApp::getViewer()
{
	return m_viewer;
}


bool VSimApp::initWithVSim(osg::Node *new_node)
{
	VSimRoot *root = dynamic_cast<VSimRoot*>(new_node);
	if (root == nullptr) {
		// create a new VSimRoot, convert from old format
		root = new VSimRoot(new_node->asGroup());
	}
	else {
		qDebug() << "Root is a VSimRoot";
	}
	
	// move all of the gui stuff over to the new root
	m_viewer->setSceneData(root->models()); // ideally this would be only models, but its easy to mess things up
	m_model_table_model.setGroup(root->models());
	m_narrative_control->load(root->narratives());
	m_window->timeSlider()->setGroup(root->models());
	
	m_window->m_undo_stack->clear();
	m_window->m_osg_widget->reset();

	// dereference the old root, apply the new one
	m_root = root;
	
	return true;
}


void VSimApp::addModel(osg::Node *node, const std::string &name)
{
	m_root->models()->addChild(node);
	node->setName(name);
	m_window->m_osg_widget->reset();
}

bool VSimApp::importModel(const std::string& filename)
{
	osg::ref_ptr<osg::Node> loadedModel(NULL);
	// TODO: special import for vsim files

	// otherwise
	loadedModel = osgDB::readNodeFile(filename);
	if (!loadedModel.get()) {
		qWarning() << "Error importing" << filename.c_str();
		QMessageBox::warning(m_window, "Import Error", "Error loading file " + QString::fromStdString(filename));
		return false;
	}
	addModel(loadedModel, Util::getFilename(filename));
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
		if (!loadedModel) {
			QMessageBox::warning(m_window, "Load Error", "Failed to load model " + QString::fromStdString(filename));
			return false;
		}
		init();
		addModel(loadedModel, Util::getFilename(filename));
		init_success = true;
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

bool VSimApp::exportNarratives()
{
	// figure out the selected narratives, if none are selected then error
	osg::ref_ptr<osg::Group> export_group = new NarrativeGroup;
	NarrativeControl::SelectionLevel level = m_narrative_control->getSelectionLevel();
	std::set<int> selection;
	if (level == NarrativeControl::NARRATIVES) {
		// export the entire selection
		selection = m_window->topBar()->ui.narratives->getSelection();
		if (selection.empty()) {
			QMessageBox::warning(m_window, "Export Narratives Error", "No narratives selected");
			return false;
		}
	}
	else {
		int nar = m_narrative_control->getCurrentNarrativeIndex();
		if (nar == -1) return false;
		selection.insert(nar);
	}
	// add all children to the export group
	for (auto index : selection) {
		export_group->addChild(m_root->narratives()->getChild(index));
	}

	// Open up the save dialog
	qDebug() << "Export narratives";
	QString filename = QFileDialog::getSaveFileName(m_window, "Export Narratives", "", "Narrative file (*.nar)");
	if (filename == "") {
		qDebug() << "Export narratives cancel";
		return false;
	}

	// Open the file, create the osg reader/writer
	qDebug() << "Exporting narratives" << filename;
	std::ofstream ofs;
	ofs.open(filename.toStdString(), std::ios::binary);
	if (!ofs.good()) {
		QMessageBox::warning(m_window, "Export Error", "Error opening file for writing " + filename);
		return false;
	}

	osgDB::ReaderWriter *rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
	if (!rw) {
		QMessageBox::warning(m_window, "Export Error", "Error creating osgb writer " + filename);
		return false;
	}

	qDebug() << "Exporting narratives" << Util::setToString(selection);
	osgDB::Options* options = new osgDB::Options;
	//options->setPluginStringData("fileType", "Ascii");
	options->setPluginStringData("WriteImageHint", "IncludeData");
	osgDB::ReaderWriter::WriteResult result = rw->writeNode(*export_group, ofs, options);

	if (result.success()) {
		return true;
	}
	else {
		QMessageBox::warning(m_window, "Save Error", "Error writing osg nodes " + filename);
		return false;
	}
}

bool VSimApp::importNarratives()
{
	// Open dialog
	qDebug("Importing narratives");
	QString filename = QFileDialog::getOpenFileName(m_window, "Import Narratives", "", "Narrative files (*.nar);;All types (*.*)");
	if (filename == "") {
		qDebug() << "import cancel";
		return false;
	}

	osg::ref_ptr<osg::Node> loadedModel;

	// open file
	std::ifstream ifs;
	ifs.open(filename.toStdString(), std::ios::binary);
	if (!ifs.good()) {
		QMessageBox::warning(m_window, "Import Narratives Error", "Error opening file " + filename);
		return false;
	}
	// create reader/writer
	osgDB::ReaderWriter *rw = osgDB::Registry::instance()->getReaderWriterForExtension("osgb");
	if (!rw) {
		QMessageBox::warning(m_window, "Import Narratives Error", "Error creating osgb writer " + filename);
		return false;
	}

	// perform read
	osgDB::ReaderWriter::ReadResult result = rw->readNode(ifs);
	if (result.success()) {
		loadedModel = result.takeNode();
	}
	else {
		QMessageBox::warning(m_window, "Import Narratives Error", "Error reading nodes " + filename);
		return false;
	}

	// check if a NarrativeGroup
	NarrativeGroup *group = nullptr;
	NarrativeGroup *cast = dynamic_cast<NarrativeGroup*>(loadedModel.get());
	osg::Group *cast_old = dynamic_cast<osg::Group*>(loadedModel.get());
	Narrative *nar = dynamic_cast<Narrative*>(loadedModel.get());

	if (cast) {
		group = cast;
	}
	else if (cast_old) {
		group = new NarrativeGroup(cast_old);
	}
	else if (nar) {
		qDebug() << "file is an old narrative";
		group = new NarrativeGroup;
		group->addChild(new Narrative2(nar));
	}
	else {
		qWarning() << "Error importing narratives - root node is not a NarrativeGroup or osg::Group";
		QMessageBox::warning(m_window, "Import Error", "Error loading file " + filename);
		return false;
	}

	m_narrative_control->loadNarratives(group);
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

	std::cout << "zero everything\n";
	auto q = Util::YPRToQuat(0, 0, 0);
	std::cout << osg::Matrixd::rotate(q);

	osg::Matrix base(
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, -1, 0, 0,
		0, 0, 0, 1);
	double yaw, pitch, roll;
	Util::quatToYPR(base.getRotate(), &yaw, &pitch, &roll);
	qDebug() << yaw * M_PI/180 << pitch * M_PI / 180 << roll * M_PI / 180;

}

void VSimApp::updateTime()
{
	double dt = m_dt_timer->nsecsElapsed() / 1.0e9;
	m_dt_timer->restart();
	emit tick(dt);
}
