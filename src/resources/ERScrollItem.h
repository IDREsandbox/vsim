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

	void showTypeIcon(EResource::ERType);
	void showGotoIcon(bool show);
	void showAutoLaunchIcon(bool show);
	void setDistance(double dist);

private:
	Ui::ERScrollItem ui;
	EResource *m_er;
	ECategory *m_cat;

	QWidget *m_icon_container;
	QLabel *m_type_icon;
	QLabel *m_goto_icon;
	QLabel *m_launch_icon;
	QLabel *m_distance_label;
};

#endif // ERSCROLLITEM_H