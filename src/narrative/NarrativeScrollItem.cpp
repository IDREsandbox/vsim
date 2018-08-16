#include "NarrativeScrollItem.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPixmapCache>

#include "Core/LockTable.h"
#include "Core/Util.h"

#include "narrative/Narrative.h"

NarrativeScrollItem::NarrativeScrollItem(QWidget *parent)
	: ScrollBoxItem(parent),
	m_narrative(nullptr),
	m_lock(nullptr)
{
	ui.setupUi(this);
	ui.description->setHAlign(Qt::AlignHCenter);

	QString dir = QCoreApplication::applicationDirPath();
	m_lock_pixmap = QPixmap(dir + "/assets/icons/vs_lock_on_16x.png");
}

void NarrativeScrollItem::setNarrative(Narrative *narrative)
{

	Util::reconnect(this, &m_narrative, narrative);
	Util::reconnect(this, &m_lock, narrative ? narrative->lockTable() : nullptr);

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
	connect(m_lock, &LockTable::sLockChanged, this, [this]() {update();});
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
		|| (tr.width() + 2) > br.width()) {
		// use the smaller font size
		font.setPointSize(small_size);
	}
	label->setFont(font);
}

void NarrativeScrollItem::paintEvent(QPaintEvent *e)
{
	ScrollBoxItem::paintEvent(e);

	QPainter p(this);

	if (m_lock->isLocked()) {
		QRect r;
		r.setSize(m_lock_pixmap.size());
		int margin = 2;
		r.moveTopRight(QPoint(width() - margin, margin));
		p.drawPixmap(r.topLeft(), m_lock_pixmap);
	}
}

void NarrativeScrollItem::resizeEvent(QResizeEvent * e)
{
	fitTitle();
}

