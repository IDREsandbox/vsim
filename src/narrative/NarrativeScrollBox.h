#ifndef NARRATIVESCROLLBOX_H
#define NARRATIVESCROLLBOX_H

#include "HorizontalScrollBox.h"
//#include "NarrativeScrollItem.h"

class NarrativeScrollBox : public HorizontalScrollBox {
	Q_OBJECT

public:
	NarrativeScrollBox(QWidget * parent = nullptr);
	~NarrativeScrollBox();

	virtual void contextMenuEvent(QContextMenuEvent *event);

	void addItem(const std::string &title, const std::string &description);

signals:
	void sDoubleClick();
	void sNew();
	void sDeleted();
	//void sDeleted(int index);

private:
	QMenu* m_slide_menu; // context menu
	QAction* m_action_new;
	QAction* m_action_delete;
	QAction* m_action_edit;

	
};

#endif // NARRATIVESCROLLBOX_H