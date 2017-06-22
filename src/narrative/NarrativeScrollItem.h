#ifndef NARRATIVESCROLLITEM_H
#define NARRATIVESCROLLITEM_H

#include <QLabel>
#include <QVBoxLayout>
#include <string>
#include "ScrollBoxItem.h"
#include "Narrative.h"

class NarrativeScrollItem : public ScrollBoxItem {
	Q_OBJECT
public:
	NarrativeScrollItem(QWidget* parent);
	NarrativeScrollItem(const Narrative&);

	void setInfo(const Narrative&);

private:
	QLabel *m_title_label;
	QLabel *m_desc_label;
	QVBoxLayout *m_layout;
	
};

#endif