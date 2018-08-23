#include "ModelGroup.h"
#include "ModelGroup.h"
#include <QDebug>
#include <regex>
#include <iostream>
#include <QDir>

#include "Model.h"
#include "OSGNodeWrapper.h"

ModelGroup::ModelGroup(QObject *parent)
	: QObject(parent)
{
	m_root = new osg::Group;
	m_root_wrapper = new OSGNodeWrapper(this);
	m_root_wrapper->setRoot(m_root);
	m_group = new TGroup<Model>(this);
	connect(m_root_wrapper, &OSGNodeWrapper::sNodeYearChanged, this,
		&ModelGroup::sKeysChanged);
	//connect(this, &GroupSignals::sInserted, this,
	//	[this](size_t index, size_t count) {

	//	for (size_t i = 0; i < count; i++) {
	//		Model *model = child(index + i);
	//		osg::Node *node = model->node();

	//		TimeInitVisitor v(m_root_wrapper); // check for T: start end
	//		node->accept(v);

	//		// TODO: add m_root_wrapper->addNodeChild() to handle signaling
	//		m_root_wrapper->sAboutToReset();
	//		m_root->addChild(node);
	//		m_root_wrapper->sReset();

	//		//connect(model, &Model::sNodeYearChanged, this, [this]() {
	//		//	yearChangePoke();
	//		//});
	//	}
	//});
	//connect(this, &GroupSignals::sAboutToRemove, this,
	//	[this](size_t index, size_t count) {
	//	for (size_t i = 0; i < count; i++) {
	//		disconnect(child(index + i), 0, this, 0);
	//		m_root->removeChild(child(i)->node());
	//	}
	//});
}

void ModelGroup::copyReference(const ModelGroup & other)
{
	clear();

	for (auto model : *other.m_group) {
		auto new_model = std::make_shared<Model>();
		new_model->copyReference(*model);
		addModel(new_model);
	}
}

void ModelGroup::addModel(std::shared_ptr<Model> m)
{
	m_group->append(m);

	m_root_wrapper->addChild(m_root, m->node(), m_root->getNumChildren());

	connect(m.get(), &Model::sNodeChanged, this, [this, m]() {
		size_t index = m_group->indexOf(m.get());

		//// remove child
		m_root_wrapper->removeChild(m_root, index);
		//// add new child
		m_root_wrapper->addChild(m_root, m->node(), index);

		//m_root_wrapper->notifyChanged(m->node());
	});
}

void ModelGroup::removeModel(Model * m)
{
	disconnect(m, 0, this, 0);

	int i = m_group->indexOf(m);
	if (i < 0 || i >= m_group->size()) return;
	m_group->remove(i, 1);

	// remove model's node too
	m_root_wrapper->removeChild(m_root, i);

}

void ModelGroup::clear()
{
	m_group->clear();
	m_root = new osg::Group;
	m_root_wrapper->setRoot(m_root);
}

TGroup<Model>* ModelGroup::group()
{
	return m_group;
}

const TGroup<Model>* ModelGroup::cgroup() const
{
	return m_group;
}

//void ModelGroup::addNode(osg::Node * node, const std::string & path)
//{
//	auto model = std::make_shared<Model>();
//	model->setPath(path);
//	model->setName(QFileInfo(path.c_str()).fileName().toStdString());
//	model->setNode(node);
//	append(model);
//}

void ModelGroup::accept(osg::NodeVisitor & visitor)
{
	// apply year to all models
	int i = 0;
	for (auto &model : *m_group) {
		if (!model) continue;
		if (!model->node()) continue;
		model->node()->accept(visitor);
	}
}

std::set<int> ModelGroup::getKeyYears()
{
	TimeGetVisitor v;
	accept(v);
	std::set<int> years = v.results();

	if (years.empty()) return years;

	return years;
}

OSGNodeWrapper * ModelGroup::rootWrapper() const
{
	return m_root_wrapper;
}

osg::Group * ModelGroup::sceneRoot() const
{
	return m_root;
}

int ModelGroup::embedAll()
{
	int count = 0;
	for (auto model : *m_group) {
		if (!model->embedded()) {
			model->embedModel(model->nodeRef());
			count++;
		}
	}
	return count;
}

void ModelGroup::debugScene() const
{
	DebugVisitor v;
	m_root->accept(v);
}
