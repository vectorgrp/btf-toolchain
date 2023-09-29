#
# Copyright (c) 2023 Vector Informatik GmbH
# 
# SPDX-License-Identifier: MIT
#

function(enable_doxygen)
  
  if(ENABLE_DOXYGEN)
    set(DOXYGEN_CALLER_GRAPH YES)
    set(DOXYGEN_CALL_GRAPH YES)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_DISABLE_INDEX YES)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_FULL_SIDEBAR YES)
    set(DOXYGEN_HTML_DYNAMIC_SECTIONS NO)
    set(DOXYGEN_HAVE_DOT YES)
    set(DOXYGEN_INTERACTIVE_SVG YES)
    set(DOXYGEN_DOT_IMAGE_FORMAT svg)
    set(DOXYGEN_EXCLUDE "${PROJECT_SOURCE_DIR}/build")
    set(DOXYGEN_HTML_OUTPUT "${PROJECT_SOURCE_DIR}/doc/doxygen_html")
    
    find_package(Doxygen REQUIRED dot)
    doxygen_add_docs(doxygen-docs 
                    "${PROJECT_SOURCE_DIR}/src"
                    "${PROJECT_SOURCE_DIR}/doc/md"
                    ALL
                    )
  endif()
endfunction()
