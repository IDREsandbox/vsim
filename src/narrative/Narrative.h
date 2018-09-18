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
	~Narrative();

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
	using SetTitleCommand = ModifyCommand2<Narrative, const std::string&,
		&Narrative::getTitle, &Narrative::setTitle>;
	using SetAuthorCommand = ModifyCommand2<Narrative, const std::string&,
		&Narrative::getAuthor, &Narrative::setAuthor>;
	using SetDescriptionCommand = ModifyCommand2<Narrative, const std::string&,
		&Narrative::getDescription, &Narrative::setDescription>;

private:
	std::string m_title;
	std::string m_description;
	std::string m_author;

	LockTable *m_lock_table;

	std::unique_ptr<LabelStyleGroup> m_styles;
};

#endif /* NARRATIVE_H */
