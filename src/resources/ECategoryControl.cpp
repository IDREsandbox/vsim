#include "resources/ECategoryControl.h"

#include "MainWindow.h"
#include "resources/EResource.h"

#include "resources/ECategoryModel.h"
#include "EditDeleteDelegate.h"
#include "CheckableListProxy.h"

#include "resources/NewCatDialog.h"

ECategoryControl::ECategoryControl(MainWindow * window, QObject * parent)
	: QObject(parent)
{
	m_undo_stack = window->m_undo_stack;

	m_category_model = new ECategoryModel(this); // -> group
	m_category_sort_proxy = new QSortFilterProxyModel(this); // -> model
	m_category_sort_proxy->setSourceModel(m_category_model);
	m_category_sort_proxy->setSortRole(Qt::DisplayRole);
	m_category_sort_proxy->setDynamicSortFilter(true);
	m_category_sort_proxy->sort(0);
	m_category_checkbox_proxy = new CheckableListProxy(this); // -> sort proxy
	m_category_checkbox_proxy->setSourceModel(m_category_sort_proxy);

	load(nullptr);

	//QListView *lv1 = new QListView(m_window);
	//lv1->setModel(m_category_model);
	//lv1->setGeometry(50, 50, 100, 200);
	//lv1->show();
	//QListView *lv2 = new QListView(m_window);
	//lv2->setModel(m_category_sort_proxy);
	//lv2->setGeometry(150, 50, 100, 200);
	//lv2->show();
	//QListView *lv3 = new QListView(m_window);
	//lv3->setModel(m_category_checkbox_proxy);
	//lv3->setGeometry(250, 50, 100, 200);
	//lv3->show();

}

void ECategoryControl::load(ECategoryGroup * cats)
{
	//if (m_categories.get()) disconnect(m_categories.get(), 0, this, 0);
	// TODO: something with null?
	m_categories = cats;
	m_category_model->setGroup(cats);
}

void ECategoryControl::execDeleteCategory(QAbstractItemModel * model, const QModelIndex & index)
{
	QMessageBox::StandardButton reply =
		QMessageBox::question(
			nullptr,
			"Delete Category",
			"Are you sure you want to delete this category?",
			QMessageBox::Yes | QMessageBox::No);
	if (reply != QMessageBox::Yes) return;

	ECategory *cat =
		dynamic_cast<ECategory*>(
			static_cast<osg::Node*>(
				model->data(index, GroupModel::PointerRole).value<void*>()));

	if (!cat) return;

	m_undo_stack->beginMacro("Delete Category");
	std::set<EResource*> resources = cat->resources();
	for (auto res : resources) {
		// null out resource categories so that they get restored on undo
		m_undo_stack->push(new EResource::SetCategoryCommand(res, nullptr));
	}
	m_undo_stack->push(new Group::DeleteNodeCommand(m_categories, m_categories->indexOf(cat)));
	m_undo_stack->endMacro();
}

ECategory *ECategoryControl::execEditCategory(QAbstractItemModel * model, const QModelIndex & index)
{
	ECategory *cat =
		dynamic_cast<ECategory*>(
			static_cast<osg::Node*>(
				model->data(index, GroupModel::PointerRole).value<void*>()));
	if (!cat) return nullptr;
	NewCatDialog dlg("Edit Category");
	dlg.setColor(cat->getColor());
	dlg.setTitle(QString::fromStdString(cat->getCategoryName()));
	dlg.exec();

	m_undo_stack->beginMacro("Edit Category");
	m_undo_stack->push(new ECategory::SetCategoryNameCommand(cat, dlg.getCatTitle().toStdString()));
	m_undo_stack->push(new ECategory::SetColorCommand(cat, dlg.getColor()));
	m_undo_stack->endMacro();
	return cat;
}

ECategory *ECategoryControl::execNewCategory()
{
	NewCatDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return nullptr;
	}

	ECategory *category = new ECategory();
	category->setCategoryName(dlg.getCatTitle().toStdString());
	category->setColor(dlg.getColor());
	m_categories->addChild(category);
	return category;
}

QAbstractItemModel * ECategoryControl::categoryModel() const
{
	return m_category_sort_proxy;
}
