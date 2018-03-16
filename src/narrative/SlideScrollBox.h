#ifndef SLIDESCROLLBOX_H
#define SLIDESCROLLBOX_H

#include <set>
#include <QDialog>
#include <QInputDialog>

#include "GroupScrollBox.h"
#include "NarrativeSlideDurationDialog.h"

class NarrativeSlide;
class SlideScrollBox : public GroupScrollBox {
	Q_OBJECT

public:
	SlideScrollBox(QWidget * parent = nullptr);

	void setGroup(Group *group) override;
	ScrollBoxItem *createItem(osg::Node *node) override;

	std::vector<NarrativeSlide*> getDirtySlides(); // slides whose thumbnails need to be drawn

signals:
	void sSetTransitionDuration();
	void sSetDuration(); // 0 if pause at node
	void sGoto(int);
	//void sTransitionTo(int);

	void sThumbnailsDirty();

protected:
	void paintEvent(QPaintEvent *event);

	void itemMousePressEvent(QMouseEvent *event, int index) override;

private:
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