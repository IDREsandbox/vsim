#include "narrative/CanvasContainer.h"
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QScrollBar>
#include <QTextCursor>
#include <QApplication>
#include <QTextDocument>
#include "FocusFilter.h"

CanvasContainer::CanvasContainer(QWidget *parent)
	: QFrame(parent)
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
	m_view->setDragMode(QGraphicsView::RubberBandDrag);
	m_view->setDragMode(QGraphicsView::DragMode::NoDrag);
	m_view->setObjectName("canvasView");
	m_view->setStyleSheet("#canvasView{background:rgba(0, 0, 0, 0);}");
	m_view->setFrameShape(QFrame::NoFrame);

	m_view->installEventFilter(this);

	m_manipulator = new TransformManipulator(m_scene, m_view, m_view);
	m_manipulator->hide();

	QGraphicsItem::GraphicsItemFlags flags;
	flags.setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsFocusable, true);
	flags.setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable, true);
	flags.setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable, true);

	setBaseHeight(800);
	setEditable(false);

	clear();

	connect(m_scene, &QGraphicsScene::changed, this,
		[this]() {
		m_view->centerOn(0, 0);
	});
	// these have to be directly connect to the manipulator otherwise
	// it crashes on destruction
	connect(m_scene, &QGraphicsScene::changed, m_manipulator, &TransformManipulator::recalculate);
	connect(m_scene, &QGraphicsScene::selectionChanged, m_manipulator, &TransformManipulator::recalculate);
}

void CanvasContainer::addItem(RectItem *item)
{
	m_scene->addItem(item);
	item->setContainer(this);
	TextRect *text = qgraphicsitem_cast<TextRect*>(item);
	CanvasImage *image = qgraphicsitem_cast<CanvasImage*>(item);
	if (text) {
		text->setBaseHeight(m_base_height);
	}
}

bool CanvasContainer::isEditable() const
{
	return m_editable;
}

void CanvasContainer::setEditable(bool editable)
{
	m_editable = editable;
	auto items = m_scene->items();

	for (auto item : items) {
		RectItem *ritem = dynamic_cast<RectItem*>(item);
		if (!ritem) continue;
		ritem->setEditable(editable);
	}

	// drag flag
	if (editable) {
		m_manipulator->setEnabled(true);
		m_view->setDragMode(QGraphicsView::RubberBandDrag);
	}
	else {
		m_manipulator->setEnabled(false);
		m_view->setDragMode(QGraphicsView::NoDrag);
	}
}

std::set<RectItem*> CanvasScene::getSelectedRects() const
{
	std::set<RectItem*> rects_out;
	auto list = selectedItems();

	for (auto item : list) {
		RectItem *rect = dynamic_cast<RectItem*>(item);
		rects_out.insert(rect);
	}
	return rects_out;
}

void CanvasScene::setSelectedRects(const std::set<RectItem*>& items)
{
	// deselect items
	auto list = selectedItems();
	for (auto item : list) {
		RectItem *rect = dynamic_cast<RectItem*>(item);
		if (items.find(rect) == items.end()) {
			rect->setSelected(false);
		}
	}

	// select items that should be
	for (auto item : items) {
		item->setSelected(true);
	}
}

void CanvasContainer::resizeEvent(QResizeEvent * event)
{

	QFrame::resizeEvent(event);

	// There was a bug where the first thumbnail drawn was off.
	// For thumbnails everything needs to be ready in 1 render.
	// Qt doesn't adjust viewport size until during the render
	// then our view matrix is out of date (not centered 0,0)
	m_view->viewport()->setGeometry(m_view->rect());

	QSize new_size = event->size();

	double factor = new_size.height();
	m_view->resetMatrix();
	m_view->scale(factor, factor);

	m_view->centerOn(0, 0);
	m_manipulator->reposition();
}

void CanvasContainer::setBaseHeight(double height)
{
	// go through all of the text things and change their scale
	auto items = m_scene->items();
	for (auto item : items) {
		TextRect *rect = dynamic_cast<TextRect*>(item);
		if (!rect) continue;
		rect->setBaseHeight(height);
	}
	m_base_height = height;
}

