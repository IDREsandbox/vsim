#include "ModelOutliner.h"
#include <QItemDelegate>
#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include <QDebug>
#include <QHeaderView>
#include <QPushButton>

#include "ModelGroup.h"
#include "OSGYearModel.h"

ModelOutliner::ModelOutliner(QWidget *parent)
	: QTreeView(parent),
	m_models(nullptr)
{
	setWindowTitle("Models");
	setWindowFlags(Qt::Dialog);
	setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	auto *oldd = itemDelegate();

	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSelectionBehavior(QAbstractItemView::SelectItems);

	OutlinerTimeDelegate *d = new OutlinerTimeDelegate(this);
	setItemDelegateForColumn(OSGYearModel::BEGIN, d);
	setItemDelegateForColumn(OSGYearModel::END, d);

	setIndentation(16);

	m_year_model = new OSGYearModel(this);
	m_year_model->includeRoot(false);
	setModel(m_year_model);
}


void ModelOutliner::setModelGroup(ModelGroup * models)
{
	if (m_models != nullptr) disconnect(m_models, 0, this, 0);
	m_models = models;
	if (!models) return;
	connect(m_models, &QObject::destroyed, this, [this]() {m_models = nullptr; });

	m_year_model->setBase(models->rootWrapper());
	connect(models, &ModelGroup::sReset, this, [this]() {
		m_year_model->setBase(m_models->rootWrapper());
	});
}

OutlinerTimeDelegate::OutlinerTimeDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
}

void OutlinerTimeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	if (option.state & QStyle::State_MouseOver && index.flags() & Qt::ItemIsEditable) {
		QStyleOptionButton button;
		if (!index.data().isValid()) {
			// we don't need the Add button
			//// paint a giant + button
			//QRect r = option.rect;
			//button.rect = QRect(r.left(), r.top(), r.width(), r.height());
			//button.text = "Add";
			//button.state = (option.state & QStyle::State_Sunken) ? QStyle::State_Sunken | QStyle::State_Enabled : QStyle::State_Enabled;
			//QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
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
	if (event->type() == QEvent::MouseButtonPress && index.flags() & Qt::ItemIsEditable)
	{
		if (!index.data().isValid()) {
			//// create a new thing
			//model->setData(index, 100);
			//return true;
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
