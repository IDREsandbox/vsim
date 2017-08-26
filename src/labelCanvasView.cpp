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

	m_fade = new QGraphicsOpacityEffect(this);
	m_fade->setOpacity(1.0);
	setGraphicsEffect(m_fade);
	m_fade_anim = new QPropertyAnimation(m_fade, "opacity", this);
	m_fade_anim->setDuration(500);
	m_fade_anim->setStartValue(0);
	m_fade_anim->setEndValue(1);
	m_fade_anim->setEasingCurve(QEasingCurve::InBack);
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

void labelCanvasView::keyPressEvent(QKeyEvent * e)
{
	qDebug() << "view key press event";
	QGraphicsView::keyPressEvent(e);
}

void labelCanvasView::fadeIn()
{
	if (!isHidden()) return;
	show();
	m_fade_anim->start();
}

void labelCanvasView::hideEvent(QHideEvent * e)
{
	QGraphicsView::hideEvent(e);
}

