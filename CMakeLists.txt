cmake_minimum_required(VERSION 3.20)

option(ELA_USE_MPI "Build an MPI version of the library" ON)
option(FORTRAN_COMPATIBLE "Build the library to be called from FORTRAN" OFF)
option(BUILD_TESTING "Build testing" ON)

set(PROJECT_NAME flexELA)
project (${PROJECT_NAME} 
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Setup MPI
if(ELA_USE_MPI)
  find_package(MPI REQUIRED)
  target_link_libraries(${PROJECT_NAME} PUBLIC  MPI::MPI_CXX)

  add_definitions(-DELA_USE_MPI)
  

  set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}_mpi)
else()
  set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}_serial)
endif(ELA_USE_MPI)

# Setup Fortran
if(FORTRAN_COMPATIBLE)

  add_definitions(-DF_STYLE)

  set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}_f)
else()
endif(FORTRAN_COMPATIBLE)

## Testing
enable_testing()
if(BUILD_TESTING)
  # Dont include tests in install
  set(INSTALL_GTEST off)  

  # Setup Google Test
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  include(GoogleTest)
  include (CTest)
endif(BUILD_TESTING)

## Build Types
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  # add sanitizing
  add_compile_options(-fsanitize=leak,address,undefined)
  link_libraries(-fsanitize=leak,address,undefined)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # generate coverage reports with gcov
    add_compile_options(-g -O0 --coverage)
    link_libraries(--coverage)

    target_link_options(${PROJECT_NAME} INTERFACE --coverage)
  endif()
endif()


add_subdirectory(src)

# Install
install(TARGETS ${PROJECT_NAME})
