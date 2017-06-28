#ifndef SLIDESCROLLBOX_H
#define SLIDESCROLLBOX_H

#include <set>

#include "HorizontalScrollBox.h"
#include "SlideScrollItem.h"

class SlideScrollBox : public HorizontalScrollBox {
	Q_OBJECT

public:
	SlideScrollBox(QWidget * parent = nullptr);

	// controller interface
	void addItem();

	std::set<int> getSlideSelection();
	//std::set<int> getTransitionSelection(); // will always be the same as slide selection

	void setDuration(float t); // < 0 is forever
	void setTranstionDuration(float t); // <= 0 is instantaneous

	// virtual overrides
	virtual void openMenu(QPoint globalPos);
	virtual void openItemMenu(QPoint globalPos);

signals:
	void sSetTransitionDuration();
	void sSetDuration();
	void sDeleteSlides();
	void sNewSlide();

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	// mouse event handling
	void handleSlideMouseEvent(QMouseEvent *event);
	void handleTransitionMouseEvent(QMouseEvent *event);

	// slide menu
	QMenu *m_bar_menu;
	QMenu *m_slide_menu; // context menu
	QAction *m_action_new;
	QAction *m_action_delete;
	QAction *m_action_edit;
	QAction *m_action_set_camera;
	QAction *m_action_set_duration;
	QAction *m_action_set_transition;

};


#endif // SLIDESCROLLBOX_H