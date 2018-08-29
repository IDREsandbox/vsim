#include "resources/ERFilterArea.h"
#include "ui_ERFilterArea.h"

#include <QStandardItemModel>

//#include "resources/ERFilterSortProxy.h"
#include "CheckableListProxy.h"
#include "Gui/EditDeleteDelegate.h"

ERFilterArea::ERFilterArea(QWidget *parent)
	: QFrame(parent),
	m_category_checkbox_model(nullptr)
{
	ui = std::make_unique<Ui::ERFilterArea>();
	ui->setupUi(this);
	setWindowTitle("Resource Filters");
	setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	int nsorts = sizeof(ER::SortByStrings) / sizeof(*ER::SortByStrings);
	for (int i = 0; i < nsorts; i++) {
		QString s = ER::SortByStrings[i];
		ui->sort_global_box->addItem(s);
		ui->sort_local_box->addItem(s);
		ui->sort_all_box->addItem(s);
	}

	// sort by
	//ui.sortByBox->addItem("Title");
	//ui.sortByBox->addItem("Year");
	//ui.sortByBox->addItem("Distance");

	// category view
	// ui.categoriesBox->setModel(m_category_model);

	//m_category_view = new QListView(ui.categoriesBox);
	//m_category_delegate = new EditDeleteDelegate(this);
	//m_category_view->setItemDelegate(m_category_delegate);
	//ui.categoriesBox->setView(m_category_view);

	// type view
	//QStandardItemModel *type_model = new QStandardItemModel(this);
	//m_type_model = type_model;
	//for (const char *s : {
	//	"hello",
	//	"world",
	//	"something"
	//	}) {
	//	type_model->appendRow(new QStandardItem(s));
	//}
	//m_type_checkbox_model = new CheckableListProxy(this);
	//m_type_checkbox_model->setSourceModel(type_model);
	//ui.filetypesBox->setModel(m_type_checkbox_model);

	connect(ui->closeButton, &QPushButton::pressed, this, &QWidget::hide);
	connect(ui->searchClearButton, &QPushButton::pressed, ui->searchLineEdit, &QLineEdit::clear);

	// signals out
	connect(ui->sort_all_box, QOverload<int>::of(&QComboBox::activated), this,
		[this](int value) {
		emit sSortAll((ER::SortBy)value);
	});
	connect(ui->sort_local_box, QOverload<int>::of(&QComboBox::activated), this,
		[this](int value) {
		emit sSortLocal((ER::SortBy)value);
	});
	connect(ui->sort_global_box, QOverload<int>::of(&QComboBox::activated), this,
		[this](int value) {
		emit sSortGlobal((ER::SortBy)value);
	});
	connect(ui->clearButton, &QAbstractButton::pressed, this, &ERFilterArea::sClear);
	connect(ui->showLocalCheckBox, &QAbstractButton::clicked, this,
		[this](bool checked) {
		emit sEnableRange(!checked);
	});
	connect(ui->yearsCheckBox, &QAbstractButton::clicked, this, &ERFilterArea::sEnableYears);
	connect(ui->enableAutoLaunchCheckBox, &QAbstractButton::clicked, this, &ERFilterArea::sEnableAutoLaunch);
	connect(ui->radiusSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, [this](double value) {
		emit sSetRadius(value);
	});
	connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &ERFilterArea::sSearch);

}

void ERFilterArea::setCategoryModel(CheckableListProxy * categories)
{
	ui->categories_box->setModel(categories);
	m_category_checkbox_model = categories;
}

void ERFilterArea::setSearch(const QString & s)
{
	ui->searchLineEdit->setText(s);
}

void ERFilterArea::setSortAll(ER::SortBy sort)
{
	ui->sort_all_box->setCurrentIndex((int)sort);
}

void ERFilterArea::setSortGlobal(ER::SortBy sort)
{
	ui->sort_global_box->setCurrentIndex((int)sort);
}

void ERFilterArea::setSortLocal(ER::SortBy sort)
{
	ui->sort_local_box->setCurrentIndex((int)sort);
}

void ERFilterArea::enableRange(bool enable)
{
	ui->showLocalCheckBox->setChecked(!enable);
}

void ERFilterArea::setRadius(float radius)
{
	ui->radiusSpinBox->setValue(radius);
}

void ERFilterArea::enableYears(bool enable)
{
	ui->yearsCheckBox->setChecked(enable);
}

void ERFilterArea::enableAutoLaunch(bool enable)
{
	ui->enableAutoLaunchCheckBox->setChecked(enable);
}

void ERFilterArea::setToAll(bool all)
{
	m_all = all;
	bool gl = !all;

	ui->sort_local_label->setVisible(gl);
	ui->sort_global_label->setVisible(gl);
	ui->sort_local_box->setVisible(gl);
	ui->sort_global_box->setVisible(gl);
	ui->sort_all_label->setVisible(all);
	ui->sort_all_box->setVisible(all);

	//adjustSize();
	// TODO: somehow get a layout request
}