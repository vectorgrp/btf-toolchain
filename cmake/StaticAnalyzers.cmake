#
# Copyright (c) 2023 Vector Informatik GmbH
# 
# SPDX-License-Identifier: MIT
#

if(ENABLE_CPPCHECK)
find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
if (CMAKE_CXX_CPPCHECK)
    list(
        APPEND CMAKE_CXX_CPPCHECK 
           ${CMAKE_SOURCE_DIR}/src
           --suppress=unknownMacro
           --enable=warning
           --inline-suppr
           --inconclusive
    )
    if (WIN32)
        add_custom_target(ANALYZE_CPPCHECK ALL
            COMMAND ${CMAKE_CXX_CPPCHECK}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Static code analysis using ${CPP_CHECK_VERSION}"
        )
    endif()
  else()
    message(SEND_ERROR "cppcheck requested but executable not found")
  endif()
endif()

if(ENABLE_CLANG_TIDY)
  find_program(CLANGTIDY clang-tidy)
  if(CLANGTIDY)
    set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY} -extra-arg=-Wno-unknown-warning-option -checks=-cppcoreguidelines-pro-type-union-access,-readability-function-cognitive-complexity)
  else()
    message(SEND_ERROR "clang-tidy requested but executable not found")
  endif()
endif()

if(ENABLE_INCLUDE_WHAT_YOU_USE)
  find_program(INCLUDE_WHAT_YOU_USE include-what-you-use)
  if(INCLUDE_WHAT_YOU_USE)
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE})
  else()
    message(SEND_ERROR "include-what-you-use requested but executable not found")
  endif()
endif()
