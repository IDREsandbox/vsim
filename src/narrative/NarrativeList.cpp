#include "narrative/NarrativeList.h"
#include "narrative/Narrative.h"

#include <QObject>
#include <QDebug>
#include <QAction>

NarrativeList::NarrativeList(QObject* parent, MainWindow* window)
	: QObject(parent), m_window(window), m_focus(-1), m_model(nullptr)
{
	m_list_gui = window->ui.narratives;

	// new
	connect(m_list_gui->m_action_new, &QAction::triggered, this, &NarrativeList::newNarrative);
	connect(m_window->ui.plus, &QPushButton::clicked, this, &NarrativeList::newNarrative);

	// delete
	connect(m_list_gui->m_action_delete, &QAction::triggered, this, &NarrativeList::deleteSelection);
	connect(m_window->ui.minus, &QPushButton::clicked, this, &NarrativeList::deleteSelection);

	// edit
	connect(m_list_gui->m_action_edit, &QAction::triggered, this, &NarrativeList::editNarrativeInfo);
	connect(m_list_gui, &HorizontalScrollBox::sDoubleClick, this, &NarrativeList::editNarrativeInfo);
	connect(m_window->ui.info, &QPushButton::clicked, this, &NarrativeList::editNarrativeInfo);
}

NarrativeList::~NarrativeList()
{
}

void NarrativeList::newNarrative()
{
	// open up the dialog
	NarrativeInfoDialog *dlg = m_window->m_narrative_info_dialog;
	dlg->clear();
	int result = dlg->exec();
	if (result == QDialog::Rejected) {
		return;
	}
	NarrativeInfo info = dlg->getInfo();

	// add item to osg and to here
	if (m_model == nullptr) {
		qDebug() << "failed to create new narrative - model is not initialized";
		return;
	}
	Narrative *narrative = new Narrative();
	narrative->setName(info.m_title);
	narrative->setAuthor(info.m_contact);
	narrative->setDescription(info.m_description);
	m_narrative_group->addChild(narrative);

	// add to gui
	m_list_gui->addItem(new NarrativeScrollItem(*narrative));
}

void NarrativeList::editNarrativeInfo()
{
	int active_item = m_list_gui->getLastSelected();
	qDebug() << "narrative list - begin edit on" << active_item;
	if (active_item < 0) {
		qWarning() << "narrative list - can't edit with no selection";
		return;
	}

	NarrativeScrollItem *item = dynamic_cast<NarrativeScrollItem*>(m_list_gui->getItem(active_item));
	Narrative *narrative = dynamic_cast<Narrative*>(m_narrative_group->getChild(active_item));

	NarrativeInfoDialog *dlg = m_window->m_narrative_info_dialog;
	dlg->setInfo(*narrative);
	int result = dlg->exec();
	if (result == QDialog::Rejected) {
		qDebug() << "narrative list - cancelled edit on" << active_item;
		return;
	}
	
	// get the new info
	NarrativeInfo info = dlg->getInfo();
	narrative->setName(info.m_title);
	narrative->setDescription(info.m_description);
	narrative->setAuthor(info.m_contact);

	item->setInfo(*narrative);
}

void NarrativeList::deleteSelection()
{
	std::set<int> selection = m_list_gui->getSelection();
	std::vector<Narrative*> deletionList;

	// get pointers to nodes to delete
	for (auto i : selection) {
		Narrative *nar = dynamic_cast<Narrative*>(m_narrative_group->getChild(i));
		if (nar == NULL) {
			qWarning() << "detected a non-narrative in the osg narrative group";
		}
		deletionList.push_back(nar);
	}
	// delete them
	for (auto delete_me : deletionList) {
		m_narrative_group->removeChild(delete_me);
	}

	m_list_gui->deleteSelection();
}

void NarrativeList::load(osg::Group * model)
{
	m_list_gui->clear();
	m_focus = -1;
	m_narrative_group = nullptr;
	m_model = model;

	// new: load narratives in a NarrativeList group
	// search for a narrative list node, if not found then create one
	unsigned int NumChildren = model->getNumChildren();
	for (unsigned int i = 0; i < NumChildren; i++)
	{
		osg::Group* group = model->getChild(i)->asGroup();
		if (group)
		{
			std::string name = group->getName();
			if (name == "NarrativeList") {
				m_narrative_group = group->asGroup();
				qDebug() << "found NarrativeList in file";
				break;
			}
		}
	}
	// if no NarrativeList was found then create one
	if (m_narrative_group == nullptr) {
		m_narrative_group = new osg::Group;
		m_narrative_group->setName("NarrativeList");
		model->addChild(m_narrative_group);
		qDebug() << "didnt find NarrativeList in file - creating a new one";
	}

	// load narratives into the gui
	for (unsigned int i = 0; i < m_narrative_group->getNumChildren(); i++)
	{
		osg::Node* c = m_narrative_group->getChild(i);
		Narrative* nar = dynamic_cast<Narrative*>(c);
		if (nar)
		{
			// add item to gui
			m_list_gui->addItem(new NarrativeScrollItem(*nar));
		}
		qDebug() << "loading narrative" << QString::fromStdString(nar->getName());
	}

	// old code: load narratives stored directly, for backward compatibility
	for (unsigned int i = 0; i < m_model->getNumChildren(); i++)
	{
		osg::Node* c = m_model->getChild(i);
		Narrative* nar = dynamic_cast<Narrative*>(c);
		if (nar)
		{
			qDebug() << "found old narrative" << nar->getName().c_str();
			// remove from old
			m_model->removeChild(nar);
			// add item to osg data structure
			m_narrative_group->addChild(nar);
			// add item to gui
			m_list_gui->addItem(new NarrativeScrollItem(*nar));
		}
	}
}
