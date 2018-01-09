#ifndef ERCONTROL_H
#define ERCONTROL_H

#include <QObject>
#include <osg/Node>
#include <QUndoStack>
#include <QSortFilterProxyModel>

class EResourceGroup;
class MainWindow;
class ECategoryGroup;
class ERDialog;
class ERDisplay;
class ERScrollBox;
class ERFilterSortProxy;
class ECategoryModel;
class CheckableListProxy;
class EditDeleteDelegate;

// manages which ER is active
// slots for creating a new ER, editing, etc
class ERControl : public QObject 
{
	Q_OBJECT;
public:
	ERControl(QObject *parent, MainWindow *window, EResourceGroup *ers);

	void execEditCategory(QAbstractItemModel *model, const QModelIndex &index);
	void execNewCategory(QAbstractItemModel *model, const QModelIndex &index);

	void load(ECategoryGroup *cats);

	// opens new cat dialog
	void newERCat();

	void debug();

private:
	std::set<int> getCombinedSelection();
	int getCombinedLastSelected();

private:
	osg::ref_ptr<ECategoryGroup> m_categories;

	QUndoStack *m_undo_stack;

	ECategoryModel *m_category_model; // -> group
	QSortFilterProxyModel *m_category_sort_proxy; // -> model
	CheckableListProxy *m_category_checkbox_proxy; // -> alphabetical proxy

	EditDeleteDelegate *m_category_delegate;

};


#endif /* ERCONTROL_H */
