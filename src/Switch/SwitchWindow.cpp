#include "SwitchWindow.h"

#include <QGridLayout>
#include <QLabel>
#include <QDebug>

#include "SwitchModel.h"

SwitchWindow::SwitchWindow(QWidget * parent)
	: QFrame(parent)
{
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

	layout->addWidget(switch_label, 0, 0);
	layout->addWidget(nodes_label, 0, 1);
	layout->addWidget(sets_label, 0, 2);
	layout->addWidget(m_switch_view, 1, 0);
	layout->addWidget(m_nodes_view, 1, 1);
	layout->addWidget(m_sets_view, 1, 2);

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
	qDebug() << "switch changed" << current;

	if (!m_model) return;

	if (!current.isValid()) {
		// how to clear a view?
		m_nodes_view->setRootIndex(QModelIndex());
		m_nodes_view->setModelColumn(1);
		return;
	}

	auto index = m_model->index(current.row(),
		SwitchListModel::columnForSection(SwitchListModel::NODES));
	qDebug() << "setting to special root index" << index;
	m_nodes_view->setRootIndex(index);
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
