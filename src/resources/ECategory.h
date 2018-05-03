#ifndef ECATEGORY_H
#define ECATEGORY_H

#include <string>
#include <osg/Node>
#include <QObject>
#include "Command.h"
#include <QColor>

class EResource;
class ECategory : public QObject {
	Q_OBJECT
public:
	ECategory(QObject *parent = nullptr);

	const std::string& getCategoryName() const;
	void setCategoryName(const std::string& name);
	QColor getColor() const;
	void setColor(QColor color); // rgb, no alpha

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
	QColor m_color;

	std::set<EResource*> m_resources;
};
#endif // ECATEGORY_HPP