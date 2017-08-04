#ifndef NARRATIVEMODEL_H
#define NARRATIVEMODEL_H

#include <QUndoStack>
#include <osg/Group>

#include "Narrative2.h"
#include "NarrativeSlide.h"
#include "NarrativeSlideLabels.h"



class NarrativeModel : public QObject {
	Q_OBJECT
public:
	NarrativeModel(osg::Group *narrative_group, QUndoStack *stack, QObject *parent = nullptr);

	//
	osg::Group *getRoot();
	Narrative2 *getNarrative(int narrative);
	NarrativeSlide *getSlide(int narrative, int slide);
	NarrativeSlideLabels *getLabel(int narrative, int slide, int label);

	// All commands
	void newNarrative(int index);
	void deleteNarrative(int index);
	void setNarrativeInfo();
	void moveNarrative();

signals:
	void sNewNarrative(int index);
	void sDeleteNarrative(int index);
	void sSetNarrativeInfo();
	void sMoveNarrative();
	//void sImportNarrative();

private:
	osg::Group *m_root;
	QUndoStack *m_stack;

//// COMMANDS
//	friend class NewNarrativeCommand;
//	friend class DeleteNarrativeCommand;
//	friend class SetNarrativeInfoCommand;
//	friend class MoveNarrativeCommand;
//	//friend class DuplicateNarrativeCommand;
//	//friend class ImportNarrativeCommand;
//	friend class NewSlideCommand;
//	friend class DeleteSlidesCommand;
//	friend class SetSlidesDurationCommand;
//	friend class SetSlidesTransitionCommand;
//	friend class SetSlidesCameraCommand;
//	friend class NewLabelCommand;
//	friend class DeleteLabelCommand;
//	friend class MoveLabelCommand;
//	friend class ResizeLabelCommand;
//	friend class SetLabelTextCommand;
};



#endif