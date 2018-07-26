# independent, reusable gui elements

set(Gui_h
  ElidedLabel.h
  NumberSlider.h
  TickSlider.h
  TitledComboBox.h
)

set(Gui_src
  ElidedLabel.cpp
  NumberSlider.cpp
  NumberSlider.ui
  TickSlider.cpp
  TitledComboBox.cpp
)

add_library(Gui_obj ${Gui_src} ${Gui_h})
target_link_libraries(Gui_obj Core_obj Qt5::Widgets)
