#ifndef NARRATIVESLIDEITEM_H
#define NARRATIVESLIDEITEM_H

#include <osg/Vec4>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <QRectF>
#include <QColor>

#include "Command.h"

class NarrativeSlideItem : public QObject
{
	Q_OBJECT
public:
	NarrativeSlideItem(QObject *parent = nullptr);

	// rect
	QRectF getRect() const;
	void setRect(QRectF rect);

	// color
	QColor getBackground() const;
	void setBackground(const QColor &color);

signals:
	void sTransformed(QRectF rect);
	void sBackgroundChanged(QColor color);

public: // COMMANDS
	class TransformCommand : public ModifyCommand<NarrativeSlideItem, QRectF> {
	public:
		TransformCommand(NarrativeSlideItem *item, const QRectF &rect, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getRect, &setRect, rect, item, parent) {}
	};

private:
	QRectF m_rect;
	QColor m_color;
};

#endif
