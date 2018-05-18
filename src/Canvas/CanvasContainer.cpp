#include "Canvas/CanvasContainer.h"
#include <QDebug>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTextCursor>
#include <QApplication>
#include <QTextDocument>

#include "FocusFilter.h"
#include "Canvas/CanvasScene.h"

CanvasContainer::CanvasContainer(QWidget *parent)
	: QFrame(parent),
	m_scene(nullptr)
{
	m_view = new QGraphicsView(this);
	m_layout = new QGridLayout(this);
	m_layout->addWidget(m_view);
	m_layout->setMargin(0);

	m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	m_view->horizontalScrollBar()->setEnabled(false);
	m_view->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	m_view->verticalScrollBar()->setEnabled(false);
	m_view->setDragMode(QGraphicsView::RubberBandDrag);
	m_view->setDragMode(QGraphicsView::DragMode::NoDrag);
	m_view->setObjectName("canvasView");
	m_view->setStyleSheet("#canvasView{background:transparent;}");
	m_view->setFrameShape(QFrame::NoFrame);

	m_view->installEventFilter(this);

	m_manipulator = new TransformManipulator(m_scene, m_view, m_view);
	m_manipulator->hide();
}

void CanvasContainer::setEditable(bool editable)
{
	m_editable = editable;
	if (m_scene) m_scene->setEditable(editable);

	m_manipulator->setEnabled(editable);
	m_view->setDragMode(editable ? QGraphicsView::RubberBandDrag : QGraphicsView::NoDrag);
}

void CanvasContainer::resizeEvent(QResizeEvent * event)
{

	QFrame::resizeEvent(event);

	// There was a bug where the first thumbnail drawn was off.
	// For thumbnails everything needs to be ready in 1 render.
	// Qt doesn't adjust viewport size until during the render
	// then our view matrix is out of date (not centered 0,0)
	m_view->viewport()->setGeometry(m_view->rect());

	rescale();
	m_manipulator->reposition();
}

void CanvasContainer::debug() {
	qDebug() << "debugging container";
	qDebug() << "view";
	qDebug() << "viewsr" << m_view->sceneRect();
	qDebug() << m_view->transform();
	qDebug() << "scene item count" << m_scene->items().size();
	qDebug() << "ssr" << m_scene->sceneRect();
	auto items = m_scene->items();
	for (auto item : items) {
		qDebug() << "i" << item->rect();
	}
}

void CanvasContainer::rescale()
{
	double factor = m_view->viewport()->size().height();
	m_view->resetMatrix();
	m_view->scale(factor, factor);

	m_view->centerOn(0, 0);
	//m_manipulator->reposition();
}

bool CanvasContainer::eventFilter(QObject * obj, QEvent * e)
{
	QKeyEvent *ke;
	QEvent::Type type = e->type();

	switch (type) {
	case QEvent::KeyPress:
	case QEvent::ShortcutOverride:
		ke = static_cast<QKeyEvent*>(e);
		// filter undo redo
		if (ke->matches(QKeySequence::Redo) || ke->matches(QKeySequence::Undo)) {
			return true;
		}
		// filter arrow scrolling
		int key = ke->key();
		if (key == Qt::Key_Left || key == Qt::Key_Right) {
			return true;
		}
		break;
	}
	return false;
}

CanvasScene *CanvasContainer::scene() const
{
	return m_scene;
}

void CanvasContainer::setScene(CanvasScene * scene)
{
	if (!scene) disconnect(m_scene, 0, this, 0);

	m_scene = scene;
	m_view->setScene(scene);
	m_manipulator->setScene(scene);

	if (!scene) return;

	connect(scene, &QObject::destroyed, this, [this]() {
		m_scene = nullptr;
	});
	connect(m_scene, &QGraphicsScene::changed,
		this, [this]() {
		m_view->centerOn(0, 0);
	});

	// other connections in TransformManipulator::setScene
	rescale();
}

TransformManipulator::TransformManipulator(CanvasScene * scene, QGraphicsView *view, QWidget * parent)
	: QWidget(parent),
	m_scene(nullptr),
	m_view(view),
	m_dragging(false),
	m_resizing(false)
{
	setGeometry(0, 0, 10, 10);
	setCursor(Qt::CursorShape::SizeAllCursor);

	setAttribute(Qt::WA_NoSystemBackground, true);

	// buttons
	for (int i = 0; i < 9; i++) {
		// create button
		ResizeButton *button = new ResizeButton(this, (Position)i, this);
		button->setGeometry(0, 0, m_button_size, m_button_size);
		m_buttons.push_back(button);
		m_buttons[i]->show();

		button->setStyleSheet(
			"background: rgb(255, 255, 255);"
			"border: 1px solid black;"
		);
	}
	m_buttons[MIDDLE_CENTER]->hide();

	setRect(QRectF());
	setScene(m_scene);
}

