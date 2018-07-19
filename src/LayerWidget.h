#ifndef LAYERWIDGET_H
#define LAYERWIDGET_H

#include <set>
#include <QWidget>
#include <QGraphicsView>
#include <QGridLayout>

// Efficiently renders overlapping + transparent layers by utilizing
// QGraphicsView's built in item caching. Might be buggy.
class LayerWidget : public QWidget {
	Q_OBJECT;
public:
	LayerWidget(QWidget *parent = nullptr);

	void setBase(QWidget *widget);
	QGraphicsProxyWidget *addLayer(QWidget *widget, bool fill = false);

	QGraphicsView *view();
	QGraphicsScene *scene();

protected:
	void resizeEvent(QResizeEvent *e) override;

private:
	QGraphicsScene *m_scene;
	QGraphicsView *m_view;
	QGridLayout *m_layout;

	std::set<QWidget*> m_fill;
};

#endif
