#include <QtWidgets/QWidget>
#include <QtWidgets>
#include <QtGui/QMouseEvent>
#include <iostream>
#include <QtCore/QBasicTimer>
#include "dragLabel.h"
#include "mrichtextedit.h"
#include "dragLabelInput.h"
#include "narrative/NarrativeSlideLabels.h"

dragLabel::dragLabel(labelCanvas *parent)
	: QTextEdit(parent),
	par(parent),
	ratioHeight(.1),
	ratioWidth(.2),
	ratioX(.1),
	ratioY(.1),
	m_label(nullptr),
	m_dragging(false),
	m_resizing(false)
{

	setFrameShape(QFrame::NoFrame);

	this->setWordWrapMode(QTextOption::WordWrap);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
//
////constructor for novel generation
//dragLabel::dragLabel(labelCanvas* parent, std::string style) 
//	: QTextEdit(QString::fromStdString("New Label"), parent)
//{
//	setStyleSheet(QString::fromStdString(style));
//	QTextDocument* temp = this->document();
//	temp->setDefaultStyleSheet("p {" + this->styleSheet().split(';').at(0) + ";" + this->styleSheet().split(';').at(2) + ";" + this->styleSheet().split(';').at(5) + ";}");
//	temp->setHtml("<p>" + temp->toHtml() + "</p>");
//
//	par = parent;
//
//	this->setGeometry((par->size().width() / 2) - 125, (par->size().height() / 2) - 55, 250, 110);
//
//	//size ratios
//	ratioHeight = 1.0 - float(float(par->size().height() - this->size().height()) / par->size().height());
//	ratioWidth = 1.0 - float(float(par->size().width() - this->size().width()) / par->size().width());
//
//	//position ratios
//	ratioY = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
//	ratioX = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
//
//	dragEdge = 0;
//	//scaleFactor = std::max(float(1.0), float(float(par->size().height()) / float(720)));
//
//	setFrameShape(QFrame::NoFrame);
//
//	this->setWordWrapMode(QTextOption::WordWrap);
//	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//}
//
////constructor for generation from data
//dragLabel::dragLabel(std::string str, std::string style, labelCanvas* parent, float rH, float rW, float rY, float rX) 
//	: QTextEdit(QString::fromStdString(str), parent), ratioHeight(rH), ratioWidth(rW), ratioY(rY), ratioX(rX)
//{
//	setStyleSheet(QString::fromStdString(style));
//	QTextDocument* temp = this->document();
//	temp->setDefaultStyleSheet("p {" + this->styleSheet().split(';').at(0) + ";" + this->styleSheet().split(';').at(2) + ";" + this->styleSheet().split(';').at(5) + ";}");
//	temp->setHtml("<p>" + temp->toHtml() + "</p>");
//
//	par = parent;
//
//	int newW = std::round(float(par->size().width() * ratioWidth));
//	int newH = std::round(float(par->size().height() * ratioHeight));
//
//	int newX = std::round(float(par->size().width() * ratioX));
//	int newY = std::round(float(par->size().height() * ratioY));
//
//	this->setGeometry(newX, newY, newW, newH);
//	//scaleFactor = std::max(float(1.0), float(float(par->size().height()) / float(720)));
//
//	dragEdge = 0;
//
//	setFrameShape(QFrame::NoFrame);
//
//	this->setWordWrapMode(QTextOption::WordWrap);
//	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//
//	//this->setFrameStyle(QFrame::NoFrame);
//}

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
	m_drag_start = event->pos();
	par->setSelection(m_index);

	QRect bottomRight = QRect(width() - (width() / 6), height() - (height() / 6), width() / 6, height() / 6);
	int border = 5;
	QRect center_rect = QRect(border, border, width() - 2 * border, height() - 2 * border);
	qDebug() << "mouse press" << event->pos() << center_rect << bottomRight;
	// Check if in the bottom right
	if (bottomRight.contains(event->pos())) {
		qDebug() << "pressed corner";
		m_dragging = true;
		m_resizing = 1;
	}
	// if the press isn't in the center rect, then it's on the edge
	else if (!center_rect.contains(event->pos())) {
		qDebug() << "pressed inside";
		m_dragging = true;
		m_resizing = 0;
	}

	QTextEdit::mousePressEvent(event);
}

void dragLabel::mouseDoubleClickEvent(QMouseEvent * event)
{
	par->setSelection(m_index);
	dragLabelInput *setTextDg = new dragLabelInput(nullptr, this->toHtml());
	setTextDg->setWindowFlags(Qt::WindowSystemMenuHint);

	int size = this->font().pixelSize();

	int result = setTextDg->exec();
	if (result == QDialog::Rejected) {
		return;
	}

	QString text = setTextDg->getInfo();

	QTextDocument *doc = m_label->getDocument();
	// edit the document in an undo-able way
	QTextCursor c(doc);
	c.beginEditBlock();
	c.select(QTextCursor::Document);
	c.removeSelectedText();
	c.insertHtml(text);
}

void dragLabel::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_dragging) {
		// resize event
		if (m_resizing) {
			
			// recalculate percentages
			ratioWidth = size().width() / (float)par->size().width();
			ratioHeight = size().height() / (float)par->size().height();
			qDebug() << "mouse release set size";
			emit sSetSize(ratioWidth, ratioHeight, m_index);
		}
		// move event
		else {
			
			ratioX = size().width() / (float)par->size().width();
			ratioY = size().height() / (float)par->size().height();
			qDebug() << "mouse release set position" << ratioX << ratioY;
			emit sSetPos(ratioX, ratioY, m_index);
		}
	}

	m_dragging = false;

	QTextEdit::mouseReleaseEvent(event);
}

void dragLabel::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton && m_dragging)
	{
		if (!m_resizing)
		{
			qDebug() << "moving";
			// move the label but don't update the data
			this->move(mapToParent(event->pos() - m_drag_start));
		}

		else if (m_resizing)
		{
			qDebug() << "resizing";
			// resize but don't update data
			this->resize(this->width() + (event->pos().x() - m_drag_start.x()), this->height() + (event->pos().y() - m_drag_start.y()));
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

void dragLabel::setPos(float x, float y)
{
	ratioX = x;
	ratioY = y;
	canvasResize();
}

void dragLabel::setSize(float w, float h)
{
	ratioWidth = w;
	ratioHeight = h;
	canvasResize();
}

void dragLabel::setLabel(NarrativeSlideLabels * label)
{
	if (m_label != nullptr) disconnect(m_label, 0, this, 0);
	m_label = label;
	if (label == nullptr) return;

	// initialize
	this->setDocument(label->getDocument());
	ratioWidth = label->getrW();
	ratioHeight = label->getrH();
	ratioX = label->getrX();
	ratioY = label->getrY();
	canvasResize();

	setStyleSheet(label->getDocument()->defaultStyleSheet());
	
	// connections - move and resize
	connect(m_label, &NarrativeSlideLabels::sMoved, this, &dragLabel::setPos);
	connect(m_label, &NarrativeSlideLabels::sResized, this, &dragLabel::setSize);
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