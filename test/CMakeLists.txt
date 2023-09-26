#include_directories(SYSTEM "${Catch2_INCLUDE_DIR}")

find_file(CATCH_INDLUCE_FILE Catch.cmake REQUIRED PATHS ${Catch2_SOURCE_DIR}/contrib)
include("${CATCH_INDLUCE_FILE}")

add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PRIVATE project_options 
                                 PUBLIC Catch2::Catch2)

add_executable(tests libBtfTest.cpp)
target_link_libraries(tests PRIVATE project_warnings project_options catch_main helper btf)
if(NOT WIN32)
  target_link_libraries(tests PRIVATE stdc++fs)
endif()
target_include_directories(tests PUBLIC "${PROJECT_SOURCE_DIR}/libHelper" "${PROJECT_SOURCE_DIR}")
target_compile_definitions(tests PRIVATE PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
catch_discover_tests(
  tests
  TEST_PREFIX
  "unittests."
  REPORTER
  xml
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)

