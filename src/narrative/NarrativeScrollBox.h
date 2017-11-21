#ifndef NARRATIVESCROLLBOX_H
#define NARRATIVESCROLLBOX_H

#include "HorizontalScrollBox.h"
#include "NarrativeScrollItem.h"
#include "narrative/NarrativeGroup.h"

class NarrativeScrollBox : public HorizontalScrollBox {
	Q_OBJECT

public:
	NarrativeScrollBox(QWidget * parent = nullptr);
	~NarrativeScrollBox();

	ScrollBoxItem *createItem(osg::Node *node) override;

signals:
	void sInfo();
	void sOpen();
};

#endif // NARRATIVESCROLLBOX_H