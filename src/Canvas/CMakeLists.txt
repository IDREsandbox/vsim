
vsim_add_flatbuffer(canvas_fb_h canvas.fbs)

set(Canvas_h
  CanvasControl.h
  CanvasContainer.h
  CanvasEditor.h
  CanvasScene.h
  CanvasToolBar.h
  CanvasSerializer.h
  ChildMaskFilter.h
  LabelStyleGroup.h
  LabelStyle.h
  LabelType.h
  ${canvas_fb_h}
)

set(Canvas_src
  CanvasControl.cpp
  CanvasContainer.cpp
  CanvasEditor.cpp
  CanvasScene.cpp
  CanvasSerializer.cpp
  CanvasToolBar.cpp
  ChildMaskFilter.cpp
  LabelStyleGroup.cpp
  LabelStyle.cpp
  StyleSettingsDialog.cpp
  StyleSettingsDialog.h
  StyleSettingsDialog.ui
  StyleSettingsWidget.cpp
  StyleSettingsWidget.h
  StyleSettingsWidget.ui
)

add_library(Canvas_obj ${Canvas_src} ${Canvas_h})
target_link_libraries(Canvas_obj Core_obj Qt5::Widgets)

vsim_add_experiment(
  Canvas_experiment
  Canvas_experiment.cpp
  Canvas_obj
)
vsim_add_experiment(
  CanvasEditor_experiment
  CanvasEditor_experiment.cpp
  Canvas_obj
)
vsim_add_test(
  CanvasSerializer_test
  CanvasSerializer_test.cpp
  "Canvas_obj;Qt5::Test"
)
