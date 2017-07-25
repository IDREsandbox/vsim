#ifndef NARRATIVESCROLLBOX_H
#define NARRATIVESCROLLBOX_H

#include "HorizontalScrollBox.h"
#include "NarrativeScrollItem.h"

class NarrativeScrollBox : public HorizontalScrollBox {
	Q_OBJECT

public:
	NarrativeScrollBox(QWidget * parent = nullptr);
	~NarrativeScrollBox();

	void addItem(const std::string &title, const std::string &description);

	virtual void openMenu(QPoint globalPos);
	virtual void openItemMenu(QPoint globalPos);

signals:
	void sNew();
	void sDelete();
	void sInfo();
	void sOpen();

private:
	QMenu* m_slide_menu; // context menu
	QAction* m_action_new;
	QAction* m_action_delete;
	QAction* m_action_info;
	QAction* m_action_open;
};

#endif // NARRATIVESCROLLBOX_H