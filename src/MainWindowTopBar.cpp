#include "MainWindowTopBar.h"

MainWindowTopBar::MainWindowTopBar(QWidget * parent) 
	: QFrame(parent) 
{
	ui.setupUi(this);
}

MainWindowTopBar::~MainWindowTopBar() {
	
}

void MainWindowTopBar::showNarratives()
{
	ui.stackedWidget->setCurrentIndex(0);
}

void MainWindowTopBar::showSlides()
{
	ui.stackedWidget->setCurrentIndex(1);
}

void MainWindowTopBar::setSlidesHeader(std::string header)
{
	ui.label_2->setText(QString::fromStdString(header));
}
