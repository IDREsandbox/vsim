#include "NarrativeScrollItem.h"

NarrativeScrollItem::NarrativeScrollItem(QWidget *parent)
	: ScrollBoxItem(parent),
	m_narrative(nullptr)
{
	ui.setupUi(this);
	ui.description->setHAlign(Qt::AlignHCenter);
	setAttribute(Qt::WA_OpaquePaintEvent); // optimization?
}

void NarrativeScrollItem::setNarrative(Narrative *narrative)
{
	if (m_narrative != nullptr) disconnect(m_narrative, 0, this, 0);
	m_narrative = narrative;
	if (narrative == nullptr) return;

	// initialize
	ui.title->setText(QString::fromStdString(narrative->getTitle()));
	ui.description->setText(QString::fromStdString(narrative->getDescription()));
	ui.author->setText(QString::fromStdString(narrative->getAuthor()));
	
	// connections
	connect(narrative, &Narrative::sTitleChanged, this,
		[this](const std::string &text) {
		ui.title->setText(QString::fromStdString(text));
		fitTitle();
	});
	connect(narrative, &Narrative::sDescriptionChanged, this,
		[this](const std::string &text) {ui.description->setText(QString::fromStdString(text)); });
	connect(narrative, &Narrative::sAuthorChanged, this,
		[this](const std::string &text) {ui.author->setText(QString::fromStdString(text)); });
}

int NarrativeScrollItem::widthFromHeight(int height) const
{
	return 16.0/9.0 * height;
}

void NarrativeScrollItem::fitTitle()
{
	auto *label = ui.title;

	int base_size = 12;
	int small_size = 9;

	label->setWordWrap(true);

	QFont font = label->font();
	font.setPointSize(base_size);

	QString text = m_narrative->getTitle().c_str();
	QFontMetrics metrics(font);

	QSize bound(label->width(), label->height());

	QRect br = label->rect();

	QRect tr = metrics.boundingRect(0, 0, label->width(), label->height(), Qt::TextWordWrap, text);

	if (tr.height() > br.height()
		|| tr.width() > br.width()) {
		// use the smaller font size
		font.setPointSize(small_size);
	}
	label->setFont(font);
}

void NarrativeScrollItem::resizeEvent(QResizeEvent * e)
{
	fitTitle();
}

