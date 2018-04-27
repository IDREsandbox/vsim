#ifndef MODELGROUP_H
#define MODELGROUP_H

#include <QObject>
#include <osg/Group>
#include <QUndoStack>
#include <QDebug>
#include <set>
#include "GroupTemplate.h"

class Model;
class ModelGroup : public TGroup<Model> {
	Q_OBJECT
public:
	ModelGroup();

	// merge another model group
	// void merge(const ModelGroup *other);

	// node edit
	// void setNodeYear(osg::Node *node, int year, bool begin);
	void addNode(osg::Node *node, const std::string &path);

	// applies node visitor to all models
	void accept(osg::NodeVisitor &visitor);

	// Set the year to view, hides/shows models accordingly
	// 0 shows all models
	void setYear(int year);
	void enableTime(bool enable);

	std::set<int> getKeyYears();

	osg::Group *sceneRoot() const;
	void debugScene() const;

signals:
	void sKeysChanged();

private:
	int m_year;
	bool m_time_enabled;

	osg::ref_ptr<osg::Group> m_root;
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
	TimeInitVisitor(Model *group);
	virtual void apply(osg::Group &node) override;
	static void touch(Model *model, osg::Node *node);
private:
	Model *m_model;
	int m_year;
};

// Returns a set of key transition times
class TimeGetVisitor : public osg::NodeVisitor {
public:
	TimeGetVisitor();
	virtual void apply(osg::Group &node) override;
	std::set<int> results() const;
private:
	std::set<int> m_begins;
	std::set<int> m_ends;
	std::set<int> bar;
};

// Hides nodes based on year
class TimeMaskVisitor : public osg::NodeVisitor {
public:
	TimeMaskVisitor(int year);
	virtual void apply(osg::Group &node) override;
private:
	int m_year;
};

class DebugVisitor : public osg::NodeVisitor {
public:
	DebugVisitor();
	virtual void apply(osg::Group &group) override;
	virtual void apply(osg::Node &node) override;

private:
	int m_tabs;
};

#endif