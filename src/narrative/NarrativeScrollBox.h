#ifndef NARRATIVESCROLLBOX_H
#define NARRATIVESCROLLBOX_H

#include "GroupScrollBox.h"

class NarrativeScrollBox : public GroupScrollBox {
	Q_OBJECT

public:
	NarrativeScrollBox(QWidget * parent = nullptr);

	ScrollBoxItem *createItem(osg::Node *node) override;

protected:
	void itemMouseDoubleClickEvent(QMouseEvent *event, int index) override;

signals:
	void sOpen();
};

#endif // NARRATIVESCROLLBOX_H