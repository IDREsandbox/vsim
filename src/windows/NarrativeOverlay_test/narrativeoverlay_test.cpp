#include "narrativeoverlay_test.h"
#include "HorizontalScrollBox.h"
#include "NarrativeLayout.h"
#include <QtWidgets/QHBoxLayout>

NarrativeOverlay_test::NarrativeOverlay_test(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	auto x = new HorizontalScrollBox(ui.centralwidget);
	//this->setCentralWidget(x);

	auto w = new QWidget(ui.centralwidget);
	w->setGeometry(QRect(0, 0, 972, 113));
	w->setStyleSheet("background-color: rgb(200, 0, 200);");

	//auto cardlayout = new CardLayout(ui.centralwidget, 10);

	auto cardlayout = new QHBoxLayout(ui.centralwidget);
	cardlayout->setObjectName("scrollAreaLayout");
	cardlayout->setContentsMargins(0, 0, 0, 0);
	cardlayout->setSpacing(10);

	auto w1 = new QWidget(w);
	w1->setStyleSheet("background-color: rgb(0, 0, 200);");
	w1->setGeometry(0,0,100,100);
	w1->setFixedWidth(100);
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
}

NarrativeOverlay_test::~NarrativeOverlay_test()
{

}
