#ifndef ECATEGORY_H
#define ECATEGORY_H

#include <string>
#include <osg/Node>
#include <QObject>
#include "Core/Command.h"
#include <QColor>

class EResource;
class ECategory : public QObject {
	Q_OBJECT
public:
	ECategory(QObject *parent = nullptr);

	void operator=(const ECategory &other);

	std::string getCategoryName() const;
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
	using SetCategoryNameCommand = ModifyCommand2<ECategory, const std::string&,
		&ECategory::getCategoryName, &ECategory::setCategoryName>;
	using SetColorCommand = ModifyCommand2<ECategory, QColor,
		&ECategory::getColor, &ECategory::setColor>;

private:
	std::string m_cat_name;
	QColor m_color;

	std::set<EResource*> m_resources;
};
#endif // ECATEGORY_HPP