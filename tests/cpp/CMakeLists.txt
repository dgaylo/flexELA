if(ELA_USE_MPI)
    add_compile_options(${MPI_CXX_COMPILE_OPTIONS})
    link_libraries(${MPI_CXX_LINK_FLAGS})

    # TODO: mpi tests

else()
    set(TEST_PGRM init_test)
    add_executable(${TEST_PGRM} init_test.cpp)
    target_link_libraries(${TEST_PGRM} GTest::gtest_main GTest::gtest flexELA)
    gtest_discover_tests(${TEST_PGRM})


    set(TEST_PGRM twodimvortex_test)
    add_executable(${TEST_PGRM} twodimvortex_test.cpp)
    target_link_libraries(${TEST_PGRM} GTest::gtest_main GTest::gtest flexELA)
    gtest_discover_tests(${TEST_PGRM})


    set(TEST_PGRM solver_test)
    add_executable(${TEST_PGRM} solver_test.cpp)
    target_link_libraries(${TEST_PGRM} GTest::gtest_main GTest::gtest flexELA)
    gtest_discover_tests(${TEST_PGRM})
endif()
