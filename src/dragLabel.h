#ifndef DRAGLABEL_H
#define DRAGLABEL_H

#include <QtWidgets/QWidget>
#include <QtWidgets>
#include <QtGui/QMouseEvent>
#include <iostream>
#include <QtCore/QBasicTimer>
#include "MainWindow.h"

class dragLabel : public QLabel
{
	Q_OBJECT

public:
	//constructor for creating new widget
	dragLabel(QWidget* parent, std::string style);

	//constructor for generation from data
	dragLabel(std::string str, std::string style, int x, int y, int w, int h, QWidget* parent);
	~dragLabel();

	void setIndex(int index);
	int getIndex();

	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void timerEvent(QTimerEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

	void updateParSize();
	void canvasResize();
	void resizeEvent(QResizeEvent *event);

signals:
	void sTextSet(QString, int);
	void sSizeSet(QSize, int);
	void sPosSet(QPoint, int);

protected:
	QWidget* par;
	QSize parSize;
	QSize oldParSize;
	
	QPoint offset;
	QPoint resizeOffset;
	
	float ratioHeight;
	float ratioWidth;
	
	bool dragEdge;
	QRect bottomRight;

	QBasicTimer timer;

	int m_index = -1;
};

#endif // DRAGLABEL_H