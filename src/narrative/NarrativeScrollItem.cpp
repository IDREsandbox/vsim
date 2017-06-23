#include <QDebug>
#include "NarrativeScrollItem.h"


NarrativeScrollItem::NarrativeScrollItem(QWidget * parent)
	: ScrollBoxItem(parent)
{

	//setStyleSheet(QLatin1String("background-color: rgb(225, 255, 48);\n"
	//	""));
	m_layout = new QVBoxLayout(this);
	this->setAutoFillBackground(true);

	m_title_label = new QLabel(this);
	m_title_label->setStyleSheet(QLatin1String("color: rgb(255, 255, 255);\n"
		"font: 12pt;"));
	m_title_label->setAlignment(Qt::AlignCenter);
	m_layout->addWidget(m_title_label);

	m_desc_label = new QLabel(this);
	m_desc_label->setStyleSheet(QStringLiteral("color: rgb(255, 255, 255);\n"
		"font: 8pt;"));
	m_desc_label->setAlignment(Qt::AlignCenter);
	m_layout->addWidget(m_desc_label);
	m_desc_label->setWordWrap(true);

}

//NarrativeScrollItem::NarrativeScrollItem(const Narrative &nar)
//	: NarrativeScrollItem(nullptr)
//{
//	setInfo(nar);
//}

//void NarrativeScrollItem::setInfo(const Narrative &nar)
//{
//	m_title_label->setText(QString::fromStdString(nar.getName()));
//	m_desc_label->setText(QString::fromStdString(nar.getDescription()));
//}

NarrativeScrollItem::NarrativeScrollItem(const std::string & title, const std::string & description)
	: NarrativeScrollItem(nullptr)
{
	setInfo(title, description);
}

void NarrativeScrollItem::setInfo(const std::string & title, const std::string & description)
{
	m_title_label->setText(title.c_str());
	m_desc_label->setText(description.c_str());
}
