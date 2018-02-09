#ifndef ERSCROLLBOX_H
#define ERSCROLLBOX_H

#include "GroupScrollBox.h"

class ERFilterSortProxy;

class ERScrollBox : public GroupScrollBox {
	Q_OBJECT

public:
	ERScrollBox(QWidget * parent = nullptr);

	ScrollBoxItem *createItem(osg::Node *node) override;

signals:
	void sEdit(); // only support single edit
	void sOpen();
	void sSetPosition();
	void sGotoPosition();
};

#endif // ERSCROLLBOX_H