vsim_add_flatbuffer(model_h model.fbs)

set(Model_h
  Model.h
  ModelEditDialog.h
  ModelGroup.h
  ModelGroupModel.h
  ModelOutliner.h
  ModelSerializer.h
  ModelUtil.h
  OSGGroupModel.h
  OSGNodeWrapper.h
  OSGYearModel.h
  ${model_h}
)

set(Model_src
  Model.cpp
  ModelEditDialog.cpp
  ModelEditDialog.ui
  ModelGroup.cpp
  ModelGroupModel.cpp
  ModelOutliner.cpp
  ModelSerializer.cpp
  ModelUtil.cpp
  OSGGroupModel.cpp
  OSGNodeWrapper.cpp
  OSGYearModel.cpp
)

add_library(Model_obj ${Model_src} ${Model_h})
target_link_libraries(Model_obj Core_obj Gui_obj Qt5::Widgets ${OPENSCENEGRAPH_LIBRARIES})

vsim_add_experiment(
  Model_experiment
  Model_experiment.cpp
  Model_obj
)