double CanvasContainer::baseHeight() const
{
	return m_base_height;
}

double CanvasContainer::toScene(int px) const
{
	return px / (double)m_base_height;
}

bool CanvasContainer::eventFilter(QObject * obj, QEvent * e)
{
	QKeyEvent *ke;
	QEvent::Type type = e->type();

	switch (type) {
	case QEvent::KeyPress:
	case QEvent::ShortcutOverride:
		ke = static_cast<QKeyEvent*>(e);
		if (ke->matches(QKeySequence::Redo) || ke->matches(QKeySequence::Undo)) {
			return true;
		}
		break;
	}
	return false;
}

void CanvasContainer::clear()
{
	m_scene->clear();
	// giant rect
	m_giant_rect = new QGraphicsRectItem();
	m_giant_rect->setRect(-10, -10, 20, 20);
	m_giant_rect->setBrush(QBrush(QColor(0, 255, 0, 0)));
	m_giant_rect->setPen(QPen(QColor(0, 0, 0), 0));
	m_scene->addItem(m_giant_rect);
}

QGraphicsScene * CanvasContainer::scene() const
{
	return m_scene;
}

CanvasScene::CanvasScene(QObject * parent)
	: QGraphicsScene(parent)
{
}

void CanvasScene::beginTransform()
{
	auto items = getSelectedRects();
	m_saved_rects.clear();
	for (auto i : items) {
		QRectF r = i->rect();
		m_saved_rects.insert(
			std::pair<RectItem*, QRectF>(i, r)
		);
	}
	setFocusItem(nullptr);
}

void CanvasScene::endTransform()
{

	std::map<RectItem*, QRectF> out_map;
	// go through old list, do a diff
	auto saved_rects = getTransformRects();
	for (auto &item_rect_pair : saved_rects) {
		RectItem *item = item_rect_pair.first;
		QRectF rect = item_rect_pair.second;

		if (item->rect() != rect) {
			out_map[item] = item->rect();
		}
	}
	if (out_map.size() > 0) {

		emit sRectsTransformed(out_map);
	}
}

const std::map<RectItem*, QRectF> &CanvasScene::getTransformRects() const
{
	return m_saved_rects;
}

void CanvasScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	QGraphicsScene::mousePressEvent(mouseEvent);
	if (!mouseEvent->isAccepted()) {
		// clear selection
		setSelectedRects({});
	}
}

void CanvasScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
	//endTransform();
}

RectItem::RectItem(QGraphicsItem * parent)
	: QAbstractGraphicsShapeItem(parent),
	m_container(nullptr),
	m_prefers_fixed(false),
	m_border_around(false),
	m_debug_paint(false)
{
	setEditable(false);

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
	resize(w, h);
	move(x, y);
}

void RectItem::resize(double w, double h)
{
	prepareGeometryChange();
	if (m_w == w && m_h == h) {
		return;
	}
	m_w = w;
	m_h = h;
	onResize(QSizeF(w, h));
}

void RectItem::move(double x, double y)
{
	setPos(x, y);
}

void RectItem::onResize(QSizeF size)
{
}

void RectItem::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	QAbstractGraphicsShapeItem::mousePressEvent(mouseEvent);
	mouseEventSelection(mouseEvent);
	mouseEvent->accept();
}

//bool RectItem::editable() const
//{
//	return m_editable;
//}

void RectItem::setEditable(bool enable)
{
	// item flags
	if (enable) {
		setFlags(
			QGraphicsItem::ItemIsFocusable
			| QGraphicsItem::ItemIsMovable
			| QGraphicsItem::ItemIsSelectable
		);
	}
	else {
		setFlags(0);
	}
}

void RectItem::setPrefersFixedRatio(bool fixed)
{
	m_prefers_fixed = fixed;
}

bool RectItem::prefersFixedRatio()
{
	return m_prefers_fixed;
}

void RectItem::setBorderAround(bool around)
{
	m_border_around = around;
}

bool RectItem::borderAround()
{
	return m_border_around;
}

CanvasScene * RectItem::canvasScene() const
{
	return dynamic_cast<CanvasScene*>(scene());
}

void RectItem::setContainer(CanvasContainer * container)
{
	m_container = container;
}

