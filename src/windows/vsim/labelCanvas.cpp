
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

	QSignalMapper* signalMapper = new QSignalMapper(this);

	connect(editDlg->ui.edit, &QPushButton::clicked, this, &labelCanvas::editCanvas);
	connect(editDlg->ui.done, &QPushButton::clicked, this, &labelCanvas::exitEdit);
	connect(editDlg->ui.label, SIGNAL(clicked()), signalMapper, SLOT(map()));
	connect(editDlg->ui.head1, SIGNAL(clicked()), signalMapper, SLOT(map()));
	connect(editDlg->ui.head2, SIGNAL(clicked()), signalMapper, SLOT(map()));
	connect(editDlg->ui.body, SIGNAL(clicked()), signalMapper, SLOT(map()));

	signalMapper->setMapping(editDlg->ui.label, QString("background: rgba(0, 0, 0, 70); color: rgb(255, 255, 255);"));
	signalMapper->setMapping(editDlg->ui.head1, QString("color:rgba(0, 255, 255, 100); background-color:rgba(0, 0, 0, 50); font-family: \"New Century Schoolbook\";"));
	signalMapper->setMapping(editDlg->ui.head2, QString("color:rgba(255, 0, 255, 100); background-color:rgba(0, 0, 0, 50); font-family: \"Times New Roman\";"));
	signalMapper->setMapping(editDlg->ui.body, QString("color:rgba(255, 255, 0, 100); background-color:rgba(0, 0, 0, 50);"));
	connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(newLabel(QString)));
}

labelCanvas::~labelCanvas() 
{
}

void labelCanvas::editCanvas() {
	editDlg->show();
}

void labelCanvas::exitEdit() {
	editDlg->hide();
}

void labelCanvas::resizeEvent(QResizeEvent* event) {
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
{//method for creating novel label and also for creating label from loaded data via NarrativeControl
	qDebug() << style;
	dragLabel *new_item = new dragLabel(this, style.toStdString());
	new_item->setGeometry(250, 250, 250, 110);
	new_item->show();
	idx = m_items.length();
	new_item->setIndex(idx);
	m_items.push_back(new_item);

	connect(new_item, SIGNAL(sTextSet(QString, int)), this, SIGNAL(sSuperTextSet(QString, int)));
	connect(new_item, SIGNAL(sSizeSet(QSize, int)), this, SIGNAL(sSuperSizeSet(QSize, int)));
	connect(new_item, SIGNAL(sPosSet(QPoint, int)), this, SIGNAL(sSuperPosSet(QPoint, int)));
}

void labelCanvas::deleteLabel(int idx) {
	m_items.removeAt(idx);
}