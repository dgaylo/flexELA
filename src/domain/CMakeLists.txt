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
endif(ELA_USE_MPI)

target_sources(${PROJECT_NAME}
    PRIVATE ${SRCS}
            ${HDRS}
)

if(BUILD_TESTING)
add_subdirectory(tests)
endif(BUILD_TESTING)