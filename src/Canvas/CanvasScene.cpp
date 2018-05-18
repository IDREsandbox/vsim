#include "CanvasScene.h"
#include "Canvas/CanvasScene.h"
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QTextDocument>
#include <QPainter>
#include "FocusFilter.h"

CanvasScene::CanvasScene(QObject * parent)
	: QGraphicsScene(parent),
	m_editable(false),
	m_base_height(800),
	m_selection_in_progress(false),
	m_moving(false)
{
	m_giant_rect = new GiantRectItem(QRectF(-10, -10, 20, 20));
	QGraphicsScene::addItem(m_giant_rect);

	connect(this, &QGraphicsScene::selectionChanged, this, [this]() {
		if (!m_selection_in_progress) {
			emit sSelectionChanged();
		}
	});
}

CanvasScene::~CanvasScene()
{
	// ensures we don't double delete
	for (auto item : m_items) {
		QGraphicsScene::removeItem(item.get());
	}
}

void CanvasScene::addItem(std::shared_ptr<CanvasItem> item)
{
	m_items.insert(item);
	QGraphicsScene::addItem(item.get());
	item->setBaseHeight(m_base_height);

	CanvasLabel *label = dynamic_cast<CanvasLabel*>(item.get());
	if (label) {
		connect(label->document(), &QTextDocument::undoCommandAdded,
			this, [this, label]() {
			emit sDocumentUndoCommandAdded(label, label->document());
		});
	}
}

void CanvasScene::removeItem(CanvasItem *item)
{
	emit sAboutToRemove(item);
	// remove document connection
	CanvasLabel *label = dynamic_cast<CanvasLabel*>(item);
	if (label) {
		disconnect(label->document(), 0, this, 0);
	}

	QGraphicsScene::removeItem(item);
	std::set<SharedItem, ItemCompare>::iterator it = m_items.find(item);
	if (it == m_items.end()) return;
	m_items.erase(it);
	emit sRemoved(item);
}

SharedItemSet CanvasScene::items() const
{
	return m_items;
}

std::set<CanvasItem*> CanvasScene::getSelected() const
{
	std::set<CanvasItem*> rects_out;

	for (auto item : selectedItems()) {
		CanvasItem *rect = dynamic_cast<CanvasItem*>(item);
		if (rect) rects_out.insert(rect);
	}
	return rects_out;
}

void CanvasScene::setSelected(const std::set<CanvasItem*>& items)
{
	m_selection_in_progress = true;
	// deselect items
	for (auto item : selectedItems()) {
		CanvasItem *rect = dynamic_cast<CanvasItem*>(item);
		if (!rect) continue;
		if (items.find(rect) == items.end()) {
			rect->setSelected(false);
		}
	}

	// select items that should be
	for (auto item : items) {
		item->setSelected(true);
	}
	m_selection_in_progress = false;
	emit selectionChanged(); // REVIEW: should this be sSelectionChanged?
}

SharedItemSet CanvasScene::getSelectedShared() const
{
	SharedItemSet shareset;
	auto set = getSelected();
	for (CanvasItem *item : set) {
		auto sitem = toShared(item);
		if (sitem) shareset.insert(sitem);
	}
	return shareset;
}

SharedItem CanvasScene::toShared(CanvasItem *item) const
{
	auto it = m_items.find(item);
	if (it == m_items.end()) {
		return nullptr;
	}
	return *it;
}

bool CanvasScene::selectionInProgress()
{
	return m_selection_in_progress;
}

void CanvasScene::setBaseHeight(double height)
{
	// go through all of the text things and change their scale
	m_base_height = height;
	for (auto item : m_items) {
		item->setBaseHeight(height);
	}
}

double CanvasScene::baseHeight() const
{
	return m_base_height;
}

double CanvasScene::toScene(int px) const
{
	return px / (double)m_base_height;
}

void CanvasScene::clear()
{
	m_items.clear();
}

void CanvasScene::beginTransform()
{
	auto items = getSelected();
	m_saved_rects.clear();
	for (auto i : items) {
		QRectF r = i->rect();
		m_saved_rects.insert(
			std::pair<CanvasItem*, QRectF>(i, r)
		);
	}
	setFocusItem(nullptr);
}

