#ifndef CANVASCONTAINER_H
#define CANVASCONTAINER_H

#include <set>
#include <vector>
#include <map>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

class CanvasScene;
class TransformManipulator;
class RectItem;
class TextRect;
class TextItem;

typedef std::vector<std::pair<RectItem*, QRectF>> ItemRectList;

// A widget with transformable items inside
class CanvasContainer : public QWidget {
	Q_OBJECT;
public:
	CanvasContainer(QWidget *parent);

	void enableEditing(bool enable);

	std::set<RectItem*> getSelectedRects() const;
	void setSelectedRects(const std::set<RectItem*> &items);

	void setBaseHeight(double height);

	// undo redo filter
	bool eventFilter(QObject *obj, QEvent *e) override;

	void clear();

signals:
	void rectsTransformed(const std::map<RectItem*, QRectF> &rects);

protected:
	void resizeEvent(QResizeEvent* event) override;

protected:
	CanvasScene *m_scene;
	QGraphicsView *m_view;
	QGridLayout *m_layout;

	bool m_editing;
	double m_base_height;
	double m_base_width;

	TransformManipulator *m_manipulator;

	// makes the scene really big so we can always center
	QGraphicsRectItem *m_giant_rect;
	// debug shape
	QGraphicsRectItem *m_center_rect;
};

// The internal scene for a CanvasContainer
class CanvasScene : public QGraphicsScene {
	Q_OBJECT;
public:
	CanvasScene(QObject *parent);
	QList<RectItem*> selectedRects() const;

	// call this before transforming items
	void beginTransform();

	// call this after transforming items
	void endTransform();

	// get the original rects for items
	// use after beginTransform()
	typedef std::vector<std::pair<RectItem*, QRectF>> ItemRectList;
	const ItemRectList &getTransformRects() const;

signals:
	void sEndTransform();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

private:
	ItemRectList m_saved_rects;
};

// A rectangle drawn around selected items with
// transform controls for resizing/moving.
class TransformManipulator : public QWidget {
public:
	TransformManipulator(CanvasScene *scene, QGraphicsView *view, QWidget *parent = nullptr);
	friend class ResizeButton;

	enum Edge {
		TOP,
		LEFT,
		RIGHT,
		BOTTOM
	};
	enum Position {
		TOP_LEFT,
		TOP_CENTER,
		TOP_RIGHT,
		MIDDLE_LEFT,
		MIDDLE_CENTER,
		MIDDLE_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_CENTER,
		BOTTOM_RIGHT
	};
	static constexpr const Qt::CursorShape CursorForPosition[] = {
		Qt::SizeFDiagCursor,
		Qt::SizeVerCursor,
		Qt::SizeBDiagCursor,
		Qt::SizeHorCursor,
		Qt::BlankCursor,
		Qt::SizeHorCursor,
		Qt::SizeBDiagCursor,
		Qt::SizeVerCursor,
		Qt::SizeFDiagCursor
	};

	void recalculate();
	void reposition();

	// scene position
	static QPointF anchorScenePos(const QRectF &rect, Position pos);

	// the current scene rect being highlighted
	QRectF baseRectF() const;
	void setRect(QRectF rect);

	void startMove(QPoint start_point);
	void previewMove(QPoint current_point);
	void endMove();
	void startResize(QPoint start_point, Position drag_pos);
	void previewResize(QPoint current_point);
	void endResize();

protected:
	void paintEvent(QPaintEvent *event) override;
	void mousePressEvent(QMouseEvent *mouseEvent) override;
	void mouseMoveEvent(QMouseEvent *mouseEvent) override;
	void mouseReleaseEvent(QMouseEvent *mouseEvent) override;

public:
	class ResizeButton : public QFrame {
	public:
		ResizeButton(TransformManipulator *rect, Position pos, QWidget *parent = nullptr);
	protected:
		void mousePressEvent(QMouseEvent *mouseEvent) override;
		void mouseMoveEvent(QMouseEvent *mouseEvent) override;
		void mouseReleaseEvent(QMouseEvent *mouseEvent) override;
	private:
		TransformManipulator *m_boss;
		Position m_pos;
	};

private:
	CanvasScene *m_scene;
	QGraphicsView *m_view;
	QList<ResizeButton*> m_buttons;
	QRectF m_rect;

	int m_button_size = 8;
	int m_frame_out = 4; // css margin
	int m_frame_in = 4; // css padding

	bool m_dragging;
	bool m_resizing;
	Position m_drag_pos;
	QPoint m_start_drag_point; // global pixel coordinates
	QRectF m_start_drag_rect; // scene coordinates
};

// A transformable rectangular item in a CanvasScene
class RectItem : public QGraphicsRectItem {
public:
	RectItem(QGraphicsItem *parent = nullptr);

	// Use these over the QGraphicsItem functions
	// Scene-relative units
	QSizeF size() const;
	//QPointF pos();
	QRectF rect() const;
	void setRect(QRectF r);
	void setRect(double x, double y, double w, double h);
	void resize(double w, double h);
	void move(double x, double y);
	//void setRotation(double deg_ccw);
protected:
	virtual void onResize(QSizeF size);

private:
	double m_w;
	double m_h;
};

// A transformable text item in a CanvasScene
class TextRect : public RectItem {
public:
	TextRect(QGraphicsItem *parent = nullptr);

	void setBaseHeight(double height); // ex. 600.0 for 800x600

	virtual QRectF boundingRect() const override;

protected:
	virtual void onResize(QSizeF size) override;
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

public:
	TextItem * m_text;
	double m_base_height;
};

// The internal text item for TextRect
class TextItem : public QGraphicsTextItem {
public:
	TextItem(TextRect *parent);

	virtual QRectF boundingRect() const override;
	virtual QPainterPath shape() const override;

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
	TextRect *m_rect;
};

#endif
