# Installing FlexELA

After downloading the repository, this page describes how to install the library and link to it.
FlexELA uses [CMake](https://cmake.org/) to configure and build the library.

## 1. Configuring FlexELA

The first is to configure the build using CMake.
From the top level directory of the repository, the minimum is to run `cmake -B [BUILD_DIRECTORY]`, where `[BUILD_DIRECTORY]` is the working directory for build files. 
Typically, one will want to specify more details about the configuration. For example,
```console
flexELA$ cmake -DELA_USE_MPI=on -DFORTRAN_COMPATIBLE=on -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../elalib -B build
```
Would specify configure a Release build in the directory `build` using MPI and designed to be called from Fortran. Installed files will be put in the directory `../elalib`.

In addition to standard CMake configuration options, ELA provides the following build options
| Build Option | Description | Default |
|:--:|:--|:--:|
| `ELA_USE_MPI` | `ON`: The library will be built to be called by parallel MPI applications.<br/>`OFF` : The library will be built to be called by serial applications. | `ON` |
| `ELA_USE_OPENMP` | `ON`: The library will be built to run use multithreading. (\ref openmpnote "see 1")<br/>`OFF` : The library will be built to run on a single thread. |  `OFF` |
| `FORTRAN_COMPATIBLE` | `ON`: The library will include Fortran interfaces and will assume array ordering is column-major.<br/>`OFF`: The library will not include any Fortran interfaces and will assume row-major. | `OFF` |
| `BUILD_TESTING` | `ON`: Build unit and integration tests.<br/>`OFF`: Do not build tests. | `ON` |
| `BUILD_Fortran_TESTING` | `ON`: Include Fortran integration tests if `BUILD_TESTING=ON` and `FORTRAN_COMPATIBLE=ON`<br/>`OFF`: Do not build these tests (CMake sometimes struggles building Fortran programs) | `ON` |

\anchor openmpnote 1. This primarily effects function in ELA_Solver.h.

### Build Types

FlexELA has few build types:
| `CMAKE_BUILD_TYPE` | Description |
|:--:|:--|
| `Release` (default) | Includes full optimizations (`-O3`) as will as IPO when the compiler supports it. Turns off some runtime checking. |
| `Profile` | Like `Release`, but no IPO and adds debug symbols to allow profiling the library.<br/>Only works GNU, Clang, and Intel compilers. |
| `Debug` | Turns on debug symbols (`-g`) and adds `-fsanitize=leak,address,undefined`. Runtime checks are included. |
| `Coverage` | Generated coverage reports for unit tests.<br/>Only works GNU and Clang compilers. |

### Specifying MPI-Wrapped Compilers

On some systems, it may be necessary to use MPI-wrapped compilers to ensure MPI libraries are linked to correctly. For example,
```console
flexELA$ cmake -DELA_USE_MPI=on -DFORTRAN_COMPATIBLE=on -DCMAKE_CXX_COMPILER=mpicxx -DCMAKE_C_COMPILER=mpicc -DCMAKE_Fortran_COMPILER=mpif90 -B build
```
sets the compilers to `mpicxx`, `mpicc`, and `mpif90`.

## 2. Building And Installing FlexELA

Here, will assume the build directory `[BUILD_DIRECTORY]` was set to `build` during configuration , as in the exmples above.
After successfully configuring FlexELA, build the library by calling 
```console
flexELA$ cmake --build build
```

### Testing

If configured with `BUILD_TESTING=ON`, the test suite can be run with
```console
flexELA$ (cd build && ctest)
```
If `ELA_USE_MPI=ON`, some tests will launch mpi programs with 4 processes.

### Installing

After building, install the library by calling
```console
flexELA$ cmake --install build
```
This will create the required `lib` and `include` files in the directory specified by `CMAKE_INSTALL_PREFIX`.

## 3. Linking to FlexELA

The ELA library has a few header file

- `ELA.h`
- `ELA_Solver.h`
- `ELA_Output.h`

The name of the library in `[CMAKE_INSTALL_PREFIX]/lib` depends on `FORTRAN_COMPATIBLE`. For typical systems,

- `libflexELA.a` when `FORTRAN_COMPATIBLE=OFF`
- `libflexELA_f.a` when `FORTRAN_COMPATIBLE=ON`

This distinction is made as each version makes different assumptions about the array ordering of the calling application.
