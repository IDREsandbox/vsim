#ifndef NARRATIVESCROLLITEM_H
#define NARRATIVESCROLLITEM_H
#include <QWidget>
#include "ui_NarrativeScrollItem.h"
#include "ScrollBoxItem.h"
#include "narrative/Narrative2.h"

class NarrativeScrollItem : public ScrollBoxItem {
	Q_OBJECT

public:
	NarrativeScrollItem(QWidget *parent);
	void setNarrative(Narrative2 *narrative);

	virtual int widthFromHeight(int height) const;

signals:
	void sDoubleClick();

private:
	Ui::NarrativeScrollItem ui;
	Narrative2 *m_narrative;
};

#endif // NARRATIVESCROLLITEM_H