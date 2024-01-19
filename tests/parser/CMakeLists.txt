

file(GLOB parser_test

    "${CMAKE_CURRENT_SOURCE_DIR}/*cpp"

)
foreach(test ${parser_test})
    get_filename_component(test_name ${test} NAME_WE)
    add_executable(${test_name} ${test})
    add_test(${test_name} ${test_name})
    target_link_libraries(${test_name} hydrogen)
    
endforeach()

