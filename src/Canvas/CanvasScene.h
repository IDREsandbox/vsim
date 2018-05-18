#ifndef CANVASSCENE_H
#define CANVASSCENE_H

#include <set>
#include <vector>
#include <map>
#include <memory>

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QUndoCommand>

#include "Canvas/LabelType.h"
#include "Core/Command.h"

class CanvasScene;
class TransformManipulator;
class CanvasItem;
class CanvasLabel;
class GiantRectItem;

typedef std::vector<std::pair<CanvasItem*, QRectF>> ItemRectList;

using SharedItem = std::shared_ptr<CanvasItem>;
struct ItemCompare {
	using is_transparent = void;
	bool operator() (const SharedItem& a, const SharedItem& b) const {
		return a.get() < b.get();
	}
	bool operator() (const SharedItem& a, CanvasItem *b) const {
		return a.get() < b;
	}
	bool operator() (CanvasItem *a, const SharedItem& b) const {
		return a < b.get();
	}
};
using SharedItemSet = std::set<SharedItem, ItemCompare>;

// so that our std::set<CanvasItem> can search with raw ptr

// A graphics scene with transformable items inside
// internal units [-.5, .5] height, [-r, r] width where r is aspect ratio
// text and borders are scaled based on baseHeight()
//   use toScene(px) to map baseheight units to
//   ex. baseHeight of 600px, text height of 10px, will make a line ~1/60 of the screen
class CanvasScene : public QGraphicsScene {
	Q_OBJECT;
public:
	CanvasScene(QObject *parent);
	~CanvasScene() override;

	void addItem(std::shared_ptr<CanvasItem> item);
	void removeItem(CanvasItem *item);

	SharedItemSet items() const;
	std::set<CanvasItem*> getSelected() const;
	void setSelected(const std::set<CanvasItem*> &items);
	SharedItemSet getSelectedShared() const;
	SharedItem toShared(CanvasItem *item) const;

	// if you want to avoid n^2 selection callbacks
	// wait until selectionInProgress() is false
	bool selectionInProgress(); 

	// Use this instead of Scene::clear()
	// - items are shared pointers
	// - don't want to clear the background rect
	void clear();

	// call this before transforming items
	// saves original rects, get with getTransformRects()
	void beginTransform();

	// call this after transforming items
	// emit sRectsTransformed
	void endTransform();

	bool transforming() const;

	void beginMove(QPointF start);
	void previewMove(QPointF preview);
	void endMove(QPointF end);
	bool moving() const;

	// get the original rects before a transformation
	const std::map<CanvasItem*, QRectF> &getTransformRects() const;

	// makes all items editable
	bool isEditable() const;
	void setEditable(bool editable);

	// pixel height of the scene
	void setBaseHeight(double height);
	double baseHeight() const;

	// maps pixel height count to scene size
	double toScene(int px) const;

signals:
	void sAdded(CanvasItem *item);
	void sAboutToRemove(CanvasItem *item);
	void sRemoved(CanvasItem *item);
	void sRectsTransformed(const std::map<CanvasItem*, QRectF> &old_rects,
		const std::map<CanvasItem*, QRectF> &new_rects);
	void sSelectionChanged(); // use this to avoid O(n^2) stuff

	// solutions for getting undo commands out
	// 1. listen for added, removed, maintain QTextDocument list (lots of code) 
	// 2. the scene can do these emissions (simpler?)
	void sDocumentUndoCommandAdded(CanvasLabel *item, QTextDocument *doc);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

private:
	std::map<CanvasItem*, QRectF> m_saved_rects;
	SharedItemSet m_items;

	bool m_editable;
	double m_base_height;
	bool m_selection_in_progress;
	bool m_moving;
	QPointF m_start_move;

	// a giant background shape so that view can always center on (0,0)
	GiantRectItem *m_giant_rect;
};

class GiantRectItem : public QGraphicsItem {
public:
	GiantRectItem(QRectF m_rect);
	// overrides
	QRectF boundingRect() const override;
	void paint(QPainter *painter,
		const QStyleOptionGraphicsItem *option,
		QWidget *widget = nullptr) override;
public:
	QRectF m_rect;
};

// A transformable rectangular item in a CanvasScene
class CanvasItem : public QAbstractGraphicsShapeItem {
public:
	CanvasItem(QGraphicsItem *parent = nullptr);

