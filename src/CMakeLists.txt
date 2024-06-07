## Add library
add_library(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

## Compiler flags
if(ELA_USE_MPI)
  target_link_libraries(${PROJECT_NAME} PUBLIC  MPI::MPI_CXX)
  add_definitions(-DELA_USE_MPI)
  set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}_mpi)
else()
  set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}_serial)
endif(ELA_USE_MPI)

if(FORTRAN_COMPATIBLE)
    add_definitions(-DF_STYLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}_f)
endif(FORTRAN_COMPATIBLE)

## generate headers
configure_file(ELA.h ELA.h)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_sources(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/ELA.h)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_sources(${PROJECT_NAME} PUBLIC 
  ELA_Solver.h
  ELA_Output.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER 
    "${CMAKE_CURRENT_BINARY_DIR}/ELA.h;${CMAKE_CURRENT_SOURCE_DIR}/ELA_Solver.h;${CMAKE_CURRENT_SOURCE_DIR}/ELA_Output.h;"
  )

## sub libraries
add_subdirectory(svector)
target_link_libraries(${PROJECT_NAME} PRIVATE svector)

add_subdirectory(domain)
target_link_libraries(${PROJECT_NAME} PRIVATE domain)

add_subdirectory(output)
target_link_libraries(${PROJECT_NAME} PRIVATE output)

## source files
target_sources(${PROJECT_NAME} PRIVATE 
    globalVariables.h
    init.cpp 
    ELA_Solver.cpp
    ELA_Output.cpp
)