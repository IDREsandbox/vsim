#ifndef DRAGLABEL_H
#define DRAGLABEL_H

#include <QtWidgets/QWidget>
#include <QtWidgets>
#include <QtGui/QMouseEvent>
#include <iostream>
#include <QtCore/QBasicTimer>


class dragLabel : public QLabel
{
	Q_OBJECT

public:
	dragLabel(QWidget* parent, std::string style) : QLabel(parent) 
	{
		setStyleSheet(QString::fromStdString(style));
		
		par = parent;
		parSize = par->size();
		oldParSize = par->size();
		
		ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
		ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
		
		dragEdge = 0;
		
		this->setMargin(15);
		this->setWordWrap(true);
	}

	dragLabel(std::string str, std::string style, QWidget* parent) : QLabel(QString::fromStdString(str), parent) 
	{
		setStyleSheet(QString::fromStdString(style));

		par = parent;
		parSize = par->size();
		oldParSize = par->size();

		ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
		ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
		
		dragEdge = 0;
		
		this->setMargin(15);
		this->setWordWrap(true);
	}

	~dragLabel()
	{ }

	void dragLabel::mousePressEvent(QMouseEvent *event)
	{
		offset = event->pos();
		resizeOffset = event->pos();

		bottomRight = QRect(width() - (width() / 6), height() - (height() / 6), width() / 6, height() / 6); 

		if (bottomRight.contains(event->pos()))
			dragEdge = 1;
		else
			dragEdge = 0;
	}

	void dragLabel::mouseReleaseEvent(QMouseEvent *event) 
	{
		if (timer.isActive()) {
			timer.stop();
			bool ok;
			QString text = QInputDialog::getText(this, tr("Label Text"),
				tr("Input Text:"), QLineEdit::Normal, this->text(), &ok);
			if (ok && !text.isEmpty())
				this->setText(text);
		}
		else {
			timer.start(300, this);
		}
	}

	void dragLabel::timerEvent(QTimerEvent *event) {
		timer.stop();
	}

	void dragLabel::mouseMoveEvent(QMouseEvent *event)
	{
		if (event->buttons() & Qt::LeftButton)
		{
			if (!dragEdge)
			{
				this->move(mapToParent(event->pos() - offset));

				ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
				ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
			}

			else if (dragEdge)
			{
				this->resize(this->width() + (event->pos().x() - resizeOffset.x()), this->height() + (event->pos().y() - resizeOffset.y()));

				ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
				ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());

				resizeOffset = event->pos();
			}
		}
	}

	void dragLabel::updateParSize() 
	{ //call in mainWindow's resizeEvent handler for first two resizes
		parSize = par->size();
		oldParSize = par->size();
	}

	void dragLabel::mainResize() 
	{ //must call this inside of mainWindow's resizeEvent handler
		parSize = par->size();

		float percentX = float((oldParSize.width() - parSize.width())) / oldParSize.width();
		float percentY = float((oldParSize.height() - parSize.height())) / oldParSize.height();

		this->resize(std::round(float(this->width() - (this->width()*percentX))), std::round(float(this->height() - (this->height()*percentY))));

		int newX = std::round(float(parSize.width() * ratioWidth));
		int newY = std::round(float(parSize.height() * ratioHeight));

		this->move(newX, newY);

		oldParSize = par->size();
	}

	void dragLabel::resizeEvent(QResizeEvent* event) 
	{ //font scaling
		QFont font = this->font();
		QRect cRect = this->contentsRect();

		if (this->text().isEmpty())
			return;

		int fontSize = 14;

		while (true)
		{
			QFont f(font);
			f.setPixelSize(fontSize);
			QRect r = QFontMetrics(f).boundingRect(this->text());
			if (r.height() <= cRect.height() && r.width() <= cRect.width())
				fontSize++;
			else
				break;
		}

		font.setPixelSize(fontSize);
		this->setFont(font);
	}

protected:
	QWidget* par;
	QSize parSize;
	QSize oldParSize;
	
	QPoint offset;
	QPoint resizeOffset;
	
	float ratioHeight;
	float ratioWidth;
	
	bool dragEdge;
	QRect bottomRight;

	QBasicTimer timer;
};

#endif // DRAGLABEL_H