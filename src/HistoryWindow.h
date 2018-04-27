#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H
#include <QUndoView>
#include <QUndoStack>

class HistoryWindow : public QWidget {
	Q_OBJECT
public:
	HistoryWindow(QWidget *parent = nullptr);
	void setStack(QUndoStack *stack);

private:
	QUndoView *m_view;
};

#endif