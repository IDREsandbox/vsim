#ifndef SLIDESCROLLBOX_H
#define SLIDESCROLLBOX_H

#include "HorizontalScrollBox.h"
#include "SlideScrollItem.h"

class SlideScrollBox : public HorizontalScrollBox {
	Q_OBJECT

public:
	SlideScrollBox(QWidget * parent = nullptr);

	virtual void contextMenuEvent(QContextMenuEvent *event);

	void addItem();

private:
	QMenu *m_slide_menu; // context menu
	QAction *m_action_new;
	QAction *m_action_delete;
	QAction *m_action_edit;
	QAction *m_action_set_camera;
	
};

#endif // SLIDESCROLLBOX_H