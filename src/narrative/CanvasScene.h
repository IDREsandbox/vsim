#ifndef CANVASSCENE_H
#define CANVASSCENE_H

#include <set>
#include <vector>
#include <map>
#include <memory>

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QUndoCommand>

#include "LabelType.h"
#include "Command.h"

class CanvasScene;
class TransformManipulator;
class RectItem;
class GiantRectItem;

typedef std::vector<std::pair<RectItem*, QRectF>> ItemRectList;

typedef std::shared_ptr<RectItem> SharedItem;

// so that our std::set<RectItem> can search with raw ptr
struct ItemCompare {
	using is_transparent = void;
	bool operator() (const SharedItem& a, const SharedItem& b) const {
		return a.get() < b.get();
	}
	bool operator() (const SharedItem& a, RectItem *b) const {
		return a.get() < b;
	}
	bool operator() (RectItem *a, const SharedItem& b) const {
		return a < b.get();
	}
};

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

	void addItem(std::shared_ptr<RectItem> item);
	void removeItem(RectItem *item);

	std::set<RectItem*> getSelectedRects() const;
	void setSelectedRects(const std::set<RectItem*> &items);

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

	// get the original rects before a transformation
	const std::map<RectItem*, QRectF> &getTransformRects() const;

	// makes all items editable
	bool isEditable() const;
	void setEditable(bool editable);

	// pixel height of the scene
	void setBaseHeight(double height);
	double baseHeight() const;

	// maps pixel height count to scene size
	double toScene(int px) const;

signals:
	void sRectsTransformed(const std::map<RectItem*, QRectF> &old_rects,
		const std::map<RectItem*, QRectF> &new_rects);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

private:
	std::map<RectItem*, QRectF> m_saved_rects;
	std::set<SharedItem, ItemCompare> m_items;

	bool m_editable;
	double m_base_height;

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
class RectItem : public QAbstractGraphicsShapeItem {
public:
	RectItem(QGraphicsItem *parent = nullptr);

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
	
	// alpha 0 for no border
	void setBorderColor(const QColor &color);
	QColor borderColor() const;

	// alpha 0 for no background
	void setBackground(const QColor &color);
	QColor background() const;

	// overrides
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter *painter,
		const QStyleOptionGraphicsItem *option,
		QWidget *widget = nullptr) override;

public: // commands
	using SetBorderWidthCommad =
		ModifyCommand2<RectItem, int, &borderWidthPixels, &setBorderWidthPixels>;
	using SetBorderColorCommand =
		ModifyCommand2<RectItem, const QColor&, &borderColor, &setBorderColor>;
	using SetBackgroundCommand =
		ModifyCommand2<RectItem, const QColor&, &background, &setBackground>;

protected:
	virtual void onResize(QSizeF size);
	virtual void onBaseHeightChange(double bh);

	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

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
class TextRect : public RectItem {
public:
	TextRect(QGraphicsItem *parent = nullptr);
	enum { Type = UserType + 2 };
	int type() const override { return Type; };

	QRectF boundingRect() const override;

	void setEditable(bool enable) override;

	void realign(); // internal, shifts text to be centered/bottom
	void setVAlign(Qt::Alignment al);

	void setDocument(QTextDocument *doc);
	QTextDocument *document();

	LabelType styleType() const;
	void setStyleType(LabelType);

protected:
	void onResize(QSizeF size) override;
	void onBaseHeightChange(double bh) override;

	QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
	TextItem * m_text;
	Qt::Alignment m_valign;
	LabelType m_style_type;
};

// The internal text item for TextRect
class TextItem : public QGraphicsTextItem {
public:
	TextItem(TextRect *parent);

	QRectF boundingRect() const override;
	QPainterPath shape() const override;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
	void focusOutEvent(QFocusEvent *event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

	TextRect *m_rect;
};

class CanvasImage : public RectItem {
public:
	CanvasImage();
	enum { Type = UserType + 3 };
	int type() const override { return Type; };

	void setPixmap(const QPixmap &p);

protected:
	void onResize(QSizeF size) override;

private:
	QGraphicsPixmapItem *m_pixmap;
};


class DocumentEditWrapperCommand : public QUndoCommand {
public:
	DocumentEditWrapperCommand(QTextDocument *doc, QUndoCommand *parent = nullptr);
	void undo();
	void redo();
private:
	QTextDocument *m_doc;
};

#endif
