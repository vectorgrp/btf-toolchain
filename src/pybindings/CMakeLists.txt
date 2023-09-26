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




