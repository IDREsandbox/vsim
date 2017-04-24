#include <QScrollBar>
#include <QResizeEvent>
#include "HorizontalScrollBox.h"

#include <QCoreApplication>

HorizontalScrollBox::HorizontalScrollBox(QWidget* parent) : QScrollArea(parent)
{
	this->setObjectName(QStringLiteral("scrollArea"));
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	this->setWidgetResizable(true);

	m_scroll_area_widget = new QWidget(this);
	m_scroll_area_widget->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
	//m_scroll_area_widget->setGeometry(QRect(0, 0, 653, 316));
	//m_scroll_area_widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	m_scroll_area_widget->setStyleSheet(QStringLiteral("background-color: rgb(200, 100, 200);"));
	this->setWidget(m_scroll_area_widget);

	//m_layout = new CardLayout(m_scroll_area_widget, 10);
	//m_layout->setHeight(m_scroll_area_widget->height());
	//m_layout->setContentsMargins(0, 0, 0, 0);
	
	// initialize menus
	m_slide_menu = new QMenu(tr("Context menu"), this);
	//QAction actionDelete("Cut", m_slide_menu);
	//QAction actionDelete("Copy", m_slide_menu);
	//QAction actionDelete("Paste", m_slide_menu);
	QAction* actionNew = new QAction("New Slide", m_slide_menu);
	QAction* actionDelete = new QAction("Delete Slide", m_slide_menu);
	//QAction actionDelete("Duplicate Slide", m_slide_menu);
	connect(actionNew, &QAction::triggered, this,



	addItem("FOO THE BAR");
	addItem("BAR The foool?");
	insertItem(1, "Insert me at 1 (aka 2)");

	for (int i = 0; i < 10; i++) {
		addItem("some itemmm");
	}

	insertItem(0, "the first of course");
	addItem("the LAST ITEM!");

	this->addItem("test");
}

void HorizontalScrollBox::addItem(QString text)
{
	insertItem(m_items.size(), text);
	horizontalScrollBar()->setValue(horizontalScrollBar()->maximum());
}

void HorizontalScrollBox::insertItem(int index, QString text)
{
	qDebug() << "insert item";
	auto newWidget = new ScrollBoxItem(m_scroll_area_widget);
	newWidget->setText(QString::number(index + 1) + QString(" - ") + text);

	newWidget->setStyleSheet(QString("background-color: rgb(") + QString::number((index * 40) % 255) + "," + QString::number(((index % 7) * 50) % 255) + "," + QString::number(100) + ");");

	m_items.insert(index, newWidget);

	
	// handle right-clicks
	newWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(newWidget, &QWidget::customContextMenuRequested, this, 
		[this, newWidget](const QPoint& pos) { 
			this->onItemMenu(newWidget->mapToGlobal(pos), newWidget); 
		}
	);
	
	//[this, newWidget](const QPoint& pos) { this->m_slide_menu->exec(newWidget->mapToGlobal(pos)); }

	newWidget->show();
	setWidgetWidth();
	//resizeEvent();
	//positionChildren();
	//QCoreApplication::postEvent(this, new QResizeEvent(size(), size()));
}

void HorizontalScrollBox::deleteItem(int position)
{
	QWidget* item = m_items.takeAt(position);
	delete item;
}

void HorizontalScrollBox::slideContextMenu(QContextMenuEvent* event)
{
	qDebug() << "context menu";

	QMenu* contextMenu = new QMenu(tr("Context menu"), this);
	QAction actionCut("Cut", this);
	QAction actionCopy("Copy", this);
	QAction actionPaste("Paste", this);
	QAction actionNew("New Slide", this);
	QAction actionDelete("Delete Slide", this);
	QAction actionDuplicate("Duplicate Slide", this);
	connect(&actionDelete, &QAction::triggered, this, [this] { qDebug() << "DELETE!!!"; });
	contextMenu->addAction(&actionDelete);

	contextMenu->exec(event->globalPos());
}

void HorizontalScrollBox::onItemMenu(QPoint pos, int slide)
{
	QMenu* slideMenu = new QMenu(tr("Context menu"), this);
	//QAction actionDelete("Cut", m_slide_menu);
	//QAction actionDelete("Copy", m_slide_menu);
	//QAction actionDelete("Paste", m_slide_menu);
	QAction* actionNew = new QAction("New Slide", slideMenu);
	QAction* actionDelete = new QAction("Delete Slide", slideMenu);
	//QAction actionDelete("Duplicate Slide", m_slide_menu);
	connect(actionNew, &QAction::triggered, this, 
		[this, slide] { 
			//this->addItem("a brand new item!"); 
			this->insertItem(slide + 1, "a brand new item!");
		}
	);
	connect(actionDelete, &QAction::triggered, this, 
		[this, slide] { 
			this->deleteItem(slide);
			qDebug() << "DELETE!!!" << slide; 
		}
	);
	slideMenu->addAction(actionNew);
	slideMenu->addAction(actionDelete);

	slideMenu->exec(pos);
}

void HorizontalScrollBox::positionChildren()
{
	//auto margins = contentsMargins();
	//QMargins margins = QMargins(5, 5, 5, 5);

	if (m_items.size() == 0)
		return;

	int xpos = 0;
	//xpos += margins.left();

	//m_height = r.height()
	int bheight = m_height;
	int bwidth = m_height*m_ratio;

	for (int i = 0; i < m_items.size(); i++) {
		QWidget *o = m_items.at(i);

		//QRect geom(xpos, margins.top(), bwidth, bheight);
		QRect geom(xpos, 0, bwidth, bheight);
		o->setGeometry(geom);

		xpos += bwidth;
		xpos += m_spacing;
	}
}

void HorizontalScrollBox::resizeEvent(QResizeEvent* event)
{
	// snapshot of current position and all that
	int oldValue = horizontalScrollBar()->value();
	auto bar = horizontalScrollBar();

	// we want to keep the same frame on the left side when we resize
	int leftmargin = 0;//m_layout->contentsMargins().left();
	int boxWidth = floor(m_height*m_ratio); //int boxWidth = m_layout->boxWidth();

	int nomargin = oldValue - leftmargin;
	int segmentSize = boxWidth + m_spacing;
	int segmentNumber = nomargin / (segmentSize); // [    frame    ][space] <- one segment
	int segmentOffset = nomargin % segmentSize;
	//float framePortion = std::min(1.0f, segmentOffset / (float)boxWidth); // 0-1 if in the frame, 1 if in the spacer
	//int spacerOffset = std::max(0, segmentOffset - boxWidth); // 0 if in the frame, 0-space is in the spacer
	
	// tell layout the new height
	//m_layout->setHeight(event->size().height());
	m_height = event->size().height();
	setWidgetWidth();

	// minimum width
	int newBoxWidth = floor(m_height*m_ratio);
	int newBoxHeight = m_height;

	// deal with other stuff
	QScrollArea::resizeEvent(event);

	int newSegmentSize = newBoxWidth + m_spacing;

	// reposition the scrollbar so that it's over the original slide
	int newValue;
	if (nomargin <= 0) {
		newValue = oldValue;
	}
	else {
		newValue = leftmargin + segmentNumber * newSegmentSize + segmentOffset;//((framePortion > 1.0) ? newBoxWidth + 
	}
	
	bar->setValue(newValue);
}

void HorizontalScrollBox::setWidgetWidth()
{
	// minimum width
	int newBoxWidth = floor(m_height*m_ratio);
	int newBoxHeight = m_height;

	int totalspace = m_spacing*(qMax(0, m_items.size() - 1));

	//int minwidth = margins.left() + margins.right() + totalspace + list.size() * boxWidth();
	int minwidth = totalspace + m_items.size() * newBoxWidth;
	m_scroll_area_widget->setMinimumWidth(minwidth);

	positionChildren();
}
