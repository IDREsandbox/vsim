#ifndef NARRATIVE_H
#define NARRATIVE_H

#include <string>
#include <osg/Node>
#include <QObject>
#include "GroupTemplate.h"
#include "Command.h"

class LabelStyleGroup;
class NarrativeOld;
class NarrativeSlide;

class Narrative : public TGroup<NarrativeSlide> {
	Q_OBJECT

public:
	Narrative();
	Narrative(const NarrativeOld *old); // converts old narrative to a new one

	const std::string& getTitle() const;
	void setTitle(const std::string& name);
	const std::string& getAuthor() const;
	void setAuthor(const std::string& author);
	const std::string& getDescription() const;
	void setDescription(const std::string& description);

	bool getLock() const{return m_locked;}
	void setLock(bool lock){ m_locked = lock;}

	LabelStyleGroup *labelStyles() const;

signals:
	void sTitleChanged(const std::string&);
	void sAuthorChanged(const std::string&);
	void sDescriptionChanged(const std::string&);

	void sNewSlide(int);
	void sDeleteSlide(int);

public: // COMMANDS

	class SetTitleCommand : public ModifyCommand<Narrative, const std::string&> {
	public:
		SetTitleCommand(Narrative *nar, const std::string &title, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getTitle, &setTitle, title, nar, parent) {}
	};
	class SetAuthorCommand : public ModifyCommand<Narrative, const std::string&> {
	public:
		SetAuthorCommand(Narrative *nar, const std::string &author, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getAuthor, &setAuthor, author, nar, parent) {}
	};
	class SetDescriptionCommand : public ModifyCommand<Narrative, const std::string&> {
	public:
		SetDescriptionCommand(Narrative *nar, const std::string &desc, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getDescription, &setDescription, desc, nar, parent) {}
	};

private:
	std::string m_title;
	std::string m_description;
	std::string m_author;
	bool m_locked;

	std::unique_ptr<LabelStyleGroup> m_styles;
};

#endif /* NARRATIVE_H */
