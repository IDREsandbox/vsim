#include "narrative/NarrativeSlideLabel.h"
#include <qgraphicseffect.h>
#include <QTextCursor>
#include <QTextFormat>
#include <QTextFrame>
#include <QTextFrame>
#include <QTextFrameFormat>
#include "LabelStyleGroup.h"


NarrativeSlideLabel::NarrativeSlideLabel()
	: m_style(0)
{
	m_document = new QTextDocument(this);
	setHtml("New Label");
	setStyle(0);
}

const std::string &NarrativeSlideLabel::getHtml() const
{
	m_text = m_document->toHtml().toStdString();
	return m_text;
}

void NarrativeSlideLabel::setHtml(const std::string &html)
{
	m_document->setHtml(QString::fromStdString(html));
}

void NarrativeSlideLabel::setStyle(int style)
{
	m_style = style;
	emit sStyleChanged();
}

int NarrativeSlideLabel::getStyle() const
{
	return m_style;
}

void NarrativeSlideLabel::applyStyle(LabelStyle *style)
{
	QTextCursor cursor = QTextCursor(m_document);
	cursor.select(QTextCursor::Document);
	cursor.setCharFormat(style->m_char_format);
	cursor.setBlockFormat(style->m_block_format);
	m_document->rootFrame()->setFrameFormat(style->m_frame_format);
	// TODO: vertical alignment
	setBackground(style->backgroundColor());
}

QTextDocument * NarrativeSlideLabel::getDocument() const
{
	return m_document;
}

NarrativeSlideLabel::DocumentEditWrapperCommand::DocumentEditWrapperCommand(QTextDocument *doc, QUndoCommand *parent)
	: QUndoCommand(parent),
	m_doc(doc)
{
}
void NarrativeSlideLabel::DocumentEditWrapperCommand::undo()
{
	m_doc->undo();
}
void NarrativeSlideLabel::DocumentEditWrapperCommand::redo()
{
	m_doc->redo();
}

