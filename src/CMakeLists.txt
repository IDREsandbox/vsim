
add_subdirectory(Core)
add_subdirectory(Gui)
add_subdirectory(Canvas)
add_subdirectory(Model)
add_subdirectory(Switch)

file(GLOB_RECURSE Deprecated_src
  deprecated/*.h
  deprecated/*.cpp
  )
add_library(Deprecated_obj ${Deprecated_src})

vsim_add_flatbuffer(erfb_h eresources.fbs)
set(ERModel_src
  ERSerializer.cpp
  ERSerializer.h
  ${erfb_h}
  resources/ERFilterSortProxy.h
  resources/ERFilterSortProxy.cpp
  resources/ECategory.h
  resources/ECategory.cpp
  resources/ECategoryGroup.h
  resources/ECategoryGroup.cpp
  resources/ECategoryModel.h
  resources/ECategoryModel.cpp
  resources/EResource.h
  resources/EResource.cpp
  resources/EResourceGroup.h
  resources/EResourceGroup.cpp
  CheckableListProxy.h
  CheckableListProxy.cpp
  resources/EREnums.h
)
add_library(ERModel_obj ${ERModel_src})
target_link_libraries(ERModel_obj Core_obj Deprecated_obj)

vsim_add_flatbuffer(narfb_h narratives.fbs)
set(NarrativeModel_src
  NarrativeSerializer.cpp
  NarrativeSerializer.h
  ${narfb_h}
  narrative/NarrativeGroup.cpp
  narrative/NarrativeGroup.h
  narrative/Narrative.cpp
  narrative/Narrative.h
  narrative/NarrativeSlide.cpp
  narrative/NarrativeSlide.h
)
add_library(NarrativeModel_obj ${NarrativeModel_src})
target_link_libraries(NarrativeModel_obj Core_obj Deprecated_obj Canvas_obj)

vsim_add_flatbuffer(vsim_fb_h vsim.fbs)
vsim_add_flatbuffer(settings_fb_h settings.fbs)
set(VSimRoot_src
  VSimRoot.cpp
  VSimRoot.h
  VSimSerializer.cpp
  VSimSerializer.h
  ${vsim_fb_h}
  ${settings_fb_h}
)
add_library(VSimRoot_obj ${VSimRoot_src})
target_link_libraries(VSimRoot_obj Core_obj Deprecated_obj NarrativeModel_obj ERModel_obj Model_obj)

set(VSim_src
  resources/ECategoryControl.cpp
  resources/ECategoryControl.h
  resources/ERControl.cpp
  resources/ERControl.h
  resources/ERDialog.cpp
  resources/ERDialog.h
  resources/ERDialog.ui
  resources/ERDisplay.cpp
  resources/ERDisplay.h
  resources/ERDisplay.ui
  resources/ERFilterArea.ui
  resources/ERFilterArea.cpp
  resources/ERFilterArea.h
  resources/ERScrollBox.cpp
  resources/ERScrollBox.h
  resources/ERScrollItem.cpp
  resources/ERScrollItem.h
  resources/NewCatDialog.cpp
  resources/NewCatDialog.h

  narrative/NarrativeControl.h
  narrative/NarrativeControl.cpp
  narrative/NarrativeInfoDialog.h
  narrative/NarrativeInfoDialog.cpp
  narrative/NarrativePlayer.cpp
  narrative/NarrativePlayer.h
  narrative/NarrativeScrollBox.cpp
  narrative/NarrativeScrollBox.h
  narrative/NarrativeScrollItem.cpp
  narrative/NarrativeScrollItem.h
  narrative/NarrativeScrollItem.ui
  narrative/NarrativeSlideDurationDialog.cpp
  narrative/NarrativeSlideDurationDialog.h
  narrative/SlideScrollBox.cpp
  narrative/SlideScrollBox.h
  narrative/SlideScrollItem.cpp
  narrative/SlideScrollItem.h
  narrative/SlideScrollItem.ui
  # narrative/StyleSettingsDialog.cpp
  # narrative/StyleSettingsDialog.h
  # narrative/StyleSettingsDialog.ui
  # narrative/StyleSettingsWidget.cpp
  # narrative/StyleSettingsWidget.h
  # narrative/StyleSettingsWidget.ui

  AboutDialog.cpp
  AboutDialog.h
  AboutDialog.ui
  BaseFirstPersonManipulator.cpp
  BaseFirstPersonManipulator.h
  BrandingOverlay.cpp
  BrandingOverlay.h
  BrandingControl.cpp
  BrandingControl.h
  CoordinateWidget.cpp
  CoordinateWidget.h
  DisplaySettingsDialog.cpp
  DisplaySettingsDialog.h
  DisplaySettingsDialog.ui
  ERBar.cpp
  ERBar.h
  ERBar.ui
  FastScrollBox.cpp
  FastScrollBox.h
  FirstPersonManipulator.cpp
  FirstPersonManipulator.h
  FlightManipulator.cpp
  FlightManipulator.h
  FocusFilter.cpp
  FocusFilter.h
  GroupScrollBox.inl
  GroupScrollBox.h
  HistoryWindow.cpp
  HistoryWindow.h
  HorizontalScrollBox.cpp
  HorizontalScrollBox.h
  KeyTracker.cpp
  KeyTracker.h
  LockDialog.cpp
  LockDialog.h
  LockDialog.ui
  MainWindow.cpp
  MainWindow.h
  MainWindow.ui
  MainWindowTopBar.cpp
  MainWindowTopBar.h
  ModelInformationDialog.cpp
  ModelInformationDialog.h
  ModelInformationDialog.ui
  mrichtextedit.cpp
  mrichtextedit.h
  mtextedit.cpp
  mtextedit.h
  Navigation.h
  NavigationControl.cpp
  NavigationControl.h
  NavigationSettingsDialog.cpp
  NavigationSettingsDialog.h
  NavigationSettingsDialog.ui
  ObjectManipulator.cpp
  ObjectManipulator.h
  OSGViewerWidget.cpp
  OSGViewerWidget.h
  ScrollBoxItem.cpp
  ScrollBoxItem.h
  Selection.cpp
  Selection.h
  SelectionStack.cpp
  SelectionStack.h
  SimpleCameraManipulator.cpp
  SimpleCameraManipulator.h
  StackObject.inl
  StackObject.h
  StatsWindow.cpp
  StatsWindow.h
  StatsWindow.ui
  TimeManager.cpp
  TimeManager.h
  TimeSlider.cpp
  TimeSlider.h
  VSimApp.cpp
  VSimApp.h
)
add_library(VSim_obj ${VSim_src})
target_link_libraries(VSim_obj VSimRoot_obj Switch_obj Gui_obj Model_obj cryptopp-static)

file(GLOB_RECURSE oldSerializers
  */Serializer*.cpp
  )

add_executable(VSim main.cpp ${oldSerializers})
# set_target_properties(VSim PROPERTIES
#   VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
target_link_libraries(VSim VSim_obj
  Qt5::Widgets Qt5::Core Qt5::Gui
  ${OPENSCENEGRAPH_LIBRARIES})

if (WIN32)
  add_dependencies(VSim CopyAssets)
  add_dependencies(VSim CopyOSGPlugins)
  add_dependencies(VSim CopyBuildDLLs)
endif()

install(TARGETS VSim DESTINATION .)


# horizontal scroll box experiment

set(HorizontalScrollBox_src
  HorizontalScrollBox_experiment.cpp
  HorizontalScrollBox.h
  HorizontalScrollBox.cpp
  ScrollBoxItem.h
  ScrollBoxItem.cpp
  Selection.h
  Selection.cpp
  SelectionStack.h
  SelectionStack.cpp
)
vsim_add_experiment(
  HorizontalScrollBox_experiment
  "${HorizontalScrollBox_src}"
  "Core_obj;Qt5::Widgets"
)

# ER serializer test
vsim_add_test(
  ERSerializer_test
  ERSerializer_test.cpp
  "ERModel_obj;Core_obj;Deprecated_obj;Qt5::Test"
)

