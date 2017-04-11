#include "narrativeoverlay_test.h"
#include "HorizontalScrollBox.h"
#include "NarrativeLayout.h"
#include <QtWidgets/QHBoxLayout>

NarrativeOverlay_test::NarrativeOverlay_test(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	auto sa = new HorizontalScrollBox(ui.centralwidget);
	ui.gridLayout->addWidget(sa);


	//auto x = new HorizontalScrollBox(ui.centralwidget);
	//this->setCentralWidget(x);

	////ui.scrollAreaWidgetContents->setFixedHeight(200);
	////ui.scrollAreaWidgetContents->setGeometry(0, 0, 1000, 200);
	////ui.scrollAreaWidgetContents->setMinimumWidth(1000);
	//auto sa = new QScrollArea(ui.centralwidget);
	//sa->setObjectName(QStringLiteral("scrollArea"));
	//sa->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	//sa->setWidgetResizable(true);

	//auto saw = new QWidget();
	//saw->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
	//saw->setGeometry(QRect(0, 0, 653, 316));
	////saw->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	//saw->setStyleSheet(QStringLiteral("background-color: rgb(200, 100, 200);"));
	//sa->setWidget(saw);

	//ui.gridLayout->addWidget(sa);

	//auto sawlayout = new CardLayout(saw, 10);
	////auto sawlayout = new QHBoxLayout(saw);

	//for (int i = 0; i < 10; i++) {
	//	auto wx = new QWidget(saw);
	//	wx->setStyleSheet(QString("background-color: rgb(") + QString::number((i*40)%255) + "," + QString::number(((i % 7) * 50)%255) + "," + QString::number(100) + ");");
	//	//wx->setStyleSheet(QString("background-color: rgb(") + QString::number(100) + "," + QString::number(0) + "," + QString::number(0) + ");");
	//	wx->setGeometry(0, 0, 100, 100);
	//	wx->setFixedWidth(100);
	//	wx->setFixedHeight(300);
	//	wx->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	//	sawlayout->addWidget(wx);
	//}



	//wx->setStyleSheet("background-color: rgb(0, 0, 200);");
	//auto wx = new QWidget(saw);

	//wx->setGeometry(0, 0, 100, 100);
	//wx->setFixedWidth(300);
	//wx->setFixedHeight(300);
	//wx->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

	//sawlayout->addWidget(wx);

	//// random box on top
	//auto ab = new QWidget(ui.centralwidget);
	//ab->setStyleSheet("background-color: rgb(0, 0, 0);");
	//ab->setMinimumHeight(100);
	//ab->setGeometry(10, 10, 400, 200);
	//ab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	////ui.gridLayout->addWidget(ab);

	//auto ablayout = new QHBoxLayout(ab);



	// bottom layout test
	auto w = new QWidget(ui.centralwidget);
	//sb->setWidget(w);
	//w->setGeometry(QRect(0, 0, 972, 113));
	//w->setMinimumSize(800, 200);
	w->setStyleSheet("background-color: rgb(200, 100, 200);");
	ui.gridLayout->addWidget(w);

	//auto cardlayout = new CardLayout(ui.centralwidget, 10);

	auto cardlayout = new QHBoxLayout(w);
	cardlayout->setObjectName("scrollAreaLayout");
	cardlayout->setContentsMargins(0, 0, 0, 0);
	cardlayout->setSpacing(10);

	auto w1 = new QWidget(w);
	w1->setStyleSheet("background-color: rgb(0, 0, 200);");
	w1->setGeometry(0,0,100,100);
	w1->setFixedWidth(100);
	w1->setFixedHeight(100);
	w1->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	
	cardlayout->addWidget(w1);

	w1 = new QWidget(w);
	w1->setObjectName(QStringLiteral("green"));
	w1->setStyleSheet("background-color: rgb(0, 200, 0);");
	w1->setMinimumSize(QSize(90, 90));
	w1->setMaximumSize(QSize(90, 90));
	w1->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

	cardlayout->addWidget(w1);

	auto widget_13 = new QWidget(w);
	widget_13->setObjectName(QStringLiteral("aaa"));
	widget_13->setMinimumSize(QSize(90, 90));
	widget_13->setMaximumSize(QSize(90, 90));
	widget_13->setStyleSheet(QStringLiteral("background-color: rgb(0, 25, 27);"));

	cardlayout->addWidget(widget_13);


	widget_13 = new QWidget(w);
	widget_13->setObjectName(QStringLiteral("widget_13"));
	widget_13->setMinimumSize(QSize(90, 90));
	widget_13->setMaximumSize(QSize(90, 90));
	widget_13->setStyleSheet(QStringLiteral("background-color: rgb(175, 25, 27);"));

	cardlayout->addWidget(widget_13);


/*

	auto container = new QWidget(ui.centralwidget);
	container->setObjectName(QStringLiteral("container"));
	auto horizontalLayout = new QHBoxLayout(container);
	horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

	auto x1 = new QWidget(container);
	x1->setObjectName(QStringLiteral("x1"));
	x1->setStyleSheet(QStringLiteral("background-color: rgb(0, 0, 150);"));

	horizontalLayout->addWidget(x1);

	auto x2 = new QWidget(container);
	x2->setObjectName(QStringLiteral("x2"));
	x2->setStyleSheet(QStringLiteral("background-color: rgb(150, 0, 0);"));

	horizontalLayout->addWidget(x2);*/

}

NarrativeOverlay_test::~NarrativeOverlay_test()
{

}
