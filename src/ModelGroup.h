#ifndef MODELGROUP_H
#define MODELGROUP_H

#include <QObject>
#include <osg/Group>
#include "ModelDataTable.h"

class ModelGroup : public QObject, public osg::Group {
	Q_OBJECT
public:
	ModelGroup();
	ModelGroup(const ModelGroup& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	META_Node(, ModelGroup);

	// merge another model group
	void merge(ModelGroup *other);

	ModelDataTable *dataTable() const;
	const ModelDataTable *getDataTable() const;
	void setDataTable(ModelDataTable *table);

	// Set the year to view, hides/shows models accordingly
	// 0 shows all models
	int getYear() const;
	void setYear(int year);

	std::set<int> getKeyYears();

signals:
	void sYearChange(int year);

private:
	osg::ref_ptr<ModelDataTable> m_data_table;
	int m_year;
};


class TimeGetVisitor : public osg::NodeVisitor {
public:
	TimeGetVisitor(std::set<int> *out);
	virtual void apply(osg::Group &node) override;
private:
	std::set<int> *m_out;
};

class TimeMaskVisitor : public osg::NodeVisitor {
public:
	TimeMaskVisitor(int year);
	virtual void apply(osg::Group &node) override;
private:
	int m_year;
};

#endif