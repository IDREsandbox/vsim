
#include "labelCanvas.h"

labelCanvas::labelCanvas(QWidget* parent)
	: QWidget(parent)
{
	editDlg = new editButtons(this);
	editDlg->move(10, 180);
	//editDlg->hide();

	connect(editDlg, SIGNAL(sNewLabel(std::string)), this, SLOT(newLabel(std::string)));
	connect(editDlg, SIGNAL(sDeleteLabel(int)), this, SLOT(deleteLabel(int)));
	connect(editDlg, SIGNAL(sExitEdit()), this, SLOT(exitEdit()));
}

labelCanvas::~labelCanvas() 
{
}

void labelCanvas::editCanvas() {
	editDlg->show();
	qDebug() << "loading narrative";
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

void labelCanvas::newLabel(std::string style) 
{//method for creating novel label and also for creating label from loaded data via NarrativeControl
	dragLabel *new_item = new dragLabel(this, style);
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