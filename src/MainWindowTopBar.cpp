#include "MainWindowTopBar.h"
#include "FocusFilter.h"

MainWindowTopBar::MainWindowTopBar(QWidget * parent) 
	: QFrame(parent) 
{
	ui.setupUi(this);

	FocusFilter *narff = new FocusFilter(this);
	ui.page1->installEventFilter(narff);
	ui.narratives->scrollArea()->installEventFilter(narff);
	for (QObject *obj : ui.narrativeControl->children()) {
		obj->installEventFilter(narff);
	}
	connect(narff, &FocusFilter::sFocusIn, this, &MainWindowTopBar::sNarrativesPoked);

	FocusFilter *slideff = new FocusFilter(this);
	ui.page2->installEventFilter(slideff);
	ui.slides->scrollArea()->installEventFilter(slideff);
	for (QObject *obj : ui.slideControl->children()) {
		obj->installEventFilter(slideff);
	}
	connect(slideff, &FocusFilter::sFocusIn, this, &MainWindowTopBar::sSlidesPoked);
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
