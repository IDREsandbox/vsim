# Testing
Here's the workflow

## Making and running one test

1. Create SomeClass_test.cpp in vsim/tests. Just copy paste an existing test for the Qt stuff.
2. Assuming you already have a build directory: `cmake --build . --target SomeClass_test`
3. Open up a console in build/Debug and run ./SomeClass_test

You might need to run `cmake --build . --target VSim` to copy dll's over for the first time.

## Running all of the tests

1. `cmake --build . --config RELEASE --target RUN_TESTS`
2. Output can be found in Testing/Temporary/LastTest.log

