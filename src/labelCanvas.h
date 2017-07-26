#ifndef LABELCANVAS_H
#define LABELCANVAS_H

#include <QtWidgets>
#include <QtGui>
#include <QtCore>
#include <QList>
#include "dragLabel.h"
#include "editButtons.h"

class dragLabel;

class labelCanvas : public QFrame
{
	Q_OBJECT

public:
	labelCanvas(QWidget* parent = nullptr);
	~labelCanvas();

	void resizeEvent(QResizeEvent* event);
	void showSlides(int idx);
	void editCanvas();
	void clearCanvas();

public slots:
	void newLabel(QString style);
	void newLabel(std::string style, std::string text, int x, int y, int w, int h, int ph, int pw, float rh, float rw);
	void deleteLabel(int idx);
	void exitEdit();

signals:
	void sSuperTextSet(QString, int);
	void sSuperSizeSet(QSize, int);
	void sSuperPosSet(QPoint, int);
	void sNewLabel(std::string, int);

public:
	QVector<dragLabel*> m_items;
	QWidget* invisible;
	editButtons* editDlg;
	int offset = 0;
	int idx = 0;
};

#endif // LABELCANVAS_H