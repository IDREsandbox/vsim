#ifndef NARRATIVESLIDE_H
#define NARRATIVESLIDE_H

#include <osg/Group>
#include <osg/Image>
#include <QDebug>
#include <QImage>
#include "deprecated/narrative/NarrativeNode.h"

#include <QUndoStack>
#include "Command.h"

class NarrativeSlide: public QObject, public osg::Group
{
	Q_OBJECT

public:	// DATA
	NarrativeSlide();
	NarrativeSlide(const NarrativeSlide& n, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	NarrativeSlide(const NarrativeNode *old, const NarrativeTransition *old_transition);
	virtual ~NarrativeSlide();
	
	META_Node(, NarrativeSlide);
	
	const osg::Matrixd& getCameraMatrix() const;
	void setCameraMatrix(const osg::Matrixd& matrix);
	
	float getDuration() const;
	void setDuration(float duration);
	bool getStayOnNode() const;
	void setStayOnNode(bool stay);
	float getTransitionDuration() const;
	void setTransitionDuration(float tduration);

	const osg::Image *getThumbnail() const;
	void setThumbnail(osg::Image *thumbnail);

signals:
	void sCameraMatrixChanged(const osg::Matrixd&);
	void sDurationChanged(float);
	void sStayOnNodeChanged(bool);
	void sTransitionDurationChanged(float);
	//void sThumbnailChanged();

public: // COMMANDS

	class SetStayOnNodeCommand : public ModifyCommand<NarrativeSlide, bool> {
	public:
		SetStayOnNodeCommand(NarrativeSlide *slide, bool stay, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getStayOnNode, &setStayOnNode, stay, slide, parent) {}
	};
	class SetDurationCommand : public ModifyCommand<NarrativeSlide, float> {
	public:
		SetDurationCommand(NarrativeSlide *slide, float duration, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getDuration, &setDuration, duration, slide, parent) {}
	};
	class SetTransitionDurationCommand : public ModifyCommand<NarrativeSlide, float> {
	public:
		SetTransitionDurationCommand(NarrativeSlide *slide, float duration, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getTransitionDuration, &setTransitionDuration, duration, slide, parent) {}
	};

private:
	osg::Matrixd m_camera_matrix;
	float m_duration;
	bool m_stay_on_node;
	float m_transition_duration;
	osg::ref_ptr<osg::Image> m_thumbnail;
};

#endif /* NARRATIVESLIDE_H */
