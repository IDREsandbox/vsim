# independent, reusable gui elements

set(Gui_h
  EditDeleteDelegate.cpp
  ElidedLabel.h
  FutureDialog.h
  NumberSlider.h
  PasswordDialog.h
  SimpleWorker.h
  TickSlider.h
  TitledComboBox.h
)

set(Gui_src
  EditDeleteDelegate.cpp
  ElidedLabel.cpp
  FutureDialog.cpp
  NumberSlider.cpp
  PasswordDialog.cpp
  SimpleWorker.cpp
  TickSlider.cpp
  TitledComboBox.cpp
)

set(Gui_ui
  FutureDialog.ui
  NumberSlider.ui
  PasswordDialog.ui
)

add_library(Gui_obj ${Gui_src} ${Gui_h} ${Gui_ui})
target_link_libraries(Gui_obj Core_obj Qt5::Widgets Qt5::Concurrent)
vsim_include_uic(Gui_obj)
