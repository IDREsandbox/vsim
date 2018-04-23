#pragma once

#include <QtWidgets/QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMenu>
#include <set>
#include "StackObject.h"

class FastScrollScene;
class FastScrollItem;
class FastScrollBox;

class FastScrollBox : public QFrame {
	Q_OBJECT
public:
	FastScrollBox(QWidget *parent = nullptr);
	
	void setMenu(QMenu *menu);
	void setItemMenu(QMenu *menu);
	void setSpacing(int spacing);
	void refresh();
	void clear();

	// selection
	void selectAll(bool select = true);
	StackObject<FastScrollItem*> *selection() const;

	void insertItems(const std::vector<std::pair<size_t, FastScrollItem*>>& insertions);
	void removeItems(const std::vector<size_t> indices);
	void removeSelected();

	size_t itemCount() const;
	FastScrollItem *item(size_t index) const;

signals:
	void sTouch();
	void sSelectionCleared();

protected:
	void resizeEvent(QResizeEvent *event) override;

	//virtual void sceneMouseEvent(QGraphicsSceneMouseEvent *event);
	virtual void itemMousePressEvent(FastScrollItem *item, QGraphicsSceneMouseEvent *event);
	virtual void itemMouseDoubleClickEvent(FastScrollItem *item, QGraphicsSceneMouseEvent *event);

private:
	void singleSelect(FastScrollItem *item);
	//void shiftSelect();

	class Scene : public QGraphicsScene {
	public:
		Scene(FastScrollBox *box, QObject *parent = nullptr);

	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
		void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
	private:
		FastScrollBox * m_box;
	};

public:
	QGraphicsView *m_view;
	Scene *m_scene;
	StackObject<FastScrollItem*> *m_selection;

	std::vector<FastScrollItem*> m_items;

	QMenu *m_menu;
	QMenu *m_item_menu;

	int m_spacing;
};

class FastScrollItem : public QGraphicsObject {
	Q_OBJECT
public:
	enum { Type = UserType + 1};
	int type() const override { return Type; }

	FastScrollItem();
	void setSize(const QSizeF &size);
	void setNumber(int number);

signals:
	void sSelected(bool select);
	void sMousePressEvent(QGraphicsSceneMouseEvent *event);
	void sMouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void sContextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	//void sShiftSelect();
	//void sRightClick();
	//void sDoubleClick();
	//void sCtrlClick();
	//void sSingleClick();

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget = 0) override;
	QRectF boundingRect() const override;
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

	virtual void onResize();
	virtual void onSelect(bool selected);

public:
//private:
	QSizeF m_size;
	int m_number;
};

class FastTextItem : public QGraphicsItem
{
public:
	FastTextItem(QGraphicsItem *parent = 0);
	void setBoundingRect(qreal x, qreal y, qreal w, qreal h);
	void setText(const QString &inText);
	void setColor(const QColor &color);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
	QRectF boundingRect() const;

private:
	QRectF m_rect;
	//QTextOption m_opt;
	QString m_text;
	QColor m_color;
};
