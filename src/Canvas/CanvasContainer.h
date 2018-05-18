#ifndef CANVASCONTAINER_H
#define CANVASCONTAINER_H

#include <set>
#include <vector>
#include <map>

#include <QGraphicsView>
#include <QGridLayout>

class CanvasScene;
class TransformManipulator;
class CanvasItem;

typedef std::vector<std::pair<CanvasItem*, QRectF>> ItemRectList;

// A viewer for CanvasScene
// can transform scene items
class CanvasContainer : public QFrame {
	Q_OBJECT;
public:
	CanvasContainer(QWidget *parent);

	// undo redo filter on view, prevents
	bool eventFilter(QObject *obj, QEvent *e) override;

	CanvasScene *scene() const;
	void setScene(CanvasScene *scene);

	void setEditable(bool editable);

	void debug();

protected:
	void resizeEvent(QResizeEvent* event) override;

private:
	void rescale();

public:
	CanvasScene *m_scene;
	QGraphicsView *m_view;
	QGridLayout *m_layout;

	bool m_editable;

	TransformManipulator *m_manipulator;
};

// A rectangle drawn around selected items with
// transform controls for resizing/moving.
class TransformManipulator : public QWidget {
public:
	TransformManipulator(CanvasScene *scene, QGraphicsView *view, QWidget *parent = nullptr);

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

	void setScene(CanvasScene *scene);
	// void enableEditing(bool); // use setEnabled() instead

	// scans canvas for selection, then repositions
	void recalculate();

	// places the buttons/mask based on current rect
	void reposition();

	// scene position
	static QPointF anchorScenePos(const QRectF &rect, Position pos);

	// the current scene rect being highlighted
	QRectF baseRectF() const;
	void setRect(QRectF rect);

	void startMove(QPoint start_point);
	void previewMove(QPoint current_point);
	void endMove(QPoint end_point);
	void startResize(QPoint start_point, Position drag_pos);
	void previewResize(QPoint current_point, bool fixed_ratio);
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

#endif
