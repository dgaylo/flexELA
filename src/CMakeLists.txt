## Add library
add_library(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

## Compiler flags
if(ELA_USE_MPI)
  target_link_libraries(${PROJECT_NAME} PUBLIC  MPI::MPI_CXX)
  add_compile_options(${MPI_CXX_COMPILE_OPTIONS})
  link_libraries(${MPI_CXX_LINK_FLAGS})
endif(ELA_USE_MPI)

if(FORTRAN_COMPATIBLE)
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

## source files
add_subdirectory(svector)
add_subdirectory(domain)
add_subdirectory(output)
add_subdirectory(checkpoint)

if(FORTRAN_COMPATIBLE)
  add_subdirectory(fortran)
endif()

target_sources(${PROJECT_NAME} PRIVATE 
    globalVariables.h
    ELA.cpp 
    ELA_Solver.cpp
    ELA_Output.cpp
)