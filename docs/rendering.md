
How does rendering work? I have no idea.

# problems

There are a lot of conflicting forces, confusing things. I'm just going to dump my complaints then say how things work.

OSG wants to render, has its own threading system, viewer, graphicswindow, views...

Running two osg viewers w/ same scene and rendering ->frame() crashes. OSG viewer by default needs to batch render I think?

If you want to renders going on (thumbnails and main view) you have to find a way to break up QGraphicsWindowEmbedded among multiple widgets.

Qt really wants to render, has its own threading system, windows

Qt has its own version of OpenGL.
\#include <QOpenGLFunctions> undefs all of \#include <gl/glew.h>.
It's hard to intermix raw gl calls with Qt gl calls.

QOpenGLWidget can't draw offscreen (until Qt 5.10).

QOpenGLWidget anti aliasing doesn't work, at least on my AMD card.

OpenGL itself is a headache.

# what is what

osg GraphicsWindow represents a renderable surface & context in osg. So a normal osg window would call window->makeCurrent() then do opengl rendering.

GraphicsWindowEmbedded is an empty shell, and expects someone else to take care of OpenGL setup and context stuff.

QOpenGLContext wraps

QScreen is?

QSurface is a renderable surface. Goes together with context.

QWindow is a native-ish thing, Qt tries to hide it so it's hard to get QWindow from QWidget and vice versa.

FBO is a FramebufferObject. It's a buffer that OpenGL can render to, stored on GPU.


# how things work now

We use QOpenGLWidget because that's the closest thing to what we want. You can read the source for it.

See the draw.io pictures

Qt rendering magic
 ^
QOpenGLWidget
 offscreen surface
  ^
 internal FBO
  ^
 our FBO (anti aliasing)
  ^
 osg code


Problems and solutions

1. anti aliasing doesn't work - we render to our own FBO first, then blit down to QOpenGLWidget's FBO

# other things I tried

QOffscreenSurface didn't work for me. I tried drawing an opengl triangle onto one then displaying the framebuffer. The opengl didn't draw, but QPaintDevice stuff did.

QWindow did work for me. I was able to paint to a fbo in a context based on a QWindow.

# potential alternatives

There is QWidget::createWindowContainer()