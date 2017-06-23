#ifndef NARRATIVESCROLLBOX_HPP
#define NARRATIVESCROLLBOX_HPP

#include "HorizontalScrollBox.h"

class NarrativeScrollBox : public HorizontalScrollBox {
	Q_OBJECT

public:
	NarrativeScrollBox(QWidget * parent = nullptr);
	~NarrativeScrollBox();

	virtual void contextMenuEvent(QContextMenuEvent *event);

	void addItem(const std::string &title, const std::string &description);

private:
	QMenu* m_slide_menu; // context menu
	QAction* m_action_new;
	QAction* m_action_delete;
	QAction* m_action_edit;

	
};

#endif // NARRATIVESCROLLBOX_HPP