#ifndef NARRATIVE2_H
#define NARRATIVE2_H

#include <string>
#include <osg/Node>
#include <QObject>
#include "narrative/NarrativeSlide.h"
#include "deprecated/narrative/Narrative.h"
#include "Group.h"

class Narrative2 : public Group {
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
	void sTitleChanged(const std::string&);
	void sAuthorChanged(const std::string&);
	void sDescriptionChanged(const std::string&);

	void sNewSlide(int);
	void sDeleteSlide(int);

public: // COMMANDS

	class SetTitleCommand : public ModifyCommand<Narrative2, const std::string&> {
	public:
		SetTitleCommand(Narrative2 *nar, const std::string &title, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getTitle, &setTitle, title, nar, parent) {}
	};
	class SetAuthorCommand : public ModifyCommand<Narrative2, const std::string&> {
	public:
		SetAuthorCommand(Narrative2 *nar, const std::string &author, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getAuthor, &setAuthor, author, nar, parent) {}
	};
	class SetDescriptionCommand : public ModifyCommand<Narrative2, const std::string&> {
	public:
		SetDescriptionCommand(Narrative2 *nar, const std::string &desc, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getDescription, &setDescription, desc, nar, parent) {}
	};

	class NewSlideCommand : public Group::NewNodeCommand<NarrativeSlide> {
	public:
		NewSlideCommand(Narrative2 *group, int index, QUndoCommand *parent = nullptr)
			: Group::NewNodeCommand<NarrativeSlide>(group, index, parent) {}
	};
	class DeleteSlideCommand : public Group::DeleteNodeCommand<NarrativeSlide> {
	public:
		DeleteSlideCommand(Narrative2 *group, int index, QUndoCommand *parent = nullptr)
			: Group::DeleteNodeCommand<NarrativeSlide>(group, index, parent) {}
	};


private:
	std::string m_title;
	std::string m_description;
	std::string m_author;
	bool m_locked;
};

#endif /* NARRATIVE2_H */
