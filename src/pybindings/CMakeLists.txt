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

if(ENABLE_PYTHON_BINDINGS)

  set(TARGET pybtf)

  pybind11_add_module(${TARGET} ${CMAKE_CURRENT_LIST_DIR}/src/libbtf_binding.cpp
                                ${CMAKE_CURRENT_LIST_DIR}/src/libhelper_binding.cpp)
                                
  target_link_libraries(${TARGET} PUBLIC helper btf
                                  PRIVATE project_options project_warnings)
  
  target_include_directories(${TARGET} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
  set_property(TARGET ${TARGET} PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)
  set_property(TARGET ${TARGET} PROPERTY CXX_INCLUDE_WHAT_YOU_USE "")
endif()




