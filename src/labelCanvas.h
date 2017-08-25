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

signals:
	void sSetPos(float rx, float ry, int index);
	void sSetSize(float rw, float rh, int index);

public:
	QVector<dragLabel*> m_items;
	QWidget* invisible;

	int idx = 0;
	int lastSelected = 0;

	UndoRedoFilter *m_undo_redo_filter;

	NarrativeSlide *m_slide;
};

// filters out ctrl-z and ctrl-y
class UndoRedoFilter : public QObject {
public:
	UndoRedoFilter(QObject *parent) : QObject(parent) {}

	bool eventFilter(QObject *obj, QEvent *e) override;
};

#endif // LABELCANVAS_H