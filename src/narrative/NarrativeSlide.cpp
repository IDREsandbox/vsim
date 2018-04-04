#include "narrative/NarrativeSlide.h"
#include "narrative/NarrativeSlideLabel.h"
#include "deprecated/osgNewWidget/VSWidget.h"
#include <QTextCursor>
#include <iostream>
NarrativeSlide::NarrativeSlide()
	: Group(),
	m_camera_matrix(),
	m_duration(4.0f),
	m_stay_on_node(false),
	m_transition_duration(4.0f),
	m_thumbnail_dirty(true)
{
}

NarrativeSlide::NarrativeSlide(const NarrativeNode * old, const NarrativeTransition * old_transition)
	: NarrativeSlide()
{
	m_camera_matrix = old->getViewMatrix();
	m_duration = old->getPauseAtNode();
	m_stay_on_node = old->getStayOnNode();
	m_transition_duration = old_transition->getDuration();

	// convert old labels into new ones
	qDebug() << "converting old slide" << old->getNumChildren();
	for (uint i = 0; i < old->getNumChildren(); i++) {
		const osg::Node *child = old->getChild(i);
		qDebug() << "child" << i << child->className();
		const VSCanvas *canvas = dynamic_cast<const VSCanvas*>(child);
		// get the children
		if (!canvas) continue;
		qDebug() << "FOUND CANVAS";
		qDebug() << canvas->getNumChildren();

		for (uint j = 0; j < canvas->getNumChildren(); j++) {
			const osg::Node *canchild = canvas->getChild(j);		
			qDebug() << "cavnas child:" << canchild->className();
		}
		const osg::Group *cg = canvas->getGroup();
		for (uint j = 0; j < cg->getNumChildren(); j++) {
			const osg::Node *canvas_child = cg->getChild(j);
			qDebug() << "cavnas child:" << canvas_child->className();

			// assume the old dimensions...
			QSizeF old_dim(1280, 800);

			// cast to label
			const VSLabel *const_label = dynamic_cast<const VSLabel*>(canvas_child);
			VSLabel *old_label = const_cast<VSLabel*>(const_label); // old code didn't use const properly
			if (const_label) {
				NarrativeSlideLabel *new_label = new NarrativeSlideLabel();
				// The old dimensions were 
				osg::Vec2f old_size = const_label->getSize();
				osg::Vec2f old_pos = const_label->getPosition();
				std::string old_text = old_label->getLabel();

				QColor bgcolor;
				QColor fontcolor;
				osg::Vec4f bgcolor_old = old_label->getColor();
				osg::Vec4f fontcolor_old = old_label->getFontColor();

				bgcolor.setRgbF(bgcolor_old.r(), bgcolor_old.g(), bgcolor_old.b(), bgcolor_old.a());
				fontcolor.setRgbF(fontcolor_old.r(), fontcolor_old.g(), fontcolor_old.b(), fontcolor_old.a());

				qDebug() << "bgcolor" << bgcolor;
				qDebug() << "fontcolor" << fontcolor;

				float height = old_label->getText()->getCharacterHeight();
				qDebug() << "charheight" << height;
				const osgText::Font *f = old_label->getText()->getFont();
				if (!f) {
					qDebug() << "no font found";
				}
				else {
					qDebug() << "found font" << f->getName().c_str();
				}

				// old coordinates were +x right, +y up, so we have to flip
				// pixels from top, [0, 800]
				float old_top = old_dim.height() - old_pos[1] - old_size[1];

				// convert y [0, 800] -> [-.5, .5]
				// convert x [0, 1280] -> [-.6, .6]
				float new_x = (old_pos[0] - old_dim.width() / 2) / old_dim.height();
				float new_y = (old_top - old_dim.height() / 2) / old_dim.height();
				float new_w = old_size[0] / old_dim.height();
				float new_h = old_size[1] / old_dim.height();

				new_label->setRect(QRectF(new_x, new_y, new_w, new_h));
				new_label->getDocument()->setPlainText(old_text.c_str());
				new_label->setBackground(bgcolor);

				QTextCursor cursor(new_label->getDocument());
				cursor.select(QTextCursor::SelectionType::Document);
				QTextCharFormat format = cursor.charFormat();
				format.setForeground(QBrush(fontcolor));
				format.setFontPointSize(height);
				cursor.setCharFormat(format);
				cursor.insertText(old_text.c_str());

				addChild(new_label);
			}
			// finally we can make a new label
			
			// TODO: cast to widget and convert images

		}
	}
}

const osg::Matrixd & NarrativeSlide::getCameraMatrix() const
{
	return m_camera_matrix;
}
void NarrativeSlide::setCameraMatrix(const osg::Matrixd & matrix)
{
	m_camera_matrix = matrix;
	emit sCameraMatrixChanged(matrix);
	dirtyThumbnail();
}

float NarrativeSlide::getDuration() const
{
	return m_duration;
}

void NarrativeSlide::setDuration(float duration)
{
	if (duration < 0) {
		qWarning() << "NarrativeSlide cannot have a negative duration";
		return;
	}
	m_duration = duration;
	emit sDurationChanged(duration);
}

bool NarrativeSlide::getStayOnNode() const
{
	return m_stay_on_node;
}

void NarrativeSlide::setStayOnNode(bool stay)
{
	m_stay_on_node = stay;
	emit sStayOnNodeChanged(stay);
}

float NarrativeSlide::getTransitionDuration() const
{
	return m_transition_duration;
}
void NarrativeSlide::setTransitionDuration(float tduration)
{
	if (tduration < 0) {
		qWarning() << "NarrativeSlide cannot have a negative transition duration";
		return;
	}
	m_transition_duration = tduration;
	emit sTransitionDurationChanged(tduration);
}

void NarrativeSlide::dirtyThumbnail()
{
	m_thumbnail_dirty = true;
	emit sThumbnailDirty();
}

bool NarrativeSlide::thumbnailDirty() const
{
	return m_thumbnail_dirty;
}

const QImage & NarrativeSlide::getThumbnail() const
{
	// TODO: insert return statement here
	return m_thumbnail;
}

void NarrativeSlide::setThumbnail(QImage thumbnail)
{
	m_thumbnail = thumbnail;
	m_thumbnail_dirty = false;
	emit sThumbnailChanged(m_thumbnail);
}