file(GLOB testingtools
    "${CMAKE_CURRENT_SOURCE_DIR}/*"
    

)
add_library(testingtools ${testingtools})
link_libraries(testingtools)