void CanvasScene::endTransform()
{
	std::map<CanvasItem*, QRectF> out_map;
	// go through old list, do a diff
	auto saved_rects = getTransformRects();
	for (auto &item_rect_pair : saved_rects) {
		CanvasItem *item = item_rect_pair.first;
		QRectF rect = item_rect_pair.second;

		if (item->rect() != rect) {
			out_map[item] = item->rect();
		}
	}
	if (out_map.size() > 0) {
		emit sRectsTransformed(saved_rects, out_map);
	}
	saved_rects = {};
}

bool CanvasScene::transforming() const
{
	return m_saved_rects.size() > 0;
}

void CanvasScene::beginMove(QPointF start)
{
	if (!isEditable()) return;
	m_start_move = start;
	m_moving = true;
	beginTransform();
}

void CanvasScene::previewMove(QPointF preview)
{
	if (!isEditable()) return;
	QPointF diff = preview - m_start_move;
	for (auto &pair : getTransformRects()) {
		CanvasItem *item = pair.first;
		QRectF r = pair.second;
		item->setRect(r.x() + diff.x(), r.y() + diff.y(),
			r.width(), r.height());
	}
}

void CanvasScene::endMove(QPointF end)
{
	if (!isEditable()) return;
	previewMove(end);
	m_moving = false;
	endTransform();
}

bool CanvasScene::moving() const
{
	return m_moving;
}

const std::map<CanvasItem*, QRectF> &CanvasScene::getTransformRects() const
{
	return m_saved_rects;
}

bool CanvasScene::isEditable() const
{
	return m_editable;
}

void CanvasScene::setEditable(bool editable)
{
	m_editable = editable;
	for (auto item : m_items) {
		item->setEditable(editable);
	}
}

void CanvasScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
	QGraphicsScene::mousePressEvent(mouseEvent);
	if (!mouseEvent->isAccepted()) {
		// clear selection
		setSelected({});
	}
}

void CanvasScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
	//endTransform();
}

GiantRectItem::GiantRectItem(QRectF rect)
	: m_rect(rect) {
}
// overrides
QRectF GiantRectItem::boundingRect() const {
	return m_rect;
}
void GiantRectItem::paint(QPainter *painter,
	const QStyleOptionGraphicsItem *option, QWidget *widget) {
	return;
}


CanvasItem::CanvasItem(QGraphicsItem * parent)
	: QAbstractGraphicsShapeItem(parent),
	m_prefers_fixed(false),
	m_border_around(false),
	m_debug_paint(false),
	m_base_height(600.0)
{
	setEditable(false);

	setRect(0, 0, 1, 1);
	setBrush(QBrush(QColor(0, 0, 0, 0)));

	QPen p = pen();
	p.setBrush(QColor(0, 0, 0));
	p.setStyle(Qt::NoPen);
	p.setJoinStyle(Qt::RoundJoin);
	p.setWidthF(0.0);
	setPen(p);
}

QSizeF CanvasItem::size() const
{
	return QSizeF(m_w, m_h);
}

QRectF CanvasItem::rect() const
{
	QPointF p = scenePos();
	return QRectF(p.x(), p.y(), m_w, m_h);
}

void CanvasItem::setRect(QRectF r)
{
	setRect(r.x(), r.y(), r.width(), r.height());
}

void CanvasItem::setRect(double x, double y, double w, double h)
{
	resize(w, h);
	move(x, y);
}

void CanvasItem::resize(double w, double h)
{
	prepareGeometryChange();
	if (m_w == w && m_h == h) {
		return;
	}
	m_w = w;
	m_h = h;
	onResize(QSizeF(w, h));
}

void CanvasItem::move(double x, double y)
{
	setPos(x, y);
}

void CanvasItem::onResize(QSizeF size) {
}

void CanvasItem::onBaseHeightChange(double bh) {
}

void CanvasItem::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
	auto f = flags();
	if (f & QGraphicsItem::ItemIsSelectable) {
		mouseEventSelection(e);
	}
	if (f & QGraphicsItem::ItemIsMovable) {
		if (e->button() == Qt::LeftButton) {
			canvasScene()->beginTransform();
		}
		e->accept();
	}
}

void CanvasItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
	CanvasScene *scene = canvasScene();
	if (scene->moving()) {
		scene->endMove(e->scenePos());
	}
}

void CanvasItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
	if (!editable()) return;
	CanvasScene *scene = canvasScene();
	
	// start moving if not already
	if (!scene->moving()) {
		scene->beginMove(e->lastScenePos());
	}
	if (scene->moving()) {
		scene->previewMove(e->scenePos());
	}
}

bool CanvasItem::editable() const
{
	return flags() & QGraphicsItem::ItemIsMovable;
}

void CanvasItem::setEditable(bool enable)
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

void CanvasItem::setPrefersFixedRatio(bool fixed)
{
	m_prefers_fixed = fixed;
}

bool CanvasItem::prefersFixedRatio()
{
	return m_prefers_fixed;
}

void CanvasItem::setBorderAround(bool around)
{
	m_border_around = around;
}

bool CanvasItem::borderAround()
{
	return m_border_around;
}

CanvasScene * CanvasItem::canvasScene() const
{
	return dynamic_cast<CanvasScene*>(scene());
}

void CanvasItem::debugPaint(bool debug)
{
	m_debug_paint = debug;
}

int CanvasItem::borderWidthPixels() const
{
	return std::lround(pen().widthF() * m_base_height);
}

void CanvasItem::setBorderWidthPixels(int px)
{
	QPen p = pen();
	qreal w = px / m_base_height;
	p.setWidthF(w);
	setPen(p);
}

QColor CanvasItem::borderColor() const
{
	return pen().color();
}

void CanvasItem::setHasBorder(bool has)
{
	QPen p = pen();
	p.setStyle(has ? Qt::SolidLine : Qt::NoPen);
	setPen(p);
}

bool CanvasItem::hasBorder() const
{
	return pen().style() != Qt::NoPen;
}

void CanvasItem::setBorderColor(const QColor & color)
{
	QPen p = pen();
	p.setColor(color);
	setPen(p);
}

QColor CanvasItem::background() const
{
	return brush().color();
}

void CanvasItem::setBackground(const QColor & color)
{
	QBrush b = brush();
	b.setColor(color);
	//if (b.color().alpha() == 0) {
	//	b.setStyle(Qt::NoBrush);
	//}
	//else {
	//	b.setColor(color);
	//	b.setStyle(Qt::BrushStyle::SolidPattern);
	//}
	setBrush(b);
}

QRectF CanvasItem::boundingRect() const
{
	// no border
	if (!hasBorder()) {
		return QRectF(0, 0, m_w, m_h);
	}

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

void CanvasItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	if (m_debug_paint) {
		painter->setPen(Qt::NoPen);
		painter->setBrush(QColor(255,255,0,150));
		painter->drawRect(boundingRect());
	}
	painter->setRenderHint(QPainter::Antialiasing);

	QRectF border_rect;
	double pw = pen().widthF();
	double w = size().width();
	double h = size().height();
	if (!m_border_around || !hasBorder()) {
		// normal rect paint
		border_rect = QRectF(0, 0, w, h);
	}
	else {
		// expanded rect paint
		// so that the border is around the original rect
		// +1/2pw on each side
		border_rect = QRectF(-pw / 2.0, -pw / 2.0, w + pw, h + pw);
	}
	painter->setPen(pen());
	painter->setBrush(brush());
	painter->drawRect(border_rect);

	// paint a dashed selection line
	if (isSelected()) {
		painter->setPen(QPen(Qt::lightGray, 0, Qt::PenStyle::DashLine));
		painter->setBrush(Qt::NoBrush);
		painter->drawRect(QRectF(0, 0, w, h));
	}
}

void CanvasItem::mouseEventSelection(QGraphicsSceneMouseEvent * event)
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
		canvasScene()->setSelected({ this });
		// refocus
		focusItem();
	}
}

double CanvasItem::baseHeight() const {
	return m_base_height;
}

void CanvasItem::setBaseHeight(double bh) {
	m_base_height = bh;
	onBaseHeightChange(bh);
}

