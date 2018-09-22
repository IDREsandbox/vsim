#ifndef SLIDESCROLLBOX_H
#define SLIDESCROLLBOX_H

#include <set>
#include <QDialog>
#include <QInputDialog>

#include "GroupScrollBox.h"
#include "NarrativeSlideDurationDialog.h"

class NarrativeSlide;
class SlideScrollBox : public GroupScrollBox<NarrativeSlide> {
	Q_OBJECT

public:
	SlideScrollBox(QWidget * parent = nullptr);

	ScrollBoxItem *createItem(NarrativeSlide *slide) override;

	//std::vector<NarrativeSlide*> getDirtySlides(); // slides whose thumbnails need to be drawn

signals:
	void sSetTransitionDuration();
	void sSetDuration(); // 0 if pause at node
	void sGoto(int);
	//void sTransitionTo(int);

	//void sThumbnailsDirty();

protected:
	void paintEvent(QPaintEvent *event) override;

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