#ifndef ERSCROLLBOX_H
#define ERSCROLLBOX_H

#include "HorizontalScrollBox.h"
#include "resources/ERScrollItem.h"
#include "resources/EResourceGroup.h"

class ERFilterSortProxy;

class ERScrollBox : public HorizontalScrollBox {
	Q_OBJECT

public:
	ERScrollBox(QWidget * parent = nullptr);
	~ERScrollBox();

	virtual ScrollBoxItem *createItem(osg::Node *node) override;

signals:
	void sEdit(); // only support single edit
	void sOpen();
	void sSetPosition();
	void sGotoPosition();
};

#endif // ERSCROLLBOX_H