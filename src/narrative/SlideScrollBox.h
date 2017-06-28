#ifndef SLIDESCROLLBOX_H
#define SLIDESCROLLBOX_H

#include <set>

#include "HorizontalScrollBox.h"
#include "SlideScrollItem.h"

class SlideScrollBox : public HorizontalScrollBox {
	Q_OBJECT

public:
	SlideScrollBox(QWidget * parent = nullptr);

	virtual void contextMenuEvent(QContextMenuEvent *event);

	// controller interface
	void addItem();

	std::set<int> getSlideSelection();
	//std::set<int> getTransitionSelection(); // will always be the same as slide selection

	void setDuration(float t); // < 0 is forever
	void setTranstionDuration(float t); // <= 0 is instantaneous

signals:
	void sSetTransitionDuration();
	void sSetDuration();
	void sDeleteSlides();
	void sNewSlide();

private:

	// slide menu
	QMenu *m_slide_menu; // context menu
	QAction *m_action_new;
	QAction *m_action_delete;
	QAction *m_action_edit;
	QAction *m_action_set_camera;
	QAction *m_action_set_duration;

	// transition menu
	QMenu *m_transition_menu;
	QAction *m_action_set_transition_duration;

};

#endif // SLIDESCROLLBOX_H