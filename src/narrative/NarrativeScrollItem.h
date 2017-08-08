#ifndef NARRATIVESCROLLITEM_H
#define NARRATIVESCROLLITEM_H
#include <QWidget>
#include "ui_NarrativeScrollItem.h"
#include "ScrollBoxItem.h"
#include "narrative/Narrative2.h"

class NarrativeScrollItem : public ScrollBoxItem {
	Q_OBJECT

public:
	NarrativeScrollItem();
	NarrativeScrollItem(Narrative2 *narrative);
	void setNarrative(Narrative2 *narrative);

protected:
	void mousePressEvent(QMouseEvent *event);

signals:
	void sDoubleClick();

private:
	Ui::NarrativeScrollItem ui;
	Narrative2 *m_narrative;
	QList<QMetaObject::Connection> m_connections;
};

#endif // NARRATIVESCROLLITEM_H