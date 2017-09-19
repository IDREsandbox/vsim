#ifndef NARRATIVESLIDELABELS_H
#define NARRATIVESLIDELABELS_H

#include <osg/Group>
#include <osg/Image>
#include <QDebug>
#include <QTextDocument>
#include <QUndoCommand>

class NarrativeSlideLabels : public QObject, public osg::Node
{
	Q_OBJECT
public:
	NarrativeSlideLabels(); 
	NarrativeSlideLabels(const NarrativeSlideLabels& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	
	//TODO: add op to copy from legacy narr
	
	virtual ~NarrativeSlideLabels();

	META_Node(, NarrativeSlideLabels);

	float getrX() const;
	void setrX(float x);
	float getrY() const;
	void setrY(float y);
	float getrW() const;
	void setrW(float w);
	float getrH() const;
	void setrH(float h);

	const std::string& getText() const;
	void setText(const std::string& text);
	const std::string& getStyle() const;
	void setStyle(const std::string& style);
	const std::string& getWidgetStyle() const;
	void setWidgetStyle(const std::string& widgetStyle);

	void setMargin(int m);
	int getMargin() const;

	void move(float x, float y);
	void resize(float w, float h);

	QTextDocument *getDocument() const;

signals:
	void sMoved(float x, float y);
	void sResized(float w, float h);

public: // COMMANDS

	class MoveCommand : public QUndoCommand {
	public:
		MoveCommand(NarrativeSlideLabels *label, float newx, float newy, QUndoCommand *parent = nullptr);
		void undo();
		void redo();
	private:
		NarrativeSlideLabels *m_label;
		float m_oldx;
		float m_oldy;
		float m_newx;
		float m_newy;
	};

	class ResizeCommand : public QUndoCommand {
	public:
		ResizeCommand(NarrativeSlideLabels *label, float neww, float newh, QUndoCommand *parent = nullptr);
		void undo();
		void redo();
	private:
		NarrativeSlideLabels *m_label;
		float m_oldw;
		float m_oldh;
		float m_neww;
		float m_newh;
	};

	class DocumentEditWrapperCommand : public QUndoCommand {
	public:
		DocumentEditWrapperCommand(QTextDocument *doc, QUndoCommand *parent = nullptr);
		void undo();
		void redo();
	private:
		QTextDocument *m_doc;
	};

public:
	float m_rX;
	float m_rY;
	float m_rW;
	float m_rH;
	int m_margin;

	QTextDocument *m_document;
	// These are just temporaries for the serializer, the actual data is stored in the document
	// the issue is that const std::string& are references to strings that dont exists
	mutable std::string m_text;
	mutable std::string m_style;

	std::string m_widget_style; // for background color
};

#endif /* NARRATIVESLIDELABELS_H */
