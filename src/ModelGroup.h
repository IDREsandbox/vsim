#ifndef MODELGROUP_H
#define MODELGROUP_H

#include <QObject>
#include <osg/Group>
#include <QUndoStack>
#include <QDebug>

class ModelGroup : public QObject, public osg::Group {
	Q_OBJECT
public:
	ModelGroup();
	ModelGroup(const ModelGroup& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	META_Node(, ModelGroup);

	// merge another model group
	void merge(ModelGroup *other);

	// Set the year to view, hides/shows models accordingly
	// 0 shows all models
	int getYear() const;
	void setYear(int year);

	std::set<int> getKeyYears();

	virtual bool addChild(osg::Node *child) override;

	static bool nodeTimeInName(const std::string &name, int * begin, int * end);

signals:
	void sYearChange(int year);

	void sUserValueChanged(osg::Node *node, std::string name);

private:
	int m_year;
};

//
//class SetUserValueCommand : public QUndoCommand {
//public:
//	SetUserValueCommand(ModelGroup *group, osg::Node *node, const std::string &name, int value, QUndoCommand *parent = nullptr)
//		: QUndoCommand(parent),
//		m_group(group),
//		m_node(node),
//		m_new_value(value)
//	{
//		int old_value = 0;
//		bool found = node->getUserValue(name, old_value);
//		if (!found) {
//			// if old value wasn't found then create it in an undo-able way
//			qDebug() << "old value not found";
//			m_old_value = 0;
//			return;
//		}
//		m_old_value = old_value;
//	}
//	void undo() {
//
//		m_node->setUserValue(m_name, m_old_value);
//		m_group->sUserValueChanged(m_node, m_name);
//	}
//	void redo() {
//
//		m_node->setUserValue(m_name, m_new_value);
//		m_group->sUserValueChanged(m_node, m_name);
//	}
//
//private:
//	ModelGroup *m_group;
//	osg::Node *m_node;
//	std::string m_name;
//	int m_old_value;
//	int m_new_value;
//};

// Goes through nodes and initializes yearBegin and yearEnd
// based on names
class TimeInitVisitor : public osg::NodeVisitor {
public:
	TimeInitVisitor();
	virtual void apply(osg::Group &node) override;
private:
	int m_year;
};

// Returns a set of key transition times
class TimeGetVisitor : public osg::NodeVisitor {
public:
	TimeGetVisitor(std::set<int> *out);
	virtual void apply(osg::Group &node) override;
private:
	std::set<int> *m_out;
};

// Hides nodes based on year
class TimeMaskVisitor : public osg::NodeVisitor {
public:
	TimeMaskVisitor(int year);
	virtual void apply(osg::Group &node) override;
private:
	int m_year;
};

#endif