void TransformManipulator::recalculate()
{
	if (!m_scene
		|| !isEnabled()) {
		hide();
		return;
	}
	auto rects = m_scene->getSelected();
	if (rects.size() == 0
		|| !m_scene->isEditable()) {
		hide();
		return;
	}
	else {
		QRectF bound;
		for (auto rect : rects) {
			bound = bound.united(rect->rect());
		}
		setRect(bound);
		show();
	}
}

void TransformManipulator::reposition()
{
	if (!m_scene || !isEnabled()) return;

	// top-left and bottom-right in view space
	QPoint tl = m_view->mapFromScene(anchorScenePos(m_rect, TOP_LEFT));
	QPoint br = m_view->mapFromScene(anchorScenePos(m_rect, BOTTOM_RIGHT));

	// top-left and bottom-right with frame added
	QPoint frame_tl = tl - QPoint(m_frame_out, m_frame_out);
	QPoint frame_br = br + QPoint(m_frame_out, m_frame_out);

	QRect frame_rect(frame_tl, frame_br);

	setGeometry(frame_rect);

	// update buttons
	for (int i = 0; i < 9; i++) {
		QPoint view_pos = m_view->mapFromScene(anchorScenePos(m_rect, (Position)i));
		QPoint manip_pos = mapFromParent(view_pos);
		QPoint button_pos = manip_pos - QPoint(m_button_size / 2, m_button_size / 2);
		m_buttons[i]->move(button_pos);
	}

	// mask buttons
	QRegion buttons = childrenRegion();

	// mask edges
	int w = size().width();
	int h = size().height();
	int frame = m_frame_in + m_frame_out;
	QRegion edge = QRegion(0, 0, w, h) - QRegion(frame, frame, w - 2 * frame, h - 2 * frame);

	QRegion mask = buttons + edge;
	setMask(mask);
}

QPointF TransformManipulator::anchorScenePos(const QRectF &rect, Position pos) {
	int row = pos / 3;
	int col = pos % 3;
	return QPointF(rect.x() + col * rect.width() / 2,
		rect.y() + row * rect.height() / 2);
}

void TransformManipulator::setScene(CanvasScene * scene)
{
	if (m_scene != nullptr) disconnect(m_scene, 0, this, 0);
	m_scene = scene;
	recalculate();

	if (scene) {
		connect(m_scene, &QGraphicsScene::changed, this, &TransformManipulator::recalculate);
		connect(m_scene, &QGraphicsScene::selectionChanged, this, &TransformManipulator::recalculate);
	}
}

QRectF TransformManipulator::baseRectF() const
{
	return m_rect;
}

void TransformManipulator::setRect(QRectF rect)
{
	if (m_rect == rect) return;
	m_rect = rect;
	reposition();
}

void TransformManipulator::startMove(QPoint start_pos)
{
	if (!m_scene) return;
	m_scene->beginTransform();
	m_dragging = true;
	m_scene->beginMove(m_view->mapToScene(start_pos));
}

void TransformManipulator::previewMove(QPoint current_point)
{
	if (!m_scene) return;
	if (!m_dragging) return;
	m_scene->previewMove(m_view->mapToScene(current_point));
}

void TransformManipulator::endMove(QPoint end_point)
{
	if (!m_scene) return;
	m_dragging = false;
	m_scene->endMove(m_view->mapToScene(end_point));
}

void TransformManipulator::startResize(QPoint start_point, Position drag_pos)
{
	if (!m_scene) return;
	m_scene->beginTransform();
	m_resizing = true;
	m_drag_pos = drag_pos;
	m_start_drag_point = start_point;
	m_start_drag_rect = m_rect;
}

