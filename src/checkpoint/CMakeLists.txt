set(HDRS
    header.h
)

set(SRCS
    header.cpp
)

target_sources(${PROJECT_NAME}
    PRIVATE ${SRCS}
            ${HDRS}
)

if(BUILD_TESTING)
add_subdirectory(tests)
endif(BUILD_TESTING)