#include <osgViewer/Viewer>
#include <iostream>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDir>

#include "MainWindow.h"

#include "narrative/NarrativeInfoDialog.h"
#include "resources/ERDialog.h"
#include "dragLabelInput.h"
#include "OSGViewerWidget.h"
#include "labelCanvas.h"
#include "labelCanvasView.h"
#include "TimeSlider.h"
#include "ModelOutliner.h"

MainWindow::MainWindow(QWidget *parent) 
	: QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	// initialize the Qt Designer stuff
	ui->setupUi(this);

	// window stuff
	setMinimumSize(1280, 720);
	ui->statusbar->showMessage("the best status bar", 0);
	setWindowTitle("VSim");
	setAcceptDrops(true);
	qDebug() << "root: " << QDir::currentPath();

	// undo stack
	m_undo_stack = new QUndoStack(this);
	m_undo_stack->setUndoLimit(50);
	QAction *undo_action = m_undo_stack->createUndoAction(this, tr("&Undo"));
	QAction *redo_action = m_undo_stack->createRedoAction(this, tr("&Redo"));
	undo_action->setShortcuts(QKeySequence::Undo);
	redo_action->setShortcuts(QKeySequence::Redo);
	ui->menuEdit->addAction(undo_action);
	ui->menuEdit->addAction(redo_action);

	// osg viewer widget
	m_osg_widget = new OSGViewerWidget(ui->root);
	m_osg_widget->lower(); // move this to the back
	ui->rootLayout->addWidget(m_osg_widget, 0, 0);

	// set viewer widget as parent of gui stuff, so signals can get through
	ui->mainSplitter->setParent(m_osg_widget);
	QGridLayout *dummylayout = new QGridLayout(m_osg_widget);
	m_osg_widget->setLayout(dummylayout);
	dummylayout->addWidget(ui->mainSplitter);
	dummylayout->setContentsMargins(QMargins()); // zero margins
	ui->mainSplitter->setMouseTracking(true);

	// drag widget
	m_drag_area = new labelCanvas();
	m_drag_area->setGeometry(0, 0, this->size().width(), this->size().height());
	m_drag_area->setMinimumSize(800, 600);

	m_view = new labelCanvasView(ui->root, m_drag_area);
	ui->rootLayout->addWidget(m_view, 0, 0);

	// embedded resources
	m_resource_group = new EResourceGroup();
	ui->local->setGroup(m_resource_group);

	m_display = new ERDisplay(this);
	m_display->setGeometry(10, 200, 265, 251);
	m_display->hide();

	// vsimapp file stuff
	connect(ui->actionNew, &QAction::triggered, this, &MainWindow::actionNew);
	connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::actionOpen);
	connect(ui->actionSave, &QAction::triggered, this, &MainWindow::actionSave);
	connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::actionSaveAs);
	connect(ui->actionImport_Model, &QAction::triggered, this, &MainWindow::actionImportModel);
	connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
	connect(ui->actionImport_Narratives, &QAction::triggered, this, &MainWindow::sImportNarratives);
	connect(ui->actionExport_Narratives, &QAction::triggered, this, &MainWindow::sExportNarratives);

	connect(ui->actionError, &QAction::triggered, this, [this] { this->ErrorDialog("heyo this is quite the error message"); });
	connect(ui->actionMessage, &QAction::triggered, this, [this] { this->MessageDialog("A beautiful message"); });
	connect(ui->actionProgress, &QAction::triggered, this, [this] { this->LoadingDialog("zzz"); });

	connect(ui->actionOSG_Debug, &QAction::triggered, this, &MainWindow::sDebugOSG);
	connect(ui->actionCamera_Debug, &QAction::triggered, this, &MainWindow::sDebugCamera);
	connect(ui->actionControl_Debug, &QAction::triggered, this, &MainWindow::sDebugControl);

	// camera manipulator
	connect(ui->actionFirst_Person_Navigation, &QAction::triggered, m_osg_widget,
		[this]() {qDebug() << "fp trigger"; m_osg_widget->setNavigationMode(OSGViewerWidget::NAVIGATION_FIRST_PERSON); });
	connect(ui->actionFlight_Navigation, &QAction::triggered, m_osg_widget,
		[this]() {qDebug() << "flight trigger";  m_osg_widget->setNavigationMode(OSGViewerWidget::NAVIGATION_FLIGHT); });
	connect(ui->actionObject_Navigation, &QAction::triggered, m_osg_widget,
		[this]() {qDebug() << "object trigger";  m_osg_widget->setNavigationMode(OSGViewerWidget::NAVIGATION_OBJECT); });
	connect(ui->actionFreeze_Camera, &QAction::toggled, m_osg_widget,
		[this](bool freeze) {qDebug() << "freeze trigger";  m_osg_widget->setCameraFrozen(freeze); });
	connect(ui->actionReset_Camera, &QAction::triggered, m_osg_widget, &OSGViewerWidget::reset);

	// embedded resources
	// new
	connect(ui->local, &ERScrollBox::sNew, this, &MainWindow::newER);
	connect(ui->plus_2, &QPushButton::clicked, this, &MainWindow::newER);
	// delete
	connect(ui->local, &ERScrollBox::sDelete, this, &MainWindow::deleteER);
	connect(ui->minus_2, &QPushButton::clicked, this, &MainWindow::deleteER);
	// edit
	connect(ui->local, &ERScrollBox::sEdit, this, &MainWindow::editERInfo);
	connect(ui->edit, &QPushButton::clicked, this, &MainWindow::editERInfo);
	// open
	connect(ui->local, &ERScrollBox::sOpen, this, &MainWindow::openResource);

	// show slides or narratives
	connect(ui->topBar->ui.open, &QPushButton::clicked, this, 
		[this]() {
			qDebug() << "open";
			this->ui->topBar->showSlides();
		});
	connect(ui->topBar->ui.left_2, &QPushButton::clicked, this,
		[this]() {
			qDebug() << "goback";
			this->ui->topBar->showNarratives();
		});

	// model outliner
	m_outliner = new ModelOutliner(this);
	connect(ui->actionModel_Outliner, &QAction::triggered, this, [this]() {
		m_outliner->show();
		m_outliner->setFocus();
	});
	m_time_slider = new TimeSlider(this);
	connect(ui->actionTime_Slider, &QAction::triggered, this, [this]() {
		m_time_slider->show();
		m_time_slider->setFocus();
	});
}

