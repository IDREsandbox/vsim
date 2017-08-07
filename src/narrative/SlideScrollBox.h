#ifndef SLIDESCROLLBOX_H
#define SLIDESCROLLBOX_H

#include <set>
#include <QDialog>
#include <QInputDialog>

#include "HorizontalScrollBox.h"
#include "SlideScrollItem.h"
#include "NarrativeSlideDurationDialog.h"
#include "Narrative2.h"

class SlideScrollBox : public HorizontalScrollBox {
	Q_OBJECT

public:
	SlideScrollBox(QWidget * parent = nullptr);

	// 
	void setNarrative(Narrative2 *narrative);

	// controller interface
	//SlideScrollItem *addItem();
	//SlideScrollItem *addItem(NarrativeSlide *slide);
	SlideScrollItem *getItem(int index);
	void newItem(int index);
	void insertNewSlide(int index, NarrativeSlide *slide);

	// gui display some dialogs
	// opens based on selection, emits signals
	//void transitionDialog();
	//void durationDialog();
	//float execTransitionDialog(float duration); // ?? on reject, >= 0 on accept with duration
	//float execDurationDialog(bool stay, float duration); // < 0 on reject, = 0 for hold, > 0 for timed slide duration

	// virtual overrides
	virtual void openMenu(QPoint globalPos);
	virtual void openItemMenu(QPoint globalPos);

	std::vector<SlideScrollItem*> getDirtySlides(); // slides whose thumbnails need to be drawn
	
signals:
	void sSetTransitionDuration();
	void sSetDuration(); // 0 if pause at node
	void sDeleteSlides();
	void sNewSlide(int);
	void sEditSlide();
	void sSetCamera();

	void sThumbnailsDirty();

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	Narrative2 *m_narrative;

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