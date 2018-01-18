# Testing
Here's the workflow

## Making and running one test

1. Create SomeClass_test.cpp in vsim/tests. Just copy paste an existing test for the Qt stuff.
2. Assuming you already have a build directory: `cmake --build . --target SomeClass_test`
3. Open up a console in build/Debug and run ./SomeClass_test

You need to run `cmake --build . --target VSim` to setup dll copying for the first time.

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