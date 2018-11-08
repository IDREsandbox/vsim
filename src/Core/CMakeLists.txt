
vsim_add_flatbuffer(types_h types.fbs)

set(Core_h
  Command.h
  GroupCommands.h
  GroupEnums.h
  GroupModelTemplate.inl
  GroupModelTemplate.h
  GroupSignals.h
  GroupTemplate.inl
  GroupTemplate.h
  HashLock.h
  ICommandStack.h
  LockTable.h
  OSGObserver.h
  SimpleCommandStack.h
  TypesSerializer.h
  WeakObject.h
  VecUtil.h
  VSimInfo.h
  Util.h
  ${types_h}
)

set(Core_src
  GroupSignals.cpp
  HashLock.cpp
  LockTable.cpp
  OSGObserver.cpp
  SimpleCommandStack.cpp
  TypesSerializer.cpp
  Util.cpp
  VecUtil.cpp
  VSimInfo.cpp
  WeakObject.cpp
)

add_library(Core_obj ${Core_src} ${Core_h})
target_link_libraries(Core_obj Qt5::Widgets ${OPENSCENEGRAPH_LIBRARIES} ${CRYPTOPP_LIBRARIES})

vsim_add_test(
  Group_test
  Group_test.cpp
  "Core_obj;Qt5::Widgets;Qt5::Test;${OPENSCENEGRAPH_LIBRARIES}"
)

vsim_add_test(
  OSGObserver_test
  OSGObserver_test.cpp
  "Core_obj;Qt5::Test;${OPENSCENEGRAPH_LIBRARIES}"
)

vsim_add_test(
  Util_test
  Util_test.cpp
  "Core_obj;Qt5::Test;${OPENSCENEGRAPH_LIBRARIES}"
)

vsim_add_test(
  VecUtil_test
  VecUtil_test.cpp
  "Core_obj;Qt5::Test;${OPENSCENEGRAPH_LIBRARIES}"
)

vsim_add_test(
  HashLock_test
  "HashLock.h;HashLock.cpp;HashLock_test.cpp"
  "cryptopp-static;Qt5::Test"
)

vsim_add_test(
  LockTable_test
  "LockTable_test.cpp"
  "Core_obj;cryptopp-static;Qt5::Test"
)

# is this needed?
# target_include_directories(Core_obj
#   PUBLIC ${CMAKE_SOURCE_DIR})
