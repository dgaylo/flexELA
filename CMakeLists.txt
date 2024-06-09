cmake_minimum_required(VERSION 3.20)

option(ELA_USE_MPI "Build an MPI version of the library" ON)
option(FORTRAN_COMPATIBLE "Build the library to be called from FORTRAN" OFF)
option(BUILD_TESTING "Build testing" ON)

set(PROJECT_NAME flexELA)
project (${PROJECT_NAME} 
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Setup library to be called from fortran
if(FORTRAN_COMPATIBLE)
  enable_language(C Fortran)
  add_definitions(-DF_STYLE)
endif()

# Setup MPI
if(ELA_USE_MPI)
  find_package(MPI REQUIRED)
  include_directories(SYSTEM ${MPI_INCLUDE_PATH})

  add_definitions(-DELA_USE_MPI)
endif(ELA_USE_MPI)


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
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  # add sanitizing
  add_compile_options(-fsanitize=leak,address,undefined -Wpedantic -Wall)
  link_libraries(-fsanitize=leak,address,undefined)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # generate coverage reports with gcov
    add_compile_options(-g -O0 --coverage -DNDEBUG)
    link_libraries(--coverage -DNDEBUG)
  endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_compile_options(-O3 -DNDEBUG)
  link_libraries(-DNDEBUG)
endif()

# older cmake doesnt support COMPILE_WARNING_AS_ERROR
if(CMAKE_VERSION VERSION_LESS "3.24.0")
  option(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
  if(CMAKE_COMPILE_WARNING_AS_ERROR)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
      add_compile_options(-Werror)
    else()
      message(Unable to apply COMPILE_WARNING_AS_ERROR)
    endif()
  endif()
endif()

add_subdirectory(src)

# Extra build options only for the library (not testing)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  # Turn on IPO
  set_property(TARGET ${PROJECT} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()


if(BUILD_TESTING)
add_subdirectory(tests)
endif(BUILD_TESTING)

# Install
install(TARGETS ${PROJECT_NAME})
