#include "NarrativeScrollBox.h"

NarrativeScrollBox::NarrativeScrollBox(QWidget * parent)
	: HorizontalScrollBox(parent),
	m_narratives(nullptr)
{
	
	// initialize menus
	m_slide_menu = new QMenu(tr("Slide context menu"), this);
	m_action_new = new QAction("New Narrative", m_slide_menu);
	m_action_delete = new QAction("Delete Narrative", m_slide_menu);
	m_action_info = new QAction("Edit Narrative Info", m_slide_menu);
	m_action_open = new QAction("Open Narrative", m_slide_menu);

	m_slide_menu->addAction(m_action_new);
	m_slide_menu->addAction(m_action_delete);
	m_slide_menu->addAction(m_action_info);
	m_slide_menu->addAction(m_action_open);

	// forward signals
	connect(m_action_new, &QAction::triggered, this, &NarrativeScrollBox::sNew);
	connect(m_action_delete, &QAction::triggered, this, &NarrativeScrollBox::sDelete);
	connect(m_action_info, &QAction::triggered, this, &NarrativeScrollBox::sInfo);
	connect(m_action_open, &QAction::triggered, this, &NarrativeScrollBox::sOpen);
}

NarrativeScrollBox::~NarrativeScrollBox() {
	
}

void NarrativeScrollBox::setNarrativeGroup(NarrativeGroup *group)
{
	if (m_narratives != nullptr) disconnect(m_narratives, 0, this, 0);

	clear();
	m_narratives = group;
	if (group == nullptr) return;

	connect(m_narratives, &NarrativeGroup::sNewNarrative, this, &NarrativeScrollBox::newItem);
	connect(m_narratives, &NarrativeGroup::sDeleteNarrative, this, &HorizontalScrollBox::deleteItem);

	for (uint i = 0; i < m_narratives->getNumChildren(); i++) {
		newItem(i);
	}
}

void NarrativeScrollBox::newItem(int index) {
	Narrative2 *nar = dynamic_cast<Narrative2*>(m_narratives->getChild(index));
	if (nar == nullptr) {
		qWarning() << "insert new narrative" << index << "is not a NarrativeSlide";
		return;
	}
	insertNewNarrative(index, nar);
}

void NarrativeScrollBox::insertNewNarrative(int index, Narrative2 * narrative)
{
	qDebug() << "insert new narrative";
	NarrativeScrollItem *item = new NarrativeScrollItem(narrative);
	connect(item, &NarrativeScrollItem::sDoubleClick, this, &NarrativeScrollBox::sOpen);
	HorizontalScrollBox::insertItem(index, item);
}

void NarrativeScrollBox::openMenu(QPoint globalPos)
{
	m_slide_menu->exec(globalPos);
}
void NarrativeScrollBox::openItemMenu(QPoint globalPos)
{
	m_slide_menu->exec(globalPos);
}

