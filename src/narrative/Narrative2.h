#ifndef NARRATIVE2_H
#define NARRATIVE2_H

#include <string>
#include <osg/Node>
#include <QObject>
#include "narrative/NarrativeSlide.h"
#include "deprecated/narrative/Narrative.h"
#include "Group.h"
#include "LabelStyle.h"

class Narrative2 : public Group {
	Q_OBJECT

public:
	Narrative2();
	Narrative2(const Narrative2& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	Narrative2(const Narrative *old); // converts old narrative to a new one
	virtual ~Narrative2();

	META_Node(, Narrative2)

	const std::string& getTitle() const;
	void setTitle(const std::string& name);
	const std::string& getAuthor() const;
	void setAuthor(const std::string& author);
	const std::string& getDescription() const;
	void setDescription(const std::string& description);

	bool getLock() const{return m_locked;}
	void setLock(bool lock){ m_locked = lock;}

	LabelStyle* getH1() { return h1; }
	LabelStyle* getH2() { return h2; }
	LabelStyle* getBod() { return bod; }
	LabelStyle* getLab() { return lab; }
	LabelStyle* getImg() { return img; }

	const LabelStyle* getH1() const { return h1; }
	void setH1(LabelStyle* H1) { h1 = H1; }
	const LabelStyle* getH2() const { return h2; }
	void setH2(LabelStyle* H2) { h2 = H2; }
	const LabelStyle* getBod() const { return bod; }
	void setBod(LabelStyle* Bod) { bod = Bod; }
	const LabelStyle* getLab() const { return lab; }
	void setLab(LabelStyle* Lab) { lab = Lab; }
	const LabelStyle* getImg() const { return img; }
	void setImg(LabelStyle* Img) { img = Img; }

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

private:
	std::string m_title;
	std::string m_description;
	std::string m_author;
	bool m_locked;

	//osg::ref_ptr<LabelStyleGroup> m_styles;
	osg::ref_ptr<LabelStyle> h1;
	osg::ref_ptr<LabelStyle> h2;
	osg::ref_ptr<LabelStyle> bod;
	osg::ref_ptr<LabelStyle> lab;
	osg::ref_ptr<LabelStyle> img;
};

#endif /* NARRATIVE2_H */
