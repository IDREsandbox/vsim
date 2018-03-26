#include "resources/ERDisplay.h"
#include <QDebug>

ERDisplay::ERDisplay(QWidget *parent)
	: QFrame(parent), m_er(nullptr)
{
	ui.setupUi(this);

	ui.text->setWordWrap(true);
}

void ERDisplay::setInfo(EResource* er)
{
	if (m_er) disconnect(m_er, 0, this, 0);

	m_er = er;
	if (er == nullptr) return;

	connect(er, &EResource::sResourceNameChanged, this,
		[this]() {ui.title->setText(QString::fromStdString(m_er->getResourceName())); });

	connect(er, &EResource::sResourceDescriptionChanged, this,
		[this]() {ui.text->setText(QString::fromStdString(m_er->getResourceDescription())); });

	connect(er, &EResource::sResourceAuthorChanged, this,
		[this]() {ui.authors->setText(QString::fromStdString(m_er->getAuthor())); });

	ui.title->setText(QString::fromStdString(er->getResourceName()));
	ui.text->setText(QString::fromStdString(er->getResourceDescription()));
	ui.authors->setText(QString::fromStdString(er->getAuthor()));
}