#include "labelCanvas.h"
#include "dragLabel.h"

#include "narrative/NarrativeSlide.h"
#include "narrative/NarrativeSlideLabels.h"

labelCanvas::labelCanvas(QWidget* parent)
	: QWidget(parent),
	m_slide(nullptr)
{
	//m_scene = new QGraphicsScene();
	//this->setScene(m_scene);
	//this->scene()->setSceneRect(this->geometry());
	//this->setStyleSheet("background: transparent");

	//this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	invisible = new QWidget(this);
	//this->scene()->addWidget(invisible);
	invisible->setGeometry(0, 0, 1, 1);
	invisible->setStyleSheet("color:rgba(255, 255, 255, 0); background:transparent;");

	m_undo_redo_filter = new UndoRedoFilter(this);

	//editDlg = new editButtons(this);
	//this->scene()->addWidget(editDlg);
	//editDlg->move(10, 180);
	//editDlg->hide();

	//this->setAttribute(Qt::WA_TransparentForMouseEvents, true);

	//scaleFactor = std::max(float(1.0), float(float(this->size().height()) / float(720)));

	/*QSignalMapper* signalMapper = new QSignalMapper(this);

	connect(editDlg->ui.done, &QPushButton::clicked, this, &labelCanvas::exitEdit);
	connect(editDlg->ui.delete_2, &QPushButton::clicked, this, &labelCanvas::deleteLabel);
	connect(editDlg->ui.label, SIGNAL(clicked()), signalMapper, SLOT(map()));
	connect(editDlg->ui.head1, SIGNAL(clicked()), signalMapper, SLOT(map()));
	connect(editDlg->ui.head2, SIGNAL(clicked()), signalMapper, SLOT(map()));
	connect(editDlg->ui.body, SIGNAL(clicked()), signalMapper, SLOT(map()));

	signalMapper->setMapping(editDlg->ui.label, QString("color:rgb(0, 0, 0); background: rgba(255, 255, 255, 70); font-size: 12pt; font-family: \"Arial\"; font-weight: bold; text-align:center;"));
	signalMapper->setMapping(editDlg->ui.head1, QString("color:rgb(240, 240, 240); background-color:rgba(0, 0, 0, 70); font-size: 36pt; font-family: \"Arial\"; font-weight: bold; text-align:center;"));
	signalMapper->setMapping(editDlg->ui.head2, QString("color:rgb(224, 147, 31); background-color:rgba(0, 0, 0, 70); font-size: 20pt; font-family: \"Arial\"; font-weight: bold; text-align:center;"));
	signalMapper->setMapping(editDlg->ui.body, QString("color:rgb(240, 240, 240); background-color:rgba(0, 0, 0, 70); font-size: 12pt; font-family: \"Arial\"; font-weight: regular; text-align:left;"));
	connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(newLabel(QString)));*/
}

labelCanvas::~labelCanvas() 
{
}

//void labelCanvas::editCanvas() 
//{
//	editDlg->show();
//	//foreach(dragLabel* lbl, m_items)
//	//	lbl->setFrameStyle(QFrame::Panel | QFrame::Raised);
//	this->setAttribute(Qt::WA_TransparentForMouseEvents, false);
//}
//
//void labelCanvas::exitEdit() 
//{
//	editDlg->hide(); 
//	//foreach(dragLabel* lbl, m_items)
//	//	lbl->setFrameStyle(QFrame::NoFrame);
//	this->setAttribute(Qt::WA_TransparentForMouseEvents, true);
//}

void labelCanvas::clearCanvas()
{
	m_items.clear();
	while (dragLabel* w = findChild<dragLabel*>())
		delete w;
	//this->scene()->clear();
	//this->scene()->addWidget(editDlg);
}

int labelCanvas::getSelection() const
{
	return lastSelected;
}

void labelCanvas::setSelection(int index)
{
	lastSelected = index;
}

void labelCanvas::setSlide(NarrativeSlide * slide)
{
	if (m_slide) disconnect(m_slide, 0, this, 0);
	m_slide = slide;
	clearCanvas();
	if (!slide) return;

	NarrativeSlideLabels* data;
	for (uint i = 0; i < slide->getNumChildren(); i++) {
		insertNewLabel(i);
	}

	connect(slide, &NarrativeSlide::sNew, this, &labelCanvas::insertNewLabel);
	connect(slide, &NarrativeSlide::sDelete, this, &labelCanvas::deleteLabel);
}

void labelCanvas::insertNewLabel(int index)
{
	qDebug() << "insert new label" << index;
	dragLabel *new_item = new dragLabel(this);
	m_items.insert(index, new_item);
	new_item->show();

	// if our group has this item, then bind it
	new_item->setLabel(dynamic_cast<NarrativeSlideLabels*>(m_slide->getChild(index)));

	// fix up indices
	for (size_t i = 0; i < m_items.size(); i++) {
		m_items[i]->setIndex(i);
	}

	// forward signals
	connect(new_item, &dragLabel::sSetPos, this, &labelCanvas::sSetPos);
	connect(new_item, &dragLabel::sSetSize, this, &labelCanvas::sSetSize);
}

void labelCanvas::deleteLabel(int index) {
	dragLabel *item = m_items.takeAt(index);
	delete item;

	// fix up indices
	for (size_t i = 0; i < m_items.size(); i++) {
		m_items[i]->setIndex(i);
	}
}

bool UndoRedoFilter::eventFilter(QObject * obj, QEvent * e)
{
	if (e->type() == QEvent::KeyPress) {
		QKeyEvent *ke = static_cast<QKeyEvent*>(e);
		if (ke->matches(QKeySequence::Redo) || ke->matches(QKeySequence::Undo)) {
			return true;
		}
	}
	return false;
}
