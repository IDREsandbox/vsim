
set(Canvas_src
  ${CMAKE_SOURCE_DIR}/src/narrative/CanvasContainer.h
  ${CMAKE_SOURCE_DIR}/src/narrative/CanvasContainer.cpp
  Canvas_experiment.cpp
)
add_executable(Canvas_experiment
  EXCLUDE_FROM_ALL
  ${Canvas_src})
target_link_libraries(Canvas_experiment Qt5::Widgets Qt5::Test ${OPENSCENEGRAPH_LIBRARIES})

