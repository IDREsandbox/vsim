#include <QtWidgets/QWidget>
#include <QtWidgets>
#include <QtGui/QMouseEvent>
#include <iostream>
#include <QtCore/QBasicTimer>
#include "MainWindow.h"
#include "dragLabel.h"
#include "mrichtextedit.h"

//constructor for novel generation
dragLabel::dragLabel(QWidget* parent, std::string style) 
	: QLabel(QString::fromStdString("New Label"), parent)
{
	setStyleSheet(QString::fromStdString(style));

	par = parent;
	parSize = par->size();
	oldParSize = par->size();

	this->setGeometry(par->size().width() / 2, par->size().height() / 2, 250, 110);

	ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
	ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());

	dragEdge = 0;

	this->setMargin(10);
	this->setWordWrap(true);
}

//constructor for generation from data
dragLabel::dragLabel(std::string str, std::string style, QWidget* parent) 
	: QLabel(QString::fromStdString(str), parent)
{
	setStyleSheet(QString::fromStdString(style));

	par = parent;
	parSize = par->size();
	oldParSize = par->size();

	//this->setGeometry(x, y, w, h);

	ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
	ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());

	dragEdge = 0;

	this->setMargin(10);
	this->setWordWrap(true);
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

		dragLabelInput *setTextDg = new dragLabelInput(nullptr, this->text());
		setTextDg->setWindowFlags(Qt::WindowSystemMenuHint);
		
		int size = this->font().pixelSize();
		
		int result = setTextDg->exec();
		if (result == QDialog::Rejected) {
			return;
		}

		QString text = setTextDg->getInfo();
		text.replace("font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400;", "font-size:" + QString::number(size) + "px;");
		this->setText(text);
		emit sTextSet(text, m_index);

		delete setTextDg;
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
			emit sPosSet(this->pos(), m_index);

			ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
			ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());
		}

		else if (dragEdge)
		{
			this->resize(this->width() + (event->pos().x() - resizeOffset.x()), this->height() + (event->pos().y() - resizeOffset.y()));
			emit sSizeSet(this->size(), m_index);

			ratioHeight = 1.0 - float(float(par->size().height() - this->pos().y()) / par->size().height());
			ratioWidth = 1.0 - float(float(par->size().width() - this->pos().x()) / par->size().width());

			resizeOffset = event->pos();
		}
	}
}

void dragLabel::updateParSize()
{ //call in canvas' resizeEvent handler for first two resizes
	parSize = par->size();
	oldParSize = par->size();
}

void dragLabel::canvasResize()
{ //must call this inside of canvas' resizeEvent handler
	parSize = par->size();

	float percentX = float((oldParSize.width() - parSize.width())) / oldParSize.width();
	float percentY = float((oldParSize.height() - parSize.height())) / oldParSize.height();

	this->resize(std::round(float(this->width() - (this->width()*percentX))), std::round(float(this->height() - (this->height()*percentY))));
	emit sSizeSet(this->size(), m_index);

	int newX = std::round(float(parSize.width() * ratioWidth));
	int newY = std::round(float(parSize.height() * ratioHeight));

	this->move(newX, newY);
	emit sPosSet(this->pos(), m_index);

	oldParSize = par->size();
}

void dragLabel::resizeEvent(QResizeEvent* event)
{ //font scaling irrespective of style
	QFont font = this->font();
	QRect rect = this->contentsRect();
	QString plainText = QTextDocumentFragment::fromHtml(this->text()).toPlainText();
	int oldSize = font.pixelSize();

	if (this->text().isEmpty())
		return;

	int size = 14;

	while (true)
	{
		QFont tempFont(font);
		tempFont.setPixelSize(size);
		QRect tempRect = QFontMetrics(tempFont).boundingRect(plainText);
		if (tempRect.height() <= rect.height() && tempRect.width() <= rect.width()) 
			size++;
		else
			break;
	}

	size = size - 10;
	if (size < 14)
		size = 14;

	QString text = this->text();
	text.replace("font-size:" + QString::number(oldSize) + "px;", "font-size:" + QString::number(size) + "px;");
	this->setText(text);

	font.setPixelSize(size);
	this->setFont(font);
}
