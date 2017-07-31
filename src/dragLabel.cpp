#include <QtWidgets/QWidget>
#include <QtWidgets>
#include <QtGui/QMouseEvent>
#include <iostream>
#include <QtCore/QBasicTimer>
#include "MainWindow.h"
#include "dragLabel.h"
#include "mrichtextedit.h"

//constructor for novel generation
dragLabel::dragLabel(labelCanvas* parent, std::string style) 
	: QTextEdit(QString::fromStdString("New Label"), parent)
{
	setStyleSheet(QString::fromStdString(style));

	par = parent;

	this->setGeometry((par->size().width() / 2) - 125, (par->size().height() / 2) - 55, 250, 110);

	//size ratios
	ratioHeight = 1.0 - float(float(par->size().height() - this->size().height()) / par->size().height());
	ratioWidth = 1.0 - float(float(par->size().width() - this->size().width()) / par->size().width());

	//position ratios
	ratioY = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
	ratioX = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());

	dragEdge = 0;
	scaleFactor = std::max(float(1.0), float(float(par->size().height()) / float(720)));

	this->setWordWrapMode(QTextOption::WordWrap);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

//constructor for generation from data
dragLabel::dragLabel(std::string str, std::string style, labelCanvas* parent, float rH, float rW, float rY, float rX) 
	: QTextEdit(QString::fromStdString(str), parent), ratioHeight(rH), ratioWidth(rW), ratioY(rY), ratioX(rX)
{
	setStyleSheet(QString::fromStdString(style));

	par = parent;

	int newW = std::round(float(par->size().width() * ratioWidth));
	int newH = std::round(float(par->size().height() * ratioHeight));

	int newX = std::round(float(par->size().width() * ratioX));
	int newY = std::round(float(par->size().height() * ratioY));

	this->setGeometry(newX, newY, newW, newH);
	scaleFactor = std::max(float(1.0), float(float(par->size().height()) / float(720)));

	dragEdge = 0;

	this->setWordWrapMode(QTextOption::WordWrap);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	this->setFrameStyle(QFrame::NoFrame);
}

dragLabel::~dragLabel()
{ }

void dragLabel::setIndex(int index)
{
	m_index = index;
}

int dragLabel::getIndex()
{
	return m_index;
}

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

		dragLabelInput *setTextDg = new dragLabelInput(nullptr, this->toHtml());
		setTextDg->setWindowFlags(Qt::WindowSystemMenuHint);
		
		int size = this->font().pixelSize();
		
		int result = setTextDg->exec();
		if (result == QDialog::Rejected) {
			return;
		}

		QString text = setTextDg->getInfo();
		//text.replace("font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400;", "font-size:" + QString::number(size) + "px;");
		this->setText(text);
		emit sTextSet(text, m_index);

		delete setTextDg;

		//recalcFontRatios();
	}

	else {
		timer.start(300, this);
		par->lastSelected = m_index;
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

			ratioY = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
			ratioX = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());

			emit sPosSet(this->pos(), m_index);
		}

		else if (dragEdge)
		{
			this->resize(this->width() + (event->pos().x() - resizeOffset.x()), this->height() + (event->pos().y() - resizeOffset.y()));

			ratioHeight = 1 - float(float(par->size().height() - this->size().height()) / par->size().height());
			ratioWidth = 1 - float(float(par->size().width() - this->size().width()) / par->size().width());

			resizeOffset = event->pos();
			emit sSizeSet(this->size(), m_index);
		}
	}
}

//void dragLabel::recalcFontRatios() {
//	QFont font = this->font();
//	QRect rect = this->contentsRect();
//	QString plainText = this->toHtml();
//
//	int oldSize = font.pixelSize();
//	QFont tempFont(font);
//	tempFont.setPixelSize(oldSize);
//	QRect tempRect = QFontMetrics(tempFont).boundingRect(plainText);
//
//	ratioFHeight = 1 - float(float(rect.height() - tempRect.height()) / rect.height());
//	ratioFWidth = 1 - float(float(rect.width() - tempRect.width()) / rect.width());
//}

void dragLabel::canvasResize()
{ //must call this inside of canvas' resizeEvent handler
	int newW = std::round(float(par->size().width() * ratioWidth));
	int newH = std::round(float(par->size().height() * ratioHeight));

	this->resize(newW, newH);

	int newX = std::round(float(par->size().width() * ratioX));
	int newY = std::round(float(par->size().height() * ratioY));

	this->move(newX, newY);
	
	scaleFactor = std::max(float(1.0), float(float(par->size().height()) / float(720)));

	//QRect rect = this->contentsRect();
	//QFont font = this->font();
	//rect.setWidth(rect.width()*ratioFWidth);
	//rect.setHeight(rect.height()*ratioFHeight);
	//int oldSize = font.pixelSize();

	//if (this->toHtml().isEmpty())
	//	return;

	//int size = 14;
	//QString plainText = this->toHtml();

	//while (true)
	//{
	//	QFont tempFont(font);
	//	tempFont.setPixelSize(size);
	//	QRect tempRect = QFontMetrics(tempFont).boundingRect(plainText);
	//	if (tempRect.height() <= rect.height() && tempRect.width() <= rect.width())
	//		size++;
	//	else
	//		break;
	//}

	//QString text = this->toHtml();
	//text.replace("font-size:" + QString::number(oldSize) + "px;", "font-size:" + QString::number(size) + "px;");
	//this->setText(text);

	//font.setPixelSize(size);
	//this->setFont(font);

	//recalcFontRatios();
}

void dragLabel::paintEvent(QPaintEvent * event)
{
	QPainter painter(viewport());
	painter.scale(scaleFactor, scaleFactor);
	
	QTextDocument* temp = this->document();

	QTextOption textOption(temp->defaultTextOption());
	temp->setTextWidth(this->size().width() / scaleFactor);
	temp->setDefaultStyleSheet("p {" + this->styleSheet().split(';').at(0) + ";}");
	temp->setHtml("<p>" + temp->toHtml() + "</p>");
	textOption.setWrapMode(QTextOption::WordWrap);
	temp->setDefaultTextOption(textOption);

	temp->drawContents(&painter, this->contentsRect());
}
