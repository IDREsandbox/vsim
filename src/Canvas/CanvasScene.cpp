#include "CanvasScene.h"
#include "Canvas/CanvasScene.h"
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QTextDocument>
#include <QPainter>
#include <QKeyEvent>
#include <QTextEdit>
#include <QClipboard>
#include <QMimeData>
#include <QMenu>
#include <QApplication>
#include <QGraphicsSceneContextMenuEvent>
#include "FocusFilter.h"
#include "Core/WeakObject.h"

CanvasScene::CanvasScene(QObject *parent)
	: QGraphicsScene(parent),
	m_editable(false),
	m_base_height(600.0),
	m_base_width(800.0),
	m_selection_in_progress(false),
	m_moving(false)
{
	m_weak_container = new WeakObject(this);
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

void CanvasScene::copy(const CanvasScene &other)
{
	clear();
	for (const auto item : other.m_item_list) {
		CanvasItem *copy = item->clone();
		SharedItem new_item = std::shared_ptr<CanvasItem>(copy);
		addItem(new_item);
	}
}

void CanvasScene::addItem(std::shared_ptr<CanvasItem> item)
{
	m_items.insert(item);
	m_item_list.push_back(item);
	QGraphicsScene::addItem(item.get());
	item->setBaseHeight(m_base_height);
	item->setEditable(m_editable);

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

	// cleanup scene
	QGraphicsScene::removeItem(item);

	// cleanup set
	auto it = m_items.find(item);
	if (it == m_items.end()) return;
	SharedItem shared = *it;
	m_items.erase(it);

	// cleanup list
	auto &v = m_item_list;
	v.erase(std::remove(v.begin(), v.end(), shared), v.end());

	emit sRemoved(item);
}

std::vector<SharedItem> CanvasScene::itemList() const
{
	return m_item_list;
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

void CanvasScene::setBaseWidth(double height)
{
	m_base_width = height;
}

double CanvasScene::baseHeight() const
{
	return m_base_height;
}

double CanvasScene::toScene(int px) const
{
	return px / m_base_height;
}

QPointF CanvasScene::toScene(QPointF pt) const
{
	QPointF mid = QPointF(m_base_width / 2, m_base_height / 2);
	return (pt - mid) / m_base_height;
}

QRectF CanvasScene::toScene(QRectF rect) const
{
	QPointF pt = toScene(rect.topLeft());
	QSizeF size = QSizeF(
		toScene(rect.width()),
		toScene(rect.height()));
	return QRectF(pt, size);
}

double CanvasScene::fromScene(double x) const
{
	return m_base_height * x;
}

QPointF CanvasScene::fromScene(QPointF pt) const
{
	QPointF mid = QPointF(m_base_width / 2, m_base_height / 2);
	return (pt * m_base_height) + mid;
}

QRectF CanvasScene::fromScene(QRectF rect) const
{
	QPointF pt = fromScene(rect.topLeft());
	QSizeF s = QSizeF(fromScene(rect.width()),
		fromScene(rect.height()));
	return QRectF(pt, s);
}

WeakObject * CanvasScene::weakContainer() const
{
	return m_weak_container;
}

void CanvasScene::clear()
{
	// do we need to clear the scene? idk
	// if stuff is still in the stack then yes
	// remove items from this scene
	auto items = m_items; // copy for safe iteration
	for (auto item : items) {
		removeItem(item.get());
	}

	m_items.clear();
	m_item_list.clear();
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
	m_saved_rects.clear();
}

bool CanvasScene::transforming() const
{
	return m_saved_rects.size() > 0;
}

void CanvasScene::beginRectTransform()
{
	if (!isEditable()) return;
	if (transforming()) return;
	m_rect_transforming = true;
	beginTransform();
}

void CanvasScene::previewRectTransform(QRectF rect)
{
	if (!isEditable()) return;
	if (!m_rect_transforming) return;

	QRectF old = getSavedRect();

	for (auto &pair : getTransformRects()) {
		// convert everything to ratios
		CanvasItem *item = pair.first;
		QRectF ro = pair.second;
		double xr = (ro.x() - old.x()) / old.width();
		double yr = (ro.y() - old.y()) / old.height();
		double wr = ro.width() / old.width();
		double hr = ro.height() / old.height();
		QRectF rrect(xr, yr, wr, hr); // ratio rect

		// use those ratios to figure out new rect
		double x2 = (xr * rect.width()) + rect.x();
		double y2 = (yr * rect.height()) + rect.y();
		double w2 = wr * rect.width();
		double h2 = hr * rect.height();

		item->setRect(x2, y2, w2, h2);
	}
}

void CanvasScene::endRectTransform(QRectF rect)
{
	if (!isEditable()) return;
	previewRectTransform(rect);
	m_rect_transforming = false;
	endTransform();
}

void CanvasScene::beginMove(QPointF start)
{
	if (!isEditable()) return;
	if (m_moving) return;
	if (start.isNull()) {
		m_start_move = getSelectedRect().topLeft();
	}
	else {
		m_start_move = start;
	}
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
	if (!transforming()) {
		beginMove();
	}
	if (end.isNull()) end = getSelectedRect().topLeft();
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

QRectF CanvasScene::getSavedRect() const
{
	QRectF rect;
	for (auto &pair : m_saved_rects) {
		rect = rect.united(pair.second);
	}
	return rect;
}

QRectF CanvasScene::getSelectedRect() const
{
	QRectF rect;
	for (auto item : m_items) {
		if (item->isSelected()) {
			rect = rect.united(item->rect());
		}
	}
	return rect;
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
	if (!editable) {
		setSelected({});
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

void CanvasScene::keyPressEvent(QKeyEvent * e)
{
	QGraphicsScene::keyPressEvent(e);

	// This prevents arrow keys from shifting the viewport around
	// It's a really indirect place to put it. Why here? because it works...
	// If you filter out on QGraphicsView::keyPressEvent() or install an event
	// filter then it prevents the presses from reaching CanvasLabels.
	int key = e->key();
	if (key == Qt::Key_Left
		|| key == Qt::Key_Right
		|| key == Qt::Key_Down
		|| key == Qt::Key_Up) {
		e->accept();
	}
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
	m_base_height(600.0),
	m_border_width(0)
{
	setEditable(false);

	setRect(0, 0, 1, 1);
	setBrush(QBrush(QColor(0, 0, 0, 0)));

	QPen p = pen();
	p.setBrush(QColor(0, 0, 0));
	p.setStyle(Qt::NoPen);
	p.setJoinStyle(Qt::PenJoinStyle::MiterJoin);
	setPen(p);
	updateBorderWidth();

	// causes some issue w/ right click menu > right click out
	// but only in vsim and not canvas experiment
	// i don't really get it
	//setCacheMode(QGraphicsItem::CacheMode::DeviceCoordinateCache);
}

CanvasItem::~CanvasItem()
{
}

void CanvasItem::copy(const CanvasItem & other)
{
	setBackground(other.background());
	m_border_around = other.m_border_around;
	m_border_width = other.m_border_width;
	setHasBorder(other.hasBorder());
	setRect(other.rect());
}

CanvasItem *CanvasItem::clone() const
{
	auto *n = new CanvasItem();
	n->copy(*this);
	return n;
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

void CanvasItem::setRect2(const QRectF & r)
{
	setRect(r);
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
			//canvasScene()->beginMove(e->scenePos());
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

void CanvasItem::updateBorderWidth()
{
	QPen p = pen();
	qreal w = m_border_width / m_base_height;
	p.setWidthF(w);
	setPen(p);
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
	return qRound(pen().widthF() * m_base_height);
}

void CanvasItem::setBorderWidthPixels(int px)
{
	m_border_width = px;
	updateBorderWidth();
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
	else if ((left || right) && !selected) {
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
	updateBorderWidth();
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
	m_text->setOpenExternalLinks(true);

	setBaseHeight(600);
	setFocusProxy(m_text);

	setDocument(m_text->document());
}

void CanvasLabel::copy(const CanvasLabel & other)
{
	CanvasItem::copy(other);
	m_valign = other.m_valign;
	m_style_type = other.m_style_type;
	setHtml(other.html());
}

CanvasLabel * CanvasLabel::clone() const
{
	auto *n = new CanvasLabel;
	n->copy(*this);
	return n;
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

QTextDocument * CanvasLabel::document() const
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

std::string CanvasLabel::html() const
{
	return document()->toHtml().toStdString();
}

QRectF CanvasLabel::rectCenteredPixels(int w, int h)
{
	w = std::max(w, 1);
	h = std::max(h, 1);
	QRectF r = rect();
	QPointF center = r.center();
	r.setSize(QSizeF(
		w / baseHeight(),
		h / baseHeight()));
	r.moveCenter(center);
	return r;
}

void CanvasLabel::setHtml(const std::string & s)
{
	document()->setHtml(QString::fromStdString(s));
}

TextItem * CanvasLabel::textItem() const
{
	return m_text;
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
	else if (change == GraphicsItemChange::ItemSceneHasChanged) {
		auto *scene = canvasScene();
		if (scene) m_text->setParent(scene->weakContainer());
	}
	return CanvasItem::itemChange(change, value);
}

TextItem::TextItem(CanvasLabel * parent)
	: QGraphicsTextItem(parent),
	m_rect(parent)
{
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

	// default font
	auto f = document()->defaultFont();
	f.setHintingPreference(QFont::PreferNoHinting);
	document()->setDefaultFont(f);
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

void TextItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *e)
{
	if (!m_rect->editable()) {
		return;
	}

	float old_width = document()->textWidth();

	QTextEdit temp;
	temp.setDocument(document());
	temp.setTextCursor(textCursor());
	temp.setTextInteractionFlags(textInteractionFlags());
	QMenu *m2 = temp.createStandardContextMenu();

	if (m2) {
		// remove undo/redo actions because they break things
		auto actions = m2->actions();
		m2->removeAction(actions[1]);
		m2->removeAction(actions[0]);

		// restore some document things because temp messes them up
		document()->setTextWidth(old_width);

		QMenu m3(e->widget());
		m3.addActions(m2->actions());

		QAction *paste = m3.addAction("Paste as plain text");
		connect(paste, &QAction::triggered, this, [this]() {
			auto cursor = textCursor();
			const QClipboard *clipboard = QApplication::clipboard();
			const QMimeData *mimeData = clipboard->mimeData();
			if (mimeData->hasText()) {
				cursor.insertText(mimeData->text());
			}
		});
		m3.exec(e->screenPos());
		setTextCursor(temp.textCursor());
		m2->deleteLater();
	}

	// have to restore text width because the QTextEdit
	// changes the text width
	document()->setTextWidth(old_width);

}

CanvasImage::CanvasImage()
{
	setPrefersFixedRatio(true);
	setBorderAround(true);
	m_pixitem = new QGraphicsPixmapItem(this);
	m_pixitem->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
}

void CanvasImage::copy(const CanvasImage & other)
{
	CanvasItem::copy(other);
	setPixmap(other.pixmap());
}

CanvasImage * CanvasImage::clone() const
{
	auto *n = new CanvasImage;
	n->copy(*this);
	return n;
}

QPixmap CanvasImage::pixmap() const
{
	return m_pixitem->pixmap();
}

void CanvasImage::setPixmap(const QPixmap & p)
{
	m_pixitem->setPixmap(p);
	onResize(size());
}

void CanvasImage::initSize()
{
	// the image probably doesn't fit into 800x600, so find a reasonable fit
	// max height: .5
	// max width: .5
	// resize accordingly
	QPixmap p = m_pixitem->pixmap();
	if (p.isNull()) return;

	double bh = baseHeight();
	double max_h = .5; // max height ratio w/ respect to canvas

	// find reasonable starting height
	double set_h; // ratio units
	double p_h = p.height() / bh;
	// too big?
	if (p_h > max_h) {
		set_h = max_h;
	}
	else if (p_h < .05) {
		set_h = .05;
	}
	else {
		set_h = p_h;
	}

	double ratio;
	ratio = p.width() / (float)p.height();

	double set_w = ratio * set_h;

	// bad number checking
	if (set_w <= 0.0
		|| set_h <= 0.0 || set_h >= 1.0) {
		qWarning() << "size init image w/ invalid dimensions" << set_w << set_h;
		set_w = .2;
		set_h = .2;
	}
	resize(set_w, set_h);
	move(-set_w / 2, -set_h / 2);
}

void CanvasImage::onResize(QSizeF size)
{
	// pixmap should fit perfectly into the item rect
	// pixmap width (300)
	// item width (.3)
	// to convert: divide by 1000, (scale by .3/300 = 1/1000)

	if (m_pixitem->pixmap().isNull()) {
		return;
	}

	QPixmap p = m_pixitem->pixmap();

	// scale
	double sx = size.width() / p.width();
	double sy = size.height() / p.height();

	QTransform t;
	t.scale(sx, sy);
	m_pixitem->setTransform(t);
}
