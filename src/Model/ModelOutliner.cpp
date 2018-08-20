#include "ModelOutliner.h"
#include "ModelOutliner.h"

#include <QItemDelegate>
#include <QApplication>
#include <QMouseEvent>
#include <QDialog>
#include <QDebug>
#include <QHeaderView>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QDir>

#include "Model.h"
#include "ModelGroup.h"
#include "ModelGroupModel.h"
#include "OSGYearModel.h"
#include "OSGNodeWrapper.h"
#include "Core/Util.h"
#include "ModelUtil.h"

ModelOutliner::ModelOutliner(QWidget *parent)
	: QWidget(parent),
	m_models(nullptr),
	m_expanded_model(nullptr),
	m_read_only(false)
{
	setWindowTitle("Model Outliner");
	setWindowFlags(Qt::Window);
	resize(500, 500);

	m_model_model = new ModelGroupModel(this);
	m_year_model = new OSGYearModel(this);
	m_time_delegate = new OutlinerTimeDelegate(this);

	QSplitter *splitter = new QSplitter(this);
	QWidget *model_container = new QWidget(splitter);
	QWidget *node_container = new QWidget(splitter);
	m_model_view = new ModelView(this, model_container);
	m_node_view = new QTreeView(node_container);
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	QLabel *model_label = new QLabel("Models", model_container);
	QLabel *node_label = new QLabel("Nodes", node_container);
	QVBoxLayout *model_layout = new QVBoxLayout(model_container);
	QVBoxLayout *node_layout = new QVBoxLayout(node_container);
	QHBoxLayout *button_row = new QHBoxLayout();
	m_failed_label = new QLabel();
	m_edit_button = new QPushButton("Edit", this);
	m_reload_button = new QPushButton("Reload", this);
	m_import_button = new QPushButton("Import", this);
	m_delete_button = new QPushButton("Remove", this);

	splitter->setOrientation(Qt::Horizontal);
	splitter->addWidget(model_container);
	splitter->addWidget(node_container);

	m_model_view->setModel(m_model_model);
	//m_model_view->setSelectionMode(QAbstractItemView::SingleSelection);

	m_node_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_node_view->setSelectionBehavior(QAbstractItemView::SelectItems);
	m_node_view->setItemDelegateForColumn(OSGYearModel::BEGIN, m_time_delegate);
	m_node_view->setItemDelegateForColumn(OSGYearModel::END, m_time_delegate);
	m_node_view->setIndentation(16);
	m_node_view->setModel(m_year_model);

	m_failed_label->setAlignment(Qt::AlignCenter);
	m_failed_label->setWordWrap(true);
	m_failed_label->hide();

	main_layout->addWidget(splitter);
	main_layout->addLayout(button_row);
	main_layout->setMargin(4);

	model_layout->addWidget(model_label);
	model_layout->addWidget(m_model_view);
	model_layout->setMargin(0);

	node_layout->addWidget(node_label, 0);
	node_layout->addWidget(m_node_view, 1);
	node_layout->addWidget(m_failed_label, 1);
	node_layout->setMargin(0);

	button_row->addWidget(m_edit_button, Qt::AlignLeft);
	button_row->addWidget(m_reload_button, Qt::AlignLeft);
	button_row->addWidget(m_import_button, Qt::AlignLeft);
	button_row->addWidget(m_delete_button, Qt::AlignLeft);
	button_row->addStretch(1);

	connect(m_reload_button, &QAbstractButton::pressed, this, [this]() {
		Model *model = m_expanded_model;
		ModelUtil::execReloadModel(m_expanded_model, m_current_dir, this);
		setExpandedModel(model);
	});
	connect(m_edit_button, &QAbstractButton::pressed, this, [this]() {
		if (m_read_only) return;
		// get shared version of selected model
		int i = m_models->cgroup()->indexOf(m_expanded_model);
		if (i < 0) return;
		auto model = m_models->group()->childShared(i);
		ModelUtil::execEditModel(model, m_current_dir, QString(), nullptr);
		setExpandedModel(model.get());
	});
	connect(m_delete_button, &QAbstractButton::pressed, this, [this]() {
		if (!m_models) return;
		if (!m_expanded_model) return;
		if (m_read_only) return;
		m_models->removeModel(m_expanded_model);
	});
	connect(m_import_button, &QAbstractButton::pressed, this, [this]() {
		if (m_read_only) return;
		auto model = ModelUtil::execImportModel(m_models, m_current_dir, QString(), this);
		if (model) setExpandedModel(model.get());
	});
}


void ModelOutliner::setCurrentDir(const QString & base_dir)
{
	m_current_dir = base_dir;
}

void ModelOutliner::setModelGroup(ModelGroup * models)
{
	Util::reconnect(this, &m_models, models);

	if (!models) return;

	m_model_model->setBase(models);

	m_year_model->setBase(models->rootWrapper());
	//m_node_view->setRootIndex(m_year_model->index(0, 0));

	//connect(models, &ModelGroup::sReset, this, [this]() {
	//	m_year_model->setBase(m_models->rootWrapper());
	//});
	//connect(models, &ModelGroup::)
}

void ModelOutliner::onCurrentChanged(const QModelIndex &model_index)
{
	if (!m_models) return;

	Model *model = m_models->group()->child(model_index.row());
	setExpandedModel(model);
}

void ModelOutliner::setReadOnly(bool read_only)
{
	m_read_only = read_only;
	if (read_only) {
		m_model_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_node_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	}
	else {
		m_model_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
		m_node_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
	}
	m_import_button->setVisible(!read_only);
	m_delete_button->setVisible(!read_only);
	m_edit_button->setVisible(!read_only);
}

void ModelOutliner::setExpandedModel(Model * model)
{
	// find the index
	int row = m_models->group()->indexOf(model);

	int current_row = m_model_view->currentIndex().row();

	// index is bad? don't set
	// index is same row? don't set
	if (row < 0 || row == current_row) {
	}
	else {
		m_model_view->setCurrentIndex(m_model_model->index(row, 0));
	}
	// this sets gui
	// gui change sets this
	// infinite loop, is it ok?


	if (!model) {
		m_node_view->hide();
		m_failed_label->show();
		m_failed_label->setText("");
		return;
	}

	if (m_node_view->rootIndex().isValid()
		&& m_expanded_model == model) {
		// already epanded
		return;
	}

	m_expanded_model = model;

	// set root to the index corresponding to model's node
	// we have to find it, use index of model

	// failed to load?
	if (model->failedToLoad()) {
		m_failed_label->show();
		m_failed_label->setText(QString("Failed to load model: ") + QString::fromStdString(model->path()));
		m_node_view->hide();
	}
	else {
		m_failed_label->hide();
		m_node_view->show();

		QModelIndex target_index = m_year_model->indexOf(model->node());
		m_node_view->setModel(m_year_model);
		m_node_view->setRootIndex(target_index);
	}
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

ModelOutliner::ModelView::ModelView(ModelOutliner *outliner, QWidget *parent)
	: QTreeView(parent),
	m_boss(outliner)
{
}

void ModelOutliner::ModelView::currentChanged(const QModelIndex & current, const QModelIndex & previous)
{
	QTreeView::currentChanged(current, previous);
	m_boss->onCurrentChanged(current);
}
