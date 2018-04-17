﻿#include "resources/ERFilterArea.h"

#include <QStandardItemModel>

#include "resources/ERFilterSortProxy.h"
#include "CheckableListProxy.h"
#include "EditDeleteDelegate.h"

ERFilterArea::ERFilterArea(QWidget *parent)
	: QFrame(parent),
	m_model(nullptr),
	m_category_checkbox_model(nullptr)
{
	ui.setupUi(this);

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
	QStandardItemModel *type_model = new QStandardItemModel(this);
	m_type_model = type_model;
	for (const char *s : {
		"hello",
		"world",
		"something"
		}) {
		type_model->appendRow(new QStandardItem(s));
	}
	m_type_checkbox_model = new CheckableListProxy(this);
	m_type_checkbox_model->setSourceModel(type_model);
	ui.filetypesBox->setModel(m_type_checkbox_model);

	connect(ui.closeButton, &QPushButton::pressed, this, &QWidget::hide);
	connect(ui.searchClearButton, &QPushButton::pressed, ui.searchLineEdit, &QLineEdit::clear);
	//connect(ui.clearButton, &QAbstractButton::pressed, this, &ERFilterArea::reset);

	reset();
}

void ERFilterArea::setModel(ERFilterSortProxy * model)
{
	m_model = model;
}

void ERFilterArea::setCategoryModel(CheckableListProxy * categories)
{
	ui.categoriesBox->setModel(categories);
	m_category_checkbox_model = categories;
}

void ERFilterArea::reset()
{
	ui.globalCheckBox->setChecked(true);
	ui.localCheckBox->setChecked(true);
	ui.yearsCheckBox->setChecked(true);
	ui.showLocalCheckBox->setChecked(false);
	ui.searchLineEdit->clear();
	ui.sortByBox->setCurrentIndex(0);
	if (m_category_checkbox_model) m_category_checkbox_model->setCheckAll(true);
	if (m_type_checkbox_model) m_type_checkbox_model->setCheckAll(true);
	ui.filetypesBox->setCurrentText("");
}