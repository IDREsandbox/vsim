
set(FutureDialog_src
  ${CMAKE_SOURCE_DIR}/src/FutureDialog.cpp
  FutureDialog_experiment.cpp
  Multithread.cpp
)
add_executable(FutureDialog_experiment
  EXCLUDE_FROM_ALL
  ${FutureDialog_src})
target_link_libraries(FutureDialog_experiment Qt5::Widgets Qt5::Concurrent)
