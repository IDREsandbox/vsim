#ifndef CARD_H
#define CARD_H

#include <QtWidgets>
#include <QList>

class CardLayout : public QLayout
{
	Q_OBJECT
public:
	CardLayout(QWidget *parent, int spacing) : QLayout(parent), m_spacing(spacing) {};
	//CardLayout(QLayout *parent, int dist) : QLayout(parent) {};
	CardLayout(int dist) : QLayout() {};
	~CardLayout();

	// virtual Qt Stuff
	void addItem(QLayoutItem *item);
	QSize sizeHint() const;
	QSize minimumSize() const;
	int count() const;
	QLayoutItem *itemAt(int) const;
	QLayoutItem *takeAt(int);
	void setGeometry(const QRect &rect);

	// 
	void setHeight(int height) { m_height = height; };
	int spacing() const { return m_spacing; };
	void setSpacing(int spacing) { m_spacing = spacing; };
	int getMinWidth() const;

	int boxHeight() const;
	int boxWidth() const;

private:
	QList<QLayoutItem*> list;

	int m_height = 100;
	int m_spacing = 0;

	float m_ratio = 4.0 / 3.0;

};
#endif