QSizeF CanvasItem::scaledSize() const
{
	QSizeF s = size();
	return QSizeF(s.width() * m_base_height, s.height() * m_base_height);
}

double CanvasItem::toPixels(double x) const {
	return x * m_base_height;
}

double CanvasItem::toScene(double pixels) const {
	return pixels / m_base_height;
}

CanvasLabel::CanvasLabel(QGraphicsItem * parent)
	: CanvasItem(parent),
	m_valign(Qt::AlignTop),
	m_style_type(LabelType::LABEL)
{
	m_text = new TextItem(this);
	m_text->show();
	m_text->setTextInteractionFlags(Qt::TextEditorInteraction);

	setBaseHeight(600);
	setFocusProxy(m_text);

	setDocument(m_text->document());
}

QRectF CanvasLabel::boundingRect() const
{
	// There was a bug where text that stuck out rendered
	// incorrectly. Making this bound bigger fixes that.

	// rect bound
	QRectF r = CanvasItem::boundingRect();

	// text bound
	// our scale is [-1,1], text scale is [-300,300]
	// so we have to scale it down
	//QRectF cr = m_text->boundingRect();
	//double w = cr.width() / m_base_height;
	//double h = cr.height() / m_base_height;

	QRectF tr = mapRectFromItem(m_text, m_text->boundingRect());

	return r.united(tr);
}

void CanvasLabel::setEditable(bool enable)
{
	CanvasItem::setEditable(enable);
	// text flags
	if (enable) {
		m_text->setTextInteractionFlags(Qt::TextEditorInteraction);
	}
	else {
		m_text->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
	}
}

void CanvasLabel::realign()
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
	QPointF new_pos(m_text->pos().x(), new_top / baseHeight());
	m_text->setPos(new_pos); // in [-1,1] coordinates
}

void CanvasLabel::setVAlign(Qt::Alignment al)
{
	m_valign = (Qt::Alignment)(Qt::AlignVertical_Mask & al);
	realign();
}

Qt::Alignment CanvasLabel::valign() const
{
	return m_valign;
}

void CanvasLabel::setDocument(QTextDocument * doc)
{
	m_text->setDocument(doc);
	QObject::connect(doc, &QTextDocument::contentsChanged, m_text, [this]() {
		this->realign();
	});
}

QTextDocument * CanvasLabel::document()
{
	return m_text->document();
}

void CanvasLabel::setTextCursor(const QTextCursor & cursor)
{
	m_text->setTextCursor(cursor);
}

QTextCursor CanvasLabel::textCursor() const
{
	return m_text->textCursor();
}

LabelType CanvasLabel::styleType() const
{
	return m_style_type;
}

void CanvasLabel::setStyleType(LabelType type)
{
	m_style_type = type;
}

void CanvasLabel::onResize(QSizeF size)
{
	m_text->setTextWidth(size.width() * baseHeight());
	realign();
}

void CanvasLabel::onBaseHeightChange(double height)
{
	m_text->setScale(1 / height);
	onResize(size());
}

QVariant CanvasLabel::itemChange(GraphicsItemChange change, const QVariant & value)
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
	return CanvasItem::itemChange(change, value);
}

TextItem::TextItem(CanvasLabel * parent)
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
	initSize();
}

void CanvasImage::initSize()
{
	QPixmap p = m_pixmap->pixmap();
	if (p.isNull()) return;

	double bh = baseHeight();
	double max_hr = .8; // max height ratio w/ respect to canvas
	double max_h = bh * max_hr;

	// find reasonable starting height
	double set_h; // big units
	if (p.height() < 50) {
		set_h = 50;
	}
	else if (p.height() < max_h) {
		set_h = p.height();
	}
	else {
		set_h = max_h;
	}

	double ratio;
	ratio = p.width() / (float)p.height();

	// convert to small units and set
	double h = set_h / bh;
	double w = ratio * h;

	// bad number checking
	if (w <= 0.0 || w >= 1.0
		|| h <= 0.0 || h >= 1.0) {
		qWarning() << "size init image w/ invalid dimensions" << w << h;
		w = .2;
		h = .2;
	}
	resize(w, h);
	move(-w / 2, -h / 2);
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
