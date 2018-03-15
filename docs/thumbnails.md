For some reason rendering thumbnails is harder and slower than I thought

issues:
- there can only be one osgViewer::Viewer per scene. why? I'm not sure but destroying one of 2 viewers looking at a scene crashes.

-> we have to use osgViewer::CompositeViewer.

Rendering is all batched together though

osg batches all 'view' renderings together whenever you call viewer->frame()

solutions
- add to viewer, frame(), remove from viewer


Qt renders separately, osg renders all at once


- first rendering of an OSGViewerWidget is really slow. I'm not sure why but maybe it has to to with making the OpenGL context.
  total: 180ms
    viewer->render() 150ms
      resizeEvent() 110ms
    canvas->render() 10ms

reusing one OSGViewerWidget

  total: 80ms
    setup 19ms
    render 38ms
      frame .4ms
    canvas 12ms

Searching for the problem:

subsequent renders are way faster and there is no resize event
  render 38ms
  r2 6ms
  r3 4ms

different img/painter
  r2 4ms

different scene
  r2 4ms

move/resize***
  r2 50ms
 solution: just use a fixed size

camera freeze
  r2 20ms
 solution: setCameraFrozen shortcutting
 after: 4ms