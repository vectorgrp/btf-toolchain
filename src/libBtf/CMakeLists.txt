set(TARGET btf)

add_library(${TARGET} STATIC  ${CMAKE_CURRENT_LIST_DIR}/src/btf_entity_types.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/btf_signal.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/btf.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/common.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/core.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/os.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/runnable.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/scheduler.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/semaphore.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/simulation.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/stimulus.cpp
                              ${CMAKE_CURRENT_LIST_DIR}/src/process.cpp)

target_link_libraries(${TARGET} PUBLIC helper 
                                PRIVATE project_options project_warnings)
target_include_directories(${TARGET} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(${TARGET} SYSTEM PUBLIC "${pybind11_INCLUDES}")
set_property(TARGET ${TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)