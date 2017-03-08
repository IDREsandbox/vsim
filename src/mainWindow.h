#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>
#include <osgQt/GraphicsWindowQt>
#include <osg/ShapeDrawable>
#include <osg/Material>

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>

#include <QtCore/QObject>

#include "OSGViewerWidget.h"
#include "viewerWidget.h"
#include "dragLabel.h"
#include "ui_mainOverlay.h"


class mainWindow : public QMainWindow
{	
	Q_OBJECT

public:
	mainWindow(QApplication& application) : QMainWindow()
	{
		actionOpen_Model = new QAction(this);
		actionOpen_Model->setObjectName(QString::fromUtf8("actionOpen_Model"));
		actionAdd_Model = new QAction(this);
		actionAdd_Model->setObjectName(QString::fromUtf8("actionAdd_Model"));
		actionImport_Narrative = new QAction(this);
		actionImport_Narrative->setObjectName(QString::fromUtf8("actionImport_Narrative"));
		actionImport_Embedded_Resource = new QAction(this);
		actionImport_Embedded_Resource->setObjectName(QString::fromUtf8("actionImport_Embedded_Resource"));
		actionSave_Narrative = new QAction(this);
		actionSave_Narrative->setObjectName(QString::fromUtf8("actionSave_Narrative"));
		actionSave_Embedded_Resource = new QAction(this);
		actionSave_Embedded_Resource->setObjectName(QString::fromUtf8("actionSave_Embedded_Resource"));
		actionLock_Narrative = new QAction(this);
		actionLock_Narrative->setObjectName(QString::fromUtf8("actionLock_Narrative"));
		actionLock_Embedded_Resource = new QAction(this);
		actionLock_Embedded_Resource->setObjectName(QString::fromUtf8("actionLock_Embedded_Resource"));
		actionExport_Model = new QAction(this);
		actionExport_Model->setObjectName(QString::fromUtf8("actionExport_Model"));
		actionSwitch_Manager = new QAction(this);
		actionSwitch_Manager->setObjectName(QString::fromUtf8("actionSwitch_Manager"));
		actionTime_Slider = new QAction(this);
		actionTime_Slider->setObjectName(QString::fromUtf8("actionTime_Slider"));
		actionQuit = new QAction(this);
		actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
		actionNavigation_Settings = new QAction(this);
		actionNavigation_Settings->setObjectName(QString::fromUtf8("actionNavigation_Settings"));
		actionContent_Creator = new QAction(this);
		actionContent_Creator->setObjectName(QString::fromUtf8("actionContent_Creator"));
		actionFonts_and_Colors = new QAction(this);
		actionFonts_and_Colors->setObjectName(QString::fromUtf8("actionFonts_and_Colors"));
		actionImage_Maker = new QAction(this);
		actionImage_Maker->setObjectName(QString::fromUtf8("actionImage_Maker"));
		actionMovie_Maker = new QAction(this);
		actionMovie_Maker->setObjectName(QString::fromUtf8("actionMovie_Maker"));
		actionAbout_VSim = new QAction(this);
		actionAbout_VSim->setObjectName(QString::fromUtf8("actionAbout_VSim"));
		actionCurrent_Coordinate = new QAction(this);
		actionCurrent_Coordinate->setObjectName(QString::fromUtf8("actionCurrent_Coordinate"));
		actionModel_Information = new QAction(this);
		actionModel_Information->setObjectName(QString::fromUtf8("actionModel_Information"));

		menubar = new QMenuBar(this);
		menubar->setObjectName(QString::fromUtf8("menubar"));
		menubar->setGeometry(QRect(0, 0, 1280, 21));
		menuFile = new QMenu(menubar);
		menuFile->setObjectName(QString::fromUtf8("menuFile"));
		menuSettings = new QMenu(menubar);
		menuSettings->setObjectName(QString::fromUtf8("menuSettings"));
		menuTools = new QMenu(menubar);
		menuTools->setObjectName(QString::fromUtf8("menuTools"));
		menuAbout = new QMenu(menubar);
		menuAbout->setObjectName(QString::fromUtf8("menuAbout"));
		this->setMenuBar(menubar);

		menubar->addAction(menuFile->menuAction());
		menubar->addAction(menuSettings->menuAction());
		menubar->addAction(menuTools->menuAction());
		menubar->addAction(menuAbout->menuAction());
		menuFile->addAction(actionOpen_Model);
		menuFile->addAction(actionAdd_Model);
		menuFile->addAction(actionImport_Narrative);
		menuFile->addAction(actionImport_Embedded_Resource);
		menuFile->addSeparator();
		menuFile->addAction(actionSave_Narrative);
		menuFile->addAction(actionSave_Embedded_Resource);
		menuFile->addSeparator();
		menuFile->addAction(actionLock_Narrative);
		menuFile->addAction(actionLock_Embedded_Resource);
		menuFile->addAction(actionExport_Model);
		menuFile->addSeparator();
		menuFile->addAction(actionSwitch_Manager);
		menuFile->addAction(actionTime_Slider);
		menuFile->addSeparator();
		menuFile->addAction(actionQuit);
		menuSettings->addAction(actionNavigation_Settings);
		menuSettings->addAction(actionContent_Creator);
		menuSettings->addAction(actionFonts_and_Colors);
		menuTools->addAction(actionImage_Maker);
		menuTools->addAction(actionMovie_Maker);
		menuAbout->addAction(actionAbout_VSim);
		menuAbout->addAction(actionCurrent_Coordinate);
		menuAbout->addAction(actionModel_Information);

		menubar->show();

		//..centrlWidget = new QWidget(this);
		////centrlWidget->setContentsMargins(0, 0, 0, 0);
		////setCentralWidget(centrlWidget);

		setMinimumSize(800, 600);

		//gw = createGraphicsWindow(0, 0, 640, 480);

		//osg::Node* f = osgDB::readNodeFile("T:\\Projects\\_UCLA\\vsim\\vsim-dependencies\\OpenSceneGraph-Data-3.4.0\\cow.osg");

		osgWidget = new OSGViewerWidget(this);
		setCentralWidget(osgWidget);

		//widget = new viewerWidget(this);
		//background = new QWidget(this);
		//background->setStyleSheet("background-color:blue;");
		//setCentralWidget(background);

		//widget = new QWidget(this);
		//widget->setStyleSheet("background-color:yellow;");
		//setCentralWidget(widget);

		//widget->layout()->setContentsMargin(0, 0, 0, 0);

		//widget->setGeometry(100, 100, 800, 600);
		//widget->setParent(this);
		//widget->show();

		//layout1 = new QWidget(this);
		////layout1->setAttribute(Qt::WA_TransparentForMouseEvents);

		//QGridLayout* otherLayout = new QGridLayout(layout1);
		//QPushButton* button2 = new QPushButton(layout1);
		//QObject::connect(button2, SIGNAL(clicked()), &application, SLOT(quit()));
		//button2->setMinimumSize(300, 300);

		//otherLayout->addWidget(button2, 2, 0);
		//layout1->setLayout(otherLayout);

		//QRegion reg(layout1->frameGeometry());
		//reg -= QRegion(layout1->geometry());
		//reg += layout1->childrenRegion();
		//layout1->setMask(reg);

		////window->setCentralWidget(otherLayout);
		layout2 = new QWidget(this);
		dragLabel* test = new dragLabel("drag widget", layout2);
		test->setObjectName(QString::fromUtf8("label"));
		//test->setParent(layout2);
		test->setGeometry(250, 250, 250, 250);
		//layout2->setAttribute(Qt::WA_TransparentForMouseEvents);

		//QGridLayout* controlsLayout = new QGridLayout(layout2);
		//QPushButton* button = new QPushButton(layout2);
		//QObject::connect(button, SIGNAL(clicked()), &application, SLOT(quit()));
		//button->setMinimumSize(140, 140);

		//controlsLayout->addWidget(button, 2, 0);
		//layout2->setLayout(controlsLayout);

		QRegion reg2(layout2->frameGeometry());
		reg2 -= QRegion(layout2->geometry());
		reg2 += layout2->childrenRegion();
		layout2->setMask(reg2);

		layout3 = new QWidget(this);
		ui = new Ui_mainOverlay();
		ui->setupUi(layout3);

		//QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(ui.topBar);
		//effect->setOpacity(0.5);
		//ui.topBar->setGraphicsEffect(effect);

		QRegion reg3(layout3->frameGeometry());
		reg3 -= QRegion(layout3->geometry());
		reg3 += layout3->childrenRegion();
		layout3->setMask(reg3);

		//oldSize = size();
	}

