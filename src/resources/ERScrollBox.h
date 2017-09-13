#ifndef ERSCROLLBOX_H
#define ERSCROLLBOX_H

#include "HorizontalScrollBox.h"
#include "resources/ERScrollItem.h"
#include "resources/EResourceGroup.h"

class ERScrollBox : public HorizontalScrollBox {
	Q_OBJECT

public:
	ERScrollBox(QWidget * parent = nullptr);
	~ERScrollBox();

	ScrollBoxItem *createItem(osg::Node *node) override;
	void setSelection(std::set<int> set, int last) override;
	void filter(int type);

signals:
	void sNew();
	void sDelete();
	void sEdit(); // only support single edit
	void sOpen();

private:
	QMenu* m_slide_menu; // context menu
	QAction* m_action_new;
	QAction* m_action_delete;
	QAction* m_action_edit;
	QAction* m_action_open;

	//int filter; // 0 global, 1 local, 2 
};

#endif // ERSCROLLBOX_H