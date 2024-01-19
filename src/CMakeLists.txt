file(GLOB hydrogen
    "${CMAKE_CURRENT_SOURCE_DIR}/basic/*"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/diagnostic/*"
   "${CMAKE_CURRENT_SOURCE_DIR}/ast/*.h"
   "${CMAKE_CURRENT_SOURCE_DIR}/ast/*.cpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/lexer/*"

#    "${CMAKE_CURRENT_SOURCE_DIR}/resolve/*.h"
#    "${CMAKE_CURRENT_SOURCE_DIR}/resolve/*.cpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/ast/*.h"
#    "${CMAKE_CURRENT_SOURCE_DIR}/ast/*.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/semantics/*.h"
   "${CMAKE_CURRENT_SOURCE_DIR}/semantics/*.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/parser/*.h"
   "${CMAKE_CURRENT_SOURCE_DIR}/parser/*.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/tools/*.h"
   "${CMAKE_CURRENT_SOURCE_DIR}/tools/*.cpp"
   

)
message(STATUS "hydrogen: ${hydrogen}")
add_library(hydrogen ${hydrogen})

