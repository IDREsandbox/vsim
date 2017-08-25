#ifndef DRAGLABEL_H
#define DRAGLABEL_H

#include <QtWidgets/QWidget>
#include <QtWidgets>
#include <QtGui/QMouseEvent>
#include <iostream>
#include <QtCore/QBasicTimer>
#include "labelCanvas.h"

class labelCanvas;
class NarrativeSlideLabels;

class dragLabel : public QTextEdit
{
	Q_OBJECT

public:
	dragLabel(labelCanvas *parent);

	//constructor for creating new widget
	//dragLabel(labelCanvas* parent, std::string style);

	//constructor for generation from data
	//dragLabel(std::string str, std::string style, labelCanvas* parent, float rH, float rW, float rY, float rX);
	~dragLabel();

	void setIndex(int index);
	int getIndex();

	void mousePressEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	//void recalcFontRatios();

	void ValignMiddle(QTextEdit* pTextEdit);

	// refresh the position/size based on percentages
	void canvasResize();

	void setPos(float x, float y);
	void setSize(float w, float h);
	
	void setLabel(NarrativeSlideLabels *label);

signals:
	//void sTextSet(QString, int);
	void sSetSize(float rw, float rh, int);
	void sSetPos(float rx, float ry, int);
	void sEdited(int index);

public:
	labelCanvas* par;

	NarrativeSlideLabels *m_label;
	
	QPoint m_drag_start;

	//float scaleFactor = 1;
	
	//float ratioFHeight;
	//float ratioFWidth;

	float ratioHeight;
	float ratioWidth;

	float ratioX;
	float ratioY;

	// width and height when dragging started
	int m_startwidth;
	int m_startheight;
	
	bool m_dragging;
	bool m_resizing;
	//QRect bottomRight;

	int m_index = -1;
};

#endif // DRAGLABEL_H