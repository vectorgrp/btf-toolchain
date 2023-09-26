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

function(cache_enable)

  if(NOT ENABLE_CACHE)
    return()
  endif()

  set(CACHE_OPTION
      "ccache"
      CACHE STRING "Compiler cache to be used")
  set(CACHE_OPTION_VALUES "ccache" "sccache")
  set_property(CACHE CACHE_OPTION PROPERTY STRINGS ${CACHE_OPTION_VALUES})
  list(
    FIND
    CACHE_OPTION_VALUES
    ${CACHE_OPTION}
    CACHE_OPTION_INDEX)

  if(${CACHE_OPTION_INDEX} EQUAL -1)
    message(
      STATUS
        "Using custom compiler cache system: '${CACHE_OPTION}', explicitly supported entries are ${CACHE_OPTION_VALUES}")
  endif()

  find_program(CACHE_BINARY ${CACHE_OPTION})
  if(CACHE_BINARY)
    message(STATUS "${CACHE_OPTION} found and enabled")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CACHE_BINARY})
  else()
    message(WARNING "${CACHE_OPTION} is enabled but was not found. Not using it")
  endif()
endfunction()