	void mainWindow::paintEvent(QPaintEvent* event)
	{
		layout2->setGeometry(rect());
		QRegion reg2(layout2->frameGeometry());
		reg2 -= QRegion(layout2->geometry());
		reg2 += layout2->childrenRegion();
		layout2->setMask(reg2);

		layout3->setGeometry(rect());
		QRegion reg3(layout3->frameGeometry());
		reg3 -= QRegion(layout3->geometry());
		reg3 += layout3->childrenRegion();
		layout3->setMask(reg3);
	}

	void mainWindow::resizeEvent(QResizeEvent *event)
	{
		Q_UNUSED(event);
		//widget->setGeometry(rect());

		/*layout1->setGeometry(rect());
		QRegion reg(layout1->frameGeometry());
		reg -= QRegion(layout1->geometry());
		reg += layout1->childrenRegion();
		layout1->setMask(reg);*/

		//osg::Node* scene = osgDB::readNodeFile("T:\\Projects\\_UCLA\\vsim\\vsim-dependencies\\OpenSceneGraph-Data-3.4.0\\cessna.osg");
		//widget->setNewScene(scene);

		layout3->setGeometry(rect());
		QRegion reg3(layout3->frameGeometry());
		reg3 -= QRegion(layout3->geometry());
		reg3 += layout3->childrenRegion();
		layout3->setMask(reg3);

		QList<dragLabel*> children = layout2->findChildren<dragLabel*>(QString::fromUtf8("label"));
		foreach(dragLabel* label, children){
			//proportionHeight = 1.0 - float(float(oldSize.height() - label->pos().y()) / oldSize.height());
			//proportionWidth = 1.0 - float(float(oldSize.width() - label->pos().x()) / oldSize.width());

			newX = osg::round(layout2->width() * label->ratioW());
			newY = osg::round(layout2->height() * label->ratioH());

			label->setGeometry(newX, newY, 250, 250);

			//oldSize = layout2->size();

			//proportionHeight = 1.0 - float(float(layout2->height() - label->pos().y()) / layout2->height());
			//proportionWidth = 1.0 - float(float(layout2->width() - label->pos().x()) / layout2->width());
		}

		layout2->setGeometry(rect());
		QRegion reg2(layout2->frameGeometry());
		reg2 -= QRegion(layout2->geometry());
		reg2 += layout2->childrenRegion();
		layout2->setMask(reg2);
	}

