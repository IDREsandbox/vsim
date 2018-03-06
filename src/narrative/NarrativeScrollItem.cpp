#include "NarrativeScrollItem.h"

NarrativeScrollItem::NarrativeScrollItem(QWidget *parent)
	: ScrollBoxItem(parent),
	m_narrative(nullptr)
{
	ui.setupUi(this);
}

void NarrativeScrollItem::setNarrative(Narrative2 *narrative)
{
	if (m_narrative != nullptr) disconnect(m_narrative, 0, this, 0);
	m_narrative = narrative;
	if (narrative == nullptr) return;

	// initialize
	ui.title->setText(QString::fromStdString(narrative->getTitle()));
	ui.description->setText(QString::fromStdString(narrative->getDescription()));
	ui.author->setText(QString::fromStdString(narrative->getAuthor()));
	
	// connections
	connect(narrative, &Narrative2::sTitleChanged, this,
		[this](const std::string &text) {ui.title->setText(QString::fromStdString(text)); });
	connect(narrative, &Narrative2::sDescriptionChanged, this,
		[this](const std::string &text) {ui.description->setText(QString::fromStdString(text)); });
	connect(narrative, &Narrative2::sAuthorChanged, this,
		[this](const std::string &text) {ui.author->setText(QString::fromStdString(text)); });
}

int NarrativeScrollItem::widthFromHeight(int height) const
{
	return 16.0/9.0 * height;
}

