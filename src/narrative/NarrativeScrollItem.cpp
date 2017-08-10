#include "NarrativeScrollItem.h"

NarrativeScrollItem::NarrativeScrollItem()
	: m_narrative(nullptr)
{
	ui.setupUi(this);
}
NarrativeScrollItem::NarrativeScrollItem(Narrative2 *narrative) 
	: NarrativeScrollItem()
{
	setNarrative(narrative);
}

void NarrativeScrollItem::setNarrative(Narrative2 *narrative)
{
	qDebug() << "set narrative disconnect";
	// disconnect current narrative
	for (auto conn : m_connections)	disconnect(conn);

	m_narrative = narrative;
	if (narrative == nullptr) return;

	m_connections.append(
		connect(narrative, &Narrative2::sTitleChanged, this,
			[this](const std::string &text) {ui.title->setText(QString::fromStdString(text)); })
		);
	m_connections.append(
		connect(narrative, &Narrative2::sDescriptionChanged, this,
			[this](const std::string &text) {ui.description->setText(QString::fromStdString(text)); })
	);
	m_connections.append(
		connect(narrative, &Narrative2::sAuthorChanged, this,
			[this](const std::string &text) {ui.author->setText(QString::fromStdString(text)); })
	);

	ui.title->setText(QString::fromStdString(narrative->getTitle()));
	ui.description->setText(QString::fromStdString(narrative->getDescription()));
	ui.author->setText(QString::fromStdString(narrative->getAuthor()));
}

void NarrativeScrollItem::mousePressEvent(QMouseEvent * event)
{
	//if (event->type() == QEvent::MouseButtonDblClick) {
	//	qDebug() << "nsi double click";
	//	emit sDoubleClick();
	//}
	ScrollBoxItem::mousePressEvent(event);
}

void NarrativeScrollItem::mouseDoubleClickEvent(QMouseEvent * event)
{
	emit sDoubleClick();
}

