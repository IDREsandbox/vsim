#include "./CanvasContainer.h"
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QScrollBar>
#include <QTextCursor>
#include <QApplication>

CanvasContainer::CanvasContainer(QWidget *parent)
	: QWidget(parent),
	m_base_height(600),
	m_base_width(800)
{
	m_view = new QGraphicsView(this);
	m_scene = new CanvasScene(this);
	m_view->setScene(m_scene);
	m_layout = new QGridLayout(this);
	m_layout->addWidget(m_view);
	m_layout->setMargin(0);

	m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	m_view->horizontalScrollBar()->setEnabled(false);
	m_view->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	m_view->verticalScrollBar()->setEnabled(false);
	//m_view->setDragMode(QGraphicsView::RubberBandDrag);
	//m_view->setDragMode(QGraphicsView::DragMode::NoDrag);
	m_view->setObjectName("canvasView");
	m_view->setStyleSheet(
		"#canvasView {"
		"	border-style: none;"
		"	background: rgba(0, 0, 0, 0);"
		"}"
	);
	m_view->installEventFilter(this);

	m_manipulator = new TransformManipulator(m_scene, m_view, m_view);
	m_manipulator->hide();

	// giant rect
	m_giant_rect = new QGraphicsRectItem();
	m_giant_rect->setRect(-10, -10, 20, 20);
	m_giant_rect->setBrush(QBrush(QColor(0, 255, 0, 0)));
	m_giant_rect->setPen(QPen(QColor(0, 0, 0), 0));
	m_scene->addItem(m_giant_rect);

	// quarters rectangle
	m_center_rect = new QGraphicsRectItem();
	m_center_rect->setRect(-.5, -.5, 1, 1);
	m_center_rect->setBrush(QBrush(QColor(255, 0, 0, 0)));
	m_center_rect->setPen(QPen(QBrush(), 0, Qt::PenStyle::NoPen));
	m_scene->addItem(m_center_rect);

	QGraphicsItem::GraphicsItemFlags flags;
	flags.setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsFocusable, true);
	flags.setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable, true);
	flags.setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable, true);

	RectItem *label;

	label = new RectItem();
	label->setRect(-.3, -.3, .1, .1);
	m_scene->addItem(label);

	label = new RectItem();
	label->setRect(-.1, -.3, .1, .1);
	m_scene->addItem(label);

	TextRect *edit;

	edit = new TextRect();
	edit->setRect(-.1, -.2, .4, .4);
	m_scene->addItem(edit);

	label = new RectItem();
	label->setRect(-.4, -.4, .2, .2);
	m_scene->addItem(label);

	enableEditing(false);

	connect(m_scene, &CanvasScene::sEndTransform, this,
		[this]() {
		std::map<RectItem*, QRectF> out_map;
		// go through old list, do a diff
		auto saved_rects = m_scene->getTransformRects();
		for (auto item_rect_pair : saved_rects) {
			RectItem *item = item_rect_pair.first;
			QRectF rect = item_rect_pair.second;

			if (item->rect() != rect) {
				out_map[item] = rect;
			}
		}
		if (out_map.size() > 0) emit rectsTransformed(out_map);
	});
	connect(m_scene, &QGraphicsScene::changed, this,
		[this]() {
		m_view->centerOn(0, 0);
	});
	// these have to be directly connect to the manipulator otherwise
	// it crashes on destruction
	connect(m_scene, &QGraphicsScene::changed, m_manipulator, &TransformManipulator::recalculate);
	connect(m_scene, &QGraphicsScene::selectionChanged, m_manipulator, &TransformManipulator::recalculate);
}

void CanvasContainer::enableEditing(bool enable)
{
	auto items = m_scene->items();

	for (auto item : items) {
		RectItem *ritem = dynamic_cast<RectItem*>(item);
		if (!ritem) continue;

		// item flags
		if (enable) {
			item->setFlags(
				QGraphicsItem::ItemIsFocusable
				| QGraphicsItem::ItemIsMovable
				| QGraphicsItem::ItemIsSelectable
			);
		}
		else {
			item->setFlags(0);
		}


		TextRect *titem = dynamic_cast<TextRect*>(ritem);
		if (!titem) continue;

		// text flags
		if (enable) {
			titem->m_text->setTextInteractionFlags(Qt::TextEditorInteraction);
		}
		else {
			titem->m_text->setTextInteractionFlags(Qt::TextBrowserInteraction);
		}
	}

	// drag flag
	if (enable) {
		m_manipulator->setEnabled(true);
		m_view->setDragMode(QGraphicsView::RubberBandDrag);
	}
	else {
		m_manipulator->setEnabled(false);
		m_view->setDragMode(QGraphicsView::NoDrag);
	}
}

