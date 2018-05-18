
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_BINARY_DIR})

set(CanvasEditor_src
  ${CMAKE_SOURCE_DIR}/src/Canvas/CanvasControl.cpp
  ${CMAKE_SOURCE_DIR}/src/Canvas/CanvasContainer.cpp
  ${CMAKE_SOURCE_DIR}/src/Canvas/CanvasScene.cpp
  ${CMAKE_SOURCE_DIR}/src/Canvas/CanvasToolBar.cpp
  ${CMAKE_SOURCE_DIR}/src/Canvas/CanvasEditor.cpp
  ${CMAKE_SOURCE_DIR}/src/Canvas/LabelStyleGroup.cpp
  ${CMAKE_SOURCE_DIR}/src/Canvas/LabelStyle.cpp
  ${CMAKE_SOURCE_DIR}/src/Core/SimpleCommandStack.cpp
  CanvasEditor_experiment.cpp
)
add_executable(CanvasEditor_experiment
  EXCLUDE_FROM_ALL
  ${CanvasEditor_src})
target_link_libraries(CanvasEditor_experiment Qt5::Widgets Qt5::Test)
# target_link_libraries(CanvasEditor_experiment Util_obj)
# ${OPENSCENEGRAPH_LIBRARIES}

set_property(TARGET CanvasEditor_experiment PROPERTY CXX_STANDARD 17)
set_target_properties(CanvasEditor_experiment PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")


