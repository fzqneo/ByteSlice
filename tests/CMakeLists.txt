include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

list(APPEND test_list
        avx-utility_test
        bitvector_block_test
        bitvector_iterator_test
        bitvector_test
        byteslice_column_block_test
        column_test
    )

# find_program(MEMCHECK_CMD valgrind )

foreach(tt ${test_list})
    add_executable(${tt}  "${tt}.cpp")
    target_link_libraries(${tt} byteslice-core gtest gtest_main)
#    add_test(NAME ${tt} COMMAND ${MEMCHECK_CMD} --tool=memcheck --leak-check=yes ./${tt} --gtest_color=yes)
    add_test(NAME ${tt} COMMAND ${tt} --gtest_color=yes)
    add_dependencies(check-build ${tt})
endforeach()



