set(HDRS
    header.h
    checkpoint.h
)

set(SRCS
    header.cpp
    checkpoint.cpp
)

target_sources(${PROJECT_NAME}
    PRIVATE ${SRCS}
            ${HDRS}
)

if(BUILD_TESTING)
add_subdirectory(tests)
endif(BUILD_TESTING)