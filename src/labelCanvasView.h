#ifndef LABELCANVASVIEW_H
#define LABELCANVASVIEW_H

#include <QtWidgets>
#include <QtGui>
#include <QtCore>
#include <QList>
#include "labelCanvas.h"

class labelCanvas;

class labelCanvasView : public QGraphicsView
{
	Q_OBJECT

public:
	labelCanvasView(QWidget* parent, labelCanvas* canvas);
	~labelCanvasView();

	void resizeEvent(QResizeEvent* event);

private:
	labelCanvas* m_canvas;
	QWidget* m_par;
	QGraphicsScene* m_scene;
	float scaleFactor = 1;
	float lastScaleFactor = 1;
};
#endif // LABELCANVASVIEW_H