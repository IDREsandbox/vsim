#include "resources/ERFilterArea.h"
#include "ui_ERFilterArea.h"

#include <QStandardItemModel>

//#include "resources/ERFilterSortProxy.h"
#include "CheckableListProxy.h"
#include "EditDeleteDelegate.h"

ERFilterArea::ERFilterArea(QWidget *parent)
	: QFrame(parent),
	m_category_checkbox_model(nullptr)
{
	ui = std::make_unique<Ui::ERFilterArea>();
	ui->setupUi(this);

	int nsorts = sizeof(ER::SortByStrings) / sizeof(*ER::SortByStrings);
	for (int i = 0; i < nsorts; i++) {
		QString s = ER::SortByStrings[i];
		ui->sortGlobalBox->addItem(s);
		ui->sortLocalBox->addItem(s);
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
	//connect(ui.clearButton, &QAbstractButton::pressed, this, &ERFilterArea::reset);

	// signals out
	connect(ui->sortLocalBox, QOverload<int>::of(&QComboBox::activated), this,
		[this](int value) {
		emit sSortLocal((ER::SortBy)value);
	});
	connect(ui->sortGlobalBox, QOverload<int>::of(&QComboBox::activated), this,
		[this](int value) {
		emit sSortGlobal((ER::SortBy)value);
	});
	connect(ui->clearButton, &QAbstractButton::pressed, this, &ERFilterArea::sClear);
	connect(ui->showLocalCheckBox, &QAbstractButton::clicked, this,
		[this](bool checked) {
		emit sEnableRange(!checked);
	});
	connect(ui->yearsCheckBox, &QAbstractButton::clicked, this,
		[this](bool checked) {
		emit sEnableYears(checked);
	});
	connect(ui->radiusSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, [this](double value) {
		emit sSetRadius(value);
	});

	reset();
}

void ERFilterArea::setCategoryModel(CheckableListProxy * categories)
{
	ui->categoriesBox->setModel(categories);
	m_category_checkbox_model = categories;
}

void ERFilterArea::reset()
{
	//ui->globalCheckBox->setChecked(true);
	//ui->localCheckBox->setChecked(true);
	//ui->yearsCheckBox->setChecked(true);
	//ui->showLocalCheckBox->setChecked(false);
	//ui->searchLineEdit->clear();
	//ui->sortGlobalBox->setCurrentIndex(0);
	//ui->sortLocalBox->setCurrentIndex(0);
	//if (m_category_checkbox_model) m_category_checkbox_model->setCheckAll(true);
	//if (m_type_checkbox_model) m_type_checkbox_model->setCheckAll(true);
	//ui->filetypesBox->setCurrentText("");
}

void ERFilterArea::setSortGlobal(ER::SortBy sort)
{
	ui->sortGlobalBox->setCurrentIndex((int)sort);
}

void ERFilterArea::setSortLocal(ER::SortBy sort)
{
	ui->sortLocalBox->setCurrentIndex((int)sort);
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
