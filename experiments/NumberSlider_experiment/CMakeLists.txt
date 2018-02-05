# message("adding numberslider")
# #find_package(Qt5Widgets)

# # Find includes in corresponding build directories
# set(CMAKE_INCLUDE_CURRENT_DIR ON)
# # Instruct CMake to run moc automatically when needed.
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTORCC ON)

# include_directories(${PROJECT_SOURCE_DIR}/src)

# set(NumberSlider_src
#   src/NumberSlider.h
#   src/NumberSlider.cpp
#   experiments/NumberSlider_experiment/NumberSlider_experiment.cpp
# )
# message("exp sources ----:\n""${NumberSlider_src}")
# add_executable(NumberSlider_experiment ${NumberSlider_src})
# add_dependencies(NumberSlider_experiment CopyDLLs)
# target_link_libraries(NumberSlider_experiment Qt5::Widgets)