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
	}

	dragLabel(std::string str, QWidget* parent) : QLabel(QString::fromStdString(str), parent) {
		setStyleSheet("background-color:green; color:blue;");
		par = parent;
		ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
		ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
	}

	~dragLabel()
	{ }

	void dragLabel::mousePressEvent(QMouseEvent *event)
	{
		offset = event->pos();
	}

	void dragLabel::mouseMoveEvent(QMouseEvent *event)
	{
		if (event->buttons() & Qt::LeftButton)
		{
			this->move(mapToParent(event->pos() - offset));
			ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
			//std::cout << ratioHeight << "    ";
			ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
			//std::cout << ratioWidth << "      ";
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
};

#endif // DRAGLABEL_H