EResource* MainWindow::getResource(int index)
{
	if (index < 0 || (uint)index >= m_resource_group->getNumChildren()) {
		return nullptr;
	}
	osg::Node *c = m_resource_group->getChild(index);
	return dynamic_cast<EResource*>(c);
}

void MainWindow::newER()
{
	ERDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return;
	}

	// for now just add to the end
	m_undo_stack->beginMacro("New Resource");
	int num_children = m_resource_group->getNumChildren();
	m_undo_stack->push(new SelectResourcesCommand(this, { num_children - 1 }, ON_UNDO));
	m_undo_stack->push(new EResourceGroup::NewEResourceCommand(m_resource_group, m_resource_group->getNumChildren()));
	m_undo_stack->push(new SelectResourcesCommand(this, { num_children }, ON_REDO));
	m_undo_stack->endMacro();

	EResource *resource = getResource(m_resource_group->getNumChildren() - 1);
	resource->setIndex(m_resource_group->getNumChildren() - 1);
	resource->setResourceName(dlg.getTitle());
	resource->setAuthor(dlg.getAuthor());
	resource->setResourceDescription(dlg.getDescription());
	//resource->set

}

void MainWindow::deleteER()
{
	std::set<int> selection = ui.local->getSelection();
	if (selection.empty()) return;
	int next_selection = nextSelectionAfterDelete(m_resource_group->getNumChildren(), selection);

	// get pointers to nodes to delete
	m_undo_stack->beginMacro("Delete Resources");
	m_undo_stack->push(new SelectResourcesCommand(this, selection, ON_UNDO));
	// delete in reverse order
	for (auto i = selection.rbegin(); i != selection.rend(); ++i) {
		qDebug() << "pushing delete resource" << *i;
		m_undo_stack->push(new EResourceGroup::DeleteEResourceCommand(m_resource_group, *i));
	}
	m_undo_stack->push(new SelectResourcesCommand(this, { next_selection }, ON_REDO));
	m_undo_stack->endMacro();
}

void MainWindow::editERInfo()
{
	int active_item = ui.local->getLastSelected();
	qDebug() << "resource list - begin edit on" << active_item;
	if (active_item < 0) {
		qWarning() << "resource list - can't edit with no selection";
		return;
	}

	EResource *resource = getResource(active_item);

	ERDialog dlg(resource);
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		qDebug() << "resource list - cancelled edit on" << active_item;
		return;
	}

	m_undo_stack->beginMacro("Set Resource Info");
	m_undo_stack->push(new SelectResourcesCommand(this, { active_item }));
	m_undo_stack->push(new EResource::SetResourceNameCommand(resource, dlg.getTitle()));
	m_undo_stack->push(new EResource::SetResourceAuthorCommand(resource, dlg.getAuthor()));
	m_undo_stack->push(new EResource::SetResourceDescriptionCommand(resource, dlg.getDescription()));
	m_undo_stack->endMacro();
}

void MainWindow::openResource()
{
	int index = ui.local->getLastSelected();
	if (index < 0) return;

	EResource *res = getResource(index);
	m_display->show();

	//m_display->ui.title->setText(QString::fromStdString(res->getResourceName()));
	m_display->setInfo(res);
}

