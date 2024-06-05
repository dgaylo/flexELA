# generate main header
configure_file(ELA.h ELA.h)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(svector)
target_link_libraries(${PROJECT_NAME} PRIVATE svector)

add_subdirectory(domain)
target_link_libraries(${PROJECT_NAME} PRIVATE domain)


target_sources(${PROJECT_NAME} PRIVATE init.cpp ELA_Solver.cpp)
target_sources(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/ELA.h ELA_Solver.h)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER 
    "${CMAKE_CURRENT_BINARY_DIR}/ELA.h;${CMAKE_CURRENT_SOURCE_DIR}/ELA_Solver.h")

if(BUILD_TESTING)
add_subdirectory(tests)
endif(BUILD_TESTING)