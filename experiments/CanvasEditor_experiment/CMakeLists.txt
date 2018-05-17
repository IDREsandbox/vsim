
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_BINARY_DIR})

set(CanvasEditor_src
  ${CMAKE_SOURCE_DIR}/src/narrative/CanvasControl.cpp
  ${CMAKE_SOURCE_DIR}/src/narrative/CanvasContainer.cpp
  ${CMAKE_SOURCE_DIR}/src/narrative/CanvasScene.cpp
  ${CMAKE_SOURCE_DIR}/src/narrative/CanvasToolBar.cpp
  ${CMAKE_SOURCE_DIR}/src/narrative/CanvasEditor.cpp
  ${CMAKE_SOURCE_DIR}/src/narrative/LabelStyleGroup.cpp
  ${CMAKE_SOURCE_DIR}/src/narrative/LabelStyle.cpp
  ${CMAKE_SOURCE_DIR}/src/SimpleCommandStack.cpp
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


