#include "labelCanvasView.h"
#include "labelCanvas.h"

#include "narrative/NarrativeSlide.h"

labelCanvasView::labelCanvasView(QWidget *parent)
	: QGraphicsView(parent),
	m_base_height(720)
{
	m_scene = new QGraphicsScene(parent);
	m_canvas = new labelCanvas(this);
	m_fadeout_canvas = new labelCanvas(this);
	this->setScene(m_scene);

	this->setStyleSheet("background: transparent");
	m_canvas->setStyleSheet("background: transparent");
	m_fadeout_canvas->setStyleSheet("background: transparent");

	this->setAttribute(Qt::WA_TransparentForMouseEvents, true);

	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	m_scene->addWidget(m_canvas);
	m_scene->addWidget(m_fadeout_canvas);

	m_fade = new QGraphicsOpacityEffect(this);
	m_fade->setOpacity(1.0);
	setGraphicsEffect(m_fade);
	m_fade_anim = new QPropertyAnimation(m_fade, "opacity", this);
	m_fade_anim->setDuration(250);
	m_fade_anim->setStartValue(0);
	m_fade_anim->setEndValue(1);
	m_fade_anim->setEasingCurve(QEasingCurve::Linear);

	m_fadeout_anim = new QPropertyAnimation(m_fade, "opacity", this);
	m_fadeout_anim->setDuration(250);
	m_fadeout_anim->setStartValue(1);
	m_fadeout_anim->setEndValue(0);
	m_fadeout_anim->setEasingCurve(QEasingCurve::Linear);

	// when fadeout is finished, hide the canvas and set to null
	connect(m_fadeout_anim, &QAbstractAnimation::finished, this,
		[this]() {
		m_fadeout_canvas->hide();
		m_fadeout_canvas->setSlide(nullptr);
	});
}

labelCanvas * labelCanvasView::canvas() const
{
	return m_canvas;
}

void labelCanvasView::setSlide(NarrativeSlide *slide, bool instant)
{
	// stop any animations
	m_fade_anim->stop();
	m_fadeout_anim->stop();

	if (!instant) {
		fadeOut(slide);
	}
	else {
		m_fadeout_canvas->setSlide(nullptr);
		m_fadeout_canvas->hide();
	}

	if (slide == nullptr) {
		m_canvas->hide();
	}
	else {
		m_canvas->setSlide(slide);
		m_canvas->show();
	}
}

void labelCanvasView::fadeOut(NarrativeSlide *slide)
{
	m_fadeout_canvas->setSlide(slide);
	m_fadeout_canvas->show();
	m_fadeout_anim->start();
}

void labelCanvasView::showCanvas(bool instant)
{
	m_canvas->show();
	if (!instant) {
		m_fade_anim->start();
	}
}

void labelCanvasView::hideCanvas(bool instant)
{
	m_canvas->hide();
	m_fade_anim->stop();
	if (!instant) {
		fadeOut(m_canvas->getSlide());
	}
}

void labelCanvasView::resizeEvent(QResizeEvent* event)
{
	float factor = float(float(this->size().height()) / float(m_base_height));

	this->resetMatrix();
	this->scale(factor, factor);
}

void labelCanvasView::keyPressEvent(QKeyEvent * e)
{
	qDebug() << "view key press event";
	QGraphicsView::keyPressEvent(e);
}
