#ifndef SLIDESCROLLITEM_H
#define SLIDESCROLLITEM_H

#include <QLabel>
#include <QVBoxLayout>
#include <string>
#include "ScrollBoxItem.h"
#include "Narrative.h"

class SlideScrollItem : public ScrollBoxItem {
	Q_OBJECT
public:
	SlideScrollItem();

private:
	QLabel *m_number_label;
	QVBoxLayout *m_layout;
};

#endif