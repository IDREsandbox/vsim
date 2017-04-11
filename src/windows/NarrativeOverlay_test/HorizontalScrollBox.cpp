#include "HorizontalScrollBox.h"
#include "NarrativeLayout.h"

HorizontalScrollBox::HorizontalScrollBox(QWidget* parent) : QScrollArea(parent)
{
	this->setObjectName(QStringLiteral("scrollArea"));
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	this->setWidgetResizable(true);

	auto saw = new QWidget(this);
	saw->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
	saw->setGeometry(QRect(0, 0, 653, 316));
	saw->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	saw->setStyleSheet(QStringLiteral("background-color: rgb(200, 100, 200);"));
	this->setWidget(saw);

	auto sawlayout = new CardLayout(saw, 10);
	sawlayout->setContentsMargins(10, 10, 10, 10);

	for (int i = 0; i < 100; i++) {
		//auto wx = new QWidget(saw);
		auto wx = new QLabel(QString::number(i), saw);
		wx->setStyleSheet(QString("background-color: rgb(") + QString::number((i * 40) % 255) + "," + QString::number(((i % 7) * 50) % 255) + "," + QString::number(100) + ");");
		//wx->setStyleSheet(QString("background-color: rgb(") + QString::number(100) + "," + QString::number(0) + "," + QString::number(0) + ");");
		wx->setGeometry(0, 0, 100, 100);
		//wx->setFixedWidth(100);
		//wx->setFixedHeight(300);
		//wx->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
		sawlayout->addWidget(wx);
	}
}

void HorizontalScrollBox::resizeEvent(QResizeEvent* event)
{
	int scrollpos = horizontalScrollBar()->value();
	auto bar = horizontalScrollBar();
	//horizontalScrollBar()->

	//if (bar->m)
	//float ratio = bar->value() / (float)bar->maximum();
	//qDebug() << bar->minimum() << bar->maximum() << bar->value() << ratio;
	//float ratio = bar->value() / bar->maximum();

	//event->accept();

	QScrollArea::resizeEvent(event);
	//this->setGeometry(event->);

	//horizontalScrollBar()->setSliderPosition(scrollpos);
	//qDebug() << "end" << scrollpos;
	
	float rescale = event->size().height()/(float)event->oldSize().height();
	
	qDebug() << "after" << bar->minimum() << bar->maximum() << bar->value();

	bar->setValue(bar->value()*rescale);
}
