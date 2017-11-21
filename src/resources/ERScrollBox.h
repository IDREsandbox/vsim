#ifndef ERSCROLLBOX_H
#define ERSCROLLBOX_H

#include "HorizontalScrollBox.h"
#include "resources/ERScrollItem.h"
#include "resources/EResourceGroup.h"

class ERScrollBox : public HorizontalScrollBox {
	Q_OBJECT

public:
	ERScrollBox(QWidget * parent = nullptr);
	~ERScrollBox();

	ScrollBoxItem *createItem(osg::Node *node) override;

	void filter(int type);

signals:
	void sEdit(); // only support single edit
	void sOpen();
	void sSetPosition();
	void sGotoPosition();

	//int filter; // 0 global, 1 local, 2 
};

#endif // ERSCROLLBOX_H