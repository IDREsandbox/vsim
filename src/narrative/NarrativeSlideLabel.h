#ifndef NARRATIVESLIDELABEL_H
#define NARRATIVESLIDELABEL_H

#include <QTextDocument>
#include <string>

#include "narrative/NarrativeSlideItem.h"
#include "LabelType.h"

class LabelStyle;
class NarrativeSlideLabel : public NarrativeSlideItem
{
	Q_OBJECT
public:
	NarrativeSlideLabel(); 

	NarrativeSlideLabel(const NarrativeSlideLabel& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}
	virtual ~NarrativeSlideLabel() {}
	META_Node(, NarrativeSlideLabel);

	// serialization
	const std::string &getHtml() const;
	void setHtml(const std::string &html);

	void setType(LabelType);
	LabelType getType() const;
	void setTypeInt(int t);
	int getTypeInt() const;
	void setStyleTypeInt(int type); // int version for serializer
	int getStyleTypeInt() const;

	void setVAlignInt(int al);
	int getVAlignInt() const;
	Qt::Alignment getVAlign() const;

	// 
	void applyStyle(LabelStyle *style);

	QTextDocument *getDocument() const;

signals:
	//void sEdited();
	void sStyleChanged();
	void sVAlignChanged(Qt::Alignment);

public: // COMMANDS
	class DocumentEditWrapperCommand : public QUndoCommand {
	public:
		DocumentEditWrapperCommand(QTextDocument *doc, QUndoCommand *parent = nullptr);
		void undo();
		void redo();
	private:
		QTextDocument *m_doc;
	};

private:
	QTextDocument *m_document;
	LabelType m_style_type;

	// This is a temporaries for the serializer, the actual data is stored in the document
	// the issue is that const std::string& are references to strings that dont exists
	mutable std::string m_text;

	Qt::Alignment m_v_align;
};

#endif
