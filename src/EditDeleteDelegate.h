#ifndef EDITDELETEDELEGATE_H
#define EDITDELETEDELEGATE_H

#include <QtWidgets/QMainWindow>
#include <QStyledItemDelegate>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>

// Puts a little pencil and X next to view items
// Enable mouse tracking for icon highlighting
class EditDeleteDelegate : public QStyledItemDelegate {
	Q_OBJECT
public:
	EditDeleteDelegate(QObject *parent = nullptr);

	void paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;

	virtual bool editorEvent(QEvent *event, QAbstractItemModel *model,
		const QStyleOptionViewItem &option, const QModelIndex &index) override;

	static QRect rectAt(QRect bounds, int indexFromRight, int padding = 0);

signals:
	void sEdit(QAbstractItemModel *model, const QModelIndex &index);
	void sDelete(QAbstractItemModel *model, const QModelIndex &index);

private:
	QPoint m_mouse_pos;
	QIcon m_edit_icon;
	QIcon m_delete_icon;
};

#endif