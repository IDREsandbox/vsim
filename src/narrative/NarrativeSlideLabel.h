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
	NarrativeSlideLabel(QObject *parent = nullptr);

	// serialization
	std::string getHtml() const;
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

	Qt::Alignment m_v_align;
};

#endif
