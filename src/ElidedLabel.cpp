#include "ElidedLabel.h"

#include <QPainter>
#include <QTextLayout>
#include <QDebug>
#include "ElidedLabel.h"

ElidedLabel::ElidedLabel(QWidget *parent)
	: QFrame(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void ElidedLabel::setText(const QString &newText)
{
	m_text = newText;
	update();
}

const QString &ElidedLabel::text() const
{
	return m_text;
}

void ElidedLabel::setHAlign(Qt::Alignment align)
{
	m_halign = align & Qt::AlignHorizontal_Mask;
}

void ElidedLabel::paintEvent(QPaintEvent *event)
{
	QFrame::paintEvent(event);

	QPainter painter(this);
	QFontMetrics fontMetrics = painter.fontMetrics();

	bool elided = false;
	int lineSpacing = fontMetrics.lineSpacing();
	int y = 0;

	QTextLayout textLayout(m_text, painter.font());
	textLayout.beginLayout();

	QTextOption opt;
	opt.setAlignment(Qt::AlignHCenter);

	while (true) {
		QTextLine line = textLayout.createLine();

		if (!line.isValid())
			break;

		line.setLineWidth(width());
		int nextLineY = y + lineSpacing;

		if (height() >= nextLineY + lineSpacing) {
			//line.draw(&painter, QPoint(0, y));
			QString line_text = m_text.mid(line.textStart(), line.textLength());
			painter.drawText(QRect(0, y, width(), height()), line_text, opt);
			y = nextLineY;
		}
		else {
			QString lastLine = m_text.mid(line.textStart());
			QString elidedLastLine = fontMetrics.elidedText(lastLine, Qt::ElideRight, width());
			//painter.drawText(QRect(0, y + fontMetrics.ascent(), width(), height()), elidedLastLine, opt);
			painter.drawText(QRect(0, y, width(), height()), elidedLastLine, opt);
			line = textLayout.createLine();
			elided = line.isValid();
			break;
		}
	}
	textLayout.endLayout();

	//if (didElide != elided) {
	//	elided = didElide;
	//	emit elisionChanged(didElide);
	//}
}