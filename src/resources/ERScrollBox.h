#ifndef ERSCROLLBOX_H
#define ERSCROLLBOX_H

#include "GroupScrollBox.h"

class ERFilterSortProxy;

class ERScrollBox : public GroupScrollBox {
	Q_OBJECT

public:
	ERScrollBox(QWidget * parent = nullptr);

	ScrollBoxItem *createItem(osg::Node *node) override;

protected:
	void itemMouseDoubleClickEvent(QMouseEvent * event, int index) override;

signals:
	void sOpen();
};

#endif // ERSCROLLBOX_H