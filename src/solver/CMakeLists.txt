set(HDRS
    solver.h
)

set(SRCS
    solver.cpp
)

target_sources(${PROJECT_NAME}
    PRIVATE ${SRCS}
            ${HDRS}
)

if(BUILD_TESTING)
add_subdirectory(tests)
endif(BUILD_TESTING)