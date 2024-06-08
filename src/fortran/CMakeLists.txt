include(FortranCInterface)
FortranCInterface_HEADER(../FC.h MACRO_NAMESPACE "F90_")

set(HDRS
    fortran.h
)

set(SRCS
    ELA_f.cpp
    ELA_Solver_f.cpp
    ELA_Output_f.cpp
)

target_sources(${PROJECT_NAME}
    PRIVATE ${SRCS}
            ${HDRS}
)