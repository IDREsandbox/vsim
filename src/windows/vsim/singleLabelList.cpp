#include "singleLabelList.h"

#include <QObject>
#include <QDebug>
#include <QAction>

singleLabelList::singleLabelList(QObject* parent, MainWindow* window)
	: QObject(parent), m_window(window), m_focus(-1), m_model(nullptr)
{
	//m_list_gui = window->ui.narratives;

	// new
	connect(m_list_gui->m_action_new, &QAction::triggered, this, &singleLabelList::newLabel);
	connect(m_window->ui.plus, &QPushButton::clicked, this, &singleLabelList::newLabel);

	// delete
	connect(m_list_gui->m_action_delete, &QAction::triggered, this, &singleLabelList::deleteLabel);
	connect(m_window->ui.minus, &QPushButton::clicked, this, &singleLabelList::deleteLabel);
}

singleLabelList::~singleLabelList()
{	}

void singleLabelList::addLabel(dragLabel* label) {
	m_labels.push_back(label);

	// add item to gui
	dragLabel *new_item = new NarrativeScrollItem(nullptr);
	new_item->;
	m_list_gui->addItem(new_item);
}

void singleLabelList::newLabel()
{
	// open up the dialog
	auto dlg = m_window->m_narrative_info_dialog;
	int result = dlg->exec();
	if (result == QDialog::Rejected) {
		return;
	}
	NarrativeInfo info = dlg->getInfo();

	// add item to osg
	if (m_model == nullptr) {
		qDebug() << "failed to create new narrative - model is not initialized";
		return;
	}
	Narrative *narrative = new Narrative();
	narrative->setName(info.m_title);
	narrative->setAuthor(info.m_contact);
	narrative->setDescription(info.m_description);
	m_narratives.push_back(narrative);
	m_model->asGroup()->addChild(narrative);

	addNarrative(narrative);

	//// add item to gui
	//NarrativeScrollItem *new_item = new NarrativeScrollItem(nullptr);
	//new_item->setInfo(dlg->getInfo());
	//m_list_gui->addItem(new_item);	
}

void singleLabelList::deleteLabel()
{
	std::set<int> selection = m_list_gui->getSelection();

	m_list_gui->deleteSelection();
}

unsigned int singleLabelList::getNumLabels()
{
	return m_narratives.size();
}

Narrative * singleLabelList::getLabelReference(unsigned int i)
{
	assert(i < m_narratives.size());
	return m_narratives[i];
}

void singleLabelList::clear()
{
	m_narratives.clear();
	m_list_gui->clear();
	m_focus = -1;
	m_model = nullptr;
}

void singleLabelList::loadFromNode(osg::Node * model)
{
	clear();
	m_model = model;

	unsigned int NumChildren = model->asGroup()->getNumChildren();
	for (unsigned int i = 0; i < NumChildren; i++)
	{
		osg::Node* c = model->asGroup()->getChild(i);
		Narrative* nar = dynamic_cast<Narrative*>(c);
		if (nar)
		{
			addLabel(nar);
			qDebug() << "found narrative" << nar->getName().c_str();
		}
	}
}

