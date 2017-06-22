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
}

NarrativeList::~NarrativeList()
{

}

void NarrativeList::addNarrative(Narrative* narrative)
{
	m_narratives.push_back(narrative);

	// add item to gui
	NarrativeScrollItem *new_item = new NarrativeScrollItem(nullptr);
	new_item->setInfo({
		narrative->getName(),
		narrative->getDescription(),
		narrative->getAuthor()
	});
	m_list_gui->addItem(new_item);
}
void NarrativeList::addBlankNarrative()
{
	m_list_gui->addBlankItem();
}
void NarrativeList::newNarrative()
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

void NarrativeList::deleteSelection()
{
	std::set<int> selection = m_list_gui->getSelection();

	m_list_gui->deleteSelection();
}

//void NarrativeList::deleteSelection(const std::set<int>& selection)
//{
//	NarrativeReferenceVector newList;
//	// TODO
//}
//
//void NarrativeList::removeNarrative(int index)
//{
//    assert(index >= 0 && index < (int)m_narrativeReferences.size());
//    NarrativeReferenceVector::iterator it = m_narrativeReferences.begin();
//    it += index;
//    it = m_narrativeReferences.erase(it);
//    int new_selected = index;
//    if (it == m_narrativeReferences.end())
//        new_selected = m_narrativeReferences.size() - 1;
//    setSelection(new_selected);
//}

unsigned int NarrativeList::getNumNarratives()
{
	return m_narratives.size();
}

Narrative * NarrativeList::getNarrativeReference(unsigned int i)
{
	assert(i < m_narratives.size());
	return m_narratives[i];
}

void NarrativeList::clear()
{
	m_narratives.clear(); 
	m_list_gui->clear();
	m_focus = -1;
	m_model = nullptr;
}

void NarrativeList::loadFromNode(osg::Node * model)
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
			addNarrative(nar);
			qDebug() << "found narrative" << nar->getName().c_str();
		}
	}
}