void TransformManipulator::previewResize(QPoint current_point, bool fixed_ratio)
{
	if (!m_scene) return;
	if (!m_resizing) return;

	// convert start/end into scene coordinates
	QPointF start_scene = m_view->mapToScene(m_start_drag_point);
	QPointF current_scene = m_view->mapToScene(current_point);
	QPointF diff = current_scene - start_scene;

	QRectF r_old = m_start_drag_rect;
	QRectF r_new = r_old;

	// figure out the new rectangle, depends on which button you're dragging
	int row = m_drag_pos / 3;
	int col = m_drag_pos % 3;

	double min_size = .01;

	// resize
	double w_old = r_old.width();
	double h_old = r_old.height();
	double w_new = w_old;
	double h_new = h_old;
	double mdiff = 1.0;
	QPointF change = diff;

	// if dragging the left or top, then our diffs work backward
	// +x is smaller, +y is smaller
	if (col == 0) {
		change.setX(-diff.x());
	}
	if (row == 0) {
		change.setY(-diff.y());
	}
	if (col == 0 || col == 2) {
		w_new = std::max(w_old + change.x(), min_size);
	}
	if (row == 0 || row == 2) {
		h_new = std::max(h_old + change.y() * mdiff, min_size);
	}

	// force the diff ratio to be the same as the start rect ratio
	double old_ratio = m_start_drag_rect.width() / m_start_drag_rect.height();
	double new_ratio = w_new / h_new;
	if (fixed_ratio) {
		// if too wide, then change height to match ratio
		if (new_ratio > old_ratio) {
			h_new = w_new / old_ratio;
		}
		// if too tall, then change height to match ratio
		else {
			w_new = h_new * old_ratio;
		}
	}

	// translation
	double x_new = r_old.x();
	double y_new = r_old.y();

	// left buttons can move x
	if (col == 0) {
		x_new = r_old.right() - w_new;
	}

	// top buttons can move y
	if (row == 0) {
		y_new = r_old.bottom() - h_new;
	}

	r_new = QRectF(x_new, y_new, w_new, h_new);

	// map all items to r_old coordinates, (0,0) is the top left
	//   old' = old - oldtl
	// then every point has an easy remapping
	//   new' = old' * scale
	// converting to new coordinates
	//   new = new' + new

	double scale_x = r_new.width() / r_old.width();
	double scale_y = r_new.height() / r_old.height();

	// resize/reposition all selected items
	for (auto &pair : m_scene->getTransformRects()) {
		CanvasItem *item = pair.first;
		QRectF r = pair.second;

		// to old coordinates
		// old corners, relative to top left
		QPointF tl = r.topLeft() - r_old.topLeft();
		QPointF br = r.bottomRight() - r_old.topLeft();

		// scale
		// new corners
		QPointF tl2 = QPointF(
			tl.x() * scale_x,
			tl.y() * scale_y
		);
		QPointF br2 = QPointF(
			br.x() * scale_x,
			br.y() * scale_y
		);

		// to new coordinates
		QRectF r2(tl2 + r_new.topLeft(), br2 + r_new.topLeft());
		item->setRect(r2);
	}
}

void TransformManipulator::endResize()
{
	if (!m_scene) return;
	m_resizing = false;
	m_scene->endTransform();
}


void TransformManipulator::paintEvent(QPaintEvent * event)
{
	if (!m_scene) return;
	// paint the default
	QPainter painter(this);

	// stroke the border blue;
	painter.setPen(QPen(QBrush(QColor(100, 100, 255)), 0));
	painter.setBrush(Qt::BrushStyle::NoBrush);
	QPainterPath path;

	int frame_size = m_frame_in + m_frame_out;
	int w = size().width() - 2 * m_frame_out;
	int h = size().height() - 2 * m_frame_out;
	painter.drawRect(m_frame_out, m_frame_out, w, h);
}

void TransformManipulator::mousePressEvent(QMouseEvent * mouseEvent)
{
	startMove(mouseEvent->globalPos());
}

void TransformManipulator::mouseMoveEvent(QMouseEvent * mouseEvent)
{
	previewMove(mouseEvent->globalPos());
}

void TransformManipulator::mouseReleaseEvent(QMouseEvent * mouseEvent)
{
	endMove(mouseEvent->globalPos());
}

TransformManipulator::ResizeButton::ResizeButton(TransformManipulator *boss, Position pos, QWidget * parent)
	: QFrame(parent),
	m_boss(boss),
	m_pos(pos)
{
	setCursor(CursorForPosition[pos]);
}

void TransformManipulator::ResizeButton::mousePressEvent(QMouseEvent *mouseEvent)
{
	m_boss->startResize(mouseEvent->globalPos(), m_pos);
}

void TransformManipulator::ResizeButton::mouseMoveEvent(QMouseEvent *mouseEvent)
{
	if (!m_boss->m_scene) return;

	// fixed ratio if 1 item selected by corners that prefers fixed
	bool fixed = false;

	if (m_pos == TOP_LEFT
		|| m_pos == TOP_RIGHT
		|| m_pos == BOTTOM_LEFT
		|| m_pos == BOTTOM_RIGHT) {
		if (mouseEvent->modifiers() & Qt::ShiftModifier) {
			fixed = true;
		}
		else {
			auto sel = m_boss->m_scene->getSelected();
			if (sel.size() == 1 && (*sel.begin())->prefersFixedRatio()) {
				fixed = true;
			}
		}
	}

	m_boss->previewResize(mouseEvent->globalPos(), fixed);
}

void TransformManipulator::ResizeButton::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
	m_boss->endResize();
}