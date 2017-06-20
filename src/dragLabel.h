#ifndef DRAGLABEL_H
#define DRAGLABEL_H

#include <QtWidgets/QWidget>
#include <QtWidgets/qlabel.h>
#include <QtGui/QMouseEvent>
#include <iostream>

class dragLabel : public QLabel
{
	Q_OBJECT

public:
	dragLabel(QWidget* parent) : QLabel(parent) {
		setStyleSheet("background-color:green; color:blue;");
		par = parent;
		parSize = par->size();
		oldParSize = par->size();
		ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
		ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
		dragEdge = 0;
		currSize.setWidth(250);
		currSize.setHeight(250);
	}

	dragLabel(std::string str, QWidget* parent) : QLabel(QString::fromStdString(str), parent) {
		setStyleSheet("background-color:green; color:blue;");
		par = parent;
		parSize = par->size();
		oldParSize = par->size();
		ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
		ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
		dragEdge = 0;
		currSize.setWidth(250);
		currSize.setHeight(250);
	}

	~dragLabel()
	{ }

	void dragLabel::mousePressEvent(QMouseEvent *event)
	{
		currSize = this->size();
		offset = event->pos();
		resizeOffset = event->pos();

		bottomRight = QRect(width() - (width() / 6), height() - (height() / 6), width() / 6, height() / 6); 

		if (bottomRight.contains(event->pos()))
			dragEdge = 1;
		else
			dragEdge = 0;
	}

	void dragLabel::mouseMoveEvent(QMouseEvent *event)
	{
		if (event->buttons() & Qt::LeftButton)
		{
			if (dragEdge == 0)
			{
				this->move(mapToParent(event->pos() - offset));

				ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
				ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
			}

			else if (dragEdge == 1)
			{
				this->resize(this->width() + (event->pos().x() - resizeOffset.x()), this->height() + (event->pos().y() - resizeOffset.y()));
				currSize.setWidth(this->width() + (event->pos().x() - resizeOffset.x()));
				currSize.setHeight(this->height() + (event->pos().y() - resizeOffset.y()));

				ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
				ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());

				resizeOffset = event->pos();
			}
		}
	}

	void dragLabel::updateParSize() {
		parSize = par->size();
		oldParSize = par->size();
	}

	void dragLabel::mainResize() {
		parSize = par->size();

		float percentX = float((oldParSize.width() - parSize.width())) / oldParSize.width();
		float percentY = float((oldParSize.height() - parSize.height())) / oldParSize.height();

		this->resize(std::round(float(this->width() - (this->width()*percentX))), std::round(float(this->height() - (this->height()*percentY))));

		int newX = std::round(float(parSize.width() * ratioWidth));
		int newY = std::round(float(parSize.height() * ratioHeight));

		this->move(newX, newY);

		oldParSize = par->size();
	}

protected:
	QWidget* par;
	QPoint offset;
	QSize currSize;
	QSize parSize;
	QSize oldParSize;
	QPoint resizeOffset;
	float ratioHeight;
	float ratioWidth;
	int dragEdge;
	QRect bottomRight;
};

#endif // DRAGLABEL_H