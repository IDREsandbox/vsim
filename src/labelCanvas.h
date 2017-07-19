#ifndef LABELCANVAS_H
#define LABELCANVAS_H

#include <QtWidgets>
#include <QtGui>
#include <QtCore>
#include <QList>
#include "dragLabel.h"
#include "editButtons.h"

class dragLabel;

class labelCanvas : public QWidget
{
	Q_OBJECT

public:
	labelCanvas(QWidget* parent = nullptr);
	~labelCanvas();

	void resizeEvent(QResizeEvent* event);
	void editCanvas();

public slots:
	void newLabel(QString style);
	void deleteLabel(int idx);
	void exitEdit();

signals:
	void sSuperTextSet(QString, int);
	void sSuperSizeSet(QSize, int);
	void sSuperPosSet(QPoint, int);

protected:
	QList<dragLabel*> m_items;
	QWidget* invisible;
	editButtons* editDlg;
	int offset = 0;
	int idx = 0;
};

#endif // LABELCANVAS_H