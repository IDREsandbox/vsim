#include "narrative/NarrativeSlideLabels.h"
#include <qgraphicseffect.h>

NarrativeSlideLabels::NarrativeSlideLabels()
	: osg::Node(),
	m_rX(0.5f),
	m_rY(0.5f),
	m_rW(0.25f),
	m_rH(0.2f),
	m_widget_style("background: rgba(0, 0, 0, 70);"),
	m_margin(10)
{
	m_document = new QTextDocument(this);
	m_document->setDefaultStyleSheet("p {color: rgb(255, 255, 255);}");
	m_document->setHtml("New Label");
	m_document->setDocumentMargin(m_margin);

	
}


NarrativeSlideLabels::NarrativeSlideLabels(const NarrativeSlideLabels & n, const osg::CopyOp & copyop)
	: osg::Node(n, copyop),
	m_rX(n.m_rX),
	m_rY(n.m_rY),
	m_rW(n.m_rW),
	m_rH(n.m_rH),
	m_widget_style("background: rgba(0, 0, 0, 70);"),
	m_margin(n.m_margin)
{
	m_document = new QTextDocument(this);
	m_document->setDefaultStyleSheet("p {color: rgb(255, 255, 255);}");
	m_document->setHtml("New Label");
	m_document->setDocumentMargin(m_margin);
}

//TODO: add op to copy from legacy narr

NarrativeSlideLabels::~NarrativeSlideLabels()
{
}

float NarrativeSlideLabels::getrX() const
{
	return m_rX;
}

void NarrativeSlideLabels::setrX(float x)
{
	m_rX = x;
}

int NarrativeSlideLabels::getMargin() const
{
	return m_margin;
}

void NarrativeSlideLabels::setMargin(int m)
{
	m_margin = m;
	m_document->setDocumentMargin(m_margin);
}

float NarrativeSlideLabels::getrY() const
{
	return m_rY;
}

void NarrativeSlideLabels::setrY(float y)
{
	m_rY = y;
}

float NarrativeSlideLabels::getrW() const
{
	return m_rW;
}

void NarrativeSlideLabels::setrW(float w)
{
	m_rW = w;
}

float NarrativeSlideLabels::getrH() const
{
	return m_rH;
}

void NarrativeSlideLabels::setrH(float h)
{
	m_rH = h;
}

const std::string& NarrativeSlideLabels::getText() const
{
	m_text = m_document->toHtml().toStdString();
	return m_text;
}

void NarrativeSlideLabels::setText(const std::string& text)
{
	m_document->setHtml(QString::fromStdString(text));
}

const std::string& NarrativeSlideLabels::getStyle() const
{
	m_style = m_document->defaultStyleSheet().toStdString();
	return m_style;
}

void NarrativeSlideLabels::setStyle(const std::string& style)
{
	m_document->setDefaultStyleSheet(QString::fromStdString(style));
}

const std::string & NarrativeSlideLabels::getWidgetStyle() const
{
	return m_widget_style;
}

void NarrativeSlideLabels::setWidgetStyle(const std::string & widgetStyle)
{
	m_widget_style = widgetStyle;
}

void NarrativeSlideLabels::move(float x, float y)
{
	if (x == m_rX && y == m_rY) return;
	m_rX = x;
	m_rY = y;
	emit sMoved(x, y);
}

void NarrativeSlideLabels::resize(float w, float h)
{
	if (w == m_rW && h == m_rH) return;
	m_rW = w;
	m_rH = h;
	emit sResized(w, h);
}

QTextDocument * NarrativeSlideLabels::getDocument() const
{
	return m_document;
}

NarrativeSlideLabels::MoveCommand::MoveCommand(NarrativeSlideLabels *label, float newx, float newy, QUndoCommand *parent)
	: QUndoCommand(parent),
	m_label(label),
	m_newx(newx),
	m_newy(newy)
{
	m_oldx = label->getrX();
	m_oldy = label->getrY();
}

void NarrativeSlideLabels::MoveCommand::undo()
{
	m_label->move(m_oldx, m_oldy);
}

void NarrativeSlideLabels::MoveCommand::redo()
{
	m_label->move(m_newx, m_newy);
}

NarrativeSlideLabels::ResizeCommand::ResizeCommand(NarrativeSlideLabels *label, float neww, float newh, QUndoCommand *parent)
	: QUndoCommand(parent),
	m_label(label),
	m_neww(neww),
	m_newh(newh)
{
	m_oldw = label->getrW();
	m_oldh = label->getrH();
}

void NarrativeSlideLabels::ResizeCommand::undo()
{
	m_label->resize(m_oldw, m_oldh);
}

void NarrativeSlideLabels::ResizeCommand::redo()
{
	m_label->resize(m_neww, m_newh);
}

NarrativeSlideLabels::DocumentEditWrapperCommand::DocumentEditWrapperCommand(QTextDocument *doc, QUndoCommand *parent)
	: QUndoCommand(parent),
	m_doc(doc)
{
}
void NarrativeSlideLabels::DocumentEditWrapperCommand::undo()
{
	m_doc->undo();
}
void NarrativeSlideLabels::DocumentEditWrapperCommand::redo()
{
	m_doc->redo();
}

