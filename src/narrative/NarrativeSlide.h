#ifndef NARRATIVESLIDE_H
#define NARRATIVESLIDE_H

#include <osg/Group>
#include <osg/Image>
#include <QDebug>
#include <QPixmap>
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
	QPixmap thumbnail() const;
	QPixmap thumbnailBackground() const;

	bool thumbnailForegroundDirty() const;
	bool thumbnailBackgroundDirty() const;

	void setThumbnailForeground(QPixmap img);
	void setThumbnailBackground(QPixmap img);

	void setForegroundDirty();
	void setBackgroundDirty();

	CanvasScene *scene() const;

signals:
	void sCameraMatrixChanged(const osg::Matrixd&);
	void sDurationChanged(float);
	void sStayOnNodeChanged(bool);
	void sTransitionDurationChanged(float);
	//void sThumbnailChanged(const QImage &image);
	void sThumbnailDirty();

	void sThumbnailChanged(QPixmap pixmap);
	//void sThumbnailForegroundChanged(QPixmap image);
	//void sThumbnailBackgroundChanged(QPixmap image);

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
	void compositeThumbnail();

private:
	osg::Matrixd m_camera_matrix;
	float m_duration;
	bool m_stay_on_node;
	float m_transition_duration;

	QPixmap m_thumbnail_foreground;
	QPixmap m_thumbnail_background;

	QPixmap m_thumbnail;

	CanvasScene *m_scene;
};

#endif /* NARRATIVESLIDE_H */
