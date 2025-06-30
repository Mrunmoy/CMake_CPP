
# CMake C++ Project template

![Build Status](https://github.com/Mrunmoy/CMake_CPP/workflows/BuildAndTest/badge.svg?branch=main)
![Coverage](https://raw.githubusercontent.com/Mrunmoy/CMake_CPP/main/coverage.svg)


This repository is CMake project structure originally by [Darius Sabaliauskas](https://github.com/Jamagas) [here](https://github.com/Jamagas/CMake). I had to make a copy of the original repo as I did not have access to be able to set github bot to push an image to the repo for code coverage. I refactored it a bit to 
- added an AppConfig.hpp header if you need to configure some defines for your application
- use a build script to build, clean and run tests project (This script can generate projects for Xcode/Visual Studio).
- added generation of code coverage reports.
- use docker build if you need to run the build in a container.
- removed gtest and use Fetchcontent to get it
- added build badge

## Table of contents
- [Project structure](#project-structure)
- [Required packages](#required-packages)
- [Building Project](#building-project)
- [Copyright and license](#copyright-and-license)

## Project structure
In this example project structure there is one master project with two static libraries.
Each library has it's own tests.


```
.
├── AppConfig.hpp.in
├── CMakeLists.txt
├── Dockerfile
├── LICENSE.txt
├── README.md
├── build.sh
├── docker-compose.yml
├── entrypoint.sh
├── libs
│   ├── Calculator
│   │   ├── CMakeLists.txt
│   │   ├── include
│   │   │   └── Calculator.hpp
│   │   ├── src
│   │   │   └── Calculator.cpp
│   │   └── tests
│   │       ├── CMakeLists.txt
│   │       ├── main.cpp
│   │       └── tests.cpp
│   └── RingBuffer
│       ├── CMakeLists.txt
│       ├── include
│       │   └── RingBuffer.hpp
│       ├── src
│       │   └── RingBuffer.cpp
│       └── tests
│           ├── CMakeLists.txt
│           ├── main.cpp
│           └── tests.cpp
└── src
    └── main.cpp
```

## Required packages

To build and generate coverage reports locally you need the following tools:

- `lcov` and `genhtml` - for capturing and generating HTML coverage reports
- `gcovr` - for summarizing coverage in CI pipelines

## Building Project
1. Running ```./build.sh --all``` will generate Xcode project when run on Mac, Visual Studio 2022 when run on Windows.
2. Running ```./build.sh --docker --all``` will build and run the tests in a docker container.

**NOTE**: 
DO NOT make changes to generated project file, instead edit CMakeLists.txt files and regenerate project using CMake.
For example, when you want to add files to project you update CMakeLists.txt to include those files and regenerate build system using CMake.

For the Xcode Project (specially building and running the tests), if you do not add `DISCOVERY_MODE PRE_TEST` to `gtest_discover_tests` like this:
```
gtest_discover_tests(${PROJECT_NAME}
	DISCOVERY_MODE PRE_TEST
)
```
you might get an error like this for example:
```
PhaseScriptExecution CMake\ PostBuild\ Rules /Users/blah/CMake_CPP/buildxc/build/RingBufferTest.build/Debug/Script-5E960E25A8C4527A9C67F81F.sh (in target 'RingBufferTest' from project 'MyApp')
    cd /Users/blah/CMake_CPP
    /bin/sh -c /Users/blah/CMake_CPP/buildxc/build/RingBufferTest.build/Debug/Script-5E960E25A8C4527A9C67F81F.sh
CMake Error at /Applications/CMake.app/Contents/share/cmake-3.26/Modules/GoogleTestAddTests.cmake:112 (message):
  Error running test executable.

    Path: '/Users/blah/CMake_CPP/buildxc/libs/RingBuffer/tests/Debug/RingBufferTest'
    Result: Subprocess killed
    Output:
```

The other option is to open the xcode project and then delete the POST BUILD step for the test project

## Copyright and license

Code and documentation copyright 2015 Darius Sabaliauskas and released under 
[the MIT license](https://github.com/Jamagas/CMake/blob/master/LICENSE).
 
