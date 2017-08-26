#include <QtWidgets/QWidget>
#include <QtWidgets>
#include <QtGui/QMouseEvent>
#include <iostream>
#include <QtCore/QBasicTimer>
#include "dragLabel.h"
#include "mrichtextedit.h"
#include "dragLabelInput.h"

//constructor for novel generation
dragLabel::dragLabel(labelCanvas* parent, std::string style) 
	: QTextEdit(QString::fromStdString("New Label"), parent)
{
	setStyleSheet(QString::fromStdString(style));
	QTextDocument* temp = this->document();
	temp->setDefaultStyleSheet("p {" + this->styleSheet().split(';').at(0) + ";" + this->styleSheet().split(';').at(2) + ";" + this->styleSheet().split(';').at(5) + ";}");
	temp->setHtml("<p>" + temp->toHtml() + "</p>");

	par = parent;

	this->setGeometry((par->size().width() / 2) - 125, (par->size().height() / 2) - 55, 250, 110);

	//size ratios
	ratioHeight = 1.0 - float(float(par->size().height() - this->size().height()) / par->size().height());
	ratioWidth = 1.0 - float(float(par->size().width() - this->size().width()) / par->size().width());

	//position ratios
	ratioY = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
	ratioX = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());

	dragEdge = 0;
	//scaleFactor = std::max(float(1.0), float(float(par->size().height()) / float(720)));

	setFrameShape(QFrame::NoFrame);

	this->setWordWrapMode(QTextOption::WordWrap);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

//constructor for generation from data
dragLabel::dragLabel(std::string str, std::string style, labelCanvas* parent, float rH, float rW, float rY, float rX) 
	: QTextEdit(QString::fromStdString(str), parent), ratioHeight(rH), ratioWidth(rW), ratioY(rY), ratioX(rX)
{
	setStyleSheet(QString::fromStdString(style));
	QTextDocument* temp = this->document();
	temp->setDefaultStyleSheet("p {" + this->styleSheet().split(';').at(0) + ";" + this->styleSheet().split(';').at(2) + ";" + this->styleSheet().split(';').at(5) + ";}");
	temp->setHtml("<p>" + temp->toHtml() + "</p>");

	par = parent;

	int newW = std::round(float(par->size().width() * ratioWidth));
	int newH = std::round(float(par->size().height() * ratioHeight));

	int newX = std::round(float(par->size().width() * ratioX));
	int newY = std::round(float(par->size().height() * ratioY));

	this->setGeometry(newX, newY, newW, newH);
	//scaleFactor = std::max(float(1.0), float(float(par->size().height()) / float(720)));

	dragEdge = 0;

	setFrameShape(QFrame::NoFrame);

	this->setWordWrapMode(QTextOption::WordWrap);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	//this->setFrameStyle(QFrame::NoFrame);
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

	QTextEdit::mousePressEvent(event);
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

		this->setText(text);
		QTextDocument* temp = this->document();
		temp->setDefaultStyleSheet("p {" + this->styleSheet().split(';').at(0) + ";" + this->styleSheet().split(';').at(2) + ";" + this->styleSheet().split(';').at(5) + ";}");
		temp->setHtml("<p>" + temp->toHtml() + "</p>");
		emit sTextSet(text, m_index);

		delete setTextDg;
	}

	else {
		timer.start(300, this);
		par->lastSelected = m_index;
	}

	QTextEdit::mouseReleaseEvent(event);
}

void dragLabel::timerEvent(QTimerEvent *event) {
	timer.stop();
}

void dragLabel::keyReleaseEvent(QKeyEvent* event)
{
	emit sTextSet(this->toHtml(), m_index);
	QTextEdit::keyReleaseEvent(event);
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

	QTextEdit::mouseMoveEvent(event);
}

//void dragLabel::resizeEvent(QResizeEvent* event)
//{
	//ValignMiddle(this);
//}

//void dragLabel::showEvent(QShowEvent* event)
//{
	//ValignMiddle(this);
//}

void dragLabel::canvasResize()
{ //must call this inside of canvas' resizeEvent handler
	int newW = std::round(float(par->size().width() * ratioWidth));
	int newH = std::round(float(par->size().height() * ratioHeight));

	this->resize(newW, newH);

	int newX = std::round(float(par->size().width() * ratioX));
	int newY = std::round(float(par->size().height() * ratioY));

	this->move(newX, newY);
}

void dragLabel::paintEvent(QPaintEvent * event)
{
	QTextEdit::paintEvent(event);
	//zoomIn(10);
//	QTextEdit::paintEvent(event);
//	//QPainter painter(viewport());
//	//painter.scale(scaleFactor, scaleFactor);
//	//
//	//ValignMiddle(this);
//
//	//QTextDocument* temp = this->document();
//
//	//QTextOption textOption(temp->defaultTextOption());
//	//temp->setTextWidth(this->size().width() / scaleFactor);
//	//temp->setDefaultStyleSheet("p {" + this->styleSheet().split(';').at(0) + ";" + this->styleSheet().split(';').at(5) + ";}");
//	//temp->setHtml("<p>" + temp->toHtml() + "</p>");
//	//textOption.setWrapMode(QTextOption::WordWrap);
//	//temp->setDefaultTextOption(textOption);
//
//	//
//	//temp->drawContents(&painter, this->contentsRect());
//
//	//QRect r = cursorRect();
//	//r.setWidth(1);
//	//painter.fillRect(r, Qt::SolidPattern);
}

void dragLabel::ValignMiddle(QTextEdit* pTextEdit)
{
	float nDocHeight = (float)pTextEdit->document()->size().height();
	if (nDocHeight == 0)						
		return;
	float nCtrlHeight = (float)pTextEdit->height();				
													
	QTextFrame * pFrame = pTextEdit->document()->rootFrame();
	QTextFrameFormat frameFmt = pFrame->frameFormat();

	float nTopMargin = (float)frameFmt.topMargin();			
	float nBBDocH = (float)(nDocHeight - nTopMargin);				
													
	if (nCtrlHeight <= nBBDocH)
		nTopMargin = 2;
	else
		nTopMargin = (nCtrlHeight - nBBDocH) / 2 - 2; //(2*std::pow(scaleFactor, 5))
	frameFmt.setTopMargin(nTopMargin); //scaleFactor

	pFrame->setFrameFormat(frameFmt);			
}