std::set<RectItem*> CanvasContainer::getSelectedRects() const
{
	std::set<RectItem*> rects_out;
	for (auto r : m_scene->selectedRects()) {
		rects_out.insert(r);
	}
	return rects_out;
}

void CanvasContainer::setSelectedRects(const std::set<RectItem*>& items)
{
	auto selected_list = m_scene->selectedRects();
	// deselect items
	for (auto selected : selected_list) {
		if (items.find(selected) == items.end()) {
			selected->setSelected(false);
		}
	}
	// select items that should be
	for (auto item : items) {
		item->setSelected(true);
	}
}

void CanvasContainer::resizeEvent(QResizeEvent * event)
{
	double factor = this->size().height();

	m_view->resetMatrix();
	m_view->scale(factor, factor);

	m_view->centerOn(0, 0);
	m_manipulator->reposition();
}

void CanvasContainer::setBaseHeight(double height)
{
	qDebug() << "SET BASE HEIGHT!" << height;
}

bool CanvasContainer::eventFilter(QObject * obj, QEvent * e)
{
	//bool duplicate = false;
	//bool send = true;

	//QKeyEvent *ke;

	//QEvent::Type type = e->type();

	//switch (type) {
	//case QEvent::KeyPress:
	//case QEvent::ShortcutOverride:
	//	ke = static_cast<QKeyEvent*>(e);
	//	if (ke->matches(QKeySequence::Redo) || ke->matches(QKeySequence::Undo)) {
	//		qDebug() << "text steal undo redo event";
	//		duplicate = true;
	//		send = false;
	//	}
	//	break;
	//case QEvent::MouseButtonPress:
	//case QEvent::MouseButtonRelease:
	//case QEvent::MouseMove:
	//	if (!m_editing) {
	//		duplicate = true;
	//		send = true;
	//	}
	//	break;
	//}

	//if (duplicate) {
	//	QApplication::sendEvent(this, e);
	//}
	//return !send;
	return false;
}

void CanvasContainer::clear()
{
	m_scene->clear();
}

CanvasScene::CanvasScene(QObject * parent)
	: QGraphicsScene(parent)
{
}

QList<RectItem*> CanvasScene::selectedRects() const
{
	auto list = selectedItems();
	QList<RectItem*> out_list;

	for (auto item : list) {
		RectItem *rect = dynamic_cast<RectItem*>(item);
		if (rect) {
			out_list.push_back(rect);
		}
	}
	return out_list;
}

void CanvasScene::beginTransform()
{
	QList<RectItem*> items = selectedRects();
	m_saved_rects.clear();
	for (auto i : items) {
		QRectF r = i->rect();
		m_saved_rects.push_back(
			std::pair<RectItem*, QRectF>(i, r)
		);
	}
}

void CanvasScene::endTransform()
{
	emit sEndTransform();
}

const CanvasScene::ItemRectList & CanvasScene::getTransformRects() const
{
	return m_saved_rects;
}

void CanvasScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	QGraphicsScene::mousePressEvent(mouseEvent);
	beginTransform();
}

void CanvasScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
	endTransform();
}

RectItem::RectItem(QGraphicsItem * parent)
	: QGraphicsRectItem(parent)
{
	setFlags(
		ItemIsFocusable
		| ItemIsMovable
		| ItemIsSelectable
	);

	setRect(0, 0, 1, 1);
	setBrush(QBrush(QColor(0, 0, 0, 0)));
	setPen(QPen(QBrush(), 0, Qt::NoPen));
}

QSizeF RectItem::size() const
{
	return QSizeF(m_w, m_h);
}

QRectF RectItem::rect() const
{
	QPointF p = scenePos();
	return QRectF(p.x(), p.y(), m_w, m_h);
}

void RectItem::setRect(QRectF r)
{
	setRect(r.x(), r.y(), r.width(), r.height());
}

void RectItem::setRect(double x, double y, double w, double h)
{
	setPos(x, y);
	m_w = w;
	m_h = h;
	onResize(QSizeF(w, h));
	QGraphicsRectItem::setRect(0, 0, w, h);
}

