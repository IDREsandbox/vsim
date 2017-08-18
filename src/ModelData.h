#ifndef MODELDATA_H
#define MODELDATA_H

#include <QObject>
#include <osg/Group>
#include <Command.h>

// Meta information which can be attached to an arbitrary osg::Group
// in the scene graph. ModelGroup looks for these guys when performing
// time/switching stuff.
// why is this a group? i have no clue
class ModelData : public QObject, public osg::Group {
	Q_OBJECT
public:
	ModelData() {};
	ModelData(const ModelData& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {};
	META_Node(, ModelData);

	int getYearBegin() const;
	void setYearBegin(int year);
	int getYearEnd() const;
	void setYearEnd(int year);

signals:
	void sYearBeginChange(int);
	void sYearEndChange(int);

public:
	// Add metadata to an existing osg::Group
	static ModelData *addModelNode(osg::Group *g);
	static void removeModelNode(osg::Group *g);
	static ModelData *getModelNode(osg::Node *g);
	static ModelData *getModelNode(osg::Group *g);

public: // COMMANDS
	// Warning there is no 0 checking in here, no exceptions or anything, so just don't pass in0 
	class SetYearBeginCommand : public ModifyCommand<ModelData, int> {
	public:
		SetYearBeginCommand(ModelData *model, int year, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getYearBegin, &setYearBegin, year, model, parent) {}
	};
	class SetYearEndCommand : public ModifyCommand<ModelData, int> {
	public:
		SetYearEndCommand(ModelData *model, int year, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getYearEnd, &setYearEnd, year, model, parent) {}
	};

private:
	int m_year_begin;
	int m_year_end;
	
};

#endif