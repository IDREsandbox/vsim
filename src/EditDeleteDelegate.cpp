#include "EditDeleteDelegate.h"

EditDeleteDelegate::EditDeleteDelegate(QObject * parent)
	: QStyledItemDelegate(parent)
{
	m_edit_icon.addFile("assets/editIcon.png");
	m_edit_icon.addFile("assets/editIconHover.png", QSize(), QIcon::Active);
	m_delete_icon.addFile("assets/deleteIcon.png");
	m_delete_icon.addFile("assets/deleteIconHover.png", QSize(), QIcon::Active);
}

void EditDeleteDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{

	QStyledItemDelegate::paint(painter, option, index);

	QRect r = option.rect;
	int h = r.height();

	// paint delete box
	if (option.state & QStyle::State_MouseOver) {
		int padding = 3;
		QRect rect = rectAt(r, 0, 3);
		QIcon::Mode mode = QIcon::Normal;
		if (rect.contains(m_mouse_pos)) {
			// do hover stuff
			mode = QIcon::Active;
		}
		m_delete_icon.paint(painter, rect, Qt::AlignCenter, mode);
		//QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
	}

	// paint edit box
	if (option.state & QStyle::State_MouseOver) {
		QRect rect = rectAt(r, 1, 3);
		QIcon::Mode mode = QIcon::Normal;
		if (rect.contains(m_mouse_pos)) {
			// do hover stuff
			mode = QIcon::Active;
		}
		m_edit_icon.paint(painter, rect, Qt::AlignCenter, mode);
	}

	// paint color box - we don't need it, QStyledItem already has one
	//QColor color(255, 0, 0);
	//painter->setBrush(QBrush(color));
	//int border = 1;
	//painter->drawRect(r.right() - h + border, r.top() + border, h - 2 * border, h - 2 * border);
}


bool EditDeleteDelegate::editorEvent(QEvent * event, QAbstractItemModel * model,
	const QStyleOptionViewItem & option, const QModelIndex & index)
{
	QRect r = option.rect;
	QRect deleteRect = rectAt(r, 0, 3);
	QRect editRect = rectAt(r, 1, 3);

	if (event->type() == QEvent::MouseMove) {
		QMouseEvent *me = (QMouseEvent*)event;
		m_mouse_pos = me->pos();
		// force repaint
		if (deleteRect.contains(m_mouse_pos)
			|| editRect.contains(m_mouse_pos)) {
			model->dataChanged(index, index);
		}
	}
	if (event->type() == QEvent::MouseButtonPress) {
		QMouseEvent *me = (QMouseEvent*)event;
		m_mouse_pos = me->pos();
		QPoint pos = me->pos();

		if (deleteRect.contains(pos)) {
			emit sDelete(model, index);
			return true;
		}
		else if (editRect.contains(pos)) {
			emit sEdit(model, index);
			return true;
		}
	}

	return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QRect EditDeleteDelegate::rectAt(QRect bounds, int indexFromRight, int padding)
{
	int w = bounds.height();
	return QRect(bounds.right() - (indexFromRight + 1) * w + padding,
		bounds.top() + padding,
		w - 2 * padding,
		w - 2 * padding);
}