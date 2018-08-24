#include "resources/ERDisplay.h"
#include <QDebug>
#include <QMouseEvent>

#include "ECategory.h"
#include "Core/Util.h"

ERDisplay::ERDisplay(QWidget *parent)
	: QFrame(parent),
	m_er(nullptr),
	m_cat(nullptr)
{
	ui.setupUi(this);

	connect(ui.open, &QAbstractButton::pressed, this, &ERDisplay::sOpen);
	connect(ui.goto_button, &QAbstractButton::pressed, this, &ERDisplay::sGoto);
	connect(ui.close, &QAbstractButton::pressed, this, &ERDisplay::sClose);
	connect(ui.close_all, &QAbstractButton::pressed, this, &ERDisplay::sCloseAll);

	m_display_opacity = new QGraphicsOpacityEffect(this);
	m_display_opacity->setOpacity(1.0);
	this->setGraphicsEffect(m_display_opacity);
	m_fade_in_anim = new QPropertyAnimation(m_display_opacity, "opacity");
	m_fade_in_anim->setDuration(250);
	m_fade_in_anim->setStartValue(0.0);
	m_fade_in_anim->setEndValue(1.0);
	m_fade_out_anim = new QPropertyAnimation(m_display_opacity, "opacity");
	m_fade_out_anim->setDuration(250);
	m_fade_out_anim->setStartValue(1.0);
	m_fade_out_anim->setEndValue(0.0);

	connect(m_fade_out_anim, &QPropertyAnimation::finished, this, [this]() {
		hide();
	});
}

void ERDisplay::setInfo(EResource* er)
{
	Util::reconnect(this, &m_er, er);
	if (!er) return;

	connect(er, &EResource::sResourceNameChanged, this, &ERDisplay::reload);
	connect(er, &EResource::sResourceAuthorChanged, this, &ERDisplay::reload);
	connect(er, &EResource::sResourceDescriptionChanged, this, &ERDisplay::reload);
	connect(er, &EResource::sErTypeChanged, this, &ERDisplay::reload);
	connect(er, &EResource::sCategoryChanged, this, [this](auto *old_cat, auto *new_cat) {
		setCategory(new_cat);
	});

	setCategory(er->category());
	reload();
}

void ERDisplay::setCategory(ECategory * cat)
{
	Util::reconnect(this, &m_cat, cat);
	reload();
	if (!cat) return;

	connect(cat, &ECategory::sCNameChanged, this, &ERDisplay::reload);
	connect(cat, &ECategory::sColorChanged, this, &ERDisplay::reload);
}

void ERDisplay::reload()
{
	if (!m_er) return;
	ui.title->setText(QString::fromStdString(m_er->getResourceName()));
	ui.description->setText(QString::fromStdString(m_er->getResourceDescription()));
	//ui.authors->setText(QString::fromStdString(m_er->getAuthor()));
	bool can_open = m_er->getERType() != EResource::ERType::ANNOTATION;
	ui.open->setEnabled(can_open);
	ui.open->setVisible(can_open);

	ui.category->setVisible(m_cat != nullptr);
	if (m_cat) {
		QColor color = m_cat->getColor();
		QColor text_color;

		if (color.lightness() > 150) { // bright colors
			text_color = QColor(0, 0, 0);
		}
		else { // dark colors
			text_color = QColor(255, 255, 255);
		}
		QString stylesheet =
			Util::colorToStylesheet(m_cat->getColor()) +
			QString().sprintf("color:rgb(%d, %d, %d);",
				text_color.red(), text_color.green(), text_color.blue());

		ui.category->setText(QString::fromStdString(m_cat->getCategoryName()));
		ui.category->setStyleSheet(stylesheet);
	}
}

void ERDisplay::setCount(int n)
{
	QString s = QString("Close All (%1)").arg(QString::number(n));
	ui.close_all->setText(s);
}

void ERDisplay::setHardVisible(bool visible)
{
	setVisible(visible);
	m_fade_in_anim->stop();
	m_fade_out_anim->stop();
	m_display_opacity->setOpacity(1.0);
}

bool ERDisplay::canFadeIn()
{
	if (!isVisible()) return true;
	if (m_fade_out_anim->state() == QPropertyAnimation::State::Running) {
		return true;
	}
	return false;
}

void ERDisplay::fadeIn()
{
	show();
	m_fade_out_anim->stop();
	m_fade_in_anim->start();
}

void ERDisplay::fadeOut()
{
	show();
	m_fade_in_anim->stop();
	m_fade_out_anim->start();
}

void ERDisplay::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::RightButton) {
		emit sCloseAll();
	}
}