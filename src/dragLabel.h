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
		parSize = par->size();
		//oldParSize = par->size();
		ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
		ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
		dragEdge = 0;
		currSize.setWidth(250);
		currSize.setHeight(250);
		//center.setX(this->rect().topLeft().x() + currSize.width() / 2);
		//center.setY(this->rect().topLeft().y() - currSize.height() / 2);
	}

	dragLabel(std::string str, QWidget* parent) : QLabel(QString::fromStdString(str), parent) {
		setStyleSheet("background-color:green; color:blue;");
		par = parent;
		parSize = par->size();
		//oldParSize = par->size();
		ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
		ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
		dragEdge = 0;
		currSize.setWidth(250);
		currSize.setHeight(250);
		//center.setX(this->rect().topLeft().x() + currSize.width() / 2);
		//center.setY(this->rect().topLeft().y() - currSize.height() / 2);
	}

	~dragLabel()
	{ }

	void dragLabel::mousePressEvent(QMouseEvent *event)
	{
		currSize = this->size();
		offset = event->pos();
		resizeOffset = event->pos();

		//we only need bottom right, oops
		//topLeft = QRect(0, 0, width() / 6, height() / 6); //dragEdge 1, top left
		//topRight = QRect(width() - (width() / 6), 0, width() / 6, height() / 6); //dragEdge 2, top right
		//bottomLeft = QRect(0, height() - (height() / 6), width() / 6, height() / 6); //dragEdge 3, bottom left
		bottomRight = QRect(width() - (width() / 6), height() - (height() / 6), width() / 6, height() / 6); //dragEdge 4, bottom right

		/* if (topLeft.contains(event->pos()))
			dragEdge = 1;
		else if (topRight.contains(event->pos()))
			dragEdge = 2;
		else if (bottomLeft.contains(event->pos()))
			dragEdge = 3; */
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
				//std::cout << ratioHeight << "    ";
				//std::cout << ratioWidth << "    ";

				//center.setX(this->rect().topLeft().x() + currSize.width() / 2);
				//center.setY(this->rect().topLeft().y() - currSize.height() / 2);
			}
			//only need bottom right for now, so other options are commented out
			/* else if (dragEdge == 1)
			{
				this->resize(this->width() + (event->pos().x() - resizeOffset.x()), this->height() + (event->pos().y() - resizeOffset.y()));
				resizeOffset = event->pos();
			}

			else if (dragEdge == 2)
			{
				this->resize(this->width() + (event->pos().x() - resizeOffset.x()), this->height() + (event->pos().y() - resizeOffset.y()));
				resizeOffset = event->pos();
			}

			else if (dragEdge == 3)
			{
				this->resize(this->width() + (event->pos().x() - resizeOffset.x()), this->height() + (event->pos().y() - resizeOffset.y()));
				resizeOffset = event->pos();
			} */

			else if (dragEdge == 1)
			{
				this->resize(this->width() + (event->pos().x() - resizeOffset.x()), this->height() + (event->pos().y() - resizeOffset.y()));
				currSize.setWidth(this->width() + (event->pos().x() - resizeOffset.x()));
				currSize.setHeight(this->height() + (event->pos().y() - resizeOffset.y()));

				ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
				ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());

				resizeOffset = event->pos();

				//center.setX(this->rect().topLeft().x() + currSize.width() / 2);
				//center.setY(this->rect().topLeft().y() - currSize.height() / 2);
			}

		}
	}

	float dragLabel::ratioH(){
		return ratioHeight;
	}

	float dragLabel::ratioW(){
		return ratioWidth;
	}

	void dragLabel::resizeEvent(QResizeEvent *event) {
		this->resize(currSize.width(), currSize.height());

		parSize = par->size();

		int newX = parSize.width() * ratioWidth;
		int newY = parSize.height() * ratioHeight;

		this->move(newX, newY);

		//float percentX = oldParSize.width() - parSize.width()/oldParSize.width();
		//float percentY = oldParSize.height() - parSize.height()/oldParSize.height();

		//this->resize(this->width() - (this->width()*percentX), this->height() - (this->height()*percentY));

		//oldParSize = par->size();
	}
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
	QPoint center;
	QWidget* par;
	QPoint offset;
	QSize currSize;
	QSize parSize;
	QSize oldParSize;
	QPoint resizeOffset;
	float ratioHeight;
	float ratioWidth;
	int dragEdge;
	QRect topLeft; 
	QRect bottomLeft;
	QRect topRight;
	QRect bottomRight;
};

#endif // DRAGLABEL_H