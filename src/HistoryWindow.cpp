#include "HistoryWindow.h"
#include <QGridLayout>
#include <QDebug>

HistoryWindow::HistoryWindow(QWidget * parent)
	: QWidget(parent)
{
	setWindowTitle("Edit History");
	setWindowFlags(Qt::Window);
	setWindowFlag(Qt::WindowType::WindowContextHelpButtonHint, false);

	QGridLayout *m_layout = new QGridLayout(this);

	m_view = new QUndoView(this);
	m_layout->addWidget(m_view);
	m_layout->setMargin(0);
}

void HistoryWindow::setStack(QUndoStack *stack)
{
	m_view->setStack(stack);
}