int MainWindow::nextSelectionAfterDelete(int total, std::set<int> selection)
{
	// figure out the selection after deleting
	int first_index = *selection.begin();
	int remaining = total - selection.size();
	int next_selection;
	if (remaining == 0) { // everyone's gone
		next_selection = -1;
	}
	else if (remaining >= first_index + 1) {
		next_selection = first_index; // select next non-deleted item
	}
	else {
		next_selection = first_index - 1; // select the previous item
	}
	return next_selection;
}

void MainWindow::selectResources(std::set<int> res)
{
	ui.local->setSelection(res);
}

SelectResourcesCommand::SelectResourcesCommand(MainWindow *control, std::set<int> resources, SelectionCommandWhen when, QUndoCommand *parent)
	: QUndoCommand(parent),
	m_control(control),
	m_resources(resources),
	m_when(when)
{
}
void SelectResourcesCommand::undo() {
	if (m_when != ON_REDO) {
		m_control->selectResources(m_resources);
	}
}
void SelectResourcesCommand::redo() {
	if (m_when != ON_UNDO) {
		m_control->selectResources(m_resources);
	}
}

void MainWindow::ErrorDialog(const std::string & msg)
{
	QErrorMessage* dialog = new QErrorMessage(this);
	dialog->showMessage(msg.c_str());
}

void MainWindow::MessageDialog(const std::string & msg)
{
	QMessageBox::warning(this, "super warning", msg.c_str());
}

void MainWindow::LoadingDialog(const std::string & msg)
{
	//auto pd = new QProgressDialog("Loading model.", "Cancel", 0, 100, this);
	//
	printf("foostart!\n");
	auto pd = new QProgressDialog("Loading model.", "Cancel", 0, 100, this);
	pd->setValue(40);
	pd->setWindowModality(Qt::WindowModal);
	pd->setMinimumDuration(0);
	pd->show();
	connect(pd, &QProgressDialog::canceled, this, [] {printf("CLOSEDD!!!\n"); });
}


OSGViewerWidget * MainWindow::getViewerWidget() const
{
	return m_osg_widget;
}

osgViewer::Viewer *MainWindow::getViewer() const
{
	return m_osg_widget->getViewer();
}

labelCanvasView *MainWindow::canvasView() const
{
	return m_view;
}

ModelOutliner * MainWindow::outliner() const
{
	return m_outliner;
}

TimeSlider * MainWindow::timeSlider() const
{
	return m_time_slider;
}

MainWindowTopBar *MainWindow::topBar() const
{
	return ui->topBar;
}

void MainWindow::paintEvent(QPaintEvent * event)
{
	//m_drag_area->setMask(m_drag_area->childrenRegion());
	//m_view->setMask(m_drag_area->childrenRegion());
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasText()) {
		qDebug() << "drag enter " << event->mimeData()->text();
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent * event)
{
	if (event->mimeData()->hasText()) {
		QString text = event->mimeData()->text();
		qDebug() << "drop file: " << text;

		if (text.startsWith("file:///")) {
			text.remove(0, 8); // remove the prefix
		}
		//m_vsimapp->openVSim(text.toStdString());
		emit sOpenFile(text.toStdString());
	}
}

void MainWindow::actionNew()
{
	qDebug("new");
	//m_vsimapp->reset();
	emit sNew();
}
void MainWindow::actionOpen()
{
	qDebug("open action");
	QString filename = QFileDialog::getOpenFileName(this, "Open .vsim", "", "VSim files (*.vsim; *.osg; *.osgt; *.osgb; );;All types (*.*)");
	if (filename == "") {
		qDebug() << "open cancel";
		return;
	}
	qDebug() << "opening - " << filename;
	
	//m_vsimapp->openVSim(filename.toStdString());
	emit sOpenFile(filename.toStdString());
}

void MainWindow::actionSave()
{
	emit sSaveCurrent();
}

void MainWindow::actionSaveAs()
{
	qDebug("saveas");
	QString filename = QFileDialog::getSaveFileName(this, "Save VSim", "", "VSim file (*.vsim);;osg ascii file (*.osgt);;osg binary file (*.osgb)");
	if (filename == "") {
		qDebug() << "saveas cancel";
		return;
	}
	qDebug() << "saving as - " << filename;
	// 
	//m_vsimapp->saveVSim(filename.toStdString());
	emit sSaveFile(filename.toStdString());
}

void MainWindow::actionImportModel()
{
	qDebug("import");
	QString filename = QFileDialog::getOpenFileName(this, "Import Model", "", "Model files (*.vsim; *.flt;*.ive;*.osg;*.osgb;*.osgt;*.obj;*.3ds; *.dae);;All types (*.*)");
	if (filename == "") {
		qDebug() << "import cancel";
		return;
	}
	qDebug() << "importing - " << filename;
	//m_vsimapp->importModel(filename.toStdString());
	emit sImportModel(filename.toStdString());
}