#ifndef NARRATIVESLIDE_H
#define NARRATIVESLIDE_H

#include <osg/Group>
#include <osg/Image>
#include <QDebug>
#include <QImage>
#include "deprecated/narrative/NarrativeNode.h"

#include <QUndoStack>
#include "Core/Command.h"
#include "Core/GroupTemplate.h"

class CanvasScene;

class NarrativeSlide : public QObject
{
	Q_OBJECT;
public:
	NarrativeSlide(QObject *parent = nullptr);
	NarrativeSlide &operator=(const NarrativeSlide &other);
	void loadOld(const NarrativeNode *old, const NarrativeTransition *old_transition);
	
	osg::Matrixd getCameraMatrix() const;
	void setCameraMatrix(const osg::Matrixd& matrix);
	
	float getDuration() const;
	void setDuration(float duration);
	bool getStayOnNode() const;
	void setStayOnNode(bool stay);
	float getTransitionDuration() const;
	void setTransitionDuration(float tduration);

	// thumbnail cache
	void dirtyThumbnail();
	bool thumbnailDirty() const;
	const QImage &getThumbnail() const;
	void setThumbnail(QImage);

	CanvasScene *scene() const;

signals:
	void sCameraMatrixChanged(const osg::Matrixd&);
	void sDurationChanged(float);
	void sStayOnNodeChanged(bool);
	void sTransitionDurationChanged(float);
	void sThumbnailChanged(const QImage &image);
	void sThumbnailDirty();

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
	class SetCameraMatrixCommand : public ModifyCommand<NarrativeSlide, const osg::Matrixd&> {
	public:
		SetCameraMatrixCommand(NarrativeSlide *slide, osg::Matrixd camera, QUndoCommand *parent = nullptr)
			: ModifyCommand(&getCameraMatrix, &setCameraMatrix, camera, slide, parent) {}
	};

private:
	osg::Matrixd m_camera_matrix;
	float m_duration;
	bool m_stay_on_node;
	float m_transition_duration;

	QImage m_thumbnail;
	bool m_thumbnail_dirty;

	CanvasScene *m_scene;
};

#endif /* NARRATIVESLIDE_H */
