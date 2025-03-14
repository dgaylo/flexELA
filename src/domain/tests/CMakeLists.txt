set(TEST_PGRM fields_test)
add_executable(${TEST_PGRM} fields_test.cpp)
target_link_libraries(${TEST_PGRM} GTest::gtest_main)
gtest_discover_tests(${TEST_PGRM})

set(TEST_PGRM domain_test)
add_executable(${TEST_PGRM} domain_test.cpp compression_test.cpp)
target_link_libraries(${TEST_PGRM} GTest::gtest_main ${PROJECT_NAME})
gtest_discover_tests(${TEST_PGRM})


if(ELA_USE_MPI)
    set(TEST_PGRM mpidomain_test)
    set(TEST_Name DomainTests.MPI)

    add_executable(${TEST_PGRM} mpi_test.cpp)
    target_link_libraries(${TEST_PGRM} GTest::gtest_main ${PROJECT_NAME})


    option(MPIRUN_OVERSUBSCRIBE off)
    if(MPIRUN_OVERSUBSCRIBE)
        set(MPI_COMMAND 
            ${MPIEXEC_EXECUTABLE}
            ${MPIEXEC_NUMPROC_FLAG} 4 --oversubscribe
            ./${TEST_PGRM}
        )
    else()
        set(MPI_COMMAND 
            ${MPIEXEC_EXECUTABLE}
            ${MPIEXEC_NUMPROC_FLAG} 4
            ./${TEST_PGRM}
        ) 
    endif()

    add_test(NAME ${TEST_Name} COMMAND ${MPI_COMMAND})

    # mpi is not leak free, turn off leak detection
    set_tests_properties(${TEST_Name} PROPERTIES ENVIRONMENT
        ASAN_OPTIONS=detect_leaks=0
    )
endif(ELA_USE_MPI)
