set(LIBRARY_NAME output)
add_library(${LIBRARY_NAME} STATIC)

set(HDRS
    output.h
    vv.h
)

set(SRCS
    vv.cpp
)


target_sources(${LIBRARY_NAME}
    PRIVATE ${SRCS}
            ${HDRS}
)

target_link_libraries(${LIBRARY_NAME} PUBLIC svector)
if(ELA_USE_MPI)
target_link_libraries(${LIBRARY_NAME} PUBLIC MPI::MPI_CXX)
endif(ELA_USE_MPI)

if(BUILD_TESTING)
add_subdirectory(tests)
endif(BUILD_TESTING)
