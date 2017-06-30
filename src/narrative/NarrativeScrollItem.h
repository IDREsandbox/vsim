#ifndef NARRATIVESCROLLITEM_H
#define NARRATIVESCROLLITEM_H
#include <QWidget>
#include "ui_NarrativeScrollItem.h"
#include "ScrollBoxItem.h"

class NarrativeScrollItem : public ScrollBoxItem {
	Q_OBJECT

public:
	NarrativeScrollItem();

	void setInfo(const std::string &title, const std::string &description);

private:
	Ui::NarrativeScrollItem ui;
};

#endif // NARRATIVESCROLLITEM_H