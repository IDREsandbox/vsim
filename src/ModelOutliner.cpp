#include "ModelOutliner.h"
#include <QItemDelegate>
#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include <QDebug>
#include <QHeaderView>
#include <QPushButton>



OutlinerTimeDelegate::OutlinerTimeDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
}
void OutlinerTimeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	if (option.state & QStyle::State_MouseOver) {
		QStyleOptionButton button;
		if (!index.data().isValid()) {
			// paint a giant + button
			QRect r = option.rect;
			button.rect = QRect(r.left(), r.top(), r.width(), r.height());
			button.text = "Add";
			//qDebug() << "option state" << option.state;
			button.state = (option.state & QStyle::State_Sunken) ? QStyle::State_Sunken | QStyle::State_Enabled : QStyle::State_Enabled;
			QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
		}
		else {
			// paint a little [-] button
			QStyleOptionButton button;
			QRect r = option.rect;
			button.rect = QRect(r.right() - 20, r.top(), 20, r.height());
			button.text = "-";
			button.state = QStyle::State_Enabled;
			QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
		}
		QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
	}
}
bool OutlinerTimeDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		if (!index.data().isValid()) {
			// create a new thing
			model->setData(index, 100);
			return true;
		}
		else {
			// scan for the [-]
			QMouseEvent * e = (QMouseEvent *)event;
			int clickX = e->x();
			int clickY = e->y();

			QRect r = option.rect;//getting the rect of the cell
			int x, y, w, h;
			x = r.right() - 20;//the X coordinate
			y = r.top();//the Y coordinate
			w = 20;//button width
			h = r.height();//button height

			if (clickX > x && clickX < x + w) {
				if (clickY > y && clickY < y + h) {
					model->setData(index, QVariant());
					return true;
				}
			}
		}
	}

	return QStyledItemDelegate::editorEvent(event, model, option, index);
	//return false;
}

//QWidget * OutlinerTimeDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
//{
//	if (!index.data().isValid()) {
//		QPushButton *button = new QPushButton(parent);
//		connect(button, &QPushButton::released, this,
//			[]() {
//			//qDebug() << "really pressed";
//		});
//
//		return button;
//	}
//	return QStyledItemDelegate::createEditor(parent, option, index);
//}


//void OutlinerTimeDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
//{
//	qDebug() << "set model data";
//	QStyledItemDelegate::setModelData(editor, model, index);
//}

ModelOutliner::ModelOutliner(QWidget *parent)
	: QTreeView(parent)
{
	setWindowTitle("Model Outliner");
	setWindowFlags(Qt::Window);
	setGeometry(0, 0, 200, 600);

	auto *oldd = itemDelegate();
	qDebug() << "old item delegate" << oldd;

	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSelectionBehavior(QAbstractItemView::SelectItems);

	
	auto d = new OutlinerTimeDelegate(this);
	setItemDelegateForColumn(2, d);
	setItemDelegateForColumn(3, d);
	//setItemDelegateForColumn(2, d);
}
