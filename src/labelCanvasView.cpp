#include "labelCanvasView.h"
#include "labelCanvas.h"

labelCanvasView::labelCanvasView(QWidget* parent, labelCanvas* canvas)
{
	m_par = parent;
	m_canvas = canvas;

	scaleFactor = float(float(this->size().height()) / float(720));

	m_scene = new QGraphicsScene(parent);
	this->setScene(m_scene);

	this->setStyleSheet("background: transparent");
	m_canvas->setStyleSheet("background: transparent");

	this->setAttribute(Qt::WA_TransparentForMouseEvents, true);

	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	m_scene->addWidget(m_canvas);
}


labelCanvasView::~labelCanvasView()
{
}

void labelCanvasView::resizeEvent(QResizeEvent* event)
{
	scaleFactor = float(float(this->size().height()) / float(720));

	this->resetMatrix();
	this->scale(scaleFactor, scaleFactor);
}

