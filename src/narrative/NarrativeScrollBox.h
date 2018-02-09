﻿#ifndef NARRATIVESCROLLBOX_H
#define NARRATIVESCROLLBOX_H

#include "GroupScrollBox.h"

class NarrativeScrollBox : public GroupScrollBox {
	Q_OBJECT

public:
	NarrativeScrollBox(QWidget * parent = nullptr);

	ScrollBoxItem *createItem(osg::Node *node) override;

signals:
	void sInfo();
	void sOpen();
};

#endif // NARRATIVESCROLLBOX_H