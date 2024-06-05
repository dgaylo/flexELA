set(LIBRARY_NAME domain)
add_library(${LIBRARY_NAME} STATIC)

set(HDRS
    domain.h
    compression.h
    fields.h
)

set(SRCS
    domain.cpp
    compression.cpp
)

if(ELA_USE_MPI)
    set(HDRS ${HDRS}
        mpidomain.h
    )

    set(SRCS ${SRCS}
        mpidomain.cpp
    )

    target_link_libraries(${LIBRARY_NAME} PUBLIC MPI::MPI_CXX)
endif(ELA_USE_MPI)

target_sources(${LIBRARY_NAME}
    PRIVATE ${SRCS}
            ${HDRS}
)

target_link_libraries(${LIBRARY_NAME} PUBLIC svector)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif(BUILD_TESTING)