CanvasContainer * RectItem::container() const
{
	return m_container;
}

void RectItem::debugPaint(bool debug)
{
	m_debug_paint = debug;
}

QRectF RectItem::boundingRect() const
{
	QRectF r;
	double pw = pen().widthF();
	double w = size().width();
	double h = size().height();
	if (!m_border_around) {
		r = QRectF(-pw / 2.0, -pw / 2.0, w + pw, h + pw);
	}
	else {
		r = QRectF(-pw, -pw, w + 2.0 * pw, h + 2.0 * pw);
	}
	return r;
}

void RectItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	if (m_debug_paint) {
		painter->setPen(Qt::NoPen);
		painter->setBrush(Qt::yellow);
		painter->drawRect(boundingRect());
	}
	QRectF r;
	double pw = pen().widthF();
	double w = size().width();
	double h = size().height();
	if (!m_border_around) {
		r = QRectF(QPointF(0, 0), size());
	}
	else {
		// +1/2pw on each side
		r = QRectF(-pw / 2.0, -pw / 2.0, w + pw, h + pw);
	}
	painter->setPen(pen());
	painter->setBrush(brush());
	painter->drawRect(r);
	// if you want a rounded border
	//painter->drawRoundedRect(r, pw, pw);
}

TransformManipulator::TransformManipulator(CanvasScene * scene, QGraphicsView *view, QWidget * parent)
	: QWidget(parent),
	m_scene(scene),
	m_view(view),
	m_dragging(false),
	m_resizing(false)
{
	setGeometry(0, 0, 10, 10);
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
	auto rects = m_scene->getSelectedRects();

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
	if (!isEnabled()) return;

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

	for (auto &pair : m_scene->getTransformRects()) {
		RectItem *item = pair.first;
		QRectF r = pair.second;

		item->setRect(r.x() + diff.x(), r.y() + diff.y(),
			r.width(), r.height());
	}
	auto rect = m_view->sceneRect();
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

void TransformManipulator::previewResize(QPoint current_point, bool fixed_ratio)
{
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
		RectItem *item = pair.first;
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
	m_resizing = false;
	m_scene->endTransform();
}


void TransformManipulator::paintEvent(QPaintEvent * event)
{
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
	// if the only selection is an image and doing corners
	bool fixed = false;

	if (m_pos == TOP_LEFT
		|| m_pos == TOP_RIGHT
		|| m_pos == BOTTOM_LEFT
		|| m_pos == BOTTOM_RIGHT) {
		if (mouseEvent->modifiers() & Qt::ShiftModifier) {
			fixed = true;
		}
		else {
			auto sel = m_boss->m_scene->getSelectedRects();
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

TextRect::TextRect(QGraphicsItem * parent)
	: RectItem(parent),
	m_valign(Qt::AlignTop)
{
	m_text = new TextItem(this);
	m_text->show();
	m_text->setTextInteractionFlags(Qt::TextEditorInteraction);

	setBaseHeight(600);
	setFocusProxy(m_text);

	setDocument(m_text->document());
}

void TextRect::setBaseHeight(double height)
{
	m_base_height = height;
	m_text->setScale(1 / height);
	onResize(size());
}

QSizeF TextRect::scaledSize() const
{
	QSizeF s = size();
	return QSizeF(s.width() * m_base_height, s.height() * m_base_height);
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
	//QRectF cr = m_text->boundingRect();
	//double w = cr.width() / m_base_height;
	//double h = cr.height() / m_base_height;

	QRectF tr = mapRectFromItem(m_text, m_text->boundingRect());

	return r.united(tr);
}

void TextRect::setEditable(bool enable)
{
	RectItem::setEditable(enable);
	// text flags
	if (enable) {
		m_text->setTextInteractionFlags(Qt::TextEditorInteraction);
	}
	else {
		m_text->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
	}
}

void TextRect::realign()
{
	// text's internal bounding rect
	QRectF r = m_text->QGraphicsTextItem::boundingRect();
	QSizeF box = scaledSize();
	double new_top = 0; // in text coordinates
	if (this->m_valign & Qt::AlignBottom) {
		new_top = box.height() - r.height();
	}
	else if (this->m_valign & Qt::AlignCenter) {
		double diff = box.height() - r.height();
		new_top = diff / 2;
	}
	else {
		new_top = 0;
	}
	QPointF new_pos(m_text->pos().x(), new_top / m_base_height);
	m_text->setPos(new_pos); // in [-1,1] coordinates
}

void TextRect::setVAlign(Qt::Alignment al)
{
	m_valign = al;
	realign();
}

void TextRect::setDocument(QTextDocument * doc)
{
	m_text->setDocument(doc);
	QObject::connect(doc, &QTextDocument::contentsChanged, m_text, [this]() {
		this->realign();
	});
}

QTextDocument * TextRect::document()
{
	return m_text->document();
}

void RectItem::mouseEventSelection(QGraphicsSceneMouseEvent * event)
{
	bool left = event->button() & Qt::LeftButton;
	bool right = event->button() & Qt::RightButton;
	bool ctrl = event->modifiers() & Qt::Modifier::CTRL;
	bool selected = isSelected();

	// add to selection if
	// - ctrl
	if ((left || right) && ctrl) {
		setSelected(!selected);
		return; // dont pass this to the text doc
	}
	// single selection if
	// - left
	// - right and not already select
	else if (left || right && !selected) {
		// scene select one item
		canvasScene()->setSelectedRects({ this });
		// refocus
		focusItem();
	}
}

void TextRect::onResize(QSizeF size)
{
	m_text->setTextWidth(size.width() * m_base_height);
	realign();
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
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

QRectF TextItem::boundingRect() const
{
	// Extends the text item bound to include the entire rectangle, even
	// if it is too small. That makes it so that clicking the rectangle
	// gives focus to the text item and places the cursor appropriately.

	// text rect
	QRectF tr = QGraphicsTextItem::boundingRect();

	// base rect
	//QRectF br = m_rect->rect();
	//QRectF brs(50, 50, br.width() / scale(), br.height() / scale());

	QRectF br = mapRectFromItem(m_rect, QRectF(QPointF(0, 0), m_rect->size()));

	return tr.united(br);
}

QPainterPath TextItem::shape() const
{
	QPainterPath p;
	p.addRect(boundingRect());
	return p;
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

void TextItem::focusOutEvent(QFocusEvent * event)
{
	QGraphicsTextItem::focusOutEvent(event);

	// setModified false causes a new undo/redo to occur when resuming editing
	document()->setModified(false);
}

void TextItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	m_rect->mouseEventSelection(event);
	QGraphicsTextItem::mousePressEvent(event);
	event->accept();
}

void TextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
	if (event->modifiers() & Qt::Modifier::CTRL) {
		mousePressEvent(event);
		return;
	}
	QGraphicsTextItem::mouseDoubleClickEvent(event);
}

CanvasImage::CanvasImage()
{
	setPrefersFixedRatio(true);
	setBorderAround(true);
	m_pixmap = new QGraphicsPixmapItem(this);
	m_pixmap->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
}

void CanvasImage::setPixmap(const QPixmap & p)
{
	m_pixmap->setPixmap(p);

	if (p.isNull()) return;

	double bh = 600;
	if (container()) bh = container()->baseHeight();

	// find reasonable starting height
	double set_h; // big units
	if (p.height() < 50) {
		set_h = 50;
	}
	else if (p.height() < bh) {
		set_h = p.height();
	}
	else {
		set_h = bh;
	}

	double ratio;
	if (p.height() == 0 || p.width() == 0) {
		ratio = 1.0;
	}
	else {
		ratio = p.width() / (float)p.height();
	}

	// convert to small units and set
	double h = set_h / bh;
	double w = ratio * h;
	resize(w, h);
}

void CanvasImage::onResize(QSizeF size)
{
	// pixels -> canvas ratio -> item ratio
	// (300) -> .5 -> .3

	if (m_pixmap->pixmap().isNull()) {
		return;
	}

	QPixmap p = m_pixmap->pixmap();

	// scale
	double sx = size.width() / p.width();
	double sy = size.height() / p.height();

	QTransform t;
	t.scale(sx, sy);
	m_pixmap->setTransform(t);
}