	osgQt::GraphicsWindowQt* mainWindow::createGraphicsWindow(int x, int y, int w, int h)
	{
		osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
		traits->windowDecoration = false;
		traits->x = x;
		traits->y = y;
		traits->width = w;
		traits->height = h;
		traits->doubleBuffer = true;
		traits->alpha = ds->getMinimumNumAlphaBits();
		traits->stencil = ds->getMinimumNumStencilBits();
		traits->sampleBuffers = ds->getMultiSamples();
		traits->samples = ds->getNumMultiSamples();

		return new osgQt::GraphicsWindowQt(traits.get());
	}

	~mainWindow()
	{

	}

protected:
	//QSize oldSize;
	//float proportionHeight;
	//float proportionWidth;
	int newX;
	int newY;

	QVBoxLayout *verticalLayout;
	//QVBoxLayout *verticalLayout_2;
	QWidget *topBar;
	QGridLayout *gridLayout;
	QWidget *plus;
	QWidget *minus;
	QWidget *open;
	QScrollArea *scrollArea;
	QWidget *scrollAreaWidgetContents;
	QWidget *forward;
	QWidget *pause;
	QWidget *info;
	QWidget *close;
	QSpacerItem *verticalSpacer;
	QWidget *bottomBar;
	QWidget* layout3;
	Ui_mainOverlay* ui;

	//QWidget* centrlWidget;
	osgQt::GraphicsWindowQt* gw;
	viewerWidget* widget;
	OSGViewerWidget* osgWidget;
	QWidget* layout1;
	QWidget* layout2;

	QAction *actionOpen_Model;
	QAction *actionAdd_Model;
	QAction *actionImport_Narrative;
	QAction *actionImport_Embedded_Resource;
	QAction *actionSave_Narrative;
	QAction *actionSave_Embedded_Resource;
	QAction *actionLock_Narrative;
	QAction *actionLock_Embedded_Resource;
	QAction *actionExport_Model;
	QAction *actionSwitch_Manager;
	QAction *actionTime_Slider;
	QAction *actionQuit;
	QAction *actionNavigation_Settings;
	QAction *actionContent_Creator;
	QAction *actionFonts_and_Colors;
	QAction *actionImage_Maker;
	QAction *actionMovie_Maker;
	QAction *actionAbout_VSim;
	QAction *actionCurrent_Coordinate;
	QAction *actionModel_Information;
	QMenuBar *menubar;
	QMenu *menuFile;
	QMenu *menuSettings;
	QMenu *menuTools;
	QMenu *menuAbout;

	//for testing
	QWidget *background;


	//QWidget* widget;
};

#endif // MAINWINDOW_H