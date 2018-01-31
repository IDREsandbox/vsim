#ifndef NARRATIVESLIDELABEL_H
#define NARRATIVESLIDELABEL_H

#include <QTextDocument>
#include <string>

#include "narrative/NarrativeSlideItem.h"

class NarrativeSlideLabel : public NarrativeSlideItem
{
	Q_OBJECT
public:
	NarrativeSlideLabel(); 

	NarrativeSlideLabel(const NarrativeSlideLabel& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}
	virtual ~NarrativeSlideLabel() {}
	META_Node(, NarrativeSlideLabel);

	const std::string &getHtml() const;
	void setHtml(const std::string &html);

	void setStyle(int style);
	int getStyle() const;

	QTextDocument *getDocument() const;

signals:
	//void sEdited();
	void sStyleChanged();

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
	int m_style;

	// This is a temporaries for the serializer, the actual data is stored in the document
	// the issue is that const std::string& are references to strings that dont exists
	mutable std::string m_text;
};

#endif
