

file(GLOB lexer_tests

    "${CMAKE_CURRENT_SOURCE_DIR}/*cpp"

)
foreach(test ${lexer_tests})
    get_filename_component(test_name ${test} NAME_WE)
    add_executable(${test_name} ${test})
    add_test(${test_name} ${test_name})
    target_link_libraries(${test_name} hydrogen)
    
endforeach()

