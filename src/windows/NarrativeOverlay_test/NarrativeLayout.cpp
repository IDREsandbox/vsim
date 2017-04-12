#include "NarrativeLayout.h"

CardLayout::~CardLayout()
{
	QLayoutItem *item;
	while ((item = takeAt(0)))
		delete item;
}

void CardLayout::addItem(QLayoutItem *item)
{
	list.append(item);
}

QSize CardLayout::sizeHint() const
{
	QSize s(0, 0);
	int n = list.count();
	if (n > 0)
		s = QSize(100, 70); //start with a nice default size
	int i = 0;
	while (i < n) {
		QLayoutItem *o = list.at(i);
		s = s.expandedTo(o->sizeHint());
		++i;
	}
	return s + n*QSize(spacing(), spacing());
}

QSize CardLayout::minimumSize() const
{
	// deal with this in the scrollbox
	return QSize(100, 50);
}

int CardLayout::count() const
{
	// QList::size() returns the number of QLayoutItems in the list
	return list.size();
}

QLayoutItem *CardLayout::itemAt(int idx) const
{
	// QList::value() performs index checking, and returns 0 if we are
	// outside the valid range
	return list.value(idx);
}


QLayoutItem *CardLayout::takeAt(int idx)
{
	// QList::take does not do index checking
	return idx >= 0 && idx < list.size() ? list.takeAt(idx) : 0;
}

void CardLayout::setGeometry(const QRect &r)
{
	QLayout::setGeometry(r);
	
	auto margins = contentsMargins();

	if (list.size() == 0)
		return;
	
	//int w = r.width() - (list.count() - 1) * spacing();
	//int h = r.height() - (list.count() - 1) * spacing();
	//int h = r.height() - margins.top() - margins.bottom();

	int xpos = 0;
	xpos += margins.left();

	//m_height = r.height()
	int bheight = boxHeight();
	int bwidth = boxWidth();

	for (int i = 0; i < list.size(); i++) {
		QLayoutItem *o = list.at(i);

		QRect geom(xpos, margins.top(), bwidth, bheight);
		o->setGeometry(geom);

		xpos += bwidth;
		xpos += m_spacing;
	}
}

int CardLayout::getMinWidth() const
{
	auto margins = contentsMargins();

	int boxh = boxHeight();
	int totalspace = m_spacing*(qMax(0, list.size() - 1));
	int minwidth = margins.left() + margins.right() + totalspace + list.size() * boxWidth();

	return minwidth;
}

int CardLayout::boxHeight() const
{
	auto margins = contentsMargins();
	return m_height - margins.top() - margins.bottom();
}

int CardLayout::boxWidth() const
{
	return floor(boxHeight()*m_ratio);
}

//int CardLayout::positionOf(int index) const
//{
//	int smallHeight = this->parentWidget()->parentWidget()->size().height();
//	auto margins = contentsMargins();
//	int boxh = smallHeight - margins.top() - margins.bottom();
//
//	int totalspace = m_spacing*(qMax(0, list.size() - 1));
//	qDebug() << totalspace;
//
//	return ;
//}
