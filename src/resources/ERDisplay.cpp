#include "resources/ERDisplay.h"
#include <QDebug>

ERDisplay::ERDisplay(QWidget *parent)
	: QFrame(parent), m_er(nullptr)
{
	ui.setupUi(this);

	ui.text->setWordWrap(true);

	connect(ui.open, &QAbstractButton::pressed, this, &ERDisplay::sOpen);
	connect(ui.goto_button, &QAbstractButton::pressed, this, &ERDisplay::sGoto);
	connect(ui.close, &QAbstractButton::pressed, this, &ERDisplay::sClose);
	connect(ui.close_all, &QAbstractButton::pressed, this, &ERDisplay::sCloseAll);

}

void ERDisplay::setInfo(EResource* er)
{
	if (m_er) disconnect(m_er, 0, this, 0);

	m_er = er;
	if (er == nullptr) return;

	connect(er, &QObject::destroyed, this, [this]() {
		m_er = nullptr;
	});
	connect(er, &EResource::sResourceNameChanged, this, &ERDisplay::reload);
	connect(er, &EResource::sResourceAuthorChanged, this, &ERDisplay::reload);
	connect(er, &EResource::sResourceDescriptionChanged, this, &ERDisplay::reload);
	connect(er, &EResource::sErTypeChanged, this, &ERDisplay::reload);

	reload();
}

void ERDisplay::reload()
{
	if (!m_er) return;
	ui.title->setText(QString::fromStdString(m_er->getResourceName()));
	ui.text->setText(QString::fromStdString(m_er->getResourceDescription()));
	ui.authors->setText(QString::fromStdString(m_er->getAuthor()));
	bool can_open = m_er->getERType() != EResource::ERType::ANNOTATION;
	ui.open->setEnabled(can_open);
}

void ERDisplay::setCount(int n)
{
	QString s = QString("Close All (%1)").arg(QString::number(n));
	ui.close_all->setText(s);
}
