#ifndef MODELGROUP_H
#define MODELGROUP_H

#include <QObject>
#include <osg/Group>
#include <QUndoStack>
#include <QDebug>
#include <set>
#include "Core/GroupTemplate.h"

class Model;
class OSGNodeWrapper;
class ModelGroup : public TGroup<Model> {
	Q_OBJECT
public:
	ModelGroup(QObject *parent = nullptr);

	// merge another model group
	// void merge(const ModelGroup *other);

	// node edit
	// void setNodeYear(osg::Node *node, int year, bool begin);
	void addNode(osg::Node *node, const std::string &path);

	// applies node visitor to all models
	void accept(osg::NodeVisitor &visitor);

	// Set the year to view, hides/shows models accordingly
	// 0 shows all models
	//void setYear(int year);
	//void enableTime(bool enable);

	std::set<int> getKeyYears();

	OSGNodeWrapper *rootWrapper() const;
	osg::Group *sceneRoot() const;
	void debugScene() const;

signals:
	void sKeysChanged();

	// model can be null if unknown
	void sNodeYearChanged(osg::Node *node, int year, bool begin, Model *model);

private:
	osg::ref_ptr<osg::Group> m_root;
	OSGNodeWrapper *m_root_wrapper;
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

#endif