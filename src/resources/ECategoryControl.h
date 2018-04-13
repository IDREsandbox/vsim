#ifndef ECATEGORYCONTROL_H
#define ECATEGORYCONTROL_H

#include <memory>
#include <QObject>
#include <QUndoStack>
#include <QSortFilterProxyModel>

class VSimApp;
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
	ECategoryControl(VSimApp *window, QObject *parent = nullptr);

	void load(ECategoryGroup *cats);

	void execDeleteCategory(QAbstractItemModel *model, const QModelIndex &index);
	ECategory *execEditCategory(QAbstractItemModel *model, const QModelIndex &index);
	std::shared_ptr<ECategory> execNewCategory();

	ECategoryModel *categoryModel() const;
	QAbstractItemModel *sortedCategoryModel() const;

private:
	VSimApp *m_app;
	ECategoryGroup *m_categories;

	QUndoStack *m_undo_stack;

	ECategoryModel *m_category_model; // -> group
	QSortFilterProxyModel *m_category_sort_proxy; // -> model
	CheckableListProxy *m_category_checkbox_proxy; // -> alphabetical proxy
};


#endif
