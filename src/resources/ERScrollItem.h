#ifndef ERSCROLLITEM_H
#define ERSCROLLITEM_H
#include <QWidget>
#include "ui_ERScrollItem.h"
#include "ScrollBoxItem.h"
#include "resources/EResource.h"

class ERScrollItem : public ScrollBoxItem {
	Q_OBJECT
	
public:
	EResource *m_er;

	ERScrollItem(QWidget *parent);	void setER(EResource *er);

	virtual int widthFromHeight(int height);

protected:
	void mouseDoubleClickEvent(QMouseEvent *event);

signals:
	void sDoubleClick();

private:
	Ui::ERScrollItem ui;
};

#endif // ERSCROLLITEM_H