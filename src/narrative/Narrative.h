#ifndef NARRATIVE_H
#define NARRATIVE_H

#include <string>
#include <osg/Node>
#include <QObject>
#include <memory>
#include "Core/GroupTemplate.h"
#include "Core/Command.h"

class LabelStyleGroup;
class NarrativeOld;
class NarrativeSlide;
class LockTable;

class Narrative : public TGroup<NarrativeSlide> {
	Q_OBJECT

public:
	Narrative(QObject *parent = nullptr);
	Narrative &operator= (const Narrative &other);

	void loadOld(const NarrativeOld *old);

	std::string getTitle() const;
	void setTitle(const std::string& name);
	std::string getAuthor() const;
	void setAuthor(const std::string& author);
	std::string getDescription() const;
	void setDescription(const std::string& description);

	LockTable *lockTable();
	const LockTable *lockTableConst() const;
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

	LockTable *m_lock_table;

	std::unique_ptr<LabelStyleGroup> m_styles;
};

#endif /* NARRATIVE_H */
