# Copyright (c) 2023 Vector Informatik GmbH
# 
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION 3.10)


project(btftoolchain CXX)

# Options -------------------------------------------------------------------------------------------------

option(ENABLE_PYTHON_BINDINGS "Generate python binding, requires pybind11." OFF)
option(ENABLE_CACHE "Enable cache if available" OFF)
option(ENABLE_DOXYGEN "Enable doxygen doc builds of source" OFF)
option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
option(ENABLE_IPO "Enable Interprocedural Optimization, aka Link Time Optimization (LTO)" OFF)
option(ENABLE_INCLUDE_WHAT_YOU_USE "Enable static analysis with include-what-you-use" OFF)
option(ENABLE_TESTING "Enable Test Builds, requires catch2" ON)
option(ENABLE_FUZZING "Enable Fuzzing Builds" OFF)

# Includes ------------------------------------------------------------------------------------------------

# Standard Project Settings
include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
  if(ENABLE_BUILD_WITH_TIME_TRACE)
    target_compile_options(project_options INTERFACE -ftime-trace)
  endif()
endif()

include(GNUInstallDirs)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# enable cache system
include(cmake/Cache.cmake)
cache_enable()

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
include(cmake/Doxygen.cmake)
enable_doxygen()

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

# enable testing. Requires catch2
if(ENABLE_TESTING)
CPMAddPackage("gh:catchorg/Catch2@2.13.3")
enable_testing()
message("Building Tests.")
add_subdirectory(test)
endif()

# enable python bindings. Requires pybind11, pybin11_stubgen and wheel
if(ENABLE_PYTHON_BINDINGS)
CPMAddPackage(NAME pybind11
			  GIT_REPOSITORY https://github.com/pybind/pybind11
			  GIT_TAG v2.6.1)
endif()


if(ENABLE_FUZZING)
  message("Building Fuzz Tests, using fuzzing sanitizer https://www.llvm.org/docs/LibFuzzer.html")
  add_subdirectory(fuzz_test)
endif()

add_subdirectory(src)

# DEB packet generation (only for Linux)
if(NOT WIN32)
	find_package (Git)
	execute_process(COMMAND ${GIT_EXECUTABLE} for-each-ref --sort=creatordate  RESULT_VARIABLE GIT_TAG_RESULT OUTPUT_VARIABLE GIT_TAG_OUTPUT)
	string(REGEX MATCHALL "[0-9]+\\.[0-9]+\\.[0-9]+" GIT_TAG_LIST "${GIT_TAG_OUTPUT}")
	list(POP_BACK GIT_TAG_LIST DEB_VERSION)
	message("Creating deb package with version " ${DEB_VERSION} )
	SET(CPACK_GENERATOR "DEB")
	SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "SE")
	set(CPACK_PACKAGE_VERSION ${DEB_VERSION})
	INCLUDE(CPack)
endif()
