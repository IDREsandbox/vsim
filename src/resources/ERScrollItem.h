#ifndef ERSCROLLITEM_H
#define ERSCROLLITEM_H
#include <QWidget>
#include "ui_ERScrollItem.h"
#include "ScrollBoxItem.h"
#include "resources/EResource.h"

class ERScrollItem : public ScrollBoxItem {
	Q_OBJECT
	
public:
	ERScrollItem();
	ERScrollItem(EResource *er);
	void setER(EResource *er);

	virtual int widthFromHeight(int height);

protected:
	void mouseDoubleClickEvent(QMouseEvent *event);

signals:
	void sDoubleClick();

private:
	Ui::ERScrollItem ui;
	EResource *m_er;
};

#endif // ERSCROLLITEM_H