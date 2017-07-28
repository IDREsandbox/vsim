#ifndef DRAGLABEL_H
#define DRAGLABEL_H

#include <QtWidgets/QWidget>
#include <QtWidgets>
#include <QtGui/QMouseEvent>
#include <iostream>
#include <QtCore/QBasicTimer>
#include "MainWindow.h"
#include "labelCanvas.h"

class dragLabel : public QTextEdit
{
	Q_OBJECT

public:
	//constructor for creating new widget
	dragLabel(labelCanvas* parent, std::string style);

	//constructor for generation from data
	dragLabel(std::string str, std::string style, labelCanvas* parent, float rH, float rW, float rY, float rX);
	~dragLabel();

	void setIndex(int index);
	int getIndex();

	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void timerEvent(QTimerEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	//void recalcFontRatios();

	void paintEvent(QPaintEvent *event);

	void canvasResize();

signals:
	void sTextSet(QString, int);
	void sSizeSet(QSize, int);
	void sPosSet(QPoint, int);

public:
	labelCanvas* par;
	
	QPoint offset;
	QPoint resizeOffset;

	float scaleFactor = 1;
	
	//float ratioFHeight;
	//float ratioFWidth;

	float ratioHeight;
	float ratioWidth;

	float ratioX;
	float ratioY;
	
	bool dragEdge;
	QRect bottomRight;

	QBasicTimer timer;

	int m_index = -1;
};

#endif // DRAGLABEL_H