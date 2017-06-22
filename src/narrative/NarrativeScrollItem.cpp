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

void NarrativeScrollItem::setInfo(const NarrativeInfo &data)
{
	m_title_label->setText(QString::fromStdString(data.m_title));
	m_desc_label->setText(QString::fromStdString(data.m_description));
	// TODO: elided text?
}



