#ifndef NARRATIVESLIDEITEM_H
#define NARRATIVESLIDEITEM_H

#include <osg/Vec4>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <QRectF>
#include <QColor>

#include "Command.h"

class NarrativeSlideItem : public QObject, public osg::Node
{
	Q_OBJECT
public:
	NarrativeSlideItem();
	NarrativeSlideItem(const NarrativeSlideItem& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}
	virtual ~NarrativeSlideItem() {}
	META_Node(, NarrativeSlideItem);

	// rect
	QRectF getRect() const;
	void setRect(QRectF rect);
	// for serializer
	const osg::Vec4 &getXYWH() const;
	void setXYWH(const osg::Vec4 &vec);

	// color
	QColor getBackground() const;
	void setBackground(const QColor &color);
	// for serializer
	const osg::Vec4 &getBackgroundRGBA() const;
	void setBackgroundRGBA(const osg::Vec4 &vec);

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

	mutable osg::Vec4 m_rect_vec;
	mutable osg::Vec4 m_color_vec;
};

#endif