void RectItem::resize(double w, double h)
{
	setRect(pos().x(), pos().y(), w, h);
}

void RectItem::move(double x, double y)
{
	setPos(x, y);
}

void RectItem::onResize(QSizeF size)
{
}

TransformManipulator::TransformManipulator(CanvasScene * scene, QGraphicsView *view, QWidget * parent)
	: QWidget(parent),
	m_scene(scene),
	m_view(view),
	m_dragging(false),
	m_resizing(false)
{
	setGeometry(0, 0, 10, 10);
	//setStyleSheet("background-color:rgba(0, 0, 255, 100);");
	setCursor(Qt::CursorShape::SizeAllCursor);

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
}

void TransformManipulator::recalculate()
{
	auto rects = m_scene->selectedRects();

	if (rects.size() == 0 || !isEnabled()) {
		hide();
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

QRectF TransformManipulator::baseRectF() const
{
	return m_rect;
}

void TransformManipulator::setRect(QRectF rect)
{
	if (m_rect == rect)
		return;
	m_rect = rect;
	reposition();
}

void TransformManipulator::startMove(QPoint start_pos)
{
	m_scene->beginTransform();
	m_dragging = true;
	m_start_drag_point = start_pos;
	m_start_drag_rect = m_rect;
}

void TransformManipulator::previewMove(QPoint current_point)
{
	if (!m_dragging) return;

	QPointF start_scene = m_view->mapToScene(m_start_drag_point);
	QPointF current_scene = m_view->mapToScene(current_point);
	QPointF diff = current_scene - start_scene;

	for (auto pair : m_scene->getTransformRects()) {
		RectItem *item = pair.first;
		QRectF r = pair.second;

		item->setRect(r.x() + diff.x(), r.y() + diff.y(),
			r.width(), r.height());
	}
}

void TransformManipulator::endMove()
{
	m_dragging = false;
	m_scene->endTransform();
}

void TransformManipulator::startResize(QPoint start_point, Position drag_pos)
{
	m_scene->beginTransform();
	m_resizing = true;
	m_drag_pos = drag_pos;
	m_start_drag_point = start_point;
	m_start_drag_rect = m_rect;
}

void TransformManipulator::previewResize(QPoint current_point)
{
	if (!m_resizing) return;

	// convert start/end into scene coordinates
	QPointF start_scene = m_view->mapToScene(m_start_drag_point);
	QPointF current_scene = m_view->mapToScene(current_point);
	QPointF diff = current_scene - start_scene;

	QRectF r_old = m_start_drag_rect;
	QRectF r_new = r_old;

	int row = m_drag_pos / 3;
	int col = m_drag_pos % 3;

	double min_size = .01;

	// move left
	if (col == 0) {
		r_new.setLeft(std::min(
			r_old.right() - min_size,
			r_old.left() + diff.x()));
	}
	// move right
	else if (col == 2) {
		r_new.setRight(std::max(
			r_old.left() + min_size,
			r_old.right() + diff.x()));
	}
	// move top
	if (row == 0) {
		r_new.setTop(std::min(
			r_old.bottom() - min_size,
			r_old.top() + diff.y()));

	}
	// move bottom
	else if (row == 2) {
		r_new.setBottom(std::max(
			r_old.top() + min_size,
			r_old.bottom() + diff.y()));
	}

	double scale_x = r_new.width() / r_old.width();
	double scale_y = r_new.height() / r_old.height();

	// resize/reposition all selected items
	for (auto pair : m_scene->getTransformRects()) {
		RectItem *item = pair.first;
		QRectF r = pair.second;

		// old corners, relative to top left
		QPointF tl = r.topLeft() - r_old.topLeft();
		QPointF br = r.bottomRight() - r_old.topLeft();

		// new corners
		QPointF tl2 = QPointF(
			tl.x() * scale_x,
			tl.y() * scale_y
		);
		QPointF br2 = QPointF(
			br.x() * scale_x,
			br.y() * scale_y
		);

		QRectF r2(tl2 + r_new.topLeft(), br2 + r_new.topLeft());
		item->setRect(r2);
	}
}

void TransformManipulator::endResize()
{
	m_resizing = false;
	m_scene->endTransform();
}


void TransformManipulator::paintEvent(QPaintEvent * event)
{
	// paint the default
	QPainter painter(this);

	// stroke the border blue;
	painter.setPen(QPen(QBrush(QColor(100, 100, 255)), 0));
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
	endMove();
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
	m_boss->previewResize(mouseEvent->globalPos());
}

void TransformManipulator::ResizeButton::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
	m_boss->endResize();
}

TextRect::TextRect(QGraphicsItem * parent)
	: RectItem(parent),
	m_base_height(600.0)
{
	m_text = new TextItem(this);
	m_text->show();
	m_text->setScale(1 / 600.0);
	m_text->setTextInteractionFlags(Qt::TextEditorInteraction);

	setFocusProxy(m_text);
}

void TextRect::setBaseHeight(double height)
{
	m_text->setScale(1 / height);
}

QRectF TextRect::boundingRect() const
{
	// There was a bug where text that stuck out rendered
	// incorrectly. Making this bound bigger fixes that.

	// rect bound
	QRectF r = RectItem::boundingRect();

	// text bound
	// our scale is [-1,1], text scale is [-300,300]
	// so we have to scale it down
	QRectF cr = m_text->boundingRect();
	double w = cr.width() / m_base_height;
	double h = cr.height() / m_base_height;

	return r.united(QRectF(0, 0, w, h));
}

void TextRect::onResize(QSizeF size)
{
	m_text->setTextWidth(size.width() * m_base_height);
}

QVariant TextRect::itemChange(GraphicsItemChange change, const QVariant & value)
{
	// note: ItemSelectedChange is before and HasChanged is after
	if (change == GraphicsItemChange::ItemSelectedHasChanged) {
		bool selected = value.value<bool>();
		if (selected) {
			// setFocusProxy takes care of this already
		}
		else {
			clearFocus();
			// clear cursor selection, put cursor at the end
			QTextCursor cursor = m_text->textCursor();
			cursor.setPosition(cursor.selectionEnd());
			m_text->setTextCursor(cursor);
		}
	}
	return RectItem::itemChange(change, value);
}

TextItem::TextItem(TextRect * parent)
	: QGraphicsTextItem(parent),
	m_rect(parent)
{
}

QRectF TextItem::boundingRect() const
{
	// Extends the text item bound to include the entire rectangle, even
	// if it is too small. That makes it so that clicking the rectangle
	// gives focus to the text item and places the cursor appropriately.

	// text rect
	QRectF tr = QGraphicsTextItem::boundingRect();

	// base rect
	QRectF br = m_rect->rect();
	QRectF brs(0, 0, br.width() / scale(), br.height() / scale());

	return tr.united(brs);
}

QPainterPath TextItem::shape() const
{
	// Extends the shape to include the rectangle.

	// get the text shape
	QPainterPath p1 = QGraphicsTextItem::shape();

	QPainterPath p2r;
	QRectF r = m_rect->rect();
	p2r.addRect(0, 0, r.width() / scale(), r.height() / scale());

	// tape them together
	return p1.united(p2r);
}

void TextItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	// Removes the text item dotted outline, we already have the rect outline
	// and the transform outline
	QStyleOptionGraphicsItem style(*option);
	//style.state &= QStyle::State_Selected; // doesn't work
	style.state.setFlag(QStyle::State_HasFocus, false);
	QGraphicsTextItem::paint(painter, &style, widget);
}

void TextItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	// Implement selection/deselection involving text items because we
	// steal all of the mouse events for the text editor.
	// Alternative:
	//  call TextRect::mousePressEvent first with transformed coordinates
	//  then call QGraphicsTextItem::mousePressEvent

	qDebug() << "text mouse press";
	bool left = event->button() & Qt::LeftButton;
	bool right = event->button() & Qt::RightButton;
	bool ctrl = event->modifiers() & Qt::Modifier::CTRL;
	bool selected = m_rect->isSelected();

	// add to selection if
	// - ctrl
	if ((left || right) && ctrl) {
			m_rect->setSelected(!selected);
			return; // dont pass this to the text doc
	}
	// single selection if
	// - left
	// - right and not already select
	else if (left || right && !selected) {
		// scene select one item
		QList<QGraphicsItem*> items = m_rect->scene()->selectedItems();
		for (auto item : items) {
			if (item != m_rect) {
				item->setSelected(false);
			}
		}
		m_rect->setSelected(true);
	}

	QGraphicsTextItem::mousePressEvent(event);
}

void TextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
	if (event->modifiers() & Qt::Modifier::CTRL) {
		mousePressEvent(event);
		return;
	}
	QGraphicsTextItem::mouseDoubleClickEvent(event);
}
