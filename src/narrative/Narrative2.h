#ifndef NARRATIVE2_H
#define NARRATIVE2_H

#include <string>
#include <osg/Node>
#include <QObject>
#include "narrative/NarrativeSlide.h"
#include "deprecated/narrative/Narrative.h"

class Narrative2: public QObject, public osg::Group {
	Q_OBJECT

public:
	Narrative2();
	Narrative2(const Narrative2& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	Narrative2(const Narrative *old); // converts old narrative to a new one
	virtual ~Narrative2();

	META_Node(, Narrative2)

	const std::string& getTitle() const;
	void setTitle(const std::string& title);
	const std::string& getAuthor() const;
	void setAuthor(const std::string& author);
	const std::string& getDescription() const;
	void setDescription(const std::string& description);

	bool getLock() const{return m_locked;}
	void setLock(bool lock){ m_locked = lock;}

signals:
	void sNewSlide(int); 
	void sDeleteSlide(int);

public: // COMMANDS
	//class NewSlideCommand : public QUndoCommand {
	//public:
	//	NewSlideCommand(Narrative2 *narrative, int slide_index, QUndoCommand *parent = nullptr);
	//	void undo();
	//	void redo();
	//private:
	//	Narrative2 *m_narrative;
	//	osg::ref_ptr<NarrativeSlide> m_slide;
	//	int m_index;
	//};
	//class DeleteSlideCommand : public QUndoCommand {
	//public:
	//	DeleteSlideCommand(Narrative2 *narrative, int slide_index, QUndoCommand *parent = nullptr);
	//	void undo();
	//	void redo();
	//private:
	//	Narrative2 *m_narrative;
	//	osg::ref_ptr<NarrativeSlide> m_slide;
	//	int m_index;
	//};

	class NewSlideCommand : public NewNodeCommand<Narrative2, NarrativeSlide> {
	public:
		NewSlideCommand(Narrative2 *narrative, int slide_index, QUndoCommand *parent = nullptr)
			: NewNodeCommand(&Narrative2::sNewSlide, &Narrative2::sDeleteSlide, narrative, slide_index, parent)	{}
	};

	class DeleteSlideCommand : public DeleteNodeCommand<Narrative2, NarrativeSlide> {
	public:
		DeleteSlideCommand(Narrative2 *narrative, int slide_index, QUndoCommand *parent = nullptr)
			: DeleteNodeCommand(&Narrative2::sNewSlide, &Narrative2::sDeleteSlide, narrative, slide_index, parent) {}
	};

private:
	std::string m_title;
	std::string m_description;
	std::string m_author;
	bool m_locked;
};

#endif /* NARRATIVE2_H */
