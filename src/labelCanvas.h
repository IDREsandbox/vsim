#ifndef LABELCANVAS_H
#define LABELCANVAS_H

#include <QtWidgets>
#include <QtGui>
#include <QtCore>
#include <QList>

#include "editButtons.h"

class dragLabel;
class NarrativeSlide;
class UndoRedoFilter;

class labelCanvas : public QWidget
{
	Q_OBJECT

public:
	labelCanvas(QWidget* parent = nullptr);
	~labelCanvas();

	void clearCanvas();

	int getSelection() const;
	void setSelection(int index);

	void setSlide(NarrativeSlide *slide);

	void insertNewLabel(int index);
	void deleteLabel(int index);

	QSize baseSize() const;

	// for stealing undo/redo from children
	bool eventFilter(QObject * obj, QEvent * e) override;

	
signals:
	void sSetPos(float rx, float ry, int index);
	void sSetSize(float rw, float rh, int index);
	void sEdited(int index);

public:
	QVector<dragLabel*> m_items;
	QWidget* invisible;

	int idx = 0;
	int lastSelected = 0;

	NarrativeSlide *m_slide;

};

#endif // LABELCANVAS_H