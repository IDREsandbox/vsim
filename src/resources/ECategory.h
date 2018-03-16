#ifndef ECATEGORY_H
#define ECATEGORY_H

#include <string>
#include <osg/Node>
#include <QObject>
#include "Group.h"
#include "Command.h"
#include <QColor>

class EResource;
class ECategory : public Group {
	Q_OBJECT

public:
	ECategory();
	ECategory(const ECategory& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);

	virtual ~ECategory();

	META_Node(, ECategory)

	const std::string& getCategoryName() const;
	void setCategoryName(const std::string& name);
	QColor getColor() const;
	void setColor(QColor color);
	// these are for the serializer
	int getRed() const;
	void setRed(int red);
	int getGreen()const;
	void setGreen(int green);
	int getBlue()const;
	void setBlue(int blue);

	// internal, use EResource::setCategory()
	void addResource(EResource *res);
	void removeResource(EResource *res);
	const std::set<EResource*> &resources() const;

signals:
	void sCNameChanged(std::string old_name, std::string new_name);
	void sColorChanged(QColor color);

// COMMANDS
public:
	class SetCategoryNameCommand : public ModifyCommand<ECategory, const std::string&> {
	public:
		SetCategoryNameCommand(ECategory *cat, const std::string &name, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getCategoryName, &setCategoryName, name, cat, parent) {}
	};
	class SetColorCommand : public ModifyCommand<ECategory, QColor> {
	public:
		SetColorCommand(ECategory *cat, QColor color, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getColor, &setColor, color, cat, parent) {}
	};

private:
	std::string m_cat_name;
	int m_blue;
	int m_red;
	int m_green;

	std::set<EResource*> m_resources;
};
#endif // ECATEGORY_HPP