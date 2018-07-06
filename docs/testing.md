# Testing
Here's the workflow

## Making and running one test

1. Create SomeClass_test.cpp in vsim/tests or in src. Just copy paste an existing test for the Qt boilerplate.

2. Add this to a CMakeLists.txt

```
vsim_add_test(
  ${test_name}
  ${test_src}
  ${test_libs}
)
vsim_add_test(
  CanvasSerializer_test
  CanvasSerializer_test.cpp
  "Canvas_obj;Qt5::Test"
)
```

I use "experiments" to test out the gui. This lets you work on code in isolation -> faster builds, better focus, etc.

```
vsim_add_experiment(
  CanvasEditor_experiment
  CanvasEditor_experiment.cpp
  Canvas_obj
)
```

2. Assuming you already have a build directory: `cmake --build . --target SomeClass_test`
3. Open up a console in build/Debug and run ./SomeClass_test.exe. You can also just open up the vsim solution. You might have to run CopyBuildDLLs, CopyAssets, CopyOSGPlugins if you get dll errors.

## Running all of the tests

1. `cmake --build . --config RELEASE --target RUN_TESTS`
2. Output can be found in Testing/Temporary/LastTest.log

## Other CMake commands

cmake --build . --config RELEASE --target RUN_TESTS
ctest . -c Release
ctest --verbose
cmake -E env CTEST_OUTPUT_ON_FAILURE=1 cmake --build . --target RUN_TESTS

cmake -E env CTEST_OUTPUT_ON_FAILURE=1 cmake --build . --target RUN_TESTS

cmake -E env CTEST_OUTPUT_ON_FAILURE=1 cmake --build . --config RELEASE --target RUN_TESTS

ctest -C RELEASE -O Output.log --output-on-failure -R Util