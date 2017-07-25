
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

	connect(editDlg->ui.edit, &QPushButton::clicked, this, &labelCanvas::editCanvas);
	connect(editDlg->ui.done, &QPushButton::clicked, this, &labelCanvas::exitEdit);
	connect(editDlg->ui.label, SIGNAL(clicked()), signalMapper, SLOT(map()));
	connect(editDlg->ui.head1, SIGNAL(clicked()), signalMapper, SLOT(map()));
	connect(editDlg->ui.head2, SIGNAL(clicked()), signalMapper, SLOT(map()));
	connect(editDlg->ui.body, SIGNAL(clicked()), signalMapper, SLOT(map()));

	signalMapper->setMapping(editDlg->ui.label, QString("background: rgba(0, 0, 0, 70); color: rgb(255, 255, 255);"));
	signalMapper->setMapping(editDlg->ui.head1, QString("color:rgba(0, 255, 255, 100); background-color:rgba(0, 0, 0, 70); font-family: \"New Century Schoolbook\";"));
	signalMapper->setMapping(editDlg->ui.head2, QString("color:rgba(255, 0, 255, 100); background-color:rgba(0, 0, 0, 70); font-family: \"Times New Roman\";"));
	signalMapper->setMapping(editDlg->ui.body, QString("color:rgba(255, 255, 0, 100); background-color:rgba(0, 0, 0, 70);"));
	connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(newLabel(QString)));
}

labelCanvas::~labelCanvas() 
{
}

void labelCanvas::editCanvas() 
{
	editDlg->show();
	this->setAttribute(Qt::WA_TransparentForMouseEvents, false);
}

void labelCanvas::exitEdit() 
{
	editDlg->hide(); 
	this->setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

void labelCanvas::clearCanvas()
{
	m_items.clear();
	while (dragLabel* w = findChild<dragLabel*>())
		delete w;
	//offset = 0;
}

void labelCanvas::resizeEvent(QResizeEvent* event) 
{
	if (offset < 2) {
		foreach (dragLabel* lbl, m_items)
			lbl->updateParSize();
		offset++;
	}

	if (offset >= 2)
		foreach (dragLabel* lbl, m_items)
			lbl->canvasResize();
}

void labelCanvas::newLabel(QString style)
{//method for creating novel label
	dragLabel *new_item = new dragLabel(this, style.toStdString());
	new_item->setGeometry(250, 250, 250, 110);
	new_item->show();
	idx = m_items.length();
	new_item->setIndex(idx);
	m_items.push_back(new_item);

	connect(new_item, SIGNAL(sTextSet(QString, int)), this, SIGNAL(sSuperTextSet(QString, int)));
	connect(new_item, SIGNAL(sSizeSet(QSize, int)), this, SIGNAL(sSuperSizeSet(QSize, int)));
	connect(new_item, SIGNAL(sPosSet(QPoint, int)), this, SIGNAL(sSuperPosSet(QPoint, int)));

	emit sNewLabel(style.toStdString(), idx);
}

void labelCanvas::newLabel(std::string style, std::string text, int x, int y, int w, int h, int ph, int pw, float rh, float rw) 
{//method for creating label from loaded data via NarrativeControl
	dragLabel *new_item = new dragLabel(text, style, this);
	new_item->oldParSize.setHeight(ph);
	new_item->oldParSize.setWidth(pw);
	new_item->parSize = this->size();
	new_item->ratioHeight = rh;
	new_item->ratioWidth = rw;
	new_item->setGeometry(x, y, w, h);
	new_item->show();
	idx = m_items.length();
	new_item->setIndex(idx);
	m_items.push_back(new_item);

	connect(new_item, SIGNAL(sTextSet(QString, int)), this, SIGNAL(sSuperTextSet(QString, int)));
	connect(new_item, SIGNAL(sSizeSet(QSize, int)), this, SIGNAL(sSuperSizeSet(QSize, int)));
	connect(new_item, SIGNAL(sPosSet(QPoint, int)), this, SIGNAL(sSuperPosSet(QPoint, int)));

	//emit sNewLabel(idx);
}

void labelCanvas::deleteLabel(int idx) {
	m_items.removeAt(idx);
}