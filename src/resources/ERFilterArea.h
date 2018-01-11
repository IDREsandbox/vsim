#ifndef ERFILTERAREA_H
#define ERFILTERAREA_H
#include <QFrame>
#include <QListView>
#include <QAbstractItemModel>
#include "ui_ERFilterArea.h"

class EditDeleteDelegate;
class CheckableListProxy;
class ERFilterSortProxy;

class ERFilterArea : public QFrame {
	Q_OBJECT
public:
	ERFilterArea(QWidget *parent);
	void setModel(ERFilterSortProxy *model);
	void setCategoryModel(CheckableListProxy *categories);

	void reset();

private:
	Ui::ERFilterArea ui;

	ERFilterSortProxy *m_model;

	//EditDeleteDelegate *m_category_delegate;
	//QListView *m_category_view;
	CheckableListProxy *m_category_checkbox_model;

	//QListView *m_type_view;
	QAbstractItemModel *m_type_model;
	CheckableListProxy *m_type_checkbox_model;
};

#endif