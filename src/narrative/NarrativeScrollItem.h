#ifndef NARRATIVESCROLLITEM_H
#define NARRATIVESCROLLITEM_H
#include <QWidget>
#include "ui_NarrativeScrollItem.h"
#include "ScrollBoxItem.h"
#include "narrative/Narrative.h"

class NarrativeScrollItem : public ScrollBoxItem {
	Q_OBJECT

public:
	NarrativeScrollItem(QWidget *parent);
	void setNarrative(Narrative *narrative);

	virtual int widthFromHeight(int height) const;

private:
	Ui::NarrativeScrollItem ui;
	Narrative *m_narrative;
};

#endif // NARRATIVESCROLLITEM_H