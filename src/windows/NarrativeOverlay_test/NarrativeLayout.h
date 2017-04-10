#ifndef CARD_H
#define CARD_H

#include <QtWidgets>
#include <QList>

class CardLayout : public QLayout
{
	Q_OBJECT
public:
	CardLayout(QWidget *parent, int dist) : QLayout(parent) {};
	//CardLayout(QLayout *parent, int dist) : QLayout(parent) {};
	CardLayout(int dist) : QLayout() {};
	~CardLayout();

	void addItem(QLayoutItem *item);
	QSize sizeHint() const;
	QSize minimumSize() const;
	int count() const;
	QLayoutItem *itemAt(int) const;
	QLayoutItem *takeAt(int);
	void setGeometry(const QRect &rect);

private:
	QList<QLayoutItem*> list;
};
#endif