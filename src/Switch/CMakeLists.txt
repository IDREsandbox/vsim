
set(Switch_h
  SwitchManager.h
  SwitchModel.h
  SwitchWindow.h
)

set(Switch_src
  SwitchManager.cpp
  SwitchModel.cpp
  SwitchWindow.cpp
)

add_library(Switch_obj ${Switch_src} ${Switch_h})
target_link_libraries(Switch_obj Core_obj Qt5::Widgets ${OPENSCENEGRAPH_LIBRARIES})

vsim_add_experiment(
  Switch_experiment
  Switch_experiment.cpp
  Switch_obj
)
vsim_add_test(
  Switch_test
  Switch_test.cpp
  "Switch_obj;Qt5::Test"
)