	enum { Type = UserType + 1 };
	int type() const override { return Type; };

	QSizeF size() const;
	QRectF rect() const;
	void setRect(QRectF r);
	void setRect(double x, double y, double w, double h);
	void resize(double w, double h);
	void move(double x, double y);
	//void setRotation(double deg_ccw);

	bool editable() const;
	virtual void setEditable(bool enable);

	// resize in fixed ratio (for images)
	bool prefersFixedRatio();
	void setPrefersFixedRatio(bool fixed);

	// pixels ~[0,600], size() ~[0,1]
	double baseHeight() const;
	void setBaseHeight(double height);
	QSizeF scaledSize() const; // returns size in terms of pixels
	double toPixels(double x) const;
	double toScene(double pixels) const;

	// paint the border completely around rect
	// i.e. no overlapping with rect (for image borders)
	void setBorderAround(bool around);
	bool borderAround();

	void mouseEventSelection(QGraphicsSceneMouseEvent *event);

	CanvasScene *canvasScene() const;

	void debugPaint(bool debug);

	// properties

	void setBorderWidthPixels(int px);
	int borderWidthPixels() const;
	void setBorderColor(const QColor &color);
	QColor borderColor() const;
	void setHasBorder(bool has);
	bool hasBorder() const;

	// alpha 0 for no background
	void setBackground(const QColor &color);
	QColor background() const;

	// overrides
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter *painter,
		const QStyleOptionGraphicsItem *option,
		QWidget *widget = nullptr) override;

public: // commands
	using SetBorderWidthCommand =
		ModifyCommand2<CanvasItem, int, &borderWidthPixels, &setBorderWidthPixels>;
	using SetBorderColorCommand =
		ModifyCommand2<CanvasItem, const QColor&, &borderColor, &setBorderColor>;
	using SetHasBorderCommand =
		ModifyCommand2<CanvasItem, bool, &hasBorder, &setHasBorder>;
	using SetBackgroundCommand =
		ModifyCommand2<CanvasItem, const QColor&, &background, &setBackground>;

protected:
	virtual void onResize(QSizeF size);
	virtual void onBaseHeightChange(double bh);

	void mousePressEvent(QGraphicsSceneMouseEvent *mouse_event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouse_event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *mouse_event) override;

private:
	double m_w;
	double m_h;
	bool m_prefers_fixed;
	bool m_border_around;
	bool m_debug_paint;
	double m_base_height;
	//bool m_editable;
};

// A transformable text item in a CanvasScene
class TextItem;
class CanvasLabel : public CanvasItem {
public:
	CanvasLabel(QGraphicsItem *parent = nullptr);
	enum { Type = UserType + 2 };
	int type() const override { return Type; };

	QRectF boundingRect() const override;

	void setEditable(bool enable) override;

	void realign(); // internal, shifts text to be centered/bottom
	void setVAlign(Qt::Alignment al);
	Qt::Alignment valign() const;

	void setDocument(QTextDocument *doc);
	QTextDocument *document();

	void setTextCursor(const QTextCursor &cursor);
	QTextCursor textCursor() const;

	LabelType styleType() const;
	void setStyleType(LabelType);

public: // commands
	using SetVAlignCommand =
		ModifyCommand2<CanvasLabel, Qt::Alignment, &valign, &setVAlign>;
	using SetStyleTypeCommand =
		ModifyCommand2<CanvasLabel, LabelType, &styleType, &setStyleType>;

protected:
	void onResize(QSizeF size) override;
	void onBaseHeightChange(double bh) override;

	QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
	TextItem * m_text;
	Qt::Alignment m_valign;
	LabelType m_style_type;
};

// The internal text item for CanvasLabel
class TextItem : public QGraphicsTextItem {
public:
	TextItem(CanvasLabel *parent);

	QRectF boundingRect() const override;
	QPainterPath shape() const override;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
	void focusOutEvent(QFocusEvent *event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

	CanvasLabel *m_rect;
};

class CanvasImage : public CanvasItem {
public:
	CanvasImage();
	enum { Type = UserType + 3 };
	int type() const override { return Type; };

	void setPixmap(const QPixmap &p);

	// finds a reasonable starting size and position
	void initSize();

protected:
	void onResize(QSizeF size) override;

private:
	QGraphicsPixmapItem *m_pixmap;
};


#endif
