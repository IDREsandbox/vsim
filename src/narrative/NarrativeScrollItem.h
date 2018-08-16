#ifndef NARRATIVESCROLLITEM_H
#define NARRATIVESCROLLITEM_H

#include "ui_NarrativeScrollItem.h"
#include "ScrollBoxItem.h"

class LockTable;
class Narrative;

class NarrativeScrollItem : public ScrollBoxItem {
	Q_OBJECT

public:
	NarrativeScrollItem(QWidget *parent);
	void setNarrative(Narrative *narrative);

	virtual int widthFromHeight(int height) const;

	// scales the title to fit
	void fitTitle();

protected:
	void paintEvent(QPaintEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;

private:
	Ui::NarrativeScrollItem ui;
	Narrative *m_narrative;
	LockTable *m_lock;
	QPixmap m_lock_pixmap;
};

#endif // NARRATIVESCROLLITEM_H