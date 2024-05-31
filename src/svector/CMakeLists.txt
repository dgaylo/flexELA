set(LIBRARY_NAME svector)
add_library(${LIBRARY_NAME} STATIC)

set(HDRS
    svector.h
    element.h
    types.h
)

set(SRCS
    svector.cpp
)


target_sources(${LIBRARY_NAME}
    PRIVATE ${SRCS}
            ${HDRS}
)

if(BUILD_TESTING)
add_subdirectory(tests)
endif(BUILD_TESTING)