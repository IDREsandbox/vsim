#include "CanvasScene.h"
#include "narrative/CanvasScene.h"
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
	m_base_height(800)
{
	m_giant_rect = new GiantRectItem(QRectF(-10, -10, 20, 20));
	QGraphicsScene::addItem(m_giant_rect);
}

CanvasScene::~CanvasScene()
{
	// ensures we don't double delete
	for (auto item : m_items) {
		QGraphicsScene::removeItem(item.get());
	}
}

void CanvasScene::addItem(std::shared_ptr<RectItem> item)
{
	m_items.insert(item);
	QGraphicsScene::addItem(item.get());
}

void CanvasScene::removeItem(RectItem *item)
{
	QGraphicsScene::removeItem(item);
	std::set<SharedItem, ItemCompare>::iterator it = m_items.find(item);
	if (it == m_items.end()) return;
	m_items.erase(it);
}

std::set<RectItem*> CanvasScene::getSelectedRects() const
{
	std::set<RectItem*> rects_out;

	for (auto item : selectedItems()) {
		RectItem *rect = dynamic_cast<RectItem*>(item);
		if (rect) rects_out.insert(rect);
	}
	return rects_out;
}

void CanvasScene::setSelectedRects(const std::set<RectItem*>& items)
{
	// deselect items
	for (auto item : selectedItems()) {
		RectItem *rect = dynamic_cast<RectItem*>(item);
		if (!rect) continue;
		if (items.find(rect) == items.end()) {
			rect->setSelected(false);
		}
	}

	// select items that should be
	for (auto item : items) {
		item->setSelected(true);
	}
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

		emit sRectsTransformed(saved_rects, out_map);
	}
}

const std::map<RectItem*, QRectF> &CanvasScene::getTransformRects() const
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
		setSelectedRects({});
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


RectItem::RectItem(QGraphicsItem * parent)
	: QAbstractGraphicsShapeItem(parent),
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

void RectItem::onResize(QSizeF size) {
}

void RectItem::onBaseHeightChange(double bh) {
}

void RectItem::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	QAbstractGraphicsShapeItem::mousePressEvent(mouseEvent);
	mouseEventSelection(mouseEvent);
	mouseEvent->accept();
}

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

void RectItem::debugPaint(bool debug)
{
	m_debug_paint = debug;
}

int RectItem::borderWidthPixels() const
{
	return std::lround(pen().widthF() * m_base_height);
}

void RectItem::setBorderWidthPixels(int px)
{
	QPen p = pen();
	p.setStyle(Qt::SolidLine);
	p.setWidthF(px / m_base_height);
	setPen(p);
}

QColor RectItem::borderColor() const
{
	return pen().color();
}

void RectItem::setBorderColor(const QColor & color)
{
	QPen p = pen();
	if (color.alpha() == 0) {
		p.setStyle(Qt::NoPen);
	}
	else {
		p.setStyle(Qt::SolidLine);
		p.setColor(color);
	}
	setPen(p);
}

QColor RectItem::background() const
{
	return brush().color();
}

void RectItem::setBackground(const QColor & color)
{
	QBrush b = brush();
	if (b.color().alpha() == 0) {
		b.setStyle(Qt::NoBrush);
	}
	else {
		b.setColor(color);
		b.setStyle(Qt::BrushStyle::SolidPattern);
	}
	setBrush(b);
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

double RectItem::baseHeight() const {
	return m_base_height;
}

void RectItem::setBaseHeight(double bh) {
	m_base_height = bh;
	onBaseHeightChange(bh);
}

QSizeF RectItem::scaledSize() const
{
	QSizeF s = size();
	return QSizeF(s.width() * m_base_height, s.height() * m_base_height);
}

double RectItem::toPixels(double x) const {
	return x * m_base_height;
}

double RectItem::toScene(double pixels) const {
	return pixels / m_base_height;
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
	QPointF new_pos(m_text->pos().x(), new_top / baseHeight());
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

LabelType TextRect::styleType() const
{
	return m_style_type;
}

void TextRect::setStyleType(LabelType type)
{
	m_style_type = type;
}

void TextRect::onResize(QSizeF size)
{
	m_text->setTextWidth(size.width() * baseHeight());
	realign();
}

void TextRect::onBaseHeightChange(double height)
{
	m_text->setScale(1 / height);
	onResize(size());
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

	double bh = baseHeight();

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
