
set(Canvas_h
  CanvasControl.h
  CanvasContainer.h
  CanvasScene.h
  CanvasToolBar.h
  CanvasEditor.h
  LabelStyleGroup.h
  LabelStyle.h
  LabelType.h
)

set(Canvas_src
  CanvasControl.cpp
  CanvasContainer.cpp
  CanvasScene.cpp
  CanvasToolBar.cpp
  CanvasEditor.cpp
  LabelStyleGroup.cpp
  LabelStyle.cpp
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

# set(Canvas_src
#   ${CMAKE_SOURCE_DIR}/src/Canvas/CanvasControl.cpp
#   ${CMAKE_SOURCE_DIR}/src/Canvas/CanvasContainer.cpp
#   ${CMAKE_SOURCE_DIR}/src/Canvas/CanvasScene.cpp
#   Canvas_experiment.cpp
# )
# add_executable(Canvas_experiment
#   EXCLUDE_FROM_ALL
#   ${Canvas_src})
# target_link_libraries(Canvas_experiment Qt5::Widgets Qt5::Test)
# set_target_properties(Canvas_experiment PROPERTIES
#   VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
