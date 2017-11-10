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

private:
	QMenu *m_slide_menu; // context menu
	QAction *m_action_new;
	QAction *m_action_delete;
	QAction *m_action_info;
	QAction *m_action_open;
};

#endif // NARRATIVESCROLLBOX_H