﻿#ifndef ERFILTERAREA_H
#define ERFILTERAREA_H
#include <QFrame>
#include <QListView>
#include <QAbstractItemModel>
#include <memory>

#include "EREnums.h"

class EditDeleteDelegate;
class CheckableListProxy;

namespace Ui { class ERFilterArea; };

class ERFilterArea : public QFrame {
	Q_OBJECT
public:
	ERFilterArea(QWidget *parent);
	void setCategoryModel(CheckableListProxy *categories);

	void reset();

	void setSortGlobal(ER::SortBy sort);
	void setSortLocal(ER::SortBy sort);
	void enableRange(bool enable);
	void setRadius(float radius);
	void enableYears(bool enable);

signals:
	void sSortGlobal(ER::SortBy sort);
	void sSortLocal(ER::SortBy sort);
	void sEnableRange(bool enable);
	void sSetRadius(float radius);
	void sEnableYears(bool enable);
	void sClear();

private:
	std::unique_ptr<Ui::ERFilterArea> ui;

	//EditDeleteDelegate *m_category_delegate;
	//QListView *m_category_view;
	CheckableListProxy *m_category_checkbox_model;

	//QListView *m_type_view;
	QAbstractItemModel *m_type_model;
	CheckableListProxy *m_type_checkbox_model;
};

#endif