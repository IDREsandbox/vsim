#include "HorizontalScrollBox.h"

HorizontalScrollBox::HorizontalScrollBox(QWidget* parent) : QScrollArea(parent)
{
	//narratives->setObjectName(QStringLiteral("narratives"));
	//narratives->setWidgetResizable(true);
	//this->setWidgetResizable(true);

	////QHBoxLayout* m_fill_layout = new QHBoxLayout(m_scroll_area_widget);
	////m_fill_layout->setObjectName("fillLayout");
	////m_fill_layout->setContentsMargins(0, 0, 0, 0);

	m_scroll_area_widget = new QWidget();
	m_scroll_area_widget->setObjectName("scrollAreaWidget");
	////m_scroll_area_widget->setGeometry(QRect(0, 0, 972, 113));
	////m_scroll_area_widget->set
	////m_scroll_area_widget->setFixedWidth(1000);
	m_scroll_area_widget->setStyleSheet("background-color: rgb(200, 0, 0);");
	//m_scroll_area_widget->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));

	//this->setWidget(m_scroll_area_widget);
	//this->setStyleSheet("background-color: rgb(0,100,0)");

	//m_scroll_area_layout = new QHBoxLayout(m_scroll_area_widget);
	//m_scroll_area_layout->setObjectName("scrollAreaLayout");
	//m_scroll_area_layout->setContentsMargins(0, 0, 0, 0);

	//m_card_layout = new CardLayout(m_scroll_area_widget);


	//QWidget* contentsWidget = new QWidget(this);
	//contentsWidget->resize(1000, 100);
	//contentsWidget->setFixedWidth(1000);
	//contentsWidget->setStyleSheet("background-color: rgb(0, 200, 200);");
	//contentsWidget->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));

	//QLabel *imageLabel = new QLabel;
	//QImage image("bigcow.png");
	//imageLabel->setPixmap(QPixmap::fromImage(image));
	////imageLabel->setText("FOOOOOOOOOOOOOOOOOO");

	//this->setBackgroundRole(QPalette::Dark);
	//this->setWidget(imageLabel);

	//{
	//	auto widget1 = new QWidget(m_scroll_area_widget);
	//	//widget_6->setObjectName(QStringLiteral("widget_6"));
	//	//QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//	widget1->setStyleSheet(QStringLiteral("background-color: rgb(0, 0, 150);"));
	//	widget1->resize(100, 100);
	//	m_scroll_area_layout->addWidget(widget1);
	//}
	//{
	//	auto widget1 = new QWidget(m_scroll_area_widget);
	//	widget1->setStyleSheet(QStringLiteral("background-color: rgb(0, 0, 150);"));
	//	widget1->resize(100, 100);
	//	m_scroll_area_layout->addWidget(widget1);
	//}
	//{
	//	auto widget1 = new QWidget(m_scroll_area_widget);
	//	widget1->setStyleSheet(QStringLiteral("background-color: rgb(0, 0, 150);"));
	//	widget1->resize(100, 100);
	//	m_scroll_area_layout->addWidget(widget1);
	//}
}
