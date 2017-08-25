#include "resources/ERScrollItem.h"

ERScrollItem::ERScrollItem()
	: m_er(nullptr)
{
	ui.setupUi(this);
}
ERScrollItem::ERScrollItem(EResource *er) 
	: ERScrollItem()
{
	setER(er);
}

void ERScrollItem::setER(EResource *er)
{
	if (m_er) disconnect(m_er, 0, this, 0);

	m_er = er;
	if (er == nullptr) return;

	connect(er, &EResource::sResourceNameChanged, this,
		[this]() {ui.title->setText(QString::fromStdString(m_er->getResourceName())); });

	ui.title->setText(QString::fromStdString(er->getResourceName()));

}

int ERScrollItem::widthFromHeight(int height)
{
	return height;
}

void ERScrollItem::mouseDoubleClickEvent(QMouseEvent * event)
{
	emit sDoubleClick();
}

