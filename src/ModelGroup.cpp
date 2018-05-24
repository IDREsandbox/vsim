#include "ModelGroup.h"
#include <QDebug>
#include <regex>
#include <iostream>
#include <QDir>

#include "Model.h"
#include "OSGNodeWrapper.h"

ModelGroup::ModelGroup(QObject *parent)
	: TGroup<Model>(parent)
{
	m_root = new osg::Group;
	m_root_wrapper = new OSGNodeWrapper(this);
	m_root_wrapper->setRoot(m_root);
	connect(m_root_wrapper, &OSGNodeWrapper::sNodeYearChanged, this,
		&ModelGroup::sKeysChanged);
	connect(this, &GroupSignals::sInserted, this,
		[this](size_t index, size_t count) {

		for (size_t i = 0; i < count; i++) {
			Model *model = child(index + i);
			osg::Node *node = model->node();

			TimeInitVisitor v(m_root_wrapper); // check for T: start end
			node->accept(v);

			// TODO: add m_root_wrapper->addNodeChild() to handle signaling
			m_root_wrapper->sAboutToReset();
			m_root->addChild(node);
			m_root_wrapper->sReset();

			//connect(model, &Model::sNodeYearChanged, this, [this]() {
			//	yearChangePoke();
			//});
		}
	});
	connect(this, &GroupSignals::sAboutToRemove, this,
		[this](size_t index, size_t count) {
		for (size_t i = 0; i < count; i++) {
			disconnect(child(index + i), 0, this, 0);
			m_root->removeChild(child(i)->node());
		}
	});
}

void ModelGroup::copyReference(const ModelGroup & other)
{
	clear();

	m_root = other.m_root;

	m_root_wrapper->setRoot(m_root);

	for (auto model : other) {
		auto new_model = std::make_shared<Model>();
		new_model->copyReference(*model);
		append(model);
	}
}

void ModelGroup::addNode(osg::Node * node, const std::string & path)
{
	auto model = std::make_shared<Model>();
	model->setPath(path);
	model->setName(QFileInfo(path.c_str()).fileName().toStdString());
	model->setNode(node);
	append(model);
}

void ModelGroup::accept(osg::NodeVisitor & visitor)
{
	// apply year to all models
	int i = 0;
	for (auto &model : *this) {
		
		if (!model) continue;
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

void ModelGroup::debugScene() const
{
	DebugVisitor v;
	m_root->accept(v);
}
