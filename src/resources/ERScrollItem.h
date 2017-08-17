#ifndef ERSCROLLITEM_H
#define ERSCROLLITEM_H
#include <QWidget>
#include "ui_ERScrollItem.h"
#include "ScrollBoxItem.h"
#include "resources/EResourcesNode.h"

class ERScrollItem : public ScrollBoxItem {
	Q_OBJECT

public:
	ERScrollItem();
	ERScrollItem(EResourcesNode *er);
	void setER(EResourcesNode *er);

	virtual int widthFromHeight(int height);

protected:
	void mouseDoubleClickEvent(QMouseEvent *event);

signals:
	void sDoubleClick();

private:
	Ui::ERScrollItem ui;
	EResourcesNode *m_er;
};

#endif // ERSCROLLITEM_H