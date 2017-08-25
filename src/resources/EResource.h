#ifndef ERESOURCE_H
#define ERESOURCE_H

#include <string>
#include <osg/Node>
#include <QObject>
//#include "deprecated/resources/EResourcesNode.h"
#include "Group.h"
#include "Command.h"

class EResource : public Group {
	Q_OBJECT

public:
	EResource();
	EResource(const EResource& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	//EResource(const EResourcesNode *old); // converts old resource to a new one
	virtual ~EResource();

	META_Node(, EResource)

	const std::string& getResourceName() const;
	void setResourceName(const std::string& name);
	const std::string& getResourceType() const;
	void setResourceType(const std::string& type);
	const std::string& getResourcePath() const;
	void setResourcePath(const std::string& path);
	const std::string& getResourceDescription() const;
	void setResourceDescription(const std::string& description);
	const std::string& getAuthor() const;
	void setAuthor(const std::string& authors);
	int getGlobal() const;
	void setGlobal(int gorl);
	int getCopyRight() const;
	void setCopyRight(int cr);
	int getMinYear() const;
	void setMinYear(int my);
	int getMaxYear() const;
	void setMaxYear(int my);
	int getReposition() const;
	void setReposition(int reposition);
	int getAutoLaunch() const;
	void setAutoLaunch(int launch);
	float getLocalRange() const;
	void setLocalRange(float lrange);
	int getErType() const;
	void setErType(int ertype);

	const osg::Matrixd& getViewMatrix() const;
	osg::Matrixd& getViewMatrix();
	void setViewMatrix(const osg::Matrixd& matrix);

	const std::string& getCategoryName() const;
	void setCategoryName(const std::string& name);

	int getRed() const;
	void setRed(int red);
	int getGreen()const;
	void setGreen(int green);
	int getBlue()const;
	void setBlue(int blue);

	void setIndex(int idx);

signals:
	void sResourceNameChanged(const std::string&, int);
	void sResourceAuthorChanged(const std::string&, int);
	void sResourceDescriptionChanged(const std::string&, int);
	void sResourceTypeChanged(const std::string&, int);
	void sResourcePathChanged(const std::string&, int);
	void sGlobalChanged(int, int);
	void sCopyRightChanged(int, int);
	void sMinYearChanged(int, int);
	void sMaxYearChanged(int, int);
	void sRepositionChanged(int, int);
	void sAutoLaunchChanged(int, int);
	void sLocalRangeChanged(float, int);
	void sErTypeChanged(int, int);
	void sViewMatrixChanged(const osg::Matrixd&, int);

	void sCategoryNameChanged(const std::string&, int);
	void sRedChanged(int, int);
	void sBlueChanged(int, int);
	void sGreenChanged(int, int);

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
	class SetResourceTypeCommand : public ModifyCommand<EResource, const std::string&> {
	public:
		SetResourceTypeCommand(EResource *res, const std::string &type, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getResourceType, &setResourceType, type, res, parent) {}
	};
	class SetResourcePathCommand : public ModifyCommand<EResource, const std::string&> {
	public:
		SetResourcePathCommand(EResource *res, const std::string &path, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getResourcePath, &setResourcePath, path, res, parent) {}
	};
	class SetGlobalCommand : public ModifyCommand<EResource, int> {
	public:
		SetGlobalCommand(EResource *res, int glob, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getGlobal, &setGlobal, glob, res, parent) {}
	};
	class SetCopyRightCommand : public ModifyCommand<EResource, int> {
	public:
		SetCopyRightCommand(EResource *res, int cr, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getCopyRight, &setCopyRight, cr, res, parent) {}
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
	class SetRepositionCommand : public ModifyCommand<EResource, int> {
	public:
		SetRepositionCommand(EResource *res, int re, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getReposition, &setReposition, re, res, parent) {}
	};
	class SetAutoLaunchCommand : public ModifyCommand<EResource, int> {
	public:
		SetAutoLaunchCommand(EResource *res, int al, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getAutoLaunch, &setAutoLaunch, al, res, parent) {}
	};
	class SetLocalRangeCommand : public ModifyCommand<EResource, float> {
	public:
		SetLocalRangeCommand(EResource *res, float lr, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getLocalRange, &setLocalRange, lr, res, parent) {}
	};
	class SetErTypeCommand : public ModifyCommand<EResource, int> {
	public:
		SetErTypeCommand(EResource *res, int er, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getErType, &setErType, er, res, parent) {}
	};
	class SetViewMatrixCommand : public ModifyCommand<EResource, const osg::Matrixd&> {
	public:
		SetViewMatrixCommand(EResource *res, const osg::Matrixd& mat, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getViewMatrix, &setViewMatrix, mat, res, parent) {}
	};

	// category commands
	class SetCategoryNameCommand : public ModifyCommand<EResource, const std::string&> {
	public:
		SetCategoryNameCommand(EResource *res, const std::string &name, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getCategoryName, &setCategoryName, name, res, parent) {}
	};
	class SetRedCommand : public ModifyCommand<EResource, int> {
	public:
		SetRedCommand(EResource *res, int red, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getRed, &setRed, red, res, parent) {}
	};
	class SetGreenCommand : public ModifyCommand<EResource, int> {
	public:
		SetGreenCommand(EResource *res, int green, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getGreen, &setGreen, green, res, parent) {}
	};
	class SetBlueCommand : public ModifyCommand<EResource, int> {
	public:
		SetBlueCommand(EResource *res, int blue, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getBlue, &setBlue, blue, res, parent) {}
	};


private:
	std::string m_name;
	std::string m_filetype;
	std::string m_filepath;
	std::string m_description;
	std::string m_authors;
	int m_global;
	int m_reposition;
	int m_launch; // 0 off, 1 on, 2 text
	int m_copyright;
	int m_min_year; // default -99999
	int m_max_year; // default 99999
	float m_local_range; // global should be false to make this valid
	int m_ertype; // 0 file, 1 link, 2 annotation
	bool m_filter;

	osg::Vec3f m_camera_position;
	osg::Matrixd m_view_matrix;

	bool m_inview;

	// categories 
	std::string m_cat_name;
	int m_blue;
	int m_red;
	int m_green;

	int m_index;
};

#endif /* ERESOURCE_H */
