#include "resources/ECategoryControl.h"

#include <QMessageBox>

#include "VSimApp.h"
#include "resources/EResource.h"
#include "resources/ECategory.h"
#include "resources/ECategoryGroup.h"
#include "resources/ECategoryModel.h"
#include "Gui/EditDeleteDelegate.h"
#include "CheckableListProxy.h"

#include "resources/NewCatDialog.h"
#include "Core/GroupCommands.h"
#include "Core/Util.h"

ECategoryControl::ECategoryControl(VSimApp *app, QObject * parent)
	: QObject(parent),
	m_app(app),
	m_categories(nullptr)
{
	m_undo_stack = app->getUndoStack();

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
	Util::reconnect(this, &m_categories, cats);
	m_category_model->setGroup(cats);
	m_categories = cats;

	if (!m_categories) return;
	m_category_sort_proxy;
}

void ECategoryControl::execDeleteCategory(QAbstractItemModel * model, const QModelIndex & index)
{
	if (m_categories->restrictedToCurrent()) return;

	QMessageBox::StandardButton reply =
		QMessageBox::question(
			nullptr,
			"Delete Category",
			"Are you sure you want to delete this category?",
			QMessageBox::Yes | QMessageBox::No);
	if (reply != QMessageBox::Yes) return;

	ECategory *cat = TGroupModel<ECategory>::get(model, index.row());
	if (!cat) return;

	auto *cmd = new QUndoCommand();
	cmd->setText("Delete Category");

	std::set<EResource*> resources = cat->resources();
	for (auto res : resources) {
		// null out resource categories so that they get restored on undo
		new EResource::SetCategoryCommand(res, nullptr, cmd);
	}
	new RemoveMultiCommand<ECategory>(m_categories, { (size_t)m_categories->indexOf(cat) }, cmd);

	m_undo_stack->push(cmd);
}

ECategory *ECategoryControl::execEditCategory(QAbstractItemModel * model, const QModelIndex & index)
{
	ECategory *cat = TGroupModel<ECategory>::get(model, index.row());
	if (!cat) return nullptr;

	NewCatDialog dlg("Edit Category");
	dlg.setColor(cat->getColor());
	dlg.setTitle(QString::fromStdString(cat->getCategoryName()));
	dlg.exec();

	auto *cmd = new QUndoCommand();
	cmd->setText("Edit Category");
	new ECategory::SetCategoryNameCommand(cat, dlg.getCatTitle().toStdString(), cmd);
	new ECategory::SetColorCommand(cat, dlg.getColor(), cmd);
	m_undo_stack->push(cmd);
	return cat;
}

std::shared_ptr<ECategory> ECategoryControl::execNewCategory()
{
	if (m_categories->restrictedToCurrent()) return nullptr;

	NewCatDialog dlg;
	int result = dlg.exec();
	if (result == QDialog::Rejected) {
		return nullptr;
	}

	auto category = std::make_shared<ECategory>();
	category->setCategoryName(dlg.getCatTitle().toStdString());
	category->setColor(dlg.getColor());
	m_categories->append(category);
	m_categories->sAnyChange();
	return category;
}

ECategoryModel *ECategoryControl::categoryModel() const
{
	return m_category_model;
}

QAbstractItemModel *ECategoryControl::sortedCategoryModel() const
{
	return m_category_sort_proxy;
}

