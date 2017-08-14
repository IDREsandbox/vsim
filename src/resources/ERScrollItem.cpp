#include "resources/ERScrollItem.h"

ERScrollItem::ERScrollItem()
	: m_er(nullptr)
{
	ui.setupUi(this);
}
ERScrollItem::ERScrollItem(EResourcesNode *er) 
	: ERScrollItem()
{
	setER(er);
}

void ERScrollItem::setER(EResourcesNode *er)
{
	if (m_er) disconnect(m_er, 0, this, 0);

	m_er = er;
	if (er == nullptr) return;

	connect(er, &EResourcesNode::sChanged, this,
		[this]() {ui.title->setText(QString::fromStdString(er->getTitle())); });

	ui.title->setText(QString::fromStdString(er->getTitle()));

}

void ERScrollItem::widthFromHeight(int height)
{
	return height;
}

void ERScrollItem::mouseDoubleClickEvent(QMouseEvent * event)
{
	emit sDoubleClick();
}

