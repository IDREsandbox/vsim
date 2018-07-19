#ifndef NARRATIVESCROLLBOX_H
#define NARRATIVESCROLLBOX_H

#include "GroupScrollBox.h"

class Narrative;

class NarrativeScrollBox : public GroupScrollBox<Narrative> {
	Q_OBJECT

public:
	NarrativeScrollBox(QWidget * parent = nullptr);

	ScrollBoxItem *createItem(Narrative *node) override;

protected:
	void itemMouseDoubleClickEvent(QMouseEvent *event, int index) override;

	void paintEvent(QPaintEvent *e) override;

signals:
	void sOpen();
};

#endif // NARRATIVESCROLLBOX_H