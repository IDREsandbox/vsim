#ifndef DRAGLABEL_H
#define DRAGLABEL_H

#include <QtWidgets/QWidget>
#include <QtWidgets/qlabel.h>
#include <QtGui/QMouseEvent>

class dragLabel : public QLabel
{
	Q_OBJECT

public:
	dragLabel(QWidget* parent) : QLabel(parent) {
		setStyleSheet("background-color:green; color:blue;");
		par = parent;
		ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
		ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
		dragEdge = 0;
	}

	dragLabel(std::string str, QWidget* parent) : QLabel(QString::fromStdString(str), parent) {
		setStyleSheet("background-color:green; color:blue;");
		par = parent;
		ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
		ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
		dragEdge = 0;
	}

	~dragLabel()
	{ }

	void dragLabel::mousePressEvent(QMouseEvent *event)
	{
		offset = event->pos();

		topLeft = QRect(0, 0, width() / 6, height() / 6); //dragEdge 1
		topRight = QRect(width() - (width() / 6), 0, width() / 6, height() / 6); //dragEdge 2
		bottomLeft = QRect(0, height() - (height() / 6), width() / 6, height() / 6); //dragEdge 3
		bottomRight = QRect(width() - (width() / 6), height() - (height() / 6), width() / 6, height() / 6); //dragEdge 4

		if (topLeft.contains(event->pos()))
			dragEdge = 1;
		else if (topRight.contains(event->pos()))
			dragEdge = 2;
		else if (bottomLeft.contains(event->pos()))
			dragEdge = 3;
		else if (bottomRight.contains(event->pos()))
			dragEdge = 4;
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
				//std::cout << ratioHeight << "    ";
				ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
				//std::cout << ratioWidth << "      ";
			}

			else if (dragEdge == 1)
			{
				this->resize(this->width() + (event->pos().x() - offset.x()), this->height() + (event->pos().y() - offset.y()));
			}

			else if (dragEdge == 2)
			{
				this->resize(this->width() + (event->pos().x() - offset.x()), this->height() + (event->pos().y() - offset.y()));
			}

			else if (dragEdge == 3)
			{
				this->resize(this->width() + (event->pos().x() - offset.x()), this->height() + (event->pos().y() - offset.y()));
			}

			else if (dragEdge == 4)
			{
				this->resize(this->width() + (event->pos().x() - offset.x()), this->height() + (event->pos().y() - offset.y()));
			}

		}
	}

	float dragLabel::ratioH(){
		return ratioHeight;
	}

	float dragLabel::ratioW(){
		return ratioWidth;
	}
	//void dragLabel::resizeEvent(QResizeEvent *event)
	//{
	//	/*int proportionHeight = 1 - ((parSize.height() - pos().y()) / parSize.height());
	//	int proportionWidth = 1 - ((parSize.width() - pos().x()) / parSize.width());
	//	
	//	parSize = par->size();

	//	int newX = parSize.width() * proportionWidth;
	//	int newY = parSize.height() * proportionHeight;

	//	this->move(mapToParent(QPoint(newX, newY)));*/

	//	this->move(250, 250);
	//}

	//void dragLabel::moveEvent(QMoveEvent *event)
	//{
	//int proportionHeight = 1 - ((parSize.height() - event->oldSize().height()) / parSize.height());
	//int proportionWidth = 1 - ((parSize.width() - event->oldSize().width()) / parSize.width());

	//parSize = par->size();

	//int newX = parSize.width() * proportionWidth;
	//int newY = parSize.height() * proportionHeight;

	//	this->move(mapToParent(newX, newY));		
	//}

protected:
	QWidget* par;
	QPoint offset;
	float ratioHeight;
	float ratioWidth;
	int dragEdge;
	QRect topLeft; 
	QRect bottomLeft;
	QRect topRight;
	QRect bottomRight;
};

#endif // DRAGLABEL_H