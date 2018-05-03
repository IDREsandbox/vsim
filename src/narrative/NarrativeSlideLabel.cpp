#include "narrative/NarrativeSlideLabel.h"
#include <qgraphicseffect.h>
#include <QTextCursor>
#include <QTextFormat>
#include <QTextFrame>
#include <QTextFrame>
#include <QTextFrameFormat>
#include "LabelStyleGroup.h"
#include "LabelStyle.h"
#include <QDebug>

NarrativeSlideLabel::NarrativeSlideLabel(QObject *parent)
	: NarrativeSlideItem(parent),
	m_style_type(LabelType::NONE)
{
	m_document = new QTextDocument(this);
	setHtml("New Label");
}

std::string NarrativeSlideLabel::getHtml() const
{
	return m_document->toHtml().toStdString();
}

void NarrativeSlideLabel::setHtml(const std::string &html)
{
	m_document->setHtml(QString::fromStdString(html));
}

void NarrativeSlideLabel::setType(LabelType style)
{
	m_style_type = style;
	emit sStyleChanged();
}

LabelType NarrativeSlideLabel::getType() const
{
	return m_style_type;
}

void NarrativeSlideLabel::setTypeInt(int t)
{
	m_style_type = static_cast<LabelType>(t);
}

int NarrativeSlideLabel::getTypeInt() const
{
	return m_style_type;
}

void NarrativeSlideLabel::setStyleTypeInt(int type)
{
	m_style_type = static_cast<LabelType>(type);
}

int NarrativeSlideLabel::getStyleTypeInt() const
{
	return m_style_type;
}

void NarrativeSlideLabel::setVAlignInt(int al)
{
	m_v_align = static_cast<Qt::Alignment>(al) & Qt::AlignVertical_Mask;
	emit sVAlignChanged(m_v_align);
}

int NarrativeSlideLabel::getVAlignInt() const
{
	return m_v_align;
}

Qt::Alignment NarrativeSlideLabel::getVAlign() const
{
	return m_v_align;
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

