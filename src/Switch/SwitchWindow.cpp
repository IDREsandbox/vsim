#include "SwitchWindow.h"

#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <QPainter>
#include <QEvent>

#include "SwitchListModel.h"

SwitchWindow::SwitchWindow(QWidget * parent)
	: QFrame(parent)
{
	resize(350, 250);
	setWindowFlags(Qt::WindowType::Window);
	setWindowTitle("Model Switches");
	QGridLayout *layout = new QGridLayout(this);


	QLabel *switch_label = new QLabel(this);
	switch_label->setText("Switches");

	QLabel *nodes_label = new QLabel(this);
	nodes_label->setText("Nodes");

	QLabel *sets_label = new QLabel(this);
	sets_label->setText("Sets");

	m_switch_view = new SwitchListView(this);
	m_nodes_view = new QListView(this);
	m_sets_view = new QListView(this);

	SwitchColorDelegate *delegate = new SwitchColorDelegate(this);
	m_sets_view->setItemDelegate(delegate);

	layout->addWidget(switch_label, 0, 0);
	layout->addWidget(sets_label, 0, 1);
	layout->addWidget(nodes_label, 0, 2);
	layout->addWidget(m_switch_view, 1, 0);
	layout->addWidget(m_sets_view, 1, 1);
	layout->addWidget(m_nodes_view, 1, 2);

	// connect switch selection to this other guy
}

void SwitchWindow::setModel(SwitchListModel *model)
{
	m_model = model;

	m_switch_view->setModel(model);
	m_switch_view->setUniformItemSizes(true);
	m_switch_view->setModelColumn(0);
	m_switch_view->setRootIndex(QModelIndex());
	m_switch_view->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

	m_nodes_view->setModel(model);
	m_nodes_view->setModelColumn(1);

	m_sets_view->setModel(model);
	m_sets_view->setModelColumn(1);
}

void SwitchWindow::onSwitchChanged(const QModelIndex & current)
{
	if (!m_model || !current.isValid()) {
		// how to clear a view?
		m_nodes_view->setRootIndex(QModelIndex());
		m_nodes_view->setModelColumn(0);

		m_sets_view->setRootIndex(QModelIndex());
		m_sets_view->setModelColumn(0);
		return;
	}

	auto index = m_model->index(current.row(),
		SwitchListModel::columnForSection(SwitchListModel::NODES));
	m_nodes_view->setRootIndex(index);

	auto index2 = m_model->index(current.row(),
		SwitchListModel::columnForSection(SwitchListModel::SETS));
	m_sets_view->setRootIndex(index2);
}

SwitchWindow::SwitchListView::SwitchListView(SwitchWindow * window)
	: QListView(window),
	m_window(window)
{
}

void SwitchWindow::SwitchListView::currentChanged(const QModelIndex & current, const QModelIndex & previous)
{
	QListView::currentChanged(current, previous);
	m_window->onSwitchChanged(current);
}

SwitchColorDelegate::SwitchColorDelegate(QObject * parent)
	: QStyledItemDelegate(parent) {
}

void SwitchColorDelegate::paint(QPainter *painter,
	const QStyleOptionViewItem &option,
	const QModelIndex &index) const
{
	//option.
	QRect r = option.rect;

	// get the check state role
	bool checked =
		index.model()->data(index, Qt::ItemDataRole::CheckStateRole) == Qt::Checked;

	if (checked) {
		painter->setBrush(QColor(200, 200, 255));
		painter->setPen(Qt::NoPen);
		painter->drawRect(r.x(), r.y(), r.width(), r.height());
	}

	QStyledItemDelegate::paint(painter, option, index);
}

bool SwitchColorDelegate::editorEvent(QEvent *event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index)
{
	QEvent::Type type = event->type();

	if (type == QEvent::MouseButtonPress) {
		Qt::CheckState state = Qt::CheckState::Checked;

		model->setData(index, state, Qt::ItemDataRole::CheckStateRole);

		return true;
	}

	return QStyledItemDelegate::editorEvent(event, model, option, index);
}
