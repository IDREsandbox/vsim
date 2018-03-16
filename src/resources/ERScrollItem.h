#ifndef ERSCROLLITEM_H
#define ERSCROLLITEM_H
#include <QWidget>
#include "ui_ERScrollItem.h"
#include "ScrollBoxItem.h"
#include "resources/EResource.h"

class ERScrollItem : public ScrollBoxItem {
	Q_OBJECT
	
public:
	ERScrollItem(QWidget *parent);

	void setER(EResource *er);
	void setCat(ECategory *cat);

	virtual int widthFromHeight(int height);

	void setColor(QColor color);

	static QString colorString(QColor color);

private:
	Ui::ERScrollItem ui;
	EResource *m_er;
	ECategory *m_cat;
};

#endif // ERSCROLLITEM_H