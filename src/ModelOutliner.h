#ifndef MODELOUTLINER_H
#define MODELOUTLINER_H
#include <QTreeView>
#include <QItemDelegate>
#include <QStyledItemDelegate>

class ModelOutliner : public QTreeView {
	Q_OBJECT
public:
	ModelOutliner(QWidget *parent = nullptr);

};

class OutlinerTimeDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
	OutlinerTimeDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

	//QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
	//	const QModelIndex &index) const override;
	//void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	//void setModelData(QWidget *editor, QAbstractItemModel *model,
	//	const QModelIndex &index) const override;

};
#endif