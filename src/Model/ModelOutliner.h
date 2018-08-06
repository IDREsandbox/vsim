#ifndef MODELOUTLINER_H
#define MODELOUTLINER_H
#include <QTreeView>
#include <QTableView>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QPushButton>
#include <QLabel>

class Model;
class ModelGroup;
class OSGYearModel;
class OutlinerTimeDelegate;
class ModelGroupModel;
class OSGNodeWrapper;

class ModelOutliner : public QWidget {
	Q_OBJECT
public:
	ModelOutliner(QWidget *parent = nullptr);

	void setModelGroup(ModelGroup *models);
	void setCurrentDir(const QString &base_dir);

	void setExpandedModel(Model *model);

	void onCurrentChanged(const QModelIndex &current_model);

private:
	class ModelView : public QTreeView {
	public:
		ModelView(ModelOutliner *outliner, QWidget *parent = nullptr);
	protected:
		void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
	private:
		ModelOutliner *m_boss;
	};

private:
	ModelGroup *m_models;

	ModelGroupModel *m_model_model;

	Model *m_expanded_model;
	OSGYearModel *m_year_model;

	ModelView *m_model_view;
	QTreeView *m_node_view;
	OutlinerTimeDelegate *m_time_delegate;
	QLabel *m_failed_label;

	QPushButton *m_import_button;
	QPushButton *m_delete_button;
	QPushButton *m_edit_button;
	QPushButton *m_reload_button;

	QString m_current_dir;
};

class OutlinerTimeDelegate : public QStyledItemDelegate
{
	Q_OBJECT;
public:
	OutlinerTimeDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};
#endif