#ifndef ERESOURCE_H
#define ERESOURCE_H

#include <string>
#include <osg/Node>
#include <osg/observer_ptr>
#include <QObject>

//#include "deprecated/resources/EResourcesNode.h"
#include "Group.h"
#include "Command.h"
#include "resources/ECategory.h"

class EResourcesNode;
class ECategoryGroup;
class EResource : public QObject, public osg::Node {
	Q_OBJECT

public:
	EResource();
	EResource(const EResource& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	EResource(const EResourcesNode *old, const std::map<std::string, ECategory*> &cats); // converts old resource to a new one
	virtual ~EResource();

	META_Node(, EResource)

	const std::string& getResourceName() const;
	void setResourceName(const std::string& name);
	const std::string& getResourcePath() const;
	void setResourcePath(const std::string& path);
	const std::string& getResourceDescription() const;
	void setResourceDescription(const std::string& description);
	const std::string& getAuthor() const;
	void setAuthor(const std::string& author);
	bool getGlobal() const;
	void setGlobal(bool global);

	enum Copyright {
		COPYRIGHTED,
		FAIRUSE,
		HELD,
		PUBLIC,
		UNKNOWN,
		PERMISSION,
		WEB,
		UNSPECIFIED
	};
	static constexpr const char *CopyrightStrings[] = { "Copyrighted Resource",
		"Fair Use",
		"Held by Creator",
		"Public Domain",
		"Unknown Source",
		"Used with Permission",
		"Web Resource",
		"Unspecified"
	};

	Copyright getCopyright() const;
	void setCopyright(Copyright cr);
	int getMinYear() const;
	void setMinYear(int my);
	int getMaxYear() const;
	void setMaxYear(int my);

	bool getReposition() const;
	void setReposition(bool reposition);
	bool getAutoLaunch() const;
	void setAutoLaunch(bool launch);
	float getLocalRange() const;
	void setLocalRange(float lrange);

	enum ERType {
		URL,
		ANNOTATION,
		FILE
	};
	ERType getERType() const;
	void setERType(ERType ertype);

	const osg::Matrixd& getCameraMatrix() const;
	osg::Matrixd& getCameraMatrix();
	void setCameraMatrix(const osg::Matrixd& matrix);

	const ECategory *getCategory() const;
	void setCategory(ECategory *category);

signals:
	void sResourceNameChanged(const std::string&);
	void sResourceAuthorChanged(const std::string&);
	void sResourceDescriptionChanged(const std::string&);
	//void sResourceTypeChanged(const std::string&);
	void sResourcePathChanged(const std::string&);
	void sGlobalChanged(bool);
	void sCopyRightChanged(Copyright);
	void sMinYearChanged(int);
	void sMaxYearChanged(int);
	void sRepositionChanged(bool);
	void sAutoLaunchChanged(bool);
	void sLocalRangeChanged(float);
	void sErTypeChanged(ERType);
	void sViewMatrixChanged(const osg::Matrixd&);

	void sCategoryChanged(const ECategory *old_category, const ECategory *new_category);

public: // resource commands
	class SetResourceNameCommand : public ModifyCommand<EResource, const std::string&> {
	public:
		SetResourceNameCommand(EResource *res, const std::string &title, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getResourceName, &setResourceName, title, res, parent) {}
	};
	class SetResourceAuthorCommand : public ModifyCommand<EResource, const std::string&> {
	public:
		SetResourceAuthorCommand(EResource *re, const std::string &author, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getAuthor, &setAuthor, author, re, parent) {}
	};
	class SetResourceDescriptionCommand : public ModifyCommand<EResource, const std::string&> {
	public:
		SetResourceDescriptionCommand(EResource *res, const std::string &desc, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getResourceDescription, &setResourceDescription, desc, res, parent) {}
	};
	class SetResourcePathCommand : public ModifyCommand<EResource, const std::string&> {
	public:
		SetResourcePathCommand(EResource *res, const std::string &path, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getResourcePath, &setResourcePath, path, res, parent) {}
	};
	class SetGlobalCommand : public ModifyCommand<EResource, bool> {
	public:
		SetGlobalCommand(EResource *res, bool global, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getGlobal, &setGlobal, global, res, parent) {}
	};
	class SetCopyrightCommand : public ModifyCommand<EResource, Copyright> {
	public:
		SetCopyrightCommand(EResource *res, Copyright cr, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getCopyright, &setCopyright, cr, res, parent) {}
	};
	class SetMinYearCommand : public ModifyCommand<EResource, int> {
	public:
		SetMinYearCommand(EResource *res, int my, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getMinYear, &setMinYear, my, res, parent) {}
	};
	class SetMaxYearCommand : public ModifyCommand<EResource, int> {
	public:
		SetMaxYearCommand(EResource *res, int my, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getMaxYear, &setMaxYear, my, res, parent) {}
	};
	class SetRepositionCommand : public ModifyCommand<EResource, bool> {
	public:
		SetRepositionCommand(EResource *res, bool re, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getReposition, &setReposition, re, res, parent) {}
	};
	class SetAutoLaunchCommand : public ModifyCommand<EResource, bool> {
	public:
		SetAutoLaunchCommand(EResource *res, bool al, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getAutoLaunch, &setAutoLaunch, al, res, parent) {}
	};
	class SetLocalRangeCommand : public ModifyCommand<EResource, float> {
	public:
		SetLocalRangeCommand(EResource *res, float lr, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getLocalRange, &setLocalRange, lr, res, parent) {}
	};
	class SetErTypeCommand : public ModifyCommand<EResource, ERType> {
	public:
		SetErTypeCommand(EResource *res, ERType er, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getERType, &setERType, er, res, parent) {}
	};
	class SetCameraMatrixCommand : public ModifyCommand<EResource, const osg::Matrixd&> {
	public:
		SetCameraMatrixCommand(EResource *res, const osg::Matrixd& mat, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getCameraMatrix, &setCameraMatrix, mat, res, parent) {}
	};

	// category commands
	//class SetCategoryCommand : public ModifyCommand<EResource, ECategory *> {
	//public:
	//	SetCategoryNameCommand(EResource *res, ECategory *category, QUndoCommand *parent = nullptr)
	//		: ModifyCommand(&getCategoryName, &setCategoryName, category, res, parent) {}
	//};
	
private:
	std::string m_name;
	//std::string m_filetype;
	std::string m_filepath;
	std::string m_description;
	std::string m_authors;
	bool m_global;
	bool m_reposition;
	int m_launch; // 0 off, 1 on, 2 text
	Copyright m_copyright;
	int m_min_year; // default 0
	int m_max_year; // default 0
	float m_local_range; // global should be false to make this valid
	ERType m_ertype; // 0 file, 1 link, 2 annotation

	osg::Vec3f m_camera_position;
	osg::Matrixd m_camera_matrix;

	//bool m_inview; // what is this?

	osg::observer_ptr<ECategory> m_category;

	int m_index;
};

#endif /* ERESOURCE_H */
