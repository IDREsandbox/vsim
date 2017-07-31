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
	void showSlides(int idx);
	void editCanvas();
	void clearCanvas();

public slots:
	void newLabel(QString style);
	void newLabel(std::string style, std::string text, float rX, float rY, float rW, float rH);
	void deleteLabel();
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
	int idx = 0;
	int lastSelected = 0;
};

#endif // LABELCANVAS_H