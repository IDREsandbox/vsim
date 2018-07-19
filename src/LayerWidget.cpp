#include "LayerWidget.h"

#include <QResizeEvent>
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QEvent>

class LayerProxyItem : public QGraphicsProxyWidget {
public:
	LayerProxyItem(QGraphicsItem *parent = nullptr)
		: QGraphicsProxyWidget(parent)
	{
	}

protected:
	bool event(QEvent *e) {
		//qDebug() << "e" << e;
		return QGraphicsProxyWidget::event(e);
	}
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
		//qDebug() << "layer item paint";
		QGraphicsProxyWidget::paint(painter, option, widget);
	}
};

LayerWidget::LayerWidget(QWidget * parent)
	: QWidget(parent)
{
	m_layout = new QGridLayout(this);
	m_layout->setMargin(0);

	m_scene = new QGraphicsScene(this);
	m_view = new QGraphicsView(this);

	m_view->setScene(m_scene);
	m_layout->addWidget(m_view, 0, 0);

	m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	m_view->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	m_view->setStyleSheet("background: transparent;");
}

void LayerWidget::setBase(QWidget *widget)
{
	m_view->setViewport(widget);
}

QGraphicsProxyWidget *LayerWidget::addLayer(QWidget *widget, bool fill)
{
	auto *proxy = new LayerProxyItem;
	proxy->setWidget(widget);

	proxy->setCacheMode(QGraphicsItem::CacheMode::DeviceCoordinateCache);

	widget->setAutoFillBackground(false);
	widget->setAttribute(Qt::WA_NoSystemBackground);

	m_scene->addItem(proxy);
	if (fill) {
		m_fill.insert(widget);
	}

	return proxy;
}

QGraphicsView * LayerWidget::view()
{
	return m_view;
}

QGraphicsScene * LayerWidget::scene()
{
	return m_scene;
}

void LayerWidget::resizeEvent(QResizeEvent * e)
{
	QRect r(QPoint(0, 0), e->size());
	m_view->setSceneRect(r);
	m_scene->setSceneRect(r);

	for (auto *w : m_fill) {
		w->resize(e->size());
	}
}
