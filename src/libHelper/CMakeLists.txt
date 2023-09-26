set(TARGET helper)

add_library(${TARGET} STATIC  ${CMAKE_CURRENT_LIST_DIR}/src/logging.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/util.cpp)

target_link_libraries(${TARGET} PRIVATE project_options project_warnings)
target_include_directories(${TARGET} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(${TARGET} SYSTEM PUBLIC "${pybind11_INCLUDES}")
set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)