#ifndef ERSCROLLBOX_H
#define ERSCROLLBOX_H

#include "GroupScrollBox.h"

class EResource;
class ERScrollBox : public GroupScrollBox<EResource> {
	Q_OBJECT

public:
	ERScrollBox(QWidget * parent = nullptr);

	ScrollBoxItem *createItem(EResource *er) override;

protected:
	void itemMouseDoubleClickEvent(QMouseEvent * event, int index) override;

signals:
	void sOpen();
};

#endif // ERSCROLLBOX_H