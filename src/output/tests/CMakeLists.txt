set(TEST_PGRM output_test)
add_executable(${TEST_PGRM} output_test.cpp)
target_link_libraries(${TEST_PGRM} GTest::gtest_main ${LIBRARY_NAME})

if(ELA_USE_MPI)
    set(TEST_Name Output.MPI)
    option(MPIRUN_OVERSUBSCRIBE off)
    if(MPIRUN_OVERSUBSCRIBE)
        set(MPI_COMMAND mpirun -np 4 --oversubscribe ./${TEST_PGRM})
    else()
        set(MPI_COMMAND mpirun -np 4 ./${TEST_PGRM})   
    endif()

    add_test(NAME ${TEST_Name} COMMAND ${MPI_COMMAND})

    # mpi is not leak free, turn off leak detection
    set_tests_properties(${TEST_Name} PROPERTIES ENVIRONMENT
    ASAN_OPTIONS=detect_leaks=0
    )
else()
    gtest_discover_tests(${TEST_PGRM})
endif(ELA_USE_MPI)
