add_library(blah "")

target_sources(blah PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/blah.c 
    ${CMAKE_CURRENT_LIST_DIR}/blah.h)
