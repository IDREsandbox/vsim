// Credits and thanks to Bastian Rieck for fitting VSim with Qt http://bastian.rieck.ru/blog/posts/2014/qt_and_openscenegraph/
// Small adaptations were made for VSim purposes

// This is a Qt Widget with an openscenegraph GraphicsWindowEmbedded + viewer inside

#ifndef OSGVIEWERWIDGET_H
#define OSGVIEWERWIDGET_H

#include <QOpenGLWidget>

#include <osg/ref_ptr>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>

class OSGViewerWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	OSGViewerWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);

	osgViewer::Viewer *getViewer();

protected:
	virtual void paintEvent(QPaintEvent* paintEvent);
	virtual void paintGL();
	virtual void resizeGL(int width, int height);

	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);

	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void wheelEvent(QWheelEvent* event);

	virtual bool event(QEvent* event);

private:
	virtual void onHome();
	virtual void onResize(int width, int height);

	osgGA::EventQueue* getEventQueue() const;

	osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphicsWindow_;
	osg::ref_ptr<osgViewer::Viewer> viewer_;

	QPoint selectionStart_;
	QPoint selectionEnd_;

	bool selectionActive_;
	bool selectionFinished_;

	void processSelection();
};
#endif