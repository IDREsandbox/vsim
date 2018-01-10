#ifndef ECATEGORYCONTROL_H
#define ECATEGORYCONTROL_H

#include <QObject>
#include <osg/Node>
#include <QUndoStack>
#include <QSortFilterProxyModel>

class MainWindow;
class EResource;
class ECategoryGroup;

class ECategory;
class ECategoryModel;
class CheckableListProxy;

// manages which ER is active
// slots for creating a new ER, editing, etc
class ECategoryControl : public QObject 
{
	Q_OBJECT
public:
	ECategoryControl(MainWindow *window, QObject *parent = nullptr);

	void load(ECategoryGroup *cats);

	void execDeleteCategory(QAbstractItemModel *model, const QModelIndex &index);
	ECategory *execEditCategory(QAbstractItemModel *model, const QModelIndex &index);
	ECategory *execNewCategory();

	QAbstractItemModel *categoryModel() const;

private:
	osg::ref_ptr<ECategoryGroup> m_categories;

	QUndoStack *m_undo_stack;

	ECategoryModel *m_category_model; // -> group
	QSortFilterProxyModel *m_category_sort_proxy; // -> model
	CheckableListProxy *m_category_checkbox_proxy; // -> alphabetical proxy
};


#endif
