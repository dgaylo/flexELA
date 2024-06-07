if(ELA_USE_MPI)
    add_compile_options(${MPI_Fortran_COMPILE_OPTIONS})
    link_libraries(${MPI_Fortran_LINK_FLAGS})
endif()

set_source_files_properties(
    init_test.f90
    PROPERTIES Fortran_PREPROCESS ON
)


set(TEST_PGRM init_test_f)
add_executable(${TEST_PGRM} init_test.f90)
target_link_libraries(${TEST_PGRM} flexELA)

if(ELA_USE_MPI)
    target_link_libraries(${TEST_PGRM} MPI::MPI_Fortran)

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

    add_test(NAME ELA_f.Init COMMAND ${MPI_COMMAND})

    # mpi is not leak free, turn off leak detection
    set_tests_properties(ELA_f.Init PROPERTIES ENVIRONMENT
        ASAN_OPTIONS=detect_leaks=0
    )
else()
    add_test(ELA_f.Init ${TEST_PGRM})
endif()