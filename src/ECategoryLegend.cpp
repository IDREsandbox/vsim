#include "ECategoryLegend.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>

#include "resources/ECategoryGroup.h"
#include "resources/ECategory.h"
#include "Core/Util.h"

ECategoryLegend::ECategoryLegend(QWidget *parent)
	: QFrame(parent),
	m_cats(nullptr),
	m_anchor(QPoint(0,0))
{
	auto *layout = new QVBoxLayout(this);

	auto *label = new QLabel(this);
	layout->addWidget(label);
	label->setText("Resource Legend");
	label->setAlignment(Qt::AlignCenter);
	auto f = label->font();
	f.setPointSize(f.pointSize() + 2);
	label->setFont(f);

	m_list = new ListView(this);
	m_list->setObjectName("list");
	m_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_list->setSelectionMode(QAbstractItemView::NoSelection);
	layout->addWidget(m_list);

	setStyleSheet(
		".ECategoryLegend{background:rgba(0,0,0,155);}"
		".QLabel{color:white;}"
		"#list{background:transparent;}"
	);

	adjustSize();
}

void ECategoryLegend::setCategoryGroup(ECategoryGroup * cats)
{
	Util::reconnect(this, &m_cats, cats);

	connect(cats, &ECategoryGroup::sAnyChange, this, &ECategoryLegend::refresh);

	refresh();
}

void ECategoryLegend::setAnchor(QPoint p)
{
	m_anchor = p;
	reposition();
}

void ECategoryLegend::refresh()
{
	// i'm lazy, repopulate everything

	m_list->clear();

	for (size_t i = 0; i < m_cats->size(); i++) {
		ECategory *cat = m_cats->child(i);

		auto *text = new QListWidgetItem;
		text->setData(Qt::DisplayRole, QString::fromStdString(cat->getCategoryName()));
		text->setData(Qt::DecorationRole, cat->getColor());
		text->setForeground(Qt::white);
		text->setBackground(Qt::transparent);
		m_list->insertItem(i, text);
	}
	m_list->updateGeometry();
	adjustSize();

}

void ECategoryLegend::paintEvent(QPaintEvent * e)
{
	QFrame::paintEvent(e);
}

void ECategoryLegend::resizeEvent(QResizeEvent * e)
{
	reposition();
}

void ECategoryLegend::reposition()
{
	QRect r = rect();
	r.moveBottomRight(m_anchor);
	move(r.topLeft());
}

ECategoryLegend::ListView::ListView(QWidget * parent)
{
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setUniformItemSizes(true);
	setSelectionRectVisible(false);
	setMovement(QListView::Movement::Static);
	setResizeMode(QListView::ResizeMode::Fixed);

	setFrameShape(QFrame::Shape::NoFrame);
	setLineWidth(0);
}

QSize ECategoryLegend::ListView::sizeHint() const
{
	//return QSize(width(), count() * sizeHintForRow(0));

	return QSize(170, sizeHintForRow(0) * count());
}
