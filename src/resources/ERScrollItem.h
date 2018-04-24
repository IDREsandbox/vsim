#ifndef ERSCROLLITEM_H
#define ERSCROLLITEM_H

#include "FastScrollBox.h"

class EResource;
class ECategory;

class ERScrollItem : public FastScrollItem {
	Q_OBJECT
	
public:
	ERScrollItem();

	enum { Type = UserType + 2 };
	int type() const override { return Type; }

	EResource *resource() const;
	void setER(EResource *er);
	void setCat(ECategory *cat);

	void updateAlias();

protected:
	void paint(QPainter *painter,
		const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
	EResource *m_er;
	ECategory *m_cat;

	QPixmap m_text_icon;
	QPixmap m_file_icon;
	QPixmap m_url_icon;
	QPixmap m_launch_icon;
	QPixmap m_goto_icon;
};

#endif