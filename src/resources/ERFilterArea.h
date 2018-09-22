#ifndef ERFILTERAREA_H
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
	~ERFilterArea();
	void setCategoryModel(CheckableListProxy *categories);

	void setSortAll(ER::SortBy sort);
	void setSortGlobal(ER::SortBy sort);
	void setSortLocal(ER::SortBy sort);
	void enableRange(bool enable);
	void setRadius(float radius);
	void enableYears(bool enable);
	void enableAutoLaunch(bool enable);
	void setSearch(const QString &s);
	void setShowLegend(bool show);

	// two configurations
	// 1. show global/local
	// 2. show all
	void setToAll(bool all);

signals:
	void sSortAll(ER::SortBy sort);
	void sSortGlobal(ER::SortBy sort);
	void sSortLocal(ER::SortBy sort);
	void sEnableRange(bool enable);
	void sSetRadius(float radius);
	void sEnableYears(bool enable);
	void sEnableAutoLaunch(bool enable);
	void sSearch(const QString &search);
	void sShowLegend(bool show);
	void sClear();

private:
	std::unique_ptr<Ui::ERFilterArea> ui;

	//EditDeleteDelegate *m_category_delegate;
	//QListView *m_category_view;
	CheckableListProxy *m_category_checkbox_model;

	//QListView *m_type_view;
	QAbstractItemModel *m_type_model;
	CheckableListProxy *m_type_checkbox_model;

	bool m_all;
};

#endif