
#include "labelCanvas.h"

labelCanvas::labelCanvas(QWidget* parent)
	: QWidget(parent)
{
	invisible = new QWidget(this);
	invisible->setGeometry(0, 0, 1, 1);
	invisible->setStyleSheet("color:rgba(255, 255, 255, 0); background:transparent;");

	editDlg = new editButtons(this);
	editDlg->move(10, 180);
	editDlg->hide();

	this->setAttribute(Qt::WA_TransparentForMouseEvents, true);

	QSignalMapper* signalMapper = new QSignalMapper(this);

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
	connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(newLabel(QString)));
}

labelCanvas::~labelCanvas() 
{
}

void labelCanvas::editCanvas() 
{
	editDlg->show();
	foreach(dragLabel* lbl, m_items)
		lbl->setFrameStyle(QFrame::Panel | QFrame::Raised);
	this->setAttribute(Qt::WA_TransparentForMouseEvents, false);
}

void labelCanvas::exitEdit() 
{
	editDlg->hide(); 
	foreach(dragLabel* lbl, m_items)
		lbl->setFrameStyle(QFrame::NoFrame);
	this->setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

void labelCanvas::clearCanvas()
{
	m_items.clear();
	while (dragLabel* w = findChild<dragLabel*>())
		delete w;
}

void labelCanvas::resizeEvent(QResizeEvent* event) 
{
	foreach(dragLabel* lbl, m_items)
		lbl->canvasResize();
}

void labelCanvas::newLabel(QString style)
{//method for creating novel label
	dragLabel *new_item = new dragLabel(this, style.toStdString());
	new_item->show();
	idx = m_items.length();
	new_item->setIndex(idx);
	m_items.push_back(new_item);

	connect(new_item, SIGNAL(sTextSet(QString, int)), this, SIGNAL(sSuperTextSet(QString, int)));
	connect(new_item, SIGNAL(sSizeSet(QSize, int)), this, SIGNAL(sSuperSizeSet(QSize, int)));
	connect(new_item, SIGNAL(sPosSet(QPoint, int)), this, SIGNAL(sSuperPosSet(QPoint, int)));

	emit sNewLabel(style.toStdString(), idx);
}

void labelCanvas::newLabel(std::string style, std::string text, float rX, float rY, float rW , float rH)
{//method for creating label from loaded data via NarrativeControl
	dragLabel *new_item = new dragLabel(text, style, this, rH, rW, rY, rX);
	new_item->show();
	idx = m_items.length();
	new_item->setIndex(idx);
	m_items.push_back(new_item);

	connect(new_item, SIGNAL(sTextSet(QString, int)), this, SIGNAL(sSuperTextSet(QString, int)));
	connect(new_item, SIGNAL(sSizeSet(QSize, int)), this, SIGNAL(sSuperSizeSet(QSize, int)));
	connect(new_item, SIGNAL(sPosSet(QPoint, int)), this, SIGNAL(sSuperPosSet(QPoint, int)));
}

void labelCanvas::deleteLabel() {
	dragLabel* f = m_items.at(lastSelected);
	m_items.removeAt(lastSelected);
	delete f;

	emit sDeleteLabel(lastSelected);

	for (int i = 0; i < m_items.count(); ++i)
	{
		dragLabel* f = m_items.at(lastSelected);
		f->m_index = i;
	}
}