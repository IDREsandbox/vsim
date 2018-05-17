
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_BINARY_DIR})

# message("binary dir: " ${CMAKE_BINARY_DIR})
# message("c bin dir: " ${CMAKE_CURRENT_BINARY_DIR})

set(Canvas_src
  ${CMAKE_SOURCE_DIR}/src/narrative/CanvasControl.cpp
  ${CMAKE_SOURCE_DIR}/src/narrative/CanvasContainer.cpp
  ${CMAKE_SOURCE_DIR}/src/narrative/CanvasScene.cpp
  Canvas_experiment.cpp
)
add_executable(Canvas_experiment
  EXCLUDE_FROM_ALL
  ${Canvas_src})
target_link_libraries(Canvas_experiment Qt5::Widgets Qt5::Test)
set_target_properties(Canvas_experiment PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")


