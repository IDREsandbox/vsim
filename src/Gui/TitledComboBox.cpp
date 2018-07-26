#include "TitledComboBox.h"

#include <QPaintEvent>
#include <QStylePainter>

TitledComboBox::TitledComboBox(QWidget * parent)
	: QComboBox(parent)
{
}

void TitledComboBox::setTitle(const QString & title)
{
	m_title = title;
}

// straight up copied from Qt source
// changed opt.currentText to be the same as title
void TitledComboBox::paintEvent(QPaintEvent * e)
{
	QStylePainter painter(this);
	painter.setPen(palette().color(QPalette::Text));

	QStyleOptionComboBox opt;
	initStyleOption(&opt);
	opt.currentText = m_title;

	painter.drawComplexControl(QStyle::CC_ComboBox, opt);
	painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}
