#ifndef LABELCANVASVIEW_H
#define LABELCANVASVIEW_H

#include <QtWidgets>
#include <QtGui>
#include <QtCore>
#include <QList>
#include <QGraphicsOpacityEffect>

class labelCanvas;
class NarrativeSlide;

// QGraphicsView gives a simple way to resize slides and their text labels. We tried things like painting a scaled QTextDocument and messing with QTextEdit but it was no fun. This could have been inside Canvas but this was just easier.
class labelCanvasView : public QGraphicsView
{
	Q_OBJECT;

public:
	labelCanvasView(QWidget* parent = nullptr);

	labelCanvas *canvas() const;

	void setSlide(NarrativeSlide *slide, bool instant);
	void showCanvas(bool instant);
	void hideCanvas(bool instant);

	// plays a fade out animation for an arbitrary slide
	void fadeOut(NarrativeSlide *slide);

	void resizeEvent(QResizeEvent* event);

	void keyPressEvent(QKeyEvent *e) override;

private:
	labelCanvas *m_canvas;
	labelCanvas *m_fadeout_canvas;
	QGraphicsScene* m_scene;

	float m_base_height;

	QGraphicsOpacityEffect *m_fade;
	QPropertyAnimation *m_fade_anim;
	QPropertyAnimation *m_fadeout_anim;
};
#endif // LABELCANVASVIEW_H