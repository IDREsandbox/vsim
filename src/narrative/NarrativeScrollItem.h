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
	NarrativeScrollItem();
	NarrativeScrollItem(const std::string& title, const std::string& description);
	//NarrativeScrollItem(const Narrative&);
	//void setInfo(const Narrative&);

	void setInfo(const std::string& title, const std::string& description);

private:
	QLabel *m_title_label;
	QLabel *m_desc_label;
	QVBoxLayout *m_layout;
	
};

#endif