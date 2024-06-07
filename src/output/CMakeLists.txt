set(HDRS
    output.h
    vv.h
    vtm.h
)

set(SRCS
    vv.cpp
    vtm.cpp
)

target_sources(${PROJECT_NAME}
    PRIVATE ${SRCS}
            ${HDRS}
)

if(BUILD_TESTING)
add_subdirectory(tests)
endif(BUILD_TESTING)
