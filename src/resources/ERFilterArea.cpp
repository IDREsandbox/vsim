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
	ui->closeButton->hide();

	// sort by
	for (size_t i = 0; i < std::size(ER::SortByStrings); i++) {
		QString s = ER::SortByStrings[i];
		ui->sort_global_box->addItem(s);
		ui->sort_local_box->addItem(s);
		ui->sort_all_box->addItem(s);
	}

	adjustSize();

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
	connect(ui->legend_checkbox, &QAbstractButton::clicked, this, &ERFilterArea::sShowLegend);
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

void ERFilterArea::setShowLegend(bool show)
{
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

	int index = all ? 1 : 0;
	ui->sort_stack->setCurrentIndex(index);
}