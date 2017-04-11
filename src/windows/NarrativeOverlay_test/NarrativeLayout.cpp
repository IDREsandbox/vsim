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
	QSize s(0, 0);
	int n = list.count();
	int i = 0;
	while (i < n) {
		QLayoutItem *o = list.at(i);
		s = s.expandedTo(o->minimumSize());
		++i;
	}
	//return s + n*QSize(spacing(), spacing());
	//m_previous_height * 100;
	//qDebug() << m_previous_height; 

	// FIXME: if you get a stack overflow... it's because the adding/removing of the scrollbar causes this to resize, then change width, then change the need for a scrollbar
	// solution? always show a scrollbar?
	// what does powerpoint do? fixed
	
	//Debug() << "parent parent" << this->parentWidget()->parentWidget()->parentWidget()->size();

	// use the height of the scrollbox
	int baseHeight = this->parentWidget()->parentWidget()->parentWidget()->size().height();
	int smallHeight = this->parentWidget()->parentWidget()->size().height();

	// if there is no scrollbar,
	// always assume there is a scrollbar
	//if 
	auto margins = contentsMargins();

	int totalspace = m_spacing*(qMax(0, list.size() - 1));
	qDebug() << totalspace;

	return QSize(margins.left() + margins.right() + totalspace + list.size() * smallHeight * m_ratio, 50);
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
	
	int w = r.width() - (list.count() - 1) * spacing();
	//int h = r.height() - (list.count() - 1) * spacing();
	int h = r.height();
	//qDebug() << r << w << h;

	int xpos = 0;
	xpos += margins.left();

	for (int i = 0; i < list.size(); i++) {
		QLayoutItem *o = list.at(i);

		QRect geom(xpos, 0, h*m_ratio, h);
		o->setGeometry(geom);

		xpos += h*m_ratio;
		xpos += m_spacing;
		//xpos += spacing();
		
	}
}
