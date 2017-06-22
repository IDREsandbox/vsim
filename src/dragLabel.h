#ifndef DRAGLABEL_H
#define DRAGLABEL_H

#include <QtWidgets/QWidget>
#include <QtWidgets>
#include <QtGui/QMouseEvent>
#include <iostream>
#include <QtCore/QBasicTimer>
#include "MainWindow.h"

class MainWindow;

class dragLabel : public QLabel
{
	Q_OBJECT

public:
	dragLabel(QWidget* parent, std::string style, MainWindow* window);
	dragLabel(std::string str, std::string style, QWidget* parent, MainWindow* window);
	~dragLabel();

	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void timerEvent(QTimerEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

	void updateParSize();
	void mainResize();
	void resizeEvent(QResizeEvent* event);

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

	MainWindow* m_window;
};

#endif // DRAGLABEL_H