#include "MainWindowTopBar.h"
#include "FocusFilter.h"

MainWindowTopBar::MainWindowTopBar(QWidget * parent) 
	: QFrame(parent) 
{
	ui.setupUi(this);

	QString dir = QCoreApplication::applicationDirPath();
	ui.lock_label->setPixmap(dir + "/assets/icons/vs_lock_on_16x.png");
	ui.lock_label->setVisible(false);
}

MainWindowTopBar::~MainWindowTopBar() {
	
}

bool MainWindowTopBar::showingNarratives() const
{
	return ui.stackedWidget->currentIndex() == 0;
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
	ui.narrative_title->setText(QString::fromStdString(header));
}

void MainWindowTopBar::showLockIcon(bool show)
{
	ui.lock_label->setVisible(show);
}
